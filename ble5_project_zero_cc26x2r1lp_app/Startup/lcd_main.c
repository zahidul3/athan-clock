/******************************************************************************

   @file  lcd_main.c

   @brief main entry of the BLE stack sample application.

   Group: CMCU, LPRF
   Target Device: CC2652

 ******************************************************************************
   
 Copyright (c) 2013-2018, Texas Instruments Incorporated
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

/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>

#include <ti/drivers/timer/GPTimerCC26XX.h>
#include <ti/sysbios/BIOS.h>
#include <icall.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

// Comment this in to use xdc.runtime.Log, but also remove UartLog_init below.
//#include <xdc/runtime/Log.h>
#include <ti/common/cc26xx/uartlog/UartLog.h> // Comment out to use xdc Log.

#include "project_zero.h"
#include "audiotest.h"

/* GrLib Includes */
#include "grlib.h"
#include "button.h"
#include "imageButton.h"
#include "radioButton.h"
#include "checkbox.h"
#include "LcdDriver/kitronix320x240x16_ssd2119_spi.h"
#include "images/images.h"
//#include "touch_P401R.h"

#include "lcd_main.h"

//Touch screen context
//touch_context g_sTouchContext;
Graphics_ImageButton primitiveButton;
Graphics_ImageButton imageButton;
Graphics_Button yesButton;
Graphics_Button noButton;

// Graphic library context
Graphics_Context g_sContext;

//Flag to know if a demo was run
bool g_ranDemo = false;

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */
// Task configuration
#define PZ_LCD_TASK_PRIORITY                     1

#ifndef PZ_LCD_TASK_STACK_SIZE
#define PZ_LCD_TASK_STACK_SIZE                   2048
#endif

#define LCD_WRITE_EVT                           Event_Id_15

// Bitwise OR of all RTOS events to pend on
#define LCD_ALL_EVENTS                        (LCD_WRITE_EVT)
/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */
// Task configuration
Task_Struct LCDTask;
#if defined __TI_COMPILER_VERSION__
#pragma DATA_ALIGN(appLCDTaskStack, 8)
#else
#pragma data_alignment=8
#endif
uint8_t appLCDTaskStack[PZ_LCD_TASK_STACK_SIZE];


// Event globally used to post local events and pend on system and
// local events.
static SyncHandle syncEventLCD = NULL;

/*******************************************************************************
 * EXTERNS
 */
static void ProjectLCD_taskFxn(UArg a0,
                                UArg a1);

void drawMainMenu(void);

void SetLCDwriteEvent(void)
{
    if(syncEventLCD != NULL)
        Event_post(syncEventLCD, LCD_WRITE_EVT);
}
/*
 *  ======== mainThread ========
 */
static void ProjectLCD_taskFxn(UArg a0, UArg a1)
{
    syncEventLCD = Event_create(NULL, NULL);

    Log_info0("In LCD task function");
    // LCD setup using Graphics Library API calls
    //SetLCDwriteEvent();
    /* Loop forever echoing */
    while (1) {

        uint32_t events;

        events = Event_pend(syncEventLCD, Event_Id_NONE, LCD_ALL_EVENTS, ICALL_TIMEOUT_FOREVER);
        if (events)
        {
            if (events & LCD_WRITE_EVT)
            {
                Log_info0("In LCD_WRITE_EVT");
                Kitronix320x240x16_SSD2119Init();
                Graphics_initContext(&g_sContext, &g_sKitronix320x240x16_SSD2119);
                Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
                Graphics_setFont(&g_sContext, &g_sFontCmss20b);
                Graphics_clearDisplay(&g_sContext);

                //touch_initInterface();
                Log_info0("Drawing main menu...");
                drawMainMenu();
            }
        }
    }

}

void drawMainMenu(void)
{
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, "MSP Graphics Library Demo",
        AUTO_STRING_LENGTH,
        159,
        15,
        TRANSPARENT_TEXT);

    // Draw TI banner at the bottom of screnn
    Graphics_drawImage(&g_sContext,
        &TI_platform_bar_red4BPP_UNCOMP,
        0,
        Graphics_getDisplayHeight(&g_sContext) - TI_platform_bar_red4BPP_UNCOMP.ySize);

    // Draw Primitives image button
    //Graphics_drawImageButton(&g_sContext, &primitiveButton);

    // Draw Images image button
    //Graphics_drawImageButton(&g_sContext, &imageButton);
}

/*******************************************************************************
 * @fn          ProjectLCD_createTask
 *
 * @brief       Application Main
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void ProjectLCD_createTask(void)
{
    Task_Params taskParams;

    // Configure task
    Task_Params_init(&taskParams);
    taskParams.stack = appLCDTaskStack;
    taskParams.stackSize = PZ_LCD_TASK_STACK_SIZE;
    taskParams.priority = PZ_LCD_TASK_PRIORITY;

    Task_construct(&LCDTask, ProjectLCD_taskFxn, &taskParams, NULL);
}
/*******************************************************************************
 */
