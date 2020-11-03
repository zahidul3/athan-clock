/******************************************************************************

 @file images.c

 @brief Image data used to display on the LCD screen.

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
#include "images.h"

/******************************************************************************
 Constants and Definitions
 *****************************************************************************/
/** Door lock image data */
static const uint8_t doorLockImgData[] =
{
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xe0, 0x07, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xc0, 0x1f, 0xf8, 0x03, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x80, 0x3f, 0xfc, 0x01, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x80, 0x7f, 0xfe, 0x01, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x01, 0xff, 0xff, 0x80, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x01, 0xff, 0xff, 0x80, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x1e, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x1e, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x1e, 0x20, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x1e, 0x30, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x1f, 0x87, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x1f, 0xb7, 0x80, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x1e, 0x63, 0x80, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x1c, 0x63, 0x80, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x03, 0xff, 0x6f, 0x80, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x03, 0xff, 0x6f, 0xc0, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0xfe, 0x4f, 0xc0, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0xfe, 0xdf, 0xc0, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0xfe, 0x1f, 0x80, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x62, 0x0e, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x03, 0x08, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x01, 0xf8, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x01, 0xf0, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0xf0, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0xf0, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x70, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xfd,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xfd,
0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xfd,
0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xfd,
0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xfd,
0xe0, 0x70, 0x0f, 0x83, 0xe0, 0xc0, 0x40, 0x3a, 0x30, 0x38, 0x1e, 0x05,
0xce, 0x31, 0xc7, 0x39, 0xce, 0x73, 0xc7, 0x38, 0xe7, 0x3b, 0xcc, 0x71,
0x9f, 0x33, 0xe6, 0x7c, 0x9f, 0x73, 0xcf, 0xb9, 0xcf, 0x9f, 0xcc, 0xf9,
0x9f, 0x93, 0xf2, 0x7c, 0x9f, 0x73, 0xcf, 0x9b, 0xcf, 0x9e, 0x09, 0xf9,
0x9f, 0x97, 0xf2, 0x00, 0x9f, 0x73, 0xcf, 0x9b, 0xc0, 0x18, 0x09, 0xfd,
0x9f, 0x93, 0xf2, 0x7f, 0x9f, 0x73, 0xcf, 0x9b, 0xcf, 0xf3, 0xc9, 0xfc,
0x9f, 0xb3, 0xe6, 0x7f, 0x9f, 0x73, 0xcf, 0x9b, 0xcf, 0xf3, 0xcc, 0xf9,
0xcf, 0x31, 0xe7, 0x39, 0x9f, 0x73, 0xcf, 0x9b, 0xe7, 0x93, 0xcc, 0xf1,
0xe0, 0x70, 0x0f, 0x81, 0x9f, 0x78, 0x4f, 0x9b, 0xf0, 0x38, 0x0e, 0x05,
0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

/** Door lock image palette information  */
static const uint32_t doorLockImgPalette[]=
{
    0x000000,   0xffffff
};

/** Door lock image information  */
const Graphics_Image doorLockImage =
{
    .bPP = IMAGE_FMT_1BPP_UNCOMP,
    .xSize = 96,
    .ySize = 96,
    .numColors = 2,
    .pPalette = doorLockImgPalette,
    .pPixel = doorLockImgData,
};

/** Door unlock image data */
static const uint8_t  doorUnlockImgData[] =
{
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xfc, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xf0, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xe0, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0x80, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xfe, 0x00, 0x7f, 0x80, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xfe, 0x01, 0xff, 0xe0, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xfc, 0x03, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xfc, 0x07, 0xff, 0xf8, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xf8, 0x07, 0xff, 0xf8, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xf8, 0x0f, 0xff, 0xfc, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xf8, 0x0f, 0xff, 0xfc, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xf8, 0x1f, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xf8, 0x1f, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xf8, 0x1f, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xf8, 0x1f, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xf8, 0x1f, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xf8, 0x1f, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xf8, 0x1f, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xf8, 0x1f, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xf8, 0x1f, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xf8, 0x1f, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xf8, 0x1f, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xf8, 0x1f, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xfc, 0x3f, 0xff, 0xff, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x1e, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x1e, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x1e, 0x20, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x1e, 0x30, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x1f, 0x20, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x1f, 0xaf, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x1f, 0x77, 0x80, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x1c, 0x63, 0x80, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x03, 0xfe, 0x67, 0x80, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x03, 0xff, 0x6f, 0xc0, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x01, 0xfe, 0x6f, 0xc0, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0xfe, 0xdf, 0xc0, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0xfe, 0xdf, 0x80, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0xfe, 0x1f, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x03, 0x0c, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x03, 0xf8, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x01, 0xf0, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x01, 0xf0, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0xf0, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x70, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

/** Door unlock image palette information */
static const uint32_t doorUnlockImgPalette[]=
{
    0x000000,   0xffffff
};

/** Door unlock image information */
const Graphics_Image doorUnlockImage =
{
    .bPP = IMAGE_FMT_1BPP_UNCOMP,
    .xSize = 96,
    .ySize = 96,
    .numColors = 2,
    .pPalette = doorUnlockImgPalette,
    .pPixel = doorUnlockImgData,
};