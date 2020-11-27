/*
 * fatsd_test.h
 *
 *  Created on: Nov 21, 2020
 *      Author: zahidhaq
 */

#ifndef APPLICATION_FATSD_TEST_H_
#define APPLICATION_FATSD_TEST_H_

#define CONFIG_SDFatFS_0                0
#define CONFIG_TI_DRIVERS_SD_COUNT      1

void FatSD_Init(void);
void FatSD_Close(void);
#endif /* APPLICATION_FATSD_TEST_H_ */
