/******************************************************************************

@file  multi_role_menu.c

@brief This file contains the multi_role menu configuration for use
with the CC2650 Bluetooth Low Energy Protocol Stack.

Group: CMCU, LPRF
Target Device: CC2652

******************************************************************************

 Copyright (c) 2013-2018, Texas Instruments Incorporated
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

#include <bcomdef.h>
#include <ti/display/Display.h>

#include "board_key.h"

#include <menu/two_btn_menu.h>
#include "multi_role_menu.h"
#include "multi_role.h"

/*
 * Menu Lists Initializations
 */


// Menu: SelectDev
// upper: Main
// NOTE: The number of items in this menu object shall be
//       equal to or greater than MAX_NUM_BLE_CONNS
MENU_OBJ(mrMenuSelectConn, "Work with", MAX_NUM_BLE_CONNS, &mrMenuMain)
  MENU_ITEM_ACTION(NULL, multi_role_doSelectConn)
  MENU_ITEM_ACTION(NULL, multi_role_doSelectConn)
  MENU_ITEM_ACTION(NULL, multi_role_doSelectConn)
  MENU_ITEM_ACTION(NULL, multi_role_doSelectConn)
  MENU_ITEM_ACTION(NULL, multi_role_doSelectConn)
  MENU_ITEM_ACTION(NULL, multi_role_doSelectConn)
  MENU_ITEM_ACTION(NULL, multi_role_doSelectConn)
  MENU_ITEM_ACTION(NULL, multi_role_doSelectConn)
MENU_OBJ_END

// Menu: Main
MENU_OBJ(mrMenuMain, "Multi-Role", 6, NULL)
  MENU_ITEM_ACTION("Discover Devices", multi_role_doDiscoverDevices)
  MENU_ITEM_ACTION("Stop Discovering", multi_role_doStopDiscovering)
  MENU_ITEM_SUBMENU(&mrMenuConnect)
  MENU_ITEM_ACTION("Cancel Connecting",multi_role_doCancelConnecting)
  MENU_ITEM_SUBMENU(&mrMenuSelectConn)
  MENU_ITEM_ACTION("Advertise", multi_role_doAdvertise)
MENU_OBJ_END

// Menu: ScanPhy
// upper: Main
MENU_OBJ(mrMenuScanPhy, "Set Scanning PHY", 2, &mrMenuMain)
  MENU_ITEM_ACTION("1 Mbps", multi_role_doSetScanPhy)
  MENU_ITEM_ACTION("Coded",  multi_role_doSetScanPhy)
MENU_OBJ_END

// Menu: Connect
// upper: Main
// NOTE: The number of items in this menu object shall be equal to
//       or greater than DEFAULT_MAX_SCAN_RES.
//       The number of items cannot exceed 27 which is the two-button menu's
//       constraint.
MENU_OBJ(mrMenuConnect, "Connect", DEFAULT_MAX_SCAN_RES, &mrMenuMain)
  MENU_ITEM_ACTION(NULL, multi_role_doConnect)
  MENU_ITEM_ACTION(NULL, multi_role_doConnect)
  MENU_ITEM_ACTION(NULL, multi_role_doConnect)
  MENU_ITEM_ACTION(NULL, multi_role_doConnect)
  MENU_ITEM_ACTION(NULL, multi_role_doConnect)
  MENU_ITEM_ACTION(NULL, multi_role_doConnect)
  MENU_ITEM_ACTION(NULL, multi_role_doConnect)
  MENU_ITEM_ACTION(NULL, multi_role_doConnect)
  MENU_ITEM_ACTION(NULL, multi_role_doConnect)
  MENU_ITEM_ACTION(NULL, multi_role_doConnect)
  MENU_ITEM_ACTION(NULL, multi_role_doConnect)
  MENU_ITEM_ACTION(NULL, multi_role_doConnect)
  MENU_ITEM_ACTION(NULL, multi_role_doConnect)
  MENU_ITEM_ACTION(NULL, multi_role_doConnect)
  MENU_ITEM_ACTION(NULL, multi_role_doConnect)
MENU_OBJ_END

// Menu: PerConnection
// upper: SelectDevice
MENU_OBJ(mrMenuPerConn, NULL, 4, &mrMenuSelectConn)
  MENU_ITEM_ACTION("GATT Read",          multi_role_doGattRead)
  MENU_ITEM_SUBMENU(&mrMenuGattWrite)
  MENU_ITEM_ACTION("Connection Update",  multi_role_doConnUpdate)
  MENU_ITEM_ACTION("Disconnect",         multi_role_doDisconnect)
MENU_OBJ_END

// Menu: GattWrite
// upper: PerConnection
MENU_OBJ(mrMenuGattWrite, "GATT Write", 4, &mrMenuPerConn)
  MENU_ITEM_ACTION("Write 0x00", multi_role_doGattWrite)
  MENU_ITEM_ACTION("Write 0x55", multi_role_doGattWrite)
  MENU_ITEM_ACTION("Write 0xAA", multi_role_doGattWrite)
  MENU_ITEM_ACTION("Write 0xFF", multi_role_doGattWrite)
MENU_OBJ_END

// Menu: ConnPhy
// upper: Main
MENU_OBJ(mrMenuConnPhy, "Set Conn PHY Preference", 5, &mrMenuMain)
  MENU_ITEM_ACTION("1 Mbps",              multi_role_doSetConnPhy)
  MENU_ITEM_ACTION("2 Mbps",              multi_role_doSetConnPhy)
  MENU_ITEM_ACTION("1 & 2 Mbps",          multi_role_doSetConnPhy)
  MENU_ITEM_ACTION("Coded",               multi_role_doSetConnPhy)
  MENU_ITEM_ACTION("1 & 2 Mbps, & Coded", multi_role_doSetConnPhy)
MENU_OBJ_END
