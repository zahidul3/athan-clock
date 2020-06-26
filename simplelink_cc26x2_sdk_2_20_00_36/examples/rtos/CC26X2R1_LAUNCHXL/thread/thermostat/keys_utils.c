/******************************************************************************

 @file keys_utils.c

 @brief Utility functions for the launch pad keys

 Group: CMCU, LPRF
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2017-2018, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: simplelink_cc26x2_sdk_2_20_00_36
 Release Date: 2018-06-27 10:07:01
 *****************************************************************************/

/******************************************************************************
 Includes
 *****************************************************************************/
#include <stdbool.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/GPIO.h>

/* Board Header files */
#include "Board.h"

#include "keys_utils.h"

/******************************************************************************
 Local Variables
 *****************************************************************************/
/* captures the keys pressed */
static volatile uint8_t keysPressed;

/* Pointer to application callback */
static KeyUtils_PressedCB_t appKeyChangeHandler = NULL;

/* clock handle for the debounce key operation */
static Clock_Handle keyClockHdl;
/* clock struct for the debounce key operation */
static Clock_Struct keyClkStruct;

/******************************************************************************
 Local Functions
 *****************************************************************************/

/**
 * @brief Callback function for the GPIO Button
 *
 * @param index identifies the button pressed.
 * @return None
 */
void gpioButtonFxn(uint_least8_t index)
{
    if (Board_GPIO_BTN1 == index)
    {
        keysPressed |=  KEYS_LEFT;
    }
    else if(Board_GPIO_BTN2 == index)
    {
        keysPressed |=  KEYS_RIGHT;
    }

    if(Clock_isActive(keyClockHdl) != true)
    {
        Clock_start(keyClockHdl);
    }
}

/**
 * @brief       Callback function called when the key debounce
 *              period expires.
 *
 * @param arg   input argument(ignored).
 * @return      None
 */
static void KeysUtils_changeHandler(UArg arg)
{
    (void)arg;

    if(appKeyChangeHandler != NULL)
    {
        /* Notify the application */
        (*appKeyChangeHandler)(keysPressed);

        /* Clear keys */
        keysPressed = 0;
    }
}

/******************************************************************************
 External Functions
 *****************************************************************************/

/* refer keys_utils.h */
void KeysUtils_initialize(KeyUtils_PressedCB_t keyCb)
{
    /* install Button1 callback */
    GPIO_setCallback(Board_GPIO_BTN1, gpioButtonFxn);

    /* Enable Button1 interrupt */
    GPIO_enableInt(Board_GPIO_BTN1);

    /* install Button2 callback */
    GPIO_setCallback(Board_GPIO_BTN2, gpioButtonFxn);

    /* Enable Button2 interrupt */
    GPIO_enableInt(Board_GPIO_BTN2);

    /* Initialze and set the clock period for the key debounce */
    Clock_Params clockParams;

    Clock_Params_init(&clockParams);
    clockParams.period = 0;
    clockParams.startFlag = false;
    Clock_construct(&keyClkStruct, KeysUtils_changeHandler, KEY_DEBOUNCE_TIMEOUT, &clockParams);

    keyClockHdl = Clock_handle(&keyClkStruct);

    appKeyChangeHandler = keyCb;
}


