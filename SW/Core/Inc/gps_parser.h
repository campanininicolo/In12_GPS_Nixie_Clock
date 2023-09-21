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


/* Defines -------------------------------------------------------------------*/
#define LINE_BUFFER_SIZE 5
#define LINE_SIZE 100




typedef struct{
  uint8_t Hours;
  uint8_t Minutes;
  uint8_t Seconds;
  uint16_t mSeconds;
  uint8_t Day;
  uint8_t Month;
  uint16_t Year;
  int8_t UTC_H_Off;
  int8_t UTC_M_Off;
  uint8_t Valid;
} GPS_DateTime_Structure_t;





void GPS_parser_init();
void GPS_get_last_time_info(struct tm *_GPS_last_date_time, uint8_t *_valid);
void GPS_parse_single_byte(uint8_t _single_byte);
struct tm parse_zda_gps_line(char *_time_line);






#endif
