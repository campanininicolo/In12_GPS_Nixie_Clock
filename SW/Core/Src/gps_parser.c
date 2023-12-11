/**
  ******************************************************************************
  * @file           : gps_parser.c
  * @brief          : Function to interface to GPS through NMEA 183
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 Nicolò Campanini.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "gps_parser.h"





// Global UART and DMA handlers for GPS
UART_HandleTypeDef *GPS_huart;
DMA_HandleTypeDef *GPS_hdma_usart_rx;

// Global variable containint the GPS buffers.
GPS_buffer_struct_t GPS_buffer_struct;
// Global variable to hold datetime extracted from GPS.
GPS_datetime_struct_t GPS_datetime_struct;





void GPS_Init(UART_HandleTypeDef *_huart, DMA_HandleTypeDef *_hdma_usart_rx)
{
  // Init the GPS buffer scructure
  GPS_buffer_struct.active_buffer = 0;
  GPS_buffer_struct.buffer_status[0] = 0;
  GPS_buffer_struct.buffer_status[1] = 0;
  GPS_buffer_struct.buffer_size[0] = 0;
  GPS_buffer_struct.buffer_size[1] = 0;
  // Init the GPS_datetime_struct as NOT-valid
  GPS_datetime_struct.unixtime = 0;
  GPS_datetime_struct.valid = 0;
  // Init the internal UART handler
  GPS_huart = _huart;
  GPS_hdma_usart_rx = _hdma_usart_rx;
  // Init the Blue LED
  HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);
}




void GPS_Start()
{
  HAL_UARTEx_ReceiveToIdle_DMA(GPS_huart, GPS_buffer_struct.buffer[GPS_buffer_struct.active_buffer], UART_BUFFER_SIZE-1);
  __HAL_DMA_DISABLE_IT(GPS_hdma_usart_rx, DMA_IT_HT);
}





GPS_datetime_struct_t GPS_Read_Datetime()
{
  return(GPS_datetime_struct);
}





void GPS_Parse_ZDA_Line(char *_time_line)
{
  // Initialize output datetime structure as NOT-valid 
  GPS_datetime_struct.valid = 0;

  char *line_end = NULL;
  line_end = strchr(_time_line, '\n');
  
  // Exit if no end is found
  if (!line_end) {
    return;
  }

  // Exit if the line is too short
  if (line_end - _time_line <= 23) {
    return;
  }

  struct tm GPS_Date_Time_buffer;
  char time_string[15] = "";
  char date_string[15] = "";

  // Extract the time line and the date line
  strncpy(time_string, _time_line + 7, 10);
  strncpy(date_string, _time_line + 18, 10);

  // Parse the time_string using sscanf.
  sscanf(time_string, "%2d%2d%2d",
      &(GPS_Date_Time_buffer.tm_hour),
      &(GPS_Date_Time_buffer.tm_min),
      &(GPS_Date_Time_buffer.tm_sec));

  // Parse the date_string using sscanf.
  sscanf(date_string, "%2d,%2d,%4d",
      &(GPS_Date_Time_buffer.tm_mday),
      &(GPS_Date_Time_buffer.tm_mon),
      &(GPS_Date_Time_buffer.tm_year));

  // Fix the date according to POSIX time
  GPS_Date_Time_buffer.tm_year -= 1900;
  GPS_Date_Time_buffer.tm_mon -= 1;

  // Return POSIX time and valid flag
  GPS_datetime_struct.unixtime = mktime(&GPS_Date_Time_buffer);
  GPS_datetime_struct.valid = 1;
}





void GPS_Update_Data()
{
  // Loop in the two buffers to find the one used.
  for (uint8_t i = 0; i < 2; i++) {

    if (GPS_buffer_struct.buffer_status[i] == 1) {

      // Set the buffer as NOT-valid
      GPS_buffer_struct.buffer_status[i] = 0;

      // Set the datetime structure as NOT-valid 
      GPS_datetime_struct.valid = 0;

      // Proceed only if the buffer starts with "$"
      if (GPS_buffer_struct.buffer[i][0] != '$') {
        return;
      }

      // Loop through the buffer looking for the different messages.
      uint8_t end_reached = 0;
      char *time_line = NULL;
      char *current_pnt = GPS_buffer_struct.buffer[i];
      char *next_pnt = NULL;

      while (end_reached == 0) {
        // Search for the message start character
        next_pnt = strchr(current_pnt, '$');

        if (!next_pnt) {
          end_reached = 1;
          // Exit
        } else {
          // Look if the ZDA line is found
          if (strncmp(next_pnt + 3, "ZDA", 3) == 0) {
            time_line = next_pnt;
          }
          // TODO: Add other lines to be found 

          // Update pointers to advance in the buffer
          current_pnt = next_pnt+1;
        }
      }

      // Parse the ZDA line if found
      if (time_line) {
        GPS_Parse_ZDA_Line(time_line);
      }

      // TODO: Add parsing of other lines
    }
  }
}






void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart == GPS_huart) {
    // Toggle LED
    HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
    // Set the buffer size and add the termination character
    GPS_buffer_struct.buffer[GPS_buffer_struct.active_buffer][Size] = '\0';
    GPS_buffer_struct.buffer_size[GPS_buffer_struct.active_buffer] = Size+1;
    // Set the flag so that data can be read from the buffer
    GPS_buffer_struct.buffer_status[GPS_buffer_struct.active_buffer] = 1;
    // Change active buffer
    GPS_buffer_struct.active_buffer = !GPS_buffer_struct.active_buffer;
    // Relaunch the UART receiver
    GPS_Start();
	}
}


