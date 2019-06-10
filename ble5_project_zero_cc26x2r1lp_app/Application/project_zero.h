/******************************************************************************

   @file  project_zero.h

   @brief This file contains the Project Zero sample application
        definitions and prototypes.

   Group: CMCU, LPRF
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

#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/drivers/PIN.h>

#include <bcomdef.h>

/*********************************************************************
 *  EXTERNAL VARIABLES
 */
extern PIN_Handle ledPinHandle;
extern PIN_Handle buttonPinHandle;
extern PIN_Handle spiPinHandle;
/*********************************************************************
 * CONSTANTS
 */
#define PZ_RLED_PIN     PIN_ID(6)   //CC26X2R1_LAUNCHXL_PIN_RLED
#define PZ_GLED_PIN     PIN_ID(7)   //CC26X2R1_LAUNCHXL_PIN_GLED
#define LCD_PWM_PIN     PIN_ID(7)   //CC26X2R1_LAUNCHXL_PIN_GLED
#define LCD_SDI_PIN     PIN_ID(9)   //CC26X2R1_LAUNCHXL_SPI1_MOSI
#define LCD_SCL_PIN     PIN_ID(10)  //CC26X2R1_LAUNCHXL_SPI1_CLK
#define LCD_SCS_PIN     PIN_ID(13)  //CC26X2R1_LAUNCHXL_PIN_BTN1
#define PA_DAC_AMP_PIN  PIN_ID(13)  //CC26X2R1_LAUNCHXL_PIN_BTN1
#define PA_DAC_SYNC_PIN PIN_ID(14)  //CC26X2R1_LAUNCHXL_PIN_BTN2
#define LCD_RESET_PIN   PIN_ID(16)  //CC26X2R1_LAUNCHXL_DIO16_TDO
#define LCD_SDC_PIN     PIN_ID(21)  //CC26X2R1_LAUNCHXL_DIO21

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task creation function for the Project Zero.
 */
extern void ProjectZero_createTask(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* APP_H */
