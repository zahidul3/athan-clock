/******************************************************************************

 @file  mac_hl_patch.h

 @brief This file includes typedefs and functions required for high level mac patches.

 Group: CMCU, LPRF
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2015-2018, Texas Instruments Incorporated
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

#ifndef MAC_HL_PATCH_H
#define MAC_HL_PATCH_H

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                          Constants
 * ------------------------------------------------------------------------------------------------
 */
#define PHY_MODE_STD            (1)
#define PHY_MODE_GEN            (2)
#define PHY_MODE_FSK_50K        (1)
#define PHY_MODE_FSK_150K       (2)
#define PHY_MODE_FSK_200K       (3)
#define PHY_MODE_SLR_5K         (4)
#define PHY_MODE_UNDEF          (0)
/* ------------------------------------------------------------------------------------------------
 *                                          Macros
 * ------------------------------------------------------------------------------------------------
 */

/*--------------------------------------------------------------------------------------------------
*           Typedef
*---------------------------------------------------------------------------------------------------
*/
typedef struct AESCCM_Transaction {
    uint8 securityLevel;
    uint8 *pKey;
    uint8 *header;
    uint8 *data;
    uint8 *mic;
    uint8 *nonce;
    uint16 headerLength;
    uint16 dataLength;
    uint16 micLength;
} AESCCM_Transaction;
/*--------------------------------------------------------------------------------------------------
*           Functions
*---------------------------------------------------------------------------------------------------
*/

uint8 macCheckPhyRate(uint8 phyID);
uint8 macCheckPhyMode(uint8 phyID);
uint8 macPibIndex(uint8 pibAttribute);
uint8 macPibCheckByPatch(uint8 pibAttribute, void *pValue);
void macSetDefaultsByPatch(uint8 pibAttribute);
uint8 macCcmEncrypt(AESCCM_Transaction *trans);
uint8 macCcmDecrypt(AESCCM_Transaction *trans);
#endif /* MAC_HL_PATCH_H */

