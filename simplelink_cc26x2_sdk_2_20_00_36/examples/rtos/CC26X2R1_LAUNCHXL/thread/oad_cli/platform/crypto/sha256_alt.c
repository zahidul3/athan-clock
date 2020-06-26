/******************************************************************************

 @file sha256_alt.c

 @brief Sha256 crypto functions.

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
#include <string.h>
#include <assert.h>
#include "sha256_alt.h"

#if defined(MBEDTLS_SHA256_ALT)

#include <ti/devices/DeviceFamily.h>
#include <ti/drivers/SHA2.h>

static SHA2_Handle sha2Handle = NULL;

/**
 * number of active contexts, used for power on/off of the crypto core
 */
static unsigned int ref_num = 0;

/**
 * \brief          Initialize SHA-256 context
 *
 * \param ctx      SHA-256 context to be initialized
 */
void mbedtls_sha256_init(mbedtls_sha256_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_sha256_context));
}

/**
 * \brief          Clear SHA-256 context
 *
 * \param ctx      SHA-256 context to be cleared
 */
void mbedtls_sha256_free(mbedtls_sha256_context *ctx)
{
    if (ctx == NULL )
        return;

    if (--ref_num == 0)
    {
        SHA2_close(sha2Handle);
        sha2Handle = NULL;
    }

    memset(ctx, 0, sizeof(mbedtls_sha256_context));
}

/**
 * \brief          SHA-256 context setup
 *
 * \param ctx      context to be initialized
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
void mbedtls_sha256_starts(mbedtls_sha256_context *ctx, int is224)
{
    SHA2_Params sha2Params;

    if (ref_num++ == 0)
    {
        SHA2_Params_init(&sha2Params);

        sha2Params.returnBehavior = SHA2_RETURN_BEHAVIOR_POLLING;
        /* Open SHA2 */
        sha2Handle = SHA2_open(0, &sha2Params);
        /* SHA2_open() failed */
        assert(sha2Handle != 0);
    }

    memset(ctx, 0, sizeof(mbedtls_sha256_context));

    if (is224)
    {
        ctx->hashSize = SHA2_HASH_SIZE_224;
    }
    else
    {
        ctx->hashSize = SHA2_HASH_SIZE_256;
    }
}

/**
 * \brief          Clone (the state of) a SHA-256 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void mbedtls_sha256_clone(mbedtls_sha256_context *dst,
                           const mbedtls_sha256_context *src)
{
    *dst = *src;
}

/**
 * \brief          SHA-256 start block
 *
 * \param ctx      SHA-256 context
 * \param data     64-byte input data block
 */
int_fast16_t sha256_startHash(mbedtls_sha256_context *ctx, const unsigned char data[64])
{
    SHA2_OperationStartHash operationStartHash;

    SHA2_OperationStartHash_init(&operationStartHash);

    /*!< The hash size to use. */
    operationStartHash.hashSize =  ctx->hashSize;

    /*!< The length of the message segment to hash, in bytes.  This
     *   length must be a multiple of the hash block size.
     *   The block size for 224 and 256 is 64 bytes and the block
     *   size for 384 and 512 is 128 bytes.
     */
    operationStartHash.length = SHA2_BLOCK_SIZE_BYTES_256;

    operationStartHash.message = data;                        /*!< Pointer to the message segment to hash. */
    operationStartHash.intermediateDigest = (uint8_t *)ctx->digest;
    return SHA2_startHash(sha2Handle, &operationStartHash);
}

/**
 * \brief          SHA-256 intermediate blocks
 *
 * \param ctx      SHA-256 context
 * \param data     64-byte input data block
 */
int_fast16_t sha256_procHash(mbedtls_sha256_context *ctx, const unsigned char data[64])
{
    SHA2_OperationProcessHash operationProcessHash;

    SHA2_OperationProcessHash_init(&operationProcessHash);

    operationProcessHash.hashSize =  ctx->hashSize;               /*!< The hash size to use. */
    operationProcessHash.length = SHA2_BLOCK_SIZE_BYTES_256;
    operationProcessHash.message = data;                          /*!< Pointer to the message segment to hash. */
    operationProcessHash.intermediateDigest = (uint8_t *)ctx->digest;
    return SHA2_processHash(sha2Handle, &operationProcessHash);
}

/**
 * \brief          SHA-256 final block
 *
 * \param ctx      SHA-256 context
 * \param output   SHA-224/256 checksum result
 */
int_fast16_t sha256_finishHash(mbedtls_sha256_context *ctx, unsigned char output[32])
{
    SHA2_OperationFinishHash operationFinishHash;

    SHA2_OperationFinishHash_init(&operationFinishHash);

    operationFinishHash.hashSize =  ctx->hashSize;               /*!< The hash size to use. */
    operationFinishHash.message = ctx->buffer;                   /*!< Pointer to the message segment to hash. */
    operationFinishHash.intermediateDigest = (uint8_t *)ctx->digest;
    operationFinishHash.finalDigest = output;
    operationFinishHash.totalLength = ctx->totalBytes;
    operationFinishHash.segmentLength = ctx->remBlockSize;
    return SHA2_finishHash(sha2Handle, &operationFinishHash);
}

/**
 * \brief          SHA-256 in one step
 *
 * \param ctx      SHA-256 context
 * \param output   SHA-224/256 checksum result
 */
int_fast16_t sha256_oneStepHash(mbedtls_sha256_context *ctx, unsigned char output[32])
{
    SHA2_OperationOneStepHash operationOneStepHash;

    /* Perform a single step hash operation of the message */
    SHA2_OperationOneStepHash_init(&operationOneStepHash);

    operationOneStepHash.hashSize       = ctx->hashSize;
    operationOneStepHash.message        = ctx->buffer;
    operationOneStepHash.digest         = output;
    operationOneStepHash.totalLength    = ctx->totalBytes;

    return SHA2_oneStepHash(sha2Handle, &operationOneStepHash);

}

/**
 * \brief          SHA-256 start/intermediate blocks
 *
 * \param ctx      SHA-256 context
 * \param data     64-byte input data block
 */
void mbedtls_sha256_process(mbedtls_sha256_context *ctx, const unsigned char data[64])
{
    int_fast16_t result;

    if (!ctx->hashBlocks)
    {
        result = sha256_startHash(ctx, data);
    }
    else
    {
        result = sha256_procHash(ctx, data);
    }
    /* handle error */
    assert(result == SHA2_STATUS_SUCCESS);

    ctx->hashBlocks++;

    (void) result;
}

/**
 * \brief          SHA-256 final digest
 *
 * \param ctx      SHA-256 context
 * \param output   SHA-224/256 checksum result
 */
void mbedtls_sha256_finish(mbedtls_sha256_context *ctx, unsigned char output[32])
{
    int_fast16_t result;

    if (ctx->hashBlocks > 0)
    {
        result = sha256_finishHash(ctx, output);
    }
    else
    {
        result = sha256_oneStepHash(ctx, output);
    }

    /* handle error */
    assert(result == SHA2_STATUS_SUCCESS);

    (void) result;
}


/**
 * \brief          SHA-256 process buffer
 *
 * \param ctx      SHA-256 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void mbedtls_sha256_update(mbedtls_sha256_context *ctx, const unsigned char *input,
                    size_t ilen)
{
    uint32_t curBlockSize, pad;

    if (ilen == 0 )
        return;

    curBlockSize = ctx->remBlockSize + ilen;
    ctx->totalBytes += (uint32_t) ilen;

    if ((curBlockSize >= SHA2_BLOCK_SIZE_BYTES_256 ) && (ctx->remBlockSize))
    {
        pad = SHA2_BLOCK_SIZE_BYTES_256 - ctx->remBlockSize;
        memcpy( (void *) (ctx->buffer + ctx->remBlockSize), input, pad );
        mbedtls_sha256_process( ctx, ctx->buffer );
        input += pad;
        ilen  -= pad;
        ctx->remBlockSize = 0;
    }

    while ((ilen > SHA2_BLOCK_SIZE_BYTES_256))
    {
        mbedtls_sha256_process( ctx, input );
        input += SHA2_BLOCK_SIZE_BYTES_256;
        ilen  -= SHA2_BLOCK_SIZE_BYTES_256;
    }

    if (ilen > 0)
    {
        memcpy((void *) (ctx->buffer + ctx->remBlockSize), input, ilen);
        ctx->remBlockSize += ilen;
    }
}

#endif /* MBEDTLS_SHA256_ALT */

