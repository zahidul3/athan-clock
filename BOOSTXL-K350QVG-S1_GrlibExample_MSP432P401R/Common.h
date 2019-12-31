/*
 * Common.h
 *
 *  Created on: Dec 13, 2019
 *      Author: Jahid's Desktop
 */

#ifndef COMMON_H_
#define COMMON_H_

typedef enum ampm{
    AM,
    PM
} AMPM;

//-----------------------------------------------------------------------------
typedef struct
{
  char str[4];  // keep even
} TsStr3;

typedef struct
{
  TsStr3   days[7];
  TsStr3   months[12];
} TsDaysMonthsConfig;

typedef struct
{
    uint8_t   Second;
    uint8_t   Minute;
    uint8_t   Hour;
    uint8_t   Day;
    uint8_t   Month;
    uint8_t   Year;
    uint8_t   DayOfWeek;  // 0-6, where 0 = Sunday
    int8_t   TimeZone;   //
} TsDateTime;

typedef enum {
  FAJR = 0,
  SUNRISE,
  ZUHR,
  ASR,
  MAGHRIB,
  ISHA,
  NUMBER_OF_ATHAN,
  CURRENT_DATE = 0xFE,
  CURRENT_TIME = 0xFF
} athan_type;

typedef struct
{
    bool athanAlarm[NUMBER_OF_ATHAN];
}TsAthanAlarm;

typedef struct
{
    athan_type athanType;
    uint8_t   Hour;
    uint8_t   Minute;
} TsAthanTime;

typedef struct
{
    athan_type athanType;
    uint8_t   Hour;
    uint8_t   Minute;
    AMPM      currentAMPM;
} TsCurrentTime;

typedef struct
{
    athan_type athanType;
    TsDateTime dateTime;
} TsCurrentDate;

typedef struct
{
    TsAthanTime athanTimes[NUMBER_OF_ATHAN];
} TsAthanTimesDay;

typedef enum UART_CMD
{
    RESET_CMD,
    ALARM_CMD,
    TIME_HOUR_CMD,
    TIME_MIN_CMD
} UART_CMD;

#endif /* COMMON_H_ */
