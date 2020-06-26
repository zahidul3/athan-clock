/******************************************************************************

 @file icall_osal_map_direct.h

 @brief ICALL OSAL API directly map the function implementation (declaration)

 Group: WCS LPC
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2016-2018, Texas Instruments Incorporated
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

#ifndef ICALL_OSAL_MAP_DIRECT_H
#define ICALL_OSAL_MAP_DIRECT_H

#ifndef OSAL_PORT2TIRTOS

#include "osal.h"
#include "saddr.h"
#include "osal_pwrmgr.h"

#else

#include "osal_port.h"
#include "saddr.h"

#ifdef USE_DMM
// Include Status macros from BLE for DMM projects to avoid duplicates
#include "comdef.h"
#endif

#include <ti/sysbios/knl/Clock.h>

#include <stdlib.h>

#endif /* !OSAL_PORT2TIRTOS */

#ifndef TIMAC_ROM_IMAGE_BUILD

#ifndef OSAL_PORT2TIRTOS

/*
**  ICALL OSAL API directly map to implementation
**  This is used in CC131X/CC135X project with using any TIMAC 15.4 ROM image
*/

#define MAP_osal_mem_alloc                              osal_mem_alloc
#define MAP_osal_mem_free                               osal_mem_free
#define MAP_osal_memcmp                                 osal_memcmp
#define MAP_osal_memcpy                                 osal_memcpy
#define MAP_osal_memset                                 osal_memset
#define MAP_osal_msg_allocate                           osal_msg_allocate
#define MAP_osal_msg_deallocate                         osal_msg_deallocate
#define MAP_osal_msg_dequeue                            osal_msg_dequeue
#define MAP_osal_msg_enqueue                            osal_msg_enqueue
#define MAP_osal_msg_enqueue_max                        osal_msg_enqueue_max
#define MAP_osal_msg_extract                            osal_msg_extract
#define MAP_osal_msg_push                               osal_msg_push
#define MAP_osal_msg_receive                            osal_msg_receive
#define MAP_osal_msg_send                               osal_msg_send
#define MAP_osal_pwrmgr_task_state                      osal_pwrmgr_task_state
#define MAP_osal_set_event                              osal_set_event
#define MAP_osal_strlen                                 osal_strlen
#define MAP_memcmp                                      memcmp
#define MAP_osal_build_uint32                           osal_build_uint32

#define MAP_ICall_getTickPeriod                         ICall_getTickPeriod
#define MAP_ICall_getTicks                              ICall_getTicks
#define MAP_ICall_setTimer                              ICall_setTimer
#define MAP_ICall_stopTimer                             ICall_stopTimer

#define MAP_sAddrCmp                                    sAddrCmp
#define MAP_sAddrCpy                                    sAddrCpy
#define MAP_sAddrExtCpy                                 sAddrExtCpy
#define MAP_sAddrExtCmp                                 sAddrExtCmp

#define MAP_OSAL_MSG_NEXT                               OSAL_MSG_NEXT
#define MAP_OSAL_MSG_Q_INIT                             OSAL_MSG_Q_INIT
#define MAP_OSAL_MSG_Q_EMPTY                            OSAL_MSG_Q_EMPTY
#define MAP_OSAL_MSG_Q_HEAD                             OSAL_MSG_Q_HEAD
#define MAP_OSAL_MSG_LEN                                OSAL_MSG_LEN
#define MAP_OSAL_MSG_ID                                 OSAL_MSG_ID

#define MAP_PWRMGR_HOLD                                 PWRMGR_HOLD
#define MAP_PWRMGR_CONSERVE                             PWRMGR_CONSERVE

#define MAP_ICall_TimerCback                            ICall_TimerCback
#define MAP_ICall_TimerID                               ICall_TimerID
#define MAP_osal_msg_q_t                                osal_msg_q_t
#define MAP_osal_msg_hdr_t                              osal_msg_hdr_t
#define MAP_osal_event_hdr_t                            osal_event_hdr_t

#else

#ifndef USE_DMM
#define SUCCESS                 OsalPort_SUCCESS
#define FAILURE                 OsalPort_FAILURE
#define INVALIDPARAMETER        OsalPort_INVALIDPARAMETER
#define INVALID_TASK            OsalPort_INVALID_TASK
#define MSG_BUFFER_NOT_AVAIL    OsalPort_MSG_BUFFER_NOT_AVAIL
#define INVALID_MSG_POINTER     OsalPort_INVALID_MSG_POINTER
#define INVALID_EVENT_ID        OsalPort_INVALID_EVENT_ID
#define INVALID_INTERRUPT_ID    OsalPort_INVALID_INTERRUPT_ID
#define NO_TIMER_AVAIL          OsalPort_NO_TIMER_AVAIL
#define NV_ITEM_UNINIT          OsalPort_NV_ITEM_UNINIT
#define NV_OPER_FAILED          OsalPort_NV_OPER_FAILED
#define INVALID_MEM_SIZE        OsalPort_INVALID_MEM_SIZE
#define NV_BAD_ITEM_LEN         OsalPort_NV_BAD_ITEM_LEN
#define TASK_NO_TASK            OsalPort_TASK_NO_TASK
#define SYS_EVENT_MSG           OsalPort_SYS_EVENT_MSG
#endif


/*
**  ICALL OSAL API directly map to implementation
**  This is used in CC131X/CC135X project with using any TIMAC 15.4 ROM image
*/
#define MAP_osal_mem_alloc                               OsalPort_malloc
#define MAP_osal_mem_free                                free
#define MAP_osal_memcmp                                  OsalPort_memcmp
#define MAP_osal_memcpy                                  memcpy
#define MAP_osal_memset                                  memset
#define MAP_osal_msg_allocate                            OsalPort_msgAllocate
#define MAP_osal_msg_deallocate                          OsalPort_msgDeallocate
#define MAP_osal_msg_dequeue                             OsalPort_msgDequeue
#define MAP_osal_msg_enqueue                             OsalPort_msgEnqueue
#define MAP_osal_msg_enqueue_max                         OsalPort_msgEnqueueMax
#define MAP_osal_msg_extract                             OsalPort_msgExtract
#define MAP_osal_msg_push                                OsalPort_msgPush
#define MAP_osal_msg_receive                             OsalPort_msgReceive
#define MAP_osal_msg_send                                OsalPort_msgSend
#define MAP_osal_pwrmgr_task_state                       OsalPort_pwrmgrTaskState
#define MAP_osal_set_event                               OsalPort_setEvent
#define MAP_osal_strlen                                  strlen
#define MAP_memcmp                                       memcmp
#define MAP_osal_build_uint32                            OsalPort_buildUint32

#define MAP_ICall_getTicks                              Clock_getTicks
#define MAP_ICall_setTimer                              OsalPort_setTimer
#define MAP_ICall_stopTimer                             OsalPort_stopTimer
#define MAP_ICall_getTickPeriod()                       Clock_tickPeriod

#define MAP_sAddrCmp                                    sAddrCmp
#define MAP_sAddrCpy                                    sAddrCpy
#define MAP_sAddrExtCpy                                 sAddrExtCpy
#define MAP_sAddrExtCmp                                 sAddrExtCmp

#define MAP_OSAL_MSG_NEXT                               OsalPort_MSG_NEXT
#define MAP_OSAL_MSG_Q_INIT                             OsalPort_MSG_Q_INIT
#define MAP_OSAL_MSG_Q_EMPTY                            OsalPort_MSG_Q_EMPTY
#define MAP_OSAL_MSG_Q_HEAD                             OsalPort_MSG_Q_HEAD
#define MAP_OSAL_MSG_LEN                                OsalPort_MSG_LEN
#define MAP_OSAL_MSG_ID                                 OsalPort_MSG_ID

#define MAP_PWRMGR_HOLD                                 OsalPort_PWR_HOLD
#define MAP_PWRMGR_CONSERVE                             OsalPort_PWR_CONSERVE

#define MAP_ICall_TimerCback                            OsalPort_TimerCback
#define MAP_ICall_TimerID                               OsalPort_TimerID
#define MAP_osal_msg_q_t                                OsalPort_MsgQ
#define MAP_osal_msg_hdr_t                              OsalPort_MsgHdr
#define MAP_osal_event_hdr_t                            OsalPort_EventHdr

#endif /* !OSAL_PORT2TIRTOS */

#endif /* TIMAC_ROM_IMAGE_BUILD */

#endif
