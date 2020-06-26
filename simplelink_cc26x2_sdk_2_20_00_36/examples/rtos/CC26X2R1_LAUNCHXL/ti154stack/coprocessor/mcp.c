/******************************************************************************

 @file  mcp.c

 @brief TIMAC Co-Processor Application

 Group: WCS LPC
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

/******************************************************************************
 Includes
 *****************************************************************************/
#include <string.h>

#include "icall.h"
#include "api_mac.h"
#include "npi_task.h"

#include "mcp.h"
#include "mt.h"
#include "mt_mac.h"

#include "board.h"
#include "board_led.h"
#include "board_lcd.h"
#if defined(CC13XX_LAUNCHXL) || defined(CC13X2R1_LAUNCHXL)
#include "board_gpio.h"
#endif
#include "timer.h"

/******************************************************************************
 External variables
 *****************************************************************************/
/* ICall thread entity for ApiMac */
extern ICall_EntityID ApiMac_appEntity;

/* ICall thread entity for NPI */
extern ICall_EntityID npiAppEntityID;

/******************************************************************************
 Local variables
 *****************************************************************************/
/* Number of requests sent to MAC */
static uint16_t numRxMsgs = 0;

/* Number of responses sent to host */
static uint16_t numTxMsgs = 0;

/******************************************************************************
 Local function prototypes
 *****************************************************************************/
static void MCP_init(void);
static void processMsg(uint16_t p1, uint16_t p2, void *pMsg);
static void relayTxMsg(void *pMsg);
static void relayRxMsg(void *pMsg);

/******************************************************************************
 ApiMac MAC callback table
 *****************************************************************************/
/*!
 API MAC Callback table
 */
static ApiMac_callbacks_t macCallbacks =
{
    /*! Associate Indication callback */
    MtMac_AssociateInd,
    /*! Associate Confirmation callback */
    MtMac_AssociateCnf,
    /*! Disassociate Indication callback */
    MtMac_DisassociateInd,
    /*! Disassociate Confirmation callback */
    MtMac_DisassociateCnf,
    /*! Beacon Notify Indication callback */
    MtMac_BeaconNotifyInd,
    /*! Orphan Indication callback */
    MtMac_OrphanInd,
    /*! Scan Confirmation callback */
    MtMac_ScanCnf,
    /*! Start Confirmation callback */
    MtMac_StartCnf,
    /*! Sync Loss Indication callback */
    MtMac_SyncLossInd,
    /*! Poll Confirm callback */
    MtMac_PollCnf,
    /*! Comm Status Indication callback */
    MtMac_CommStatusInd,
    /*! Poll Indication Callback */
    MtMac_PollInd,
    /*! Data Confirmation callback */
    MtMac_DataCnf,
    /*! Data Indication callback */
    MtMac_DataInd,
    /*! Purge Confirm callback */
    MtMac_PurgeCnf,
    /*! WiSUN Async Indication callback */
    MtMac_AsyncInd,
    /*! WiSUN Async Confirmation callback */
    MtMac_AsyncCnf,
    /*! Unprocessed message callback */
    processMsg
};

/******************************************************************************
 Public Functions
 *****************************************************************************/
/*!
 Application task processing.

 Public function defined in mcp.h
 */
void MCP_task(void)
{
    /* Initialize this application */
    MCP_init();

    while(true)
    {
        /* Wait for MAC/NPI messages */
        ApiMac_processIncoming();
    }
}

/******************************************************************************
 Local Functions
 *****************************************************************************/
/*!
 * @brief   Initialize this application
 */
static void MCP_init(void)
{
    /* Initialize the MAC interface, do not enable FH */
    (void)ApiMac_init(false);
    /* Register the MAC Callbacks */
    ApiMac_registerCallbacks(&macCallbacks);

    /* Configure and create the NPI task */
    NPITask_createTask(0);
    /* Register callback function for incoming NPI messages */
    NPITask_registerIncomingRXEventAppCB((npiIncomingEventCBack_t)relayRxMsg,
                                                                  INTERCEPT);
    /* Register callback function for outgoing NPI messages */
    NPITask_registerIncomingTXEventAppCB((npiIncomingEventCBack_t)relayTxMsg,
                                                                  ECHO);

    /* Start up the MT message handler */
    MT_init(npiAppEntityID, ICALL_SERVICE_CLASS_NPI);

    /* Turn off all LEDs */
    Board_Led_initialize();
#if defined(CC13XX_LAUNCHXL) || defined(CC13X2R1_LAUNCHXL)
    /* Init any RF GPIOs */
    Board_Gpio_initialize();
#endif
    /* Initialize the LCD */
    Board_LCD_open();
    LCD_WRITE_STRING("Texas Instruments", 1);
    LCD_WRITE_STRING("TIMAC Co-Processor", 2);
}

/*!
 * @brief   Process a message from ApiMac
 *
 * @param   entityID - ICall application entity ID
 * @param   param2   - not used
 * @param   pMsg     - pointer to incoming message buffer
 */
static void processMsg(uint16_t entityID, uint16_t param2, void *pMsg)
{
    /* Intentionally not used */
    (void)param2;

    /* Verify it's an NPI message */
    if(entityID == npiAppEntityID)
    {
        /* Hand it to MT incoming message handler */
        MT_processIncoming(pMsg);

        /* Count an incoming MT message */
        numRxMsgs += 1;

        LCD_WRITE_STRING_VALUE("MT RX Msgs:", numRxMsgs, 10, 4);
        Board_Led_control(board_led_type_LED1, board_led_state_BLINK);
    }
}

/*!
 * @brief   Relay an NPI task RX message to ApiMac task,
 *          allows MT message processing at ApiMac task context
 *
 * @param   pMsg - pointer to incoming message buffer
 */
static void relayRxMsg(void *pMsg)
{
    NPIMSG_msg_t *pNpiMsg = pMsg;

    /* Relay the message to ApiMac */
    ICall_send(npiAppEntityID, ApiMac_appEntity,
               ICALL_MSG_FORMAT_KEEP, pNpiMsg->pBuf);

    ICall_free(pNpiMsg);
}

/*!
 * @brief   Count an NPI task TX message from ApiMac task
 *
 * @param   pMsg - pointer to incoming message buffer
 */
static void relayTxMsg(void *pMsg)
{
    /* Intentionally not used */
    (void)pMsg;

    /* Count an outgoing MT message */
    numTxMsgs += 1;

    LCD_WRITE_STRING_VALUE("MT TX Msgs:", numTxMsgs, 10, 5);
    Board_Led_control(board_led_type_LED2, board_led_state_BLINK);
}
