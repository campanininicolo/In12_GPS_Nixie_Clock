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
#include "string.h"





char received_byte = 0;
char line_buffer[LINE_BUFFER_SIZE][LINE_SIZE];
char read_pointer = 0;
char write_pointer = 0;
char valid_lines = 0;
char first_line_found = 0;




void GPS_parser_init()
{
	received_byte = 0;
	read_pointer = 0;
	write_pointer = 0;
	valid_lines = 0;
	first_line_found = 0;
}



void add_valid_line()
{
  write_pointer++;
  if (write_pointer >= LINE_BUFFER_SIZE) {
    write_pointer = 0;
  }
  if (valid_lines < LINE_BUFFER_SIZE) {
    valid_lines++;
  }
}



void remove_valid_line()
{
  read_pointer++;
  if (read_pointer >= LINE_BUFFER_SIZE) {
    read_pointer = 0;
  }
  if (valid_lines > 0) {
    valid_lines--;
  }
}





void GPS_get_last_time_info(struct tm *_GPS_last_date_time, uint8_t *_valid)
{
  // Buffer to store the time line found.
  char time_line_buffer[LINE_SIZE];
  time_line_buffer[0] = '\0';

  if (valid_lines > 0) {
    // Time line found. Move it to the buffer.
    strcpy(time_line_buffer, line_buffer[read_pointer]);
    remove_valid_line();
    // Toggle LED.
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    // Check if time lines have been found and if the lenght is coherent with valid line
    if (strlen(time_line_buffer) > 23) {
      // A time line has been found.
      // Parse the time line.
      *(_GPS_last_date_time) = parse_zda_gps_line(time_line_buffer);
      *_valid = 1;

    } else {
      *_valid = 0;
    }

  } else {
     *_valid = 0;
  }

}





void GPS_parse_single_byte(uint8_t _single_byte)
{
	if (!first_line_found) {
	  if (_single_byte == '\n') {
      first_line_found = 1;
      line_buffer[write_pointer][0] = '\0';
	  }
	} else {
	  if (_single_byte == '\n') {
      strncat(line_buffer[write_pointer], &_single_byte, 1);
      // Check if it is a ZDA line. Discard otherwise.
      if (strncmp(line_buffer[read_pointer] + 3, "ZDA", (unsigned int) 3) == 0) {
        add_valid_line();
      }
		line_buffer[write_pointer][0] = '\0';
	  } else {
		  strncat(line_buffer[write_pointer], &_single_byte, 1);
	  }
	}
}





struct tm parse_zda_gps_line(char *_time_line)
{
  // Time structure for data
  struct tm GPS_Date_Time_buffer;

  char time_string[15] = "";
  char date_string[15] = "";
  char prova[40] = "";

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

  // Fix the date
  GPS_Date_Time_buffer.tm_year -= 1900;
  GPS_Date_Time_buffer.tm_mon -= 1;

  return(GPS_Date_Time_buffer);
}


