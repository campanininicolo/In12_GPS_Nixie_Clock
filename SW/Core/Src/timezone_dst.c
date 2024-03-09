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
    struct tm utc_buf;
    
    // Extract now struct tm for UTC
    gmtime_r(&_utc_unixtime, &utc_buf);

    // Check if date is between the last Sunday in March and the last Sunday in October at 1 AM
    int year = utc_buf.tm_year + 1900;
    time_t lastSundayMarch = mktime(&(struct tm){.tm_year = year, .tm_mon = 2, .tm_mday = 31 - ((5 + 2 * year / 4 - year / 100 + year / 400) % 7), .tm_hour = 1});
    time_t lastSundayOctober = mktime(&(struct tm){.tm_year = year, .tm_mon = 9, .tm_mday = 31 - ((5 + 2 * year / 4 - year / 100 + year / 400) % 7), .tm_hour = 1});
    // Check for DST rules
    uint8_t dst_effective = (difftime(_utc_unixtime, lastSundayMarch) > 0 && difftime(_utc_unixtime, lastSundayOctober) < 0);

    // Apply the correct amount of seconds offset
    return(_utc_unixtime + TIMEZONE_OFFSET_S + dst_effective * DST_OFFSET_S);
}





time_t RTC_to_time_t(RTC_TimeTypeDef *timeTypeDef, RTC_DateTypeDef *dateTypeDef) {
    struct tm t;
    t.tm_sec = timeTypeDef->Seconds;
    t.tm_min = timeTypeDef->Minutes;
    t.tm_hour = timeTypeDef->Hours;
    t.tm_mday = dateTypeDef->Date;
    t.tm_mon = dateTypeDef->Month - 1; // In struct tm, month is 0-11
    t.tm_year = dateTypeDef->Year + 100; // Years since 1900
    t.tm_isdst = -1; // -1 means info not available

    time_t time = mktime(&t);
    return time;
}





void time_t_to_RTC(time_t time, RTC_TimeTypeDef *timeTypeDef, RTC_DateTypeDef *dateTypeDef) {
    struct tm *t;
    t = gmtime(&time);

    timeTypeDef->Seconds = t->tm_sec;
    timeTypeDef->Minutes = t->tm_min;
    timeTypeDef->Hours = t->tm_hour;
    dateTypeDef->Date = t->tm_mday;
    dateTypeDef->Month = t->tm_mon + 1; // In struct tm, month is 0-11
    dateTypeDef->Year = t->tm_year - 100; // Years since 1900
}

