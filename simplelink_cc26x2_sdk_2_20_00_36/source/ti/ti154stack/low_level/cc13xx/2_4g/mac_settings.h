/******************************************************************************

 @file  mac_settings.h

 @brief Describe the purpose and contents of the file.

 Group: CMCU, LPRF
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2006-2018, Texas Instruments Incorporated
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
#ifndef INCLUDE_GUARD
#define INCLUDE_GUARD

#include <driverlib/rf_mailbox.h>
#ifdef DeviceFamily_CC13X0
#include <driverlib/rf_common_cmd.h>
#include <driverlib/../../cc26x0/driverlib/rf_ieee_mailbox.h>
#include <driverlib/../../cc26x0/driverlib/rf_ieee_cmd.h>
#else
#include <driverlib/rf_common_cmd.h>
#include <driverlib/rf_ieee_mailbox.h>
#include <driverlib/rf_ieee_cmd.h>
#endif
/* ------------------------------------------------------------------------------------------------
 *                                            Defines
 * ------------------------------------------------------------------------------------------------
 */
//To be removed when solution to RFCORE49 is available
#ifndef FREQ_2_4G
#define RFCORE49
#else
#if defined(CC26X2R1_LAUNCHXL) || defined(CC13X2R1_LAUNCHXL)
#define RFCORE98
#endif
#endif
/* ------------------------------------------------------------------------------------------------
 *                                           Constants
 * ------------------------------------------------------------------------------------------------
 */
// RF Core variables
extern uint32_t pOverrides[];

// RF Core API commands
extern rfc_CMD_RADIO_SETUP_t RF_cmdRadioSetup;
extern rfc_CMD_NOP_t RF_cmdNop;
extern rfc_CMD_IEEE_TX_t RF_cmdIEEETx;
extern rfc_CMD_IEEE_RX_t RF_cmdIEEERx;
extern rfc_CMD_IEEE_CSMA_t RF_cmdIEEECsma;
extern rfc_CMD_SCH_IMM_t RF_cmdScheduleImmediate;
extern rfc_CMD_SET_RAT_CMP_t RF_cmdRat;
extern rfc_CMD_SET_RAT_CMP_t RF_cmdRat1;
#endif // INCLUDE_GUARD
