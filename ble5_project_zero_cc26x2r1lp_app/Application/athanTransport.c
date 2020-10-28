/*
 * athanTransport.c
 *
 *  Created on: Oct 25, 2020
 *      Author: zahidhaq
 */

#include <ti/drivers/UART.h>
#include <ti/common/cc26xx/uartlog/UartLog.h>  // Comment out if using xdc Log
#include "athanTransport.h"

#define MAX_PACKET_LEN 255
uint8_t MsgBuffer[MAX_PACKET_LEN] = {0};

extern UART_Handle uart1Handle;

void SendAthanPacketToLCD(athan_packet_type cmd, uint8_t* data, uint8_t dataLen)
{
    TsAthanPacket* athanPacket;
    uint8_t athanPacketLen = (ATHAN_PACKET_HEADER_LEN + dataLen);
    static uint8_t seqNum = 0;

    athanPacket = (TsAthanPacket*)MsgBuffer;
    memset(MsgBuffer, 0, MAX_PACKET_LEN);

    athanPacket->version = ATHAN_PACKET_VERSION;
    athanPacket->sequenceNum = seqNum++;
    athanPacket->dataLen = dataLen;
    athanPacket->dataType = cmd;
    memcpy(&athanPacket->data, data, dataLen);

    Log_info1("Writing packet to LCD len: %d", athanPacketLen);

    UART_write(uart1Handle, athanPacket, athanPacketLen);
}
