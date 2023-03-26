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





void GPS_check_for_time_line()
{
    if (valid_lines > 0)
    {
      if(strncmp(line_buffer[read_pointer], "$GNZDA", (unsigned int) 6) == 0) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
      }
      remove_valid_line();
    }
}





void GPS_parse_single_byte(uint8_t single_byte)
{
	if (!first_line_found) {
	  if (single_byte == '\n') {
		first_line_found = 1;
		line_buffer[write_pointer][0] = '\0';
	  }
	} else {
	  if (single_byte == '\n') {
		strncat(line_buffer[write_pointer], &single_byte, 1);
		add_valid_line();
		line_buffer[write_pointer][0] = '\0';
	  } else {
		strncat(line_buffer[write_pointer], &single_byte, 1);
	  }
	}
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
