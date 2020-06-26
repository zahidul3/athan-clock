/******************************************************************************

 @file disp_utils.c

 @brief Utility functions for the display interface

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
/* Standard Library Header files */
#include <stddef.h>
#include <string.h>

/* Board Header files */
#include "Board.h"

/* grlib header defines `NDEBUG`, undefine here to avoid a compile warning */
#ifdef NDEBUG
#undef NDEBUG
#endif

/* TIRTOS specific driver header files */
#include <ti/display/Display.h>
#include <ti/display/DisplayExt.h>
#include <ti/grlib/grlib.h>


/******************************************************************************
 Local variables
 *****************************************************************************/
/* handle to the LCD display */
Display_Handle lcdHandle = NULL;
/* handle to the serial display */
Display_Handle serialHandle = NULL;

/******************************************************************************
 External Functions
 *****************************************************************************/

/* refer disp_utils.h */
void DispUtils_open(void)
{
    /* Initialize display and open LCD and serial types of display. */
    Display_Params params;
    Display_Params_init(&params);
    params.lineClearMode = DISPLAY_CLEAR_BOTH;

#if BOARD_DISPLAY_USE_LCD
    lcdHandle = Display_open(Display_Type_LCD, &params);
#endif /* BOARD_DISPLAY_USE_LCD */
    serialHandle = Display_open(Display_Type_UART, &params);
}

#if BOARD_DISPLAY_USE_LCD
/* refer disp_utils.h */
void DispUtils_lcdDraw(const Graphics_Image* pImage)
{
    /* Check if the selected Display type was found and successfully opened */
    if (lcdHandle)
    {
        /*
         * Use the GrLib extension to get the GraphicsLib context object of the
         * LCD, if it is supported by the display type.
         */
        Graphics_Context *context = DisplayExt_getGraphicsContext(lcdHandle);

        if (context) {
            /* Draw splash */
            Graphics_drawImage(context, pImage, 0, 0);
            Graphics_flushBuffer(context);
        }
    }
}
#endif /* BOARD_DISPLAY_USE_LCD */
