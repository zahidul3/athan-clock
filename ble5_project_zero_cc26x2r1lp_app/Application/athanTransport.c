/*
 * athanTransport.c
 *
 *  Created on: Oct 25, 2020
 *      Author: zahidhaq
 */
#include <string.h>

#include "athanTransportPHY.h"
#include "athanTransport.h"

#define MAX_PACKET_LEN 256
uint8_t MsgBuffer[MAX_PACKET_LEN] = {0};

void SendAthanPacket(athan_packet_type cmd, uint8_t* data, uint8_t dataLen)
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

    SendAthanPacketPHY(athanPacket, athanPacketLen);
}
