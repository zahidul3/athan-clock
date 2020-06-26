/******************************************************************************

 @file graphicext.c

 @brief Graphics extension APIs used for animations.

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
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
/* TIRTOS specific driver header files */
#include <xdc/runtime/Assert.h>
#include <ti/sysbios/knl/Clock.h>
#include "graphicext.h"

/******************************************************************************
 Constants and Definitions
 *****************************************************************************/

/******************************************************************************
 Local variables
 *****************************************************************************/
static Clock_Handle animationClkHandle;
static Clock_Struct animationClkStruct;
static Display_Handle *lcdHdl;
static Event_Struct *eventStruct;
static uint32_t graphicsEvent;
static bool contAnimation = false;
static bool overlayBack = false;
static GraphicExt_sprite_t *sprites = NULL;
static Graphics_Image *backgroundFrames = NULL;
static uint8_t listLength;
static uint8_t currentStartFrame;
static uint8_t currentEndFrame;
static uint8_t currentFrame;
static const uint32_t spriteImgPalette[]=
{
    0x000000,   0xffffff
};

/******************************************************************************
 Function Prototype
 *****************************************************************************/

/*  timeout call back for animation. */
static void GraphicExt_timerCallBack(UArg a0);
/*  flush sprites to LCD screen. */
static void GraphicExt_flushSprites(Graphics_Context *context);
/*  flush an animation frame to LCD screen. */
static void GraphicExt_flushFrame(Graphics_Context *context);
/*  determine if sprite collides with yPos. */
static bool GraphicExt_spInRange(uint8_t yPos, GraphicExt_sprite_t *sprite);



/******************************************************************************
 Local Functions
 *****************************************************************************/

 /**
 * @brief Flush the sprites after being drawn by GraphicExt_drawSprites.
 *
 * @param  context   LCD Graphics context.
 *
 * @return None
 */
static void GraphicExt_flushFrame(Graphics_Context *context)
{
    int8_t increment = (currentFrame > currentEndFrame) ? -1 : 1;

    if (context)
    {
        Graphics_drawImage(context, &backgroundFrames[currentFrame], 0, 0);
        if(overlayBack &&  sprites != NULL)
        {
            GraphicExt_flushSprites(context);
        }
        Graphics_flushBuffer(context);
        currentFrame+= increment;

        if(contAnimation && (currentFrame == currentEndFrame))
        {
            currentFrame = currentStartFrame;
        }

        if(Clock_isActive(animationClkHandle) == true)
        {
            Clock_stop(animationClkHandle);
        }
        if(currentFrame != currentEndFrame)
        {
            Clock_start(animationClkHandle);
        }
    }
}

 /**
 * @brief Determines if a sprite is within range.
 *
 * @param  yPos     Y possition.
 * @param  sprite   Sprite to be compared vs yPos.
 *
 * @return None
 */
static bool GraphicExt_spInRange(uint8_t yPos, GraphicExt_sprite_t *sprite)
{
    uint8_t spriteY = sprite->posY;
    uint8_t spriteH = sprite->image->ySize;
    return ((spriteY <= yPos) && ((spriteY + spriteH) > yPos));
}

 /**
 * @brief Flush the sprites after being drawn by GraphicExt_drawSprites.
 *
 * @param  context   LCD Graphics context.
 *
 * @return None
 */
static void GraphicExt_flushSprites(Graphics_Context *context)
{
    const uint8_t *pBuff;

    uint8_t *pBackground = NULL;
    uint8_t widthBytesBack = 0;
    uint8_t posxBytesback = 0;
    uint8_t posYSprt;
    uint8_t *currentLine;
    uint8_t widthBytes;
    uint8_t posxBytes;
    unsigned int lcdPxlX = (unsigned int)(context->clipRegion.xMax) + 1;
    unsigned int lcdPxlY = (unsigned int)(context->clipRegion.yMax) + 1;

    bool drawL = false;
    GraphicExt_sprite_t backSprite = {0,0,&backgroundFrames[currentFrame]};

    currentLine = (uint8_t*)malloc((lcdPxlX/8));

    if (currentLine != NULL)
    {
        Graphics_Image currentLineImg =
        {
            .bPP       = IMAGE_FMT_1BPP_UNCOMP,
            .xSize     = lcdPxlX,
            .ySize     = 1,
            .numColors = 2,
            .pPalette  = spriteImgPalette,
            .pPixel    = currentLine,
        };

        if(overlayBack)
        {
            pBackground = (uint8_t*)backgroundFrames[currentFrame].pPixel;
            widthBytesBack = backgroundFrames[currentFrame].xSize/8;
        }
        for(int lineNo = 0; lineNo < lcdPxlY; lineNo++)
        {
            memset(currentLine, 0xFF, (lcdPxlX/8));


            for(uint16_t currSprite = 0; currSprite < listLength; currSprite++)
            {
                posxBytes = (sprites[currSprite].posX)/8;
                widthBytes = sprites[currSprite].image->xSize/8;
                posYSprt = sprites[currSprite].posY;
                pBuff = sprites[currSprite].image->pPixel;
                if(pBuff != NULL &&
                   GraphicExt_spInRange(lineNo,&sprites[currSprite]))
                {
                    memcpy((currentLine + posxBytes),
                           (pBuff + ((lineNo - posYSprt)*widthBytes)), widthBytes);
                    drawL = true;
                }

            }
            if(pBackground != NULL &&
               GraphicExt_spInRange(lineNo,&backSprite))
            {
                drawL = true;
                for(uint8_t lineIdx = 0; lineIdx < widthBytesBack; lineIdx++)
                {
                    currentLine[lineIdx + posxBytesback] &= pBackground[lineIdx];
                }
                pBackground+=widthBytesBack;
            }
            /*if there was no sprite in this line then dont bother redrawing the line*/
            if(drawL)
            {
                Graphics_drawImage(context, &currentLineImg, 0, lineNo);
                drawL = false;
            }

        }
        free(currentLine);
    }

}

/**
 * @brief Callback function for the timer timeout.
 *
 * @param  a0      Argument passed to CB if set when timer is initialized.
 *
 * @return None
 */
static void GraphicExt_timerCallBack(UArg a0)
{
    Event_post(Event_handle(eventStruct), graphicsEvent);
}

 /******************************************************************************
 External Functions
 *****************************************************************************/
/**
* Documented in graphicext.h
*/

void GraphicExt_initAnimationModule(Display_Handle *lcdHandle,
                                    Event_Struct *appEventStruct,
                                    uint32_t regEventID,
                                    uint32_t frameDuration)
{
    lcdHdl = lcdHandle;
    eventStruct = appEventStruct;
    graphicsEvent = regEventID;

    Clock_Params clockParams;

    /* Convert clockDuration in milliseconds to ticks. */
    uint32_t clockTicks = frameDuration * (1000 / Clock_tickPeriod);

    /* Setup parameters. */
    Clock_Params_init(&clockParams);


    /* If period is 0, this is a one-shot timer. */
    clockParams.period = 0;

    /*
     Starts immediately after construction if true, otherwise wait for a
     call to start.
     */
    clockParams.startFlag = false;

    /*/ Initialize clock instance. */
    Clock_construct(&animationClkStruct, GraphicExt_timerCallBack, clockTicks, &clockParams);

    animationClkHandle = Clock_handle(&animationClkStruct);
}

/**
* Documented in graphicext.h
*/
void GraphicExt_animateBackground(Graphics_Image *frames, uint8_t startFrame,
                        uint8_t endFrame, bool continuous)
{
    contAnimation = continuous;
    currentStartFrame = startFrame;
    currentEndFrame = endFrame;
    currentFrame = startFrame;
    backgroundFrames = frames;

    if (lcdHdl)
    {
        /*
         * Use the GrLib extension to get the GraphicsLib context object of the
         * LCD, if it is supported by the display type.
         */
        Graphics_Context *context = DisplayExt_getGraphicsContext(*lcdHdl);
        GraphicExt_flushFrame(context);
    }
}

/**
* Documented in graphicext.h
*/
void GraphicExt_stopAnimate(void)
{
    if(Clock_isActive(animationClkHandle) == true)
    {
        Clock_stop(animationClkHandle);
    }
}

/**
* Documented in graphicext.h
*/
void GraphicExt_drawSprites(GraphicExt_sprite_t *spriteList,
                            uint8_t spriteListLen,
                            bool overlayBackground)
{
    sprites = spriteList;
    listLength = spriteListLen;
    overlayBack = overlayBackground;
    if (lcdHdl)
    {
        Graphics_Context *context = DisplayExt_getGraphicsContext(*lcdHdl);
        GraphicExt_flushSprites(context);
        Graphics_flushBuffer(context);
    }
}

/**
* Documented in graphicext.h
*/
void GraphicExt_processEvt(void)
{
    if (lcdHdl)
    {
        /*
         * Use the GrLib extension to get the GraphicsLib context object of the
         * LCD, if it is supported by the display type.
         */
        Graphics_Context *context = DisplayExt_getGraphicsContext(*lcdHdl);
        GraphicExt_flushFrame(context);
    }
}
