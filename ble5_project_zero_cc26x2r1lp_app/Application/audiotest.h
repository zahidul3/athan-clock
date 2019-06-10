/*
 * audiotest.h
 *
 *  Created on: Sep 1, 2018
 *      Author: Nida
 */

#ifndef AUDIOTEST_H_
#define AUDIOTEST_H_

/*
 * 1-3V3    PWR     21-5V           40-DIO7-LED PWM     20-GND
 * 2-DIO23  x       22-GND          39-DIO6             19-DIO12
 * 3-DIO2   x       23-DIO25        38-DIO20-SPI0_CLK SD   18-DIO11
 * 4-DIO3   x       24-DIO26        37-DIO19-SPI0_MOSI DI  17-
 * 5-DIO22          25-DIO27        36-DIO18            16-LPRST
 * 6-DIO24          26-DIO28        35-BPRST            15-DIO9-SPI1_MOSI-LCDSDI
 * 7-DIO10-SPI1_CLK,lcdscl 27-DIO29        34-TMS       14-DIO8- SD DO
 * 8-DIO21-LCD SDC,SDCS  28-DIO30        33-TCK              13-DIO13 AMPON,LCD_SCS(BTN1)
 * 9-DIO4-SCL             29-DIO0         32-DIO16-LCDRST     12-DIO14-Board_DAC_SYNC(BTN2)
 * 10-DIO5-SDA          30-DIO1         31-DIO17            11-DIO15
 */

/*
 * Task creation function for the Project Zero.
 */
extern void ProjectAzan_createTask(void);
extern void SetPlaybackAzanEvent(void);
extern void SetRecordAzanEvent(void);

/** @brief Synchronization object data type */
typedef void *SyncHandle;

#endif /* AUDIOTEST_H_ */
