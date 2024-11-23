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





int is_Leap_Year(int year) {
    // Determine if year is leap year
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}





int get_Last_Day_Of_Month(int year, int month) {
    // Function to get the last day of a given month
    switch (month) {
        case 1: return 31;                              // January
        case 2: return is_Leap_Year(year) ? 29 : 28;    // February
        case 3: return 31;                              // March
        case 4: return 30;                              // April
        case 5: return 31;                              // May
        case 6: return 30;                              // June
        case 7: return 31;                              // July
        case 8: return 31;                              // August
        case 9: return 30;                              // September
        case 10: return 31;                             // October
        case 11: return 30;                             // November
        case 12: return 31;                             // December
        default: return 0;                              // Invalid month
    }
}





int get_last_Sunday(int year, int month) {
    // Function to calculate the last Sunday of a month
    // Get the last day of the month
    int lastDay = get_Last_Day_Of_Month(year, month);
    // Adjust month and year for Zeller's formula
    if (month < 3) {
        month += 12;
        year -= 1;
    }
    // Zeller's Congruence formula
    int K = year % 100;
    int J = year / 100;
    // Calculate the day of the week (0 = Saturday, 1 = Sunday, ..., 6 = Friday)
    int h = (lastDay + (13 * (month + 1)) / 5 + K + K / 4 + J / 4 + 5 * J) % 7;
    // Adjust h: h = 0 (Saturday), 1 (Sunday), ..., 6 (Friday)
    // If h == 1, the last day is Sunday
    if (h == 1) {
        return lastDay; // Last day is Sunday
    } else {
        // Calculate how many days back to the last Sunday
        return lastDay - (h + 6) % 7; // Correctly get the last Sunday
    }
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





void Apply_timezone_dst(RTC_TimeTypeDef *timeTypeDef, RTC_DateTypeDef *dateTypeDef) {
    // Converti RTC_TimeTypeDef e RTC_DateTypeDef in time_t
    time_t _utc_unixtime = RTC_to_time_t(timeTypeDef, dateTypeDef);
    struct tm utc_buf;
    // Estrai la struttura tm per UTC
    gmtime_r(&_utc_unixtime, &utc_buf);
    // Genera year e month in un formato comodo per estrarre ultima domenica di marzo e ottobre
    int year = utc_buf.tm_year + 1900;
    int last_sunday_march = get_last_Sunday(year, 3);
    int last_sunday_october = get_last_Sunday(year, 10);
    // Genera time_t per ultime domeniche di marzo e ottobre
    time_t lastSundayMarch = mktime(&(struct tm){.tm_year = utc_buf.tm_year, .tm_mon = 2, .tm_mday = last_sunday_march, .tm_hour = 1});
    time_t lastSundayOctober = mktime(&(struct tm){.tm_year = utc_buf.tm_year, .tm_mon = 9, .tm_mday = last_sunday_october, .tm_hour = 1});
    // Controlla le regole DST
    uint8_t dst_effective = (difftime(_utc_unixtime, lastSundayMarch) > 0 && difftime(_utc_unixtime, lastSundayOctober) < 0);
    // Applica il corretto offset di secondi
    _utc_unixtime += TIMEZONE_OFFSET_S + dst_effective * DST_OFFSET_S;
    // Converti il time_t modificato di nuovo in RTC_TimeTypeDef e RTC_DateTypeDef
    time_t_to_RTC(_utc_unixtime, timeTypeDef, dateTypeDef);
}



