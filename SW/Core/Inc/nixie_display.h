/**
  ******************************************************************************
  * @file           : nixie_display.h
  * @brief          : Header for nixie_display.c file.
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
#ifndef __NIXIE_DISPLAY_H
#define __NIXIE_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "time.h"


/* Types ---------------------------------------------------------------------*/


#define SPI_BUFFER_SIZE 8




void Nixie_init(SPI_HandleTypeDef *_hspi);
void Nixie_enable_HV();
void Nixie_disable_HV();
void Nixie_update_display(uint8_t _hours, uint8_t _minutes, uint8_t _seconds);




#endif
