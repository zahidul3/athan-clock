/******************************************************************************

 @file shade.c

 @brief shade example application

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

/******************************************************************************
 Includes
 *****************************************************************************/
#include <openthread/config.h>
#include <openthread-core-config.h>

/* Standard Library Header files */
#include <assert.h>
#include <stddef.h>
#include <string.h>

/* OpenThread public API Header files */
#include <openthread/openthread.h>
#include <openthread/coap.h>
#include <openthread/platform/logging.h>
#include <openthread/platform/uart.h>

/* TIRTOS specific header files */
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/BIOS.h>

/* POSIX Header files */
#include <sched.h>
#include <pthread.h>

/* OpenThread Internal/Example Header files */
#include "otsupport/otrtosapi.h"
#include "otsupport/otinstance.h"

/* Board Header files */
#include "Board.h"

#include "images.h"
#include "shade.h"
#include "utils/code_utils.h"

#include "disp_utils.h"
#include "keys_utils.h"
#include "otstack.h"

/* Private configuration Header files */
#include "task_config.h"

#if (OPENTHREAD_ENABLE_APPLICATION_COAP == 0)
#error "OPENTHREAD_ENABLE_APPLICATION_COAP needs to be defined and set to 1"
#endif

/******************************************************************************
 Constants and definitions
 *****************************************************************************/

/* read attribute */
#define ATTR_READ     0x01
/* write attribute */
#define ATTR_WRITE    0x02
/* report attribute */
#define ATTR_REPORT   0x04

/* coap attribute descriptor */
typedef struct
{
    const char*    uriPath; /* attribute URI */
    uint16_t       type;    /* type of resource: read only or read write */
    uint8_t*       pValue;  /* pointer to value of attribute state */

} attrDesc_t;

/**
 * Pre shared key of the device used during the commissioning
 * stage.
 */
const uint8_t pskd[] = "SHADEEX1";

/******************************************************************************
 Local variables
 *****************************************************************************/
/* TI-RTOS events structure for passing state to the processing loop */
static Event_Struct shadeEvents;

/* OpenThread Stack thread call stack */
static char stack[TASK_CONFIG_SHADE_TASK_STACK_SIZE];

/* coap resource for the application */
static otCoapResource coapResource;

/* coap attribute state of the application */
static uint8_t attrState[15] = SHADE_STATE_OPEN;

/* coap attribute discriptor for the application */
const attrDesc_t coapAttr = {
    SHADE_STATE_URI,
    (ATTR_READ|ATTR_WRITE),
    attrState,
};

/* Holds the server setup state: True indicates CoAP server has been setup */
static bool serverSetup;

/******************************************************************************
 Function Prototype
 *****************************************************************************/

/*  Shade processing thread. */
void *Shade_task(void *arg0);

/******************************************************************************
 Local Functions
 *****************************************************************************/

/**
 * @brief Callback function registered with the Coap server.
 *        Processes the coap request from the clients.
 *
 * @param  aContext      A pointer to the context information.
 * @param  aHeader       A pointer to the CoAP header.
 * @param  aMessage      A pointer to the message.
 * @param  aMessageInfo  A pointer to the message info.
 *
 * @return None
 */
static void coapHandleServer(void *aContext, otCoapHeader *aHeader,
                             otMessage *aMessage,
                             const otMessageInfo *aMessageInfo)
{
    otError error = OT_ERROR_NONE;
    otCoapHeader responseHeader;
    otMessage *responseMessage = NULL;
    otCoapCode responseCode = OT_COAP_CODE_CHANGED;
    otCoapCode messageCode = otCoapHeaderGetCode(aHeader);

    otCoapHeaderInit(&responseHeader, OT_COAP_TYPE_ACKNOWLEDGMENT, responseCode);
    otCoapHeaderSetMessageId(&responseHeader, otCoapHeaderGetMessageId(aHeader));
    otCoapHeaderSetToken(&responseHeader, otCoapHeaderGetToken(aHeader),
                         otCoapHeaderGetTokenLength(aHeader));
    otCoapHeaderSetPayloadMarker(&responseHeader);

    if(OT_COAP_CODE_GET == messageCode)
    {
        responseMessage = otCoapNewMessage((otInstance*)aContext,
                                           &responseHeader);

        otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);
        error = otMessageAppend(responseMessage, attrState,strlen((const char*)attrState));
        otEXPECT(OT_ERROR_NONE == error);

        error = otCoapSendResponse((otInstance*)aContext, responseMessage,
                                   aMessageInfo);
        otEXPECT(OT_ERROR_NONE == error);
    }
    else if(OT_COAP_CODE_POST == messageCode)
    {
        char data[32];
        uint16_t offset = otMessageGetOffset(aMessage);
        uint16_t read = otMessageRead(aMessage, offset, data, sizeof(data) - 1);
        data[read] = '\0';

        /* process message */
        if(strcmp(SHADE_STATE_OPEN, data) == 0)
        {
            /* send open event */
            Shade_postEvt(Shade_evtOpen);
        }
        else if(strcmp(SHADE_STATE_CLOSED, data) == 0)
        {
            /* send close event */
            Shade_postEvt(Shade_evtClosed);
        }
        else if(strcmp(SHADE_STATE_DRAWN, data) == 0)
        {
            /* send drawn event */
            Shade_postEvt(Shade_evtDrawn);
        }
        else
        {
            /* no valid body, fail without response */
            otEXPECT_ACTION(false, error = OT_ERROR_NO_BUFS);
        }

        /* update the attribute state */
        strcpy((char *)attrState, data);

        responseMessage = otCoapNewMessage((otInstance*)aContext,
                                           &responseHeader);

        otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);
        error = otMessageAppend(responseMessage, attrState, strlen((const char*)attrState));
        otEXPECT(OT_ERROR_NONE == error);

        error = otCoapSendResponse((otInstance*)aContext,
                                   responseMessage, aMessageInfo);
        otEXPECT(OT_ERROR_NONE == error);
    }

exit:

    if (error != OT_ERROR_NONE && responseMessage != NULL)
    {
        otMessageFree(responseMessage);
    }
}

/**
 * @brief sets up the application coap server.
 *
 * @param aInstance A pointer to the context information.
 * @param attr      Attribute data
 *
 * @return OT_ERROR_NONE if successful, else error code
 */
static otError setupCoapServer(otInstance *aInstance, const attrDesc_t *attr)
{
    otError error = OT_ERROR_NONE;

    OtRtosApi_lock();
    error = otCoapStart(aInstance, OT_DEFAULT_COAP_PORT);
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

    if(attr->type & (ATTR_READ | ATTR_WRITE))
    {
        coapResource.mHandler = &coapHandleServer;
        coapResource.mUriPath = (const char*)attr->uriPath;
        coapResource.mContext = aInstance;

        OtRtosApi_lock();
        error = otCoapAddResource(aInstance, &(coapResource));
        OtRtosApi_unlock();
        otEXPECT(OT_ERROR_NONE == error);
    }

exit:
    return error;
}


/**
 * @brief Handles the key press events.
 *
 * @param keysPressed identifies which keys were pressed
 * @return None
 */
static void processKeyChangeCB(uint8_t keysPressed)
{
    if (keysPressed & KEYS_RIGHT)
    {
        Shade_postEvt(Shade_evtKeyRight);
    }
}

/**
 * @brief Processes the OT stack events
 *
 * @param evt Event identifier.
 * @return None
 */
static void processOtStackEvents(uint8_t evt)
{
    switch (evt)
    {
    case OT_STACK_EVENT_NWK_JOINED:
        Shade_postEvt(Shade_evtNwkJoined);
        break;

    case OT_STACK_EVENT_NWK_JOINED_FAILURE:
        Shade_postEvt(Shade_evtNwkJoinFailure);
        break;

    case OT_STACK_EVENT_NWK_DATA_CHANGED:
        Shade_postEvt(Shade_evtNwkSetup);
        break;

    default:
        // do nothing
        break;
    }
}

/**
 * @brief Initialize and construct the TIRTOS events.
 *
 * @return None
 */
static void initEvent(void)
{
    Event_construct(&shadeEvents, NULL);
}

/**
 * @brief Processes the events.
 *
 * @return None
 */
static void processEvents(void)
{
    UInt events = Event_pend(Event_handle(&shadeEvents), Event_Id_NONE,
                             (Shade_evtOpen | Shade_evtClosed |
                              Shade_evtDrawn | Shade_evtNwkSetup |
                              Shade_evtKeyRight | Shade_evtNwkJoined |
                              Shade_evtNwkJoinFailure),
                             BIOS_WAIT_FOREVER);

    if (events & Shade_evtOpen)
    {
        /* perform activity related to the shade open event. */
        DISPUTILS_SERIALPRINTF( 0, 0, "Shade Open Event received");
        DispUtils_lcdDraw(&Images_shadeOpen);
    }

    if (events & Shade_evtClosed)
    {
        /* perform activity related to the shade closed event */
        DISPUTILS_SERIALPRINTF( 0, 0, "Shade close Event received");
        DispUtils_lcdDraw(&Images_shadeClosed);
    }

    if (events & Shade_evtDrawn)
    {
       /* perform activity related to the shade drawn event */
        DISPUTILS_SERIALPRINTF(0, 0, "Shade drawn Event received");
        DispUtils_lcdDraw(&Images_shadeDrawn);
    }

    if (events & Shade_evtNwkSetup)
    {
        if (false == serverSetup)
        {
            serverSetup = true;
            (void)setupCoapServer(OtInstance_get(), &coapAttr);

            /* display unlock image on LCD */
            DISPUTILS_SERIALPRINTF(1, 0, "CoAP server setup done");
            DispUtils_lcdDraw(&Images_shadeOpen);
        }
    }

    if (events & Shade_evtKeyRight)
    {
        if ((!otDatasetIsCommissioned(OtInstance_get())) &&
            (OtStack_joinState() != OT_STACK_EVENT_NWK_JOIN_IN_PROGRESS))
        {
            DISPUTILS_SERIALPRINTF(1, 0, "Joining Nwk ...");
            DISPUTILS_LCDPRINTF(1, 0, "Joining Nwk ...");

            OtStack_joinNetwork((const char*)pskd);
        }
    }

    if (events & Shade_evtNwkJoined)
    {
        DISPUTILS_SERIALPRINTF( 1, 0, "Joined Nwk");
        DISPUTILS_LCDPRINTF(1, 0, "Joined Nwk");

        (void)OtStack_setupNetwork();
    }

    if (events & Shade_evtNwkJoinFailure)
    {
        DISPUTILS_SERIALPRINTF(1, 0, "Join Failure");
        DISPUTILS_LCDPRINTF(1, 0, "Join Failure");
    }
}

/**
 * Return thread priority after initialization.
 */
static void resetPriority(void)
{
    pthread_t           self;
    int                 policy;
    struct sched_param  param;
    int                 ret;

    self = pthread_self();

    ret = pthread_getschedparam(self, &policy, &param);
    assert(ret == 0);

    param.sched_priority = TASK_CONFIG_SHADE_TASK_PRIORITY;

    ret = pthread_setschedparam(self, policy, &param);
    assert(ret == 0);
    (void)ret;
}


/******************************************************************************
 External Functions
 *****************************************************************************/

#if (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_APP)
/**
 * Documented in openthread/platform/logging.h.
 */
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    (void)aLogLevel;
    (void)aLogRegion;
    (void)aFormat;
    /* Do nothing. */
}
#endif

/**
 * Documented in openthread/platform/uart.h.
 */
void otPlatUartReceived(const uint8_t *aBuf, uint16_t aBufLength)
{
    (void)aBuf;
    (void)aBufLength;
    /* Do nothing. */
}

/**
 * Documented in openthread/platform/uart.h.
 */
void otPlatUartSendDone(void)
{
    /* Do nothing. */
}

/* Documented in shade.h */
void Shade_postEvt(Shade_evt_t event)
{
    Event_post(Event_handle(&shadeEvents), event);
}

/**
 * Documented in task_config.h.
 */
void Shade_taskCreate(void)
{
    pthread_t           thread;
    pthread_attr_t      pAttrs;
    struct sched_param  priParam;
    int                 retc;

    retc = pthread_attr_init(&pAttrs);
    assert(retc == 0);

    retc = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
    assert(retc == 0);

    priParam.sched_priority = sched_get_priority_max(SCHED_OTHER);
    retc = pthread_attr_setschedparam(&pAttrs, &priParam);
    assert(retc == 0);

    retc = pthread_attr_setstack(&pAttrs, (void *)stack,
                                 TASK_CONFIG_SHADE_TASK_STACK_SIZE);
    assert(retc == 0);

    retc = pthread_create(&thread, &pAttrs, Shade_task, NULL);
    assert(retc == 0);

    retc = pthread_attr_destroy(&pAttrs);
    assert(retc == 0);

    (void)retc;
}

/**
 *  Shade processing thread.
 */
void *Shade_task(void *arg0)
{
    bool commissioned;
    initEvent();

    KeysUtils_initialize(processKeyChangeCB);

    OtStack_registerCallback(processOtStackEvents);

    DispUtils_open();

    resetPriority();

    DISPUTILS_SERIALPRINTF(0, 0, "Shade init!");

#ifndef ALLOW_PRECOMMISSIONED_NETWORK_JOIN
    OtRtosApi_lock();
    commissioned = otDatasetIsCommissioned(OtInstance_get());
    OtRtosApi_unlock();
    if (false == commissioned)
    {
        otExtAddress extAddress;

        OtRtosApi_lock();
        otLinkGetFactoryAssignedIeeeEui64(OtInstance_get(), &extAddress);
        OtRtosApi_unlock();

        DISPUTILS_SERIALPRINTF(2, 0, "pskd: %s", pskd);
        DISPUTILS_SERIALPRINTF(3, 0, "EUI64: 0x%02x%02x%02x%02x%02x%02x%02x%02x",
                               extAddress.m8[0], extAddress.m8[1], extAddress.m8[2],
                               extAddress.m8[3], extAddress.m8[4], extAddress.m8[5],
                               extAddress.m8[6], extAddress.m8[7]);

        DISPUTILS_LCDPRINTF(2, 0, "pskd:");
        DISPUTILS_LCDPRINTF(3, 0, "%s", pskd);
        DISPUTILS_LCDPRINTF(4, 0, "EUI64:");
        DISPUTILS_LCDPRINTF(5, 0, "%02x%02x%02x%02x%02x%02x%02x%02x",
                            extAddress.m8[0], extAddress.m8[1], extAddress.m8[2],
                            extAddress.m8[3], extAddress.m8[4], extAddress.m8[5],
                            extAddress.m8[6], extAddress.m8[7]);

    }
#endif /* !ALLOW_PRECOMMISSIONED_NETWORK_JOIN */

   /* process events */
    while (1)
    {
        processEvents();
    }
}

