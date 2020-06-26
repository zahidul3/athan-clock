/******************************************************************************

 @file sha256_alt.h

 @brief SHA256 specific alternate implimentation items.

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

#ifndef MBEDTLS_SHA256_ALT_H
#define MBEDTLS_SHA256_ALT_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls-config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_SHA256_ALT)
#ifdef __cplusplus
extern "C" {
#endif
#include <ti/drivers/SHA2.h>
/**
 * \brief          SHA-256 context structure
 */
typedef struct
{
    uint32_t totalBytes;         /*!< number of bytes processed  */
    uint32_t digest[8];          /*!< intermediate digest state  */
    unsigned char buffer[64];    /*!< data block being processed */
    SHA2_HashSize hashSize;      /*!< 1 => SHA-256, 0 = SHA-224 */
    int hashBlocks;              /*!< number of 64-byte blocks processed */
    int remBlockSize;            /*!< remaining unprocessed data */
}
mbedtls_sha256_context;

/**
 * \brief          Initialize SHA-256 context
 *
 * \param ctx      SHA-256 context to be initialized
 */
void mbedtls_sha256_init( mbedtls_sha256_context *ctx );

/**
 * \brief          Clear SHA-256 context
 *
 * \param ctx      SHA-256 context to be cleared
 */
void mbedtls_sha256_free( mbedtls_sha256_context *ctx );

/**
 * \brief          Clone (the state of) a SHA-256 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void mbedtls_sha256_clone( mbedtls_sha256_context *dst,
                           const mbedtls_sha256_context *src );

/**
 * \brief          SHA-256 context setup
 *
 * \param ctx      context to be initialized
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
void mbedtls_sha256_starts( mbedtls_sha256_context *ctx, int is224 );

/**
 * \brief          SHA-256 process buffer
 *
 * \param ctx      SHA-256 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void mbedtls_sha256_update( mbedtls_sha256_context *ctx, const unsigned char *input,
                    size_t ilen );

/**
 * \brief          SHA-256 final digest
 *
 * \param ctx      SHA-256 context
 * \param output   SHA-224/256 checksum result
 */
void mbedtls_sha256_finish( mbedtls_sha256_context *ctx, unsigned char output[32] );

/* Internal use */
void mbedtls_sha256_process( mbedtls_sha256_context *ctx, const unsigned char data[64] );

#ifdef __cplusplus
}
#endif
#endif /* MBEDTLS_SHA256_ALT */

#endif /* MBEDTLS_SHA256_ALT_H */

