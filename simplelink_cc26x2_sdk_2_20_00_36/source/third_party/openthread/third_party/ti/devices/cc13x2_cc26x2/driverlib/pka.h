/******************************************************************************
*  Filename:       pka.h
*  Revised:        2017-07-20 16:47:34 +0200 (Thu, 20 Jul 2017)
*  Revision:       49381
*
*  Description:    PKA header file.
*
*  Copyright (c) 2015 - 2017, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

//*****************************************************************************
//
//! \addtogroup peripheral_group
//! @{
//! \addtogroup pka_api
//! @{
//
//*****************************************************************************

#ifndef __PKA_H__
#define __PKA_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include "../inc/hw_types.h"
#include "../inc/hw_memmap.h"
#include "../inc/hw_ints.h"
#include "../inc/hw_pka.h"
#include "../inc/hw_pka_ram.h"
#include "interrupt.h"
#include "sys_ctrl.h"
#include "debug.h"
#include <string.h>

//*****************************************************************************
//
// Support for DriverLib in ROM:
// This section renames all functions that are not "static inline", so that
// calling these functions will default to implementation in flash. At the end
// of this file a second renaming will change the defaults to implementation in
// ROM for available functions.
//
// To force use of the implementation in flash, e.g. for debugging:
// - Globally: Define DRIVERLIB_NOROM at project level
// - Per function: Use prefix "NOROM_" when calling the function
//
//*****************************************************************************
#if !defined(DOXYGEN)
    #define PKAGetOpsStatus                 NOROM_PKAGetOpsStatus
    #define PKABigNumModStart               NOROM_PKABigNumModStart
    #define PKABigNumModGetResult           NOROM_PKABigNumModGetResult
    #define PKABigNumCmpStart               NOROM_PKABigNumCmpStart
    #define PKABigNumCmpGetResult           NOROM_PKABigNumCmpGetResult
    #define PKABigNumInvModStart            NOROM_PKABigNumInvModStart
    #define PKABigNumInvModGetResult        NOROM_PKABigNumInvModGetResult
    #define PKABigNumMultiplyStart          NOROM_PKABigNumMultiplyStart
    #define PKABigNumMultGetResult          NOROM_PKABigNumMultGetResult
    #define PKABigNumAddStart               NOROM_PKABigNumAddStart
    #define PKABigNumAddGetResult           NOROM_PKABigNumAddGetResult
    #define PKAEccMultiplyStart             NOROM_PKAEccMultiplyStart
    #define PKAEccMultiplyGetResult         NOROM_PKAEccMultiplyGetResult
    #define PKAEccAddStart                  NOROM_PKAEccAddStart
    #define PKAEccAddGetResult              NOROM_PKAEccAddGetResult
#endif


//*****************************************************************************
//
// Function return values
//
//*****************************************************************************
#define PKA_STATUS_SUCCESS             0 //!< Success
#define PKA_STATUS_FAILURE             1 //!< Failure
#define PKA_STATUS_INVALID_PARAM       2 //!< Invalid parameter
#define PKA_STATUS_BUF_UNDERFLOW       3 //!< Buffer underflow
#define PKA_STATUS_RESULT_0            4 //!< Result is all zeros
#define PKA_STATUS_A_GR_B              5 //!< Big number compare return status if the first big number is greater than the second.
#define PKA_STATUS_A_LT_B              6 //!< Big number compare return status if the first big number is less than the second.
#define PKA_STATUS_OPERATION_BUSY      7 //!< PKA operation is in progress.
#define PKA_STATUS_OPERATION_RDY       8 //!< No PKA operation is in progress.

//*****************************************************************************
//
// Union for parameters that forces 32-bit alignment on the byte array.
//
//*****************************************************************************
typedef union {
    uint8_t     byte[32];
    uint32_t    word[32 / sizeof(uint32_t)];
} PKA_EccParam256;

//*****************************************************************************
//
//! \brief X coordinate of the generator point of the NISTP256 curve.
//
//*****************************************************************************
extern const PKA_EccParam256 NISTP256_generatorX;


//*****************************************************************************
//
//! \brief Y coordinate of the generator point of the NISTP256 curve.
//
//*****************************************************************************
extern const PKA_EccParam256 NISTP256_generatorY;


//*****************************************************************************
//
//! \brief Prime of the NISTP256 curve.
//
//*****************************************************************************
extern const PKA_EccParam256 NISTP256_prime;


//*****************************************************************************
//
//! \brief a constant of the NISTP256 curve when expressed in short
//! Weierstrass form (y^3 = x^2 + a*x + b).
//
//*****************************************************************************
extern const PKA_EccParam256 NISTP256_a;


//*****************************************************************************
//
//! \brief b constant of the NISTP256 curve when expressed in short
//! Weierstrass form (y^3 = x^2 + a*x + b).
//
//*****************************************************************************
extern const PKA_EccParam256 NISTP256_b;


//*****************************************************************************
//
//! \brief Order of the NISTP256 curve.
//
//*****************************************************************************
extern const PKA_EccParam256 NISTP256_order;

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************

//*****************************************************************************
//
//! \brief Gets the PKA operation status.
//!
//! This function gets information on whether any PKA operation is in
//! progress or not. This function allows to check the PKA operation status
//! before starting any new PKA operation.
//!
//! \return Returns a status code.
//! - \ref PKA_STATUS_OPERATION_BUSY if the PKA operation is in progress.
//! - \ref PKA_STATUS_OPERATION_RDY if the PKA operation is not in progress.
//
//*****************************************************************************
extern uint32_t  PKAGetOpsStatus(void);

//*****************************************************************************
//
//! \brief Starts a big number modulus operation.
//!
//! This function starts the modulo operation on the big number \c bigNum
//! using the divisor \c modulus. The PKA RAM location where the result
//! will be available is stored in \c resultPKAMemAddr.
//!
//! \param [in] bigNum is the pointer to the big number on which modulo operation
//!        needs to be carried out.
//!
//! \param [in] bigNumLength is the size of the big number \c bigNum in bytes.
//!
//! \param [in] modulus is the pointer to the divisor.
//!
//! \param [in] modulusLength is the size of the divisor \c modulus in bytes.
//!
//! \param [out] resultPKAMemAddr is the pointer to the result vector location
//!        which will be set by this function.
//!
//!\return Returns a status code.
//! - \ref PKA_STATUS_SUCCESS if successful in starting the operation.
//! - \ref PKA_STATUS_OPERATION_BUSY, if the PKA module is busy doing
//!        some other operation.
//!
//! \sa PKABigNumModGetResult()
//
//*****************************************************************************
extern uint32_t  PKABigNumModStart(uint8_t *bigNum, uint32_t bigNumLength, uint8_t *modulus, uint32_t modulusLength, uint32_t *resultPKAMemAddr);

//*****************************************************************************
//
//! \brief Gets the result of the big number modulus operation.
//!
//! This function gets the result of the big number modulus operation which was
//! previously started using the function PKABigNumModStart().
//!
//! \param [out] resultBuf is the pointer to buffer where the result needs to
//!        be stored.
//!
//! \param [in] length is the size of the provided buffer in bytes.
//!
//! \param [in] resultPKAMemAddr is the address of the result location which
//!        was provided by the start function PKABigNumModStart().
//!
//! \return Returns a status code.
//! - \ref PKA_STATUS_SUCCESS if successful.
//! - \ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
//!        the operation.
//! - \ref PKA_STATUS_RESULT_0 if the result is all zeros.
//! - \ref PKA_STATUS_BUF_UNDERFLOW if the \c length is less than the length
//!        of the result.
//!
//! \sa PKABigNumModStart()
//
//*****************************************************************************
extern uint32_t  PKABigNumModGetResult(uint8_t *resultBuf, uint32_t length, uint32_t resultPKAMemAddr);

//*****************************************************************************
//
//! \brief Starts the comparison of two big numbers.
//!
//! This function starts the comparison of two big numbers pointed by
//! \c bigNum1 and \c bigNum2.
//!
//! \note \c bigNum1 and \c bigNum2 must have same size.
//!
//! \param [in] bigNum1 is the pointer to the first big number.
//!
//! \param [in] bigNum2 is the pointer to the second big number.
//!
//! \param [in] length is the size of the big numbers in bytes.
//!
//!\return Returns a status code.
//! - \ref PKA_STATUS_SUCCESS if successful in starting the operation.
//! - \ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
//!        some other operation.
//!
//! \sa PKABigNumCmpGetResult()
//
//*****************************************************************************
extern uint32_t  PKABigNumCmpStart(uint8_t *bigNum1, uint8_t *bigNum2, uint32_t length);

//*****************************************************************************
//
//! \brief Gets the result of the comparison operation of two big numbers.
//!
//! This function provides the results of the comparison of two big numbers
//! which was started using the PKABigNumCmpStart().
//!
//! \return Returns a status code.
//! - \ref PKA_STATUS_OPERATION_BUSY if the operation is in progress.
//! - \ref PKA_STATUS_SUCCESS if the two big numbers are equal.
//! - \ref PKA_STATUS_A_GR_B  if the first number is greater than the second.
//! - \ref PKA_STATUS_A_LT_B if the first number is less than the second.
//!
//! \sa PKABigNumCmpStart()
//
//*****************************************************************************
extern uint32_t  PKABigNumCmpGetResult(void);

//*****************************************************************************
//
//! \brief Starts a big number inverse modulo operation.
//!
//! This function starts the the inverse modulo operation on \c bigNum
//! using the divisor \c modulus.
//!
//! \param [in] bigNum is the pointer to the buffer containing the big number
//!        (dividend).
//!
//! \param [in] bigNumLength is the size of the \c bigNum in bytes.
//!
//! \param [in] modulus is the pointer to the buffer containing the divisor.
//!
//! \param [in] modulusLength is the size of the divisor in bytes.
//!
//! \param [out] resultPKAMemAddr is the pointer to the result vector location
//!        which will be set by this function.
//!
//!
//!\return Returns a status code.
//! - \ref PKA_STATUS_SUCCESS if successful in starting the operation.
//! - \ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
//!        some other operation.
//!
//! \sa PKABigNumInvModGetResult()
//
//*****************************************************************************
extern uint32_t  PKABigNumInvModStart(uint8_t *bigNum, uint32_t bigNumLength, uint8_t *modulus, uint32_t modulusLength, uint32_t *resultPKAMemAddr);


//*****************************************************************************
//
//! \brief Gets the result of the big number inverse modulo operation.
//!
//! This function gets the result of the big number inverse modulo operation
//! previously started using the function PKABigNumInvModStart().
//!
//! \param [out] resultBuf is the pointer to buffer where the result needs to be
//!        stored.
//!
//! \param [in] length is the size of the provided buffer in bytes.
//!
//! \param [in] resultPKAMemAddr is the address of the result location which
//!        was provided by the start function PKABigNumInvModStart().
//!
//! \return Returns a status code.
//! - \ref PKA_STATUS_SUCCESS if the operation is successful.
//! - \ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy performing
//!        the operation.
//! - \ref PKA_STATUS_RESULT_0 if the result is all zeros.
//! - \ref PKA_STATUS_BUF_UNDERFLOW if the length of the provided buffer is less
//!        than the result.
//!
//! \sa PKABigNumInvModStart()
//
//*****************************************************************************
extern uint32_t  PKABigNumInvModGetResult(uint8_t *resultBuf, uint32_t length, uint32_t resultPKAMemAddr);


//*****************************************************************************
//
//! \brief Starts the multiplication of two big numbers.
//!
//! \param [in] multiplicand is the pointer to the buffer containing the big
//!        number multiplicand.
//!
//! \param [in] multiplicandLength is the size of the multiplicand in bytes.
//!
//! \param [in] multiplier is the pointer to the buffer containing the big
//!        number multiplier.
//!
//! \param [in] multiplierLength is the size of the multiplier in bytes.
//!
//! \param [out] resultPKAMemAddr is the pointer to the result vector location
//!        which will be set by this function.
//!
//!\return Returns a status code.
//! - \ref PKA_STATUS_SUCCESS if successful in starting the operation.
//! - \ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
//!        some other operation.
//!
//! \sa PKABigNumMultGetResult()
//
//*****************************************************************************
extern uint32_t  PKABigNumMultiplyStart(uint8_t *multiplicand, uint32_t multiplicandLength, uint8_t *multiplier, uint32_t multiplierLength, uint32_t *resultPKAMemAddr);


//*****************************************************************************
//
//! \brief Gets the result of the big number multiplication.
//!
//! This function gets the result of the multiplication of two big numbers
//! operation previously started using the function PKABigNumMultiplyStart().
//!
//! \param [out] resultBuf is the pointer to buffer where the result needs to be
//!        stored.
//!
//! \param [in, out] resultLength is the address of the variable containing the length of the
//!        buffer in bytes. After the operation, the actual length of the resultant is stored
//!        at this address.
//!
//! \param [in] resultPKAMemAddr is the address of the result location which
//!        was provided by the start function PKABigNumMultiplyStart().
//!
//! \return Returns a status code.
//! - \ref PKA_STATUS_SUCCESS if the operation is successful.
//! - \ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy performing
//!        the operation.
//! - \ref PKA_STATUS_RESULT_0 if the result is all zeros.
//! - \ref PKA_STATUS_FAILURE if the operation is not successful.
//! - \ref PKA_STATUS_BUF_UNDERFLOW if the length of the provided buffer is less
//!        then the length of the result.
//!
//! \sa PKABigNumMultiplyStart()
//
//*****************************************************************************
extern uint32_t  PKABigNumMultGetResult(uint8_t *resultBuf, uint32_t *resultLength, uint32_t resultPKAMemAddr);

//*****************************************************************************
//
//! \brief Starts the addition of two big numbers.
//!
//! \param [in] bigNum1 is the pointer to the buffer containing the first
//!        big number.
//!
//! \param [in] bigNum1Length is the size of the first big number in bytes.
//!
//! \param [in] bigNum2 is the pointer to the buffer containing the second
//!        big number.
//!
//! \param [in] bigNum2Length is the size of the second big number in bytes.
//!
//! \param [out] resultPKAMemAddr is the pointer to the result vector location
//!        which will be set by this function.
//!
//!\return Returns a status code.
//! - \ref PKA_STATUS_SUCCESS if successful in starting the operation.
//! - \ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
//!        some other operation.
//!
//! \sa PKABigNumAddGetResult()
//
//*****************************************************************************
extern uint32_t  PKABigNumAddStart(uint8_t *bigNum1, uint32_t bigNum1Length, uint8_t *bigNum2, uint32_t bigNum2Length, uint32_t *resultPKAMemAddr);

//*****************************************************************************
//
//! \brief Gets the result of the addition operation on two big numbers.
//!
//! \param [out] resultBuf is the pointer to buffer where the result
//!        needs to be stored.
//!
//! \param [in, out] resultLength is the address of the variable containing
//!        the length of the buffer.  After the operation the actual length of the
//!        resultant is stored at this address.
//!
//! \param [in] resultPKAMemAddr is the address of the result location which
//!        was provided by the start function PKABigNumAddStart().
//!
//!
//! \return Returns a status code.
//! - \ref PKA_STATUS_SUCCESS if the operation is successful.
//! - \ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy performing
//!        the operation.
//! - \ref PKA_STATUS_RESULT_0 if the result is all zeros.
//! - \ref PKA_STATUS_FAILURE if the operation is not successful.
//! - \ref PKA_STATUS_BUF_UNDERFLOW if the length of the provided buffer is less
//!        then the length of the result.
//!
//! \sa PKABigNumAddStart()
//
//*****************************************************************************
extern uint32_t  PKABigNumAddGetResult(uint8_t *resultBuf, uint32_t *resultLength, uint32_t resultPKAMemAddr);

//*****************************************************************************
//
//! \brief Starts ECC multiplication.
//!
//! \param [in] scalar is pointer to the buffer containing the scalar
//!        value to be multiplied.
//!
//! \param [in] curvePointX is the pointer to the buffer containing the
//!        X coordinate of the elliptic curve point to be multiplied.
//!        The point must be on the given curve.
//!
//! \param [in] curvePointY is the pointer to the buffer containing the
//!        Y coordinate of the elliptic curve point to be multiplied.
//!        The point must be on the given curve.
//!
//! \param [in] prime is the prime of the curve.
//!
//! \param [in] a is the a constant of the curve when the curve equation is expressed
//!        in shor Weierstrass form (y^3 = x^2 + a*x + b).
//!
//! \param [in] b is the b constant of the curve when the curve equation is expressed
//!        in shor Weierstrass form (y^3 = x^2 + a*x + b).
//!
//! \param [in] length is the length of the curve parameters in bytes.
//!
//! \param [out] resultPKAMemAddr is the pointer to the result vector location
//!        which will be set by this function.
//!
//!\return Returns a status code.
//! - \ref PKA_STATUS_SUCCESS if successful in starting the operation.
//! - \ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
//!        some other operation.
//!
//! \sa PKAEccMultiplyGetResult()
//
//*****************************************************************************
extern uint32_t  PKAEccMultiplyStart(const uint8_t *scalar, const uint8_t *curvePointX, const uint8_t *curvePointY, const uint8_t *prime, const uint8_t *a, const uint8_t *b, uint32_t length, uint32_t *resultPKAMemAddr);

//*****************************************************************************
//
//! \brief Gets the result of ECC multiplication
//!
//! This function gets the result of ECC point multiplication operation on the
//! EC point and the scalar value, previously started using the function
//! PKAEccMultiplyStart().
//!
//! \param [out] curvePointX is the pointer to the structure where the X coordinate
//!         of the resultant EC point will be stored.
//!
//! \param [out] curvePointY is the pointer to the structure where the Y coordinate
//!         of the resultant EC point will be stored.
//!
//! \param [in] resultPKAMemAddr is the address of the result location which
//!        was provided by the start function PKAEccMultiplyStart().
//!
//! \param [in] length is the length of the curve parameters in bytes.
//!
//! \return Returns a status code.
//! - \ref PKA_STATUS_SUCCESS if the operation is successful.
//! - \ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy performing
//!        the operation.
//! - \ref PKA_STATUS_RESULT_0 if the result is all zeros.
//! - \ref PKA_STATUS_FAILURE if the operation is not successful.
//!
//! \sa PKAEccMultiplyStart()
//
//*****************************************************************************
extern uint32_t  PKAEccMultiplyGetResult(uint8_t *curvePointX, uint8_t *curvePointY, uint32_t resultPKAMemAddr, uint32_t length);

//*****************************************************************************
//
//! \brief Starts the ECC addition.
//!
//! \param [in] curvePoint1X is the pointer to the buffer containing the
//!        X coordinate of the first elliptic curve point to be added.
//!        The point must be on the given curve.
//!
//! \param [in] curvePoint1Y is the pointer to the buffer containing the
//!        Y coordinate of the first elliptic curve point to be added.
//!        The point must be on the given curve.
//!
//! \param [in] curvePoint2X is the pointer to the buffer containing the
//!        X coordinate of the second elliptic curve point to be added.
//!        The point must be on the given curve.
//!
//! \param [in] curvePoint2Y is the pointer to the buffer containing the
//!        Y coordinate of the second elliptic curve point to be added.
//!        The point must be on the given curve.
//!
//! \param [in] prime is the prime of the curve.
//!
//! \param [in] a is the a constant of the curve when the curve equation is expressed
//!        in shor Weierstrass form (y^3 = x^2 + a*x + b).
//!
//! \param [in] b is the b constant of the curve when the curve equation is expressed
//!        in shor Weierstrass form (y^3 = x^2 + a*x + b).
//!
//! \param [in] length is the length of the curve parameters in bytes.
//!
//! \param [out] resultPKAMemAddr is the pointer to the result vector location
//!        which will be set by this function.
//!
//!\return Returns a status code.
//! - \ref PKA_STATUS_SUCCESS if successful in starting the operation.
//! - \ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy doing
//!        some other operation.
//!
//! \sa PKAEccAddGetResult()
//
//*****************************************************************************
extern uint32_t  PKAEccAddStart(const uint8_t *curvePoint1X, const uint8_t *curvePoint1Y, const uint8_t *curvePoint2X, const uint8_t *curvePoint2Y, const uint8_t *prime, const uint8_t *a, uint32_t length, uint32_t *resultPKAMemAddr);

//*****************************************************************************
//
//! \brief Gets the result of the ECC addition
//!
//! This function gets the result of ECC point addition operation on the
//! on the two given EC points, previously started using the function
//! PKAEccAddStart().
//!
//! \param [out] curvePointX is the pointer to the structure where the X coordinate
//!         of the resultant EC point will be stored.
//!
//! \param [out] curvePointY is the pointer to the structure where the Y coordinate
//!         of the resultant EC point will be stored.
//!
//! \param [in] resultPKAMemAddr is the address of the result location which
//!        was provided by the start function PKAEccAddGetResult().
//!
//! \param [in] length is the length of the curve parameters in bytes.
//!
//! \return Returns a status code.
//! - \ref PKA_STATUS_SUCCESS if the operation is successful.
//! - \ref PKA_STATUS_OPERATION_BUSY if the PKA module is busy performing the operation.
//! - \ref PKA_STATUS_RESULT_0 if the result is all zeros.
//! - \ref PKA_STATUS_FAILURE if the operation is not successful.
//!
//! \sa PKAEccAddStart()
//
//*****************************************************************************
extern uint32_t  PKAEccAddGetResult(uint8_t *curvePointX, uint8_t *curvePointY, uint32_t resultPKAMemAddr, uint32_t length);

//*****************************************************************************
//
// Support for DriverLib in ROM:
// Redirect to implementation in ROM when available.
//
//*****************************************************************************
#if !defined(DRIVERLIB_NOROM) && !defined(DOXYGEN)
    #include "../driverlib/rom.h"
    #ifdef ROM_PKAGetOpsStatus
        #undef  PKAGetOpsStatus
        #define PKAGetOpsStatus                 ROM_PKAGetOpsStatus
    #endif
    #ifdef ROM_PKABigNumModStart
        #undef  PKABigNumModStart
        #define PKABigNumModStart               ROM_PKABigNumModStart
    #endif
    #ifdef ROM_PKABigNumModGetResult
        #undef  PKABigNumModGetResult
        #define PKABigNumModGetResult           ROM_PKABigNumModGetResult
    #endif
    #ifdef ROM_PKABigNumCmpStart
        #undef  PKABigNumCmpStart
        #define PKABigNumCmpStart               ROM_PKABigNumCmpStart
    #endif
    #ifdef ROM_PKABigNumCmpGetResult
        #undef  PKABigNumCmpGetResult
        #define PKABigNumCmpGetResult           ROM_PKABigNumCmpGetResult
    #endif
    #ifdef ROM_PKABigNumInvModStart
        #undef  PKABigNumInvModStart
        #define PKABigNumInvModStart            ROM_PKABigNumInvModStart
    #endif
    #ifdef ROM_PKABigNumInvModGetResult
        #undef  PKABigNumInvModGetResult
        #define PKABigNumInvModGetResult        ROM_PKABigNumInvModGetResult
    #endif
    #ifdef ROM_PKABigNumMultiplyStart
        #undef  PKABigNumMultiplyStart
        #define PKABigNumMultiplyStart          ROM_PKABigNumMultiplyStart
    #endif
    #ifdef ROM_PKABigNumMultGetResult
        #undef  PKABigNumMultGetResult
        #define PKABigNumMultGetResult          ROM_PKABigNumMultGetResult
    #endif
    #ifdef ROM_PKABigNumAddStart
        #undef  PKABigNumAddStart
        #define PKABigNumAddStart               ROM_PKABigNumAddStart
    #endif
    #ifdef ROM_PKABigNumAddGetResult
        #undef  PKABigNumAddGetResult
        #define PKABigNumAddGetResult           ROM_PKABigNumAddGetResult
    #endif
    #ifdef ROM_PKAEccMultiplyStart
        #undef  PKAEccMultiplyStart
        #define PKAEccMultiplyStart             ROM_PKAEccMultiplyStart
    #endif
    #ifdef ROM_PKAEccMultiplyGetResult
        #undef  PKAEccMultiplyGetResult
        #define PKAEccMultiplyGetResult         ROM_PKAEccMultiplyGetResult
    #endif
    #ifdef ROM_PKAEccAddStart
        #undef  PKAEccAddStart
        #define PKAEccAddStart                  ROM_PKAEccAddStart
    #endif
    #ifdef ROM_PKAEccAddGetResult
        #undef  PKAEccAddGetResult
        #define PKAEccAddGetResult              ROM_PKAEccAddGetResult
    #endif
#endif

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif  // __PKA_H__

//*****************************************************************************
//
//! Close the Doxygen group.
//! @}
//! @}
//
//*****************************************************************************
