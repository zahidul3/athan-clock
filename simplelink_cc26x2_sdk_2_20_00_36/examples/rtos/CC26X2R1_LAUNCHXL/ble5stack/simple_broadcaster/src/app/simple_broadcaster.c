/******************************************************************************

 @file  simple_broadcaster.c

 @brief This file contains the Simple Broadcaster sample application for
        use with the CC2650 Bluetooth Low Energy Protocol Stack.

 Group: CMCU, LPRF
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2011-2018, Texas Instruments Incorporated
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

/*********************************************************************
 * INCLUDES
 */

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/display/Display.h>

#include "util.h"
#include <icall.h>
/* This Header file contains all BLE API and icall structure definition */
#include <icall_ble_api.h>
#include <devinfoservice.h>

#ifdef USE_RCOSC
#include "rcosc_calibration.h"
#endif //USE_RCOSC

#include "board.h"
#include "simple_broadcaster.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// Address mode of the local device
// Note: When using the DEFAULT_ADDRESS_MODE as ADDRMODE_RANDOM or 
// ADDRMODE_RP_WITH_RANDOM_ID, GAP_DeviceInit() should be called with 
// it's last parameter set to a static random address
#define DEFAULT_ADDRESS_MODE                  ADDRMODE_PUBLIC

// Task configuration
#define SB_TASK_PRIORITY                     1

#ifndef SB_TASK_STACK_SIZE
#define SB_TASK_STACK_SIZE                   1024
#endif

#define SB_ADV_EVT                           1

// Internal Events for RTOS application
#define SB_ICALL_EVT                         ICALL_MSG_EVENT_ID // Event_Id_31
#define SB_QUEUE_EVT                         UTIL_QUEUE_EVENT_ID // Event_Id_30

#define SB_ALL_EVENTS                        (SB_ICALL_EVT | \
                                               SB_QUEUE_EVT)

// Spin if the expression is not true
#define SIMPLEBROADCASTER_ASSERT(expr) if (!(expr)) HAL_ASSERT_SPINLOCK;
/*********************************************************************
 * TYPEDEFS
 */

// App event passed from stack modules. This type is defined by the application
// since it can queue events to itself however it wants.
typedef struct
{
  uint8_t event;                // event type
  void    *pData;               // pointer to message
} sbEvt_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Display Interface
Display_Handle dispHandle = NULL;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Event globally used to post local events and pend on system and
// local events.
static ICall_SyncHandle syncEvent;

// Queue object used for app messages
static Queue_Struct appMsgQueue;
static Queue_Handle appMsgQueueHandle;

// Task configuration
Task_Struct sbTask;
#if defined __TI_COMPILER_VERSION__
#pragma DATA_ALIGN(sbTaskStack, 8)
#else
#pragma data_alignment=8
#endif
uint8_t sbTaskStack[SB_TASK_STACK_SIZE];

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8 scanRspData[] =
{
  // complete name
  0x15,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  'S',
  'i',
  'm',
  'p',
  'l',
  'e',
  'B',
  'L',
  'E',
  'B',
  'r',
  'o',
  'a',
  'd',
  'c',
  'a',
  's',
  't',
  'e',
  'r',

  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8 advertData[] =
{
  // Flags; this sets the device to use limited discoverable
  // mode (advertises for 30 seconds at a time) instead of general
  // discoverable mode (advertises indefinitely)
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

#ifndef BEACON_FEATURE

  // three-byte broadcast of the data "1 2 3"
  0x04,   // length of this data including the data type byte
  GAP_ADTYPE_MANUFACTURER_SPECIFIC, // manufacturer specific adv data type
  1,
  2,
  3

#else

  // 25 byte beacon advertisement data
  // Preamble: Company ID - 0x000D for TI, refer to https://www.bluetooth.org/en-us/specification/assigned-numbers/company-identifiers
  // Data type: Beacon (0x02)
  // Data length: 0x15
  // UUID: 00000000-0000-0000-0000-000000000000 (null beacon)
  // Major: 1 (0x0001)
  // Minor: 1 (0x0001)
  // Measured Power: -59 (0xc5)
  0x1A, // length of this data including the data type byte
  GAP_ADTYPE_MANUFACTURER_SPECIFIC, // manufacturer specific adv data type
  0x0D, // Company ID - Fixed
  0x00, // Company ID - Fixed
  0x02, // Data Type - Fixed
  0x15, // Data Length - Fixed
  0x00, // UUID - Variable based on different use cases/applications
  0x00, // UUID
  0x00, // UUID
  0x00, // UUID
  0x00, // UUID
  0x00, // UUID
  0x00, // UUID
  0x00, // UUID
  0x00, // UUID
  0x00, // UUID
  0x00, // UUID
  0x00, // UUID
  0x00, // UUID
  0x00, // UUID
  0x00, // UUID
  0x00, // UUID
  0x00, // Major
  0x01, // Major
  0x00, // Minor
  0x01, // Minor
  0xc5  // Power - The 2's complement of the calibrated Tx Power

#endif // !BEACON_FEATURE
};

// Container to store advertising event data when passing from advertising
// callback to app event. See the respective event in GapAdvScan_Event_IDs
// in gap_advertiser.h for the type that pBuf should be cast to.
typedef struct
{
  uint32_t event;
  void *pBuf;
} sbGapAdvEventData_t;

// Advertising handles
static uint8 advHandleLegacy;

// Address mode
static GAP_Addr_Modes_t addrMode = DEFAULT_ADDRESS_MODE;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void SimpleBroadcaster_init(void);
static void SimpleBroadcaster_taskFxn(UArg a0, UArg a1);

static void SimpleBroadcaster_processStackMsg(ICall_Hdr *pMsg);
static void SimpleBroadcaster_processGapMessage(gapEventHdr_t *pMsg);
static void SimpleBroadcaster_processAppMsg(sbEvt_t *pMsg);
static void SimpleBroadcaster_advCallback(uint32_t event, void *pBuf, uintptr_t arg);
static bool SimpleBroadcaster_processAdvEvent(sbGapAdvEventData_t *pEventData);
static void SimpleBroadcaster_enqueueMsg(uint8_t event, void *pData);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
/*
static gapRolesCBs_t simpleBroadcaster_BroadcasterCBs =
{
  SimpleBroadcaster_stateChangeCB   // Profile State Change Callbacks
};
*/
/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SimpleBroadcaster_createTask
 *
 * @brief   Task creation function for the Simple Broadcaster.
 *
 * @param   none
 *
 * @return  none
 */
void SimpleBroadcaster_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = sbTaskStack;
  taskParams.stackSize = SB_TASK_STACK_SIZE;
  taskParams.priority = SB_TASK_PRIORITY;

  Task_construct(&sbTask, SimpleBroadcaster_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      SimpleBroadcaster_init
 *
 * @brief   Initialization function for the Simple Broadcaster App
 *          Task. This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notification ...).
 *
 * @param   none
 *
 * @return  none
 */
static void SimpleBroadcaster_init(void)
{
	// ******************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntity, &syncEvent);

#ifdef USE_RCOSC
  RCOSC_enableCalibration();
#endif // USE_RCOSC

  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueueHandle = Util_constructQueue(&appMsgQueue);

  // Open LCD
  dispHandle = Display_open(Display_Type_ANY, NULL);

  // Register with GAP for HCI/Host messages. This is needed to receive HCI
  // events. For more information, see the HCI section in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-latest/
  GAP_RegisterForMsgs(selfEntity);

  //Initialize GAP layer for Peripheral role and register to receive GAP events
  GAP_DeviceInit(GAP_PROFILE_BROADCASTER, selfEntity, addrMode, NULL);

  Display_printf(dispHandle, 0, 0, "BLE Broadcaster");
}

/*********************************************************************
 * @fn      SimpleBroadcaster_processEvent
 *
 * @brief   Application task entry point for the Simple Broadcaster.
 *
 * @param   none
 *
 * @return  none
 */
static void SimpleBroadcaster_taskFxn(UArg a0, UArg a1)
{
  // Initialize application
  SimpleBroadcaster_init();

  // Application main loop
  for (;;)
  {
    // Get the ticks since startup
    uint32_t tickStart = Clock_getTicks();

    uint32_t events;

    events = Event_pend(syncEvent, Event_Id_NONE, SB_ALL_EVENTS,
                        ICALL_TIMEOUT_FOREVER);

    if (events)
    {
      ICall_EntityID dest;
      ICall_ServiceEnum src;
      ICall_HciExtEvt *pMsg = NULL;

      if (ICall_fetchServiceMsg(&src, &dest,
                                (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
      {
        if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
        {
          // Process inter-task message
          SimpleBroadcaster_processStackMsg((ICall_Hdr *)pMsg);
        }

        if (pMsg)
        {
          ICall_freeMsg(pMsg);
        }
      }

      // If RTOS queue is not empty, process app message.
      if (events & SB_QUEUE_EVT)
      {
        while (!Queue_empty(appMsgQueueHandle))
        {
          sbEvt_t *pMsg = (sbEvt_t *)Util_dequeueMsg(appMsgQueueHandle);
          if (pMsg)
          {
            // Process message.
            SimpleBroadcaster_processAppMsg(pMsg);

            // Free the space from the message.
            ICall_free(pMsg);
          }
        }
      }
    }
  }
}

/*********************************************************************
 * @fn      SimpleBroadcaster_processStackMsg
 *
 * @brief   Process an incoming stack message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void SimpleBroadcaster_processStackMsg(ICall_Hdr *pMsg)
{
  switch (pMsg->event)
  {
    case GAP_MSG_EVENT:
      SimpleBroadcaster_processGapMessage((gapEventHdr_t*) pMsg);
      break;

    default:
      // do nothing
      break;
  }
}

/*********************************************************************
 * @fn      SimpleBroadcaster_processGapMessage
 *
 * @brief   Process an incoming GAP event.
 *
 * @param   pMsg - message to process
 */
static void SimpleBroadcaster_processGapMessage(gapEventHdr_t *pMsg)
{
  switch(pMsg->opcode)
  {
    case GAP_DEVICE_INIT_DONE_EVENT:
    {
      bStatus_t status = FAILURE;

      gapDeviceInitDoneEvent_t *pPkt = (gapDeviceInitDoneEvent_t *)pMsg;

      if(pPkt->hdr.status == SUCCESS)
      {
        // Store the system ID
        uint8_t systemId[DEVINFO_SYSTEM_ID_LEN];

        // use 6 bytes of device address for 8 bytes of system ID value
        systemId[0] = pPkt->devAddr[0];
        systemId[1] = pPkt->devAddr[1];
        systemId[2] = pPkt->devAddr[2];

        // set middle bytes to zero
        systemId[4] = 0x00;
        systemId[3] = 0x00;

        // shift three bytes up
        systemId[7] = pPkt->devAddr[5];
        systemId[6] = pPkt->devAddr[4];
        systemId[5] = pPkt->devAddr[3];

        // Set Device Info Service Parameter
        DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);

        // Display device address
        Display_printf(dispHandle, 1, 0, "%s Addr: %s",
                       (addrMode <= ADDRMODE_RANDOM) ? "Dev" : "ID",
                       Util_convertBdAddr2Str(pPkt->devAddr));
        Display_printf(dispHandle, 2, 0, "Initialized");

        // Setup and start Advertising
        // For more information, see the GAP section in the User's Guide:
        // http://software-dl.ti.com/lprf/ble5stack-latest/

        // Temporary memory for advertising parameters. These will be copied
        // by the GapAdv module
        GapAdv_params_t advParamLegacy = GAPADV_PARAMS_LEGACY_SCANN_CONN;

        #ifndef BEACON_FEATURE
          advParamLegacy.eventProps = GAP_ADV_PROP_SCANNABLE | GAP_ADV_PROP_LEGACY;
        #else
          advParamLegacy.eventProps = GAP_ADV_PROP_LEGACY;
        #endif // !BEACON_FEATURE

        // Create Advertisement set
        status = GapAdv_create(&SimpleBroadcaster_advCallback, &advParamLegacy,
                               &advHandleLegacy);
        SIMPLEBROADCASTER_ASSERT(status == SUCCESS);

        // Load advertising data
        status = GapAdv_loadByHandle(advHandleLegacy, GAP_ADV_DATA_TYPE_ADV,
                                     sizeof(advertData), advertData);
        SIMPLEBROADCASTER_ASSERT(status == SUCCESS);

        // Load scan response data
        status = GapAdv_loadByHandle(advHandleLegacy, GAP_ADV_DATA_TYPE_SCAN_RSP,
                                     sizeof(scanRspData), scanRspData);
        SIMPLEBROADCASTER_ASSERT(status == SUCCESS);

        // Set event mask
        status = GapAdv_setEventMask(advHandleLegacy,
                                     GAP_ADV_EVT_MASK_START_AFTER_ENABLE |
                                     GAP_ADV_EVT_MASK_END_AFTER_DISABLE |
                                     GAP_ADV_EVT_MASK_SET_TERMINATED);
        SIMPLEBROADCASTER_ASSERT(status == SUCCESS);

        // Enable legacy advertising
        status = GapAdv_enable(advHandleLegacy, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
        SIMPLEBROADCASTER_ASSERT(status == SUCCESS);
      }

      break;
    }

    default:
      Display_clearLine(dispHandle, 2);
      break;
  }
}

/*********************************************************************
 * @fn      SimpleBroadcaster_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   state - message state.
 */
static void SimpleBroadcaster_enqueueMsg(uint8_t event, void *pData)
{
  sbEvt_t *pMsg = ICall_malloc(sizeof(sbEvt_t));

  // Create dynamic pointer to message.
  if(pMsg)
  {
    pMsg->event = event;
    pMsg->pData = pData;

    // Enqueue the message.
    Util_enqueueMsg(appMsgQueueHandle, syncEvent, (uint8_t *)pMsg);
  }
}

/*********************************************************************
 * @fn      SimpleBroadcaster_advCallback
 *
 * @brief   GapAdv module callback
 *
 * @param   pMsg - message to process
 */
static void SimpleBroadcaster_advCallback(uint32_t event, void *pBuf, uintptr_t arg)
{
  sbGapAdvEventData_t *pData = ICall_malloc(sizeof(sbGapAdvEventData_t));

  if (pData)
  {
    pData->event = event;
    pData->pBuf = pBuf;

    SimpleBroadcaster_enqueueMsg(SB_ADV_EVT, pData);
  }
}

/*********************************************************************
 * @fn      SimpleBroadcaster_processAppMsg
 *
 * @brief   Process an incoming callback from a profile.
 *
 * @param   pMsg - message to process
 *
 * @return  None.
 */
static void SimpleBroadcaster_processAppMsg(sbEvt_t *pMsg)
{
  bool safeToDealloc = TRUE;

  switch (pMsg->event)
  {
    case SB_ADV_EVT:
      safeToDealloc = SimpleBroadcaster_processAdvEvent((sbGapAdvEventData_t*)(pMsg->pData));
      break;

    default:
      // Do nothing.
      break;
  }

  if (safeToDealloc)
  {
    // Free message data
    if (pMsg->pData)
    {
      ICall_free(pMsg->pData);
    }
  }
}

/*********************************************************************
 * @fn      SimpleBroadcaster_processAdvEvent
 *
 * @brief   Process advertising event in app context
 *
 * @param   pEventData
 */
static bool SimpleBroadcaster_processAdvEvent(sbGapAdvEventData_t *pEventData)
{
  bool safeToDealloc = TRUE;

  switch (pEventData->event)
  {
    case GAP_EVT_ADV_START_AFTER_ENABLE:
      Display_printf(dispHandle, 2, 0, "Advertising");
      break;

    case GAP_EVT_INSUFFICIENT_MEMORY:
      safeToDealloc = FALSE;
      break;

    default:
      // Do nothing.
      break;
  }

  return(safeToDealloc);
}

/*********************************************************************
*********************************************************************/
