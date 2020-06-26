#include <bcomdef.h>
#include <ti/display/Display.h>
#include <menu/two_btn_menu.h>
#include "simple_central_menu.h"
#include "simple_central.h"

/*
 * Menu Lists Initializations
 */

// Menu: Main
// upper: none
MENU_OBJ(scMenuMain, "Simple Central", 6, NULL)
  MENU_ITEM_SUBMENU(&scMenuScanPhy)
  MENU_ITEM_ACTION("Discover Devices",  SimpleCentral_doDiscoverDevices)
  MENU_ITEM_ACTION("Stop Discovering",  SimpleCentral_doStopDiscovering)
  MENU_ITEM_SUBMENU(&scMenuConnect)
  MENU_ITEM_ACTION("Cancel Connecting", SimpleCentral_doCancelConnecting)
  MENU_ITEM_SUBMENU(&scMenuSelectConn)
MENU_OBJ_END

// Menu: ScanPhy
// upper: Main
MENU_OBJ(scMenuScanPhy, "Set Scanning PHY", 2, &scMenuMain)
  MENU_ITEM_ACTION("1 Mbps", SimpleCentral_doSetScanPhy)
  MENU_ITEM_ACTION("Coded",  SimpleCentral_doSetScanPhy)
MENU_OBJ_END

// Menu: Connect
// upper: Main
// NOTE: The number of items in this menu object shall be equal to
//       or greater than DEFAULT_MAX_SCAN_RES.
//       The number of items cannot exceed 27 which is the two-button menu's
//       constraint.
MENU_OBJ(scMenuConnect, "Connect to", DEFAULT_MAX_SCAN_RES, &scMenuMain)
  MENU_ITEM_ACTION(NULL, SimpleCentral_doConnect)
  MENU_ITEM_ACTION(NULL, SimpleCentral_doConnect)
  MENU_ITEM_ACTION(NULL, SimpleCentral_doConnect)
  MENU_ITEM_ACTION(NULL, SimpleCentral_doConnect)
  MENU_ITEM_ACTION(NULL, SimpleCentral_doConnect)
  MENU_ITEM_ACTION(NULL, SimpleCentral_doConnect)
  MENU_ITEM_ACTION(NULL, SimpleCentral_doConnect)
  MENU_ITEM_ACTION(NULL, SimpleCentral_doConnect)
MENU_OBJ_END

// Menu: SelectDev
// upper: Main
// NOTE: The number of items in this menu object shall be
//       equal to or greater than MAX_NUM_BLE_CONNS
MENU_OBJ(scMenuSelectConn, "Work with", MAX_NUM_BLE_CONNS, &scMenuMain)
  MENU_ITEM_ACTION(NULL, SimpleCentral_doSelectConn)
  MENU_ITEM_ACTION(NULL, SimpleCentral_doSelectConn)
  MENU_ITEM_ACTION(NULL, SimpleCentral_doSelectConn)
  MENU_ITEM_ACTION(NULL, SimpleCentral_doSelectConn)
MENU_OBJ_END

// Menu: PerConnection
// upper: SelectDevice
MENU_OBJ(scMenuPerConn, NULL, 7, &scMenuSelectConn)
  MENU_ITEM_ACTION("GATT Read",          SimpleCentral_doGattRead)
  MENU_ITEM_SUBMENU(&scMenuGattWrite)
  MENU_ITEM_ACTION("Start RSSI Reading", SimpleCentral_doRssiRead)
  MENU_ITEM_ACTION("Stop RSSI Reading",  SimpleCentral_doRssiRead)
  MENU_ITEM_ACTION("Connection Update",  SimpleCentral_doConnUpdate)
  MENU_ITEM_SUBMENU(&scMenuConnPhy)
  MENU_ITEM_ACTION("Disconnect",         SimpleCentral_doDisconnect)
MENU_OBJ_END

// Menu: GattWrite
// upper: PerConnection
MENU_OBJ(scMenuGattWrite, "GATT Write", 4, &scMenuPerConn)
  MENU_ITEM_ACTION("Write 0x00", SimpleCentral_doGattWrite)
  MENU_ITEM_ACTION("Write 0x55", SimpleCentral_doGattWrite)
  MENU_ITEM_ACTION("Write 0xAA", SimpleCentral_doGattWrite)
  MENU_ITEM_ACTION("Write 0xFF", SimpleCentral_doGattWrite)
MENU_OBJ_END

// Menu: ConnPhy
// upper: Main
MENU_OBJ(scMenuConnPhy, "Set Conn PHY Preference", 5, &scMenuMain)
  MENU_ITEM_ACTION("1 Mbps",              SimpleCentral_doSetConnPhy)
  MENU_ITEM_ACTION("2 Mbps",              SimpleCentral_doSetConnPhy)
  MENU_ITEM_ACTION("1 & 2 Mbps",          SimpleCentral_doSetConnPhy)
  MENU_ITEM_ACTION("Coded",               SimpleCentral_doSetConnPhy)
  MENU_ITEM_ACTION("1 & 2 Mbps, & Coded", SimpleCentral_doSetConnPhy)
MENU_OBJ_END
