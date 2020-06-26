/******************************************************************************

 @file otstack.c

 @brief Implements the processing thread for the OpenThread Stack.

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
#include OPENTHREAD_PROJECT_CORE_CONFIG_FILE

/* Standard Library Header files */
#include <assert.h>
#include <stddef.h>

/* POSIX Header files */
#include <pthread.h>
#include <sched.h>

/* RTOS header files */
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Event.h>

/* OpenThread public API Header files */
#include <openthread/diag.h>
#include <openthread/openthread.h>

/* OpenThread Internal/Example Header files */
#include "otsupport/otinstance.h"
#include "otsupport/otrtosapi.h"
#include "platform/platform.h"

/* Example/Board Header files */
#include "Board.h"
#include "task_config.h"

/* OpenThread Stack thread */
extern void *OtStack_task(void *arg0);

/******************************************************************************
 Constants and definitions
 *****************************************************************************/

#define OT_STACK_EVENT_SIGNAL_ALARM_PROCESS   Event_Id_00
#define OT_STACK_EVENT_SIGNAL_RADIO_PROCESS   Event_Id_01
#define OT_STACK_EVENT_SIGNAL_TASLETS_PENDING Event_Id_02
#define OT_STACK_EVENT_SIGNAL_UART_PROCESS    Event_Id_03
#define OT_STACK_EVENT_SIGNAL_RANDOM_PROCESS  Event_Id_04
#define OT_STACK_EVENT_SIGNAL_ALARMU_PROCESS  Event_Id_05

/******************************************************************************
 Local variables
 *****************************************************************************/

/* OpenThread Instance */
static otInstance *OtStack_instance;

/* TI-RTOS events structure for passing state to the processing loop */
static Event_Struct OtStack_events;

/* OpenThread Stack thread call stack */
static char OtStack_stack[TASK_CONFIG_OT_TASK_STACK_SIZE];

/**
 * Documented in task_config.h.
 */
void OtStack_taskCreate(void)
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

    retc = pthread_attr_setstack(&pAttrs, (void *)OtStack_stack,
                                 TASK_CONFIG_OT_TASK_STACK_SIZE);
    assert(retc == 0);

    retc = pthread_create(&thread, &pAttrs, OtStack_task, NULL);
    assert(retc == 0);

    retc = pthread_attr_destroy(&pAttrs);
    assert(retc == 0);

    (void) retc;

    GPIO_setConfig(Board_GPIO_BUTTON0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING);
    GPIO_setConfig(Board_GPIO_BUTTON1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING);
}

/**
 * Callback from OpenThread stack to indicate tasklets are pending processing.
 */
void otTaskletsSignalPending(otInstance *aInstance)
{
    (void)aInstance;
    Event_post(Event_handle(&OtStack_events), OT_STACK_EVENT_SIGNAL_TASLETS_PENDING);
}

/**
 * Callback from the alarm module indicating need for processing.
 */
void platformAlarmSignal()
{
    Event_post(Event_handle(&OtStack_events), OT_STACK_EVENT_SIGNAL_ALARM_PROCESS);
}

/**
 * Callback from the alarm module indicating need for processing.
 */
void platformAlarmMicroSignal()
{
    Event_post(Event_handle(&OtStack_events), OT_STACK_EVENT_SIGNAL_ALARMU_PROCESS);
}

/**
 * Callback from the alarm module indicating need for processing.
 */
void platformUartSignal()
{
    Event_post(Event_handle(&OtStack_events), OT_STACK_EVENT_SIGNAL_UART_PROCESS);
}

/**
 * Callback from the radio module indicating need for processing.
 */
void platformRadioSignal()
{
    Event_post(Event_handle(&OtStack_events), OT_STACK_EVENT_SIGNAL_RADIO_PROCESS);
}

/**
 * Callback from the random generator module indicating need for
 * processing.
 */
void platformRandomSignal(void)
{
    Event_post(Event_handle(&OtStack_events), OT_STACK_EVENT_SIGNAL_RANDOM_PROCESS);
}


/**
 * Documented in otsupport/otinstance.h.
 */
otInstance *OtInstance_get(void)
{
    return OtStack_instance;
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

    param.sched_priority = TASK_CONFIG_OT_TASK_PRIORITY;

    ret = pthread_setschedparam(self, policy, &param);
    assert(ret == 0);

    (void) ret;

}

/**
 * Main processing thread for OpenThread Stack.
 */
void *OtStack_task(void *arg0)
{
    /* Initialize the processing loop event structure */
    Event_construct(&OtStack_events, NULL);

    /* Initialize the platform */
    PlatformInit(0, NULL);

    OtRtosApi_init();

    OtStack_instance = otInstanceInitSingle();
    assert(OtStack_instance);

#if OPENTHREAD_ENABLE_DIAG
    otDiagInit(OtStack_instance);
#endif

    /* If both buttons are pressed on boot, reset the OpenThread settings */
    if(GPIO_read(Board_GPIO_BUTTON0) == 0U && GPIO_read(Board_GPIO_BUTTON1) == 0U)
    {
        otInstanceFactoryReset(OtStack_instance);
    }

    resetPriority();

    while (1)
    {
        UInt events;

        events = Event_pend(Event_handle(&OtStack_events),
                            Event_Id_NONE,
                            (OT_STACK_EVENT_SIGNAL_ALARM_PROCESS
                             | OT_STACK_EVENT_SIGNAL_ALARMU_PROCESS
                             | OT_STACK_EVENT_SIGNAL_RADIO_PROCESS
                             | OT_STACK_EVENT_SIGNAL_TASLETS_PENDING
                             | OT_STACK_EVENT_SIGNAL_UART_PROCESS
                             | OT_STACK_EVENT_SIGNAL_RANDOM_PROCESS),
                            BIOS_WAIT_FOREVER);

        if (events & OT_STACK_EVENT_SIGNAL_ALARM_PROCESS)
        {
            OtRtosApi_lock();
            platformAlarmProcess(OtStack_instance);
            OtRtosApi_unlock();
        }
#if OPENTHREAD_CONFIG_ENABLE_PLATFORM_USEC_TIMER
        if (events & OT_STACK_EVENT_SIGNAL_ALARMU_PROCESS)
        {
            OtRtosApi_lock();
            platformAlarmMicroProcess(OtStack_instance);
            OtRtosApi_unlock();
        }
#endif
        if (events & OT_STACK_EVENT_SIGNAL_RADIO_PROCESS)
        {
            OtRtosApi_lock();
            platformRadioProcess(OtStack_instance);
            OtRtosApi_unlock();
        }

        if (events & OT_STACK_EVENT_SIGNAL_TASLETS_PENDING)
        {
            OtRtosApi_lock();
            otTaskletsProcess(OtStack_instance);
            OtRtosApi_unlock();
        }

        if (events & OT_STACK_EVENT_SIGNAL_UART_PROCESS)
        {
            OtRtosApi_lock();
            platformUartProcess();
            OtRtosApi_unlock();
        }

        if (events & OT_STACK_EVENT_SIGNAL_RANDOM_PROCESS)
        {
            OtRtosApi_lock();
            platformRandomProcess();
            OtRtosApi_unlock();
        }

    }
}


