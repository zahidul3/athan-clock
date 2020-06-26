/*
 * Copyright (c) 2016-2018, Texas Instruments Incorporated
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
 */

/*
 *  ======== watchdog.c ========
 */
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Watchdog.h>

/* Example/Board Header files */
#include "Board.h"


volatile bool serviceFlag = true;
volatile bool watchdogExpired = false;
Watchdog_Handle watchdogHandle;

/*
 *  ======== watchdogCallback ========
 *  Watchdog interrupt callback function.
 */
void watchdogCallback(uintptr_t unused)
{
    /* Clear watchdog interrupt flag */
    Watchdog_clear(watchdogHandle);

    watchdogExpired = true;

    /* Insert timeout handling code here. */
}

/*
 *  ======== gpioButtonFxn ========
 *  Callback function for the GPIO interrupt on Board_GPIO_BUTTON0.
 */
void gpioButtonFxn(uint_least8_t index)
{
    /* Clear serviceFlag to stop continuously servicing the watchdog */
    serviceFlag = false;
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    Watchdog_Params params;

    /* Call driver init functions */
    GPIO_init();
    Watchdog_init();

    /* Configure the LED and button pins */
    GPIO_setConfig(Board_GPIO_LED0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_GPIO_BUTTON0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    /* Turn off user LED */
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);

    /* Install Button callback */
    GPIO_setCallback(Board_GPIO_BUTTON0, gpioButtonFxn);

    /* Enable interrupts */
    GPIO_enableInt(Board_GPIO_BUTTON0);

    /* Create and enable a Watchdog with resets disabled */
    Watchdog_Params_init(&params);
    params.callbackFxn = (Watchdog_Callback)watchdogCallback;
    params.resetMode = Watchdog_RESET_OFF;

    watchdogHandle = Watchdog_open(Board_WATCHDOG0, &params);
    if (watchdogHandle == NULL) {
        /* Error opening Watchdog */
        while (1);
    }

    /* Enter continous loop */
    while (true) {

        /* Service watchdog if serviceFlag is true */
        if (serviceFlag) {
            Watchdog_clear(watchdogHandle);
        }

        /* If watchdog expired since last started, turn ON LED */
        if (watchdogExpired) {
            GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
            sleep(5);
            Watchdog_clear(watchdogHandle);
            serviceFlag = true;
            watchdogExpired = false;
            GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
        }
    }
}
