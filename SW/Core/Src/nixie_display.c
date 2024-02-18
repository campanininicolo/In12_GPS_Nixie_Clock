/**
  ******************************************************************************
  * @file           : nixie_display.c
  * @brief          : Function to the Nixie Display
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

#include "nixie_display.h"





// Global SPI handler for Nixie display
SPI_HandleTypeDef *Nixie_hspi;
// Global TIM handler for Nixie display
TIM_HandleTypeDef *Nixie_htim;
// Global variable for PWM channel
uint32_t Nixie_PWM_channel = 0;

// Buffer for SPI communication 
uint8_t Nixie_SPI_buffer[SPI_BUFFER_SIZE];





void Nixie_init(SPI_HandleTypeDef *_hspi, TIM_HandleTypeDef *_htim, uint32_t _PWM_channel)
{
  // Save the SPI handler
  Nixie_hspi = _hspi;
  // Save the PWM Timer handler
  Nixie_htim = _htim;
  // Save the PWM Timer channel
  Nixie_PWM_channel = _PWM_channel;

  // Disable HV
  Nixie_disable_HV();
  // Set Latch to default state
  HAL_GPIO_WritePin(LATCH_EN_GPIO_Port, LATCH_EN_Pin, GPIO_PIN_RESET);
  // Init SPI buffer at zero
  for (uint8_t i = 0; i < SPI_BUFFER_SIZE; i++) {
    Nixie_SPI_buffer[i] = 0;
  }

  // Start the PWM generation
  HAL_TIM_PWM_Start(Nixie_htim, Nixie_PWM_channel);
  // Set the PWM duty cycle to zero (safety)
  __HAL_TIM_SET_COMPARE(Nixie_htim, Nixie_PWM_channel, 0);
}





void Nixie_enable_HV()
{
  // Turn-off the GPIO
  HAL_GPIO_WritePin(HV_OFF_GPIO_Port, HV_OFF_Pin, GPIO_PIN_RESET);
}





void Nixie_disable_HV()
{
  // Turn-on the GPIO
  HAL_GPIO_WritePin(HV_OFF_GPIO_Port, HV_OFF_Pin, GPIO_PIN_SET);
}





void Nixie_update_display(uint8_t _hours, uint8_t _minutes, uint8_t _seconds)
{
  // Buffer variables for the two HV5530 drivers
  uint32_t driver_1 = 0;
  uint32_t driver_2 = 0;

  // Clamp values
  if (_hours > 99)
    _hours = 99;

  if (_minutes > 99)
	  _minutes = 99;

  if (_seconds > 99)
	  _seconds = 99;

  uint8_t hours_dec = _hours / 10;
  uint8_t hours_uni = _hours - (hours_dec * 10);

  uint8_t minutes_dec = _minutes / 10;
  uint8_t minutes_uni = _minutes - (minutes_dec * 10);

  uint8_t seconds_dec = _seconds / 10;
  uint8_t seconds_uni = _seconds - (seconds_dec * 10);

  // Driver 1 works with N1, N2, N3
  // N3 --> Minutes Dec --> 1 - 10
  // N2 --> Hours Uni --> 12 - 21
  // N3 --> Hours Dec --> 22 - 31

  // Driver 2 works with N4, N5, N6
  // N6 --> Seconds Uni --> 1 - 10
  // N5 --> Seconds Dec --> 11 - 20
  // N4 --> Minutes Uni --> 22 - 31

  driver_1 = ((uint32_t) 1) << minutes_dec;
  driver_1 = driver_1 | (((uint32_t) 1) << (hours_uni + 11));
  driver_1 = driver_1 | (((uint32_t) 1) << (hours_dec + 21));

  driver_2 = ((uint32_t) 1) << seconds_uni;
  driver_2 = driver_2 | (((uint32_t) 1) << (seconds_dec + 10));
  driver_2 = driver_2 | (((uint32_t) 1) << (minutes_uni + 21));

  // Compose the SPI buffer
  Nixie_SPI_buffer[0] = (uint8_t) (driver_2 >> 24);
  Nixie_SPI_buffer[1] = (uint8_t) (driver_2 >> 16);
  Nixie_SPI_buffer[2] = (uint8_t) (driver_2 >> 8);
  Nixie_SPI_buffer[3] = (uint8_t) (driver_2);

  Nixie_SPI_buffer[4] = (uint8_t) (driver_1 >> 24);
  Nixie_SPI_buffer[5] = (uint8_t) (driver_1 >> 16);
  Nixie_SPI_buffer[6] = (uint8_t) (driver_1 >> 8);
  Nixie_SPI_buffer[7] = (uint8_t) (driver_1);

  // Send data to screen
  HAL_SPI_Transmit_IT(Nixie_hspi, Nixie_SPI_buffer, SPI_BUFFER_SIZE);
}





void Nixie_set_brightness(uint8_t _brightness)
{
  // Clamp brightness value
  if (_brightness > 99)
    _brightness = 99;
  // Set the PWM register
  __HAL_TIM_SET_COMPARE(Nixie_htim, Nixie_PWM_channel, _brightness);
}





Nixie_mode_enum_t Nixie_get_mode() 
{
  // Variabile statica per tenere traccia delle chiamate in modalità scramble
  static int scramble_calls_left = 0;

  // Se ci sono ancora chiamate in modalità scramble, decrementa il contatore e ritorna SCRAMBLE
  if (scramble_calls_left > 0) {
      scramble_calls_left--;
      return SCRAMBLE;
  }

  // Genera un numero casuale tra 0 e RANDOM_RANGE
  int random_number = rand() % RANDOM_RANGE;

  // Se il numero è minore di SCRAMBLE_PROBABILITY, imposta il contatore a SCRAMBLE_CALLS e ritorna SCRAMBLE
  // Altrimenti, ritorna NORMAL
  if (random_number < SCRAMBLE_PROBABILITY) {
      scramble_calls_left = SCRAMBLE_CALLS;
      return SCRAMBLE;
  } else {
      return NORMAL;
  }
}





void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == Nixie_hspi) {
    // Toggle the Latch pin (multiple times to have correct timing)
		HAL_GPIO_WritePin(LATCH_EN_GPIO_Port, LATCH_EN_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LATCH_EN_GPIO_Port, LATCH_EN_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LATCH_EN_GPIO_Port, LATCH_EN_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LATCH_EN_GPIO_Port, LATCH_EN_Pin, GPIO_PIN_RESET);
	}
}





void Nixie_get_random(uint8_t *_value_h, uint8_t *_value_m, uint8_t *_value_s)
{
  // Generate random values
  *_value_h = rand() % 100;
  *_value_m = rand() % 100;
  *_value_s = rand() % 100;
}


