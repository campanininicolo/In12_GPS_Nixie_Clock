/**
  ******************************************************************************
  * @file           : timezone_dst.c
  * @brief          : Function to the Timezone DST library
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

#include "timezone_dst.h"






time_t Apply_timezone_dst(time_t _utc_unixtime)
{
    time_t tmz_unixtime;
    time_t dst_offset;
    struct tm tmz_buf;
    
    // Extract now struct tm
    gmtime_r(&tmz_unixtime, &tmz_buf);

    // Check for DST rules




    // Apply the correct amount of seconds offset
    tmz_unixtime = _utc_unixtime + TIMEZONE_OFFSET_S;
}


