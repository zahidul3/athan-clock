/*
 * athan_app.c
 *
 *  Created on: Oct 28, 2020
 *      Author: zahidhaq
 */
/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stdbool.h>
#include <bcomdef.h>
#include <ti/common/cc26xx/uartlog/UartLog.h>
#include "athanCalendar.h"
#include "athanTransport.h"
#include "athanTransportPHY.h"
#include "uart_lcd.h"
#include "project_zero.h"
#include "audiotest.h"
#include "athan_app.h"

/*********************************************************************
 * MACROS
 */
#define SEC_CALIBRATION_THRESHOLD 16384 //20000

/*********************************************************************
 * TYPEDEFS
 */
int32_t secCalibrationCnt = 0;

#pragma DATA_ALIGN(currentDateTime, 8)
TsDateTime currentDateTime =
        {
         13, //sec
         20, //min
         12, //hour
         25,  //day
         12,  //month
         19, //year (last 2 digits)
         3,  //dayOfWeek 0-6, where 0 = Sunday
         0b01011000   //zone
        };

#pragma DATA_ALIGN(currentAthanTimesDay, 8)
TsAthanTimesDay currentAthanTimesDay =     {FAJR, 5, 7,       \
                                            SUNRISE, 6, 18,   \
                                            ZUHR, 12, 48,     \
                                            ASR, 5, 27,       \
                                            MAGHRIB, 7, 19,   \
                                            ISHA, 8, 49,      \
                                           };


/*********************************************************************
 * GLOBAL VARIABLES
 */
bool gUpdateAthanTime = FALSE;
uint8_t gSecondDelay = 0;
uint8_t athanTimesIndex = 0;
uint8_t EnableSleepAfterSec = 0;
TsCurrentTime athanTimeMatch;

#pragma DATA_ALIGN(currentAthanAlarmEnable, 8)
TsAthanAlarm currentAthanAlarmEnable = {true, false, true, true, true, true};

AMPM currentAMPM = PM;
AMPM athanAMPM = AM;

uint8_t currentHourStandard = 0;
uint8_t currentMinStandard = 0;

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * EXTERNS
 */
extern UARTLCDSTATE UartLcdState;

const volatile TsDaysMonthsConfig constDaysMonthsConfig = \
{
"Sun","Mon","Tue","Wed","Thu","Fri","Sat",
"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};

void PrintCurrentDateTime(void)
{
    Log_info6("currentDateTime: %s %02d %02d:%02d:%02d %s", (void *)&constDaysMonthsConfig.months[currentDateTime.Month-1], currentDateTime.Day,\
              currentHourStandard, currentMinStandard, currentDateTime.Second, (currentAMPM ? "PM" : "AM"));
}

// Get real current time from connecting to iOS device via BLE
void UpdateCurrentTimeFromIOS(TsIOSDateTime* pDateTimeIOS)
{
    currentDateTime.Year = (uint8_t)(pDateTimeIOS->Year - 2000);
    currentDateTime.Month = pDateTimeIOS->Month;
    currentDateTime.Day = pDateTimeIOS->Day;
    currentDateTime.Hour = pDateTimeIOS->Hour;
    currentDateTime.Minute = pDateTimeIOS->Minute;
    currentDateTime.Second = pDateTimeIOS->Second;
    currentDateTime.DayOfWeek = pDateTimeIOS->DayOfWeek;
}

void setAthanAlarm(uint8_t alarmBits)
{
    int index = 0;
    for(index = 0; index < NUMBER_OF_ATHAN; index++)
    {
        currentAthanAlarmEnable.athanAlarm[index] = alarmBits & 0x01;
        alarmBits = alarmBits >> 1;
    }
}

void SendAthanTimes(void)
{
    switch(currentDateTime.Month)
    {
        case JANUARY:
            memcpy(&currentAthanTimesDay.athanTimes[0], &ATHAN_SCHEDULE.athanMonths.athanTimesJan[currentDateTime.Day-1].athanTimes[0], sizeof(TsAthanTimesDay));
            break;
        case FEBRUARY:
            memcpy(&currentAthanTimesDay.athanTimes[0], &ATHAN_SCHEDULE.athanMonths.athanTimesFeb[currentDateTime.Day-1].athanTimes[0], sizeof(TsAthanTimesDay));
            break;
        case MARCH:
            memcpy(&currentAthanTimesDay.athanTimes[0], &ATHAN_SCHEDULE.athanMonths.athanTimesMar[currentDateTime.Day-1].athanTimes[0], sizeof(TsAthanTimesDay));
            break;
        case APRIL:
            memcpy(&currentAthanTimesDay.athanTimes[0], &ATHAN_SCHEDULE.athanMonths.athanTimesApr[currentDateTime.Day-1].athanTimes[0], sizeof(TsAthanTimesDay));
            break;
        case MAY:
            memcpy(&currentAthanTimesDay.athanTimes[0], &ATHAN_SCHEDULE.athanMonths.athanTimesMay[currentDateTime.Day-1].athanTimes[0], sizeof(TsAthanTimesDay));
            break;
        case JUNE:
            memcpy(&currentAthanTimesDay.athanTimes[0], &ATHAN_SCHEDULE.athanMonths.athanTimesJun[currentDateTime.Day-1].athanTimes[0], sizeof(TsAthanTimesDay));
            break;
        case JULY:
            memcpy(&currentAthanTimesDay.athanTimes[0], &ATHAN_SCHEDULE.athanMonths.athanTimesJul[currentDateTime.Day-1].athanTimes[0], sizeof(TsAthanTimesDay));
            break;
        case AUGUST:
            memcpy(&currentAthanTimesDay.athanTimes[0], &ATHAN_SCHEDULE.athanMonths.athanTimesAug[currentDateTime.Day-1].athanTimes[0], sizeof(TsAthanTimesDay));
            break;
        case SEPTEMBER:
            memcpy(&currentAthanTimesDay.athanTimes[0], &ATHAN_SCHEDULE.athanMonths.athanTimesSep[currentDateTime.Day-1].athanTimes[0], sizeof(TsAthanTimesDay));
            break;
        case OCTOBER:
            memcpy(&currentAthanTimesDay.athanTimes[0], &ATHAN_SCHEDULE.athanMonths.athanTimesOct[currentDateTime.Day-1].athanTimes[0], sizeof(TsAthanTimesDay));
            break;
        case NOVEMBER:
            memcpy(&currentAthanTimesDay.athanTimes[0], &ATHAN_SCHEDULE.athanMonths.athanTimesNov[currentDateTime.Day-1].athanTimes[0], sizeof(TsAthanTimesDay));
            break;
        case DECEMBER:
            memcpy(&currentAthanTimesDay.athanTimes[0], &ATHAN_SCHEDULE.athanMonths.athanTimesDec[currentDateTime.Day-1].athanTimes[0], sizeof(TsAthanTimesDay));
            break;
    }
    gUpdateAthanTime = true;
    gSecondDelay = (currentDateTime.Second + 7) % 60;
}

void SendCurrentDateTimeToLCD(void)
{
    uint32_t key;
    //CRITICAL_ENTER(&key);
    DISABLE_SLEEP();
    Log_info0("Writing current date/time");
    SendAthanPacket(MSG_CURRENT_TIME, &currentDateTime.Second, sizeof(TsDateTime));
    //CRITICAL_EXIT(&key);
}

void SendAthanTimesToLCD(void)
{
    uint32_t key;
    //CRITICAL_ENTER(&key);
    DISABLE_SLEEP();
    Log_info0("Writing athan times");
    SendAthanPacket(MSG_ATHAN_TIMES, &currentAthanTimesDay.athanTimes[0].athanType, sizeof(TsAthanTimesDay));
    //CRITICAL_EXIT(&key);
}

void sendAthanMatchToLCD(void)
{
    uint32_t key;
    //CRITICAL_ENTER(&key);
    DISABLE_SLEEP();
    Log_info0("Writing athan match");
    SendAthanPacket(MSG_ATHAN_ALERT, &athanTimeMatch, sizeof(TsCurrentTime));
    //CRITICAL_EXIT(&key);
}

void sendTestDateTimeToLCD(void)
{
    TsDateTime testDateTime;
    uint8_t x = 0;
    uint32_t key;
    //CRITICAL_ENTER(&key);
    DISABLE_SLEEP();
    Log_info0("Writing test date time");
    testDateTime.Second = x++;
    testDateTime.Minute = x++;
    testDateTime.Hour = x++;
    testDateTime.Day = x++;
    testDateTime.Month = x++;
    testDateTime.Year = x++;
    testDateTime.DayOfWeek = x++;
    testDateTime.TimeZone = x++;
    memcpy(&testDateTime.Second, &currentDateTime.Second, sizeof(TsDateTime));
    SendAthanPacket(MSG_CURRENT_TIME, &testDateTime.Second, sizeof(TsDateTime));

    //CRITICAL_EXIT(&key);
}

//                        jan feb mar apr may jun jul aug sep oct nov dec
const uint8_t dates[] = {0, 31, 28, 31, 30, 31, 30, 31, 31 ,30, 31, 30, 31 };
uint8 DaysInMonth(uint8_t month, uint8_t year)
{
  if(month>12) return 0;
  else if((month==2) && ((year & 0x03)==0)) return 29;
  else return dates[month];
}

void ModifyDateTime(uint8 param)
{
    if(param == TIME_HOUR_CMD)
    {
        DATE_TIME.Hour++;
        DATE_TIME.Minute--;
    }
    //else if(param == TIME_MIN_CMD)
    //    DATE_TIME.Minute++;

    IncDateTime(1);
    //sendCurrentDateTimeToLCD();
    //SendUART1CurrentTime();
}

/*
timeZone   Bits: assz zzzz

a:  0 = winter time (no offset)
    1 = summer time (+1 offset)

z:  2s compliment 5 bit time zone offset

s:  Adjust Scheme. As above
    00 = Use config adjustment data
    01 = No adjustment
    10 = USA dates
    11 = not defined
*/

#define DAYLIGHT_SAVINGS_ON       0x80
#define ZONE_SCHEME               0x03

// ----------------------------------------------------------------------------
static int8_t GetTimeZone(TsDateTime * time)
{
  int8_t zone;

  zone = time->TimeZone & 0x1F;
  if(zone & 0x10) zone |= 0xE0;  // sign extended  -12 to +12
#ifndef MS1
  zone = 0;
#endif
  return zone;
}
// ----------------------------------------------------------------------------
static void SetTimeZone(TsDateTime * time, int8_t dzone)
{
  int8_t zone;

  zone = dzone & 0x1F;
  time->TimeZone = (time->TimeZone & 0xE0) | zone;
}

bool isAthanTime(TsDateTime dateTime, TsCurrentTime* athanTimeMatch)
{
    uint8 athanHour;
    uint8 athanMin;

    //convert to 12 hr format
    if(dateTime.Hour>12)
    {
        dateTime.Hour -= 12;
        currentAMPM = PM;
    }
    else if(dateTime.Hour==0)
    {
        dateTime.Hour = 12;
        currentAMPM = AM;
    }
    else if(dateTime.Hour==12)
    {
        currentAMPM = PM;
    }
    else
    {
        currentAMPM = AM;
    }

    currentHourStandard = dateTime.Hour;
    currentMinStandard = dateTime.Minute;

    for(int x=0; x<NUMBER_OF_ATHAN; x++)
    {
        athanHour = currentAthanTimesDay.athanTimes[x].Hour;
        athanMin = currentAthanTimesDay.athanTimes[x].Minute;

        switch(x)
        {
        case FAJR:
            athanAMPM = AM;
            break;
        case SUNRISE:
            athanAMPM = AM;
            continue;
        case ZUHR:
            if(athanHour < 12)
                athanAMPM = AM;
            else
                athanAMPM = PM;
            break;
        case ASR:
            athanAMPM = PM;
            break;
        case MAGHRIB:
            athanAMPM = PM;
            break;
        case ISHA:
            athanAMPM = PM;
            break;
        }

        if((currentHourStandard == athanHour) && (currentMinStandard == athanMin) && (currentAMPM == athanAMPM) && (currentAthanAlarmEnable.athanAlarm[x] == true))
        {
            Log_info0("Athan time happened!!!");
            athanTimeMatch->athanType = x;
            athanTimeMatch->Minute = currentMinStandard;
            athanTimeMatch->Hour = currentHourStandard;
            athanTimeMatch->currentAMPM = athanAMPM;
            return true;
        }
    }
    return false;
}

void EnableSleepAfterDelay(uint8_t delayInSeconds)
{
    EnableSleepAfterSec = DATE_TIME.Second + delayInSeconds; // Enable sleep after 2 seconds
}

// 1Hz Main Current Time calculation loop
void IncDateTime(uint8 updateStats)
{
    //add extra seconds to calibrate for delay
    secCalibrationCnt++;
    if(secCalibrationCnt > SEC_CALIBRATION_THRESHOLD)
    {
        secCalibrationCnt = 0;
        DATE_TIME.Second++;
    }
    DATE_TIME.Second++;

    if(EnableSleepAfterSec && (EnableSleepAfterSec == DATE_TIME.Second))
    {
        ENABLE_SLEEP();
        EnableSleepAfterSec = 0;
    }

    ToggleHeartbeatLED();

    //update athan time with delay
    if(gUpdateAthanTime && (DATE_TIME.Second==gSecondDelay))
    {
        sendUART1data();
        gUpdateAthanTime = false;
    }

  if(DATE_TIME.Second>=60 || updateStats)
  {
    DATE_TIME.Second = 0;
    DATE_TIME.Minute++;
    if(DATE_TIME.Minute>=60)
    {
      DATE_TIME.Minute = 0;
      DATE_TIME.Hour++;
      if(DATE_TIME.Hour>=24)
      {
        DATE_TIME.Hour = 0;
        DATE_TIME.DayOfWeek++;
        if(DATE_TIME.DayOfWeek>=7) DATE_TIME.DayOfWeek = 0;
        DATE_TIME.Day++;
        if(DATE_TIME.Day>DaysInMonth(DATE_TIME.Month, DATE_TIME.Year))
        {
          DATE_TIME.Day = 1;
          DATE_TIME.Month++;
          if(DATE_TIME.Month>12)
          {
            DATE_TIME.Month = 1;
            DATE_TIME.Year++;
          }
        }

        SendCurrentDateTimeToLCD(); //update date on daily basis
      }

      uint8_t tz;

      tz = (DATE_TIME.TimeZone >> 5) & ZONE_SCHEME;
      if(tz==1); // USA no daylight savings
      if(tz==2) // USA normal daylight savings
      {
        if((DATE_TIME.Hour==2) && (DATE_TIME.DayOfWeek==0))                                // sunday 2am
        {
          if((DATE_TIME.Month==3) && ((DATE_TIME.Day>=8) && (DATE_TIME.Day<=15))  && ((DATE_TIME.TimeZone & DAYLIGHT_SAVINGS_ON)==0))                // 2nd sunday in march and not adjusted
          {
            DATE_TIME.Hour=3;                               // advance 1 hour
            SetTimeZone(&DATE_TIME, GetTimeZone(&DATE_TIME) + 1);
            DATE_TIME.TimeZone |= DAYLIGHT_SAVINGS_ON;      // set DAYLIGHT_SAVINGS_ON
          }
          else if((DATE_TIME.Month==11) && ((DATE_TIME.Day>=1) && (DATE_TIME.Day<=7))  && (DATE_TIME.TimeZone & DAYLIGHT_SAVINGS_ON)) // 1st sunday in november and adjusted
          {
            DATE_TIME.Hour=1;                               // back 1 hour
            SetTimeZone(&DATE_TIME, GetTimeZone(&DATE_TIME) - 1);
            DATE_TIME.TimeZone &= ~DAYLIGHT_SAVINGS_ON;     // clear DAYLIGHT_SAVINGS_ON
          }
        }
      }

      SendAthanTimes();
    }

    // Send current time to LCD
    SendUART1CurrentTime();
    SaveCurrentTimeToFlash();

    if(isAthanTime(DATE_TIME, &athanTimeMatch)) //check every min if athan should play
    {
        SetPlaybackAzanEvent();
        PostUARTLCDAthanAlert();
    }
  }
}

void HandleRxMessage(TsAthanPacket* athanPacket)
{
    switch(athanPacket->dataType){
        case MSG_LCD_CMD:
        //Log_info0("MSG_LCD_CMD rx!\n\r");
        TsLCDTouchCmd* lcdCmd = (TsLCDTouchCmd*)&athanPacket->data;
        Log_info1("MSG_LCD_CMD rx! lcd_cmd: %d\n\r", lcdCmd->command);
        switch(lcdCmd->command)
        {
        case TURN_OFF_ALARM:
            Log_info0("TURN_OFF_ALARM rx!\n\r");
            TurnOffAthanAlarm();
            break;
        case ALARM_CMD:
            Log_info1("ALARM_CMD rx: 0x%02x!\n\r", lcdCmd->data);
            setAthanAlarm(lcdCmd->data);
            break;
        default:
            break;
        }
        break;
        case MSG_BUTTON_PRESS:
            TsButtonPress* buttonPressCmd = (TsButtonPress*)&athanPacket->data;
            switch(buttonPressCmd->button){
            case RIGHT_KEY:
                Log_info0("MSG_BUTTON_PRESS rx: RIGHT_KEY!\n\r");
                SetPlaybackAzanEvent();
                break;
            case LEFT_KEY:
                Log_info0("MSG_BUTTON_PRESS rx: LEFT_KEY!\n\r");
                SetRecordAzanEvent();
                break;
            default:
                Log_info0("MSG_BUTTON_PRESS rx: UNKNOWN_KEY!\n\r");
                break;
            }
            break;
            case MSG_DEVICE_RESET:
                Log_info0("MSG_DEVICE_RESET rx!\n\r");
                SendCurrentDateTimeToLCD();
                SendAthanTimes();
                break;
            case MSG_CURRENT_TIME:
                Log_info0("MSG_CURRENT_TIME rx!\n\r");
                break;
            case MSG_ATHAN_TIMES:
                Log_info0("MSG_ATHAN_TIMES rx!\n\r");
                break;
            case MSG_ATHAN_ALERT:
                Log_info0("MSG_ATHAN_ALERT rx!!!\n\r");
                break;
            default:
                break;
    }
}

void AthanApp_init(void)
{
    initUART_LCD();
    initLCDPinInt();
    // resurrect the time last saved from flash
    readCurrentTimeFromFlash();
    SendAthanTimes();
    if(isAthanTime(DATE_TIME, &athanTimeMatch)) //check every min
    {
        SetPlaybackAzanEvent();
        PostUARTLCDAthanAlert();
    }
    SendCurrentDateTimeToLCD();
}
