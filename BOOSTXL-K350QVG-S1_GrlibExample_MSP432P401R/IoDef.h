/*
 * IoDef.h
 *
 *  Created on: Dec 12, 2019
 *      Author: Jahid's Desktop
 */

/* TODO:
 * Fix Daylight Savings
 * add stop during alarm
 * digitally controlled volume
 * separate the app code from example
 */

/* (X,Y) coordinates of LCD Alarm Clock
 * (2648,12197) (2403,3551)
 *  -------------
    |Athan Times|
    |           |
    |           |
    |           |
    |           |
    |           |
    |           |
    |           |
    -------------
    (13449,13693) (12534,3742)
 */
#ifndef IODEF_H_
#define IODEF_H_

#define CC_INT_PORT         GPIO_PORT_P6
#define CC_INT_PIN          GPIO_PIN0

#define HEARTBEAT_PORT      GPIO_PORT_P1
#define HEARTBEAT_PIN       GPIO_PIN0

#define BUTTON1_PORT        GPIO_PORT_P1
#define BUTTON1_PIN         GPIO_PIN1

#endif /* IODEF_H_ */

