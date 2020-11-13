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

typedef enum UARTLCDSTATE{
    UART_LCD_IDLE,
    UART_RECEIVING_ATHAN_PACKET,
    UART_COMPLETE_ATHAN_PACKET
} UARTLCDSTATE;

typedef enum
{
    RESET_CMD, //0
    ALARM_CMD, //1
    TIME_HOUR_CMD, //2
    TIME_MIN_CMD, //3
    TURN_OFF_ALARM, //4

} LCD_TOUCH_CMD;

typedef struct
{
  LCD_TOUCH_CMD command;
  uint8_t data;
} TsLCDTouchCmd;

typedef enum {
  MSG_CURRENT_TIME, // TsDateTime
  MSG_ATHAN_TIMES,   // TsAthanTimesDay
  MSG_ATHAN_ALERT,  // TsAthanTime
  MSG_BUTTON_PRESS, // TsButtonPress
  MSG_DEVICE_RESET,
  MSG_LCD_CMD       // TsLCDTouchCmd
} athan_packet_type;

typedef struct
{
    uint8_t version;
    uint8_t sequenceNum;
    uint8_t dataLen;
    athan_packet_type dataType;
    uint8_t data[];
} TsAthanPacket;


void SendAthanPacket(athan_packet_type cmd, uint8_t* data, uint8_t dataLen);

#endif /* APPLICATION_ATHANTRANSPORT_H_ */
