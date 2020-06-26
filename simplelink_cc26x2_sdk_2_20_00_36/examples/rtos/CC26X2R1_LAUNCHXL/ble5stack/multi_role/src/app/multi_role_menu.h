/******************************************************************************

 @file  multi_role_menu.h

 @brief This file contains macros, type definitions, and function prototypes
        for two-button menu implementation.

 Group: WCS BTS
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2016-2018, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: simplelink_cc26x2_sdk_2_20_00_36
 Release Date: 2018-06-27 10:07:01
 *****************************************************************************/

#ifndef MULTI_ROLE_MENU_H
#define MULTI_ROLE_MENU_H

#ifdef __cplusplus
extern "C"
{
#endif

// Duplicate menu item indices.
#define MR_ITEM_NONE        TBM_ITEM_NONE
#define MR_ITEM_ALL         TBM_ITEM_ALL


#define MR_ITEM_STARTDISC      TBM_ITEM(0)  // "Discover Devices"
#define MR_ITEM_STOPDISC       TBM_ITEM(1)  // "Stop Discovering"
#define MR_ITEM_CONNECT        TBM_ITEM(2)  // "Connect To"
#define MR_ITEM_CANCELCONN     TBM_ITEM(3)  // "Cancel Connecting"
#define MR_ITEM_SELECTCONN     TBM_ITEM(4)  // "Work With"
#define MR_ITEM_ADVERTISE      TBM_ITEM(5)  // "Work With"

#define MR_ITEM_GATTREAD    TBM_ITEM(0)  // "GATT Read"
#define MR_ITEM_GATTWRITE   TBM_ITEM(1)  // "GATT Write"
#define MR_ITEM_UPDATE      TBM_ITEM(2)  // "Connection Update"
#define MR_ITEM_DISCONNECT  TBM_ITEM(3)  // "Disconnect"

/*
 * Menus Declarations
 */

/* Main Menu Object */
extern tbmMenuObj_t mrMenuMain;
extern tbmMenuObj_t mrMenuScanPhy;
extern tbmMenuObj_t mrMenuConnPhy;
extern tbmMenuObj_t mrMenuConnect;
extern tbmMenuObj_t mrMenuSelectConn;
extern tbmMenuObj_t mrMenuPerConn;
extern tbmMenuObj_t mrMenuGattWrite;
extern tbmMenuObj_t mrMenuAdvertise;

#ifdef __cplusplus
}
#endif

#endif /* MULTI_ROLE_MENU_H */

