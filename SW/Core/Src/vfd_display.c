/**
  ******************************************************************************
  * @file           : vfd_display.c
  * @brief          : Function to the Vfd Display
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

#include "vfd_display.h"





// Global SPI handler for Vfd display
SPI_HandleTypeDef *Vfd_hspi;
// Global TIM handler for Vfd display
TIM_HandleTypeDef *Vfd_htim;
// Global variable for PWM channel
uint32_t Vfd_PWM_channel = 0;
// Display map
uint8_t vfd_map[10] = { 0b10101111,   // 0 numeral
                        0b10100000,   // 1 numeral
                        0b11001101,   // 2 numeral
                        0b11101001,   // 3 numeral
                        0b11100010,   // 4 numeral
                        0b01101011,   // 5 numeral
                        0b01101111,   // 6 numeral
                        0b10100001,   // 7 numeral
                        0b11101111,   // 8 numeral
                        0b11101011    // 9 numeral
};

// Buffer for SPI communication 
uint8_t Vfd_SPI_buffer[SPI_BUFFER_SIZE];





void Vfd_init(SPI_HandleTypeDef *_hspi, TIM_HandleTypeDef *_htim, uint32_t _PWM_channel)
{
  // Save the SPI handler
  Vfd_hspi = _hspi;
  // Save the PWM Timer handler
  Vfd_htim = _htim;
  // Save the PWM Timer channel
  Vfd_PWM_channel = _PWM_channel;

  // Disable Filamente supply
  Vfd_disable_Filament();
  // Set Latch to default state
  HAL_GPIO_WritePin(LATCH_EN_GPIO_Port, LATCH_EN_Pin, GPIO_PIN_RESET);
  // Init SPI buffer at zero
  for (uint8_t i = 0; i < SPI_BUFFER_SIZE; i++) {
    Vfd_SPI_buffer[i] = 0;
  }

  // Start the PWM generation
  HAL_TIM_PWM_Start(Vfd_htim, Vfd_PWM_channel);
  // Set the PWM duty cycle to zero (99 due to reversed logic) (safety)
  __HAL_TIM_SET_COMPARE(Vfd_htim, Vfd_PWM_channel, 99);
}





void Vfd_enable_Filament()
{
  // Turn-off the GPIO
  HAL_GPIO_WritePin(HV_OFF_GPIO_Port, HV_OFF_Pin, GPIO_PIN_SET);
}





void Vfd_disable_Filament()
{
  // Turn-on the GPIO
  // Modified w.r.t. Nixie reversing the polarity
  HAL_GPIO_WritePin(HV_OFF_GPIO_Port, HV_OFF_Pin, GPIO_PIN_RESET);
}





void Vfd_update_display(uint8_t _hours, uint8_t _minutes, uint8_t _seconds)
{
  // Buffer variables for the three HV5812 drivers
  uint32_t driver_1 = 0;
  uint32_t driver_2 = 0;
  uint32_t driver_3 = 0;

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

  // Driver 1 works with D1, D2,
  // HV_OUT_1 to HV_OUT_9 map to D2 (hours_uni)
  // HV_OUT_12 to HV_OUT_20 map to D1 (hours_dec)

  // Driver 2 works with D3, D4,
  // HV_OUT_1 to HV_OUT_9 map to D4 (minutes_uni)
  // HV_OUT_12 to HV_OUT_20 map to D3 (minutes_dec)

  // Driver 3 works with D5, D6,
  // HV_OUT_1 to HV_OUT_9 map to D6 (seconds_uni)
  // HV_OUT_12 to HV_OUT_20 map to D5 (seconds_dec)

  driver_1 = 0x00000801;
  driver_1 = driver_1 | (((uint32_t) vfd_map[hours_dec]) << 12);
  driver_1 = driver_1 | (((uint32_t) vfd_map[hours_uni]) << 1);

  driver_2 = 0x00000801;
  driver_2 = driver_2 | (((uint32_t) vfd_map[minutes_dec]) << 12);
  driver_2 = driver_2 | (((uint32_t) vfd_map[minutes_uni]) << 1);

  driver_3 = 0x00000801;
  driver_3 = driver_3 | (((uint32_t) vfd_map[seconds_dec]) << 12);
  driver_3 = driver_3 | (((uint32_t) vfd_map[seconds_uni]) << 1);

  // Compose the SPI buffer
  Vfd_SPI_buffer[0] = (uint8_t) (driver_3 >> 16);
  Vfd_SPI_buffer[1] = (uint8_t) (driver_3 >> 8);
  Vfd_SPI_buffer[2] = (uint8_t) (driver_3);
  Vfd_SPI_buffer[3] = (uint8_t) (driver_2 >> 12);
  Vfd_SPI_buffer[4] = (uint8_t) (driver_2 >> 4);
  Vfd_SPI_buffer[5] = (uint8_t) (((driver_2 & 0x0F) << 4) | (driver_1 >> 16));
  Vfd_SPI_buffer[6] = (uint8_t) (driver_1 >> 8);
  Vfd_SPI_buffer[7] = (uint8_t) (driver_1);

  // Send data to screen
  HAL_SPI_Transmit_IT(Vfd_hspi, Vfd_SPI_buffer, SPI_BUFFER_SIZE);
}





void Vfd_set_brightness(uint8_t _brightness)
{
  // Clamp brightness value
  if (_brightness > 99)
    _brightness = 99;
  // Compute the required value knowing the logic is reversed
  uint8_t actual_brightness = 99 - _brightness;
  // Set the PWM register
  __HAL_TIM_SET_COMPARE(Vfd_htim, Vfd_PWM_channel, actual_brightness);
}





Vfd_mode_enum_t Vfd_get_mode() 
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
	if (hspi == Vfd_hspi) {
    // Toggle the Latch pin (multiple times to have correct timing)
		HAL_GPIO_WritePin(LATCH_EN_GPIO_Port, LATCH_EN_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LATCH_EN_GPIO_Port, LATCH_EN_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LATCH_EN_GPIO_Port, LATCH_EN_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LATCH_EN_GPIO_Port, LATCH_EN_Pin, GPIO_PIN_RESET);
	}
}





void Vfd_get_random(uint8_t *_value_h, uint8_t *_value_m, uint8_t *_value_s)
{
  // Generate random values
  *_value_h = rand() % 100;
  *_value_m = rand() % 100;
  *_value_s = rand() % 100;
}


