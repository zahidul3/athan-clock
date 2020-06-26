/******************************************************************************

 @file  rtos_api.h

 @brief Header for RTOS proxy for stack's interface to the RTOS commands.

 Group: CMCU, LPRF/BTS
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2015-2018, Texas Instruments Incorporated
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

#ifndef RTOS_API_H
#define RTOS_API_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

extern uint32_t *rtosApiTblPtr;

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

// RTOS proxy index for RTOS API
#define SWI_DISABLE               0
#define SWI_RESTORE               1

/*
** RTOS API Proxy
*/

#define RTOS_TABLE(index)      (*((uint32_t *)((uint32_t)rtosApiTblPtr + (uint32_t)((index)*4))))

#define SWI_disable              ((uint16_t (*)(void))                                                               RTOS_TABLE(SWI_DISABLE))
#define SWI_restore              ((void     (*)(uint16_t key))                                                       RTOS_TABLE(SWI_RESTORE))

#ifdef __cplusplus
}
#endif

#endif /* RTOS_API_H */
