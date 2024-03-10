/**
  ******************************************************************************
  * @file           : gps_parser.h
  * @brief          : Header for gps_parser.c file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPS_PARSER_H
#define __GPS_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "time.h"


/* Types ---------------------------------------------------------------------*/
/* Types ---------------------------------------------------------------------*/
#define RTC_UPDATE_CNT 250



#define UART_BUFFER_SIZE 1024

typedef struct{
  char buffer[2][UART_BUFFER_SIZE];
  uint8_t active_buffer;
  uint8_t buffer_status[2]; 
  uint16_t buffer_size[2];
}GPS_buffer_struct_t;



typedef struct{
  RTC_TimeTypeDef time;
  RTC_DateTypeDef date;
  uint8_t valid;
} GPS_datetime_struct_t;




typedef enum {
  NOT_NEEDED,
  NEEDED
} GPS_RTC_update_t;




void GPS_Init(UART_HandleTypeDef *_huart, DMA_HandleTypeDef *_hdma_usart_rx);
void GPS_Start();
GPS_datetime_struct_t GPS_Read_Datetime();

void GPS_Update_Data();

GPS_RTC_update_t GPS_RTC_check_update();





#endif
