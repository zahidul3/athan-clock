/*
 * athanTransport.h
 *
 *  Created on: Oct 25, 2020
 *      Author: zahidhaq
 */

#ifndef APPLICATION_ATHANTRANSPORT_H_
#define APPLICATION_ATHANTRANSPORT_H_

#include <stdint.h>

#define ATHAN_PACKET_VERSION        0x55
#define ATHAN_PACKET_HEADER_LEN     4

typedef enum {
  MSG_CURRENT_TIME, // TsDateTime
  MSG_ATHAN_TIMES,   // TsAthanTimesDay
  MSG_ATHAN_ALERT,  // TsAthanTime

} athan_packet_type;

typedef struct
{
    uint8_t version;
    uint8_t sequenceNum;
    uint8_t dataLen;
    athan_packet_type dataType;
    uint8_t* data;
} TsAthanPacket;


void SendAthanPacketToLCD(athan_packet_type cmd, uint8_t* data, uint8_t dataLen);

#endif /* APPLICATION_ATHANTRANSPORT_H_ */
