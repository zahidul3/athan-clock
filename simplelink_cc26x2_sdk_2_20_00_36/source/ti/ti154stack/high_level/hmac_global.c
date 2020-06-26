/******************************************************************************

 @file  hmac_global.c

 @brief This file contains the externs for FH ROM API initialization.

 Group: CMCU, LPRF
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2017-2018, Texas Instruments Incorporated
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

/*******************************************************************************
 * INCLUDES
 */

#include <stddef.h>
#include "mac_api.h"
#include "mac_main.h"
#include "hal_types.h"
#include "mac_coord.h"
#include "mac_data.h"
#include "mac_device.h"
#include "mac_beacon.h"
#include "mac_beacon_device.h"
#include "mac_beacon_coord.h"
#include "mac_mgmt.h"
#include "mac_pib.h"
#include "mac_scan.h"
#include "mac_spec.h"
#include "mac_radio.h"
#include "mac_security_pib.h"
#include <crypto_mac_api.h>
#include "mac_pwr.h"
#include "mac_timer.h"

//#ifdef FEATURE_BEACON_MODE
/* Coordinator action set 1 */
macAction_t macBeaconCoordAction1[2];

macBeaconCoord_t macBeaconCoord;
//#endif

//#if defined (FEATURE_BEACON_MODE) || defined (FEATURE_NON_BEACON_MODE)
/* Device beacon action function table */
macAction_t macBeaconDeviceAction[4];

macBeaconDevice_t macBeaconDevice;
//#endif

//#if defined (FEATURE_BEACON_MODE) || defined (FEATURE_NON_BEACON_MODE)
#if 1
uint16 macBeaconMargin[] =
{
  96,
  96,
  96,
  96,
  96,
  96,
  96,
  96,
  96,
  160,
  220,
  380,
  700,
  1324,
  2588,
  0
};

/* mac_beacon data */
macBeacon_t macBeacon;

#endif

/* Coordinator action set 1 */
macAction_t macCoordAction1[2];

/* Coordinator action set 2 */
macAction_t macCoordAction2[7];

/* Coordinator action set 3 */
macAction_t macCoordAction3[4];

/* Coordinator info */
macCoord_t  macCoord;

/* Table of which IE's to be included for the specific command indexed by command identifier */
const uint32 macCmdIncludeFHIe[] =
{
  0,                                             /* MAC_INTERNAL_BEACON */
  MAC_FH_UT_IE,                                  /* MAC_INTERNAL_ZERO_DATA */
  MAC_FH_UT_IE,                                  /* MAC_INTERNAL_ASSOC_REQ */
  MAC_FH_UT_IE + MAC_FH_BT_IE,                   /* MAC_INTERNAL_ASSOC_RSP */
  MAC_FH_UT_IE,                                  /* MAC_INTERNAL_DISASSOC_NOTIF */
  MAC_FH_UT_IE + MAC_FH_US_IE,                   /* MAC_INTERNAL_DATA_REQ */
  0,                                             /* MAC_INTERNAL_PAN_CONFLICT */
  0,                                             /* MAC_INTERNAL_ORPHAN_NOTIF */
  0,                                             /* MAC_INTERNAL_BEACON_REQ */
  0,                                             /* MAC_INTERNAL_COORD_REALIGN */
  0,                                             /* MAC_INTERNAL_GTS_REQ */
  0,                                             /* MAC_INTERNAL_COEXIST_EB_REQ */
  0,                                             /* MAC_INTERNAL_ENHANCED_BEACON_REQ */
  MAC_FH_UT_IE + MAC_FH_RSL_IE + MAC_FH_BT_IE    /* MAC_INTERNAL_ENH_ACK */
};

/* Table of command frame sizes indexed by command identifier */
const uint8 macCmdBufSize[] =
{
  0,                                                                    /* MAC_INTERNAL_BEACON */
  MAC_ZERO_DATA_FRAME_LEN + MAC_TX_OFFSET_LEN + sizeof(macTx_t),        /* MAC_INTERNAL_ZERO_DATA */
  MAC_ASSOC_REQ_FRAME_LEN + MAC_TX_OFFSET_LEN + sizeof(macTx_t),        /* MAC_INTERNAL_ASSOC_REQ */
  MAC_ASSOC_RSP_FRAME_LEN + MAC_TX_OFFSET_LEN + sizeof(macTx_t),        /* MAC_INTERNAL_ASSOC_RSP */
  MAC_DISASSOC_NOTIF_FRAME_LEN + MAC_TX_OFFSET_LEN + sizeof(macTx_t),   /* MAC_INTERNAL_DISASSOC_NOTIF */
  MAC_DATA_REQ_FRAME_LEN + MAC_TX_OFFSET_LEN + sizeof(macTx_t),         /* MAC_INTERNAL_DATA_REQ */
  MAC_PAN_CONFLICT_FRAME_LEN + MAC_TX_OFFSET_LEN + sizeof(macTx_t),     /* MAC_INTERNAL_PAN_CONFLICT */
  MAC_ORPHAN_NOTIF_FRAME_LEN + MAC_TX_OFFSET_LEN + sizeof(macTx_t),     /* MAC_INTERNAL_ORPHAN_NOTIF */
  MAC_BEACON_REQ_FRAME_LEN + MAC_TX_OFFSET_LEN + sizeof(macTx_t),       /* MAC_INTERNAL_BEACON_REQ */
  MAC_COORD_REALIGN_FRAME_LEN + MAC_TX_OFFSET_LEN + sizeof(macTx_t),    /* MAC_INTERNAL_COORD_REALIGN */
  MAC_GTS_REQ_FRAME_LEN + MAC_TX_OFFSET_LEN + sizeof(macTx_t),          /* MAC_INTERNAL_GTS_REQ */
  MAC_COEXIST_EB_REQ_FRAME_LEN + MAC_TX_OFFSET_LEN + sizeof(macTx_t),      /* MAC_INTERNAL_COEXIST_EB_REQ */
  MAC_ENHANCED_BEACON_REQ_FRAME_LEN + MAC_TX_OFFSET_LEN + sizeof(macTx_t), /* MAC_INTERNAL_ENHANCED_BEACON_REQ */
  MAC_ENH_ACK_FRAME_LEN + MAC_TX_OFFSET_LEN + sizeof(macTx_t)           /* MAC_INTERNAL_ENH_ACK */
};

/* Table of MAC command payload length by command identifier */
const uint8 macCmdLen[] =
{
  MAC_ASSOC_REQ_PAYLOAD,              /* MAC_ASSOC_REQ_FRAME */
  MAC_ASSOC_RSP_PAYLOAD,              /* MAC_ASSOC_RSP_FRAME */
  MAC_DISASSOC_NOTIF_PAYLOAD,         /* MAC_DISASSOC_NOTIF_FRAME */
  MAC_DATA_REQ_PAYLOAD,               /* MAC_DATA_REQ_FRAME */
  MAC_PAN_CONFLICT_PAYLOAD,           /* MAC_PAN_CONFLICT_FRAME */
  MAC_ORPHAN_NOTIF_PAYLOAD,           /* MAC_ORPHAN_NOTIF_FRAME */
  MAC_BEACON_REQ_PAYLOAD,             /* MAC_BEACON_REQ_FRAME */
  MAC_COORD_REALIGN_PAYLOAD,          /* MAC_COORD_REALIGN_FRAME */
  MAC_GTS_REQ_PAYLOAD                 /* MAC_GTS_REQ_FRAME */
};


/* TX frame success to event */
const uint8 macTxFrameSuccess[] =
{
  MAC_NO_ACTION_EVT,                  /* MAC_INTERNAL_BEACON */
  MAC_NO_ACTION_EVT,                  /* MAC_INTERNAL_DATA */
  MAC_TX_ASSOC_REQ_OK_EVT,            /* MAC_INTERNAL_ASSOC_REQ */
  MAC_TX_ASSOC_RSP_EVT,               /* MAC_INTERNAL_ASSOC_RSP */
  MAC_TX_DISASSOC_EVT,                /* MAC_INTERNAL_DISASSOC_NOTIF */
  MAC_TX_DATA_REQ_OK_EVT,             /* MAC_INTERNAL_DATA_REQ */
  MAC_TX_CONFLICT_OK_EVT,             /* MAC_INTERNAL_PAN_CONFLICT */
  MAC_TX_ORPHAN_OK_EVT,               /* MAC_INTERNAL_ORPHAN_NOTIF */
  MAC_TX_BEACON_REQ_OK_EVT,           /* MAC_INTERNAL_BEACON_REQ */
  MAC_TX_REALIGN_OK_EVT,              /* MAC_INTERNAL_COORD_REALIGN */
  MAC_NO_ACTION_EVT,                  /* MAC_INTERNAL_GTS_REQ */
  MAC_NO_ACTION_EVT,                  /* MAC_INTERNAL_COEXIST_EB_REQ */
  MAC_TX_ENHANCED_BEACON_REQ_OK_EVT,  /* MAC_INTERNAL_ENHANCED_BEACON_REQ */
  MAC_NO_ACTION_EVT                   /* MAC_INTERNAL_ENH_ACK */
};

/* TX frame failure to event */
const uint8 macTxFrameFailed[] =
{
  MAC_NO_ACTION_EVT,                  /* MAC_INTERNAL_BEACON */
  MAC_NO_ACTION_EVT,                  /* MAC_INTERNAL_DATA */
  MAC_TX_ASSOC_REQ_FAIL_EVT,          /* MAC_INTERNAL_ASSOC_REQ */
  MAC_TX_ASSOC_RSP_EVT,               /* MAC_INTERNAL_ASSOC_RSP */
  MAC_TX_DISASSOC_EVT,                /* MAC_INTERNAL_DISASSOC_NOTIF */
  MAC_TX_DATA_REQ_FAIL_EVT,           /* MAC_INTERNAL_DATA_REQ */
  MAC_NO_ACTION_EVT,                  /* MAC_INTERNAL_PAN_CONFLICT */
  MAC_TX_ORPHAN_FAIL_EVT,             /* MAC_INTERNAL_ORPHAN_NOTIF */
  MAC_TX_BEACON_REQ_FAIL_EVT,         /* MAC_INTERNAL_BEACON_REQ */
  MAC_TX_REALIGN_FAIL_EVT,            /* MAC_INTERNAL_COORD_REALIGN */
  MAC_NO_ACTION_EVT,                  /* MAC_INTERNAL_GTS_REQ */
  MAC_NO_ACTION_EVT,                   /* MAC_INTERNAL_COEXIST_EB_REQ */
  MAC_TX_ENHANCED_BEACON_REQ_FAIL_EVT, /* MAC_INTERNAL_ENHANCED_BEACON_REQ */
  MAC_NO_ACTION_EVT                   /* MAC_INTERNAL_ENH_ACK */
};

/* mac_data data */
macData_t macData;

/* pointer to current tx frame */
macTx_t *pMacDataTx = NULL;

/* indirect data handling functions */
macDataTxFcn_t macDataTxIndirect = NULL;
macDataTxFcn_t macDataRequeueIndirect = NULL;

/* critical beacon handling function */
macRxBeaconCritical_t macDataRxBeaconCritical = NULL;

/* beacon tx complete function */
macTxBeaconComplete_t macDataTxBeaconComplete = NULL;

/* tx frame set schedule function */
macDataTxFcn_t macDataSetSched = NULL;

/* tx frame check schedule function */
macDataCheckSched_t macDataCheckSched = NULL;

/* tx frame check tx time function */
macDataCheckTxTime_t macDataCheckTxTime = NULL;

/* tx frame beacon requeue function */
macDataTxFcn_t macDataBeaconRequeue = NULL;

/* tx timer for Green Power delayed transmission */
macTimer_t macDataTxDelay;

/* Device action set 1 */
macAction_t macDeviceAction1[14];

/* Device action set 2 */
macAction_t macDeviceAction2[5];

/* MAC device info */
macDevice_t macDevice;

/* State table for MAC_IDLE_ST */
const uint8 macIdleSt[][MAC_NUM_COLS] =
{
/* Event                         Action                             Next State */
/* API_DATA_REQ_EVT */          {MAC_API_DATA_REQ,                  MAC_IDLE_ST},
/* API_PURGE_REQ_EVT */         {MAC_API_PURGE_REQ,                 MAC_IDLE_ST},
/* API_ASSOCIATE_REQ_EVT */     {MAC_API_ASSOCIATE_REQ,             MAC_ASSOCIATING_ST},
/* API_ASSOCIATE_RSP_EVT */     {MAC_API_ASSOCIATE_RSP,             MAC_IDLE_ST},
/* API_DISASSOCIATE_REQ_EVT */  {MAC_API_DISASSOCIATE_REQ,          MAC_IDLE_ST},
/* API_ORPHAN_RSP_EVT */        {MAC_API_ORPHAN_RSP,                MAC_IDLE_ST},
/* API_POLL_REQ_EVT */          {MAC_API_POLL_REQ,                  MAC_POLLING_ST},
/* API_SCAN_REQ_EVT */          {MAC_API_SCAN_REQ,                  MAC_SCANNING_ST},
/* API_START_REQ_EVT */         {MAC_API_START_REQ,                 MAC_STARTING_ST},
#if defined (FEATURE_BEACON_MODE) || defined (FEATURE_NON_BEACON_MODE)
/* API_SYNC_REQ_EVT */          {MAC_API_SYNC_REQ,                  MAC_IDLE_ST},
#else
/* API_SYNC_REQ_EVT */          {MAC_NO_ACTION,                     MAC_IDLE_ST},
#endif /* (FEATURE_BEACON_MODE) || (FEATURE_NON_BEACON_MODE) */
/* API_WS_ASYNC_REQ_EVT */      {MAC_API_WS_ASYNC_REQ,              MAC_IDLE_ST},
/* API_PWR_ON_REQ_EVT */        {MAC_API_PWR_ON_REQ,                MAC_IDLE_ST},
/* RX_ASSOC_REQ_EVT */          {MAC_RX_ASSOC_REQ,                  MAC_IDLE_ST},
/* RX_ASSOC_RSP_EVT */          {MAC_NO_ACTION,                     MAC_IDLE_ST},
/* RX_DISASSOC_EVT */           {MAC_RX_DISASSOC,                   MAC_IDLE_ST},
/* RX_DATA_REQ_EVT */           {MAC_RX_DATA_REQ,                   MAC_IDLE_ST},
/* RX_PAN_CONFLICT_EVT */       {MAC_RX_PAN_CONFLICT,               MAC_IDLE_ST},
/* RX_ORPHAN_EVT */             {MAC_RX_ORPHAN,                     MAC_IDLE_ST},
/* RX_BEACON_REQ_EVT */         {MAC_RX_BEACON_REQ,                 MAC_IDLE_ST},
/* RX_REALIGN_EVT */            {MAC_RX_COORD_REALIGN,              MAC_IDLE_ST},
/* RX_GTS_REQ_EVT */            {MAC_NO_ACTION,                     MAC_IDLE_ST},
/* RX_BEACON_EVT */             {MAC_RX_BEACON,                     MAC_IDLE_ST},
/* RX_DATA_IND_EVT */           {MAC_DATA_RX_IND,                   MAC_IDLE_ST},
/* TX_ASSOC_REQ_OK_EVT */       {MAC_NO_ACTION,                     MAC_IDLE_ST},
/* TX_ASSOC_REQ_FAIL_EVT */     {MAC_NO_ACTION,                     MAC_IDLE_ST},
/* TX_ASSOC_RSP_EVT */          {MAC_COMM_STATUS_IND,               MAC_IDLE_ST},
/* TX_DISASSOC_EVT */           {MAC_DISASSOC_COMPLETE,             MAC_IDLE_ST},
/* TX_ORPHAN_OK_EVT */          {MAC_NO_ACTION,                     MAC_IDLE_ST},
/* TX_ORPHAN_FAIL_EVT */        {MAC_NO_ACTION,                     MAC_IDLE_ST},
/* TX_BEACON_REQ_OK_EVT */      {MAC_NO_ACTION,                     MAC_IDLE_ST},
/* TX_BEACON_REQ_FAIL_EVT */    {MAC_NO_ACTION,                     MAC_IDLE_ST},
/* TX_REALIGN_OK_EVT */         {MAC_COMM_STATUS_IND,               MAC_IDLE_ST},
/* TX_REALIGN_FAIL_EVT */       {MAC_COMM_STATUS_IND,               MAC_IDLE_ST},
/* TX_DATA_REQ_OK_EVT */        {MAC_POLL_DATA_REQ_COMPLETE,        MAC_IDLE_ST},
/* TX_DATA_REQ_FAIL_EVT */      {MAC_POLL_DATA_REQ_COMPLETE,        MAC_IDLE_ST},
/* TX_DATA_REQ_PEND_EVT */      {MAC_START_FRAME_RESPONSE_TIMER,    MAC_POLLING_ST},
/* TX_CONFLICT_OK_EVT */        {MAC_PAN_CONFLICT_COMPLETE,         MAC_IDLE_ST},
/* TIM_RESPONSE_WAIT_EVT */     {MAC_NO_ACTION,                     MAC_IDLE_ST},
/* TIM_FRAME_RESPONSE_EVT */    {MAC_NO_ACTION,                     MAC_IDLE_ST},
/* TIM_SCAN_EVT */              {MAC_NO_ACTION,                     MAC_IDLE_ST},
/* TIM_SYNC_EVT */              {MAC_NO_ACTION,                     MAC_IDLE_ST},
/* TIM_EXP_INDIRECT_EVT */      {MAC_INDIRECT_EXPIRE,               MAC_IDLE_ST},
/* INT_SCAN_COMPLETE_EVT */     {MAC_NO_ACTION,                     MAC_IDLE_ST},
/* INT_ASSOC_FAILED_EVT */      {MAC_ASSOC_FAILED,                  MAC_IDLE_ST},
/* INT_AUTO_POLL_EVT */         {MAC_AUTO_POLL,                     MAC_IDLE_ST},
/* INT_START_COMPLETE_EVT */    {MAC_NO_ACTION,                     MAC_IDLE_ST},
/* INT_TX_COMPLETE_EVT */       {MAC_DATA_SEND,                     MAC_IDLE_ST},
/* INT_BEACON_RESP_TIM_EVT */   {MAC_NO_ACTION,                     MAC_IDLE_ST},
/* INT_BROADCAST_PEND_EVT */    {MAC_START_BROADCAST_PEND_TIMER,    MAC_IDLE_ST},
/* NO_ACTION_EVT */             {MAC_NO_ACTION,                     MAC_IDLE_ST},
/* RX_ENHANCED_BEACON_REQ_EVT*/ {MAC_RX_ENHANCED_BEACON_REQ,        MAC_IDLE_ST},
/* TX_ENHANCED_BEACON_REQ_OK_EVT*/   {MAC_NO_ACTION,                MAC_IDLE_ST},
/* TX_ENHANCED_BEACON_REQ_FAIL_EVT*/ {MAC_NO_ACTION,                MAC_IDLE_ST},
/* CSMA_TIM_EXP_EVT*/           {MAC_CSMA_TIM_EXPIRY,               MAC_IDLE_ST},
/* BC_TIM_EXP_EVT*/             {MAC_BC_TIM_EXPIRY,                 MAC_IDLE_ST}
};

/* State table for MAC_ASSOCIATING_ST */
const uint8 macAssociatingSt[][MAC_NUM_COLS] =
//macStateTbl_t macAssociatingSt =
{
/* Event                         Action                             Next State */
/* API_DATA_REQ_EVT */          {MAC_API_BAD_STATE,                 MAC_ASSOCIATING_ST},
/* API_PURGE_REQ_EVT */         {MAC_API_PURGE_REQ,                 MAC_ASSOCIATING_ST},
/* API_ASSOCIATE_REQ_EVT */     {MAC_API_BAD_STATE,                 MAC_ASSOCIATING_ST},
/* API_ASSOCIATE_RSP_EVT */     {MAC_API_BAD_STATE,                 MAC_ASSOCIATING_ST},
/* API_DISASSOCIATE_REQ_EVT */  {MAC_API_DISASSOCIATE_REQ,          MAC_ASSOCIATING_ST},
/* API_ORPHAN_RSP_EVT */        {MAC_API_BAD_STATE,                 MAC_ASSOCIATING_ST},
/* API_POLL_REQ_EVT */          {MAC_API_POLL_REQ,                  MAC_ASSOCIATING_ST},
/* API_SCAN_REQ_EVT */          {MAC_API_BAD_STATE,                 MAC_ASSOCIATING_ST},
/* API_START_REQ_EVT */         {MAC_API_BAD_STATE,                 MAC_ASSOCIATING_ST},
/* API_SYNC_REQ_EVT */          {MAC_API_SYNC_REQ,                  MAC_ASSOCIATING_ST},
/* API_WS_ASYNC_REQ_EVT */      {MAC_NO_ACTION,                     MAC_ASSOCIATING_ST},
/* API_PWR_ON_REQ_EVT */        {MAC_API_PWR_ON_REQ,                MAC_ASSOCIATING_ST},
/* RX_ASSOC_REQ_EVT */          {MAC_RX_ASSOC_REQ,                  MAC_ASSOCIATING_ST},
/* RX_ASSOC_RSP_EVT */          {MAC_RX_ASSOC_RSP,                  MAC_IDLE_ST},
/* RX_DISASSOC_EVT */           {MAC_RX_DISASSOC,                   MAC_IDLE_ST},
/* RX_DATA_REQ_EVT */           {MAC_RX_DATA_REQ,                   MAC_ASSOCIATING_ST},
/* RX_PAN_CONFLICT_EVT */       {MAC_RX_PAN_CONFLICT,               MAC_ASSOCIATING_ST},
/* RX_ORPHAN_EVT */             {MAC_RX_ORPHAN,                     MAC_ASSOCIATING_ST},
/* RX_BEACON_REQ_EVT */         {MAC_RX_BEACON_REQ,                 MAC_ASSOCIATING_ST},
/* RX_REALIGN_EVT */            {MAC_RX_COORD_REALIGN,              MAC_ASSOCIATING_ST},
/* RX_GTS_REQ_EVT */            {MAC_NO_ACTION,                     MAC_ASSOCIATING_ST},
/* RX_BEACON_EVT */             {MAC_RX_BEACON,                     MAC_ASSOCIATING_ST},
/* RX_DATA_IND_EVT */           {MAC_DATA_RX_IND,                   MAC_ASSOCIATING_ST},
/* TX_ASSOC_REQ_OK_EVT */       {MAC_START_RESPONSE_TIMER,          MAC_ASSOCIATING_ST},
/* TX_ASSOC_REQ_FAIL_EVT */     {MAC_ASSOC_FAILED,                  MAC_IDLE_ST},
/* TX_ASSOC_RSP_EVT */          {MAC_COMM_STATUS_IND,               MAC_ASSOCIATING_ST},
/* TX_DISASSOC_EVT */           {MAC_DISASSOC_COMPLETE,             MAC_ASSOCIATING_ST},
/* TX_ORPHAN_OK_EVT */          {MAC_NO_ACTION,                     MAC_ASSOCIATING_ST},
/* TX_ORPHAN_FAIL_EVT */        {MAC_NO_ACTION,                     MAC_ASSOCIATING_ST},
/* TX_BEACON_REQ_OK_EVT */      {MAC_NO_ACTION,                     MAC_ASSOCIATING_ST},
/* TX_BEACON_REQ_FAIL_EVT */    {MAC_NO_ACTION,                     MAC_ASSOCIATING_ST},
/* TX_REALIGN_OK_EVT */         {MAC_COMM_STATUS_IND,               MAC_ASSOCIATING_ST},
/* TX_REALIGN_FAIL_EVT */       {MAC_COMM_STATUS_IND,               MAC_ASSOCIATING_ST},
/* TX_DATA_REQ_OK_EVT */        {MAC_ASSOC_DATA_REQ_COMPLETE,       MAC_ASSOCIATING_ST},
/* TX_DATA_REQ_FAIL_EVT */      {MAC_ASSOC_DATA_REQ_COMPLETE,       MAC_ASSOCIATING_ST},
/* TX_DATA_REQ_PEND_EVT */      {MAC_START_FRAME_RESPONSE_TIMER,    MAC_ASSOCIATING_ST},
/* TX_CONFLICT_OK_EVT */        {MAC_PAN_CONFLICT_COMPLETE,         MAC_ASSOCIATING_ST},
/* TIM_RESPONSE_WAIT_EVT */     {MAC_ASSOC_DATA_REQ,                MAC_ASSOCIATING_ST},
/* TIM_FRAME_RESPONSE_EVT */    {MAC_ASSOC_FRAME_RESPONSE_TIMEOUT,  MAC_IDLE_ST},
/* TIM_SCAN_EVT */              {MAC_NO_ACTION,                     MAC_ASSOCIATING_ST},
/* TIM_SYNC_EVT */              {MAC_NO_ACTION,                     MAC_ASSOCIATING_ST},
/* TIM_EXP_INDIRECT_EVT */      {MAC_INDIRECT_EXPIRE,               MAC_ASSOCIATING_ST},
/* INT_SCAN_COMPLETE_EVT */     {MAC_NO_ACTION,                     MAC_ASSOCIATING_ST},
/* INT_ASSOC_FAILED_EVT */      {MAC_ASSOC_FAILED,                  MAC_IDLE_ST},
/* INT_AUTO_POLL_EVT */         {MAC_AUTO_POLL,                     MAC_ASSOCIATING_ST},
/* INT_START_COMPLETE_EVT */    {MAC_NO_ACTION,                     MAC_ASSOCIATING_ST},
/* INT_TX_COMPLETE_EVT */       {MAC_DATA_SEND,                     MAC_ASSOCIATING_ST},
/* INT_BEACON_RESP_TIM_EVT */   {MAC_BEACON_START_FRAME_RSP_TIMER,  MAC_ASSOCIATING_ST},
/* INT_BROADCAST_PEND_EVT */    {MAC_START_BROADCAST_PEND_TIMER,    MAC_ASSOCIATING_ST},
/* NO_ACTION_EVT */             {MAC_NO_ACTION,                     MAC_ASSOCIATING_ST},
/* RX_ENHANCED_BEACON_REQ_EVT*/ {MAC_RX_ENHANCED_BEACON_REQ,        MAC_ASSOCIATING_ST},
/* TX_ENHANCED_BEACON_REQ_OK_EVT*/   {MAC_NO_ACTION,                MAC_ASSOCIATING_ST},
/* TX_ENHANCED_BEACON_REQ_FAIL_EVT*/ {MAC_NO_ACTION,                MAC_ASSOCIATING_ST},
/* CSMA_TIM_EXP_EVT*/           {MAC_CSMA_TIM_EXPIRY,               MAC_ASSOCIATING_ST},
/* BC_TIM_EXP_EVT*/             {MAC_BC_TIM_EXPIRY,                 MAC_ASSOCIATING_ST}
};

/* State table for MAC_POLLING_ST */
const uint8 macPollingSt[][MAC_NUM_COLS] =
{
/* Event                         Action                             Next State */
/* API_DATA_REQ_EVT */          {MAC_API_DATA_REQ,                  MAC_POLLING_ST},
/* API_PURGE_REQ_EVT */         {MAC_API_PURGE_REQ,                 MAC_POLLING_ST},
/* API_ASSOCIATE_REQ_EVT */     {MAC_API_ASSOCIATE_REQ,             MAC_ASSOCIATING_ST},
/* API_ASSOCIATE_RSP_EVT */     {MAC_API_ASSOCIATE_RSP,             MAC_POLLING_ST},
/* API_DISASSOCIATE_REQ_EVT */  {MAC_API_DISASSOCIATE_REQ,          MAC_POLLING_ST},
/* API_ORPHAN_RSP_EVT */        {MAC_API_ORPHAN_RSP,                MAC_POLLING_ST},
/* API_POLL_REQ_EVT */          {MAC_API_BAD_STATE,                 MAC_POLLING_ST},
/* API_SCAN_REQ_EVT */          {MAC_API_PENDING,                   MAC_POLLING_ST},
/* API_START_REQ_EVT */         {MAC_API_PENDING,                   MAC_POLLING_ST},
/* API_SYNC_REQ_EVT */          {MAC_API_SYNC_REQ,                  MAC_POLLING_ST},
/* API_WS_ASYNC_REQ_EVT */      {MAC_NO_ACTION,                     MAC_POLLING_ST},
/* API_PWR_ON_REQ_EVT */        {MAC_API_PWR_ON_REQ,                MAC_POLLING_ST},
/* RX_ASSOC_REQ_EVT */          {MAC_RX_ASSOC_REQ,                  MAC_POLLING_ST},
/* RX_ASSOC_RSP_EVT */          {MAC_POLL_RX_ASSOC_RSP,             MAC_IDLE_ST},
/* RX_DISASSOC_EVT */           {MAC_POLL_RX_DISASSOC,              MAC_IDLE_ST},
/* RX_DATA_REQ_EVT */           {MAC_RX_DATA_REQ,                   MAC_POLLING_ST},
/* RX_PAN_CONFLICT_EVT */       {MAC_RX_PAN_CONFLICT,               MAC_POLLING_ST},
/* RX_ORPHAN_EVT */             {MAC_RX_ORPHAN,                     MAC_POLLING_ST},
/* RX_BEACON_REQ_EVT */         {MAC_RX_BEACON_REQ,                 MAC_POLLING_ST},
/* RX_REALIGN_EVT */            {MAC_RX_COORD_REALIGN,              MAC_POLLING_ST},
/* RX_GTS_REQ_EVT */            {MAC_NO_ACTION,                     MAC_POLLING_ST},
/* RX_BEACON_EVT */             {MAC_RX_BEACON,                     MAC_POLLING_ST},
/* RX_DATA_IND_EVT */           {MAC_POLL_DATA_RX_IND,              MAC_IDLE_ST},
/* TX_ASSOC_REQ_OK_EVT */       {MAC_NO_ACTION,                     MAC_POLLING_ST},
/* TX_ASSOC_REQ_FAIL_EVT */     {MAC_NO_ACTION,                     MAC_POLLING_ST},
/* TX_ASSOC_RSP_EVT */          {MAC_COMM_STATUS_IND,               MAC_POLLING_ST},
/* TX_DISASSOC_EVT */           {MAC_DISASSOC_COMPLETE,             MAC_POLLING_ST},
/* TX_ORPHAN_OK_EVT */          {MAC_NO_ACTION,                     MAC_POLLING_ST},
/* TX_ORPHAN_FAIL_EVT */        {MAC_NO_ACTION,                     MAC_POLLING_ST},
/* TX_BEACON_REQ_OK_EVT */      {MAC_NO_ACTION,                     MAC_POLLING_ST},
/* TX_BEACON_REQ_FAIL_EVT */    {MAC_NO_ACTION,                     MAC_POLLING_ST},
/* TX_REALIGN_OK_EVT */         {MAC_COMM_STATUS_IND,               MAC_POLLING_ST},
/* TX_REALIGN_FAIL_EVT */       {MAC_COMM_STATUS_IND,               MAC_POLLING_ST},
/* TX_DATA_REQ_OK_EVT */        {MAC_POLL_DATA_REQ_COMPLETE,        MAC_IDLE_ST},
/* TX_DATA_REQ_FAIL_EVT */      {MAC_POLL_DATA_REQ_COMPLETE,        MAC_IDLE_ST},
/* TX_DATA_REQ_PEND_EVT */      {MAC_START_FRAME_RESPONSE_TIMER,    MAC_POLLING_ST},
/* TX_CONFLICT_OK_EVT */        {MAC_PAN_CONFLICT_COMPLETE,         MAC_POLLING_ST},
/* TIM_RESPONSE_WAIT_EVT */     {MAC_NO_ACTION,                     MAC_POLLING_ST},
/* TIM_FRAME_RESPONSE_EVT */    {MAC_POLL_FRAME_RESPONSE_TIMEOUT,   MAC_IDLE_ST},
/* TIM_SCAN_EVT */              {MAC_NO_ACTION,                     MAC_POLLING_ST},
/* TIM_SYNC_EVT */              {MAC_NO_ACTION,                     MAC_POLLING_ST},
/* TIM_EXP_INDIRECT_EVT */      {MAC_INDIRECT_EXPIRE,               MAC_POLLING_ST},
/* INT_SCAN_COMPLETE_EVT */     {MAC_NO_ACTION,                     MAC_POLLING_ST},
/* INT_ASSOC_FAILED_EVT */      {MAC_ASSOC_FAILED,                  MAC_POLLING_ST},
/* INT_AUTO_POLL_EVT */         {MAC_AUTO_POLL,                     MAC_POLLING_ST},
/* INT_START_COMPLETE_EVT */    {MAC_NO_ACTION,                     MAC_POLLING_ST},
/* INT_TX_COMPLETE_EVT */       {MAC_DATA_SEND,                     MAC_POLLING_ST},
/* INT_BEACON_RESP_TIM_EVT */   {MAC_BEACON_START_FRAME_RSP_TIMER,  MAC_POLLING_ST},
/* INT_BROADCAST_PEND_EVT */    {MAC_START_BROADCAST_PEND_TIMER,    MAC_POLLING_ST},
/* NO_ACTION_EVT */             {MAC_NO_ACTION,                     MAC_POLLING_ST},
/* RX_ENHANCED_BEACON_REQ_EVT*/ {MAC_RX_ENHANCED_BEACON_REQ,        MAC_POLLING_ST},
/* TX_ENHANCED_BEACON_REQ_OK_EVT*/  {MAC_NO_ACTION,                 MAC_POLLING_ST},
/* TX_ENHANCED_BEACON_REQ_FAIL_EVT*/{MAC_NO_ACTION,                 MAC_POLLING_ST},
/* CSMA_TIM_EXP_EVT*/           {MAC_CSMA_TIM_EXPIRY,               MAC_POLLING_ST},
/* BC_TIM_EXP_EVT*/             {MAC_BC_TIM_EXPIRY,                 MAC_POLLING_ST}
};

#if defined (FEATURE_BEACON_MODE) || defined (FEATURE_NON_BEACON_MODE)
/* State table for MAC_SCANNING_ST */
const uint8 macScanningSt[][MAC_NUM_COLS] =
{
/* Event                         Action                             Next State */
/* API_DATA_REQ_EVT */          {MAC_API_BAD_STATE,                 MAC_SCANNING_ST},
/* API_PURGE_REQ_EVT */         {MAC_API_PURGE_REQ,                 MAC_SCANNING_ST},
/* API_ASSOCIATE_REQ_EVT */     {MAC_API_BAD_STATE,                 MAC_SCANNING_ST},
/* API_ASSOCIATE_RSP_EVT */     {MAC_API_BAD_STATE,                 MAC_SCANNING_ST},
/* API_DISASSOCIATE_REQ_EVT */  {MAC_API_BAD_STATE,                 MAC_SCANNING_ST},
/* API_ORPHAN_RSP_EVT */        {MAC_API_BAD_STATE,                 MAC_SCANNING_ST},
/* API_POLL_REQ_EVT */          {MAC_API_BAD_STATE,                 MAC_SCANNING_ST},
/* API_SCAN_REQ_EVT */          {MAC_SCAN_FAILED_IN_PROGRESS,       MAC_SCANNING_ST},
/* API_START_REQ_EVT */         {MAC_API_PENDING,                   MAC_SCANNING_ST},
/* API_SYNC_REQ_EVT */          {MAC_API_BAD_STATE,                 MAC_SCANNING_ST},
/* API_WS_ASYNC_REQ_EVT */      {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* API_PWR_ON_REQ_EVT */        {MAC_API_PWR_ON_REQ,                MAC_SCANNING_ST},
/* RX_ASSOC_REQ_EVT */          {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* RX_ASSOC_RSP_EVT */          {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* RX_DISASSOC_EVT */           {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* RX_DATA_REQ_EVT */           {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* RX_PAN_CONFLICT_EVT */       {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* RX_ORPHAN_EVT */             {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* RX_BEACON_REQ_EVT */         {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* RX_REALIGN_EVT */            {MAC_SCAN_RX_COORD_REALIGN,         MAC_SCANNING_ST},
/* RX_GTS_REQ_EVT */            {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* RX_BEACON_EVT */             {MAC_SCAN_RX_BEACON,                MAC_SCANNING_ST},
/* RX_DATA_IND_EVT */           {MAC_DATA_RX_IND,                   MAC_SCANNING_ST},
/* TX_ASSOC_REQ_OK_EVT */       {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* TX_ASSOC_REQ_FAIL_EVT */     {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* TX_ASSOC_RSP_EVT */          {MAC_COMM_STATUS_IND,               MAC_SCANNING_ST},
/* TX_DISASSOC_EVT */           {MAC_DISASSOC_COMPLETE,             MAC_SCANNING_ST},
/* TX_ORPHAN_OK_EVT */          {MAC_SCAN_START_TIMER,              MAC_SCANNING_ST},
/* TX_ORPHAN_FAIL_EVT */        {MAC_SCAN_NEXT_CHAN,                MAC_SCANNING_ST},
/* TX_BEACON_REQ_OK_EVT */      {MAC_SCAN_START_TIMER,              MAC_SCANNING_ST},
/* TX_BEACON_REQ_FAIL_EVT */    {MAC_SCAN_NEXT_CHAN,                MAC_SCANNING_ST},
/* TX_REALIGN_OK_EVT */         {MAC_COMM_STATUS_IND,               MAC_SCANNING_ST},
/* TX_REALIGN_FAIL_EVT */       {MAC_COMM_STATUS_IND,               MAC_SCANNING_ST},
/* TX_DATA_REQ_OK_EVT */        {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* TX_DATA_REQ_FAIL_EVT */      {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* TX_DATA_REQ_PEND_EVT */      {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* TX_CONFLICT_OK_EVT */        {MAC_PAN_CONFLICT_COMPLETE,         MAC_SCANNING_ST},
/* TIM_RESPONSE_WAIT_EVT */     {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* TIM_FRAME_RESPONSE_EVT */    {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* TIM_SCAN_EVT */              {MAC_SCAN_NEXT_CHAN,                MAC_SCANNING_ST},
/* TIM_SYNC_EVT */              {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* TIM_EXP_INDIRECT_EVT */      {MAC_INDIRECT_EXPIRE,               MAC_SCANNING_ST},
/* INT_SCAN_COMPLETE_EVT */     {MAC_SCAN_COMPLETE,                 MAC_IDLE_ST},
/* INT_ASSOC_FAILED_EVT */      {MAC_ASSOC_FAILED,                  MAC_SCANNING_ST},
/* INT_AUTO_POLL_EVT */         {MAC_AUTO_POLL,                     MAC_SCANNING_ST},
/* INT_START_COMPLETE_EVT */    {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* INT_TX_COMPLETE_EVT */       {MAC_DATA_SEND,                     MAC_SCANNING_ST},
/* INT_BEACON_RESP_TIM_EVT */   {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* INT_BROADCAST_PEND_EVT */    {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* NO_ACTION_EVT */             {MAC_NO_ACTION,                     MAC_SCANNING_ST},
/* RX_ENHANCED_BEACON_REQ_OK_EVT */   {MAC_NO_ACTION,               MAC_SCANNING_ST},
/* TX_ENHANCED_BEACON_REQ_OK_EVT */   {MAC_SCAN_START_TIMER,        MAC_SCANNING_ST},
/* TX_ENHANCED_BEACON_REQ_FAIL_EVT */ {MAC_SCAN_NEXT_CHAN,          MAC_SCANNING_ST},
/* CSMA_TIM_EXP_EVT*/           {MAC_CSMA_TIM_EXPIRY,               MAC_SCANNING_ST},
/* BC_TIM_EXP_EVT*/             {MAC_BC_TIM_EXPIRY,                 MAC_SCANNING_ST}
};
#endif /* (FEATURE_BEACON_MODE) || (FEATURE_NON_BEACON_MODE) */

/* State table for MAC_STARTING_ST */
const uint8 macStartingSt[][MAC_NUM_COLS] =
{
/* Event                         Action                             Next State */
/* API_DATA_REQ_EVT */          {MAC_API_DATA_REQ,                  MAC_STARTING_ST},
/* API_PURGE_REQ_EVT */         {MAC_API_PURGE_REQ,                 MAC_STARTING_ST},
/* API_ASSOCIATE_REQ_EVT */     {MAC_API_PENDING,                   MAC_STARTING_ST},
/* API_ASSOCIATE_RSP_EVT */     {MAC_API_ASSOCIATE_RSP,             MAC_STARTING_ST},
/* API_DISASSOCIATE_REQ_EVT */  {MAC_API_DISASSOCIATE_REQ,          MAC_STARTING_ST},
/* API_ORPHAN_RSP_EVT */        {MAC_API_ORPHAN_RSP,                MAC_STARTING_ST},
/* API_POLL_REQ_EVT */          {MAC_API_PENDING,                   MAC_STARTING_ST},
/* API_SCAN_REQ_EVT */          {MAC_API_PENDING,                   MAC_STARTING_ST},
/* API_START_REQ_EVT */         {MAC_API_BAD_STATE,                 MAC_STARTING_ST},
/* API_SYNC_REQ_EVT */          {MAC_API_PENDING,                   MAC_STARTING_ST},
/* API_WS_ASYNC_REQ_EVT */      {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* API_PWR_ON_REQ_EVT */        {MAC_API_PWR_ON_REQ,                MAC_STARTING_ST},
/* RX_ASSOC_REQ_EVT */          {MAC_RX_ASSOC_REQ,                  MAC_STARTING_ST},
/* RX_ASSOC_RSP_EVT */          {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* RX_DISASSOC_EVT */           {MAC_RX_DISASSOC,                   MAC_STARTING_ST},
/* RX_DATA_REQ_EVT */           {MAC_RX_DATA_REQ,                   MAC_STARTING_ST},
/* RX_PAN_CONFLICT_EVT */       {MAC_RX_PAN_CONFLICT,               MAC_STARTING_ST},
/* RX_ORPHAN_EVT */             {MAC_RX_ORPHAN,                     MAC_STARTING_ST},
/* RX_BEACON_REQ_EVT */         {MAC_RX_BEACON_REQ,                 MAC_STARTING_ST},
/* RX_REALIGN_EVT */            {MAC_RX_COORD_REALIGN,              MAC_STARTING_ST},
/* RX_GTS_REQ_EVT */            {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* RX_BEACON_EVT */             {MAC_RX_BEACON,                     MAC_STARTING_ST},
/* RX_DATA_IND_EVT */           {MAC_DATA_RX_IND,                   MAC_STARTING_ST},
/* TX_ASSOC_REQ_OK_EVT */       {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* TX_ASSOC_REQ_FAIL_EVT */     {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* TX_ASSOC_RSP_EVT */          {MAC_COMM_STATUS_IND,               MAC_STARTING_ST},
/* TX_DISASSOC_EVT */           {MAC_DISASSOC_COMPLETE,             MAC_STARTING_ST},
/* TX_ORPHAN_OK_EVT */          {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* TX_ORPHAN_FAIL_EVT */        {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* TX_BEACON_REQ_OK_EVT */      {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* TX_BEACON_REQ_FAIL_EVT */    {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* TX_REALIGN_OK_EVT */         {MAC_START_CONTINUE,                MAC_STARTING_ST},
/* TX_REALIGN_FAIL_EVT */       {MAC_START_COMPLETE,                MAC_IDLE_ST},
/* TX_DATA_REQ_OK_EVT */        {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* TX_DATA_REQ_FAIL_EVT */      {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* TX_DATA_REQ_PEND_EVT */      {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* TX_CONFLICT_OK_EVT */        {MAC_PAN_CONFLICT_COMPLETE,         MAC_STARTING_ST},
/* TIM_RESPONSE_WAIT_EVT */     {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* TIM_FRAME_RESPONSE_EVT */    {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* TIM_SCAN_EVT */              {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* TIM_SYNC_EVT */              {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* TIM_EXP_INDIRECT_EVT */      {MAC_INDIRECT_EXPIRE,               MAC_STARTING_ST},
/* INT_SCAN_COMPLETE_EVT */     {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* INT_ASSOC_FAILED_EVT */      {MAC_ASSOC_FAILED,                  MAC_STARTING_ST},
/* INT_AUTO_POLL_EVT */         {MAC_AUTO_POLL,                     MAC_STARTING_ST},
/* INT_START_COMPLETE_EVT */    {MAC_START_COMPLETE,                MAC_IDLE_ST},
/* INT_TX_COMPLETE_EVT */       {MAC_DATA_SEND,                     MAC_STARTING_ST},
/* INT_BEACON_RESP_TIM_EVT */   {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* INT_BROADCAST_PEND_EVT */    {MAC_START_BROADCAST_PEND_TIMER,    MAC_STARTING_ST},
/* NO_ACTION_EVT */             {MAC_NO_ACTION,                     MAC_STARTING_ST},
/* RX_ENHANCED_BEACON_REQ_EVT*/ {MAC_RX_ENHANCED_BEACON_REQ,        MAC_STARTING_ST},
/* TX_ENHANCED_BEACON_REQ_OK_EVT*/   {MAC_NO_ACTION,                MAC_STARTING_ST},
/* TX_ENHANCED_BEACON_REQ_FAIL_EVT*/ {MAC_NO_ACTION,                MAC_STARTING_ST},
/* CSMA_TIM_EXP_EVT*/           {MAC_CSMA_TIM_EXPIRY,               MAC_STARTING_ST},
/* BC_TIM_EXP_EVT*/             {MAC_BC_TIM_EXPIRY,                 MAC_STARTING_ST}
};

/* Message buffer size table used by macSendMsg */
const uint16 macSendMsgBufSize[] =
{
  0,                                                 /* API_DATA_REQ_EVT ignored */
  (uint16) offsetof(macApiEvent_t, mac) +             /* API_PURGE_REQ_EVT */
    sizeof(macMcpsPurgeReq_t),
  (uint16) offsetof(macApiDataEvent_t, mac) +         /* API_ASSOCIATE_REQ_EVT */
    sizeof(ApiMac_mlmeAssociateReq_t) + MAC_ASSOC_REQ_FRAME_LEN,
  (uint16) offsetof(macApiDataEvent_t, mac) +         /* API_ASSOCIATE_RSP_EVT */
    sizeof(ApiMac_mlmeAssociateRsp_t) + MAC_ASSOC_RSP_FRAME_LEN,
  (uint16) offsetof(macApiDataEvent_t, mac) +         /* API_DISASSOCIATE_REQ_EVT */
    sizeof(ApiMac_mlmeDisassociateReq_t) + MAC_DISASSOC_NOTIF_FRAME_LEN,
  (uint16) offsetof(macApiDataEvent_t, mac) +         /* API_ORPHAN_RSP_EVT */
    sizeof(ApiMac_mlmeOrphanRsp_t) + MAC_COORD_REALIGN_FRAME_LEN,
  (uint16) offsetof(macApiEvent_t, mac) +             /* API_POLL_REQ_EVT */
    sizeof(ApiMac_mlmePollReq_t),
  /* Note: We need to use macMlmeScanReq_t for new code as well */
  (uint16) offsetof(macApiEvent_t, mac) +             /* API_SCAN_REQ_EVT */
    sizeof(macMlmeScanReq_t),
  (uint16) offsetof(macApiEvent_t, mac) +             /* API_START_REQ_EVT */
    sizeof(ApiMac_mlmeStartReq_t),
  (uint16) offsetof(macApiEvent_t, mac) +             /* API_SYNC_REQ_EVT */
      sizeof(ApiMac_mlmeSyncReq_t),
  (uint16) offsetof(macApiDataEvent_t, mac) +          /* API_WS_ASYNC_REQ_EVT */
      sizeof(ApiMac_mlmeWSAsyncReq_t) + MAC_WS_ASYNC_REQ_FRAME_LEN,
  (uint16) sizeof(macEventHdr_t)                      /* API_PWR_ON_REQ_EVT */
};

/* Parameter data size table used by macSendMsg */
const uint8 macSendMsgDataSize[] =
{
  0,                                          /* API_DATA_REQ_EVT ignored */
  (uint8) sizeof(macMcpsPurgeReq_t),          /* API_PURGE_REQ_EVT */
  (uint8) sizeof(ApiMac_mlmeAssociateReq_t),      /* API_ASSOCIATE_REQ_EVT */
  (uint8) sizeof(ApiMac_mlmeAssociateRsp_t),      /* API_ASSOCIATE_RSP_EVT */
  (uint8) sizeof(ApiMac_mlmeDisassociateReq_t),   /* API_DISASSOCIATE_REQ_EVT */
  (uint8) sizeof(ApiMac_mlmeOrphanRsp_t),         /* API_ORPHAN_RSP_EVT */
  (uint8) sizeof(ApiMac_mlmePollReq_t),           /* API_POLL_REQ_EVT */
  (uint8) sizeof(macMlmeScanReq_t),               /* API_SCAN_REQ_EVT */
  (uint8) sizeof(ApiMac_mlmeStartReq_t),          /* API_START_REQ_EVT */
  (uint8) sizeof(ApiMac_mlmeSyncReq_t),           /* API_SYNC_REQ_EVT */
  (uint8) sizeof(ApiMac_mlmeWSAsyncReq_t),        /* API_WS_ASYNC_REQ_EVT */
  0                                           /* API_PWR_ON_REQ_EVT */
};

/* Lookup callback event based on MAC event */
const uint8 macCbackEventTbl[] =
{
  MAC_MCPS_DATA_CNF,            /* API_DATA_REQ_EVT */
  MAC_MCPS_PURGE_CNF,           /* API_PURGE_REQ_EVT */
  MAC_MLME_ASSOCIATE_CNF,       /* API_ASSOCIATE_REQ_EVT */
  MAC_MLME_COMM_STATUS_IND,     /* API_ASSOCIATE_RSP_EVT */
  MAC_MLME_DISASSOCIATE_CNF,    /* API_DISASSOCIATE_REQ_EVT */
  MAC_MLME_COMM_STATUS_IND,     /* API_ORPHAN_RSP_EVT */
  MAC_MLME_POLL_CNF,            /* API_POLL_REQ_EVT */
  MAC_MLME_SCAN_CNF,            /* API_SCAN_REQ_EVT */
  MAC_MLME_START_CNF,           /* API_START_REQ_EVT */
  0,                            /* API_SYNC_REQ_EVT */
  MAC_MLME_WS_ASYNC_CNF,        /* API_WS_ASYNC_REQ_EVT */
  MAC_PWR_ON_CNF                /* API_PWR_ON_REQ_EVT */
};

/* OSAL task id */
uint8 macTaskId;

/* mac_main data */
macMain_t macMain;

/* State table */
macStateTbl_t macStateTbl[] =
{
  macIdleSt,            /* MAC_IDLE_ST */
  NULL,                 /* MAC_ASSOCIATING_ST */
  NULL,                 /* MAC_POLLING_ST */
#if defined (FEATURE_BEACON_MODE) || defined (FEATURE_NON_BEACON_MODE)
  macScanningSt,        /* MAC_SCANNING_ST */
#else
  NULL,                 /* MAC_SCANNING_ST */
#endif /* (FEATURE_BEACON_MODE) || (FEATURE_NON_BEACON_MODE) */
  NULL                  /* MAC_STARTING_ST */
};


/* initialization functions */
macReset_t macReset[MAC_FEAT_MAX];

/* Table of state machine action function table pointers */
macActionSet_t macActionSet[MAC_ACTION_SET_MAX] =
{
  NULL,                 /* Mgmt set 1 */
  NULL,                 /* Mgmt set 2 */
  NULL,                 /* Scan */
  NULL,                 /* Device set 1 */
  NULL,                 /* Device set 2 */
  NULL,                 /* Coordinator set 1 */
  NULL,                 /* Coordinator set 2 */
  NULL,                 /* Coordinator set 3 */
  NULL                  /* Beacon device */
};


/* MAC mangement info */
macMgmt_t macMgmt;

/* TRUE if operating as a PAN coordinator */
bool macPanCoordinator;

/* Action set 1 */
macAction_t macMgmtAction1[10];

/* Action set 2 */
macAction_t macMgmtAction2[5];

/**
  Flag which identifies whether frequency hopping is enabled or
  not.
*/
//bool freqHopEnabled = FALSE;

/**
  frequency hopping object. Holds the function pointers related
  to frequency hopping module.
*/
fhObject_t fhObject = {0};

bool csmPhy = FALSE;

/* PIB default values */
const macPib_t macPibDefaults =
{
  54,                                         /* ackWaitDuration */
  FALSE,                                      /* associationPermit */
  TRUE,                                       /* autoRequest */
  FALSE,                                      /* battLifeExt */
  6,                                          /* battLifeExtPeriods */

  NULL,                                       /* *pMacBeaconPayload */
  0,                                          /* beaconPayloadLength */
  MAC_BO_NON_BEACON,                          /* beaconOrder */
  0,                                          /* beaconTxTime */
  0,                                          /* bsn */

  {0, SADDR_MODE_EXT},                        /* coordExtendedAddress */
  MAC_SHORT_ADDR_NONE,                        /* coordShortAddress */
  0,                                          /* dsn */
  FALSE,                                      /* gtsPermit */
  4,                                          /* maxCsmaBackoffs */

  MAC_DEFAULT_MIN_BE,                         /* minBe */
  0xFFFF,                                     /* panId */
  FALSE,                                      /* promiscuousMode */
  FALSE,                                      /* rxOnWhenIdle */
  MAC_SHORT_ADDR_NONE,                        /* shortAddress */

  MAC_SO_NONE,                                /* superframeOrder */
  1000,                                       /* transactionPersistenceTime */
  FALSE,                                      /* assocciatedPanCoord */
  MAC_DEFAULT_MAX_BE,                         /* maxBe */
  MAC_DEFAULT_FRAME_WAIT_TIME,                /* maxFrameTotalWaitTime */
  3,                                          /* maxFrameRetries */
  MAC_2P4G_RESPONSE_WAIT_TIME,                /* responseWaitTime */
  0,                                          /* syncSymbolOffset */
  TRUE,                                       /* timeStampSupported */
#ifdef FEATURE_MAC_SECURITY
  TRUE,                                      /* securityEnabled */
#else
  FALSE,                                      /* securityEnabled */
#endif
  0,                                          /* ebsn */
  0,                                          /* Enhanced beacon order */
  16383,                                      /* Non Beacon PAN enhanced beacon order */
  15,                                         /* ehanced beacon offset time slot */
  FALSE,                                      /* MPM IE to be included */

  MAC_PREAMBLE_LEN_8,                         /* FSK presamble len, 4 to 31 */
  0,                                          /* MR-FSK PHY SFD, valid values 0, 1 */

  /* Proprietary */
  14,                                         /* phyTransmitPower */
#ifndef FREQ_2_4G
  MAC_CHAN_LOWEST,                            /* logicalChannel */
#else
  11,                                         /* logicalChannel */
#endif
  {0, SADDR_MODE_EXT},                        /* extendedAddress */
  1,                                          /* altBe */
  MAC_BO_NON_BEACON,                          /* deviceBeaconOrder */
  0,                                          /* power savings for rf4ce */
  0,                                          /* default is frame Ver is always 0,
                                                 except for security enabled packets */
#ifndef FREQ_2_4G
  9,                                          /* default channel page */
  MAC_STD_US_915_PHY_1,                       /* current PHY Descriptor ID */
#else
  0,                                          /* default channel page */
  0,                                          /* current PHY Descriptor ID */
#endif
  0,                                          /* fcsType */

  0,                                          /* diagsRxCrcPass */
  0,                                          /* diagsRxCrcFail */
  0,                                          /* diagsRxBcast */
  0,                                          /* diagsTxBcast */
  0,                                          /* diagsRxUcast */
  0,                                          /* diagsTxUcast */
  0,                                          /* diagsTxUcastRetry */
  0,                                          /* diagsTxUcastFail */
  0,                                          /* diagsRxSecFail */
  0,                                          /* diagsTxSecFail */
  -83,                                        /* LBT RSSI Threshold */
  0,                                          /* range extender mode */
  0,                                          /* enable Ack Pending for Data Pkts */
#ifndef FREQ_2_4G
  MAC_RF_FREQ_SUBG,                           /* rf freq */
#else
  MAC_RF_FREQ_2_4G,                           /* rf freq */
#endif
  0                                           /* PA type */
};


/* PIB access and min/max table.  min/max of 0/0 means not checked; if min/max are
 * equal, element is read-only
 */
const macPibTbl_t macPibTbl[] =
{
  {offsetof(macPib_t, ackWaitDuration), sizeof(uint8), 54, 54},                      /* MAC_ACK_WAIT_DURATION */
  {offsetof(macPib_t, associationPermit), sizeof(bool), FALSE, TRUE},                /* MAC_ASSOCIATION_PERMIT */
  {offsetof(macPib_t, autoRequest), sizeof(bool), FALSE, TRUE},                      /* MAC_AUTO_REQUEST */
  {offsetof(macPib_t, battLifeExt), sizeof(bool), FALSE, TRUE},                      /* MAC_BATT_LIFE_EXT */
  {offsetof(macPib_t, battLifeExtPeriods), sizeof(uint8), 6, 6},                     /* MAC_BATT_LIFE_EXT_PERIODS */

  {offsetof(macPib_t, pBeaconPayload), sizeof(uint8 *), 0, 0},                       /* MAC_BEACON_PAYLOAD */
  {offsetof(macPib_t, beaconPayloadLength), sizeof(uint8), 0, MAC_PIB_MAX_BEACON_PAYLOAD_LEN}, /* MAC_BEACON_PAYLOAD_LENGTH */
  {offsetof(macPib_t, beaconOrder), sizeof(uint8), 0, 15},                           /* MAC_BEACON_ORDER */
  {offsetof(macPib_t, beaconTxTime), sizeof(uint32), 1, 1},                          /* MAC_BEACON_TX_TIME */
  {offsetof(macPib_t, bsn), sizeof(uint8), 0x00, 0xFF},                              /* MAC_BSN */

  {offsetof(macPib_t, coordExtendedAddress.addr.extAddr), sizeof(sAddrExt_t), 0, 0}, /* MAC_COORD_EXTENDED_ADDRESS */
  {offsetof(macPib_t, coordShortAddress), sizeof(uint16), 0, 0},                     /* MAC_COORD_SHORT_ADDRESS */
  {offsetof(macPib_t, dsn), sizeof(uint8), 0x00, 0xFF},                              /* MAC_DSN */
  {offsetof(macPib_t, gtsPermit), sizeof(bool), FALSE, TRUE},                        /* MAC_GTS_PERMIT */

  /* Range of maxCsmaBackoffs is between 0 and 5 in IEEE 802.15.4.
   * Such restriction is removed here to allow 802.15.4 non-compliant experimental
   * applications.
   * To be compliant with 802.15.4, application or upper layer must not set this
   * PIB attribute out of range of 802.15.4.
   * The range is still restricted to 254 since value 255 would cause backoff
   * counter to overflow, wrap around and loop infinitely. */
  {offsetof(macPib_t, maxCsmaBackoffs), sizeof(uint8), 0, 254},                      /* MAC_MAX_CSMA_BACKOFFS */

  {offsetof(macPib_t, minBe), sizeof(uint8), 0, 8},                                  /* MAC_MIN_BE */
  {offsetof(macPib_t, panId), sizeof(uint16), 0, 0},                                 /* MAC_PAN_ID */
  {offsetof(macPib_t, promiscuousMode), sizeof(bool), FALSE, TRUE},                  /* MAC_PROMISCUOUS_MODE */
  {offsetof(macPib_t, rxOnWhenIdle), sizeof(bool), FALSE, TRUE},                     /* MAC_RX_ON_WHEN_IDLE */
  {offsetof(macPib_t, shortAddress), sizeof(uint16), 0, 0},                          /* MAC_SHORT_ADDRESS */

  {offsetof(macPib_t, superframeOrder), sizeof(uint8), 0, 15},                       /* MAC_SUPERFRAME_ORDER */
  {offsetof(macPib_t, transactionPersistenceTime), sizeof(uint16), 0, 0},            /* MAC_TRANSACTION_PERSISTENCE_TIME */
  {offsetof(macPib_t, associatedPanCoord), sizeof(bool), FALSE, TRUE},               /* MAC_ASSOCIATED_PAN_COORD */

  /* Range of maxBe is between 3 and 8 in IEEE 802.15.4.
   * Such restriction is removed here to allow 802.15.4 non-compliant experimental
   * applications.
   * To be compliant with 802.15.4, application or upper layer must not set this
   * PIB attribute out of range of 802.15.4. */
  {offsetof(macPib_t, maxBe), sizeof(uint8), 0, 8},                                  /* MAC_MAX_BE */
  {offsetof(macPib_t, maxFrameTotalWaitTime), sizeof(uint16), 0x00, 0xFF},           /* MAC_MAX_FRAME_RESPONSE_TIME */

  {offsetof(macPib_t, maxFrameRetries), sizeof(uint8), 0, 7},                        /* MAC_MAX_FRAME_RETRIES */
  {offsetof(macPib_t, responseWaitTime), sizeof(uint8), 2, 255},                     /* MAC_RESPONSE_WAIT_TIME */
  {offsetof(macPib_t, syncSymbolOffset), sizeof(uint8), 0, 0},                       /* MAC_SYNC_SYMBOL_OFFSET */
  {offsetof(macPib_t, timeStampSupported), sizeof(bool), FALSE, TRUE},               /* MAC_TIMESTAMP_SUPPORTED */
  {offsetof(macPib_t, securityEnabled), sizeof(bool), FALSE, TRUE},                  /* MAC_SECURITY_ENABLED */

  {offsetof(macPib_t, ebsn), sizeof(uint8), 0x00, 0xFF},                             /* MAC_EBSN                */
  {offsetof(macPib_t, eBeaconOrder), sizeof(uint8), 0, 15 },                         /* MAC_EBEACON_ORDER       */
  {offsetof(macPib_t, eBeaconOrderNBPAN), sizeof(uint16), 0x00, 0xFF},               /* MAC_EBEACON_ORDER_NBPAN */
  {offsetof(macPib_t, offsetTimeSlot), sizeof(uint8), 1, 15},                        /* MAC_OFFSET_TIMESLOT     */
  {offsetof(macPib_t, includeMPMIE), sizeof(bool), FALSE, TRUE},                     /* MAC_INCLUDE_MPMIE       */

  {offsetof(macPib_t, fskPreambleLen), sizeof(uint8), MAC_MIN_PREAMBLE_LEN, MAC_MAX_PREAMBLE_LEN},              /* MAC_PHY_FSK_PREAMBLE_LEN */
  {offsetof(macPib_t, mrFSKSFD), sizeof(uint8), 0, 1},                               /* MAC_PHY_MRFSKSFD        */

  /* Proprietary PIBs */
  {offsetof(macPib_t, phyTransmitPower), sizeof(uint8), 0, 0xFF},                   /* MAC_PHY_TRANSMIT_POWER_SIGNED */
  {offsetof(macPib_t, logicalChannel), sizeof(uint8), MAC_CHAN_LOWEST, MAC_CHAN_HIGHEST},     /* MAC_LOGICAL_CHANNEL */
  {offsetof(macPib_t, extendedAddress.addr.extAddr), sizeof(sAddrExt_t), 0, 0},      /* MAC_EXTENDED_ADDRESS */
  {offsetof(macPib_t, altBe), sizeof(uint8), 0, 8},                                  /* MAC_ALT_BE */
  {offsetof(macPib_t, deviceBeaconOrder), sizeof(uint8), 0, 15},                     /* MAC_DEVICE_BEACON_ORDER */
  {offsetof(macPib_t, rf4cepowerSavings), sizeof(uint8), 0, 1},                      /* MAC_RF4CE_POWER_SAVINGS */
  {offsetof(macPib_t, frameVersionSupport), sizeof(uint8), 0, 0xFF},                 /* MAC_FRAME_VERSION_SUPPORT */
#ifndef FREQ_2_4G
  {offsetof(macPib_t, channelPage), sizeof(uint8), 0, 10},                            /* MAC_CHANNEL_PAGE */
  {offsetof(macPib_t, curPhyID), sizeof(uint8), 0, MAC_MRFSK_GENERIC_PHY_ID_END}, /* MAC_PHY_CURRENT_DESCRIPTOR_ID */
#else
  {offsetof(macPib_t, channelPage), sizeof(uint8), 0, 0},                            /* MAC_CHANNEL_PAGE */
  {offsetof(macPib_t, curPhyID), sizeof(uint8), 0, 0},                               /* MAC_PHY_CURRENT_DESCRIPTOR_ID */
#endif
  {offsetof(macPib_t, fcsType), sizeof(bool), FALSE, TRUE },                         /* MAC_FCS_TYPE */

  {offsetof(macPib_t, diagsRxCrcPass), sizeof(uint32), 0, 0},                        /* MAC_DIAGS_RX_CRC_PASS */
  {offsetof(macPib_t, diagsRxCrcFail), sizeof(uint32), 0, 0},                        /* MAC_DIAGS_RX_CRC_FAIL */
  {offsetof(macPib_t, diagsRxBcast), sizeof(uint32), 0, 0},                          /* MAC_DIAGS_RX_BCAST */
  {offsetof(macPib_t, diagsTxBcast), sizeof(uint32), 0, 0},                          /* MAC_DIAGS_TX_BCAST */
  {offsetof(macPib_t, diagsRxUcast), sizeof(uint32), 0, 0},                          /* MAC_DIAGS_RX_UCAST */
  {offsetof(macPib_t, diagsTxUcast), sizeof(uint32), 0, 0},                          /* MAC_DIAGS_TX_UCAST */
  {offsetof(macPib_t, diagsTxUcastRetry), sizeof(uint32), 0, 0},                     /* MAC_DIAGS_TX_UCAST_RETRY */
  {offsetof(macPib_t, diagsTxUcastFail), sizeof(uint32), 0, 0},                      /* MAC_DIAGS_TX_UCAST_FAIL */
  {offsetof(macPib_t, diagsRxSecFail), sizeof(uint32), 0, 0},                        /* MAC_DIAGS_RX_SEC_FAIL */
  {offsetof(macPib_t, diagsTxSecFail), sizeof(uint32), 0, 0},                        /* MAC_DIAGS_TX_SEC_FAIL */
  {offsetof(macPib_t, rssiThreshold), sizeof(int8), 0, 0xFF},                        /* MAC_RSSI_THRESHOLD */
  {offsetof(macPib_t, rangeExt), sizeof(uint8), 0, 1},                               /* MAC_RANGE_EXTENDER */
  {offsetof(macPib_t, enDataAckPending), sizeof(uint8), 0, 1},                       /* DATA ACK PENDING */
#ifndef FREQ_2_4G
  {offsetof(macPib_t, rfFreq), sizeof(uint8), MAC_RF_FREQ_SUBG, MAC_RF_FREQ_2_4G},   /* MAC_RF_BAND */
#else
  {offsetof(macPib_t, rfFreq), sizeof(uint8), MAC_RF_FREQ_2_4G, MAC_RF_FREQ_2_4G},   /* MAC_RF_BAND */
#endif
  {offsetof(macPib_t, paType), sizeof(uint8), 0, 1},                                 /* MAC_RF_PA_TYPE */
};


#if defined(COMBO_MAC) || !defined(FREQ_2_4G)

/* MAC 15.4g Standard PHY Descriptor Table */
CODE const macMRFSKPHYDesc_t macMRFSKStdPhyTable[MAC_STANDARD_PHY_DESCRIPTOR_ENTRIES] =
{
  { MAC_STD_US_915_BAND_MODE_1_CENTER_FREQ_KHZ, MAC_STD_US_915_BAND_MODE_1_CHAN_SPACING_KHZ,
    MAC_STD_US_915_BAND_MODE_1_TOTAL_CHANNELS, MAC_2_FSK_MODULATION, MAC_MRFSK_SYMBOL_RATE_50_K,
  MAC_MODULATION_INDEX_2FSK_50K,
    MAC_CCA_TYPE_CSMA_CA },
  { MAC_STD_US_915_BAND_MODE_2_CENTER_FREQ_KHZ, MAC_STD_US_915_BAND_MODE_2_CHAN_SPACING_KHZ,
    MAC_STD_US_915_BAND_MODE_2_TOTAL_CHANNELS, MAC_2_FSK_MODULATION, MAC_MRFSK_SYMBOL_RATE_150_K,
  MAC_MODULATION_INDEX_2FSK_150K,
    MAC_CCA_TYPE_CSMA_CA },
  { MAC_STD_ETSI_863_BAND_MODE_1_CENTER_FREQ_KHZ, MAC_STD_ETSI_863_BAND_MODE_1_CHAN_SPACING_KHZ,
    MAC_STD_ETSI_863_BAND_MODE_1_TOTAL_CHANNELS, MAC_2_FSK_MODULATION, MAC_MRFSK_SYMBOL_RATE_50_K,
  MAC_MODULATION_INDEX_2FSK_50K,
    MAC_CCA_TYPE_LBT },
};

/* MAC 15.4g Generic PHY Descriptor Table */
macMRFSKPHYDesc_t macMRFSKGenPhyTable[MAC_GENERIC_PHY_DESCRIPTOR_ENTRIES] =
{
 { MAC_GENERIC_CHINA_433_BAND_MODE_1_CENTER_FREQ_KHZ, MAC_GENERIC_CHINA_433_BAND_MODE_1_CHAN_SPACING_KHZ,
   MAC_GENERIC_CHINA_433_BAND_MODE_1_TOTAL_CHANNELS, MAC_2_FSK_MODULATION, MAC_MRFSK_SYMBOL_RATE_50_K,
 MAC_MODULATION_INDEX_2FSK_50K,
   MAC_CCA_TYPE_CSMA_CA },
 { MAC_GENERIC_US_LRM_915_BAND_MODE_1_CENTER_FREQ_KHZ, MAC_GENERIC_US_LRM_915_BAND_MODE_1_CHAN_SPACING_KHZ,
   MAC_GENERIC_US_LRM_915_BAND_MODE_1_TOTAL_CHANNELS, MAC_2_FSK_MODULATION, MAC_MRFSK_SYMBOL_RATE_20_K,
 MAC_MODULATION_INDEX_2FSK_5K,
   MAC_CCA_TYPE_CSMA_CA },
 { MAC_GENERIC_CHINA_LRM_433_BAND_MODE_1_CENTER_FREQ_KHZ, MAC_GENERIC_CHINA_LRM_433_BAND_MODE_1_CHAN_SPACING_KHZ,
   MAC_GENERIC_CHINA_LRM_433_BAND_MODE_1_TOTAL_CHANNELS, MAC_2_FSK_MODULATION, MAC_MRFSK_SYMBOL_RATE_20_K,
 MAC_MODULATION_INDEX_2FSK_5K,
   MAC_CCA_TYPE_CSMA_CA },
 { MAC_GENERIC_ETSI_LRM_863_BAND_MODE_1_CENTER_FREQ_KHZ, MAC_GENERIC_ETSI_LRM_863_BAND_MODE_1_CHAN_SPACING_KHZ,
   MAC_GENERIC_ETSI_LRM_863_BAND_MODE_1_TOTAL_CHANNELS, MAC_2_FSK_MODULATION, MAC_MRFSK_SYMBOL_RATE_20_K,
 MAC_MODULATION_INDEX_2FSK_5K,
   MAC_CCA_TYPE_LBT },
 { MAC_GENERIC_US_915_BAND_MODE_3_CENTER_FREQ_KHZ, MAC_GENERIC_US_915_BAND_MODE_3_CHAN_SPACING_KHZ,
   MAC_GENERIC_US_915_BAND_MODE_3_TOTAL_CHANNELS, MAC_2_FSK_MODULATION, MAC_MRFSK_SYMBOL_RATE_200_K,
 MAC_MODULATION_INDEX_2FSK_200K,
   MAC_CCA_TYPE_CSMA_CA },
 { MAC_GENERIC_ETSI_863_BAND_MODE_2_CENTER_FREQ_KHZ, MAC_GENERIC_ETSI_863_BAND_MODE_2_CHAN_SPACING_KHZ,
   MAC_GENERIC_ETSI_863_BAND_MODE_2_TOTAL_CHANNELS, MAC_2_FSK_MODULATION, MAC_MRFSK_SYMBOL_RATE_200_K,
 MAC_MODULATION_INDEX_2FSK_200K,
   MAC_CCA_TYPE_LBT },
};
#endif
#if defined(COMBO_MAC) || defined(FREQ_2_4G)

/* MAC 15.4 IEEE PHY Descriptor Table */
const macIEEEPHYDesc_t macIEEEPhyTable =
  { MAC_IEEE_CENTER_FREQ_KHZ, MAC_IEEE_CHAN_SPACING_KHZ, MAC_IEEE_TOTAL_CHANNELS,
    0, 0, 0, MAC_CCA_TYPE_CSMA_CA };
#endif

/* MAC PIB */
macPib_t macPib;

macTimerHeader_t timerUnalignedQ;
macTimerHeader_t timerAlignedQ;

int32 macTimerRollover = MAC_SYMBOL_TIMER_DEFAULT_ROLLOVER;
 int32 macTimerNewRollover = MAC_SYMBOL_TIMER_DEFAULT_ROLLOVER;
macTimer_t *macTimerActive;

/* timer rollover table indexed by beacon order */
const uint32 CODE macTimerRolloverValue[] =
{
  MAC_BEACON_INTERVAL(0),
  MAC_BEACON_INTERVAL(1),
  MAC_BEACON_INTERVAL(2),
  MAC_BEACON_INTERVAL(3),
  MAC_BEACON_INTERVAL(4),
  MAC_BEACON_INTERVAL(5),
  MAC_BEACON_INTERVAL(6),
  MAC_BEACON_INTERVAL(7),
  MAC_BEACON_INTERVAL(8),
  MAC_BEACON_INTERVAL(9),
  MAC_BEACON_INTERVAL(10),
  MAC_BEACON_INTERVAL(11),
  MAC_BEACON_INTERVAL(12),
  MAC_BEACON_INTERVAL(13),
  MAC_BEACON_INTERVAL(14),
  MAC_SYMBOL_TIMER_DEFAULT_ROLLOVER
};

//#if defined (FEATURE_BEACON_MODE) || defined (FEATURE_NON_BEACON_MODE)
#if 1
/* Scan action set */
macAction_t macScanAction[6];

macScan_t macScan;
#endif

#ifdef FEATURE_MAC_SECURITY
/* Security related MAC PIB */
macSecurityPib_t macSecurityPib;

#ifdef STATIC_MAC_SECURITY_DEFAULTS
/* Security related PIB default values */
const macSecurityPib_t macSecurityPibDefaults =
{
  0,                                                 /* keyTableEntries */
  0,                                                 /* deviceTableEntries */
  0,                                                 /* securityLevelTableEntries */
  0x00,                                              /* autoRequestSecurityLevel */
  0,                                                 /* autoRequestKeyIdMode */
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},  /* autoRequestKeySource */
  0xFF,                                              /* autoRequestKeyIndex */
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},  /* defaultKeySource */
  {0, SADDR_MODE_EXT},                               /* panCoordExtendedAddress */
  MAC_SHORT_ADDR_NONE,                               /* panCoordShortAddress */
  {                                                  /* macKeyTable */
    {NULL, 0, NULL, 0, NULL, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {NULL, 0, NULL, 0, NULL, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}
  },
  {                                                  /* macKeyIdLookupList */
    {{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00}},
    {{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00}}
  },
  {                                                  /* macKeyDeviceList */
    {{0x00, false, false}, {0x00, false, false}, {0x00, false, false}  /* The rest, if any, will be initialized to zeros */},
    {{0x00, false, false}, {0x00, false, false}, {0x00, false, false}  /* The rest, if any, will be initialized to zeros */}
  },
  {
    {{MAC_FRAME_TYPE_DATA, MAC_DATA_REQ_FRAME},{MAC_FRAME_TYPE_COMMAND, MAC_DATA_REQ_FRAME}}, /* macKeyUsageList */
    {{MAC_FRAME_TYPE_DATA, MAC_DATA_REQ_FRAME},{MAC_FRAME_TYPE_COMMAND, MAC_DATA_REQ_FRAME}}
  },
  {                                                  /* macDeviceTable */
    {0x0000, 0xFFFF, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, 0, 0, FALSE},
    {0x0000, 0xFFFF, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, 0, 0, FALSE},
    {0x0000, 0xFFFF, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, 0, 0, FALSE}
    /* The rest, if any, will be initialized to zeros */
  },
  {                                                   /* macSecurityLevelTable */
    {MAC_FRAME_TYPE_DATA, MAC_DATA_REQ_FRAME, MAC_SEC_LEVEL_ENC_MIC_32, TRUE},
    {MAC_FRAME_TYPE_COMMAND, MAC_DATA_REQ_FRAME, MAC_SEC_LEVEL_ENC_MIC_32, TRUE}
  }
};
#endif

/* PIB access and min/max table.  min/max of 0/0 means not checked; if min/max are
 * equal, element is read-only
 */
const macSecurityPibTbl_t macSecurityPibTbl[] =
{
  {offsetof(macSecurityPib_t, keyTableEntries), sizeof(uint16), 0, MAX_KEY_TABLE_ENTRIES},         /* MAC_KEY_TABLE_ENTRIES */
  {offsetof(macSecurityPib_t, deviceTableEntries), sizeof(uint16), 0, MAX_DEVICE_TABLE_ENTRIES},   /* MAC_DEVICE_TABLE_ENTRIES */
  {offsetof(macSecurityPib_t, securityLevelTableEntries), sizeof(uint8), 0, MAX_SECURITY_LEVEL_TABLE_ENTRIES},
                                                                                                  /* MAC_SECURITY_LEVEL_TABLE_ENTRIES */
  {0, 0, 0, 0},                                                                                   /* MAC_FRAME_COUNTER */
  {offsetof(macSecurityPib_t, autoRequestSecurityLevel), sizeof(uint8), 0, 7},                    /* MAC_AUTO_REQUEST_SECURITY_LEVEL */
  {offsetof(macSecurityPib_t, autoRequestKeyIdMode), sizeof(uint8), 0, 3},                        /* MAC_AUTO_REQUEST_KEY_ID_MODE */
  {offsetof(macSecurityPib_t, autoRequestKeySource), MAC_KEY_SOURCE_MAX_LEN*sizeof(uint8), 0, 0}, /* MAC_AUTO_REQUEST_KEY_SOURCE*/
  {offsetof(macSecurityPib_t, autoRequestKeyIndex), sizeof(uint8), 0x01, 0xFF},                   /* MAC_AUTO_REQUEST_KEY_INDEX */
  {offsetof(macSecurityPib_t, defaultKeySource), MAC_KEY_SOURCE_MAX_LEN*sizeof(uint8), 0, 0},     /* MAC_DEFAULT_KEY_SOURCE */
  {offsetof(macSecurityPib_t, panCoordExtendedAddress), sizeof(sAddrExt_t), 0, 0},                /* MAC_PAN_COORD_EXTENDED_ADDRESS */
  {offsetof(macSecurityPib_t, panCoordShortAddress), sizeof(uint16), 0, 0},                       /* MAC_PAN_COORD_SHORT_ADDRESS */
};


#if defined( FEATURE_MAC_PIB_PTR )

/* Pointer to the mac security PIB */
macSecurityPib_t* pMacSecurityPib = &macSecurityPib;

#endif

/* Crypto driver handle */
#if !defined(CC13X2R1_LAUNCHXL) && !defined(CC26X2R1_LAUNCHXL)
CryptoCC26XX_Handle Crypto_handle;
#else
AESCCM_Handle AESCCM_handle;
#endif

/* Crypto driver function table */
uint32_t *macCryptoDrvTblPtr;

/* Length M of authentication tag indexed by security level */
CODE const uint8 macAuthTagLen[] =
{
  0,                                  /* MAC_SEC_LEVEL_NONE */
  MAC_MIC_32_LEN,                     /* MAC_SEC_LEVEL_MIC_32 */
  MAC_MIC_64_LEN,                     /* MAC_SEC_LEVEL_MIC_64 */
  MAC_MIC_128_LEN,                    /* MAC_SEC_LEVEL_MIC_128 */
  0,                                  /* MAC_SEC_LEVEL_ENC */
  MAC_MIC_32_LEN,                     /* MAC_SEC_LEVEL_ENC_MIC_32 */
  MAC_MIC_64_LEN,                     /* MAC_SEC_LEVEL_ENC_MIC_64 */
  MAC_MIC_128_LEN                     /* MAC_SEC_LEVEL_ENC_MIC_128 */
};

/* Length of key source indexed by key identifier mode */
CODE const uint8 macKeySourceLen[] =
{
  MAC_KEY_ID_IMPLICIT_LEN,            /* MAC_KEY_ID_MODE_IMPLICIT */
  MAC_KEY_ID_1_LEN,                   /* MAC_KEY_ID_MODE_1 */
  MAC_KEY_ID_4_LEN,                   /* MAC_KEY_ID_MODE_4 */
  MAC_KEY_ID_8_LEN                    /* MAC_KEY_ID_MODE_8 */
};
#endif

#if defined (FEATURE_BEACON_MODE) || defined (FEATURE_NON_BEACON_MODE) || defined (STACK_LIBRARY)

uint8 macBeaconPayload[MAC_PIB_MAX_BEACON_PAYLOAD_LEN] = {0};

#endif


#ifndef ENHACK_MALLOC
uint8 enhancedAckBuf[250];
#endif

