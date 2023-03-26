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
#define LINE_BUFFER_SIZE 10
#define LINE_SIZE 100





void GPS_parser_init();
void GPS_check_for_time_line(void);
void GPS_parse_single_byte(uint8_t single_byte);






#endif
