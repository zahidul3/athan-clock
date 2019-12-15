/*
 * athanCalendar.h
 *
 *  Created on: May 7, 2019
 *      Author: Jahid's Desktop
 */

#ifndef APPLICATION_ATHANCALENDAR_H_
#define APPLICATION_ATHANCALENDAR_H_

#include "Common.h"

#define JANUARY   1
#define FEBRUARY  2
#define MARCH     3
#define APRIL     4
#define MAY       5
#define JUNE      6
#define JULY      7
#define AUGUST    8
#define SEPTEMBER 9
#define OCTOBER   10
#define NOVEMBER  11
#define DECEMBER  12

#define SUNDAY    0
#define MONDAY    1
#define TUESDAY   2
#define WEDNESDAY 3
#define THURSDAY  4
#define FRIDAY    5
#define SATURDAY  6

#define NO_DAYLIGHT_SAVINGS 0x00
#define US_DAYLIGHT_SAVINGS 0x01
#define EU_DAYLIGHT_SAVINGS 0x02

typedef struct
{
    TsAthanTimesDay athanTimesJan[31];
    TsAthanTimesDay athanTimesFeb[29];
    TsAthanTimesDay athanTimesMar[31];
    TsAthanTimesDay athanTimesApr[30];
    TsAthanTimesDay athanTimesMay[31];
    TsAthanTimesDay athanTimesJun[30];
    TsAthanTimesDay athanTimesJul[31];
    TsAthanTimesDay athanTimesAug[31];
    TsAthanTimesDay athanTimesSep[30];
    TsAthanTimesDay athanTimesOct[31];
    TsAthanTimesDay athanTimesNov[30];
    TsAthanTimesDay athanTimesDec[31];
} TsAthanTimesMonth;

typedef union
{
    TsAthanTimesMonth athanMonths;
} TuAthanTimesYear;

extern const volatile TuAthanTimesYear ATHAN_SCHEDULE;

#endif /* APPLICATION_ATHANCALENDAR_H_ */
