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
#include "string.h"





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
    if (strncmp(line_buffer[read_pointer] + 3, "ZDA", (unsigned int) 3) == 0)
    {
      HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
    remove_valid_line();
  }
}




void GPS_get_last_time_info(GPS_DateTime_Structure_t *_GPS_Last_DateTime)
{
  // Buffer to store the time line found.
  char time_line_buffer[LINE_SIZE];
  time_line_buffer[0] = '\0';
  GPS_DateTime_Structure_t GPS_Date_Time_buffer;

  // Run through the saved lines to find the last time line sent.
  // Only the latest will be used .
  if (valid_lines > 0)
  {
    if (strncmp(line_buffer[read_pointer] + 3, "ZDA", (unsigned int) 3) == 0)
    {
      // Time line found. Move it to the buffer.
      strcpy(time_line_buffer, line_buffer[read_pointer]);
    }
    remove_valid_line();
  }

  // Check if time lines have been found
  if (strlen(time_line_buffer) > 0)
  {
    // A time line has been found. 
    // Parse the time line.
    *(_GPS_Last_DateTime) = parse_zda_gps_line(time_line_buffer);

  } else {
    // No Time line has been found
    // Fill the valid flag in the GPSDateTimeStructure to 0 signalling no valid data returned.
    _GPS_Last_DateTime->Valid = 0;
  }
  
}





void GPS_parse_single_byte(uint8_t _single_byte)
{
	if (!first_line_found) {
	  if (_single_byte == '\n') {
		first_line_found = 1;
		line_buffer[write_pointer][0] = '\0';
	  }
	} else {
	  if (_single_byte == '\n') {
		strncat(line_buffer[write_pointer], &_single_byte, 1);
		add_valid_line();
		line_buffer[write_pointer][0] = '\0';
	  } else {
		strncat(line_buffer[write_pointer], &_single_byte, 1);
	  }
	}
}





GPS_DateTime_Structure_t parse_zda_gps_line(char *_time_line)
{
  // Initialize Structure for raw data.
  GPS_DateTime_Structure_t DateTime_Raw = {0xFF, 0xFF, 0xFF, 0xFFFF, 0xFF, 0xFF, 0xFFFF, 0xFF, 0xFF, 0};

  char time_string[15] = "";

  // Parse the string using sscanf.
  sscanf(_time_line,"%*s,%s,%d,%d,%d,%d,%d*%*s",
      time_string,
      &(DateTime_Raw.Day),
      &(DateTime_Raw.Month),
      &(DateTime_Raw.Year),
      &(DateTime_Raw.UTC_H_Off),
      &(DateTime_Raw.UTC_M_Off));

  // Parse the time_string using sscanf.
  sscanf(time_string, "%2d%2d%2d.%d",
      &(DateTime_Raw.Hours),
      &(DateTime_Raw.Minutes),
      &(DateTime_Raw.Seconds),
      &(DateTime_Raw.mSeconds));

  // Check validity of the data read.
  uint8_t hours_valid = (DateTime_Raw.Hours >= 0) && (DateTime_Raw.Day <= 23);
  uint8_t minutes_valid = (DateTime_Raw.Minutes >= 0) && (DateTime_Raw.Day <= 59);
  uint8_t seconds_valid = (DateTime_Raw.Seconds >= 0) && (DateTime_Raw.Seconds <= 59);
  uint8_t mseconds_valid = (DateTime_Raw.mSeconds >= 0) && (DateTime_Raw.mSeconds <= 999);

  uint8_t day_valid = (DateTime_Raw.Day >= 1) && (DateTime_Raw.Day <= 31);
  uint8_t month_valid = (DateTime_Raw.Month >= 1) && (DateTime_Raw.Month <= 12);
  uint8_t year_valid = (DateTime_Raw.Year >= 1) && (DateTime_Raw.Year <= 9999);
  uint8_t utc_h_off_valid = (DateTime_Raw.UTC_H_Off >= -13) && (DateTime_Raw.UTC_H_Off <= 13);
  uint8_t utc_m_off_valid = (DateTime_Raw.UTC_M_Off >= 0) && (DateTime_Raw.UTC_M_Off <= 59);

  // Overall Validity.
  DateTime_Raw.Valid = hours_valid
      && minutes_valid
      && seconds_valid
      && mseconds_valid
      && day_valid
      && month_valid
      && year_valid
      && utc_h_off_valid
      && utc_m_off_valid;

  return(DateTime_Raw);
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
