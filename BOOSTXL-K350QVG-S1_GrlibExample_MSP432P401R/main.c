/* --COPYRIGHT--,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/


/*********************************************************************
 * INCLUDES
 */
/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/* GrLib Includes */
#include "grlib.h"
#include "button.h"
#include "imageButton.h"
#include "radioButton.h"
#include "checkbox.h"
#include "LcdDriver/kitronix320x240x16_ssd2119_spi.h"
#include "images/images.h"
#include "touch_P401R.h"

/* App Includes */
#include "uart_debug.h"
#include "uart_cc.h"
#include "IoDef.h"
#include "Common.h"

//Touch screen context
touch_context g_sTouchContext;
Graphics_ImageButton primitiveButton;
Graphics_ImageButton imageButton;
Graphics_Button yesButton;
Graphics_Button noButton;

// Graphic library context
Graphics_Context g_sContext;

//Flag to know if a demo was run
bool g_ranDemo = false;

void Delay(uint16_t msec);
void boardInit(void);
void clockInit(void);
void initializeDemoButtons(void);
void drawMainMenu(void);
void runPrimitivesDemo(void);
void runImagesDemo(void);
void drawRestarDemo(void);

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
/* Application Defines */
#define NAMAZ_START_POS     40
#define NAMAZ_SPACING       40

#define NAMAZ_TIME_START_POS_X     140
#define NAMAZ_TIME_SIZE 5

/*********************************************************************
 * TYPEDEFS
 */
typedef enum LCD_SCREEN
{
    MAIN_SCREEN,
    DATE_SCREEN
}LCD_SCREEN;

typedef struct
{
  UART_CMD  command;
  uint8_t   data;
} __attribute__((__packed__)) TxData;

typedef enum UARTLCDSTATE{
    UART_LCD_IDLE,
    UART_FINISH_CURRENT_TIME,
    UART_FINISH_CURRENT_DATE,
    UART_FINISH_ATHAN_TIME,
    UART_RECEIVING_ATHAN_TIME,
    UART_RECEIVING_CURRENT_TIME,
    UART_RECEIVING_CURRENT_DATE
} UARTLCDSTATE;

/*********************************************************************
 * GLOBAL VARIABLES
 */
TsDateTime currentDateTime =
        {
         0, //sec
         48, //min
         12, //hour
         7, //day
         5, //month
         19,//year
         2, //dayOfWeek 0-6, where 0 = Sunday
         0 //zone
        };

TsAthanTimesDay currentAthanTimesDay = {0};

uint8_t rxBuffer[20] = {0};

uint8_t index = 0;

LCD_SCREEN LcdScreen = MAIN_SCREEN;

TxData txData = {0};
uint8_t TxDataLen = 0;

UARTLCDSTATE UartLcdState = UART_LCD_IDLE;

/*********************************************************************
 * LOCAL VARIABLES
 */
uint8_t CurrentHour;
uint8_t CurrentMinute;
AMPM CurrentAMPM = PM;

uint16_t athanThreshold[6]= {4000, 5200, 6700, 8200, 9700, 17000};
const char* const arr[] = { "Fajr", "Sunrise", "Dhuhr", "Asr", "Maghrib", "Isha", 0 }; // all const
uint8_t alarmState = 0b00111101; //inverted bit field of alarm state

int32_t athanDescColor[6] = {GRAPHICS_COLOR_GREEN, GRAPHICS_COLOR_GRAY, GRAPHICS_COLOR_GREEN, GRAPHICS_COLOR_GREEN, GRAPHICS_COLOR_GREEN, GRAPHICS_COLOR_GREEN};

char buffer[DEBUG_BUFFER_SIZE];
char bufferTouch[DEBUG_BUFFER_SIZE];
bool TxDone = false;

static uint32_t sysTickCount = 0;
static uint16_t xSum, ySum;

void setCurrentTime12Hr(TsDateTime dateTime)
{
    //convert to 12 hr format
    if(dateTime.Hour>12)
    {
        dateTime.Hour -= 12;
    }
    else if(dateTime.Hour==0)
    {
        dateTime.Hour = 12;
    }
    CurrentHour = dateTime.Hour;
    CurrentMinute = dateTime.Minute;
}

/* EUSCI A0 UART ISR - Receives one byte at a time */
void EUSCIA2_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);

    MAP_UART_clearInterruptFlag(EUSCI_A2_BASE, status);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        if((UartLcdState <= UART_FINISH_ATHAN_TIME) && index > 9)
            index = 0;

        rxBuffer[index] = MAP_UART_receiveData(EUSCI_A2_BASE);

        if((rxBuffer[index] == CURRENT_TIME) && (UartLcdState <= UART_FINISH_ATHAN_TIME) && (index == 0))
            UartLcdState = UART_RECEIVING_CURRENT_TIME;
        else if((rxBuffer[index] == CURRENT_DATE) && (UartLcdState <= UART_FINISH_ATHAN_TIME) && (index == 0))
            UartLcdState = UART_RECEIVING_CURRENT_DATE;
        else if((rxBuffer[index] == FAJR) && (index == 0))
            UartLcdState = UART_RECEIVING_ATHAN_TIME;

        index++;

        if((index >= sizeof(TsCurrentTime)) && (UartLcdState == UART_RECEIVING_CURRENT_TIME))
        {
            CurrentHour = rxBuffer[1];
            CurrentMinute = rxBuffer[2];
            CurrentAMPM = rxBuffer[3];

            memset(buffer, 0, DEBUG_BUFFER_SIZE);
            ltoa(CurrentHour,buffer);
            uint8_t strLen = strlen(buffer);
            buffer[strLen] = ':';
            ltoa(CurrentMinute,(buffer+strLen+1));
            printDebugString("Received current time: ");
            printDebugString(buffer);
            printDebugString("\n\r");

            drawMainMenu();
            UartLcdState = UART_FINISH_CURRENT_TIME;
            index = 0;
            //memcpy(&currentAthanTimesDay.athanTimes[0].athanType, rxBuffer, sizeof(currentAthanTimesDay));
        }

        if((index >= sizeof(TsCurrentDate)) && (UartLcdState == UART_RECEIVING_CURRENT_DATE))
        {
            memcpy(&currentDateTime.Second, &rxBuffer[1], sizeof(TsDateTime));
            memset(buffer, 0, DEBUG_BUFFER_SIZE);
            ltoa(currentDateTime.Month,buffer);
            uint8_t strLen = strlen(buffer);
            buffer[strLen] = '/';
            ltoa(currentDateTime.Day,(buffer+strLen+1));
            printDebugString("Received current date: ");
            printDebugString(buffer);
            printDebugString("\n\r");

            setCurrentTime12Hr(currentDateTime);
            drawDateTimeMenu();
            UartLcdState = UART_FINISH_CURRENT_DATE;
            index = 0;
        }

        if((index >= sizeof(currentAthanTimesDay)) && (UartLcdState == UART_RECEIVING_ATHAN_TIME)) //18
        {
            memcpy(&currentAthanTimesDay.athanTimes[0].athanType, rxBuffer, sizeof(currentAthanTimesDay));
            g_ranDemo = true;
            printDebugString("Received athan time\n\r");
            drawMainMenu();
            UartLcdState = UART_FINISH_ATHAN_TIME;
            index = 0;
        }
    }

    if(status & EUSCI_A_UART_TRANSMIT_INTERRUPT)
    {
        if(TxDone != true)
        {
            TxDataLen--;
            uint8_t dataByte = GetUARTData();
            MAP_UART_transmitData(EUSCI_A2_BASE, dataByte);
            TxDone = true;
        }
        else
        {
            //polling_delay_ms(7); //wait for CC to wake and stabilize UART
            GPIO_setOutputLowOnPin(CC_INT_PORT, CC_INT_PIN);
            GPIO_enableInterrupt(BUTTON1_PORT, BUTTON1_PIN);
        }
    }
}

uint8_t GetUARTData()
{
    if(TxDataLen == sizeof(txData))
        return txData.command;
    else
        return txData.data;
}

//Sends data to CC device
void SendUARTCmd(uint8_t* data, uint8_t len)
{
    TxDone = false;
    GPIO_setOutputHighOnPin(CC_INT_PORT, CC_INT_PIN);
    polling_delay_ms(7); //wait for CC to wake and stabilize UART
    TxDataLen = len;
    uint8_t dataByte = GetUARTData();
    //printDebugInt(dataByte);
    MAP_UART_transmitData(EUSCI_A2_BASE, dataByte);
}

void buttonIntHandler(void)
{
    GPIO_disableInterrupt(BUTTON1_PORT, BUTTON1_PIN);
    printDebugString("Button Pressed!\n\r");
    GPIO_clearInterruptFlag(BUTTON1_PORT, BUTTON1_PIN);

    txData.command = 0x00;
    txData.data = 0x01;
    SendUARTCmd(&txData, sizeof(txData));
}

/*
 * SysTick interrupt handler. This handler toggles HEARTBEAT LED on/off.
 */
void SysTick_Handler(void)
{
    sysTickCount++;
    GPIO_toggleOutputOnPin(HEARTBEAT_PORT, HEARTBEAT_PIN);
}

//Crude polling method
void polling_delay_ms(uint16_t millis)
{
    uint32_t i,j = 0;
    uint32_t time = 120000;

    //for(j = 0; j < millis; j++)
    {
        for(i = 0; i < (time*millis); i++)
        {
            ;
        }
    }
}

void SendResetCmd(void)
{
    printDebugString("Sending reset cmd\n\r");
    txData.command = RESET_CMD;
    txData.data = 0;
    SendUARTCmd(&txData, sizeof(txData));
}

void main(void)
{
    int16_t ulIdx;
    WDT_A_hold(__WDT_A_BASE__);
    MAP_Interrupt_disableMaster();

    /* Initialize the demo. */
    boardInit();
    clockInit();
    initializeDemoButtons();

    polling_delay_ms(64);
    //
    // Configure the pins that connect to the LCD as GPIO outputs.
    //
    GPIO_setAsOutputPin(CC_INT_PORT, CC_INT_PIN);
    GPIO_setOutputLowOnPin(CC_INT_PORT, CC_INT_PIN);

    GPIO_setAsOutputPin(HEARTBEAT_PORT, HEARTBEAT_PIN);
    GPIO_setOutputLowOnPin(HEARTBEAT_PORT, HEARTBEAT_PIN);

    GPIO_toggleOutputOnPin(HEARTBEAT_PORT, HEARTBEAT_PIN);

    //setup S1 Left Button GPIO
    GPIO_setAsInputPinWithPullUpResistor(BUTTON1_PORT, BUTTON1_PIN);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_interruptEdgeSelect(BUTTON1_PORT, BUTTON1_PIN, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_registerInterrupt(BUTTON1_PORT, buttonIntHandler);
    GPIO_enableInterrupt(BUTTON1_PORT, BUTTON1_PIN);

    MAP_SysTick_setPeriod(12000000);
    MAP_SysTick_enableModule();

    initUART_CC();      //9600baud 8N1
    initUART0Debug();   //921600baud 8N1

    //MAP_Interrupt_enableSleepOnIsrExit();

    /* Globally enable interrupts. */
    MAP_Interrupt_enableMaster();
    MAP_SysTick_enableInterrupt();

    // LCD setup using Graphics Library API calls
    Kitronix320x240x16_SSD2119Init();
    Graphics_initContext(&g_sContext, &g_sKitronix320x240x16_SSD2119);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setFont(&g_sContext, &g_sFontCmss36b);
    Graphics_clearDisplay(&g_sContext);

    touch_initInterface();

    SendResetCmd();
    drawMainMenu();

    printDebugString("Hello World!\n\r");
/*
    while(1)
    {
        MAP_PCM_gotoLPM0();
    }
*/

    // Loop to detect touch
    while(1)
    {
        /* Wait for a touch to be detected and wait ~4ms. */
        while(!touch_detectedTouch())
        {
            ;
        }
        touch_delay();

        /* Sample the X and Y measurements of the touch screen. */
        xSum = touch_sampleX();
        ySum = touch_sampleY();

        if(LcdScreen == DATE_SCREEN)
            drawMainMenu();

        if(xSum > 1024)
        {
            memset(bufferTouch, 0, DEBUG_BUFFER_SIZE);
            ltoa(xSum, bufferTouch);
            uint8_t strLenX = strlen(bufferTouch);
            bufferTouch[strLenX] = ',';
            ltoa(ySum,(&bufferTouch[strLenX+1]));
            uint8_t strLen = strlen(bufferTouch);
            bufferTouch[strLen] = '\n';
            bufferTouch[strLen+1] = '\r';
            bufferTouch[strLen+2] = 0;
            //ltoa(ySum,(bufferTouch+strLen+1));
            //printDebugString("Touched: ");
            printDebugString(bufferTouch);

            if(ySum < 4600)
            {
                printDebugString("Touched Min!");
                txData.command = TIME_MIN_CMD;
                txData.data = 0x00;
                SendUARTCmd(&txData, sizeof(txData));
            }
            else if(ySum < 7600)
            {
                printDebugString("Touched Hour!");
                txData.command = TIME_HOUR_CMD;
                txData.data = 0x00;
                SendUARTCmd(&txData, sizeof(txData));
            }
            else if((ySum < 14000) && (xSum > 12000))
            {
                printDebugString("Touched TIME!");
                drawDateTimeMenu();
            }
            else
            {
                for(ulIdx=0; ulIdx<6; ulIdx++)
                {
                    if(xSum <= athanThreshold[ulIdx])
                    {
                        printDebugInt(ulIdx);
                        printDebugString(arr[ulIdx]);
                        if(athanDescColor[ulIdx] == GRAPHICS_COLOR_GREEN)
                        {
                            athanDescColor[ulIdx] = GRAPHICS_COLOR_GRAY;
                            alarmState = (alarmState & ~(1<<ulIdx)); //clear bit
                            txData.command = ALARM_CMD;
                            txData.data = alarmState;
                            SendUARTCmd(&txData, sizeof(txData));
                        }
                        else
                        {
                            athanDescColor[ulIdx] = GRAPHICS_COLOR_GREEN;
                            alarmState = (alarmState | (1<<ulIdx)); //set bit
                            txData.command = ALARM_CMD;
                            txData.data = alarmState;
                            SendUARTCmd(&txData, sizeof(txData));
                        }

                        drawMainMenu();
                        break;
                    }
                }
            }
        }

        while(touch_detectedTouch())
        {
            ;
        }
        //printDebugString("\n\r");

        //SendUARTCmd('A');
        //touch_updateCurrentTouch(&g_sTouchContext);

//        if(g_ranDemo == true)
//        {
//            g_ranDemo = false;
//            drawMainMenu();
//        }

//        if( g_sTouchContext.touch)
//        {
//            if(Graphics_isImageButtonSelected(&primitiveButton, g_sTouchContext.x,  g_sTouchContext.y)){
//                Graphics_drawSelectedImageButton(&g_sContext,&primitiveButton);
//                runPrimitivesDemo();
//            }else if (Graphics_isImageButtonSelected(&imageButton, g_sTouchContext.x,  g_sTouchContext.y)){
//                Graphics_drawSelectedImageButton(&g_sContext,&imageButton);
//                runImagesDemo();
//            }
//
//            if(g_ranDemo == true)
//            {
//                g_ranDemo = false;
//                drawMainMenu();
//            }
//        }
    }

}

void initializeDemoButtons(void)
{
    // Initiliaze primitives Demo Button
    primitiveButton.xPosition=20;
    primitiveButton.yPosition=50;
    primitiveButton.borderWidth=5;
    primitiveButton.selected=false;
    primitiveButton.imageWidth=Primitives_Button4BPP_UNCOMP.xSize;
    primitiveButton.imageHeight=Primitives_Button4BPP_UNCOMP.ySize;
    primitiveButton.borderColor=GRAPHICS_COLOR_WHITE;
    primitiveButton.selectedColor=GRAPHICS_COLOR_RED;
    primitiveButton.image=&Primitives_Button4BPP_UNCOMP;


    // Initiliaze images Demo Button
    imageButton.xPosition=180;
    imageButton.yPosition=50;
    imageButton.borderWidth=5;
    imageButton.selected=false;
    imageButton.imageWidth=Primitives_Button4BPP_UNCOMP.xSize;
    imageButton.imageHeight=Primitives_Button4BPP_UNCOMP.ySize;
    imageButton.borderColor=GRAPHICS_COLOR_WHITE;
    imageButton.selectedColor=GRAPHICS_COLOR_RED;
    imageButton.image=&Images_Button4BPP_UNCOMP;

    yesButton.xMin = 80;
    yesButton.xMax = 150;
    yesButton.yMin = 80;
    yesButton.yMax = 120;
    yesButton.borderWidth = 1;
    yesButton.selected = false;
    yesButton.fillColor = GRAPHICS_COLOR_RED;
    yesButton.borderColor = GRAPHICS_COLOR_RED;
    yesButton.selectedColor = GRAPHICS_COLOR_BLACK;
    yesButton.textColor = GRAPHICS_COLOR_BLACK;
    yesButton.selectedTextColor = GRAPHICS_COLOR_RED;
    yesButton.textXPos = 100;
    yesButton.textYPos = 90;
    yesButton.text = "YES";
    yesButton.font = &g_sFontCm18;

    noButton.xMin = 180;
    noButton.xMax = 250;
    noButton.yMin = 80;
    noButton.yMax = 120;
    noButton.borderWidth = 1;
    noButton.selected = false;
    noButton.fillColor = GRAPHICS_COLOR_RED;
    noButton.borderColor = GRAPHICS_COLOR_RED;
    noButton.selectedColor = GRAPHICS_COLOR_BLACK;
    noButton.textColor = GRAPHICS_COLOR_BLACK;
    noButton.selectedTextColor = GRAPHICS_COLOR_RED;
    noButton.textXPos = 200;
    noButton.textYPos = 90;
    noButton.text = "NO";
    noButton.font = &g_sFontCm18;
}

//Draws TIME at bottom of screen
void drawTimeMenu(void)
{
    char buffer[NAMAZ_TIME_SIZE];

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
    Graphics_drawString(&g_sContext, "TIME", sizeof("TIME"), 3, NAMAZ_START_POS+NAMAZ_SPACING*6, TRANSPARENT_TEXT);

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    sprintf(buffer,"%02d:%02d", CurrentHour, CurrentMinute);
    Graphics_drawString(&g_sContext, buffer, NAMAZ_TIME_SIZE, NAMAZ_TIME_START_POS_X, NAMAZ_START_POS+NAMAZ_SPACING*6, TRANSPARENT_TEXT);
}

void drawDateTimeMenu(void)
{
    char buffer[NAMAZ_TIME_SIZE];

    LcdScreen = DATE_SCREEN;
    printDebugString("Drawing Date Time Menu\n\r");

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawString(&g_sContext, "Date Times", sizeof("Date Times"), 3, 3, TRANSPARENT_TEXT);

    //Graphics_drawString(&g_sContext, (void *)&constDaysMonthsConfig.months[currentDateTime.Month-1], 3, 3, NAMAZ_START_POS+NAMAZ_SPACING*0, TRANSPARENT_TEXT);
    sprintf(buffer,"%02d-%02d-%02d", currentDateTime.Month, currentDateTime.Day, currentDateTime.Year);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_drawString(&g_sContext, buffer, 8, 3, NAMAZ_START_POS+NAMAZ_SPACING*0, TRANSPARENT_TEXT);
}

void drawMainMenu(void)
{
    char buffer[NAMAZ_TIME_SIZE*2];

    LcdScreen = MAIN_SCREEN;
    printDebugString("Drawing Main Menu\n\r");

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawString(&g_sContext, "Athan Times", sizeof("Athan Times"), 3, 3, TRANSPARENT_TEXT);

    // Draw TI banner at the bottom of screnn
//    Graphics_drawImage(&g_sContext,
//        &TI_platform_bar_red4BPP_UNCOMP,
//        0,
//        Graphics_getDisplayHeight(&g_sContext) - TI_platform_bar_red4BPP_UNCOMP.ySize);

    Graphics_setForegroundColor(&g_sContext, athanDescColor[0]);
    Graphics_drawString(&g_sContext, "Fajr", sizeof("Fajr"), 3, NAMAZ_START_POS+NAMAZ_SPACING*0, TRANSPARENT_TEXT);
    Graphics_setForegroundColor(&g_sContext, athanDescColor[1]);
    Graphics_drawString(&g_sContext, "Sunrise", sizeof("Sunrise"), 3, NAMAZ_START_POS+NAMAZ_SPACING*1, TRANSPARENT_TEXT);
    Graphics_setForegroundColor(&g_sContext, athanDescColor[2]);
    Graphics_drawString(&g_sContext, "Dhuhr", sizeof("Dhuhr"), 3, NAMAZ_START_POS+NAMAZ_SPACING*2, TRANSPARENT_TEXT);
    Graphics_setForegroundColor(&g_sContext, athanDescColor[3]);
    Graphics_drawString(&g_sContext, "Asr", sizeof("Asr"), 3, NAMAZ_START_POS+NAMAZ_SPACING*3, TRANSPARENT_TEXT);
    Graphics_setForegroundColor(&g_sContext, athanDescColor[4]);
    Graphics_drawString(&g_sContext, "Maghrib", sizeof("Maghrib"), 3, NAMAZ_START_POS+NAMAZ_SPACING*4, TRANSPARENT_TEXT);
    Graphics_setForegroundColor(&g_sContext, athanDescColor[5]);
    Graphics_drawString(&g_sContext, "Isha", sizeof("Isha"), 3, NAMAZ_START_POS+NAMAZ_SPACING*5, TRANSPARENT_TEXT);

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
    Graphics_drawString(&g_sContext, "TIME", sizeof("TIME"), 3, NAMAZ_START_POS+NAMAZ_SPACING*6, TRANSPARENT_TEXT);

    sprintf(buffer,"%02d:%02d", currentAthanTimesDay.athanTimes[0].Hour, currentAthanTimesDay.athanTimes[0].Minute);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_drawString(&g_sContext, buffer, NAMAZ_TIME_SIZE, NAMAZ_TIME_START_POS_X, NAMAZ_START_POS+NAMAZ_SPACING*0, TRANSPARENT_TEXT);
    sprintf(buffer,"%02d:%02d", currentAthanTimesDay.athanTimes[1].Hour, currentAthanTimesDay.athanTimes[1].Minute);
    Graphics_drawString(&g_sContext, buffer, NAMAZ_TIME_SIZE, NAMAZ_TIME_START_POS_X, NAMAZ_START_POS+NAMAZ_SPACING*1, TRANSPARENT_TEXT);
    sprintf(buffer,"%02d:%02d", currentAthanTimesDay.athanTimes[2].Hour, currentAthanTimesDay.athanTimes[2].Minute);
    Graphics_drawString(&g_sContext, buffer, NAMAZ_TIME_SIZE, NAMAZ_TIME_START_POS_X, NAMAZ_START_POS+NAMAZ_SPACING*2, TRANSPARENT_TEXT);
    sprintf(buffer,"%02d:%02d", currentAthanTimesDay.athanTimes[3].Hour, currentAthanTimesDay.athanTimes[3].Minute);
    Graphics_drawString(&g_sContext, buffer, NAMAZ_TIME_SIZE, NAMAZ_TIME_START_POS_X, NAMAZ_START_POS+NAMAZ_SPACING*3, TRANSPARENT_TEXT);
    sprintf(buffer,"%02d:%02d", currentAthanTimesDay.athanTimes[4].Hour, currentAthanTimesDay.athanTimes[4].Minute);
    Graphics_drawString(&g_sContext, buffer, NAMAZ_TIME_SIZE, NAMAZ_TIME_START_POS_X, NAMAZ_START_POS+NAMAZ_SPACING*4, TRANSPARENT_TEXT);
    sprintf(buffer,"%02d:%02d", currentAthanTimesDay.athanTimes[5].Hour, currentAthanTimesDay.athanTimes[5].Minute);
    Graphics_drawString(&g_sContext, buffer, NAMAZ_TIME_SIZE, NAMAZ_TIME_START_POS_X, NAMAZ_START_POS+NAMAZ_SPACING*5, TRANSPARENT_TEXT);

    sprintf(buffer,"%02d:%02d%s", CurrentHour, CurrentMinute, (CurrentAMPM ? "PM" : "AM"));
    Graphics_drawString(&g_sContext, buffer, NAMAZ_TIME_SIZE+2, NAMAZ_TIME_START_POS_X-(50), NAMAZ_START_POS+NAMAZ_SPACING*6, TRANSPARENT_TEXT);
    // Draw Primitives image button
    //Graphics_drawImageButton(&g_sContext, &primitiveButton);

    // Draw Images image button
    //Graphics_drawImageButton(&g_sContext, &imageButton);
}

void runPrimitivesDemo(void)
{
    int16_t ulIdx;
    uint32_t color;

    Graphics_Rectangle myRectangle1 = { 10, 50, 155, 120};
    Graphics_Rectangle myRectangle2 = { 150, 100, 300, 200};
    Graphics_Rectangle myRectangle3 = { 0, 0, 319, 239};

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawString(&g_sContext, "Draw Pixels & Lines", AUTO_STRING_LENGTH, 60, 5, TRANSPARENT_TEXT);
    Graphics_drawPixel(&g_sContext, 45, 45);
    Graphics_drawPixel(&g_sContext, 45, 50);
    Graphics_drawPixel(&g_sContext, 50, 50);
    Graphics_drawPixel(&g_sContext, 50, 45);
    Graphics_drawLine(&g_sContext, 60, 60, 200, 200);
    Graphics_drawLine(&g_sContext, 30, 200, 200, 60);
    Graphics_drawLine(&g_sContext, 0, Graphics_getDisplayHeight(&g_sContext) - 1,
            Graphics_getDisplayWidth(&g_sContext) - 1,
            Graphics_getDisplayHeight(&g_sContext) - 1);
    Delay(2000);
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, "Draw Rectangles", AUTO_STRING_LENGTH, 159, 15, TRANSPARENT_TEXT);
    Graphics_drawRectangle(&g_sContext, &myRectangle1);
    Graphics_fillRectangle(&g_sContext, &myRectangle2);
    // Text won't be visible on screen due to transparency (foreground colors match)
    Graphics_drawStringCentered(&g_sContext, "Normal Text", AUTO_STRING_LENGTH, 225, 120, TRANSPARENT_TEXT);
    // Text draws foreground and background for opacity
    Graphics_drawStringCentered(&g_sContext, "Opaque Text", AUTO_STRING_LENGTH, 225, 150, OPAQUE_TEXT);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);

    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    // Text draws with inverted foreground color to become visible
    Graphics_drawStringCentered(&g_sContext, "Invert Text", AUTO_STRING_LENGTH, 225, 180, TRANSPARENT_TEXT);
    Delay(2000);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    // Invert the foreground and background colors
    Graphics_fillRectangle(&g_sContext, &myRectangle3);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_drawStringCentered(&g_sContext, "Invert Colors", AUTO_STRING_LENGTH, 159, 15, TRANSPARENT_TEXT);
    Graphics_drawRectangle(&g_sContext, &myRectangle1);
    Graphics_fillRectangle(&g_sContext, &myRectangle2);
    // Text won't be visible on screen due to transparency
    Graphics_drawStringCentered(&g_sContext, "Normal Text", AUTO_STRING_LENGTH, 225, 120, TRANSPARENT_TEXT);
    // Text draws foreground and background for opacity
    Graphics_drawStringCentered(&g_sContext, "Opaque Text", AUTO_STRING_LENGTH, 225, 150, OPAQUE_TEXT);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    // Text draws with inverted color to become visible
    Graphics_drawStringCentered(&g_sContext, "Invert Text", AUTO_STRING_LENGTH, 225, 180, TRANSPARENT_TEXT);
    Delay(2000);
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, "Draw Circles", AUTO_STRING_LENGTH, 159, 15, TRANSPARENT_TEXT);
    Graphics_drawCircle(&g_sContext, 100, 100, 50);
    Graphics_fillCircle(&g_sContext, 200, 140, 70);
    Delay(2000);
    Graphics_clearDisplay(&g_sContext);
    // Add some more color
    Graphics_setForegroundColor(&g_sContext, ClrLawnGreen);
    Graphics_setBackgroundColor(&g_sContext, ClrBlack);
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, "Rainbow of Colored Lines", AUTO_STRING_LENGTH, 159, 15, TRANSPARENT_TEXT);
    // Draw a quarter rectangle sweep of lines from red to purple.
    for(ulIdx = 128; ulIdx >= 1; ulIdx--)
    {
        // Red Color
        * ((uint16_t*) (&color)+1)  = 255;
        // Blue and Green Colors
        * ((uint16_t*) (&color))  = ((((128 - ulIdx) * 255) >> 7) << ClrBlueShift);

        Graphics_setForegroundColor(&g_sContext, color);
        Graphics_drawLine(&g_sContext, 160, 200, 32, ulIdx + 72);
    }
    // Draw a quarter rectangle sweep of lines from purple to blue.
    for(ulIdx = 128; ulIdx >= 1; ulIdx--)
    {
        // Red Color
        * ((uint16_t*) (&color)+1)  = (ulIdx * 255) >> 7;
        // Blue and Green Colors
        * ((uint16_t*) (&color))  = 255 << ClrBlueShift;

        Graphics_setForegroundColor(&g_sContext, color);
        Graphics_drawLine(&g_sContext, 160, 200, 160 - ulIdx, 72);
    }
    // Clear Red Color
    * ((uint16_t*) (&color)+1)  = 0;
    // Draw a quarter rectangle sweep of lines from blue to teal.
    for(ulIdx = 128; ulIdx >= 1; ulIdx--)
    {
        // Blue and Green Colors
        * ((uint16_t*) (&color))  = ((((128 - ulIdx) * 255) >> 7) << ClrGreenShift) | (255 << ClrBlueShift);

        Graphics_setForegroundColor(&g_sContext, color);
        Graphics_drawLine(&g_sContext, 160, 200, 288 - ulIdx, 72);
    }
    // Draw a quarter rectangle sweep of lines from teal to green.
    for(ulIdx = 128; ulIdx >= 0; ulIdx--)
    {
        // Blue and Green Colors
        * ((uint16_t*) (&color))  = (255 << ClrGreenShift) | (((ulIdx * 255) >> 7) << ClrBlueShift);

        Graphics_setForegroundColor(&g_sContext, color);
        Graphics_drawLine(&g_sContext, 160, 200, 288, 200 - (ulIdx));
    }
    Delay(2000);
    g_ranDemo = true;

    drawRestarDemo();

}



void runImagesDemo(void)
{
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, "Draw Uncompressed Image", AUTO_STRING_LENGTH, 159, 200, TRANSPARENT_TEXT);
    Delay(2000);
    // Draw Image on the display
    Graphics_drawImage(&g_sContext, &lcd_color_320x2408BPP_UNCOMP, 0, 0);
    Delay(2000);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, "Draw RLE4 compressed Image", AUTO_STRING_LENGTH, 159, 200, TRANSPARENT_TEXT);
    Delay(2000);
    Graphics_drawImage(&g_sContext, &TI_logo_150x1501BPP_COMP_RLE4, 85, 45);
    Delay(2000);

    g_ranDemo = true;

    drawRestarDemo();
}

void drawRestarDemo(void)
{
    g_ranDemo = false;
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, "Would you like to go back",
        AUTO_STRING_LENGTH,
        159,
        45,
        TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "to the main menu?",
        AUTO_STRING_LENGTH,
        159,
        65,
        TRANSPARENT_TEXT);

    // Draw Primitives image button
    Graphics_drawButton(&g_sContext, &yesButton);

    // Draw Images image button
    Graphics_drawButton(&g_sContext, &noButton);

    do{
        touch_updateCurrentTouch(&g_sTouchContext);
        if(Graphics_isButtonSelected(&noButton, g_sTouchContext.x,  g_sTouchContext.y))
        {
            Graphics_drawSelectedButton(&g_sContext, &noButton);
            g_ranDemo = true;
        }else{
            if(g_ranDemo)
            {
                Graphics_drawReleasedButton(&g_sContext, &noButton);
                g_ranDemo = false;
            }
        }
    }while(!Graphics_isButtonSelected(&yesButton, g_sTouchContext.x,  g_sTouchContext.y));

    Graphics_drawSelectedButton(&g_sContext, &yesButton);

    g_ranDemo = true;
    Delay(1000);

}


void boardInit()
{
    FPU_enableModule();
}

static volatile uint32_t aclk, mclk, smclk, hsmclk, bclk;


void clockInit(void)
{
    /* 2 flash wait states, VCORE = 1, running off DC-DC, 48 MHz */
    FlashCtl_setWaitState( FLASH_BANK0, 2);
    FlashCtl_setWaitState( FLASH_BANK1, 2);
    PCM_setPowerState( PCM_AM_DCDC_VCORE1 );
    CS_setDCOCenteredFrequency( CS_DCO_FREQUENCY_48 );
    CS_setDCOFrequency(48000000);

    //CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_0);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    /* Setting MCLK to REFO at 128Khz for LF mode */

    MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    //MAP_PCM_setPowerState(PCM_AM_LF_VCORE0);
    /* Configure and enable SysTick */

    aclk = CS_getACLK();
    mclk = CS_getMCLK();
    smclk = CS_getSMCLK();
    hsmclk = CS_getHSMCLK();
    bclk = CS_getBCLK();

    return;
}


void Delay(uint16_t msec){
    uint32_t i=0;
    uint32_t time=(msec/1000)*(SYSTEM_CLOCK_SPEED/15);

    for(i=0;i<time;i++);
}
