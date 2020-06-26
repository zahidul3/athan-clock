/******************************************************************************

 @file tempsensor.h

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

#ifndef _TEMPSENSOR_H_
#define _TEMPSENSOR_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 Constants and definitions
 *****************************************************************************/

/* Temperature sensor temperature string */
#define TEMPSENSOR_TEMP_URI     "tempsensor/temperature"

#define THERMOSTAT_TEMP_URI     "thermostat/temperature"

#ifndef THERMOSTAT_ADDRESS_LSB
#define THERMOSTAT_ADDRESS_LSB  7
#endif

/**
 * Temperature Sensor events.
 */
typedef enum
{
    TempSensor_evtReportTemp     = Event_Id_00, /* report timeout event */
    TempSensor_evtNwkSetup       = Event_Id_01, /* openthread network is setup */
    TempSensor_evtAddressValid   = Event_Id_02, /* GUA registered, we may begin reporting */
    TempSensor_evtKeyRight       = Event_Id_03, /* Right key is pressed */
    TempSensor_evtNwkJoined      = Event_Id_04, /* Joined the network */
    TempSensor_evtNwkJoinFailure = Event_Id_05  /* Failed joining network */
} TempSensor_evt;

/******************************************************************************
 External functions
 *****************************************************************************/

/**
 * @brief Posts an event to the Temperature Sensor task.
 *
 * @param event event to post.
 * @return None
 */
extern void TempSensor_postEvt(TempSensor_evt event);

/**
 * @brief Sets the reporting address for the temp sensor task.
 *
 * @param [in] aAddress address with prefix to copy.
 *
 * @note The reporting address least significant byte *will* be set to
 *       @ref THERMOSTAT_ADDRESS_LSB. The address passed into this function
 *       is only used for the prefix assignment.
 */
extern void TempSensor_notifyGUA(otNetifAddress *aAddress);

#ifdef __cplusplus
}
#endif

#endif /* _TEMPSENSOR_H_ */

