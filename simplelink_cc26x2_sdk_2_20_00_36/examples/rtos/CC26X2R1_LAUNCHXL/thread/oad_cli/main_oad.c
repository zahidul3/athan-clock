/******************************************************************************

 @file main.c

 @brief Bootstrapping code for the OpenThread Stack thread and Application thread.

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

/* Standard Library Header files */
#include <assert.h>
#include <stdint.h>
#include <string.h>

/* POSIX Header files */
#include <sched.h>
#include <pthread.h>

/* RTOS header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/NVS.h>
#include <ti/drivers/UART.h>
#include <ti/sysbios/BIOS.h>

#include <ti/drivers/AESECB.h>
#include <ti/drivers/SHA2.h>
#include <ti/drivers/ECJPAKE.h>

/* Example/Board Header files */
#include "Board.h"

/* Private configuration Header files */
#include "task_config.h"

#define OAD_SINGLE_APP  1
#include "oad_image_header.h"

/* Low level driverlib files (non-rtos) */
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/flash.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/cpu.h)

/**
 * Invalidate the OAD IMAGE HEADER.
 *
 * This invalidates the OAD header thus, on the next reboot the BIM
 * will find an invalid image and restore the factory iamge.
 */
static void invalidate_oad_image_header(void)
{
    /*
     * We only need to invalidate the IMAGE header
     * We can do this by writing a zero  "zeros" over the signature.
     */

    /* no IRQ chance, we disable here */
    CPUcpsid();

    /* our data buffer cannot be in flash... so it is on the stack */
    uint8_t zeros[ sizeof( oad_image_header.h.imgID ) ];
    memset( zeros, 0, sizeof(zeros) );
    FlashProgram( &zeros[0],
                  (uint32_t)(&oad_image_header.h.imgID),
                  sizeof(oad_image_header.h.imgID) );

    /* press the virtual reset button */
    SysCtrlSystemReset();
}

/**
 * Entry point for the application.
 *
 * Creates the OpenThread Stack task and Application task, then starts the
 * TI-RTOS scheduler.
 *
 * @note This function should never return.
 */
int main(void)
{
    /* Call driver init functions */
    Board_initGeneral();

    GPIO_init();

    GPIO_setConfig(Board_GPIO_BUTTON1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING);

    NVS_init();

    /* The BTN, when not pressed reads as 1, when pressed reads as 0 */
    if( !GPIO_read( Board_GPIO_BTN1 ) )
    {
        invalidate_oad_image_header();
    }

    UART_init();

    ECJPAKE_init();

    AESECB_init();

    SHA2_init();

    cli_taskCreate();

    OtStack_taskCreate();

    /* Start sys/bios, this will never return */
    BIOS_start();

    return (0);
}
