/******************************************************************************

 @file  mac_autopend.h

 @brief This file contains the data structures and APIs for CC13xx
        RF Core Firmware Specification for IEEE 802.15.4.

 Group: CMCU, LPRF
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2009-2018, Texas Instruments Incorporated
 All rights reserved.

 IMPORTANT: Your use of this Software is limited to those specific rights
 granted under the terms of a software license agreement between the user
 who downloaded the software, his/her employer (which must be your employer)
 and Texas Instruments Incorporated (the "License"). You may not use this
 Software unless you agree to abide by the terms of the License. The License
 limits your use, and you acknowledge, that the Software may not be modified,
 copied or distributed unless embedded on a Texas Instruments microcontroller
 or used solely and exclusively in conjunction with a Texas Instruments radio
 frequency transceiver, which is integrated into your product. Other than for
 the foregoing purpose, you may not use, reproduce, copy, prepare derivative
 works of, modify, distribute, perform, display or sell this Software and/or
 its documentation for any purpose.

 YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
 PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
 NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
 TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
 NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
 LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
 INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
 OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
 OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
 (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

 Should you have any questions regarding your right to use this Software,
 contact Texas Instruments Incorporated at www.TI.com.

 ******************************************************************************
 Release Name: simplelink_cc26x2_sdk_2_20_00_36
 Release Date: 2018-06-27 10:07:01
 *****************************************************************************/

#ifndef MAC_AUTOPEND_H
#define MAC_AUTOPEND_H

/* ------------------------------------------------------------------------------------------------
 *                                      Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "mac_high_level.h"

/* ------------------------------------------------------------------------------------------------
 *                                      Defines
 * ------------------------------------------------------------------------------------------------
 */
#define MAC_SRCMATCH_INVALID_INDEX           0xFF

#define MAC_SRCMATCH_SHORT_ENTRY_SIZE        4
#define MAC_SRCMATCH_EXT_ENTRY_SIZE          8

#define MAC_SRCMATCH_SHORT_MAX_NUM_ENTRIES   5
#define MAC_SRCMATCH_EXT_MAX_NUM_ENTRIES     5


/* ------------------------------------------------------------------------------------------------
 *                                      Global Variables
 * ------------------------------------------------------------------------------------------------
 */
extern uint8 macSrcResIndex;

/* ------------------------------------------------------------------------------------------------
 *                                      Global Functions
 * ------------------------------------------------------------------------------------------------
 */
MAC_INTERNAL_API bool MAC_SrcMatchCheckResult(void);

#endif // MAC_AUTOPEND_H
