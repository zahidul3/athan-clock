/*
 * button_led.c
 *
 *  Created on: Oct 29, 2020
 *      Author: zahidhaq
 */
/* Pin driver handles */
#include <stdlib.h>
#include <Board.h>
#include <ti/common/cc26xx/uartlog/UartLog.h>
#include "button_led.h"
#include "project_zero.h"

/* Global memory storage for a PIN_Config table */
static PIN_State ledPinState;
static PIN_State intPinState;

static void intCallbackFxn(PIN_Handle handle, PIN_Id pinId);

PIN_Handle intPinHandle;
PIN_Handle ledPinHandle;

/*
 * Initial LED pin configuration table
 *   - LEDs Board_PIN_LED0 & Board_PIN_LED1 are off.
 */
PIN_Config ledPinTable[] = {
    PZ_RLED_PIN | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_PIN_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

/*
 * Initial LED pin configuration table
 *   - LEDs Board_PIN_LED0 & Board_PIN_LED1 are off.
 */
PIN_Config intPinTable[] = {
    LCD_INT_PIN | PIN_INPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

void ToggleHeartbeatLED(void)
{
    //Heartbeat GREEN LED @ 1Hz
    if(PIN_getOutputValue(PZ_GLED_PIN))
        PIN_setOutputValue(ledPinHandle, PZ_GLED_PIN, 0);
    else
        PIN_setOutputValue(ledPinHandle, PZ_GLED_PIN, 1);
}

void EnableLCDEdgeInt(void)
{
    PIN_setConfig(intPinHandle, PIN_BM_IRQ, LCD_INT_PIN | PIN_IRQ_POSEDGE);
}

static uint32_t timeStampSec = 0;

/*********************************************************************
 * @fn     intCallbackFxn
 *
 * @brief  Callback from PIN driver on interrupt
 *
 *         Sets in motion the debouncing.
 *
 * @param  handle    The PIN_Handle instance this is about
 * @param  pinId     The pin that generated the interrupt
 */
static void intCallbackFxn(PIN_Handle handle, PIN_Id pinId)
{
    if((GetRunningTime() - timeStampSec) > 2) // 2s debounce
    {
        timeStampSec = GetRunningTime();
        // Disable interrupt on that pin for now. Re-enabled after debounce.
        //PIN_setConfig(handle, PIN_BM_IRQ, pinId | PIN_IRQ_DIS);
        DISABLE_SLEEP();
        Log_info0("In Button interrupt");
        FlushUARTRXFifo();
        UartReadLCD();
    }
}

void initLCDPinInt(void)
{
    // Open LED pins
    ledPinHandle = PIN_open(&ledPinState, ledPinTable);
    if(!ledPinHandle)
    {
        Log_error0("Error initializing board LED pins");
    }

    // Open LCD Interrupt Pin
    intPinHandle = PIN_open(&intPinState, intPinTable);
    if(!intPinHandle)
    {
        Log_error0("Error initializing intPinHandle pins");
    }

    EnableLCDEdgeInt();

    // Setup callback for button pins
    if(PIN_registerIntCb(intPinHandle, &intCallbackFxn) != 0)
    {
        Log_error0("Error registering int callback function");
    }
}
