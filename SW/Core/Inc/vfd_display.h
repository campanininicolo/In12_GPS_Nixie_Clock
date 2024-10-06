/**
  ******************************************************************************
  * @file           : vfd_display.h
  * @brief          : Header for vfd_display.c file.
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
#ifndef __VFD_DISPLAY_H
#define __VFD_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "time.h"


/* Types ---------------------------------------------------------------------*/
#define SPI_BUFFER_SIZE 8
#define SCRAMBLE_PROBABILITY 1
#define SCRAMBLE_CALLS 25
#define RANDOM_RANGE 1000


typedef enum {
  NORMAL,
  SCRAMBLE
} Vfd_mode_enum_t;




/* Functions -----------------------------------------------------------------*/
void Vfd_init(SPI_HandleTypeDef *_hspi, TIM_HandleTypeDef *_htim, uint32_t _PWM_channel);
void Vfd_enable_Filament();
void Vfd_disable_Filament();
void Vfd_update_display(uint8_t _hours, uint8_t _minutes, uint8_t _seconds);
void Vfd_set_brightness(uint8_t _brightness);
Vfd_mode_enum_t Vfd_get_mode();
void Vfd_get_random(uint8_t *_value_h, uint8_t *_value_m, uint8_t *_value_s);





#endif
