/******************************************************************************

 @file  macconfig.h

 @brief MAC Stack Thread Configuration Data Type

 Group: CMCU, LPRF
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2014-2018, Texas Instruments Incorporated
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
#ifndef MACCONFIG_H
#define MACCONFIG_H

#include <stdint.h>
#include <icall.h>
#include "nvintf.h"
#include "mac_user_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

//! Initialization Successful
#define MACCONFIG_INIT_SUCCESS         0
#define MACCONFIG_INIT_PLATFORM_BLOCK  1

/**
 * @brief       MAC Thread configuration structure used to pass
 *              configuration information to the stack thread.
 */

typedef struct
{
    //! The device's extended address. MACNP only
    uint8_t extendedAddress[8];

    //! Non-volatile function pointers
    NVINTF_nvFuncts_t nvFps;

    //! The Application Thread Entity ID. MACNP only
    ICall_EntityID applicationEntityID;

    //!Initialization failure, non-zero means failure. MACNP only
    uint8_t initFail;

    //! MAC initialization structures
    macUserCfg_t macConfig;
    
} mac_Config_t;

#ifdef __cplusplus
}
#endif

#endif /* MACCONFIG_H */
