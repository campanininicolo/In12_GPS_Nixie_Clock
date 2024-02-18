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
#define SCRAMBLE_PROBABILITY 1
#define SCRAMBLE_CALLS 25
#define RANDOM_RANGE 1000


typedef enum {
  NORMAL,
  SCRAMBLE
} Nixie_mode_enum_t;




/* Functions -----------------------------------------------------------------*/
void Nixie_init(SPI_HandleTypeDef *_hspi, TIM_HandleTypeDef *_htim, uint32_t _PWM_channel);
void Nixie_enable_HV();
void Nixie_disable_HV();
void Nixie_update_display(uint8_t _hours, uint8_t _minutes, uint8_t _seconds);
void Nixie_set_brightness(uint8_t _brightness);
Nixie_mode_enum_t Nixie_get_mode();
void Nixie_get_random(uint8_t *_value_h, uint8_t *_value_m, uint8_t *_value_s);





#endif
