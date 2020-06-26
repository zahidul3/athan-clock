/******************************************************************************

 @file thermostat.c

 @brief Thermostat example application

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
#include <stdlib.h>

/* OpenThread public API Header files */
#include <openthread/openthread.h>
#include <openthread/coap.h>
#include <openthread/platform/logging.h>
#include <openthread/platform/uart.h>

/* grlib header defines `NDEBUG`, undefine here to avoid a compile warning */
#ifdef NDEBUG
#undef NDEBUG
#endif

/* TIRTOS specific driver header files */
#include <ti/display/Display.h>
#include <ti/display/DisplayExt.h>
#include <ti/grlib/grlib.h>

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

#include "graphicext.h"
#include "images.h"
#include "thermostat.h"
#include "utils/code_utils.h"

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

/* integer value of character '0' for char to int conversion*/
const uint8_t zeroCharOffset = '0';

/* read attribute */
#define ATTR_READ     0x01
/* write attribute */
#define ATTR_WRITE    0x02
/* report attribute */
#define ATTR_REPORT   0x04
/* Number of attributes in thermostat application */
#define ATTR_COUNT  2
/* period for animation frames for LCD in milliseconds */
#define FRAME_PERIOD 230
/* Maximum number of characters for displayed temp including null terminator*/
#define TEMP_MAX_CHARS 3

/* coap attribute descriptor */
typedef struct
{
    const char*          uriPath; /* attribute URI */
    uint16_t             type; /* type of resource: read only or read write */
    char*                pValue;  /* pointer to value of attribute state */
    otCoapResource       *pAttrCoapResource; /* coap resource for this attr */
    otCoapRequestHandler pAttrHandlerCB;/* call back function for this attr */
} attrDesc_t;

/**
 * Thermostat Modes.
 */
typedef enum
{
  Thermostat_modeCooling,
  Thermostat_modeHeating,
} Thermostat_mode;

/**
 * Pre shared key of the device used during the commissioning
 * stage.
 */
const uint8_t pskd[] = "THERMSTAT1";

/******************************************************************************
 Local variables
 *****************************************************************************/

/* handle to the LCD display */
Display_Handle lcdHandle = NULL;
/* handle to the serial display */
Display_Handle serialHandle = NULL;

/* TI-RTOS events structure for passing state to the processing loop */
static Event_Struct thermostatEvents;

/* OpenThread Stack thread call stack */
static char stack[TASK_CONFIG_THERMOSTAT_TASK_STACK_SIZE];

/* coap resources for the application */
static otCoapResource coapResourceTemp;
static otCoapResource coapResourceSetPt;

/* default coap attribute values of the application */
static char thermostatTemp[TEMP_MAX_CHARS] = "68";
static int temperature = 68;
static char thermostatSetPt[TEMP_MAX_CHARS] = "34";
static int setPoint = 34;
static Thermostat_mode thermostatMode = Thermostat_modeCooling;

/* coap attribute discriptor for the application */
static attrDesc_t coapAttrs[ATTR_COUNT] = {
{
    .uriPath = THERMOSTAT_TEMP_URI,
    .type = (ATTR_READ|ATTR_WRITE),
    .pValue = thermostatTemp,
    .pAttrCoapResource = &coapResourceTemp
},
{
    .uriPath = THERMOSTAT_SET_PT_URI,
    .type = (ATTR_READ|ATTR_WRITE),
    .pValue = thermostatSetPt,
    .pAttrCoapResource = &coapResourceSetPt
}
};

/* Holds the server setup state: 1 indicates CoAP server has been setup */
static bool serverSetup;

/******************************************************************************
 Function Prototype
 *****************************************************************************/

/*  Thermostat processing thread. */
void *Thermostat_task(void *arg0);

/******************************************************************************
 Local Functions
 *****************************************************************************/
/**
 * @brief Calculates the offset for the font sprite buffers based on a char.
 *
 * @param character character to map to font, any char from '0' to '9'.
 * @param charOffset offset for the font buffer.
 *
 * @return None
 */
static uint32_t getCharOffset(uint8_t character, int charOffset)
{
    return (charOffset * (character - zeroCharOffset));
}

/**
 * @brief update the temperature font buffers on the temperature digit sprites.
 *
 * @param temperatureStr current temperature in string form.
 *
 * @return None
 */
static void updateTemperatureLcd(char *temperatureStr)
{
    uint8_t tempLength = strlen(temperatureStr);
    char leftDigitChar = temperatureStr[0];
    char rightDigitChar = temperatureStr[1];
    uint32_t fontPixelOffset;

    if(tempLength == 1)
    {
        leftDigitChar = '0';
        rightDigitChar = temperatureStr[0];
    }

    fontPixelOffset = getCharOffset(leftDigitChar, TEMP_FONT_OFFSET);
    thermostatSpriteList[0].image->pPixel = (uint8_t*)(thermostatTempFontPix +
                                                        fontPixelOffset);
    fontPixelOffset = getCharOffset(rightDigitChar, TEMP_FONT_OFFSET);
    thermostatSpriteList[1].image->pPixel = (uint8_t*)(thermostatTempFontPix +
                                                        fontPixelOffset);

}

/**
 * @brief update the setpoint font buffers on the setpoint digit sprites.
 *
 * @param temperatureStr current setpoint in string form.
 *
 * @return None
 */
static void updateSetPtLcd(char *temperatureStr)
{
    uint8_t tempLength = strlen(temperatureStr);
    char leftDigitChar = temperatureStr[0];
    char rightDigitChar = temperatureStr[1];
    uint32_t fontPixelOffset;
    if(tempLength == 1)
    {
        leftDigitChar = '0';
        rightDigitChar = temperatureStr[0];
    }
    fontPixelOffset = getCharOffset(leftDigitChar, SETPT_FONT_OFFSET);
    thermostatSpriteList[2].image->pPixel = (uint8_t*)(thermostatSetPtFontPix +
                                                   fontPixelOffset);

    fontPixelOffset = getCharOffset(rightDigitChar, SETPT_FONT_OFFSET);
    thermostatSpriteList[3].image->pPixel = (uint8_t*)(thermostatSetPtFontPix +
                                                   fontPixelOffset);
}

/**
 * @brief update the mode of the thermostat by comparing the setpoint and temp.
 *
 * @return None
 */
static void updateThermostatMode(void)
{
    if(thermostatMode == Thermostat_modeHeating && temperature > setPoint)
    {
        thermostatMode = Thermostat_modeCooling;
        thermostatSpriteList[5].image->pPixel = (uint8_t*)(thermostatModePix);
        /* Update the background animation */
        GraphicExt_animateBackground((Graphics_Image*)thermostatBackgroundImage, 0,
                        COOL_ANIMATION_END, true);
    }
    else if(thermostatMode == Thermostat_modeCooling && temperature < setPoint)
    {
        thermostatMode = Thermostat_modeHeating;
        thermostatSpriteList[5].image->pPixel = (uint8_t*)(thermostatModePix +
                                                           MODE_ICON_OFFSET);
       GraphicExt_animateBackground((Graphics_Image*)thermostatBackgroundImage,
                                    COOL_ANIMATION_END,
                                    HEAT_ANIMATION_END, true);
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
static void coapHandleSetPt(void *aContext, otCoapHeader *aHeader,
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
        error = otMessageAppend(responseMessage, thermostatSetPt,strlen((const char*)thermostatSetPt));
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

        if (read < TEMP_MAX_CHARS)
        {
            /* process message */
            Thermostat_postEvt(Thermostat_evtSetPtChange);

            /* update the attribute state */
            strncpy(thermostatSetPt, data, TEMP_MAX_CHARS);
        }

        responseMessage = otCoapNewMessage((otInstance*)aContext,
                                           &responseHeader);

        otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);
        error = otMessageAppend(responseMessage, thermostatSetPt,
                                strlen((const char*)thermostatSetPt));
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
static void coapHandleTemp(void *aContext, otCoapHeader *aHeader,
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
        error = otMessageAppend(responseMessage, thermostatTemp,strlen((const char*)thermostatTemp));
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


        if (read < TEMP_MAX_CHARS)
        {
            /* process message */
            Thermostat_postEvt(Thermostat_evtTempChange);
            /* update the temperature attribute */
            strncpy(thermostatTemp, data, TEMP_MAX_CHARS);
        }

        responseMessage = otCoapNewMessage((otInstance*)aContext,
                                           &responseHeader);

        otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);
        error = otMessageAppend(responseMessage, thermostatTemp,
                                strlen((const char*)thermostatTemp));
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
static otError setupCoapServer(otInstance *aInstance,
                                   const attrDesc_t *attr)
{
    otError error = OT_ERROR_NONE;

    OtRtosApi_lock();
    error = otCoapStart(aInstance, OT_DEFAULT_COAP_PORT);
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

    if(attr->type & (ATTR_READ | ATTR_WRITE))
    {
        attr->pAttrCoapResource->mHandler = attr->pAttrHandlerCB;
        attr->pAttrCoapResource->mUriPath = (const char*)attr->uriPath;
        attr->pAttrCoapResource->mContext = aInstance;

        OtRtosApi_lock();
        error = otCoapAddResource(aInstance, attr->pAttrCoapResource);
        OtRtosApi_unlock();
        otEXPECT(OT_ERROR_NONE == error);
    }

exit:
    return error;
}

/**
 * @brief Display's the image on the LCD screen.
 *
 * @param image pointer to the Graphics Image data structure which has the
 *              information of the image data.
 * @return None
 */
static void flushLcd(void)
{
    GraphicExt_drawSprites(thermostatSpriteList, SPRITE_COUNT, true);
}
/**
 * @brief Initialize and construct the TIRTOS events.
 *
 * @return None
 */
static void initEvent(void)
{
    Event_construct(&thermostatEvents, NULL);
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
        Thermostat_postEvt(Thermostat_evtKeyRight);
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
        Thermostat_postEvt(Thermostat_evtNwkJoined);
        break;

    case OT_STACK_EVENT_NWK_JOINED_FAILURE:
        Thermostat_postEvt(Thermostat_evtNwkJoinFailure);
        break;

    case OT_STACK_EVENT_NWK_DATA_CHANGED:
        Thermostat_postEvt(Thermostat_evtNwkSetup);
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
    UInt events = Event_pend(Event_handle(&thermostatEvents), Event_Id_NONE,
                             (Thermostat_evtTempChange |
                              Thermostat_evtSetPtChange |
                              Thermostat_evtNwkSetup |
                              Thermostat_evtGraphicExt |
                              Thermostat_evtKeyRight |
                              Thermostat_evtNwkJoined |
                              Thermostat_evtNwkJoinFailure),
                             BIOS_WAIT_FOREVER);

    if (events & Thermostat_evtTempChange)
    {
        /* perform activity related to the lock event. */
        if (serialHandle)
        {
            Display_printf(serialHandle, 0, 0, "Temperature Event received:");
            Display_printf(serialHandle, 0, 0, thermostatTemp);
        }
        /* convert thermostat temperature to an integer*/
        temperature = atoi(thermostatTemp);
        /* update the Lcd screen with new temperature value and mode */
        updateTemperatureLcd(thermostatTemp);
        updateThermostatMode();
        flushLcd();
    }

    if (events & Thermostat_evtSetPtChange)
    {
       /* perform activity related to the un lock event */
        if (serialHandle)
        {
            Display_printf(serialHandle, 0, 0, "Setpoint Event received:");
            Display_printf(serialHandle, 0, 0, thermostatSetPt);
        }
        /* convert thermostat set point to an integer*/
        setPoint = atoi(thermostatSetPt);
        /* update the Lcd screen with new setpoint value and mode */
        updateSetPtLcd(thermostatSetPt);
        updateThermostatMode();
        flushLcd();
    }

    if (events & Thermostat_evtNwkSetup)
    {
        if (false == serverSetup)
        {
            /* set callback functions */
            coapAttrs[0].pAttrHandlerCB = &coapHandleTemp;
            coapAttrs[1].pAttrHandlerCB = &coapHandleSetPt;
            /* register coap attributes */
            (void)setupCoapServer(OtInstance_get(), &coapAttrs[0]);
            (void)setupCoapServer(OtInstance_get(), &coapAttrs[1]);

            serverSetup = true;

            /* Initialize the LCD screen */
            updateTemperatureLcd(thermostatTemp);
            updateSetPtLcd(thermostatSetPt);

            /* Flush the sprites to the LCD */
            flushLcd();
            GraphicExt_animateBackground((Graphics_Image*)thermostatBackgroundImage,
                                                0,
                                                COOL_ANIMATION_END, true);
            updateThermostatMode();
        }
    }

    if (events & Thermostat_evtGraphicExt)
    {
        /* process the GraphicExt event */
        GraphicExt_processEvt();
    }

    if (events & Thermostat_evtKeyRight)
    {
        if ((!otDatasetIsCommissioned(OtInstance_get())) &&
            (OtStack_joinState() != OT_STACK_EVENT_NWK_JOIN_IN_PROGRESS))
        {
            if (serialHandle)
            {
                Display_printf(serialHandle, 1, 0, "Joining Nwk ...");
            }

            if (lcdHandle)
            {
                Display_printf(lcdHandle, 1, 0, "Joining Nwk ...");
            }

            OtStack_joinNetwork((const char*)pskd);
        }
    }

    if (events & Thermostat_evtNwkJoined)
    {
        if (serialHandle)
        {
            Display_printf(serialHandle, 1, 0, "Joined Nwk");
        }

        if (lcdHandle)
        {
            Display_printf(lcdHandle, 1, 0, "Joined Nwk");
        }

        (void)OtStack_setupNetwork();
    }

    if (events & Thermostat_evtNwkJoinFailure)
    {
        if (serialHandle)
        {
            Display_printf(serialHandle, 1, 0, "Join Failure");
        }

        if (lcdHandle)
        {
            Display_printf(lcdHandle, 1, 0, "Join Failure");
        }
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

    param.sched_priority = TASK_CONFIG_THERMOSTAT_TASK_PRIORITY;

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

/* Documented in thermostat.h */
void Thermostat_postEvt(Thermostat_evt event)
{
    Event_post(Event_handle(&thermostatEvents), event);
}

/**
 * Documented in task_config.h.
 */
void Thermostat_taskCreate(void)
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
                                 TASK_CONFIG_THERMOSTAT_TASK_STACK_SIZE);
    assert(retc == 0);

    retc = pthread_create(&thread, &pAttrs, Thermostat_task, NULL);
    assert(retc == 0);

    retc = pthread_attr_destroy(&pAttrs);
    assert(retc == 0);
    (void)retc;
}

/**
 *  Thermostat processing thread.
 */
void *Thermostat_task(void *arg0)
{
    int ret;
    bool commissioned;

    initEvent();

    KeysUtils_initialize(processKeyChangeCB);

    OtStack_registerCallback(processOtStackEvents);

    /* Initialize display and open LCD and serial types of display. */
    Display_Params params;
    Display_Params_init(&params);
    params.lineClearMode = DISPLAY_CLEAR_BOTH;

    lcdHandle = Display_open(Display_Type_LCD, &params);
    serialHandle = Display_open(Display_Type_UART, &params);

    resetPriority();

    if (serialHandle)
    {
        Display_printf(serialHandle, 0, 0, "Thermostat init!");
    }

    /* Initialize the extended graphics library */
    GraphicExt_initAnimationModule(&lcdHandle,
                                    &thermostatEvents,
                                    Thermostat_evtGraphicExt,
                                    FRAME_PERIOD);
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

        if (serialHandle)
        {
            Display_printf(serialHandle, 2, 0, "pskd: %s", pskd);
            Display_printf(serialHandle, 3, 0, "EUI64: 0x%02x%02x%02x%02x%02x%02x%02x%02x",
                           extAddress.m8[0], extAddress.m8[1], extAddress.m8[2],
                           extAddress.m8[3], extAddress.m8[4], extAddress.m8[5],
                           extAddress.m8[6], extAddress.m8[7]);
        }

        if (lcdHandle)
        {
            Display_printf(lcdHandle, 2, 0, "pskd:");
            Display_printf(lcdHandle, 3, 0, "%s", pskd);
            Display_printf(lcdHandle, 4, 0, "EUI64:");
            Display_printf(lcdHandle, 5, 0, "%02x%02x%02x%02x%02x%02x%02x%02x",
                           extAddress.m8[0], extAddress.m8[1], extAddress.m8[2],
                           extAddress.m8[3], extAddress.m8[4], extAddress.m8[5],
                           extAddress.m8[6], extAddress.m8[7]);
        }
    }
#endif /* !ALLOW_PRECOMMISSIONED_NETWORK_JOIN */

    /* process events */
    while (1)
    {
        ret = processEvents();
        if(ret != 0)
        {
            break;
        }
    }
    return NULL;
}

