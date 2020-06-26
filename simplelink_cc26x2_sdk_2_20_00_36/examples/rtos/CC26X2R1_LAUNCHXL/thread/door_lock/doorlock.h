/******************************************************************************

 @file doorlock.h

 @brief Door lock example application

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

#ifndef _DOORLOCK_H_
#define _DOORLOCK_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 Constants and definitions
 *****************************************************************************/
/* Door lock uri string */
#define DOORLOCK_URI           "doorlock/"
/* Door lock state string */
#define DOORLOCK_STATE_URI     "doorlock/lockstate"
/* Door unlock state string */
#define DOORLOCK_STATE_LOCK    "lock"
/* Door locked state string */
#define DOORLOCK_STATE_UNLOCK  "unlock"

/**
 * Door lock events.
 */
typedef enum
{
  DoorLock_evtLock           = Event_Id_00, /* Lock event received */
  DoorLock_evtUnlock         = Event_Id_01, /* unlock event received */
  DoorLock_evtNwkSetup       = Event_Id_02, /* openthread network is setup */
  DoorLock_evtKeyLeft        = Event_Id_03, /* Left Key is pressed */
  DoorLock_evtKeyRight       = Event_Id_04, /* Right key is pressed */
  DoorLock_evtNwkJoined      = Event_Id_05, /* Joined the network */
  DoorLock_evtNwkJoinFailure = Event_Id_06  /* Failed joining network */

} DoorLock_evt;

/******************************************************************************
 External functions
 *****************************************************************************/

/**
 * @brief Posts an event to the Door Lock task.
 *
 * @param event event to post.
 * @return None
 */
extern void DoorLock_postEvt(DoorLock_evt event);

#ifdef __cplusplus
}
#endif

#endif /* _DOORLOCK_H_ */
