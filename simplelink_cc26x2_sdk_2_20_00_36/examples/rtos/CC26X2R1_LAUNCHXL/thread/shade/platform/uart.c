/******************************************************************************

 @file uart.c

 @brief TIRTOS platform specific uart functions for OpenThread

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

#include <openthread/config.h>

#include <stddef.h>

#include <utils/code_utils.h>
#include <openthread/platform/uart.h>
#include <openthread/types.h>

#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Event.h>

#include "Board.h"
#include "platform.h"

/**
 * Configure the UART core for 115200 baud 8-N-1, no HW flow control.
 *
 * @note Make sure that data being passed to @ref otPlatUartSend is in
 *       persistent readable memory.
 */

/**
 * Event flags marked in @ref PlatformUart_events.
 */
#define PLATFORM_UART_EVENT_TX_DONE  Event_Id_00
#define PLATFORM_UART_EVENT_RX_DONE  Event_Id_01

/**
 * Size of the statically allocated buffer to pass data from the callback to
 * the processing loop.
 */
#define PLATFORM_UART_RECV_BUF_LEN 32

/**
 * Statically allocated data buffer.
 */
static uint8_t PlatformUart_receiveBuffer[PLATFORM_UART_RECV_BUF_LEN];

/**
 * Number of bytes in the receive buffer.
 */
static size_t PlatformUart_receiveLen;

/**
 * The buffer to be sent.
 */
static uint8_t const *PlatformUart_sendBuffer = NULL;

/**
 * TI-RTOS events structure for passing state to the processing loop.
 */
static Event_Struct PlatformUart_events;

/**
 * Uart driver handle.
 */
static UART_Handle PlatformUart_uartHandle;

/**
 * Callback for when the UART driver finishes reading.
 *
 * This is triggered when the buffer is full, or when the UART hardware times
 * out.
 */
static void uartReadCallback(UART_Handle aHandle, void *aBuf, size_t aLen)
{
    (void)aHandle;
    (void)aBuf;
    PlatformUart_receiveLen = aLen;
    Event_post(Event_handle(&PlatformUart_events),
               PLATFORM_UART_EVENT_RX_DONE);
    platformUartSignal();
}

/**
 * Callback for when the UART driver finishes writing a buffer.
 */
static void uartWriteCallback(UART_Handle aHandle, void *aBuf, size_t aLen)
{
    (void)aHandle;
    (void)aBuf;
    (void)aLen;
    /* XXX: partial writes are not handled */
    PlatformUart_sendBuffer = NULL;
    Event_post(Event_handle(&PlatformUart_events),
               PLATFORM_UART_EVENT_TX_DONE);
    platformUartSignal();
}

/**
 * Function documented in platform/uart.h
 */
otError otPlatUartEnable(void)
{
    UART_Params params;

    Event_construct(&PlatformUart_events, NULL);

    UART_Params_init(&params);

    params.readMode         = UART_MODE_CALLBACK;
    params.writeMode        = UART_MODE_CALLBACK;
    params.readCallback     = uartReadCallback;
    params.writeCallback    = uartWriteCallback;
    params.readDataMode     = UART_DATA_BINARY;
    params.writeDataMode    = UART_DATA_BINARY;
    params.readEcho         = UART_ECHO_OFF;
    params.baudRate         = 115200;
    params.dataLength       = UART_LEN_8;
    params.stopBits         = UART_STOP_ONE;
    params.parityType       = UART_PAR_NONE;

    PlatformUart_uartHandle = UART_open(Board_UART0, &params);

    /* allow the uart driver to return before the read buffer is full */
    UART_control(PlatformUart_uartHandle, UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE,
                 NULL);

    /* begin reading from the uart */
    UART_read(PlatformUart_uartHandle, PlatformUart_receiveBuffer,
              sizeof(PlatformUart_receiveBuffer));

    return OT_ERROR_NONE;
}

/**
 * Function documented in platform/uart.h
 */
otError otPlatUartDisable(void)
{
    UART_close(PlatformUart_uartHandle);

    Event_destruct(&PlatformUart_events);

    return OT_ERROR_NONE;
}

/**
 * Function documented in platform/uart.h
 */
otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    otError error = OT_ERROR_NONE;
    otEXPECT_ACTION(PlatformUart_sendBuffer == NULL, error = OT_ERROR_BUSY);
    UART_write(PlatformUart_uartHandle, aBuf, aBufLength);

exit:
    return error;
}

/**
 * Function documented in platform.h
 */
void platformUartProcess(void)
{
    UInt events = Event_pend(Event_handle(&PlatformUart_events), Event_Id_NONE,
            (PLATFORM_UART_EVENT_TX_DONE | PLATFORM_UART_EVENT_RX_DONE),
            BIOS_NO_WAIT);

    if(events & PLATFORM_UART_EVENT_TX_DONE)
    {
        otPlatUartSendDone();
    }

    if(events & PLATFORM_UART_EVENT_RX_DONE)
    {
        otPlatUartReceived(PlatformUart_receiveBuffer, PlatformUart_receiveLen);
        PlatformUart_receiveLen = 0;
        UART_read(PlatformUart_uartHandle, PlatformUart_receiveBuffer,
                  sizeof(PlatformUart_receiveBuffer));

    }
}

