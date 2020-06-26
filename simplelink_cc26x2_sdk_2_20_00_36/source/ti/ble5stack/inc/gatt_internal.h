/******************************************************************************

 @file  gatt_internal.h

 @brief This file contains internal interfaces for the Generic
        Attribute Profile (GATT) module.

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

/*********************************************************************
 *
 * WARNING!!!
 *
 * THE API'S FOUND IN THIS FILE ARE FOR INTERNAL STACK USE ONLY!
 * FUNCTIONS SHOULD NOT BE CALLED DIRECTLY FROM APPLICATIONS, AND ANY
 * CALLS TO THESE FUNCTIONS FROM OUTSIDE OF THE STACK MAY RESULT IN
 * UNEXPECTED BEHAVIOR
 *
 */


#ifndef GATT_INTERNAL_H
#define GATT_INTERNAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "osal_cbtimer.h"

#include "att.h"
#include "gatt.h"
#include "gattservapp.h"

/*********************************************************************
 * MACROS
 */
#define TIMER_VALID( id )                ( ( (id) != INVALID_TIMER_ID ) && \
                                           ( (id) != TIMEOUT_TIMER_ID ) )

#define TIMER_STATUS( id )               ( (id) == TIMEOUT_TIMER_ID ? bleTimeout : \
                                           (id) == INVALID_TIMER_ID ? SUCCESS : blePending )

/*********************************************************************
 * CONSTANTS
 */

// ATT Flow Control Violated bit (bit 7)
#define ATT_FCV_BIT                      0x80

/*********************************************************************
 * TYPEDEFS
 */
// Srtucture for Attribute Version Information attribute
typedef struct
{
  uint8 attVersion;        // Attribute Protocol Version
  uint8 gattVersion;       // Generic Attribute Profile Version
  uint16 manufacturerName; // Manufacturer Name
} gattVersionInfo_t;

// Function prototype to parse an attribute protocol request message
typedef bStatus_t (*gattParseReq_t)( uint8 sig, uint8 cmd, uint8 *pParams, uint16 len, attMsg_t *pMsg );

// Function prototype to parse an attribute protocol response message
typedef bStatus_t (*gattParseRsp_t)( uint8 *pParams, uint16 len, attMsg_t *pMsg );

// Function prototype to process an attribute protocol message
typedef bStatus_t (*gattProcessMsg_t)( uint16 connHandle,  attPacket_t *pPkt, uint8 *pSafeToDealloc );

// Function prototype to process an attribute protocol request message
typedef bStatus_t (*gattProcessReq_t)( uint16 connHandle,  attMsg_t *pMsg );

// Structure to keep Client info
typedef struct
{
  // Info maintained for Client that expecting a response back
  uint16 connHandle;           // connection message was sent out
  uint8 method;                // type of response to be received
  gattParseRsp_t pfnParseRsp;  // function to parse response to be received
  uint8 timerId;               // response timeout timer id
  uint8 taskId;                // task to be notified of response

  uint8 pendingInd;            // pending indication sent to app, and flow
                               // control violated bit (bit 7)
  // GATT Request message
  gattMsg_t req;               // request message

  // Info maintained for GATT Response message
  uint8 numRsps;               // number of responses received

  uint8 nextTransactionTaskId; // taskID of stack layer waiting to send a request.
} gattClientInfo_t;

// Service record list item
typedef struct _attAttrList
{
  struct _attAttrList *next;  // pointer to next service record
  gattService_t service;      // service record
} gattServiceList_t;

// Structure to keep Attribute Server info
typedef struct
{
  // Info maintained for Handle Value Confirmation message
  uint16 connHandle;    // connection message was sent on
  uint8 timerId;        // confirmation timeout timer id
  uint8 taskId;         // task to be notified of confirmation

  uint8 pendingReq;     // pending request sent to app, and flow control
                        // violated bit (bit 7)
} gattServerInfo_t;

/*********************************************************************
 * VARIABLES
 */

extern uint8 gattTaskID;

/*********************************************************************
 * GATT Task Functions
 */

extern bStatus_t gattNotifyEvent( uint8 taskId, uint16 connHandle, uint8 status, uint8 method, gattMsg_t *pMsg );

extern void gattSendFlowCtrlEvt( uint16 connHandle, uint8 opcode, uint8 pendingOpcode );

extern void gattStartTimer( pfnCbTimer_t pfnCbTimer, uint8 *pData, uint16 timeout, uint8 *pTimerId );

extern void gattStopTimer( uint8 *pTimerId );

extern uint8 *gattGetPayload( gattMsg_t *pMsg, uint8 opcode );

extern void gattProcessOSALMsg( osal_event_hdr_t *pMsg );

extern void gattProcessRxData( l2capDataEvent_t *pL2capMsg );

/*********************************************************************
 * GATT Client Functions
 */

extern void gattRegisterClient( gattProcessMsg_t pfnProcessMsg );

extern uint8 gattProcessMultiReqs( uint16 connHandle, gattClientInfo_t *pClient, uint8 method, gattMsg_t *pMsg );

extern uint8 gattProcessFindInfo( gattClientInfo_t *pClient, uint8 method, gattMsg_t *pMsg );

extern uint8 gattProcessFindByTypeValue( gattClientInfo_t *pClient, uint8 method, gattMsg_t *pMsg );

extern uint8 gattProcessReadByType( gattClientInfo_t *pClient, uint8 method, gattMsg_t *pMsg );

extern uint8 gattProcessReadLong( gattClientInfo_t *pClient, uint8 method, gattMsg_t *pMsg );

extern uint8 gattProcessReadByGrpType( gattClientInfo_t *pClient, uint8 method, gattMsg_t *pMsg );

extern bStatus_t gattProcessWriteLong( gattClientInfo_t *pClient, uint8 method, gattMsg_t *pMsg );

extern bStatus_t gattProcessReliableWrites( gattClientInfo_t *pClient, uint8 method, gattMsg_t *pMsg );

extern void gattStoreClientInfo( gattClientInfo_t *pClient, gattMsg_t *pReq, uint8 method, gattParseRsp_t pfnParseRsp, uint8 taskId );

extern gattClientInfo_t *gattFindClientInfo( uint16 connHandle );

extern bStatus_t gattGetClientStatus( uint16 connHandle, gattClientInfo_t **p2pClient );

extern void gattResetClientInfo( gattClientInfo_t *pClient );

extern void gattClientStartTimer( uint8 *pData, uint16 timeout, uint8 *pTimerId );

extern bStatus_t gattFindInfo( uint16 connHandle, attFindInfoReq_t *pReq, uint8 taskId );

extern bStatus_t gattFindByTypeValue( uint16 connHandle, gattFindByTypeValueReq_t *pReq );

extern bStatus_t gattReadByType( uint16 connHandle, attReadByTypeReq_t *pReq, uint8 discByCharUUID, uint8 taskId );

extern bStatus_t gattRead( uint16 connHandle, attReadReq_t *pReq, uint8 taskId );

extern bStatus_t gattReadLong( uint16 connHandle, attReadBlobReq_t *pReq, uint8 taskId );

extern bStatus_t gattReadByGrpType( uint16 connHandle, attReadByGrpTypeReq_t *pReq, uint8 taskId );

extern bStatus_t gattWrite( uint16 connHandle, attWriteReq_t *pReq, uint8 taskId );

extern bStatus_t gattWriteLong( uint16 connHandle, attPrepareWriteReq_t *pReq, uint8 taskId );

extern bStatus_t gattPrepareWriteReq( uint16 connHandle, uint16 handle, uint16 offset, uint16 len, uint8 *pValue );

extern bStatus_t gattClientProcessMsgCB( uint16 connHandle, attPacket_t *pPkt, uint8 *pSafeToDealloc );

extern void gattClientNotifyTxCB( uint16 connHandle, uint8 opcode );

extern void gattClientHandleTimerCB( uint8 *pData );

extern void gattClientHandleConnStatusCB( uint16 connectionHandle, uint8 changeType );

/*********************************************************************
 * GATT Client Functions
 */

extern void gattRegisterServer( gattProcessMsg_t pfnProcessMsg );

extern bStatus_t gattProcessExchangeMTUReq( uint16 connHandle, attMsg_t *pMsg );

extern bStatus_t gattProcessFindInfoReq( uint16 connHandle, attMsg_t *pMsg );

extern bStatus_t gattProcessFindByTypeValueReq( uint16 connHandle, attMsg_t *pMsg );

extern bStatus_t gattProcessReadByTypeReq( uint16 connHandle, attMsg_t *pMsg );

extern bStatus_t gattProcessReadReq( uint16 connHandle, attMsg_t *pMsg );

extern bStatus_t gattProcessReadMultiReq( uint16 connHandle, attMsg_t *pMsg );

extern bStatus_t gattProcessReadByGrpTypeReq( uint16 connHandle, attMsg_t *pMsg );

extern bStatus_t gattProcessWriteReq( uint16 connHandle, attMsg_t *pMsg );

extern bStatus_t gattProcessExecuteWriteReq( uint16 connHandle, attMsg_t *pMsg );

extern gattParseReq_t gattParseReq( uint8 method );

extern gattProcessReq_t gattProcessReq( uint8 method );

extern gattService_t *gattFindService( uint16 handle );

extern void gattStoreServerInfo( gattServerInfo_t *pServer, uint8 taskId );

extern bStatus_t gattGetServerStatus( uint16 connHandle, gattServerInfo_t **p2pServer );

extern gattServerInfo_t *gattFindServerInfo( uint16 connHandle );

extern void gattResetServerInfo( gattServerInfo_t *pServer );

extern void gattServerStartTimer( uint8 *pData, uint16 timeout, uint8 *pTimerId );

extern uint16 gattServiceLastHandle( uint16 handle );

extern bStatus_t gattServerProcessMsgCB( uint16 connHandle, attPacket_t *pPkt, uint8 *pSafeToDealloc );

extern void gattServerNotifyTxCB( uint16 connHandle, uint8 opcode );

extern void gattServerHandleTimerCB( uint8 *pData );

extern void gattServerHandleConnStatusCB( uint16 connectionHandle, uint8 changeType );

// Due to being referenced from ROM:

extern bStatus_t               gattServApp_ProcessExchangeMTUReq( gattMsgEvent_t *pMsg );
extern bStatus_t               gattServApp_ProcessReadByTypeReq( gattMsgEvent_t *pMsg, bStatus_t status, uint16 *pErrHandle );
extern bStatus_t               gattServApp_ProcessPrepareWriteReq( gattMsgEvent_t *pMsg, uint16 *pErrHandle, uint8 *pSafeToDealloc );
extern bStatus_t               gattServApp_ProcessExecuteWriteReq( gattMsgEvent_t *pMsg, uint16 *pErrHandle );
extern bStatus_t               gattServApp_ProcessReadBlobReq( gattMsgEvent_t *pMsg, uint16 *pErrHandle );
extern bStatus_t               gattServApp_ProcessReadByGrpTypeReq( gattMsgEvent_t *pMsg, uint16 *pErrHandle );
extern bStatus_t               gattServApp_ProcessReadMultiReq( gattMsgEvent_t *pMsg, uint16 *pErrHandle );
extern bStatus_t               gattServApp_ProcessReadReq( gattMsgEvent_t *pMsg, uint16 *pErrHandle );
extern bStatus_t               gattServApp_ProcessWriteReq( gattMsgEvent_t *pMsg, uint16 *pErrHandle, uint8 *pSafeToDealloc );
extern bStatus_t               gattServApp_ProcessFindByTypeValueReq( gattMsgEvent_t *pMsg, uint16 *pErrHandle );
extern uint8                   gattServApp_PrepareWriteQInUse( void );
extern CONST gattServiceCBs_t *gattServApp_FindServiceCBs( uint16 service );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* GATT_INTERNAL_H */
