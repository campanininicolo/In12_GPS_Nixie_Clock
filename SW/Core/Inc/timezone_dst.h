/**
  ******************************************************************************
  * @file           : timezone_dst.h
  * @brief          : Header for timezone_dst.c file.
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
#ifndef __TIMEZONE_DST_H
#define __TIMEZONE_DST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "time.h"


/* Types ---------------------------------------------------------------------*/

#define TIMEZONE_OFFSET_S   3600
#define DST_OFFSET_S        3600



/* Functions -----------------------------------------------------------------*/

time_t Apply_timezone_dst(time_t _utc_unixtime);





#endif