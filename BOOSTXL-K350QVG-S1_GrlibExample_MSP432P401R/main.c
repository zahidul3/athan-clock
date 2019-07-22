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

#define CC_INT_PORT         GPIO_PORT_P6
#define CC_INT_PIN          GPIO_PIN0

#define HEARTBEAT_PORT      GPIO_PORT_P1
#define HEARTBEAT_PIN       GPIO_PIN0

#define HEARTBEAT_PORT      GPIO_PORT_P1
#define HEARTBEAT_PIN       GPIO_PIN0

#define BUTTON1_PORT        GPIO_PORT_P1
#define BUTTON1_PIN         GPIO_PIN1

/* Application Defines */
#define SLAVE_ADDRESS_1     0x40
#define NUM_OF_RX_BYTES     4

#define NAMAZ_START_POS     40
#define NAMAZ_SPACING       40

#define NAMAZ_TIME_START_POS_X     140
#define NAMAZ_TIME_SIZE 5

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
  CURRENT_TIME = 0xFF
} athan_type;

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
} TsCurrentTime;

typedef struct
{
    TsAthanTime athanTimes[NUMBER_OF_ATHAN];
} TsAthanTimesDay;

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

//![Simple UART Config]
/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 9600 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 *http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        78,                                     // BRDIV = 78
        2,                                       // UCxBRF = 2
        0,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};
//![Simple UART Config]


/******************************************************************************
 * The USCI_B0 data ISR RX vector is used to move received data from the I2C
 * master to the MSP432 memory.
 ******************************************************************************/
void EUSCIB0_IRQHandler(void)
{
    uint_fast16_t status;
    uint_fast8_t data;

    status = MAP_I2C_getEnabledInterruptStatus(EUSCI_B0_BASE);
    MAP_I2C_clearInterruptFlag(EUSCI_B0_BASE, status);

    /* RXIFG for Slave Address 1*/
    if (status & EUSCI_B_I2C_START_INTERRUPT)
    {
        MAP_I2C_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
    }
    else if (status & EUSCI_B_I2C_RECEIVE_INTERRUPT0)
    {
        data = MAP_I2C_slaveGetData(EUSCI_B0_BASE);
        Graphics_drawString(&g_sContext, "I2C Received", NAMAZ_TIME_SIZE, NAMAZ_TIME_START_POS_X, NAMAZ_START_POS+NAMAZ_SPACING*6, TRANSPARENT_TEXT);
    }
}

void initI2C(void)
{

    /* Select Port 1 for I2C - Set Pin 6, 7 to input Primary Module Function,
     *   (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
     */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN6 + GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    /* eUSCI I2C Slave Configuration */
    MAP_I2C_initSlave(EUSCI_B0_BASE, SLAVE_ADDRESS_1, EUSCI_B_I2C_OWN_ADDRESS_OFFSET0, EUSCI_B_I2C_OWN_ADDRESS_ENABLE);

    //MAP_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_MODE);

    /* Enable the module and enable interrupts */
    MAP_I2C_enableModule(EUSCI_B0_BASE);
    MAP_Interrupt_enableInterrupt(INT_EUSCIB0);

    MAP_I2C_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_START_INTERRUPT);
    MAP_Interrupt_enableSleepOnIsrExit();
}

static uint32_t index = 0;

typedef enum UARTLCDstate{
    UART_LCD_IDLE,
    UART_FINISH_CURRENT_TIME,
    UART_FINISH_ATHAN_TIME,
    UART_RECEIVING_ATHAN_TIME,
    UART_RECEIVING_CURRENT_TIME
} UARTLCDSTATE;

UARTLCDSTATE UartLcdState = UART_LCD_IDLE;

uint8_t CurrentHour;
uint8_t CurrentMinute;

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

/* EUSCI A0 UART ISR - Echoes data back to PC host */
void EUSCIA2_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);

    MAP_UART_clearInterruptFlag(EUSCI_A2_BASE, status);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        if(UartLcdState <= UART_FINISH_ATHAN_TIME)
            index = 0;


        rxBuffer[index] = MAP_UART_receiveData(EUSCI_A2_BASE);
        //MAP_UART_transmitData(EUSCI_A0_BASE, rxBuffer[index]);
        if(rxBuffer[index] == CURRENT_TIME && UartLcdState <= UART_FINISH_ATHAN_TIME && index == 0)
            UartLcdState = UART_RECEIVING_CURRENT_TIME;
        else if(rxBuffer[index] == FAJR && index == 0)
            UartLcdState = UART_RECEIVING_ATHAN_TIME;

        index++;

        if(index >= sizeof(TsCurrentTime) && UartLcdState == UART_RECEIVING_CURRENT_TIME)
        {
            printDebug("Received current time\n\r");
            GPIO_toggleOutputOnPin(HEARTBEAT_PORT, HEARTBEAT_PIN);

            CurrentHour = rxBuffer[1];
            CurrentMinute = rxBuffer[2];
            //setCurrentTime12Hr(currentDateTime);
            drawMainMenu();
            UartLcdState = UART_FINISH_CURRENT_TIME;
            index = 0;
            //memcpy(&currentAthanTimesDay.athanTimes[0].athanType, rxBuffer, sizeof(currentAthanTimesDay));
        }

        if(index >= sizeof(currentAthanTimesDay) && UartLcdState == UART_RECEIVING_ATHAN_TIME) //18
        {
            memcpy(&currentAthanTimesDay.athanTimes[0].athanType, rxBuffer, sizeof(currentAthanTimesDay));
            g_ranDemo = true;
            printDebug("Received athan time\n\r");
            drawMainMenu();
            UartLcdState = UART_FINISH_ATHAN_TIME;
            index = 0;
        }
    }
}

void initUART0Debug(void)
{
    /* Selecting P1.2 and P1.3 in UART mode */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Setting DCO to 12MHz */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    //![Simple UART Example]
    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A0_BASE);

    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT | EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA0);
    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableMaster();
    //![Simple UART Example]
}

void initUART(void)
{
    /* Selecting P3.2/PM_UCA2RXD/PM_UCA2SOMI and P3.3/PM_UCA2TXD/PM_UCA2SIMO in UART mode */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
            GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Setting DCO to 12MHz */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    //![Simple UART Example]
    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A2_BASE, &uartConfig);

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A2_BASE);

    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA2);
    //MAP_Interrupt_enableSleepOnIsrExit();
    //MAP_Interrupt_enableMaster();
    //![Simple UART Example]
}

char * DebugString;
uint8_t DebugStringLen;
uint8_t DebugStringIndex;
bool printDebugInProgress = false;

/* EUSCI A0 UART ISR - Echoes data back to PC host */
void EUSCIA0_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);

    MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        MAP_UART_transmitData(EUSCI_A0_BASE, MAP_UART_receiveData(EUSCI_A0_BASE));
    }
    else if(status & EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
    {
        if(DebugStringLen)
        {
            MAP_UART_transmitData(EUSCI_A0_BASE, DebugString[DebugStringIndex++]);
            DebugStringLen--;
        }
        else
        {
            printDebugInProgress = false;
            free(DebugString);
        }
    }

}

void printDebug(char * debugString)
{
    if(printDebugInProgress == false)
    {
        DebugStringIndex = 0;
        printDebugInProgress = true;
        DebugStringLen = strlen(debugString);
        DebugString = malloc(DebugStringLen);
        memcpy(DebugString, debugString, DebugStringLen);
        MAP_UART_transmitData(EUSCI_A0_BASE, DebugString[DebugStringIndex++]);
        DebugStringLen--;
    }
}

void buttonIntHandler(void)
{
    GPIO_clearInterruptFlag(BUTTON1_PORT, BUTTON1_PIN);
    //GPIO_toggleOutputOnPin(HEARTBEAT_PORT, HEARTBEAT_PIN);

    printDebug("Button Pressed!\n\r");
    GPIO_toggleOutputOnPin(CC_INT_PORT, CC_INT_PIN);

    MAP_UART_transmitData(EUSCI_A2_BASE, 'I');
}

uint32_t sysTickCount = 0;
/*
 * SysTick interrupt handler. This handler toggles RGB LED on/off.
 */
void SysTick_Handler(void)
{
    sysTickCount++;
    GPIO_toggleOutputOnPin(HEARTBEAT_PORT, HEARTBEAT_PIN);

}

void main(void)
 {
//  int16_t ulIdx;
    WDT_A_hold(__WDT_A_BASE__);
    MAP_Interrupt_disableMaster();

    /* Initialize the demo. */
    boardInit();
    clockInit();
    initializeDemoButtons();

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

    MAP_SysTick_setPeriod(48000000);
    MAP_SysTick_enableModule();
    MAP_SysTick_enableInterrupt();

    MAP_Interrupt_enableMaster();

    /* Globally enable interrupts. */
    __enable_interrupt();

    // LCD setup using Graphics Library API calls
    Kitronix320x240x16_SSD2119Init();
    Graphics_initContext(&g_sContext, &g_sKitronix320x240x16_SSD2119);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setFont(&g_sContext, &g_sFontCmss36b);
    Graphics_clearDisplay(&g_sContext);

    touch_initInterface();

    //initI2C();
    drawMainMenu();
    //drawTimeMenu();

    initUART();
    initUART0Debug(); //9600baud 8N1

    printDebug("Hello World!\n\r");

    // Loop to detect touch
    while(1)
    {
        touch_updateCurrentTouch(&g_sTouchContext);

        if(g_ranDemo == true)
        {
            g_ranDemo = false;
            drawMainMenu();
        }

        if( g_sTouchContext.touch)
        {
            if(Graphics_isImageButtonSelected(&primitiveButton, g_sTouchContext.x,  g_sTouchContext.y)){
                Graphics_drawSelectedImageButton(&g_sContext,&primitiveButton);
                runPrimitivesDemo();
            }else if (Graphics_isImageButtonSelected(&imageButton, g_sTouchContext.x,  g_sTouchContext.y)){
                Graphics_drawSelectedImageButton(&g_sContext,&imageButton);
                runImagesDemo();
            }

            if(g_ranDemo == true)
            {
                g_ranDemo = false;
                drawMainMenu();
            }
        }
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

void drawTimeMenu(void)
{
    char buffer[NAMAZ_TIME_SIZE];

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
    Graphics_drawString(&g_sContext, "TIME", sizeof("TIME"), 3, NAMAZ_START_POS+NAMAZ_SPACING*6, TRANSPARENT_TEXT);

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    sprintf(buffer,"%02d:%02d", CurrentHour, CurrentMinute);
    Graphics_drawString(&g_sContext, buffer, NAMAZ_TIME_SIZE, NAMAZ_TIME_START_POS_X, NAMAZ_START_POS+NAMAZ_SPACING*6, TRANSPARENT_TEXT);
}

void drawMainMenu(void)
{
    char buffer[NAMAZ_TIME_SIZE];

    printDebug("Drawing Main Menu\n\r");

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawString(&g_sContext, "Athan Times", sizeof("Athan Times"), 3, 3, TRANSPARENT_TEXT);

    // Draw TI banner at the bottom of screnn
//    Graphics_drawImage(&g_sContext,
//        &TI_platform_bar_red4BPP_UNCOMP,
//        0,
//        Graphics_getDisplayHeight(&g_sContext) - TI_platform_bar_red4BPP_UNCOMP.ySize);

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
    Graphics_drawString(&g_sContext, "Fajr", sizeof("Fajr"), 3, NAMAZ_START_POS+NAMAZ_SPACING*0, TRANSPARENT_TEXT);
    Graphics_drawString(&g_sContext, "Sunrise", sizeof("Sunrise"), 3, NAMAZ_START_POS+NAMAZ_SPACING*1, TRANSPARENT_TEXT);
    Graphics_drawString(&g_sContext, "Dhuhr", sizeof("Dhuhr"), 3, NAMAZ_START_POS+NAMAZ_SPACING*2, TRANSPARENT_TEXT);
    Graphics_drawString(&g_sContext, "Asr", sizeof("Asr"), 3, NAMAZ_START_POS+NAMAZ_SPACING*3, TRANSPARENT_TEXT);
    Graphics_drawString(&g_sContext, "Maghrib", sizeof("Maghrib"), 3, NAMAZ_START_POS+NAMAZ_SPACING*4, TRANSPARENT_TEXT);
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

    sprintf(buffer,"%02d:%02d", CurrentHour, CurrentMinute);
    Graphics_drawString(&g_sContext, buffer, NAMAZ_TIME_SIZE, NAMAZ_TIME_START_POS_X, NAMAZ_START_POS+NAMAZ_SPACING*6, TRANSPARENT_TEXT);
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

void clockInit(void)
{
    /* 2 flash wait states, VCORE = 1, running off DC-DC, 48 MHz */
    FlashCtl_setWaitState( FLASH_BANK0, 2);
    FlashCtl_setWaitState( FLASH_BANK1, 2);
    PCM_setPowerState( PCM_AM_DCDC_VCORE1 );
    CS_setDCOCenteredFrequency( CS_DCO_FREQUENCY_48 );
    CS_setDCOFrequency(48000000);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, 375);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, 1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, 1);
    /* Setting MCLK to REFO at 128Khz for LF mode */
    //MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    //MAP_CS_initClockSignal(CS_MCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    //MAP_PCM_setPowerState(PCM_AM_LF_VCORE0);
    /* Configure and enable SysTick */

    return;
}


void Delay(uint16_t msec){
    uint32_t i=0;
    uint32_t time=(msec/1000)*(SYSTEM_CLOCK_SPEED/15);

    for(i=0;i<time;i++);
}
