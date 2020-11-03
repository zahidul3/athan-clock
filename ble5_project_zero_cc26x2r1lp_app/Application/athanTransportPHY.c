/*
 * athanTransportPHY.c
 *
 *  Created on: Oct 28, 2020
 *      Author: zahidhaq
 */

#include <ti/drivers/UART.h>
#include <ti/common/cc26xx/uartlog/UartLog.h>  // Comment out if using xdc Log
#include "athanTransportPHY.h"

extern UART_Handle uart1Handle;


void SendAthanPacketPHY(TsAthanPacket* athanPacketPHY, uint8_t athanPacketLen)
{
    if(uart1Handle)
    {
        Log_info1("Writing packet to LCD len: %d", athanPacketLen);
        UART_write(uart1Handle, athanPacketPHY, athanPacketLen);
    }
}

