/******************************************************************************

 @file random.c

 @brief TIRTOS platform specific random functions for OpenThread

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

#include <openthread/config.h>

#include <openthread/types.h>
#include <utils/code_utils.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/prcm.h)
#include DeviceFamily_constructPath(driverlib/trng.h)

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/utils/RingBuf.h>

#include <openthread/platform/random.h>

#include <mbedtls/entropy_poll.h>

#include "platform.h"

enum
{
    PLATFORM_TRNG_MIN_SAMPLES_PER_CYCLE = (1 << 8),
    PLATFORM_TRNG_MAX_SAMPLES_PER_CYCLE = (1 << 8),
    PLATFORM_TRNG_CLOCKS_PER_SAMPLE     = 1,
};

/**
 *  Size of the buffer allocation for the ring buffer.
 *
 *  Please note Larger buffer pool size will impact the start up
 *  time.
 */
#define RING_BUFFER_SIZE 32

/* buffer used by the ringbuffer object */
static unsigned char ringBuf[RING_BUFFER_SIZE];

/* ring buffer object */
static RingBuf_Object ringBufObject;

/**
 * @internal
 * @brief Initialize the TRNG module and wait for it to be ready
 *        @note:Power to the module needs to be turned on
 *              before calling this function.
 *
 * @return None
 */
static void initTRNG(void)
{
    TRNGConfigure(PLATFORM_TRNG_MIN_SAMPLES_PER_CYCLE,
                  PLATFORM_TRNG_MAX_SAMPLES_PER_CYCLE,
                  PLATFORM_TRNG_CLOCKS_PER_SAMPLE);
    TRNGEnable();

    /* wait for the TRNG to be ready */
    while (!(TRNGStatusGet() & TRNG_NUMBER_READY))
    {
        ;
    }
}

/**
 * @internal
 * @brief Fills the ring buffer pool with random data obtained
 *        from the TRNG module
 *
 * @return returns the size of the random bytes written in the
 *         ringbuffer pool.
 */
static uint32_t fillRandomPool(void)
{
    size_t fillSize = RING_BUFFER_SIZE -
                      RingBuf_getCount((RingBuf_Handle)&ringBufObject);
    size_t length   = 0;

    union
    {
        uint32_t u32[2];
        uint8_t u8[8];
    } buffer;

    Power_setDependency(PowerCC26XX_PERIPH_TRNG);

    /* initialize and enable TRNG if TRNG is not enabled */
    if (0 == (HWREG(TRNG_BASE + TRNG_O_CTL) & TRNG_CTL_TRNG_EN))
    {
        initTRNG();
    }

    while (length < fillSize)
    {
        if (length % 8 == 0)
        {
            /* we've run to the end of the buffer */
            while (!(TRNGStatusGet() & TRNG_NUMBER_READY))
            {
                ;
            }

            /*
             * don't use TRNGNumberGet here because it will tell the TRNG to
             * refill the entropy pool, instead we do it ourself.
             */
            buffer.u32[0] = HWREG(TRNG_BASE + TRNG_O_OUT0);
            buffer.u32[1] = HWREG(TRNG_BASE + TRNG_O_OUT1);
            HWREG(TRNG_BASE + TRNG_O_IRQFLAGCLR) = 0x1;
        }

        RingBuf_put((RingBuf_Handle)&ringBufObject, buffer.u8[length % 8]);
        length++;
    }

    Power_releaseDependency(PowerCC26XX_PERIPH_TRNG);

    return length;
}

/**
 * Fill an arbitrary area with random data
 *
 * @param [out] aOutput area to place the random data
 * @param [in] aLen size of the area to place random data
 * @param [out] oLen how much of the output was written to
 *
 * @return indication of error
 * @retval 0 no error occured
 */
static int TRNGPoll(unsigned char *aOutput, size_t aLen, size_t *oLen)
{
    size_t length = 0;
    union
    {
        uint32_t u32[2];
        uint8_t u8[8];
    } buffer;

    while (length < aLen)
    {
        if (length % 8 == 0)
        {
            /* we've run to the end of the buffer */
            while (!(TRNGStatusGet() & TRNG_NUMBER_READY))
            {
                ;
            }

            /*
             * don't use TRNGNumberGet here because it will tell the TRNG to
             * refill the entropy pool, instead we do it ourself.
             */
            buffer.u32[0] = HWREG(TRNG_BASE + TRNG_O_OUT0);
            buffer.u32[1] = HWREG(TRNG_BASE + TRNG_O_OUT1);
            HWREG(TRNG_BASE + TRNG_O_IRQFLAGCLR) = 0x1;
        }

        aOutput[length] = buffer.u8[length % 8];

        length++;
        *oLen = length;
    }
    return 0;
}

/**
 * @internal
 * @brief Fill an arbitrary area with the random data.
 *        It first gets random from the pool if available else
 *        call the TRNGPoll() to get random data from TRNG
 *        module.
 *
 * @param aOutput area to place the random data
 * @param aLen size if the area to place random data
 * @param oLen how much of the output was written to
 *
 * @return returns 0 if no error occurred, -1 if error.
 */
static int getRandom(unsigned char *aOutput, size_t aLen, size_t *oLen)
{
    size_t count = RingBuf_getCount((RingBuf_Handle)&ringBufObject);

    if (count >= aLen)
    {
        for (int i = 0; i < aLen; i++)
        {
            (void)RingBuf_get(&ringBufObject, &aOutput[i]);
        }

        *oLen = aLen;
    }
    else
    {
        size_t length = 0;
        size_t tempSize = 0;

        /* jump start the TRNG peripheral since we will be using below */
        Power_setDependency(PowerCC26XX_PERIPH_TRNG);

        /* initialize and enable TRNG if TRNG is not enabled */
        if (0 == (HWREG(TRNG_BASE + TRNG_O_CTL) & TRNG_CTL_TRNG_EN))
        {
            initTRNG();
        }

        while (length < count)
        {
            (void)RingBuf_get(&ringBufObject, &aOutput[length]);
            length++;
        }

        /* and get the rest directly from TRNG */
        TRNGPoll(&aOutput[length], aLen - count, &tempSize);

        /* release the TRNG power dependency */
        Power_releaseDependency(PowerCC26XX_PERIPH_TRNG);

        length += tempSize;
        *oLen = length;

        /* notify the main loop that we need to replenish the random pool */
        platformRandomSignal();
    }

    return 0;
}


/**
 * Function documented in platform.h
 */
void platformRandomInit(void)
{
    /* construct a ring buffer to be used for pool of random bytes */
    RingBuf_construct(&ringBufObject, ringBuf, RING_BUFFER_SIZE);

    /* fill the random pool */
    (void)fillRandomPool();
}

/**
 * Function documented in platform/random.h
 */
uint32_t otPlatRandomGet(void)
{
    union
    {
        uint32_t u32;
        uint8_t u8[4];
    } buffer;

    size_t  temp_size = 0;

    getRandom((unsigned char *)buffer.u8, 4, &temp_size);

    return buffer.u32;
}


/**
 * Function documented in platform/random.h
 */
otError otPlatRandomSecureGet(uint16_t aInputLength, uint8_t *aOutput,
                              uint16_t *aOutputLength)
{
    otError error     = OT_ERROR_NONE;
    size_t  length    = aInputLength;
    size_t  temp_size = 0;

    otEXPECT_ACTION(aOutput && aOutputLength, error = OT_ERROR_INVALID_ARGS);

    otEXPECT_ACTION(getRandom((unsigned char *)aOutput, length, &temp_size) == 0,
                     error = OT_ERROR_FAILED);

exit:
    if (aOutputLength)
    {
        *aOutputLength = temp_size;
    }
    return error;
}

/**
 * Function documented in platform.h
 */
void platformRandomProcess(void)
{
    /* fill the pool with random bytes from TRNG */
    (void)fillRandomPool();
}

/**
 * Function documented in platform/random.h
 */
otError otPlatRandomGetTrue(uint8_t *aOutput, uint16_t aOutputLength)
{
    otError error = OT_ERROR_NONE;
    size_t  temp_size = 0;

    otEXPECT_ACTION(NULL != aOutput, error = OT_ERROR_INVALID_ARGS);

    otEXPECT_ACTION(getRandom((unsigned char *)aOutput, aOutputLength, &temp_size) == 0, error = OT_ERROR_FAILED);
    (void)temp_size;

exit:
    return error;
}

