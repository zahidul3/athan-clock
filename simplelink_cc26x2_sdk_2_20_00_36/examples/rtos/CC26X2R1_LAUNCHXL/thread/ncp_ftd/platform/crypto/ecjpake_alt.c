/******************************************************************************

 @file ecjpake_alt.c

 @brief ECJPAKE implementation for TI chip

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

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ECJPAKE_ALT)
#include "mbedtls/ecjpake.h"
#include "ecjpake_alt.h"
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
#include <string.h>

/*
 * Convert a mbedtls_ecjpake_role to identifier string
 */
static const char * const ecjpake_id[] = {
    "client",
    "server"
};

#define ID_MINE     ( ecjpake_id[ ctx->role ] )
#define ID_PEER     ( ecjpake_id[ 1 - ctx->role ] )

/*
 * Size of the temporary buffer for ecjpake_hash:
 * 3 EC points plus their length, plus ID and its length (4 + 6 bytes)
 */
#define ECJPAKE_HASH_BUF_LEN    ( 3 * ( 4 + MBEDTLS_ECP_MAX_PT_LEN ) + 4 + 6 )

#define ECJPAKE_ALT_CHK(f) do { if( ( ret = f ) != 0 ) goto cleanup; } while( 0 )

/**
 * TLS curve SECP256R1 ID value (refer RFC4492)
 */
#define TLS_CURVE_SECP256R1_ID          (23)


/**
 * @brief checks if the elliptical point is zero or not
 *
 * @param pt pointer to the ec point.
 * @param len length of the ec point in bytes.
 *
 * @return return true if the point is zero otherwise false.
 */
static bool ec_point_is_zero(unsigned char *pt, int len)
{
    unsigned long* point = (unsigned long*)pt;

    /* check if point is equal to zero */
    for (int i = 0; i < (len/sizeof(unsigned long)); i++)
    {
      if ( point[i] != 0 )
      {
          break;
      }
      return (true);
    }

    return (false);
}

/**
 * @brief generates a private key using the random number generator function.
 *
 * @param private_key   pointer to the private key
 * @param f_rng         random number function
 * @param p_rng         input parameter to the random number function
 *
 * @return zero if successful otherwise non-zero error values.
 */
static int gen_private_key(unsigned char* private_key,
                           int (*f_rng)(void *, unsigned char *, size_t),
                           void *p_rng)
{
    bool zero = false;
    int i,j;
    size_t n_size = ECC_SECP256R1_LENGTH;
    unsigned long* pkey = (unsigned long*)private_key;

    j = 0;
    do
    {
        f_rng(p_rng, private_key, n_size);

        /* check if private_key is equal to zero */
        for (i = 0; i < 8; i++)
        {
          if ( pkey[0] != 0 )
          {
              break;
          }
          zero = true;
        }
        j++;
    } while ( (zero) && (j < 3) );

   return ((zero)==false ? 0 : -1);
}

/**
 * @brief reverses the given big number
 *
 * @param s     pointer to the big number to be reversed
 * @param len   length of the big number
 * @return None
 */
static void big_num_reverse(unsigned char *s, int len)
{
  int     ix, iy;
  unsigned char t;

  ix = 0;
  iy = len - 1;
  while (ix < iy)
  {
    t     = s[ix];
    s[ix] = s[iy];
    s[iy] = t;
    ++ix;
    --iy;
  }
}

/**
 * @brief writes the big number into the buffer.
 *
 * @param buf   pointer to the buffer where the big number point
 *              needs to be written.
 * @param ilen  available length of the buffer.
 * @param pt    pointer to the big number.
 *
 * @return int   zero if succcessful otherwise negative value.
 */
static int write_binary(unsigned char *buf, size_t ilen, unsigned char* pt)
{
    if( ilen < ECC_SECP256R1_LENGTH )
    {
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    memcpy((void *)buf, pt, ECC_SECP256R1_LENGTH);
    big_num_reverse(buf, ECC_SECP256R1_LENGTH);

    return (0);
}

/**
 * @brief reads the binary date from the buffer and into the
 *        provided point.
 *
 * @param buf pointer to the buffer.
 * @param ilen length of the buffer
 * @param pt pointer to the point.
 *
 * @return int zero if successful otherwise negative value.
 */
static int read_binary(const unsigned char *buf, size_t ilen,
                       unsigned char* pt)
{
    if( ilen < 1 )
    {
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    memcpy( pt, buf, ilen );
    big_num_reverse( pt, ilen );

    return (0);
}

/**
 * @brief reads the key value from the buffer.
 *
 * @param buf   pointer to the buffer.
 * @param ilen  input length of the buffer.
 * @param key   pointer to the key.
 *
 * @return zero if successful otherwise negative values.
 */
static int read_key(const unsigned char *buf, size_t ilen, unsigned char* key)
{
    size_t plen = ECC_SECP256R1_LENGTH;

    if( ilen < 1 )
    {
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    if( buf[0] != 0x04 )
    {
        return( MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE );
    }

    memcpy(key, buf + 1, plen*2);
    big_num_reverse(key, plen);
    big_num_reverse(key + plen, plen);

    return (0);
}

/**
 * @brief read the point from the tls structure.
 *
 * @param buf       pointer to the buffer containing the tls
 *                  structure.
 * @param point     pointer to the ec point.
 * @param buf_len   length of the buffer.
 *
 * @return zero if successful otherwise negative values.
 */
static int read_tls_point(const unsigned char **buf,
                          unsigned char * point, size_t buf_len)
{
    unsigned char data_len;
    const unsigned char *buf_start;

    /*
     * at least two bytes (1 for length, at least one for data)
     */
    if( buf_len < 2 )
    {
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    data_len = *(*buf)++;

    if( data_len < 1 || data_len > buf_len - 1 )
    {
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    buf_start = *buf;
    *buf += data_len;

    return read_key(buf_start, data_len, point);
}

/**
 * @brief writes the value of the ec point in the required tls
 *        point structure.
 *
 * @param point pointer to the value of the point to be written.
 * @param olen  total length consumed by the point.
 * @param buf   pointer to the buffer.
 * @param blen  length of the buffer.
 *
 * @return zero if successful otherwise negative value.
 */
static int tls_write_point(const unsigned char *point,
                           size_t *olen,
                           unsigned char *buf, size_t blen)
{
    size_t plen = ECC_SECP256R1_LENGTH;

    if( blen < 66 )
    {
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    buf[1] = 0x04; /* uncompressed point format  */
    memcpy(buf + 2, point, plen*2);
    big_num_reverse(buf + 2, plen);
    big_num_reverse(buf + plen + 2, plen);
    *olen = 65;

    /*
     * write length to the first byte and update total length
     */
    buf[0] = (unsigned char) *olen;
    ++*olen;

    return( 0 );
}

/**
 * @brief write the ec point (public) to the buffer.
 *
 * @param ecpoint pointer to the ec point (public).
 * @param format  format of the ec point.
 * @param olen    output length consumed.
 * @param buf     pointer to the buffer.
 * @param buflen  length of the buffer.
 *
 * @return zero if successful otherwise negative value.
 */
static int ec_point_write_binary(const unsigned char* ecpoint,
                                 int format,
                                 size_t *olen,
                                 unsigned char *buf,
                                 size_t buflen)
{
    int ret = 0;
    size_t plen = ECC_SECP256R1_LENGTH;

    if( format != MBEDTLS_ECP_PF_UNCOMPRESSED )
    {
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    *olen = 2 * plen + 1;

    if( buflen < *olen )
    {
        return( MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL );
    }

    buf[0] = 0x04;
    memcpy(buf + 1, ecpoint, plen*2);
    big_num_reverse(buf + 1, plen);
    big_num_reverse(buf + 1 + plen, plen);

    return( ret );
}

/*
 * Initialize context
 */
void mbedtls_ecjpake_init(mbedtls_ecjpake_context *ctx)
{
    if(ctx == NULL)
    {
        return;
    }

    ctx->md_info = NULL;
    mbedtls_ecp_group_init( &ctx->grp );
    ctx->point_format = MBEDTLS_ECP_PF_UNCOMPRESSED;

    /* ECJPAKE driver setup */
    ECJPAKE_Params params;
    ECJPAKE_Params_init(&params);

    ctx->handle = ECJPAKE_open(0, &params);
}

/*
 * Free context
 */
void mbedtls_ecjpake_free(mbedtls_ecjpake_context *ctx)
{
    if(ctx == NULL)
    {
        return;
    }

    ctx->md_info = NULL;

    ECJPAKE_close(ctx->handle);
    ctx->handle = NULL;
}

/*
 * Setup context
 */
int mbedtls_ecjpake_setup(mbedtls_ecjpake_context *ctx,
                          mbedtls_ecjpake_role role,
                          mbedtls_md_type_t hash,
                          mbedtls_ecp_group_id curve,
                          const unsigned char *secret,
                          size_t len)
{
    int ret = 0;

    ctx->role = role;

    if( ( ctx->md_info = mbedtls_md_info_from_type(hash) ) == NULL )
    {
        return( MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE );
    }

    ECJPAKE_ALT_CHK( mbedtls_ecp_group_load(&ctx->grp, curve) );

    if (ctx->handle)
    {
        ctx->roundTwoGenerated = false;

        /* NISTP256 generator */
        CryptoKeyPlaintext_initKey(&ctx->nistP256GeneratorCryptoKey,
                                   (uint8_t*)ECCParams_NISTP256.generatorX,
                                   (ECCParams_NISTP256.length * 2));

        /* Pre-shared secret */
        read_binary(secret, len, &ctx->preSharedSecretKeyingMaterial[0]);

        CryptoKeyPlaintext_initKey(&ctx->preSharedSecretCryptoKey,
                                   ctx->preSharedSecretKeyingMaterial,
                                   len);

        CryptoKeyPlaintext_initKey(&ctx->myPrivateCryptoKey1,
                                   ctx->myPrivateKeyMaterial1,
                                   sizeof(ctx->myPrivateKeyMaterial1));
        CryptoKeyPlaintext_initKey(&ctx->myPrivateCryptoKey2,
                                   ctx->myPrivateKeyMaterial2,
                                   sizeof(ctx->myPrivateKeyMaterial2));
        CryptoKeyPlaintext_initKey(&ctx->myPrivateCryptoV1,
                                   ctx->myPrivateVMaterial1,
                                   sizeof(ctx->myPrivateVMaterial1));
        CryptoKeyPlaintext_initKey(&ctx->myPrivateCryptoV2,
                                   ctx->myPrivateVMaterial2,
                                   sizeof(ctx->myPrivateVMaterial2));

        CryptoKeyPlaintext_initBlankKey(&ctx->myPublicCryptoKey1,
                                        ctx->myPublicKeyMaterial1,
                                        sizeof(ctx->myPublicKeyMaterial1));
        CryptoKeyPlaintext_initBlankKey(&ctx->myPublicCryptoKey2,
                                        ctx->myPublicKeyMaterial2,
                                        sizeof(ctx->myPublicKeyMaterial2));
        CryptoKeyPlaintext_initBlankKey(&ctx->myPublicCryptoV1,
                                        ctx->myPublicVMaterial1,
                                        sizeof(ctx->myPublicVMaterial1));
        CryptoKeyPlaintext_initBlankKey(&ctx->myPublicCryptoV2,
                                        ctx->myPublicVMaterial2,
                                        sizeof(ctx->myPublicVMaterial2));
        CryptoKeyPlaintext_initBlankKey(&ctx->myPublicCryptoV3,
                                        ctx->myPublicVMaterial3,
                                        sizeof(ctx->myPublicVMaterial3));
        CryptoKeyPlaintext_initBlankKey(&ctx->myCombinedPrivateKey,
                                        ctx->myCombinedPrivateKeyMaterial1,
                                        sizeof(ctx->myCombinedPrivateKeyMaterial1));
        CryptoKeyPlaintext_initBlankKey(&ctx->myCombinedPublicKey,
                                        ctx->myCombinedPublicKeyMaterial1,
                                        sizeof(ctx->myCombinedPublicKeyMaterial1));
        CryptoKeyPlaintext_initBlankKey(&ctx->myGeneratorKey,
                                        ctx->myGenerator,
                                        sizeof(ctx->myGenerator));

        CryptoKeyPlaintext_initBlankKey(&ctx->theirPublicCryptoKey1,
                                        ctx->theirPublicKeyMaterial1,
                                        sizeof(ctx->theirPublicKeyMaterial1));
        CryptoKeyPlaintext_initBlankKey(&ctx->theirPublicCryptoKey2,
                                        ctx->theirPublicKeyMaterial2,
                                        sizeof(ctx->theirPublicKeyMaterial2));
        CryptoKeyPlaintext_initBlankKey(&ctx->theirCombinedPublicKey,
                                        ctx->theirCombinedPublicKeyMaterial1,
                                        sizeof(ctx->theirCombinedPublicKeyMaterial1));
        CryptoKeyPlaintext_initBlankKey(&ctx->theirGeneratorKey,
                                        ctx->theirGenerator,
                                        sizeof(ctx->theirGenerator));

        CryptoKeyPlaintext_initKey(&ctx->myPrivateCryptoV3,
                                   ctx->myPrivateVMaterial3,
                                   sizeof(ctx->myPrivateVMaterial3));

    }

cleanup:
    return( ret );

}


/*
 * Check if context is ready for use
 */
int mbedtls_ecjpake_check(const mbedtls_ecjpake_context *ctx)
{
    if( ctx->md_info == NULL ||
        ctx->grp.id == MBEDTLS_ECP_DP_NONE ||
        ctx->handle ==  NULL )
    {
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    return( 0 );
}

/**
 * @brief write a point plus its length to a buffer
 *
 * @param p     pointer to the buffer.
 * @param end   end of the buffer.
 * @param pf    point format.
 * @param point pointer to the ec point to be written.
 *
 * @return zero if successfule otherwise negative values.
 */
static int ecjpake_write_len_point(unsigned char **p,
                                   const unsigned char *end,
                                   const int pf,
                                   const unsigned char *point)
{
    int ret;
    size_t len;

    /* Need at least 4 for length plus 1 for point */
    if( end < *p || end - *p < 5 )
    {
        return( MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL );
    }

    ret = ec_point_write_binary( point, pf, &len, *p + 4, end - ( *p + 4 ) );
    if( ret != 0 )
    {
        return( ret );
    }

    (*p)[0] = (unsigned char)( ( len >> 24 ) & 0xFF );
    (*p)[1] = (unsigned char)( ( len >> 16 ) & 0xFF );
    (*p)[2] = (unsigned char)( ( len >>  8 ) & 0xFF );
    (*p)[3] = (unsigned char)( ( len       ) & 0xFF );

    *p += 4 + len;

    return (0);
}

/**
 * @brief Computes hash for the ZKP.
 *
 * @param md_info   hash info.
 * @param pf        point format
 * @param G         generator point.
 * @param V         emphereal point for ZKP.
 * @param X         emphereal point for which hash needs to be generated.
 * @param id        identity of the device.
 * @param hash      pointer to the hash buffer.
 *
 * @return zero if successful otherwise negative value.
 */
static int ecjpake_hash(const mbedtls_md_info_t *md_info,
                        const int pf,
                        const unsigned char *G,
                        const unsigned char *V,
                        const unsigned char *X,
                        const char *id,
                        unsigned char *hash)
{
    int ret;
    unsigned char buf[ECJPAKE_HASH_BUF_LEN];
    unsigned char *p = buf;
    const unsigned char *end = buf + sizeof( buf );
    const size_t id_len = strlen( id );

    /* Write things to temporary buffer */
    ECJPAKE_ALT_CHK(ecjpake_write_len_point(&p, end, pf, G));
    ECJPAKE_ALT_CHK(ecjpake_write_len_point(&p, end, pf, V));
    ECJPAKE_ALT_CHK(ecjpake_write_len_point(&p, end, pf, X));

    if( end - p < 4 )
    {
        return( MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL );
    }

    *p++ = (unsigned char)( ( id_len >> 24 ) & 0xFF );
    *p++ = (unsigned char)( ( id_len >> 16 ) & 0xFF );
    *p++ = (unsigned char)( ( id_len >>  8 ) & 0xFF );
    *p++ = (unsigned char)( ( id_len       ) & 0xFF );

    if( end < p || (size_t)( end - p ) < id_len )
    {
        return( MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL );
    }

    memcpy( p, id, id_len );
    p += id_len;

    /* Compute hash */
    ECJPAKE_ALT_CHK(mbedtls_md(md_info, buf, p - buf, hash));

cleanup:
    return( ret );
}

/**
 * @brief Parses ECShnorrZKP and verifies it.
 *
 * @param ctx               pointer to context.
 * @param generator_key     pointer to the generator crypto key.
 * @param generator_point   pointer to the generator point.
 * @param X                 pointer to the emphereal public key.
 * @param public_key        public key.
 * @param id                device identity
 * @param p                 pointer to the buffer.
 * @param end               end of the buffer.
 *
 * @return zero if successful otherwise negative value.
 */
static int ecjpake_zkp_read(mbedtls_ecjpake_context *ctx,
                            CryptoKey* generator_key,
                            unsigned char* generator_point,
                            unsigned char* X,
                            CryptoKey* public_key,
                            const char *id,
                            const unsigned char **p,
                            const unsigned char *end)
{
    int ret;
    size_t r_len;
    ECJPAKE_OperationVerifyZKP operation_verify_zkp;
    CryptoKey their_public_v;
    unsigned char their_public_v_material[64];
    unsigned char r[ECC_SECP256R1_LENGTH];
    unsigned char hash[ECC_SECP256R1_LENGTH];

    /*
     * struct {
     *     ECPoint V;
     *     opaque r<1..2^8-1>;
     * } ECSchnorrZKP;
     */
    if( end < *p )
    {
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    ECJPAKE_ALT_CHK(read_tls_point(p, their_public_v_material, end - *p));

    if( end < *p || (size_t)( end - *p ) < 1 )
    {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto cleanup;
    }

    r_len = *(*p)++;

    CryptoKeyPlaintext_initKey(&their_public_v,
                               their_public_v_material,
                               sizeof(their_public_v_material));

    if( end < *p || (size_t)( end - *p ) < r_len )
    {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto cleanup;
    }

    ECJPAKE_ALT_CHK(read_binary( *p, r_len, r ));

    *p += r_len;

    /*
     * Verification
     */
    ECJPAKE_ALT_CHK(ecjpake_hash(ctx->md_info,
                                  ctx->point_format,
                                  generator_point,
                                  their_public_v_material,
                                  X, id, hash));
    big_num_reverse(hash, ECC_SECP256R1_LENGTH);

    ECJPAKE_OperationVerifyZKP_init(&operation_verify_zkp);
    operation_verify_zkp.curve                = &ECCParams_NISTP256;
    operation_verify_zkp.theirGenerator       = generator_key;
    operation_verify_zkp.theirPublicKey       = public_key;
    operation_verify_zkp.theirPublicV         = &their_public_v;
    operation_verify_zkp.hash                 = hash;
    operation_verify_zkp.r                    = r;

    ECJPAKE_ALT_CHK(ECJPAKE_verifyZKP(ctx->handle, &operation_verify_zkp));

cleanup:

    return( ret );
}

/**
 * @brief Parses a public key and its ZKP and verifies the public key.
 *
 * @param ctx               pointer to the context.
 * @param G                 generator key
 * @param generator_point   generator key value.
 * @param X                 input public key which needs to be verified
 * @param public_key        public key
 * @param id                device identifier.
 * @param p                 pointer to the buffer.
 * @param end               end of the pointer.
 *
 * @return zero if successful otherwise negative.
 */
static int ecjpake_k_zkp_read( mbedtls_ecjpake_context *ctx,
                             CryptoKey* G,
                             unsigned char* generator_point,
                             unsigned char* X,
                             CryptoKey* public_key,
                             const char *id,
                             const unsigned char **p,
                             const unsigned char *end )
{
    int ret;

    if( end < *p )
    {
        return( MBEDTLS_ERR_ECP_BAD_INPUT_DATA );
    }

    /*
     * struct {
     *     ECPoint X;
     *     ECSchnorrZKP zkp;
     * } ECJPAKEKeyKP;
     */
    ECJPAKE_ALT_CHK( read_tls_point(p, X, end - *p) );
    if( ec_point_is_zero(X,ECC_SECP256R1_LENGTH) &&
        ec_point_is_zero(X + ECC_SECP256R1_LENGTH, ECC_SECP256R1_LENGTH) )
    {
        ret = MBEDTLS_ERR_ECP_INVALID_KEY;
        goto cleanup;
    }

    ECJPAKE_ALT_CHK( ecjpake_zkp_read( ctx, G, generator_point, X,
                                       public_key, id, p, end ) );

cleanup:
    return( ret );
}

/**
 * @brief Reads the two sets of emphereal public key and its ZKP and verifies
 *        the proof
 *
 * @param ctx               pointer to the context.
 * @param G                 generator cryptokey
 * @param generator_point   generator point value.
 * @param Xa                pointer to the emphereal public key 1 from peer.
 * @param publicCryptoKey1  correponding public key cryptokey of Xa.
 * @param Xb                pointer to the emphereal public key 1 from peer.
 * @param publicCryptoKey2  correponding public key cryptokey of Xb.
 * @param id                device identifier.
 * @param buf               buffer to read the keys and zkp from.
 * @param len               length of the buffer.
 *
 * @return zero if successful otherwise failure.
 */
static int ecjpake_kzkp_kzkp_read(mbedtls_ecjpake_context *ctx,
                                  CryptoKey* G,
                                  unsigned char* generator_point,
                                  unsigned char* Xa,
                                  CryptoKey* publicCryptoKey1,
                                  unsigned char* Xb,
                                  CryptoKey* publicCryptoKey2,
                                  const char *id,
                                  const unsigned char *buf,
                                  size_t len)
{
    int ret;
    const unsigned char *p = buf;
    const unsigned char *end = buf + len;

    ECJPAKE_ALT_CHK(ecjpake_k_zkp_read(ctx, G, generator_point, Xa,
                                       publicCryptoKey1, id, &p, end));
    ECJPAKE_ALT_CHK(ecjpake_k_zkp_read(ctx, G, generator_point, Xb,
                                       publicCryptoKey2, id, &p, end));

cleanup:
    return( ret );

}

/**
 * @brief   writes two sets of emphereal public key and its ZKP.
 *
 * @param ctx   pointer to the context.
 * @param G     generator point.
 * @param id    identifier.
 * @param buf   buffer to write to.
 * @param len   length of the buffer.
 * @param olen  total length written in the buffer.
 * @param f_rng pointer to the random generator function.
 * @param p_rng parameter to the randome generator function.
 *
 * @return zero if successful otherwise failure.
 */
static int ecjpake_kzkp_kzkp_write(mbedtls_ecjpake_context *ctx,
                                   const unsigned char *G,
                                   const char *id,
                                   unsigned char *buf,
                                   size_t len,
                                   size_t *olen,
                                   int (*f_rng)(void *, unsigned char *, size_t),
                                   void *p_rng )
{
    int ret;
    unsigned char *p = buf;
    const unsigned char *end = buf + len;
    unsigned char hash[ECC_SECP256R1_LENGTH];
    unsigned char r[ECC_SECP256R1_LENGTH];

    /* Generate round one keys */
    ECJPAKE_OperationRoundOneGenerateKeys   roundOneGenerateKeys;
    ECJPAKE_OperationGenerateZKP            operationGenerateZKP;


    if( end < p )
    {
        return( MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL );
    }

    /* Generate private keys */
    ECJPAKE_ALT_CHK(gen_private_key(ctx->myPrivateKeyMaterial1, f_rng, p_rng));
    ECJPAKE_ALT_CHK(gen_private_key(ctx->myPrivateKeyMaterial2, f_rng, p_rng));
    ECJPAKE_ALT_CHK(gen_private_key(ctx->myPrivateVMaterial1, f_rng, p_rng));
    ECJPAKE_ALT_CHK(gen_private_key(ctx->myPrivateVMaterial2, f_rng, p_rng));
    ECJPAKE_ALT_CHK(gen_private_key(ctx->myPrivateVMaterial3, f_rng, p_rng));


    ECJPAKE_OperationRoundOneGenerateKeys_init(&roundOneGenerateKeys);
    roundOneGenerateKeys.curve             = &ECCParams_NISTP256;
    roundOneGenerateKeys.myPrivateKey1     = &ctx->myPrivateCryptoKey1;
    roundOneGenerateKeys.myPrivateKey2     = &ctx->myPrivateCryptoKey2;
    roundOneGenerateKeys.myPublicKey1      = &ctx->myPublicCryptoKey1;
    roundOneGenerateKeys.myPublicKey2      = &ctx->myPublicCryptoKey2;
    roundOneGenerateKeys.myPrivateV1       = &ctx->myPrivateCryptoV1;
    roundOneGenerateKeys.myPrivateV2       = &ctx->myPrivateCryptoV2;
    roundOneGenerateKeys.myPublicV1        = &ctx->myPublicCryptoV1;
    roundOneGenerateKeys.myPublicV2        = &ctx->myPublicCryptoV2;

    ECJPAKE_ALT_CHK(ECJPAKE_roundOneGenerateKeys(ctx->handle, &roundOneGenerateKeys));

     /* write X1 */
    ECJPAKE_ALT_CHK(tls_write_point( ctx->myPublicKeyMaterial1, &len, p, end - p));
    p += len;

    ECJPAKE_ALT_CHK(ecjpake_hash(ctx->md_info,
                                 ctx->point_format,
                                 ECCParams_NISTP256.generatorX,
                                 ctx->myPublicVMaterial1,
                                 ctx->myPublicKeyMaterial1,
                                 id,
                                 hash));

    big_num_reverse(hash, ECC_SECP256R1_LENGTH);

    /* generate round one ZKPs */
    ECJPAKE_OperationGenerateZKP_init(&operationGenerateZKP);
    operationGenerateZKP.curve              = &ECCParams_NISTP256;
    operationGenerateZKP.myPrivateKey       = &ctx->myPrivateCryptoKey1;
    operationGenerateZKP.myPrivateV         = &ctx->myPrivateCryptoV1;
    operationGenerateZKP.hash               = hash;
    operationGenerateZKP.r                  = r;

    ECJPAKE_ALT_CHK(ECJPAKE_generateZKP(ctx->handle, &operationGenerateZKP));

    /* write ZKP for X1 (V1 and r1) */
    ECJPAKE_ALT_CHK(tls_write_point(ctx->myPublicVMaterial1, &len, p, end - p));
    p += len;

    *p++ = ECC_SECP256R1_LENGTH;

    ECJPAKE_ALT_CHK(write_binary(p, end - p, r));
    p += ECC_SECP256R1_LENGTH;

    ECJPAKE_ALT_CHK(ecjpake_hash(ctx->md_info,
                                 ctx->point_format,
                                 ECCParams_NISTP256.generatorX,
                                 ctx->myPublicVMaterial2,
                                 ctx->myPublicKeyMaterial2,
                                 id,
                                 hash));

    big_num_reverse(hash, ECC_SECP256R1_LENGTH);

    ECJPAKE_OperationGenerateZKP_init(&operationGenerateZKP);
    operationGenerateZKP.curve              = &ECCParams_NISTP256;
    operationGenerateZKP.myPrivateKey       = &ctx->myPrivateCryptoKey2;
    operationGenerateZKP.myPrivateV         = &ctx->myPrivateCryptoV2;
    operationGenerateZKP.hash               = hash;
    operationGenerateZKP.r                  = r;

    ECJPAKE_ALT_CHK(ECJPAKE_generateZKP(ctx->handle, &operationGenerateZKP));

    /* write X2 */
    ECJPAKE_ALT_CHK(tls_write_point(ctx->myPublicKeyMaterial2, &len, p, end - p));
    p += len;

    /* write ZKP for X2 */
    ECJPAKE_ALT_CHK(tls_write_point(ctx->myPublicVMaterial2, &len, p, end - p));
    p += len;

    *p++ = ECC_SECP256R1_LENGTH;
    ECJPAKE_ALT_CHK(write_binary(p, end - p, r));

    p += ECC_SECP256R1_LENGTH;

    *olen = p - buf;

cleanup:
    return( ret );
}

/*
 * Read and process the first round message
 */
int mbedtls_ecjpake_read_round_one( mbedtls_ecjpake_context *ctx,
                                    const unsigned char *buf,
                                    size_t len )
{
    return ecjpake_kzkp_kzkp_read(ctx,
                                  &ctx->nistP256GeneratorCryptoKey,
                                  (uint8_t*)ECCParams_NISTP256.generatorX,
                                  ctx->theirPublicKeyMaterial1,
                                  &ctx->theirPublicCryptoKey1,
                                  ctx->theirPublicKeyMaterial2,
                                  &ctx->theirPublicCryptoKey2,
                                  ID_PEER,
                                  buf,
                                  len);
}

/*
 * Generate and write the first round message
 */
int mbedtls_ecjpake_write_round_one(mbedtls_ecjpake_context *ctx,
                                    unsigned char *buf, size_t len, size_t *olen,
                                    int (*f_rng)(void *, unsigned char *, size_t),
                                    void *p_rng)
{
    return( ecjpake_kzkp_kzkp_write(ctx, ECCParams_NISTP256.generatorX,
                                    ID_MINE, buf, len, olen, f_rng, p_rng));
}

/*
 * Read and process second round message.
 */
int mbedtls_ecjpake_read_round_two( mbedtls_ecjpake_context *ctx,
                                    const unsigned char *buf,
                                    size_t len )
{
    int ret;
    const unsigned char *p = buf;
    const unsigned char *end = buf + len;

    // Generate round two keys
    ECJPAKE_OperationRoundTwoGenerateKeys   roundTwoGenerateKeys;

    ECJPAKE_OperationRoundTwoGenerateKeys_init(&roundTwoGenerateKeys);
    roundTwoGenerateKeys.curve                 = &ECCParams_NISTP256;
    roundTwoGenerateKeys.myPrivateKey2         = &ctx->myPrivateCryptoKey2;
    roundTwoGenerateKeys.myPublicKey1          = &ctx->myPublicCryptoKey1;
    roundTwoGenerateKeys.myPublicKey2          = &ctx->myPublicCryptoKey2;
    roundTwoGenerateKeys.theirPublicKey1       = &ctx->theirPublicCryptoKey1;
    roundTwoGenerateKeys.theirPublicKey2       = &ctx->theirPublicCryptoKey2;
    roundTwoGenerateKeys.preSharedSecret       = &ctx->preSharedSecretCryptoKey;
    roundTwoGenerateKeys.theirNewGenerator     = &ctx->theirGeneratorKey;
    roundTwoGenerateKeys.myNewGenerator        = &ctx->myGeneratorKey;
    roundTwoGenerateKeys.myCombinedPrivateKey  = &ctx->myCombinedPrivateKey;
    roundTwoGenerateKeys.myCombinedPublicKey   = &ctx->myCombinedPublicKey;
    roundTwoGenerateKeys.myPrivateV            = &ctx->myPrivateCryptoV3;
    roundTwoGenerateKeys.myPublicV             = &ctx->myPublicCryptoV3;

    ECJPAKE_ALT_CHK(ECJPAKE_roundTwoGenerateKeys(ctx->handle,
                                                 &roundTwoGenerateKeys));

    /*
     * struct {
     *     ECParameters curve_params;   // only client reading server msg
     *     ECJPAKEKeyKP ecjpake_key_kp;
     * } Client/ServerECJPAKEParams;
     */
    if( ctx->role == MBEDTLS_ECJPAKE_CLIENT )
    {
       uint16_t curve_name_id;
        /*
         * We expect at least three bytes (see below)
         */
        if( len < 3 )
        {
            ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
            goto cleanup;
        }

        /*
         * First byte is curve_type; only named_curve is handled
         */
        if( *p++ != MBEDTLS_ECP_TLS_NAMED_CURVE )
        {
            ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
            goto cleanup;
        }

        /*
         * Next two bytes are the namedcurve value
         */
        curve_name_id = (p[1]) | (((uint16_t)p[0]) << 8);
        p += 2;

        if (curve_name_id != TLS_CURVE_SECP256R1_ID )
        {
            ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
            goto cleanup;
        }
    }

    ECJPAKE_ALT_CHK(ecjpake_k_zkp_read(ctx, &ctx->theirGeneratorKey,
                                       ctx->theirGenerator,
                                       ctx->theirCombinedPublicKeyMaterial1,
                                       &ctx->theirCombinedPublicKey,
                                       ID_PEER, &p, end));

cleanup:
    return( ret );

}

/*
 * Generate and write the second round message.
 */
int mbedtls_ecjpake_write_round_two( mbedtls_ecjpake_context *ctx,
                            unsigned char *buf, size_t len, size_t *olen,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng )
{
    int ret;
    unsigned char hash[ECC_SECP256R1_LENGTH];
    unsigned char r[ECC_SECP256R1_LENGTH];
    unsigned char *p = buf;
    const unsigned char *end = buf + len;
    size_t ec_len;

    ECJPAKE_OperationGenerateZKP            operationGenerateZKP;

    if (ctx->roundTwoGenerated == false)
    {
        ECJPAKE_OperationRoundTwoGenerateKeys   roundTwoGenerateKeys;

        ECJPAKE_OperationRoundTwoGenerateKeys_init(&roundTwoGenerateKeys);
        roundTwoGenerateKeys.curve                = &ECCParams_NISTP256;
        roundTwoGenerateKeys.myPrivateKey2        = &ctx->myPrivateCryptoKey2;
        roundTwoGenerateKeys.myPublicKey1         = &ctx->myPublicCryptoKey1;
        roundTwoGenerateKeys.myPublicKey2         = &ctx->myPublicCryptoKey2;
        roundTwoGenerateKeys.theirPublicKey1      = &ctx->theirPublicCryptoKey1;
        roundTwoGenerateKeys.theirPublicKey2      = &ctx->theirPublicCryptoKey2;
        roundTwoGenerateKeys.preSharedSecret      = &ctx->preSharedSecretCryptoKey;
        roundTwoGenerateKeys.theirNewGenerator    = &ctx->theirGeneratorKey;
        roundTwoGenerateKeys.myNewGenerator       = &ctx->myGeneratorKey;
        roundTwoGenerateKeys.myCombinedPrivateKey = &ctx->myCombinedPrivateKey;
        roundTwoGenerateKeys.myCombinedPublicKey  = &ctx->myCombinedPublicKey;
        roundTwoGenerateKeys.myPrivateV           = &ctx->myPrivateCryptoV3;
        roundTwoGenerateKeys.myPublicV            = &ctx->myPublicCryptoV3;

        ECJPAKE_ALT_CHK(ECJPAKE_roundTwoGenerateKeys(ctx->handle,
                                                     &roundTwoGenerateKeys));
        ctx->roundTwoGenerated = true;
    }

    ECJPAKE_ALT_CHK(ecjpake_hash(ctx->md_info,
                                 ctx->point_format,
                                 ctx->myGenerator,
                                 ctx->myPublicVMaterial3,
                                 ctx->myCombinedPublicKeyMaterial1,
                                 ID_MINE,
                                 hash));
    big_num_reverse(hash, ECC_SECP256R1_LENGTH);


    ECJPAKE_OperationGenerateZKP_init(&operationGenerateZKP);
    operationGenerateZKP.curve              = &ECCParams_NISTP256;
    operationGenerateZKP.myPrivateKey       = &ctx->myCombinedPrivateKey;
    operationGenerateZKP.myPrivateV         = &ctx->myPrivateCryptoV3;
    operationGenerateZKP.hash               = hash;
    operationGenerateZKP.r                  = r;

    ECJPAKE_ALT_CHK(ECJPAKE_generateZKP(ctx->handle, &operationGenerateZKP));

     /*
     * Now write things out
     *
     * struct {
     *     ECParameters curve_params;   // only server writing its message
     *     ECJPAKEKeyKP ecjpake_key_kp;
     * } Client/ServerECJPAKEParams;
     */
    if( ctx->role == MBEDTLS_ECJPAKE_SERVER )
    {
        if( end < p )
        {
            ret = MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
            goto cleanup;
        }

        ECJPAKE_ALT_CHK(mbedtls_ecp_tls_write_group(&ctx->grp, &ec_len,
                                                    p, end - p));
        p += ec_len;
    }

    if( end < p )
    {
        ret = MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
        goto cleanup;
    }

    /* write public key X */
    ECJPAKE_ALT_CHK(tls_write_point(ctx->myCombinedPublicKeyMaterial1,
                                    &ec_len,
                                    p,
                                    end - p));
    p += ec_len;

    /* write ZKP for X (V and r) */
    ECJPAKE_ALT_CHK(tls_write_point(ctx->myPublicVMaterial3, &len, p, end - p));
    p += len;

    *p = ECC_SECP256R1_LENGTH;
    p++;
    ECJPAKE_ALT_CHK(write_binary(p, end - p, r));
    p += ECC_SECP256R1_LENGTH;

    *olen = p - buf;

cleanup:
    return( ret );
}

/*
 * Derive PMS
 */
int mbedtls_ecjpake_derive_secret( mbedtls_ecjpake_context *ctx,
                            unsigned char *buf, size_t len, size_t *olen,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng )
{
    int ret;
    unsigned char kx[ECC_SECP256R1_LENGTH] = {0};
    ECJPAKE_OperationComputeSharedSecret    computeSharedSecret;
    CryptoKey                               sharedSecretCryptoKey;
    uint8_t                                 sharedSecretKeyingMaterial1[64];

    *olen = mbedtls_md_get_size( ctx->md_info );
    if( len < *olen )
    {
        return( MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL );
    }

    CryptoKeyPlaintext_initKey(&sharedSecretCryptoKey,
                               sharedSecretKeyingMaterial1,
                               sizeof(sharedSecretKeyingMaterial1));


    /* Generate shared secret */
    ECJPAKE_OperationComputeSharedSecret_init(&computeSharedSecret);
    computeSharedSecret.curve                      = &ECCParams_NISTP256;
    computeSharedSecret.myCombinedPrivateKey       = &ctx->myCombinedPrivateKey;
    computeSharedSecret.theirCombinedPublicKey     = &ctx->theirCombinedPublicKey;
    computeSharedSecret.theirPublicKey2            = &ctx->theirPublicCryptoKey2;
    computeSharedSecret.myPrivateKey2              = &ctx->myPrivateCryptoKey2;
    computeSharedSecret.sharedSecret               = &sharedSecretCryptoKey;

    ECJPAKE_ALT_CHK(ECJPAKE_computeSharedSecret(ctx->handle,
                                                &computeSharedSecret));

    memcpy(kx, sharedSecretKeyingMaterial1, ECC_SECP256R1_LENGTH);
    big_num_reverse(kx, 32);
    ECJPAKE_ALT_CHK(mbedtls_md(ctx->md_info, kx, ECC_SECP256R1_LENGTH, buf));

cleanup:
    return ret;
}

#undef ID_MINE
#undef ID_PEER

#endif /* MBEDTLS_ECJPAKE_ALT */
