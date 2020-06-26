/******************************************************************************

 @file  mac_radio_defs.h

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

#ifndef MAC_RADIO_DEFS_H
#define MAC_RADIO_DEFS_H

/* ------------------------------------------------------------------------------------------------
 *                                             Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "hal_defs.h"
#include "hal_board_cfg.h"
#include "hal_mac_cfg.h"
#include "mac_spec.h"
#include "mac_mcu.h"
#include "mac.h"
#include "mac_low_level.h"

/* ------------------------------------------------------------------------------------------------
 *                                      Target Specific Defines
 * ------------------------------------------------------------------------------------------------
 */

/* MAC RAT frequency in MHz */
#define MAC_RAT_MHZ                    (4)

/* Symbol to RAT ratio 20/0.25 = 80 */
#define MAC_SYMBOL_TO_RAT_RATIO        ((uint32)(macSpecUsecsPerSymbol * MAC_RAT_MHZ))

/* Past time determination window.
 * Note that this value has to be greater than RAT timer past determination
 * window size.
 * RAT timer past determination window should be smaller than 1sec.
 */
#define MAC_BACKOFF_PAST_WINDOW        (1000000ul*MAC_RAT_MHZ)

/* CC13xx rollover cannot exceed 0xFFFFFFFF / 80 = 53687091 or 1073.74 seconds.
 * Past time determination window is also considered */
#define MAC_SYMBOL_MAXIMUM_ROLLOVER                                    \
  ((0xFFFFFFFF - MAC_BACKOFF_PAST_WINDOW)/MAC_SYMBOL_TO_RAT_RATIO)



/* ------------------------------------------------------------------------------------------------
 *                                    Unique Radio Define
 * ------------------------------------------------------------------------------------------------
 */
#define FEATURE_MAC_RADIO_HARDWARE_OVERFLOW_NO_ROLLOVER


/* ------------------------------------------------------------------------------------------------
 *                                    Common Radio Defines
 * ------------------------------------------------------------------------------------------------
 */
#define MAC_RADIO_CHANNEL_DEFAULT               0
#define MAC_RADIO_CHANNEL_INVALID               0xFF
#define MAC_RADIO_TX_POWER_INVALID              0xFFFFFFFF

#define MAC_RADIO_RECEIVER_SENSITIVITY_DBM      -97 /* dBm */
#define MAC_RADIO_RECEIVER_SATURATION_DBM       10  /* dBm */

/* offset applied to hardware RSSI value to get RF power level in dBm units */
#define MAC_RADIO_RSSI_OFFSET                   HAL_MAC_RSSI_OFFSET

/* precise values for small delay on both receive and transmit side */
#define MAC_RADIO_RX_TX_PROP_DELAY_MIN_USEC     3.076  /* usec */
#define MAC_RADIO_RX_TX_PROP_DELAY_MAX_USEC     3.284  /* usec */

/* Number of the maximum steps for the random part of LBT listening period.
   Each step represents 0.5 ms. */
#define MAC_LBT_MAX_RND_STEP                     10  // 5 ms at maximum
/* LBT threshold for 863Mhz band, 88kHz receiver band, 14dBm output */
#define MAC_LBT_THRESHOLD                       -92 // -92 dBm

/* ------------------------------------------------------------------------------------------------
 *                                      Common Radio Macros
 * ------------------------------------------------------------------------------------------------
 */
#define MAC_RADIO_MCU_INIT()                          macMcuInit()

#define MAC_RADIO_TURN_ON_POWER()                     /* Nothing for CC13xx */
#define MAC_RADIO_TURN_OFF_POWER()                    /* Nothing for CC13xx */

#define MAC_RADIO_SET_RX_THRESHOLD(x)                 /* Nothing for CC13xx */
#define MAC_RADIO_ENABLE_RX_THRESHOLD_INTERRUPT()     /* Nothing for CC13xx */
#define MAC_RADIO_DISABLE_RX_THRESHOLD_INTERRUPT()    /* Nothing for CC13xx */
#define MAC_RADIO_CLEAR_RX_THRESHOLD_INTERRUPT_FLAG() /* Nothing for CC13xx */

#define MAC_RADIO_RX_ON()                             macSendReceiveCmd()
#define MAC_RADIO_RXTX_OFF()                          st( macStopCmd(TRUE); )

#define MAC_RADIO_FLUSH_TX_FIFO()                     /* Nothing for CC13xx */

#define MAC_RADIO_SET_CHANNEL(x)                      macSendFsCmd(x)

#define MAC_RADIO_SET_TX_POWER(x)                     /* TODO: Figure out what to do  */

/* MAC frame filtering */
#define MAC_RADIO_SET_PAN_COORDINATOR(b)              SET_FRAME_FILTER_PAN_COORDINATOR( macFrmFilter.filterOpt, b )
#define MAC_RADIO_SET_PAN_ID(x)                       st( macFrmFilter.localPanID = x; )
#define MAC_RADIO_SET_SHORT_ADDR(x)                   st( macFrmFilter.localShortAddr = x; )
#define MAC_RADIO_SET_EXT_ADDR(p)                     MAP_osal_memcpy( macFrmFilter.localExtAddr, p, 8 )
#if defined(FREQ_2_4G) || defined(COMBO_MAC)
#define MAC_RADIO_SET_IEEE_PAN_COORDINATOR(b)         st( RF_cmdIEEERx.frameFiltOpt.bPanCoord = b; )
#define MAC_RADIO_SET_IEEE_PAN_ID(x)                  st( RF_cmdIEEERx.localPanID = x; )
#define MAC_RADIO_SET_IEEE_SHORT_ADDR(x)              st( RF_cmdIEEERx.localShortAddr = x; )
#define MAC_RADIO_SET_IEEE_EXT_ADDR(p)                st( MAP_osal_memcpy( &RF_cmdIEEERx.localExtAddr, p, 8 ); )
#endif

#define MAC_RADIO_REQUEST_ACK_TX_DONE_CALLBACK()      /* nothing required */
#define MAC_RADIO_CANCEL_ACK_TX_DONE_CALLBACK()       /* nothing required */

#define MAC_RADIO_RANDOM_BYTE()                       macMcuRandomByte()
#define MAC_RADIO_RANDOM_WORD()                       macMcuRandomWord()

#define MAC_RADIO_TX_RESET()                          /* Nothing for CC13xx */
#define MAC_RADIO_TX_PREP_CSMA_UNSLOTTED()            macRadioTxPrepCsmaUnslotted()
#define MAC_RADIO_TX_PREP_CSMA_SLOTTED()              macRadioTxPrepCsmaSlotted()
#define MAC_RADIO_TX_PREP_SLOTTED()                   macRadioTxPrepSlotted()
#define MAC_RADIO_TX_PREP_GREEN_POWER()               macRadioTxPrepGreenPower()
#define MAC_RADIO_TX_PREP_LBT(x)                      macRadioTxPrepLbt(x)
#define MAC_RADIO_TX_GO(x)                            macRadioTxGo(x)
#define MAC_RADIO_TX_GO_SLOTTED()                     macRadioTxGoSlotted()
#define MAC_RADIO_TX_GO_SLOTTED_CSMA()                macRadioTxGoSlottedCsma()
#define MAC_RADIO_TX_GO_GREEN_POWER()                 macRadioTxGoGreenPower()
#define MAC_RADIO_TX_GO_LBT()                         macRadioTxGoLbt()

/*-----------ACK Timeout-------------*/
#define MAC_RADIO_TX_REQUEST_ACK_TIMEOUT_CALLBACK()   macRadioTxRequestAckTimeout()
#define MAC_RADIO_TX_CANCEL_ACK_TIMEOUT_CALLBACK()    macRadioTxCancelAckTimeout()

#define MAC_RADIO_TIMER_TICKS_PER_USEC()              MAC_RAT_MHZ
#define MAC_RADIO_TIMER_TICKS_PER_BACKOFF()          (MAC_RAT_MHZ * MAC_SPEC_USECS_PER_BACKOFF)
#define MAC_RADIO_TIMER_TICKS_PER_SYMBOL()           (MAC_RAT_MHZ * MAC_SPEC_USECS_PER_SYMBOL)

#define MAC_RADIO_TIMER_CAPTURE(x)                    macMcuTimerCapture(x)
#define MAC_RADIO_TIMER_FORCE_DELAY(x)                macMcuTimerForceDelay(x)

#define MAC_RADIO_TIMER_SLEEP()                       /* Nothing for CC13xx */
#define MAC_RADIO_TIMER_WAKE_UP()                     /* Nothing for CC13xx */

#define MAC_RADIO_TURN_ON_OSC()                       /* Nothing for CC13xx */
#define MAC_RADIO_TURN_OFF_OSC()                      /* Nothing for CC13xx */

#define MAC_RADIO_CLEAR_SYMBOL_COUNT()                st( MAC_RAT_ADJ = 0xFFFFFFFF - RF_getCurrentTime(); )
#define MAC_RADIO_SYMBOL_COUNT()                      ( (uint32)( RF_getCurrentTime() - macPrevPeriodRatCount ) / MAC_SYMBOL_TO_RAT_RATIO )
#define MAC_RADIO_SYMBOL_CAPTURE(x)                   macMcuOverflowCapture(x)
#define MAC_RADIO_SYMBOL_SET_COUNT(x)                 macMcuOverflowSetCount(x)
#define MAC_RADIO_SYMBOL_SET_COMPARE(x)               macMcuOverflowSetCompare(x)

#define MAC_RADIO_SYMBOL_COMPARE_CLEAR_INTERRUPT()    /* obselete for CC253x and CC13xx */

#define MAC_RADIO_SYMBOL_SET_PERIOD(x)                macMcuOverflowSetPeriod(x)

/*--------Source Matching------------*/
#if defined(FREQ_2_4G) || defined(COMBO_MAC)
#define MAC_RADIO_TURN_ON_RX_FRAME_FILTERING()        st( RF_cmdIEEERx.frameFiltOpt.frameFiltEn = 1; )
#define MAC_RADIO_TURN_OFF_RX_FRAME_FILTERING()       st( RF_cmdIEEERx.frameFiltOpt.frameFiltEn = 0; )
#define MAC_RADIO_TURN_ON_AUTO_ACK()                  st( RF_cmdIEEERx.frameFiltOpt.autoAckEn = 1; )
#define MAC_RADIO_TURN_ON_AUTOPEND()                  st( RF_cmdIEEERx.frameFiltOpt.autoPendEn = 1; )
#define MAC_RADIO_TURN_ON_PEND_DATA_REQ()             st( RF_cmdIEEERx.frameFiltOpt.bPendDataReqOnly = 1; )
#define MAC_RADIO_TURN_ON_PENDING_OR()                st( RF_cmdIEEERx.frameFiltOpt.defaultPend = 1; )
#define MAC_RADIO_TURN_OFF_PENDING_OR()               st( RF_cmdIEEERx.frameFiltOpt.defaultPend = 0; )
#define MAC_RADIO_SRC_MATCH_GET_EN(x)                 macSrcMatchGetEnableBit(x)
#define MAC_RADIO_SRC_MATCH_GET_PENDEN(x)             macSrcMatchGetPendEnBit(x)
#define MAC_RADIO_GET_SRC_SHORTEN(x)                  st( x = macSrcShortAddrList.srcMatchEn;  )
#define MAC_RADIO_GET_SRC_EXTEN(x)                    st( x = macSrcExtAddrList.srcMatchEn;  )
#define MAC_RADIO_GET_SRC_SHORTPENDEN(x)              st( x = macSrcShortAddrList.srcPendEn; )
#define MAC_RADIO_GET_SRC_EXTENPEND(x)                st( x = macSrcExtAddrList.srcPendEn; )
#define MAC_RADIO_SRC_MATCH_TABLE_SHORT_READ(offset, p, len) st( MAP_osal_memcpy( (p), (macSrcShortAddrList.shortAddrEntry + (offset)), (len)); )
#define MAC_RADIO_SRC_MATCH_TABLE_EXT_READ(offset, p, len)   st( MAP_osal_memcpy( (p), (macSrcExtAddrList.extAddrEntry + (offset)), (len)); )
#define MAC_RADIO_SRC_MATCH_TABLE_SHORT_WRITE(offset, p, len) st ( MAP_osal_memcpy( (macSrcShortAddrList.shortAddrEntry + (offset)), (p), (len) ); )
#define MAC_RADIO_SRC_MATCH_TABLE_EXT_WRITE(offset, p, len)   st ( MAP_osal_memcpy( (macSrcExtAddrList.extAddrEntry + (offset)), (p), (len) ); )
#define MAC_RADIO_SRC_MATCH_SET_SHORTPENDEN(x)        st( macSrcShortAddrList.srcPendEn = x; )
#define MAC_RADIO_SRC_MATCH_SET_EXTPENDEN(x)          st( macSrcExtAddrList.srcPendEn = x; )
#define MAC_RADIO_SRC_MATCH_SET_SHORTEN(x)            st( macSrcShortAddrList.srcMatchEn = x; )
#define MAC_RADIO_SRC_MATCH_SET_EXTEN(x)              st( macSrcExtAddrList.srcMatchEn = x; )
#define MAC_RADIO_SRC_MATCH_RESULT()                  MAC_SrcMatchCheckResult()
#define MAC_RADIO_GET_RADIO_STATUS(x)                 st( x = RF_cmdIEEERx.status; )
#define MAC_RADIO_TURN_ON_SRC_MATCH()                 /* Nothing to do for CC26xx */
#endif

/*-----------ACK Timer---------------*/
#define MAC_RADIO_REQUEST_TX_ACK()                    macRadioTxRequestTxAck()
#define MAC_RADIO_CANCEL_TX_ACK()                     macRadioTxCancelTxAck()

/* ----------MAC Symbols-------------*/

/* The number of symbols forming a basic CSMA-CA time period */
#define MAC_A_UNIT_BACKOFF_PERIOD                     (macUnitBackoffPeriod)

/* The number of symbols for overall backoff */
#define MAC_A_UNIT_BACKOFF_CCA_PERIOD                 (MAC_A_UNIT_BACKOFF_PERIOD + MAC_A_CCA_TIME)

/* Microseconds in one symbol */
#define MAC_SPEC_USECS_PER_SYMBOL                     (macSpecUsecsPerSymbol)

/* Microseconds in one backoff period */
#define MAC_SPEC_USECS_PER_BACKOFF                    (macSpecUsecsPerSymbol * MAC_A_UNIT_BACKOFF_PERIOD)

/* Turnaround time to send ACK in symbols */
#define MAC_ACK_TIMEOUT_ADJ                           (2400)

#define MAC_AIFS_PERIOD                               (1000/ macSpecUsecsPerSymbol)

#define MAC_AIFS_ADJ_PERIOD                           ((1000 + MAC_ACK_TIMEOUT_ADJ)/ macSpecUsecsPerSymbol)


/* ------------------------------------------------------------------------------------------------
 *                                    Common Radio Externs
 * ------------------------------------------------------------------------------------------------
 */


/**************************************************************************************************
 */
#endif
