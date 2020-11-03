/*
 * Common.h
 *
 *  Created on: Dec 13, 2019
 *      Author: Jahid's Desktop
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum ampm
{
    AM,
    PM
} AMPM;

typedef enum
{
    FAJR = 0,
    SUNRISE,  //1
    ZUHR,     //2
    ASR,      //3
    MAGHRIB,  //4
    ISHA,     //5
    NUMBER_OF_ATHAN, // = 6
} athan_type;

typedef enum
{
    NO_KEY,
    RIGHT_KEY,
    LEFT_KEY,
    ALL_KEYS
} BUTTON_KEYS;

typedef enum
{
    RESET_CMD,
    ALARM_CMD,
    TIME_HOUR_CMD,
    TIME_MIN_CMD
} LCD_TOUCH_CMD;

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
    int8_t    TimeZone;   //
} TsDateTime;

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

typedef struct
{
    BUTTON_KEYS button;
    uint8_t pressed;
} TsButtonPress;

typedef struct
{
  LCD_TOUCH_CMD command;
  uint8_t data;
} TsLCDTouchCmd;

#endif /* COMMON_H_ */
