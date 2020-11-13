/*
 * athan_app.h
 *
 *  Created on: Oct 28, 2020
 *      Author: zahidhaq
 */

#ifndef APPLICATION_ATHAN_APP_H_
#define APPLICATION_ATHAN_APP_H_

void IncDateTime(uint8 updateStats);
void AthanApp_init(void);
void PrintCurrentDateTime(void);
void SendAthanTimesToLCD(void);
void SendCurrentDateTimeToLCD(void);
void sendTestDateTimeToLCD(void);
void SendAthanTimes(void);
void EnableSleepAfterDelay(uint8_t delayInSeconds);
void HandleRxMessage(TsAthanPacket* athanPacket);
void UpdateCurrentTimeFromIOS(TsIOSDateTime* pDateTimeIOS);
#endif /* APPLICATION_ATHAN_APP_H_ */
