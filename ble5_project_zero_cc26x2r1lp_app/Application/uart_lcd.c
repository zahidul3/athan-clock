/*
 * uart_lcd.c
 *
 *  Created on: Oct 29, 2020
 *      Author: zahidhaq
 */

#include <ti/common/cc26xx/uartlog/UartLog.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>

#include "Common.h"
#include <Board.h>
#include "button_led.h"
#include "project_zero.h"
#include "athanTransport.h"
#include "uart_lcd.h"

UART_Handle uart1Handle = NULL;
char uart1ReadBuf[16] = {0};

uint8_t AthanPacketRxBuffer[256] = {0};
volatile uint8_t rxByteIndex = 0;
uint8_t athanCMD = 0;
uint8_t athanAlarm = 0;

void FlushUARTRXFifo(void)
{
    rxByteIndex = 0;
    UART_control(uart1Handle, UARTCC26XX_CMD_RX_FIFO_FLUSH,  NULL);
}

void UartReadLCD(void)
{
    UART_read(uart1Handle, uart1ReadBuf, 1);
}

TsAthanPacket rxHeader;
UARTLCDSTATE UartLcdState = UART_LCD_IDLE;

static void LCDUART_readCallBack(UART_Handle handle, void *ptr, size_t size)
{
    uint32_t key;
    uint8_t byteRead;
    CRITICAL_ENTER(&key);

    if(UartLcdState != UART_RECEIVING_ATHAN_PACKET)
        rxByteIndex = 0;

    byteRead = *((uint8_t*)ptr);
    AthanPacketRxBuffer[rxByteIndex] = byteRead;

    if((rxByteIndex == 0) && (byteRead == ATHAN_PACKET_VERSION))
    {
        UartLcdState = UART_RECEIVING_ATHAN_PACKET;
        rxHeader.version = byteRead;
    }

    if(UartLcdState == UART_RECEIVING_ATHAN_PACKET)
    {
        if(rxByteIndex == 1)
        {
            rxHeader.sequenceNum = byteRead;
        }
        else if(rxByteIndex == 2)
        {
            rxHeader.dataLen = byteRead;
        }
        else if(rxByteIndex == 3)
        {
            rxHeader.dataType = byteRead;
        }

        rxByteIndex++;

        if(rxByteIndex >= ATHAN_PACKET_HEADER_LEN + rxHeader.dataLen)
        {
            UartLcdState = UART_COMPLETE_ATHAN_PACKET;
            HandleRxMessage(AthanPacketRxBuffer);
            //EnableLCDEdgeInt();
            UartLcdState = UART_LCD_IDLE;
            ENABLE_SLEEP();
        }
        else
        {
            UART_read(uart1Handle, uart1ReadBuf, 1);
        }
    }
    CRITICAL_EXIT(&key);
}

// -----------------------------------------------------------------------------
//! \brief      This callback is invoked on Write completion
//!
//! \param[in]  handle - handle to the UART port
//! \param[in]  ptr    - pointer to data to be transmitted
//! \param[in]  size   - size of the data
//!
//! \return     void
// -----------------------------------------------------------------------------
static void LCDUART_writeCallBack(UART_Handle handle, void *ptr, size_t size)
{
    uint32_t key;
    CRITICAL_ENTER(&key);
    EnableSleepAfterDelay(2);
    Log_info1("Wrote %d bytes to LCD", size);
    CRITICAL_EXIT(&key);
}

// UART to LCD @9600baud for low power
void initUART_LCD(void)
{
    UART_Params uartParamsLCD;
    UART_Params_init(&uartParamsLCD);

    uartParamsLCD.baudRate = 9600; //
    uartParamsLCD.readDataMode = UART_DATA_BINARY;
    uartParamsLCD.writeDataMode = UART_DATA_BINARY;
    uartParamsLCD.dataLength = UART_LEN_8;
    uartParamsLCD.stopBits = UART_STOP_ONE;
    uartParamsLCD.readMode = UART_MODE_CALLBACK;
    uartParamsLCD.writeMode = UART_MODE_CALLBACK;
    uartParamsLCD.readEcho = UART_ECHO_OFF;

    uartParamsLCD.writeCallback = LCDUART_writeCallBack;
    uartParamsLCD.readCallback = LCDUART_readCallBack;

    uart1Handle = UART_open(Board_UART1, &uartParamsLCD);

    if(uart1Handle != NULL)
        Log_info0("UART 1 Initialized");
    else
        Log_error0("UART 1 NOT Initialized");

    //Enable Partial Reads on all subsequent UART_read()
    UART_control(uart1Handle, UARTCC26XX_RETURN_PARTIAL_ENABLE,  NULL);
}
