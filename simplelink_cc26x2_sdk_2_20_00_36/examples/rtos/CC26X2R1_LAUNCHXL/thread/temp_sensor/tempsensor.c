/******************************************************************************

 @file tempsensor.c

 @brief Temperature Sensor example application

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
#include <stdio.h>

/* OpenThread public API Header files */
#include <openthread/openthread.h>
#include <openthread/coap.h>
#include <openthread/platform/logging.h>
#include <openthread/platform/uart.h>


/* TIRTOS specific header files */
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/BIOS.h>

/* driverlib specific header */
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/aon_batmon.h)

/* POSIX Header files */
#include <sched.h>
#include <pthread.h>

/* OpenThread Internal/Example Header files */
#include "otsupport/otrtosapi.h"
#include "otsupport/otinstance.h"

/* Board Header files */
#include "Board.h"

#include "tempsensor.h"
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

/* Reporting interval in milliseconds */
#define REPORTING_INTERVAL  10000

#define DEFAULT_COAP_HEADER_TOKEN_LEN 2

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
const uint8_t pskd[] = "TMPSENS1";

/******************************************************************************
 Local variables
 *****************************************************************************/

/* clock handle */
Clock_Handle reportClkHandle;

/* clock structure for reporting timer */
Clock_Struct reportClkStruct;

/* ipv6 address to send the reporting temperature to */
otIp6Address thermostatAddress;

/* port to report the temperature to */
uint16_t peerPort = OT_DEFAULT_COAP_PORT;

/* TI-RTOS events structure for passing state to the processing loop */
static Event_Struct tempSensorEvents;

/* OpenThread Stack thread call stack */
static char stack[TASK_CONFIG_TEMPSENSOR_TASK_STACK_SIZE];

/* coap resource for the application */
static otCoapResource coapResource;

/* coap attribute state of the application */
static uint8_t attrTemperature[5] = "70";
static int temperatureValue = 70;

/* coap attribute discriptor for the application */
const attrDesc_t coapAttr = {
    TEMPSENSOR_TEMP_URI,
    (ATTR_READ|ATTR_REPORT),
    attrTemperature,
};

/* Holds the server setup state: 1 indicates CoAP server has been setup */
static bool serverSetup;

/******************************************************************************
 Function Prototype
 *****************************************************************************/

/*  Temperature Sensor processing thread. */
static void *TempSensor_task(void *arg0);
/*  timeout call back for reporting. */
static void reportingTimeoutCB(UArg a0);

/******************************************************************************
 Local Functions
 *****************************************************************************/

/**
 * @brief Configure the timer.
 *
 * @param timeout  time in milliseconds.
 * @return None
 */
static void configureReportingTimer(uint32_t timeout)
{
    Clock_Params clockParams;

    /* Convert clockDuration in milliseconds to ticks. */
    uint32_t clockTicks = timeout * (1000 / Clock_tickPeriod);

    /* Setup parameters. */
    Clock_Params_init(&clockParams);

    /* If period is 0, this is a one-shot timer. */
    clockParams.period = 0;

    /*
     Starts immediately after construction if true, otherwise wait for a
     call to start.
     */
    clockParams.startFlag = false;

    /*/ Initialize clock instance. */
    Clock_construct(&reportClkStruct, reportingTimeoutCB, clockTicks,
                    &clockParams);

    reportClkHandle = Clock_handle(&reportClkStruct);
}

/**
 * @breif Starts the reporting timer.
 *
 * Should be called after the reporting timer has been created, and after the
 * GUA has been registered.
 */
static void startReportingTimer(void)
{
    Clock_start(reportClkHandle);
}

/**
 * @brief Callback function registered with the Coap server.
 *        Processes the coap request from the clients.
 *
 * @param  a0      argument passed by the clock if set up.
 *
 * @return None
 */
static void reportingTimeoutCB(UArg a0)
{
    TempSensor_postEvt(TempSensor_evtReportTemp);
}

/**
 * @brief Reports the temperature to another coap device.
 *
 * @return None
 */
static void tempSensorReport(void)
{
    otError error = OT_ERROR_NONE;
    otMessage *requestMessage = NULL;
    otMessageInfo messageInfo;
    otCoapHeader requestHeader;
    otInstance *instance = OtInstance_get();
    int32_t celsiusTemp;

    /* make sure there is a new temperature reading otherwise just report the previous temperature */
    if(AONBatMonNewTempMeasureReady())
    {
        /* Read the temperature in degrees C from the internal temp sensor */
        celsiusTemp = AONBatMonTemperatureGetDegC();

        /* convert temp to Fahrenheit */
        temperatureValue = (int)((celsiusTemp * 9) / 5) + 32;
        /* convert temperature to string attribute */
        snprintf((char*)attrTemperature, sizeof(attrTemperature), "%d",
                 temperatureValue);
    }

    /* print the reported value to the terminal */
    DISPUTILS_SERIALPRINTF(0, 0, "Reporting Temperature:");
    DISPUTILS_SERIALPRINTF(0, 0, (char*)attrTemperature);


    OtRtosApi_lock();
    otCoapHeaderInit(&requestHeader, OT_COAP_TYPE_NON_CONFIRMABLE, OT_COAP_CODE_POST);
    otCoapHeaderGenerateToken(&requestHeader, DEFAULT_COAP_HEADER_TOKEN_LEN);
    error = otCoapHeaderAppendUriPathOptions(&requestHeader,
                                             THERMOSTAT_TEMP_URI);
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

    OtRtosApi_lock();
    otCoapHeaderSetPayloadMarker(&requestHeader);
    requestMessage = otCoapNewMessage(instance, &requestHeader);
    OtRtosApi_unlock();
    otEXPECT_ACTION(requestMessage != NULL, error = OT_ERROR_NO_BUFS);

    OtRtosApi_lock();
    error = otMessageAppend(requestMessage, attrTemperature,
                            strlen((const char*) attrTemperature));
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

    memset(&messageInfo, 0, sizeof(messageInfo));
    messageInfo.mPeerAddr = thermostatAddress;
    messageInfo.mPeerPort = peerPort;
    messageInfo.mInterfaceId = OT_NETIF_INTERFACE_ID_THREAD;

    OtRtosApi_lock();
    error = otCoapSendRequest(instance, requestMessage, &messageInfo, NULL,
                              NULL);
    OtRtosApi_unlock();

    /* Restart the clock */
    if(Clock_isActive(reportClkHandle) == true)
    {
        Clock_stop(reportClkHandle);
    }
    Clock_start(reportClkHandle);

    exit:

    if(error != OT_ERROR_NONE && requestMessage != NULL)
    {
        OtRtosApi_lock();
        otMessageFree(requestMessage);
        OtRtosApi_unlock();
    }
}

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
    otCoapHeaderSetMessageId(&responseHeader,
                             otCoapHeaderGetMessageId(aHeader));
    otCoapHeaderSetToken(&responseHeader, otCoapHeaderGetToken(aHeader),
                         otCoapHeaderGetTokenLength(aHeader));
    otCoapHeaderSetPayloadMarker(&responseHeader);

    if(OT_COAP_CODE_GET == messageCode)
    {
        responseMessage = otCoapNewMessage((otInstance*)aContext,
                                           &responseHeader);

        otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);
        error = otMessageAppend(responseMessage, attrTemperature,
                                strlen((const char*) attrTemperature));
        otEXPECT(OT_ERROR_NONE == error);

        error = otCoapSendResponse((otInstance*)aContext, responseMessage,
                                   aMessageInfo);
        otEXPECT(OT_ERROR_NONE == error);
    }

exit:

    if(error != OT_ERROR_NONE && responseMessage != NULL)
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
 * @brief Initialize and construct the TIRTOS events.
 *
 * @return None
 */
static void initEvent(void)
{
    Event_construct(&tempSensorEvents, NULL);
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
        TempSensor_postEvt(TempSensor_evtKeyRight);
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
        TempSensor_postEvt(TempSensor_evtNwkJoined);
        break;

    case OT_STACK_EVENT_NWK_JOINED_FAILURE:
        TempSensor_postEvt(TempSensor_evtNwkJoinFailure);
        break;

    case OT_STACK_EVENT_NWK_DATA_CHANGED:
        TempSensor_postEvt(TempSensor_evtNwkSetup);
        break;

    default:
        // do nothing
        break;
    }
}

/**
 * @brief Processes the events.
 *
 * @return None
 */
static int processEvents(void)
{
    UInt events = Event_pend(Event_handle(&tempSensorEvents), Event_Id_NONE,
                             (TempSensor_evtReportTemp | TempSensor_evtNwkSetup |
                              TempSensor_evtAddressValid | TempSensor_evtKeyRight |
                              TempSensor_evtNwkJoined | TempSensor_evtNwkJoinFailure),
                             BIOS_WAIT_FOREVER);

    if(events & TempSensor_evtReportTemp)
    {
        /* perform activity related to the report event. */
        DISPUTILS_SERIALPRINTF( 0, 0, "Report Event received");
        tempSensorReport();
    }

    if(events & TempSensor_evtNwkSetup)
    {
        if (false == serverSetup)
        {
            serverSetup = true;
            (void)setupCoapServer(OtInstance_get(), &coapAttr);

            DISPUTILS_SERIALPRINTF(1, 0, "CoAP server setup done");
        }
    }

    if (events & TempSensor_evtKeyRight)
    {
        if ((!otDatasetIsCommissioned(OtInstance_get())) &&
            (OtStack_joinState() != OT_STACK_EVENT_NWK_JOIN_IN_PROGRESS))
        {
            DISPUTILS_SERIALPRINTF(1, 0, "Joining Nwk ...");
            OtStack_joinNetwork((const char*)pskd);
        }
    }

    if (events & TempSensor_evtNwkJoined)
    {
        DISPUTILS_SERIALPRINTF( 1, 0, "Joined Nwk");
        (void)OtStack_setupNetwork();
    }

    if (events & TempSensor_evtNwkJoinFailure)
    {
        DISPUTILS_SERIALPRINTF(1, 0, "Join Failure");
    }


    if(events & TempSensor_evtAddressValid)
    {
        startReportingTimer();
    }
    return 0;
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

    param.sched_priority = TASK_CONFIG_TEMPSENSOR_TASK_PRIORITY;

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

/*
 * Documented in tempsensor.h
 */
void TempSensor_postEvt(TempSensor_evt event)
{
    Event_post(Event_handle(&tempSensorEvents), event);
}

/*
 * Documented in tempsensor.h
 */
void TempSensor_notifyGUA(otNetifAddress *aAddress)
{
    thermostatAddress = aAddress->mAddress;
    thermostatAddress.mFields.m8[OT_IP6_ADDRESS_SIZE - 1]
        = THERMOSTAT_ADDRESS_LSB;
    Event_post(Event_handle(&tempSensorEvents), TempSensor_evtAddressValid);
}

/*
 * Documented in task_config.h.
 */
void TempSensor_taskCreate(void)
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
                                 TASK_CONFIG_TEMPSENSOR_TASK_STACK_SIZE);
    assert(retc == 0);

    retc = pthread_create(&thread, &pAttrs, TempSensor_task, NULL);
    assert(retc == 0);

    retc = pthread_attr_destroy(&pAttrs);
    assert(retc == 0);
    (void)retc;
}


/**
 *  Temp Sensor processing thread.
 */
void *TempSensor_task(void *arg0)
{
    int ret;
    bool commissioned;

    initEvent();

    KeysUtils_initialize(processKeyChangeCB);

    OtStack_registerCallback(processOtStackEvents);

    DispUtils_open();

    AONBatMonEnable();

    resetPriority();

    DISPUTILS_SERIALPRINTF(0, 0, "Temperature Sensor init!");

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
    }
#endif /* !ALLOW_PRECOMMISSIONED_NETWORK_JOIN */


    configureReportingTimer(REPORTING_INTERVAL);
    /* process events */
    while(1)
    {
        ret = processEvents();
        if(ret != 0)
        {
            break;
        }
    }
    return NULL;
}

