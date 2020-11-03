/*
 * athanTransportPHY.c
 *
 *  Created on: Oct 28, 2020
 *      Author: zahidhaq
 */
#include <stdint.h>
#include "athanTransport.h"
#include "athanTransportPHY.h"

void SendAthanPacketPHY(TsAthanPacket* athanPacketPHY, uint8_t athanPacketLen)
{
    //Log_info1("Writing packet to LCD len: %d", athanPacketLen);
    SendUARTCmd(athanPacketPHY, athanPacketLen);
}
