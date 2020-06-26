/******************************************************************************

 @file radio.c

 @brief TIRTOS platform specific radio functions for OpenThread

 Group: CMCU, LPRF
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2017-2018, Texas Instruments Incorporated
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

#include <openthread/config.h>

#include "radio.h"

#include <assert.h>
#include <openthread/diag.h>
#include <openthread/openthread.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/diag.h>
#include <openthread/platform/radio.h>
#include <openthread/platform/random.h> /* to seed the CSMA function */
#include <openthread/types.h>
#include <utils/code_utils.h>
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/chipinfo.h)
#include DeviceFamily_constructPath(driverlib/rf_data_entry.h)
#include DeviceFamily_constructPath(driverlib/rf_common_cmd.h)
#include DeviceFamily_constructPath(driverlib/rf_mailbox.h)
#include DeviceFamily_constructPath(driverlib/rf_ieee_cmd.h)
#include DeviceFamily_constructPath(driverlib/rf_ieee_mailbox.h)
#include DeviceFamily_constructPath(inc/hw_ccfg.h)
#include DeviceFamily_constructPath(inc/hw_fcfg1.h)
#include DeviceFamily_constructPath(rf_patches/rf_patch_cpe_ieee_802_15_4.h)
#include DeviceFamily_constructPath(rf_patches/rf_patch_mce_ieee_802_15_4.h)

#include <ti/drivers/PIN.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/rf/RF.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/sysbios/knl/Event.h>
#include <xdc/runtime/Types.h>

#include "platform.h"
#include "Board.h"

/* The sync word used by the radio TX test command */
#define PLATFORM_RADIO_TX_TEST_SYNC_WORD 0x71764129

/* word sent by the TX test command in modulated mode
 * 0b10101010101010101010
 */
#define PLATFORM_RADIO_TX_TEST_MODULATED_WORD 0xAAAA

/* Word sent by the TX test command in unmodulated mode
 * 0b11111111111111111111
 */
#define PLATFORM_RADIO_TX_TEST_UNMODULATED_WORD 0xFFFF

static RF_TxPowerTable_Value sCurrentOutputPower;

/* forward declaration for startTransmit */
static RF_CmdHandle startTransmit(RF_Handle aRfHandle);

/* state of the RF interface */
static volatile platformRadio_phyState sState;

/* Sticky TxDone flag to handle certian race condition */
static bool sTxIsDone;

/* TI-RTOS events structure for passing state to the processing loop */
static Event_Struct rfEvents;

/* Control flag to disable channel switching by the stack. This is used by
 * `rfCoreDiagChannelDisable` and `rfCoreDiagChannelEnable`.
 */
static bool sDisableChannel = false;

/* Overrides from SmartRF Studio 7 2.10.0#94 */
static uint32_t sIEEEOverrides[] =
{
    // override_ieee_802_15_4.xml
    // PHY: Use MCE RAM patch, RFE ROM bank 1
    MCE_RFE_OVERRIDE(1,0,0,0,1,0),
    // Synth: Use 48 MHz crystal, enable extra PLL filtering
    (uint32_t)0x02400403,
    // Synth: Configure extra PLL filtering
    (uint32_t)0x001C8473,
    // Synth: Configure synth hardware
    (uint32_t)0x00088433,
    // Synth: Set minimum RTRIM to 3
    (uint32_t)0x00038793,
    // Synth: Configure faster calibration
    HW32_ARRAY_OVERRIDE(0x4004,1),
    // Synth: Configure faster calibration
    (uint32_t)0x1C0C0618,
    // Synth: Configure faster calibration
    (uint32_t)0xC00401A1,
    // Synth: Configure faster calibration
    (uint32_t)0x00010101,
    // Synth: Configure faster calibration
    (uint32_t)0xC0040141,
    // Synth: Configure faster calibration
    (uint32_t)0x00214AD3,
    // Synth: Decrease synth programming time-out (0x0298 RAT ticks = 166 us)
    (uint32_t)0x02980243,
    // DC/DC regulator: In Tx, use DCDCCTL5[3:0]=0xC (DITHER_EN=1 and IPEAK=4). In Rx, use DCDCCTL5[3:0]=0xC (DITHER_EN=1 and IPEAK=4).
    (uint32_t)0xFCFC08C3,
    // Rx: Set LNA bias current offset to +15 to saturate trim to max (default: 0)
    (uint32_t)0x000F8883,
    // override_frontend_id.xml
    (uint32_t)0xFFFFFFFF,
};

/**
 * TX Power dBm lookup table from SmartRF Studio 7 2.10.0#94
 */
RF_TxPowerTable_Entry txPowerTable[] =
{
    {-21, RF_TxPowerTable_DEFAULT_PA_ENTRY(7, 3, 0, 3)    },
    {-18, RF_TxPowerTable_DEFAULT_PA_ENTRY(9, 3, 0, 3)    },
    {-15, RF_TxPowerTable_DEFAULT_PA_ENTRY(12, 2, 0, 100) },
    {-12, RF_TxPowerTable_DEFAULT_PA_ENTRY(40, 2, 0, 8)   },
    {-10, RF_TxPowerTable_DEFAULT_PA_ENTRY(12, 2, 0, 11)  },
    {-9,  RF_TxPowerTable_DEFAULT_PA_ENTRY(13, 2, 0, 5)   },
    {-6,  RF_TxPowerTable_DEFAULT_PA_ENTRY(13, 1, 0, 16)  },
    {-5,  RF_TxPowerTable_DEFAULT_PA_ENTRY(14, 1, 0, 17)  },
    {-3,  RF_TxPowerTable_DEFAULT_PA_ENTRY(17, 1, 0, 20)  },
    {0,   RF_TxPowerTable_DEFAULT_PA_ENTRY(25, 1, 0, 26)  },
    {1,   RF_TxPowerTable_DEFAULT_PA_ENTRY(28, 1, 0, 28)  },
    {2,   RF_TxPowerTable_DEFAULT_PA_ENTRY(13, 0, 0, 34)  },
    {3,   RF_TxPowerTable_DEFAULT_PA_ENTRY(17, 0, 0, 42)  },
    {4,   RF_TxPowerTable_DEFAULT_PA_ENTRY(22, 0, 0, 54)  },
    {5,   RF_TxPowerTable_DEFAULT_PA_ENTRY(30, 0, 0, 74)  },
    RF_TxPowerTable_TERMINATION_ENTRY
};

/*
 * Number of retry counts left to the currently transmitting frame.
 *
 * Initialized when a frame is passed to be sent over the air, and decremented
 * by the radio ISR every time the transmit command string fails to receive a
 * corresponding ack.
 */
static volatile unsigned int sTransmitRetryCount = 0;

/**
 * This structure is used when processing the RX queue entries.
 * Rather then pass around many parameters, they all collected into a
 * single structure and a reference is passed.
 */
struct rx_queue_info {

    /* Openthread instance for the OpenThread callback */
    otInstance *aInstance;

    /* The events being processed */
    UInt events;

    /* The RX Frame to pass to OpenThread */
    otRadioFrame receiveFrame;

    /* The payload of the rx frame */
    uint8_t *payload;

    /* Points to the rx queue entry being processed */
    rfc_dataEntryGeneral_t *curEntry;

    /* The first entry processed, used to terminate the loop */
    rfc_dataEntryGeneral_t *startEntry;
};

/*
 * Radio command structures that run on the CM0.
 */
static volatile rfc_CMD_RADIO_SETUP_t        sRadioSetupCmd;

static volatile rfc_CMD_IEEE_MOD_FILT_t      sModifyReceiveFilterCmd;
static volatile rfc_CMD_IEEE_MOD_SRC_MATCH_t sModifyReceiveSrcMatchCmd;

static volatile rfc_CMD_IEEE_ED_SCAN_t       sEdScanCmd;

static volatile rfc_CMD_TX_TEST_t            sTxTestCmd;

static volatile rfc_CMD_IEEE_RX_t            sReceiveCmd;

static volatile rfc_CMD_IEEE_CSMA_t          sCsmaBackoffCmd;
static volatile rfc_CMD_IEEE_TX_t            sTransmitCmd;
static volatile rfc_CMD_IEEE_RX_ACK_t        sTransmitRxAckCmd;

static volatile ext_src_match_data_t         sSrcMatchExtData;
static volatile short_src_match_data_t       sSrcMatchShortData;

/* struct containing radio stats */
static volatile rfc_ieeeRxOutput_t sRfStats;

/*
 * Four receive buffers entries with room for 1 max IEEE802.15.4 frame in each
 *
 * These will be setup in a circular buffer configuration by /ref sRxDataQueue.
 */
#define RX_BUF_SIZE 144
static __attribute__((aligned(4))) uint8_t sRxBuf0[RX_BUF_SIZE];
static __attribute__((aligned(4))) uint8_t sRxBuf1[RX_BUF_SIZE];
static __attribute__((aligned(4))) uint8_t sRxBuf2[RX_BUF_SIZE];
static __attribute__((aligned(4))) uint8_t sRxBuf3[RX_BUF_SIZE];

/*
 * The RX Data Queue used by @ref sReceiveCmd.
 */
static __attribute__((aligned(4))) dataQueue_t sRxDataQueue = { 0 };

/* openthread data primitives */
static otRadioFrame sTransmitFrame;
static otError      sTransmitError;

static __attribute__ ((aligned(4))) uint8_t sTransmitPsdu[OT_RADIO_FRAME_MAX_SIZE];

static RF_Object sRfObject;

/* this can be const, but we receive a compilation warning */
static RF_Mode sRfMode = {
    .rfMode      = RF_MODE_AUTO,
    .cpePatchFxn = rf_patch_cpe_ieee_802_15_4,
    .mcePatchFxn = rf_patch_mce_ieee_802_15_4,
    .rfePatchFxn = NULL,
};

static RF_Handle sRfHandle;

static RF_CmdHandle sReceiveCmdHandle;
static RF_CmdHandle sTransmitCmdHandle;
static RF_CmdHandle sTxTestCmdHandle;

/* seed the radio random using random from TRNG */
static uint16_t seedRandom;

/**
 * @brief Post a Radio Signal
 *
 * Some Radio event has occurred, wake the process loops.
 *
 * @param evts [in] events to post
 */
static void radioSignal(UInt evts)
{
    if (evts != Event_Id_NONE)
    {
        /* our events, for our handler */
        Event_post(Event_handle(&rfEvents), evts);
        /* then post a main loop event */
        platformRadioSignal();
    }
}

/**
 * @brief initialize the RX/TX buffers
 *
 * Zeros out the receive and transmit buffers and sets up the data structures
 * of the receive queue.
 */
static void rfCoreInitBufs(void)
{
    rfc_dataEntry_t *entry;
    memset(sRxBuf0, 0x00, sizeof(sRxBuf0));
    memset(sRxBuf1, 0x00, sizeof(sRxBuf1));
    memset(sRxBuf2, 0x00, sizeof(sRxBuf2));
    memset(sRxBuf3, 0x00, sizeof(sRxBuf3));

    entry               = (rfc_dataEntry_t *)sRxBuf0;
    entry->pNextEntry   = sRxBuf1;
    entry->config.lenSz = DATA_ENTRY_LENSZ_BYTE;
    entry->length       = sizeof(sRxBuf0) - sizeof(rfc_dataEntry_t);

    entry               = (rfc_dataEntry_t *)sRxBuf1;
    entry->pNextEntry   = sRxBuf2;
    entry->config.lenSz = DATA_ENTRY_LENSZ_BYTE;
    entry->length       = sizeof(sRxBuf1) - sizeof(rfc_dataEntry_t);

    entry               = (rfc_dataEntry_t *)sRxBuf2;
    entry->pNextEntry   = sRxBuf3;
    entry->config.lenSz = DATA_ENTRY_LENSZ_BYTE;
    entry->length       = sizeof(sRxBuf2) - sizeof(rfc_dataEntry_t);

    entry               = (rfc_dataEntry_t *)sRxBuf3;
    entry->pNextEntry   = sRxBuf0;
    entry->config.lenSz = DATA_ENTRY_LENSZ_BYTE;
    entry->length       = sizeof(sRxBuf3) - sizeof(rfc_dataEntry_t);

    sRxDataQueue.pCurrEntry = sRxBuf0;
    sRxDataQueue.pLastEntry = NULL;

    sTransmitFrame.mPsdu   = sTransmitPsdu;
    sTransmitFrame.mLength = 0;
}

/**
 * @brief Sets the transmit.
 *
 * Sets the transmit power within the radio setup command or the override list.
 */
otError rfCoreSetTransmitPower(RF_TxPowerTable_Value powerCfg)
{
    otError               retval = OT_ERROR_NONE;

    /* The cc2652r does not have an internal PA, and doesn't need code to
     * switch.
     */
    sCurrentOutputPower = powerCfg;
    RF_setTxPower(sRfHandle, powerCfg);

    return retval;
}

/**
 * @brief initializes the setup command structure
 *
 * The sRadioSetupCmd struct is used by the RF driver to bring the
 */
void rfCoreInitSetupCmd(void)
{
    static const rfc_CMD_RADIO_SETUP_t cRadioSetupCmd =
    {
        .commandNo                  = CMD_RADIO_SETUP,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
            .pastTrig               = 1, // XXX: workaround for RF scheduler
        },
        .condition                  =
        {
            .rule                   = COND_NEVER,
        },
        .mode                       = 1, // IEEE 802.15.4 mode
        .loDivider                  = 0x00,        
        .config                     =
        {
            .biasMode               = 0x0,
            .frontEndMode           = 0x0,
            .analogCfgMode          = 0x0,
            .bNoFsPowerUp           = 0x0,
        },

    };

    sCurrentOutputPower = RF_TxPowerTable_findValue(txPowerTable,
                                                    RF_TxPowerTable_MAX_DBM);

    /* initialize radio setup command */
    sRadioSetupCmd              = cRadioSetupCmd;
    /* initially set the radio tx power to the max */
    sRadioSetupCmd.pRegOverride = sIEEEOverrides;
}

/**
 * @brief initialize the RX command structure
 *
 * Sets the default values for the receive command structure.
 */
static void rfCoreInitReceiveParams(void)
{
    static const rfc_CMD_IEEE_RX_t cReceiveCmd =
    {
        .commandNo                  = CMD_IEEE_RX,
        .status                     = IDLE,
        .pNextOp                    = NULL,
        .startTime                  = 0u,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
            .pastTrig               = 1, // XXX: workaround for RF scheduler
        },
        .condition                  =
        {
            .rule                   = COND_NEVER,
        },
        .channel                    = OT_RADIO_CHANNEL_MIN,
        .rxConfig                   =
        {
            .bAutoFlushCrc          = 1,
            .bAutoFlushIgn          = 0,
            .bIncludePhyHdr         = 0,
            .bIncludeCrc            = 0,
            .bAppendRssi            = 1,
            .bAppendCorrCrc         = 1,
            .bAppendSrcInd          = 0,
            .bAppendTimestamp       = 0,
        },
        .frameFiltOpt               =
        {
            .frameFiltEn            = 1,
            .frameFiltStop          = 1,
            .autoAckEn              = 1,
            .slottedAckEn           = 0,
            .autoPendEn             = 0,
            .defaultPend            = 0,
            .bPendDataReqOnly       = 0,
            .bPanCoord              = 0,
            .maxFrameVersion        = 3,
            .bStrictLenFilter       = 1,
        },
        .frameTypes                 =
        {
            .bAcceptFt0Beacon       = 1,
            .bAcceptFt1Data         = 1,
            .bAcceptFt2Ack          = 1,
            .bAcceptFt3MacCmd       = 1,
            .bAcceptFt4Reserved     = 1,
            .bAcceptFt5Reserved     = 1,
            .bAcceptFt6Reserved     = 1,
            .bAcceptFt7Reserved     = 1,
        },
        .ccaOpt                     =
        {
            .ccaEnEnergy            = 1,
            .ccaEnCorr              = 1,
            .ccaEnSync              = 1,
            .ccaCorrOp              = 1,
            .ccaSyncOp              = 0,
            .ccaCorrThr             = 3,
        },
        .ccaRssiThr                 = -90,
        .endTrigger                 =
        {
            .triggerType            = TRIG_NEVER,
        },
        .endTime                    = 0U,
    };
    sReceiveCmd = cReceiveCmd;

    sReceiveCmd.pRxQ            = &sRxDataQueue;
    sReceiveCmd.pOutput         = (rfc_ieeeRxOutput_t *) &sRfStats;

    sReceiveCmd.numShortEntries = PLATFORM_RADIO_SHORTADD_SRC_MATCH_NUM;
    sReceiveCmd.pShortEntryList = (void *) &sSrcMatchShortData;

    sReceiveCmd.numExtEntries   = PLATFORM_RADIO_EXTADD_SRC_MATCH_NUM;
    sReceiveCmd.pExtEntryList   = (uint32_t *) &sSrcMatchExtData;
}

/**
 * @brief Get the receive command's sensitivity.
 */
int8_t otPlatRadioGetReceiveSensitivity(otInstance *aInstance)
{
    (void)aInstance;
    return sReceiveCmd.ccaRssiThr;
}

/**
 * @brief initialize the TI-RTOS Event Object
 *
 * Constructs the Event Object for messaging between the command handlers and
 * the processing loop.
 */
static void rfCoreInitEvent(void)
{
    Event_construct(&rfEvents, NULL);
}

/**
 * @brief sends the direct abort command to the radio core
 *
 * @param [in] aRfHandle    The handle for the RF core client
 * @param [in] aRfCmdHandle The command handle to be aborted
 *
 * @return the value from the command status register
 * @retval RF_StatSuccess the command completed correctly
 */
static RF_Stat rfCoreExecuteAbortCmd(RF_Handle aRfHandle,
        RF_CmdHandle aRfCmdHandle)
{
    return RF_cancelCmd(aRfHandle, aRfCmdHandle, RF_DRIVER_ABORT);
}

/**
 * @brief enable/disable filtering
 *
 * Uses the radio core to alter the current running RX command filtering
 * options. This ensures there is no access fault between the CM3 and CM0 for
 * the RX command.
 *
 * This function leaves the type of frames to be filtered the same as the
 * receive command.
 *
 * @note An IEEE RX command *must* be running while this command executes.
 *
 * @param [in] aRfHandle The handle for the RF core client
 * @param [in] aEnable   TRUE: enable frame filtering
 *                       FALSE: disable frame filtering
 *
 * @return the value from the command status register
 * @retval RF_StatCmdDoneSuccess the command completed correctly
 */
static RF_Stat rfCoreModifyRxFrameFilter(RF_Handle aRfHandle, bool aEnable)
{
    /* memset skipped because sModifyReceiveFilterCmd has only 3 members */
    sModifyReceiveFilterCmd.commandNo = CMD_IEEE_MOD_FILT;
    /* copy current frame filtering and frame types from running RX command */
    memcpy((void *)&sModifyReceiveFilterCmd.newFrameFiltOpt, (void *)&sReceiveCmd.frameFiltOpt,
           sizeof(sModifyReceiveFilterCmd.newFrameFiltOpt));
    memcpy((void *)&sModifyReceiveFilterCmd.newFrameTypes, (void *)&sReceiveCmd.frameTypes,
           sizeof(sModifyReceiveFilterCmd.newFrameTypes));

    sModifyReceiveFilterCmd.newFrameFiltOpt.frameFiltEn = aEnable ? 1 : 0;

    return RF_runImmediateCmd(aRfHandle, (uint32_t *)&sModifyReceiveFilterCmd);
}

/**
 * @brief enable/disable autoPend
 *
 * Uses the radio core to alter the current running RX command filtering
 * options. This ensures there is no access fault between the CM3 and CM0 for
 * the RX command.
 *
 * This function leaves the type of frames to be filtered the same as the
 * receive command.
 *
 * @note An IEEE RX command *must* be running while this command executes.
 *
 * @param [in] aRfHandle The handle for the RF core client
 * @param [in] aEnable TRUE: enable autoPend, FALSE: disable autoPend
 *
 * @return the value from the command status register
 * @retval RF_StatCmdDoneSuccess the command completed correctly
 */
static RF_Stat rfCoreModifyRxAutoPend(RF_Handle aRfHandle, bool aEnable)
{
    /* memset skipped because sModifyReceiveFilterCmd has only 3 members */
    sModifyReceiveFilterCmd.commandNo = CMD_IEEE_MOD_FILT;
    /* copy current frame filtering and frame types from running RX command */
    memcpy((void *)&sModifyReceiveFilterCmd.newFrameFiltOpt, (void *)&sReceiveCmd.frameFiltOpt,
           sizeof(sModifyReceiveFilterCmd.newFrameFiltOpt));
    memcpy((void *)&sModifyReceiveFilterCmd.newFrameTypes, (void *)&sReceiveCmd.frameTypes,
           sizeof(sModifyReceiveFilterCmd.newFrameTypes));

    sModifyReceiveFilterCmd.newFrameFiltOpt.autoPendEn = aEnable ? 1 : 0;

    return RF_runImmediateCmd(aRfHandle, (uint32_t *)&sModifyReceiveFilterCmd);
}

/**
 * @brief sends the immediate modify source matching command to the radio core
 *
 * Uses the radio core to alter the current source matching parameters used by
 * the running RX command. This ensures there is no access fault between the
 * CM3 and CM0, and ensures that the RX command has cohesive view of the data.
 * The CM3 may make alterations to the source matching entries if the entry is
 * marked as disabled.
 *
 * @note An IEEE RX command *must* be running while this command executes.
 *
 * @param [in] aRfHandle The handle for the RF core client
 * @param [in] aEntryNo  The index of the entry to alter
 * @param [in] aType     TRUE: the entry is a short address
 *                       FALSE: the entry is an extended address
 * @param [in] aEnable   Whether the given entry is to be enabled or disabled
 *
 * @return the value from the command status register
 * @retval RF_StatCmdDoneSuccess the command completed correctly
 */
static RF_Stat rfCoreModifySourceMatchEntry(RF_Handle aRfHandle,
        uint8_t aEntryNo, platformRadio_address aType, bool aEnable)
{
    /* memset kept to save 60 bytes of text space, gcc can't optimize the
     * following bitfield operation if it doesn't know the fields are zero
     * already.
     */
    memset((void *)&sModifyReceiveSrcMatchCmd, 0,
            sizeof(sModifyReceiveSrcMatchCmd));

    sModifyReceiveSrcMatchCmd.commandNo = CMD_IEEE_MOD_SRC_MATCH;

    /* we only use source matching for pending data bit, so enabling and
     * pending are the same to us.
     */
    if (aEnable)
    {
        sModifyReceiveSrcMatchCmd.options.bEnable = 1;
        sModifyReceiveSrcMatchCmd.options.srcPend = 1;
    }
    else
    {
        sModifyReceiveSrcMatchCmd.options.bEnable = 0;
        sModifyReceiveSrcMatchCmd.options.srcPend = 0;
    }

    sModifyReceiveSrcMatchCmd.options.entryType = aType;
    sModifyReceiveSrcMatchCmd.entryNo = aEntryNo;

    return RF_runImmediateCmd(aRfHandle,
            (uint32_t *)&sModifyReceiveSrcMatchCmd);
}

/**
 * @brief walks the short address source match list to find an address
 *
 * @param [in] address the short address to search for
 *
 * @return the index where the address was found
 * @retval PLATFORM_RADIO_SRC_MATCH_NONE the address was not found
 */
static uint8_t rfCoreFindShortSrcMatchIdx(const uint16_t aAddress)
{
    uint8_t i;

    for (i = 0; i < PLATFORM_RADIO_SHORTADD_SRC_MATCH_NUM; i++)
    {
        if (sSrcMatchShortData.shortAddrEnt[i].shortAddr == aAddress)
        {
            return i;
        }
    }

    return PLATFORM_RADIO_SRC_MATCH_NONE;
}

/**
 * @brief walks the short address source match list to find an empty slot
 *
 * @return the index of an unused address slot
 * @retval PLATFORM_RADIO_SRC_MATCH_NONE no unused slots available
 */
static uint8_t rfCoreFindEmptyShortSrcMatchIdx(void)
{
    uint8_t i;

    for (i = 0; i < PLATFORM_RADIO_SHORTADD_SRC_MATCH_NUM; i++)
    {
        if ((sSrcMatchShortData.srcMatchEn[i / 32] & (1 << (i % 32))) == 0u)
        {
            return i;
        }
    }

    return PLATFORM_RADIO_SRC_MATCH_NONE;
}

/**
 * @brief walks the ext address source match list to find an address
 *
 * @param [in] address the ext address to search for
 *
 * @return the index where the address was found
 * @retval PLATFORM_RADIO_SRC_MATCH_NONE the address was not found
 */
static uint8_t rfCoreFindExtSrcMatchIdx(const uint64_t *aAddress)
{
    uint8_t i;

    for (i = 0; i < PLATFORM_RADIO_EXTADD_SRC_MATCH_NUM; i++)
    {
        if (sSrcMatchExtData.extAddrEnt[i] == *aAddress)
        {
            return i;
        }
    }

    return PLATFORM_RADIO_SRC_MATCH_NONE;
}

/**
 * @brief walks the ext address source match list to find an empty slot
 *
 * @return the index of an unused address slot
 * @retval PLATFORM_RADIO_SRC_MATCH_NONE no unused slots available
 */
static uint8_t rfCoreFindEmptyExtSrcMatchIdx(void)
{
    uint8_t i;

    for (i = 0; i < PLATFORM_RADIO_EXTADD_SRC_MATCH_NUM; i++)
    {
        if ((sSrcMatchExtData.srcMatchEn[i / 32] & (1 << (i % 32))) == 0u)
        {
            return i;
        }
    }

    return PLATFORM_RADIO_SRC_MATCH_NONE;
}

/**
 * @brief   handle end of tx when an ACK is requested
 *
 * @retval  event mask to post if done, or 0 if nothing to do
 */
static UInt handleTxAck(RF_Handle aRfHandle)
{
    UInt evt = Event_Id_NONE;

    switch (sTransmitRxAckCmd.status)
    {
    case IEEE_DONE_TIMEOUT:
        if (sTransmitRetryCount < IEEE802154_MAC_MAX_FRAMES_RETRIES)
        {
            /* re-submit the tx command chain */
            sTransmitRetryCount++;
            sTransmitCmdHandle = startTransmit(aRfHandle);
        }
        else
        {
            /* signal polling function we are done transmitting, we failed
             * to send the packet
             */
            sTransmitError = OT_ERROR_NO_ACK;
            evt = RF_EVENT_TX_DONE;
        }
        break;

    case IEEE_DONE_ACK:
        /* signal polling function we are done transmitting */
        sTransmitError = OT_ERROR_NONE;
        evt = RF_EVENT_TX_DONE;
        break;

    case IEEE_DONE_ACKPEND:
        /* signal polling function we are done transmitting */
        sTransmitError = OT_ERROR_NONE;
        evt = RF_EVENT_TX_DONE;
        break;

    case IEEE_DONE_BUSY:
        /* signal polling function we are done transmitting */
        sTransmitError = OT_ERROR_CHANNEL_ACCESS_FAILURE;
        evt = RF_EVENT_TX_DONE;
        break;

    default:
        /* signal polling function we are done transmitting */
        sTransmitError = OT_ERROR_FAILED;
        evt = RF_EVENT_TX_DONE;
        break;
    }
    return evt;
}


/**
 * Shared callback for Tx or Rx command chains, it is called when
 * various events occur during tx and rx
 *
 * @param [in] aRfHandle    Handle of the RF object
 * @param [in] aRfCmdHandle Handle of the command chain that finished
 * @param [in] aRfEventMask events that happened to trigger this callback
 */
static void rfCommonCallback(RF_Handle aRfHandle,
                             RF_CmdHandle aRfCmdHandle,
                             RF_EventMask aRfEventMask)
{
    UInt evts = Event_Id_NONE;
    bool need_ack;
    bool was_tx;

    if (aRfEventMask & RF_EventTXAck)
    {
        /* A packet was received, that packet required an ACK and the
         * ACK has been transmitted
         */
        evts |= RF_EVENT_RX_ACK_DONE;
    }

    if (aRfEventMask & RF_EventRxEntryDone)
    {
        /* A packet was received the packet MAY require an ACK Or the
         * packet might not (ie: a broadcast)
         */
        evts |= RF_EVENT_RX_DONE;
    }

    if (aRfEventMask & (RF_EventLastFGCmdDone | RF_EventLastCmdDone))
    {
        /* the LastFgCmdDone occurs at the end of a Transmit chain.
         * the Last<non-fg>CmdDone could occur in an RX chain.
         */
        seedRandom = sCsmaBackoffCmd.randomState;

        need_ack = false;
        was_tx = false;

        /* Where we transmitting? */
        if (sTransmitCmd.pPayload != NULL)
        {
            was_tx = true;
            if (sTransmitCmd.pPayload[0] & IEEE802154_ACK_REQUEST)
            {
                need_ack = true;
            }
        }
        /* If we transmitted, there are 3 steps
         *    1) CSMA (backoff)
         *    2) Transmit packet
         *    3) RX ack (optional)
         */
        if (need_ack)
        {
            /* determine if we received the ack or if we must retry
             * due to timeout/no-response.
             */
            evts |= handleTxAck(aRfHandle);
        }
        else if (was_tx)
        {
            /* non-ACK [broadcast] case */

            if (sTransmitCmd.status == IEEE_DONE_OK)
            {
                /* Success */
                sTransmitError = OT_ERROR_NONE;
            }
            else if (sCsmaBackoffCmd.status != IEEE_DONE_OK)
            {
                /* noisy environment */
                sTransmitError = OT_ERROR_CHANNEL_ACCESS_FAILURE;
            }
            else
            {
                /* retry failure or other */
                sTransmitError = OT_ERROR_FAILED;
            }
            evts |= RF_EVENT_TX_DONE;
        }
    }

    /* tell radio processing loop what happened */
    radioSignal(evts);
}

/**
 * @brief Start a transmission, or a start a retry
 *
 * @param aRfHandle [in] the rf handle
 * @return RF command handle for the transmission.
 */
static RF_CmdHandle startTransmit(RF_Handle aRfHandle)
{
    RF_CmdHandle r;
    RF_ScheduleCmdParams rfScheduleCmdParams;

    /* no error has occured (yet) */
    sTransmitError = OT_ERROR_NONE;

    /* Clear the sticky tx done flag */
    sTxIsDone = false;

    RF_ScheduleCmdParams_init(&rfScheduleCmdParams);

    r = RF_scheduleCmd(aRfHandle, (RF_Op *)&sCsmaBackoffCmd,
                       &rfScheduleCmdParams, rfCommonCallback,
                       RF_EventLastFGCmdDone);
    return r;
}

/**
 * @brief sends the tx command to the radio core
 *
 * Sends the packet to the radio core to be sent asynchronously.
 *
 * @param [in] aRfHandle The handle for the RF core client
 * @param [in] aPsdu a pointer to the data to be sent
 * @note this *must* be 4 byte aligned and not include the FCS, that is
 * calculated in hardware.
 * @param [in] aLen the length in bytes of data pointed to by psdu.
 *
 * @return handle of the running command returned by the command scheduler
 */
static RF_CmdHandle rfCoreSendTransmitCmd(RF_Handle aRfHandle,
                                          uint8_t *aPsdu,
                                          uint8_t aLen)
{
    static const rfc_CMD_IEEE_CSMA_t cCsmaBackoffCmd =
    {
        .commandNo                  = CMD_IEEE_CSMA,
        .status                     = IDLE,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
            .pastTrig               = 1, // XXX: workaround for RF scheduler
        },
        .condition                  =
        {
            .rule                   = COND_ALWAYS,
        },
        .macMaxBE                   = IEEE802154_MAC_MAX_BE,
        .macMaxCSMABackoffs         = IEEE802154_MAC_MAX_CSMA_BACKOFFS,
        .csmaConfig                 =
        {
            .initCW                 = 1,
            .bSlotted               = 0,
            .rxOffMode              = 0,
        },
        .NB                         = 0,
        .BE                         = IEEE802154_MAC_MIN_BE,
        .remainingPeriods           = 0,
        .endTrigger                 =
        {
            .triggerType            = TRIG_NEVER,
        },
        .endTime                    = 0x00000000,
    };
    static const rfc_CMD_IEEE_TX_t cTransmitCmd =
    {
        .commandNo                  = CMD_IEEE_TX,
        .status                     = IDLE,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
            .pastTrig               = 1, // XXX: workaround for RF scheduler
        },
        .condition                  =
        {
            .rule                   = COND_NEVER,
        },
        .pNextOp                    = NULL,
    };
    static const rfc_CMD_IEEE_RX_ACK_t cTransmitRxAckCmd =
    {
        .commandNo                  = CMD_IEEE_RX_ACK,
        .status                     = IDLE,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
            .pastTrig               = 1, // XXX: workaround for RF scheduler
        },
        .endTrigger                 =
        {
            .triggerType            = TRIG_REL_START,
            .pastTrig               = 1,
        },
        .condition                  =
        {
            .rule                   = COND_NEVER,
        },
        .pNextOp                    = NULL,
        /* number of RAT ticks to wait before claiming we haven't received an
         * ack
         */
        .endTime                    =
            ((IEEE802154_MAC_ACK_WAIT_DURATION * PLATFORM_RADIO_RAT_TICKS_PER_SEC)
             / IEEE802154_SYMBOLS_PER_SEC),
    };


    /* reset retry count */
    sTransmitRetryCount = 0;

    sCsmaBackoffCmd               = cCsmaBackoffCmd;
    sCsmaBackoffCmd.randomState   = seedRandom;
    sCsmaBackoffCmd.pNextOp       = (rfc_radioOp_t *) &sTransmitCmd;

    sTransmitCmd = cTransmitCmd;
    /* no need to look for an ack if the tx operation was stopped */
    sTransmitCmd.payloadLen = aLen;

    /* XXX: the command callback uses the pPayload pointer is a pseudo
     * "transmit active" flag, if NULL a transmission is not active
     */
    sTransmitCmd.pPayload = aPsdu;

    /* XXX: we ignore the ack request flag if the diag module is enabled. */
    if (!otDiagIsEnabled() && (aPsdu[0] & IEEE802154_ACK_REQUEST))
    {
        /* setup the receive ack command to follow the tx command */
        sTransmitCmd.condition.rule = COND_STOP_ON_FALSE;
        sTransmitCmd.pNextOp = (rfc_radioOp_t *) &sTransmitRxAckCmd;

        sTransmitRxAckCmd = cTransmitRxAckCmd;
        sTransmitRxAckCmd.seqNo = aPsdu[IEEE802154_DSN_OFFSET];
    }
    else
    {
        /* it was a broadcast, for example a beacon, no ack expected */
    }

    sTransmitError = OT_ERROR_NONE;

    return startTransmit(aRfHandle);
}

/**
 * @brief sends the rx command to the radio core
 *
 * Sends the pre-built receive command to the radio core. This sets up the
 * radio to receive packets according to the settings in the global rx command.
 *
 * @note This function does not alter any of the parameters of the rx command.
 * It is only concerned with sending the command to the radio core. See @ref
 * otPlatRadioSetPanId for an example of how the rx settings are set changed.
 *
 * @param [in] aRfHandle The handle for the RF core client
 *
 * @return handle of the running command returned by the command scheduler
 */
static RF_CmdHandle rfCoreSendReceiveCmd(RF_Handle aRfHandle)
{
    RF_ScheduleCmdParams rfScheduleCmdParams;
    RF_ScheduleCmdParams_init(&rfScheduleCmdParams);

    sReceiveCmd.status = IDLE;

    return RF_scheduleCmd(aRfHandle, (RF_Op *)&sReceiveCmd,
                          &rfScheduleCmdParams, rfCommonCallback,
                          (RF_EventLastCmdDone | RF_EventRxEntryDone |
                           RF_EventTXAck));
}

/**
 * @brief Callback for the Energy Detect command.
 *
 * @param [in] aRfHandle    Handle of the RF object
 * @param [in] aRfCmdHandle Handle of the command chain that finished
 * @param [in] aRfEventMask events that happened to trigger this callback
 */
static void rfCoreEdScanCmdCallback(RF_Handle aRfHandle,
                                    RF_CmdHandle aRfCmdHandle,
                                    RF_EventMask aRfEventMask)
{
    radioSignal(RF_EVENT_ED_SCAN_DONE);
}

/**
 * @brief sends the energy detect scan command to the radio core
 *
 * Sends the Energy Detect scan command to the radio core. This scans the given
 * channel for activity.
 *
 * @param [in] aRfHandle The handle for the RF core client
 * @param [in] aChannel The IEEE page 0 channel to scan
 * @param [in] aDuration Time in ms to scan
 *
 * @return handle of the running command returned by the command scheduler
 */
static RF_CmdHandle rfCoreSendEdScanCmd(RF_Handle aRfHandle,
                                        uint8_t aChannel,
                                        uint16_t aDuration)
{
    static const rfc_CMD_IEEE_ED_SCAN_t cEdScanCmd =
    {
        .commandNo                  = CMD_IEEE_ED_SCAN,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
            .pastTrig               = 1, // XXX: workaround for RF scheduler
        },
        .condition                  =
        {
            .rule                   = COND_NEVER,
        },
        .ccaOpt                     =
        {
            .ccaEnEnergy            = 1,
            .ccaEnCorr              = 1,
            .ccaEnSync              = 1,
            .ccaCorrOp              = 1,
            .ccaSyncOp              = 0,
            .ccaCorrThr             = 3,
        },
        .ccaRssiThr                 = -90,
        .endTrigger                 =
        {
            .triggerType            = TRIG_REL_START,
            .pastTrig               = 1,
        },
    };

    RF_ScheduleCmdParams rfScheduleCmdParams;
    RF_ScheduleCmdParams_init(&rfScheduleCmdParams);

    sEdScanCmd = cEdScanCmd;

    sEdScanCmd.channel = aChannel;

    /* duration is in ms */
    sEdScanCmd.endTime = aDuration * (PLATFORM_RADIO_RAT_TICKS_PER_SEC / 1000);

    return RF_scheduleCmd(aRfHandle,
                          (RF_Op *)&sEdScanCmd,
                          &rfScheduleCmdParams,
                          rfCoreEdScanCmdCallback,
                          RF_EventLastCmdDone);
}

/**
 * @brief Callback for the Transmit Test command.
 *
 * @param [in] aRfHandle    Handle of the RF object
 * @param [in] aRfCmdHandle Handle of the command chain that finished
 * @param [in] aRfEventMask events that happened to trigger this callback
 */
static void rfCoreTxTestCmdCallback(RF_Handle aRfHandle,
                                    RF_CmdHandle aRfCmdHandle,
                                    RF_EventMask aRfEventMask)
{
    (void) aRfHandle;
    (void) aRfCmdHandle;
    (void) aRfEventMask;

    return;
}

/**
 * @param [in] aRfHandle The handle for the RF core client
 *
 * @return handle of the running command returned by the command scheduler
 */
static RF_CmdHandle rfCoreSendTxTestCmd(RF_Handle aRfHandle, bool aModulated)
{
    static const rfc_CMD_TX_TEST_t cTxTestCmd =
    {
        .commandNo                  = CMD_TX_TEST,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
            .pastTrig               = 1, // XXX: workaround for RF scheduler
        },
        .condition                  =
        {
            .rule                   = COND_NEVER,
        },
        .config                     =
        {
            .bUseCw                 = 0,
            .bFsOff                 = 0,
            .whitenMode             = 2,
        },
        .endTrigger                 =
        {
            .triggerType            = TRIG_NEVER,
        },
        .syncWord                   = PLATFORM_RADIO_TX_TEST_SYNC_WORD,
    };

    RF_ScheduleCmdParams rfScheduleCmdParams;
    RF_ScheduleCmdParams_init(&rfScheduleCmdParams);

    sTxTestCmd = cTxTestCmd;

    if (aModulated)
    {
        sTxTestCmd.txWord = PLATFORM_RADIO_TX_TEST_MODULATED_WORD;
    }
    else
    {
        sTxTestCmd.config.bUseCw = 1;
        sTxTestCmd.txWord        = PLATFORM_RADIO_TX_TEST_UNMODULATED_WORD;
    }

    return RF_scheduleCmd(aRfHandle, (RF_Op *) &sTxTestCmd,
                          &rfScheduleCmdParams, rfCoreTxTestCmdCallback,
                          RF_EventLastCmdDone);
}

/**
 * Default error callback for RF Driver.
 *
 * Errors are unlikely, and fatal.
 */
static void rfCoreErrorCallback(RF_Handle aHandle,
                                RF_CmdHandle aCmdHandle,
                                RF_EventMask aEvents)
{
    while(1);
}

/**
 * Function documented in radio.h
 */
void rfCoreDiagChannelDisable(uint8_t aChannel)
{
    otPlatRadioReceive(NULL, aChannel);
    sDisableChannel = true;
}

/**
 * Function documented in radio.h
 */
void rfCoreDiagChannelEnable(uint8_t aChannel)
{
    sDisableChannel = false;
    otPlatRadioReceive(NULL, aChannel);
}

/**
 * Function documented in platform.h
 */
void platformRadioInit(void)
{
    rfCoreInitBufs();
    rfCoreInitSetupCmd();
    /* Populate the RX parameters data structure with default values */
    rfCoreInitReceiveParams();
    rfCoreInitEvent();

    /* get the seed from true random generator */
    seedRandom = otPlatRandomGet();

    sState = platformRadio_phyState_Disabled;
}

/**
 * Function documented in platform/radio.h
 */
otError otPlatRadioEnable(otInstance *aInstance)
{
    otError error = OT_ERROR_BUSY;
    RF_Params rfParams;
    (void)aInstance;

    if (sState == platformRadio_phyState_Sleep)
    {
        error = OT_ERROR_NONE;
    }
    else if (sState == platformRadio_phyState_Disabled)
    {
        RF_Params_init(&rfParams);

        rfParams.pErrCb         = rfCoreErrorCallback;

        sRfHandle = RF_open(&sRfObject, &sRfMode,
                (RF_RadioSetup *)&sRadioSetupCmd, &rfParams);

        otEXPECT_ACTION(sRfHandle != NULL, error = OT_ERROR_FAILED);
        sState = platformRadio_phyState_Sleep;

        rfCoreSetTransmitPower(sCurrentOutputPower);
    }

exit:
    if (error == OT_ERROR_FAILED)
    {
        sState = platformRadio_phyState_Disabled;
    }

    return error;
}

/**
 * Function documented in platform/radio.h
 */
bool otPlatRadioIsEnabled(otInstance *aInstance)
{
    (void)aInstance;
    return (sState != platformRadio_phyState_Disabled);
}

/**
 * Function documented in platform/radio.h
 */
otError otPlatRadioDisable(otInstance *aInstance)
{
    otError error = OT_ERROR_BUSY;
    (void)aInstance;

    if (sState == platformRadio_phyState_Disabled)
    {
        error = OT_ERROR_NONE;
    }
    else if (sState == platformRadio_phyState_Sleep)
    {
        RF_close(sRfHandle);
        sState = platformRadio_phyState_Disabled;
        error = OT_ERROR_NONE;
    }

    return error;
}

/**
 * Function documented in platform/radio.h
 */
otError otPlatRadioEnergyScan(otInstance *aInstance, uint8_t aScanChannel,
        uint16_t aScanDuration)
{
    otError error = OT_ERROR_NONE;
    (void)aInstance;

    switch (sState)
    {
    case platformRadio_phyState_Receive:
        sState = platformRadio_phyState_EdScan;
        /* abort receive */
        rfCoreExecuteAbortCmd(sRfHandle, sReceiveCmdHandle);
        otEXPECT_ACTION((sReceiveCmd.status != PENDING
                         && sReceiveCmd.status != ACTIVE
                         && sReceiveCmd.status != IEEE_SUSPENDED),
                        error = OT_ERROR_FAILED);

        /* fall through */
    case platformRadio_phyState_Sleep:
        sState = platformRadio_phyState_EdScan;
        otEXPECT_ACTION(rfCoreSendEdScanCmd(sRfHandle, aScanChannel,
                                            aScanDuration) >= 0,
                        error = OT_ERROR_FAILED);
        break;

    default:
        error = OT_ERROR_BUSY;
        break;
    }

exit:
    if (OT_ERROR_NONE != error)
    {
        sState = platformRadio_phyState_Sleep;
    }
    return error;
}

/**
 * Function documented in platform/radio.h
 */
otError otPlatRadioSetTransmitPower(otInstance *aInstance, int8_t aPower)
{
    RF_TxPowerTable_Value powerCfg;
    (void)aInstance;

    powerCfg = RF_TxPowerTable_findValue(txPowerTable, aPower);

    return rfCoreSetTransmitPower(powerCfg);
}

/**
 * Function documented in platform/radio.h
 */
otError otPlatRadioGetTransmitPower(otInstance *aInstance, int8_t *aPower)
{
    otError error = OT_ERROR_NONE;
    (void)aInstance;

    otEXPECT_ACTION(aPower != NULL, error = OT_ERROR_INVALID_ARGS);
    /* XXX: to be replaced with `RF_getTxPower` once `sCurrentOutputPower` can be removed */
    *aPower = RF_TxPowerTable_findPowerLevel(txPowerTable, sCurrentOutputPower);

exit:
    return error;
}

/**
 * Function documented in platform/radio.h
 */
otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel)
{
    otError error = OT_ERROR_BUSY;
    (void)aInstance;

    if (sState == platformRadio_phyState_Sleep)
    {
        /* initialize the receive command
         * no memset here because we assume init has been called and we may
         * have changed some values in the rx command
         */
        if (!sDisableChannel)
        {
            /* If the diag module has not locked out changing the channel */
            sReceiveCmd.channel = aChannel;
        }
        sReceiveCmdHandle = rfCoreSendReceiveCmd(sRfHandle);
        otEXPECT_ACTION(sReceiveCmdHandle >= 0, error = OT_ERROR_FAILED);
        sState = platformRadio_phyState_Receive;
        error = OT_ERROR_NONE;
    }
    else if (sState == platformRadio_phyState_Receive)
    {
        if (sReceiveCmd.channel == aChannel || sDisableChannel)
        {
            /* we are already running on the correct channel or the diag module
             * has disallowed switching channels.
             */
            error = OT_ERROR_NONE;
        }
        else
        {
            rfCoreExecuteAbortCmd(sRfHandle, sReceiveCmdHandle);
            otEXPECT_ACTION((sReceiveCmd.status != PENDING
                        && sReceiveCmd.status != ACTIVE
                        && sReceiveCmd.status != IEEE_SUSPENDED),
                     error = OT_ERROR_FAILED);

            sReceiveCmd.channel = aChannel;
            sReceiveCmdHandle = rfCoreSendReceiveCmd(sRfHandle);
            otEXPECT_ACTION(sReceiveCmdHandle >= 0, error = OT_ERROR_FAILED);
            error = OT_ERROR_NONE;
        }
    }

exit:
    return error;
}

/**
 * Function documented in platform/radio.h
 */
otError otPlatRadioSleep(otInstance *aInstance)
{
    otError error = OT_ERROR_BUSY;
    (void)aInstance;

    if (sState == platformRadio_phyState_Sleep)
    {
        error = OT_ERROR_NONE;
    }
    else if (sState == platformRadio_phyState_Receive)
    {
        rfCoreExecuteAbortCmd(sRfHandle, sReceiveCmdHandle);
        otEXPECT_ACTION((sReceiveCmd.status != PENDING
                             && sReceiveCmd.status != ACTIVE
                             && sReceiveCmd.status != IEEE_SUSPENDED),
                        error = OT_ERROR_FAILED);

        sState = platformRadio_phyState_Sleep;

        /* The upper layers like to thrash the interface from RX to sleep.
         * Aborting and restarting the commands wastes time and energy, but
         * can be done as often as requested; yielding the RF driver causes
         * the whole core to be shutdown. Delay yield until the rf processing
         * loop to make sure we actually want to sleep.
         */
        radioSignal(RF_EVENT_SLEEP_YIELD);
        error = OT_ERROR_NONE;
    }

exit:
    return error;
}

/**
 * Function documented in platform/radio.h
 */
otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
    (void)aInstance;
    return &sTransmitFrame;
}

/**
 * Function documented in platform/radio.h
 */
otError otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame)
{
    otError error = OT_ERROR_BUSY;

    if (sState == platformRadio_phyState_Receive)
    {
        sState = platformRadio_phyState_Transmit;

        /* removing 2 bytes of CRC placeholder, generated in hardware */
        sTransmitCmdHandle = rfCoreSendTransmitCmd(sRfHandle, aFrame->mPsdu,
                aFrame->mLength - 2);
        otEXPECT_ACTION(sTransmitCmdHandle >= 0, error = OT_ERROR_FAILED);
        error = OT_ERROR_NONE;
    }

exit:
    return error;
}

/**
 * Function documented in platform/radio.h
 */
int8_t otPlatRadioGetRssi(otInstance *aInstance)
{
    (void)aInstance;
    return sRfStats.maxRssi;
}

/**
 * Function documented in platform/radio.h
 */
#ifdef __TI_ARM__
/*
 * ti-cgt warns about using enums as bitfields because by default all enums
 * are packed to save space. We could pass the `--enum_type=int` switch to the
 * compiler, but this creates linking errors with our board support libraries.
 *
 * Instead we disable `#190-D enumerated type mixed with another type`.
 */
#pragma diag_push
#pragma diag_suppress 190
#endif
otRadioCaps otPlatRadioGetCaps(otInstance *aInstance)
{
    (void)aInstance;
    return OT_RADIO_CAPS_ACK_TIMEOUT | OT_RADIO_CAPS_ENERGY_SCAN
        | OT_RADIO_CAPS_TRANSMIT_RETRIES;
}
#ifdef __TI_ARM__
#pragma diag_pop
#endif

/**
 * Function documented in platform/radio.h
 */
void otPlatRadioEnableSrcMatch(otInstance *aInstance, bool aEnable)
{
    (void)aInstance;

    if (sReceiveCmd.status == ACTIVE || sReceiveCmd.status == IEEE_SUSPENDED)
    {
        /* we have a running or backgrounded rx command */
        otEXPECT(rfCoreModifyRxAutoPend(sRfHandle, aEnable)
                == RF_StatCmdDoneSuccess);
    }
    else
    {
        /* if we are promiscuous, then frame filtering should be disabled */
        sReceiveCmd.frameFiltOpt.autoPendEn = aEnable ? 1 : 0;
    }

exit:
    return;
}

/**
 * Function documented in platform/radio.h
 */
otError otPlatRadioAddSrcMatchShortEntry(otInstance *aInstance,
        const uint16_t aShortAddress)
{
    otError error = OT_ERROR_NONE;
    (void)aInstance;
    uint8_t idx = rfCoreFindShortSrcMatchIdx(aShortAddress);

    if (idx == PLATFORM_RADIO_SRC_MATCH_NONE)
    {
        /* the entry does not exist already, add it */
        otEXPECT_ACTION((idx = rfCoreFindEmptyShortSrcMatchIdx())
                != PLATFORM_RADIO_SRC_MATCH_NONE,
                error = OT_ERROR_NO_BUFS);
        sSrcMatchShortData.shortAddrEnt[idx].shortAddr = aShortAddress;
        sSrcMatchShortData.shortAddrEnt[idx].panId = sReceiveCmd.localPanID;
    }

    if (sReceiveCmd.status == ACTIVE || sReceiveCmd.status == IEEE_SUSPENDED)
    {
        /* we have a running or backgrounded rx command */
        otEXPECT_ACTION(rfCoreModifySourceMatchEntry(sRfHandle, idx,
                    platformRadio_address_short, true) == RF_StatCmdDoneSuccess,
                error = OT_ERROR_FAILED);
    }
    else
    {
        /* we are not running, so we must update the values ourselves */
        sSrcMatchShortData.srcPendEn[idx / 32] |= (1 << (idx % 32));
        sSrcMatchShortData.srcMatchEn[idx / 32] |= (1 << (idx % 32));
    }

exit:
    return error;
}

/**
 * Function documented in platform/radio.h
 */
otError otPlatRadioClearSrcMatchShortEntry(otInstance *aInstance,
        const uint16_t aShortAddress)
{
    otError error = OT_ERROR_NONE;
    (void)aInstance;
    uint8_t idx;
    otEXPECT_ACTION((idx = rfCoreFindShortSrcMatchIdx(aShortAddress))
            != PLATFORM_RADIO_SRC_MATCH_NONE,
            error = OT_ERROR_NO_ADDRESS);

    if (sReceiveCmd.status == ACTIVE || sReceiveCmd.status == IEEE_SUSPENDED)
    {
        /* we have a running or backgrounded rx command */
        otEXPECT_ACTION(rfCoreModifySourceMatchEntry(sRfHandle, idx,
                    platformRadio_address_short, false) == RF_StatCmdDoneSuccess,
                error = OT_ERROR_FAILED);
    }
    else
    {
        /* we are not running, so we must update the values ourselves */
        sSrcMatchShortData.srcPendEn[idx / 32] &= ~(1 << (idx % 32));
        sSrcMatchShortData.srcMatchEn[idx / 32] &= ~(1 << (idx % 32));
    }

exit:
    return error;
}

/**
 * Function documented in platform/radio.h
 */
otError otPlatRadioAddSrcMatchExtEntry(otInstance *aInstance,
        const otExtAddress *aExtAddress)
{
    otError error = OT_ERROR_NONE;
    (void)aInstance;
    uint8_t idx = rfCoreFindExtSrcMatchIdx((uint64_t *)aExtAddress);

    if (idx == PLATFORM_RADIO_SRC_MATCH_NONE)
    {
        /* the entry does not exist already, add it */
        otEXPECT_ACTION((idx = rfCoreFindEmptyExtSrcMatchIdx())
                != PLATFORM_RADIO_SRC_MATCH_NONE,
                error = OT_ERROR_NO_BUFS);
        sSrcMatchExtData.extAddrEnt[idx] = *((uint64_t *)aExtAddress);
    }

    if (sReceiveCmd.status == ACTIVE || sReceiveCmd.status == IEEE_SUSPENDED)
    {
        /* we have a running or backgrounded rx command */
        otEXPECT_ACTION(rfCoreModifySourceMatchEntry(sRfHandle, idx,
                    platformRadio_address_ext, true) == RF_StatCmdDoneSuccess,
                error = OT_ERROR_FAILED);
    }
    else
    {
        /* we are not running, so we must update the values ourselves */
        sSrcMatchExtData.srcPendEn[idx / 32] |= (1 << (idx % 32));
        sSrcMatchExtData.srcMatchEn[idx / 32] |= (1 << (idx % 32));
    }

exit:
    return error;
}

/**
 * Function documented in platform/radio.h
 */
otError otPlatRadioClearSrcMatchExtEntry(otInstance *aInstance,
        const otExtAddress *aExtAddress)
{
    otError error = OT_ERROR_NONE;
    (void)aInstance;
    uint8_t idx;
    otEXPECT_ACTION((idx = rfCoreFindExtSrcMatchIdx((uint64_t *)aExtAddress))
            != PLATFORM_RADIO_SRC_MATCH_NONE, error = OT_ERROR_NO_ADDRESS);

    if (sReceiveCmd.status == ACTIVE || sReceiveCmd.status == IEEE_SUSPENDED)
    {
        /* we have a running or backgrounded rx command */
        otEXPECT_ACTION(rfCoreModifySourceMatchEntry(sRfHandle, idx,
                    platformRadio_address_ext, false) == RF_StatCmdDoneSuccess,
                error = OT_ERROR_FAILED);
    }
    else
    {
        /* we are not running, so we must update the values ourselves */
        sSrcMatchExtData.srcPendEn[idx / 32] &= ~(1 << (idx % 32));
        sSrcMatchExtData.srcMatchEn[idx / 32] &= ~(1 << (idx % 32));
    }

exit:
    return error;
}

/**
* Function documented in platform/radio.h
*/
void otPlatRadioClearSrcMatchShortEntries(otInstance *aInstance)
{
    (void)aInstance;

    if (sState == platformRadio_phyState_Receive
            || sState == platformRadio_phyState_Transmit)
    {
        unsigned int i;
        for (i = 0; i < PLATFORM_RADIO_SHORTADD_SRC_MATCH_NUM; i++)
        {
            /* we have a running or backgrounded rx command */
            otEXPECT(rfCoreModifySourceMatchEntry(sRfHandle, i,
                        platformRadio_address_short, false) == CMDSTA_Done);
        }
    }
    else
    {
        /* we are not running, so we can erase them ourselves */
        memset((void *)&sSrcMatchShortData, 0, sizeof(sSrcMatchShortData));
    }
exit:
    return;
}

/**
* Function documented in platform/radio.h
*/
void otPlatRadioClearSrcMatchExtEntries(otInstance *aInstance)
{
    (void)aInstance;

    if (sState == platformRadio_phyState_Receive
            || sState == platformRadio_phyState_Transmit)
    {
        unsigned int i;
        for (i = 0; i < PLATFORM_RADIO_EXTADD_SRC_MATCH_NUM; i++)
        {
            /* we have a running or backgrounded rx command */
            otEXPECT(rfCoreModifySourceMatchEntry(sRfHandle, i,
                        platformRadio_address_ext, false) == CMDSTA_Done);
        }
    }
    else
    {
        /* we are not running, so we can erase them ourselves */
        memset((void *)&sSrcMatchExtData, 0, sizeof(sSrcMatchExtData));
    }
exit:
    return;
}

/**
 * Function documented in platform/radio.h
 */
bool otPlatRadioGetPromiscuous(otInstance *aInstance)
{
    (void)aInstance;
    /* we are promiscuous if we are not filtering */
    return sReceiveCmd.frameFiltOpt.frameFiltEn == 0;
}

/**
 * Function documented in platform/radio.h
 */
void otPlatRadioSetPromiscuous(otInstance *aInstance, bool aEnable)
{
    (void)aInstance;

    if (sReceiveCmd.status == ACTIVE || sReceiveCmd.status == IEEE_SUSPENDED)
    {
        /* we have a running or backgrounded rx command */
        /* if we are promiscuous, then frame filtering should be disabled */
        otEXPECT(rfCoreModifyRxFrameFilter(sRfHandle, !aEnable)
                == RF_StatCmdDoneSuccess);
        /* XXX should we dump any queued messages ? */
    }
    else
    {
        /* if we are promiscuous, then frame filtering should be disabled */
        sReceiveCmd.frameFiltOpt.frameFiltEn = aEnable ? 0 : 1;
    }

exit:
    return;
}

/**
 * Function documented in platform/radio.h
 */
void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64)
{
    uint8_t *eui64;
    unsigned int i;
    (void)aInstance;

    /* The IEEE MAC address can be stored two places. We check the Customer
     * Configuration was not set before defaulting to the Factory
     * Configuration.
     */
    eui64 = (uint8_t *)(CCFG_BASE + CCFG_O_IEEE_MAC_0);

    for (i = 0; i < OT_EXT_ADDRESS_SIZE; i++)
    {
        /* If the EUI is all ones, then the EUI is invalid, or wasn't set. */
        if (eui64[i] != 0xFF)
        {
            break;
        }
    }

    if (i >= OT_EXT_ADDRESS_SIZE)
    {
        /* The ccfg address was all 0xFF, switch to the fcfg */
        eui64 = (uint8_t *)(FCFG1_BASE + FCFG1_O_MAC_15_4_0);
    }

    /* The IEEE MAC address is stored in network byte order (big endian).
     * The caller seems to want the address stored in little endian format,
     * which is backwards of the conventions setup by @ref
     * otPlatRadioSetExtendedAddress. otPlatRadioSetExtendedAddress assumes
     * that the address being passed to it is in network byte order (big
     * endian), so the caller of otPlatRadioSetExtendedAddress must swap the
     * endianness before calling.
     *
     * It may be easier to have the caller of this function store the IEEE
     * address in network byte order (big endian).
     */
    for (i = 0; i < OT_EXT_ADDRESS_SIZE; i++)
    {
        aIeeeEui64[i] = eui64[(OT_EXT_ADDRESS_SIZE - 1) - i];
    }
}

/**
 * Function documented in platform/radio.h
 *
 * @note it is entirely possible for this function to fail, but there is no
 * valid way to return that error since the funciton prototype was changed.
 */
void otPlatRadioSetPanId(otInstance *aInstance, uint16_t aPanid)
{
    (void)aInstance;

    /* XXX: if the pan id is the broadcast pan id (0xFFFF) the auto ack will
     * not work. This is due to the design of the CM0 and follows IEEE 802.15.4
     */
    if (sState == platformRadio_phyState_Receive)
    {
        /* stop the running receive operation */
        rfCoreExecuteAbortCmd(sRfHandle, sReceiveCmdHandle);
        otEXPECT(sReceiveCmd.status != PENDING
                    && sReceiveCmd.status != ACTIVE
                    && sReceiveCmd.status != IEEE_SUSPENDED);

        sReceiveCmd.localPanID = aPanid;
        sReceiveCmdHandle = rfCoreSendReceiveCmd(sRfHandle);
    }
    else if (sState != platformRadio_phyState_Transmit)
    {
        sReceiveCmd.localPanID = aPanid;
    }

exit:
    return;
}

/**
 * Function documented in platform/radio.h
 *
 * @note it is entirely possible for this function to fail, but there is no
 * valid way to return that error since the function prototype was changed.
 */
void otPlatRadioSetExtendedAddress(otInstance *aInstance, const otExtAddress *aAddress)
{
    (void)aInstance;

    if (sState == platformRadio_phyState_Receive)
    {
        /* stop the running receive operation */
        rfCoreExecuteAbortCmd(sRfHandle, sReceiveCmdHandle);
        otEXPECT(sReceiveCmd.status != PENDING
                    && sReceiveCmd.status != ACTIVE
                    && sReceiveCmd.status != IEEE_SUSPENDED);

        sReceiveCmd.localExtAddr = *((uint64_t *)(aAddress));
        sReceiveCmdHandle = rfCoreSendReceiveCmd(sRfHandle);
    }
    else if (sState != platformRadio_phyState_Transmit)
    {
        sReceiveCmd.localExtAddr = *((uint64_t *)(aAddress));
    }

exit:
    return;
}

/**
 * Function documented in platform/radio.h
 *
 * @note it is entirely possible for this function to fail, but there is no
 * valid way to return that error since the funciton prototype was changed.
 */
void otPlatRadioSetShortAddress(otInstance *aInstance, uint16_t aAddress)
{
    (void)aInstance;

    if (sState == platformRadio_phyState_Receive)
    {
        /* stop the running receive operation */
        rfCoreExecuteAbortCmd(sRfHandle, sReceiveCmdHandle);
        otEXPECT(sReceiveCmd.status != PENDING
                    && sReceiveCmd.status != ACTIVE
                    && sReceiveCmd.status != IEEE_SUSPENDED);

        sReceiveCmd.localShortAddr = aAddress;
        sReceiveCmdHandle = rfCoreSendReceiveCmd(sRfHandle);
    }
    else if (sState != platformRadio_phyState_Transmit)
    {
        sReceiveCmd.localShortAddr = aAddress;
    }

exit:
    return;
}

/**
 * Function documented in platform/radio.h
 */
otError otPlatDiagRadioToneStart(otInstance *aInstance, bool aModulated)
{
    (void)aInstance;
    otError retval = OT_ERROR_NONE;

    otEXPECT_ACTION(platformRadio_phyState_Receive == sState
                        || platformRadio_phyState_Sleep == sState,
                    retval = OT_ERROR_INVALID_STATE);

    if (platformRadio_phyState_Receive == sState)
    {
        /* stop the running receive operation */
        rfCoreExecuteAbortCmd(sRfHandle, sReceiveCmdHandle);
    }

    sTxTestCmdHandle = rfCoreSendTxTestCmd(sRfHandle, aModulated);

    otEXPECT_ACTION(NULL != sTxTestCmdHandle, retval = OT_ERROR_FAILED);

exit:
    return retval;
}

/**
 * Function documented in platform/radio.h
 */
otError otPlatDiagRadioToneStop(otInstance *aInstance)
{
    (void)aInstance;
    otError retval = OT_ERROR_NONE;

    otEXPECT_ACTION(platformRadio_phyState_Receive == sState
                        || platformRadio_phyState_Sleep == sState,
                    retval = OT_ERROR_INVALID_STATE);

    /* stop the running receive operation */
    rfCoreExecuteAbortCmd(sRfHandle, sTxTestCmdHandle);

    if (platformRadio_phyState_Receive == sState)
    {
        /* re-issue the receive operation */
        sReceiveCmdHandle = rfCoreSendReceiveCmd(sRfHandle);
    }

exit:
    return retval;
}

static void platformRadioProcessTransmitDone(otInstance *aInstance,
                                             otRadioFrame *aTransmitFrame,
                                             otRadioFrame *aAckFrame,
                                             otError aTransmitError)
{
    /* clear the sticky txdone event flag */
    sTxIsDone = false;

    /* clear the pseudo-transmit-active flag */
    sTransmitCmd.pPayload = NULL;

#if OPENTHREAD_ENABLE_DIAG
    if (otPlatDiagModeGet())
    {
        otPlatDiagRadioTransmitDone(aInstance, aTransmitFrame, aTransmitError);
    }
    else
#endif /* OPENTHREAD_ENABLE_DIAG */
    {
        otPlatRadioTxDone(aInstance, aTransmitFrame, aAckFrame, aTransmitError);
    }
}

static void platformRadioProcessReceiveDone(otInstance *aInstance,
                                            otRadioFrame *aReceiveFrame,
                                            otError aReceiveError)
{
#if OPENTHREAD_ENABLE_DIAG
    if (otPlatDiagModeGet())
    {
        otPlatDiagRadioReceiveDone(aInstance, aReceiveFrame, aReceiveError);
    }
    else
#endif /* OPENTHREAD_ENABLE_DIAG */
    {
        otPlatRadioReceiveDone(aInstance, aReceiveFrame, aReceiveError);
    }
}


/**
 * Release an entry in the RX buffer list
 */
static void releaseQueueEntry(struct rx_queue_info *p)
{
    p->curEntry->status = DATA_ENTRY_PENDING;
}

/**
 * Given a queue entry, give the next queue entry.
 */
static void nextQueueEntry(struct rx_queue_info *p)
{
    /* is this the first time? */

    if (p->startEntry == NULL)
    {
        /* yes */
        p->startEntry = p->curEntry;
        /* next */
        p->curEntry = (rfc_dataEntryGeneral_t *)(p->curEntry->pNextEntry);
        return;
    }

    /* are we done? */
    if (p->curEntry == p->startEntry)
    {
        /* YES  */
        p->curEntry = NULL;
    }
    else
    {
        /* Next */
        p->curEntry = (rfc_dataEntryGeneral_t *)(p->curEntry->pNextEntry);
    }
}

/**
 * Release the current entry and move to the next entry in the rx queue.
 */
static void releaseAndNext(struct rx_queue_info *p)
{
    releaseQueueEntry(p);
    nextQueueEntry(p);
}


/**
 * Handle reception of an ACK packet.
 */
static void handleRxAck(struct rx_queue_info *p)
{
    /* We only care about ACKs in TX state */
    if (platformRadio_phyState_Transmit != sState)
    {
        releaseAndNext(p);
        return;
    }

    /*
     * Also note: Due to CSMA backoff other devices may be
     * transmitting and acking, most commonly with differing DSN numbers
     *
     * Check is this ACK for our DSN?
     */
    if (p->receiveFrame.mPsdu[IEEE802154_DSN_OFFSET] != sTransmitFrame.mPsdu[IEEE802154_DSN_OFFSET])
    {
        /* not our ACK, we don't want it */
        releaseAndNext(p);
        return;
    }

    /*
     * This can occur in the following RARE case in a large network.
     *
     * Condition 1:
     * We have started transmiting a packet with a DSN, example X.
     * Thus we are in the TX state.
     *
     * Condition 2:
     * BUT we have not yet completed transmission of that packet.
     *
     * Condition 3:
     * Another device happens to be at the same DSN number.
     *
     * Condition 4:
     * That other device won the CSMA contest...  and transmitted and
     * possibly received their ACK before we transmitted.
     *
     * Condition 5:
     * We just recieved their ACK packet.
     *
     * Ignoring the "timing" component, this is a varient of
     * the famous Birthday problem, where dsn=birthday
     *
     * For more details:
     *   http://mathworld.wolfram.com/BirthdayProblem.html
     *
     * Thus if we have not finished transmitting ignore this ack packet
     */
    if (!(p->events & RF_EVENT_TX_DONE))
    {
        nextQueueEntry(p);
        return;
    }

    /* If a TX error occured, it would have been handled elsewhere */
    if (sTransmitError != OT_ERROR_NONE)
    {
        /* Ignore this, errors would have been handled in RETRY operation */
        releaseAndNext(p);
        return;
    }

    /* SUCCESS */

    /* go back to receive state */
    sState = platformRadio_phyState_Receive;

    /* inform upper layer */
    platformRadioProcessTransmitDone(p->aInstance,
                                     &sTransmitFrame,
                                     &p->receiveFrame,
                                     OT_ERROR_NONE);

    /* release this queue entry and move to the next queue entry */
    releaseAndNext(p);
}

/**
 * Handle reception of a data packet (ie: Non-ACK) packet.
 */
static void handleRxData(struct rx_queue_info *p)
{
    bool need_ack;
    bool tx_ack_done;

    /* Does the packet require an ACK? */
    need_ack = !!(p->receiveFrame.mPsdu[0] & IEEE802154_ACK_REQUEST);

    /* Assuming the ACK was required, has the ack been transmitted? */
    tx_ack_done = !!(p->events & RF_EVENT_RX_ACK_DONE);

    if ((!need_ack) || (need_ack && tx_ack_done))
    {
        /* The conditions are:
     *
     * 1) no ack required means:
     *    we have received a broadcast
     *
     * OR
     *
     * 2) ack required && tx_ack_done means:
     *    normal data packet and the ack
     *    for the packet is complete
     *
     * In both cases, we are done
     */
        platformRadioProcessReceiveDone(p->aInstance,
                                        &(p->receiveFrame),
                                        OT_ERROR_NONE);

        /* Release this queue entry and move to the next queue entry */
        releaseAndNext(p);
    }
    else
    {
        /* not done yet, just move on to the next queue entry */
        nextQueueEntry(p);
    }
}


/**
 * An RX queue entry is in the finished state, process it.
 */
static void handleRxFinish(struct rx_queue_info *p)
{
    int len;
    int rssi;
    rfc_ieeeRxCorrCrc_t    *crcCorr;
    uint8_t *payload;

    /* We have received a packet
     *
     * the common case is: Success.
     *
     * Get the information appended to the end of the frame.  This
     * array access looks like it is a fencepost error, but the first
     * byte is the number of bytes that follow.
     */

    /* extract things from the payload */
    payload = &(p->curEntry->data);
    len     = payload[0] & 0x0ff;
    crcCorr = (rfc_ieeeRxCorrCrc_t *)&payload[len];
    rssi    = payload[len-1];


    /* construct the common case "Success" */
#if OPENTHREAD_ENABLE_RAW_LINK_API
    // TODO: Propagate CM0 timestamp
    p->receiveFrame.mMsec    = otPlatAlarmMilliGetNow();
    p->receiveFrame.mUsec    = 0;  // Don't support microsecond timer for now.
#endif
    p->receiveFrame.mLength  = len;
    p->receiveFrame.mPsdu    = &(payload[1]);
    p->receiveFrame.mChannel = sReceiveCmd.channel;
    p->receiveFrame.mRssi    = rssi;
    p->receiveFrame.mLqi     = crcCorr->status.corr;

    /* if a CRC error, or invalid length occured */
    if (crcCorr->status.bCrcErr  || (len >= (OT_RADIO_FRAME_MAX_SIZE+2)))
    {
        /* toss this packet */
        memset(&(p->receiveFrame), 0x0, sizeof(p->receiveFrame));
        platformRadioProcessReceiveDone(p->aInstance,
                                        &(p->receiveFrame),
                                        OT_ERROR_FCS);
        /* Release this queue entry and move to the next queue entry */
        releaseAndNext(p);
        return;
    }

    /* Is this an ACK frame? */
    if ((p->receiveFrame.mPsdu[0] & IEEE802154_FRAME_TYPE_MASK) == IEEE802154_FRAME_TYPE_ACK)
    {
        /* then handle the ack */
        handleRxAck(p);
    }
    else
    {
        /* otherwise a broadcast (ie: beacon) or data that requires an ack */
        handleRxData(p);
    }
}

/**
 * Scan through the RX queue, looking for completed entries.
 */
static void processRxQueue(otInstance *aInstance, UInt events)
{
    struct rx_queue_info rqi;

    rqi.aInstance       = aInstance;
    rqi.events          = events;
    rqi.startEntry      = NULL;
    rqi.curEntry        = (rfc_dataEntryGeneral_t *)sRxDataQueue.pCurrEntry;

    /* loop through receive queue */
    while (rqi.curEntry != NULL)
    {
        switch (rqi.curEntry->status)
        {
        case DATA_ENTRY_UNFINISHED:
            /* the command was aborted, cleanup the entry */
            /* Release this entry and move to the next entry */
            releaseAndNext(&rqi);
            break;
        case DATA_ENTRY_FINISHED:
            /* Something is in this queue entry, process what we find */
            handleRxFinish(&rqi);
            break;
        default:
            /* Else - busy, or unused, just move to the next entry */
            nextQueueEntry(&rqi);
            break;
        }
    }
}

/**
 * Handle events in the TX state.
 */
static void handleTxState(otInstance *aInstance, UInt events)
{
    if (!(events & RF_EVENT_TX_DONE))
    {
        /* if we are not done transmitting, we have nothing to do */
        return;
    }

    /* We are done transmitting.. */


    /* Did we have an error of some sort? (ie: Retry, Timeout, etc) */
    if (sTransmitError != OT_ERROR_NONE)
    {
        /* something has declared an error */
        sState = platformRadio_phyState_Receive;
        otError tmp;
        tmp = sTransmitError;
        /* clear transmit error BEFORE the callback */
        sTransmitError = OT_ERROR_NONE;
        platformRadioProcessTransmitDone(aInstance,
                                         &sTransmitFrame,
                                         NULL,
                                         tmp);
        /*
         * Why do we clear before the callback? Because the callback
         * MAY trigger another transmission.
         *
         * Two things can occur during that transmission
         *
         * Most likely there will be no problem [this is good].
         *
         * Unlikely: The transmission may have immediate error which
         * would set the sTransmitError to FAIL. When the callback is
         * finished the code here would clear this error... Hence the
         * tmp variable, and clearing the error prior to the callback.
         */
        return;
    }

    /* Transmission is complete */

    /* Does the packet require an ACK? */
    if (!(sTransmitCmd.pPayload[0] & IEEE802154_ACK_REQUEST))
    {
        /* transmit packet does not require an ack */

        /* return to receive state */
        sState = platformRadio_phyState_Receive;

        /* callback */
        platformRadioProcessTransmitDone(aInstance,
                                         &sTransmitFrame,
                                         NULL,
                                         OT_ERROR_NONE);
        return;
    }

    /* transmission is complete, but we must find the ACK in the RX queue */
    processRxQueue(aInstance, events);
}


/**
 * Internal function to handle radio events
 */
static void handleRadioEvents(otInstance *aInstance, UInt events)
{
    if (events & RF_EVENT_ED_SCAN_DONE)
    {
        sState = platformRadio_phyState_Sleep;

        if (sEdScanCmd.status == IEEE_DONE_OK)
        {
            otPlatRadioEnergyScanDone(aInstance, sEdScanCmd.maxRssi);
        }
        else
        {
            otPlatRadioEnergyScanDone(aInstance, PLATFORM_RADIO_INVALID_RSSI);
        }
    }

    if (sState == platformRadio_phyState_Receive)
    {
        processRxQueue(aInstance,events);
    }
    else if (sState == platformRadio_phyState_Transmit)
    {
        handleTxState(aInstance,events);

        /* While we are in TX state, we may have received a packet
         * during the CSMA period.
         */
        processRxQueue(aInstance, events);
    }
    else
    {
        /* not tx or rx (possibly ED scan) */
    }

    if (events & RF_EVENT_RX_DONE)
    {
        /* Restart the background RX command if needed */
        if (sState == platformRadio_phyState_Receive
                && sReceiveCmd.status != PENDING
                && sReceiveCmd.status != ACTIVE
                && sReceiveCmd.status != IEEE_SUSPENDED)
        {
            sReceiveCmdHandle = rfCoreSendReceiveCmd(sRfHandle);
        }

    }


    if (events & RF_EVENT_SLEEP_YIELD)
    {
        if (sState == platformRadio_phyState_Sleep)
        {
            RF_yield(sRfHandle);
        }
    }
}

/**
 * Function documented in platform.h
 * This is called from the main process loop.
 */

void platformRadioProcess(otInstance *aInstance)
{
    UInt events;
    for(;;)
    {
        events = Event_pend(Event_handle(&rfEvents),
                            Event_Id_NONE,
                            (
                                RF_EVENT_TX_DONE
                                | RF_EVENT_ED_SCAN_DONE
                                | RF_EVENT_RX_DONE
                                | RF_EVENT_RX_ACK_DONE
                                | RF_EVENT_SLEEP_YIELD
                                ),
                            BIOS_NO_WAIT);

        /* Semi-race condition can occur.
         *
         * TX_DONE indicates the transmission is complete.
         *
         * The transmitted packet MAY or MAY NOT require an ACK.
         *
         * RX_ACK_DONE indicates the ACK for that transmission is
         * complete.
         *
         * For TX packets requiring an ACK, we need to know that both
         * events have occured, however due to timing (load) these
         * events may be delivered either (a) together at the same
         * time, or (b) delivered as two seperate callbacks.
         *
         * To solve (b) [delivered seperately] we have a simple static
         * flag to remember the TX_DONE event (making it sticky).
         *
         * This sTxIsDone is cleared at the start of transmission, and
         * the end of transmission.
         */

        /* If the TX_DONE event occurred... remember it */
        if (events & RF_EVENT_TX_DONE)
        {
            sTxIsDone = true;
        }
        else if (sTxIsDone)
        {
            events |= RF_EVENT_TX_DONE;
        }

        /* If nothing - then we are done */
        if (events ==  Event_Id_NONE)
        {
            break;
        }

        /* else handle the event */
        handleRadioEvents(aInstance, events);

        /* loop, to determine if there are more radio events. An example
         * is: transmit a polling packet, receive the ACKPEND, followed by
         * the RX data packet, followed by the ack event of the rx packet.
         *
         * Often, a tx done occurs followed by the rx ack
         * the hope here is to resolve both at the same time.
         */
    }
}

