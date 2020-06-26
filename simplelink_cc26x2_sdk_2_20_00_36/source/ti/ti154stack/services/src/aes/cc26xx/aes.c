/******************************************************************************

 @file  aes.c

 @brief Describe the purpose and contents of the file.

 Group: CMCU, LPRF
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2006-2018, Texas Instruments Incorporated
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
 * INCLUDES
 */

#include <stdint.h>
#include <string.h>
#include "aes.h"

#include "rom.h"

#if defined __GNUC__ && !defined i386 && !defined ccs && !defined __arm__
  #include "avr/include/avr/pgmspace.h"
#endif

/******************************************************************************
 * MACROS
 */

// Support for constant tables in flash
#ifdef __IAR_SYSTEMS_ICC__
#ifdef CC2420DB
  #define  PRGM     __flash
#elif defined __ICCARM__ || defined __ICC430__
  #define  PRGM
#else
  #define  PRGM     __code
#endif
  #define  IBOX(i)  InvSbox[i]
  #define  MUL2(i)  FFMult2[i]
  #define  MUL3(i)  FFMult3[i]
  #define  POLY(i)  Poly2Power[i]
  #define  POWR(i)  Power2Poly[i]
  #define  RCON(i)  RCon[i]
  #define  SBOX(i)  Sbox[i]
#elif defined __GNUC__ && !defined i386 && !defined ccs && !defined __arm__
  #define  PRGM     PROGMEM
  #define  IBOX(i)  pgm_read_byte_near(&InvSbox[i])
  #define  MUL2(i)  pgm_read_byte_near(&FFMult2[i])
  #define  MUL3(i)  pgm_read_byte_near(&FFMult3[i])
  #define  POLY(i)  pgm_read_byte_near(&Poly2Power[i])
  #define  POWR(i)  pgm_read_byte_near(&Power2Poly[i])
  #define  RCON(i)  pgm_read_byte_near(&RCon[i])
  #define  SBOX(i)  pgm_read_byte_near(&Sbox[i])
#else
  #define  PRGM
  #define  SBOX(i)  Sbox[i]
  #define  RCON(i)  RCon[i]
  #define  MUL2(i)  FFMult2[i]
  #define  MUL3(i)  FFMult3[i]
#endif

/******************************************************************************
 * CONSTANTS
 */

#define  BLOCK_LENGTH  128   // Defined by AES
#define  KEY_ZLENGTH   128   // ZigBee only uses 128 bit keys

#define  STATE_BLENGTH  16   // Number of bytes in State
#define  KEY_BLENGTH    16   // Number of bytes in Key

#define  Nb  4   // (BLOCK_LENGTH / 32) = Number of columns in State (also known as Nc)
#define  Nk  4   // (KEY_ZLENGTH / 32) = Number of columns in Key
#define  Nr  10  // Number of Rounds

#define  KEY_EXP_LENGTH  176   // Nb * (Nr+1) * 4

/******************************************************************************
 * TYPEDEFS
 */

/******************************************************************************
 * LOCAL VARIABLES
 */

const uint8_t PRGM FFMult2[256] =  // Multiply by 0x02 Table
{
    0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x16,0x18,0x1a,0x1c,0x1e,
    0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,0x30,0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e,
    0x40,0x42,0x44,0x46,0x48,0x4a,0x4c,0x4e,0x50,0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e,
    0x60,0x62,0x64,0x66,0x68,0x6a,0x6c,0x6e,0x70,0x72,0x74,0x76,0x78,0x7a,0x7c,0x7e,
    0x80,0x82,0x84,0x86,0x88,0x8a,0x8c,0x8e,0x90,0x92,0x94,0x96,0x98,0x9a,0x9c,0x9e,
    0xa0,0xa2,0xa4,0xa6,0xa8,0xaa,0xac,0xae,0xb0,0xb2,0xb4,0xb6,0xb8,0xba,0xbc,0xbe,
    0xc0,0xc2,0xc4,0xc6,0xc8,0xca,0xcc,0xce,0xd0,0xd2,0xd4,0xd6,0xd8,0xda,0xdc,0xde,
    0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xee,0xf0,0xf2,0xf4,0xf6,0xf8,0xfa,0xfc,0xfe,
    0x1b,0x19,0x1f,0x1d,0x13,0x11,0x17,0x15,0x0b,0x09,0x0f,0x0d,0x03,0x01,0x07,0x05,
    0x3b,0x39,0x3f,0x3d,0x33,0x31,0x37,0x35,0x2b,0x29,0x2f,0x2d,0x23,0x21,0x27,0x25,
    0x5b,0x59,0x5f,0x5d,0x53,0x51,0x57,0x55,0x4b,0x49,0x4f,0x4d,0x43,0x41,0x47,0x45,
    0x7b,0x79,0x7f,0x7d,0x73,0x71,0x77,0x75,0x6b,0x69,0x6f,0x6d,0x63,0x61,0x67,0x65,
    0x9b,0x99,0x9f,0x9d,0x93,0x91,0x97,0x95,0x8b,0x89,0x8f,0x8d,0x83,0x81,0x87,0x85,
    0xbb,0xb9,0xbf,0xbd,0xb3,0xb1,0xb7,0xb5,0xab,0xa9,0xaf,0xad,0xa3,0xa1,0xa7,0xa5,
    0xdb,0xd9,0xdf,0xdd,0xd3,0xd1,0xd7,0xd5,0xcb,0xc9,0xcf,0xcd,0xc3,0xc1,0xc7,0xc5,
    0xfb,0xf9,0xff,0xfd,0xf3,0xf1,0xf7,0xf5,0xeb,0xe9,0xef,0xed,0xe3,0xe1,0xe7,0xe5
};

const uint8_t PRGM FFMult3[256] =  // Multiply by 0x03 Table
{
    0x00,0x03,0x06,0x05,0x0c,0x0f,0x0a,0x09,0x18,0x1b,0x1e,0x1d,0x14,0x17,0x12,0x11,
    0x30,0x33,0x36,0x35,0x3c,0x3f,0x3a,0x39,0x28,0x2b,0x2e,0x2d,0x24,0x27,0x22,0x21,
    0x60,0x63,0x66,0x65,0x6c,0x6f,0x6a,0x69,0x78,0x7b,0x7e,0x7d,0x74,0x77,0x72,0x71,
    0x50,0x53,0x56,0x55,0x5c,0x5f,0x5a,0x59,0x48,0x4b,0x4e,0x4d,0x44,0x47,0x42,0x41,
    0xc0,0xc3,0xc6,0xc5,0xcc,0xcf,0xca,0xc9,0xd8,0xdb,0xde,0xdd,0xd4,0xd7,0xd2,0xd1,
    0xf0,0xf3,0xf6,0xf5,0xfc,0xff,0xfa,0xf9,0xe8,0xeb,0xee,0xed,0xe4,0xe7,0xe2,0xe1,
    0xa0,0xa3,0xa6,0xa5,0xac,0xaf,0xaa,0xa9,0xb8,0xbb,0xbe,0xbd,0xb4,0xb7,0xb2,0xb1,
    0x90,0x93,0x96,0x95,0x9c,0x9f,0x9a,0x99,0x88,0x8b,0x8e,0x8d,0x84,0x87,0x82,0x81,
    0x9b,0x98,0x9d,0x9e,0x97,0x94,0x91,0x92,0x83,0x80,0x85,0x86,0x8f,0x8c,0x89,0x8a,
    0xab,0xa8,0xad,0xae,0xa7,0xa4,0xa1,0xa2,0xb3,0xb0,0xb5,0xb6,0xbf,0xbc,0xb9,0xba,
    0xfb,0xf8,0xfd,0xfe,0xf7,0xf4,0xf1,0xf2,0xe3,0xe0,0xe5,0xe6,0xef,0xec,0xe9,0xea,
    0xcb,0xc8,0xcd,0xce,0xc7,0xc4,0xc1,0xc2,0xd3,0xd0,0xd5,0xd6,0xdf,0xdc,0xd9,0xda,
    0x5b,0x58,0x5d,0x5e,0x57,0x54,0x51,0x52,0x43,0x40,0x45,0x46,0x4f,0x4c,0x49,0x4a,
    0x6b,0x68,0x6d,0x6e,0x67,0x64,0x61,0x62,0x73,0x70,0x75,0x76,0x7f,0x7c,0x79,0x7a,
    0x3b,0x38,0x3d,0x3e,0x37,0x34,0x31,0x32,0x23,0x20,0x25,0x26,0x2f,0x2c,0x29,0x2a,
    0x0b,0x08,0x0d,0x0e,0x07,0x04,0x01,0x02,0x13,0x10,0x15,0x16,0x1f,0x1c,0x19,0x1a
};

const uint8_t PRGM RCon[10] =  // Rcon Table used in Key Schedule generation
{
    0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36
};

const uint8_t PRGM Sbox[256] =  // SubBytes Transformation Table
{
    0x63,0x7C,0x77,0x7B,0xF2,0x6B,0x6F,0xC5,0x30,0x01,0x67,0x2B,0xFE,0xD7,0xAB,0x76,
    0xCA,0x82,0xC9,0x7D,0xFA,0x59,0x47,0xF0,0xAD,0xD4,0xA2,0xAF,0x9C,0xA4,0x72,0xC0,
    0xB7,0xFD,0x93,0x26,0x36,0x3F,0xF7,0xCC,0x34,0xA5,0xE5,0xF1,0x71,0xD8,0x31,0x15,
    0x04,0xC7,0x23,0xC3,0x18,0x96,0x05,0x9A,0x07,0x12,0x80,0xE2,0xEB,0x27,0xB2,0x75,
    0x09,0x83,0x2C,0x1A,0x1B,0x6E,0x5A,0xA0,0x52,0x3B,0xD6,0xB3,0x29,0xE3,0x2F,0x84,
    0x53,0xD1,0x00,0xED,0x20,0xFC,0xB1,0x5B,0x6A,0xCB,0xBE,0x39,0x4A,0x4C,0x58,0xCF,
    0xD0,0xEF,0xAA,0xFB,0x43,0x4D,0x33,0x85,0x45,0xF9,0x02,0x7F,0x50,0x3C,0x9F,0xA8,
    0x51,0xA3,0x40,0x8F,0x92,0x9D,0x38,0xF5,0xBC,0xB6,0xDA,0x21,0x10,0xFF,0xF3,0xD2,
    0xCD,0x0C,0x13,0xEC,0x5F,0x97,0x44,0x17,0xC4,0xA7,0x7E,0x3D,0x64,0x5D,0x19,0x73,
    0x60,0x81,0x4F,0xDC,0x22,0x2A,0x90,0x88,0x46,0xEE,0xB8,0x14,0xDE,0x5E,0x0B,0xDB,
    0xE0,0x32,0x3A,0x0A,0x49,0x06,0x24,0x5C,0xC2,0xD3,0xAC,0x62,0x91,0x95,0xE4,0x79,
    0xE7,0xC8,0x37,0x6D,0x8D,0xD5,0x4E,0xA9,0x6C,0x56,0xF4,0xEA,0x65,0x7A,0xAE,0x08,
    0xBA,0x78,0x25,0x2E,0x1C,0xA6,0xB4,0xC6,0xE8,0xDD,0x74,0x1F,0x4B,0xBD,0x8B,0x8A,
    0x70,0x3E,0xB5,0x66,0x48,0x03,0xF6,0x0E,0x61,0x35,0x57,0xB9,0x86,0xC1,0x1D,0x9E,
    0xE1,0xF8,0x98,0x11,0x69,0xD9,0x8E,0x94,0x9B,0x1E,0x87,0xE9,0xCE,0x55,0x28,0xDF,
    0x8C,0xA1,0x89,0x0D,0xBF,0xE6,0x42,0x68,0x41,0x99,0x2D,0x0F,0xB0,0x54,0xBB,0x16
};

#ifdef INCLUDE_AES_DECRYPT
const uint8_t PRGM InvSbox[256] =  // Inverse SubBytes Transformation Table
{
    0x52,0x09,0x6a,0xD5,0x30,0x36,0xA5,0x38,0xBF,0x40,0xA3,0x9E,0x81,0xF3,0xD7,0xFB,
    0x7C,0xE3,0x39,0x82,0x9B,0x2F,0xFF,0x87,0x34,0x8E,0x43,0x44,0xC4,0xDE,0xE9,0xCB,
    0x54,0x7B,0x94,0x32,0xA6,0xC2,0x23,0x3D,0xEE,0x4C,0x95,0x0B,0x42,0xFA,0xC3,0x4E,
    0x08,0x2E,0xA1,0x66,0x28,0xD9,0x24,0xB2,0x76,0x5B,0xA2,0x49,0x6D,0x8B,0xD1,0x25,
    0x72,0xF8,0xF6,0x64,0x86,0x68,0x98,0x16,0xD4,0xA4,0x5C,0xCC,0x5D,0x65,0xB6,0x92,
    0x6C,0x70,0x48,0x50,0xFD,0xED,0xB9,0xDA,0x5E,0x15,0x46,0x57,0xA7,0x8D,0x9D,0x84,
    0x90,0xD8,0xAB,0x00,0x8C,0xBC,0xD3,0x0A,0xF7,0xE4,0x58,0x05,0xB8,0xB3,0x45,0x06,
    0xD0,0x2C,0x1E,0x8F,0xCA,0x3F,0x0F,0x02,0xC1,0xAF,0xBD,0x03,0x01,0x13,0x8A,0x6B,
    0x3A,0x91,0x11,0x41,0x4F,0x67,0xDC,0xEA,0x97,0xF2,0xCF,0xCE,0xF0,0xB4,0xE6,0x73,
    0x96,0xAC,0x74,0x22,0xE7,0xAD,0x35,0x85,0xE2,0xF9,0x37,0xE8,0x1C,0x75,0xDF,0x6E,
    0x47,0xF1,0x1A,0x71,0x1D,0x29,0xC5,0x89,0x6F,0xB7,0x62,0x0E,0xAA,0x18,0xBE,0x1B,
    0xFC,0x56,0x3E,0x4B,0xC6,0xD2,0x79,0x20,0x9A,0xDB,0xC0,0xFE,0x78,0xCD,0x5A,0xF4,
    0x1F,0xDD,0xA8,0x33,0x88,0x07,0xC7,0x31,0xB1,0x12,0x10,0x59,0x27,0x80,0xEC,0x5F,
    0x60,0x51,0x7F,0xA9,0x19,0xB5,0x4A,0x0D,0x2D,0xE5,0x7A,0x9F,0x93,0xC9,0x9C,0xEF,
    0xA0,0xE0,0x3B,0x4D,0xAE,0x2A,0xF5,0xB0,0xC8,0xEB,0xBB,0x3C,0x83,0x53,0x99,0x61,
    0x17,0x2B,0x04,0x7E,0xBA,0x77,0xD6,0x26,0xE1,0x69,0x14,0x63,0x55,0x21,0x0C,0x7D
};

#ifdef USE_FF_TABLE
const uint8_t PRGM Poly2Power[256] =  // GF(2^8) Polynomial to Power Table
{
    0x00,0x00,0x19,0x01,0x32,0x02,0x1a,0xc6,0x4b,0xc7,0x1b,0x68,0x33,0xee,0xdf,0x03,
    0x64,0x04,0xe0,0x0e,0x34,0x8d,0x81,0xef,0x4c,0x71,0x08,0xc8,0xf8,0x69,0x1c,0xc1,
    0x7d,0xc2,0x1d,0xb5,0xf9,0xb9,0x27,0x6a,0x4d,0xe4,0xa6,0x72,0x9a,0xc9,0x09,0x78,
    0x65,0x2f,0x8a,0x05,0x21,0x0f,0xe1,0x24,0x12,0xf0,0x82,0x45,0x35,0x93,0xda,0x8e,
    0x96,0x8f,0xdb,0xbd,0x36,0xd0,0xce,0x94,0x13,0x5c,0xd2,0xf1,0x40,0x46,0x83,0x38,
    0x66,0xdd,0xfd,0x30,0xbf,0x06,0x8b,0x62,0xb3,0x25,0xe2,0x98,0x22,0x88,0x91,0x10,
    0x7e,0x6e,0x48,0xc3,0xa3,0xb6,0x1e,0x42,0x3a,0x6b,0x28,0x54,0xfa,0x85,0x3d,0xba,
    0x2b,0x79,0x0a,0x15,0x9b,0x9f,0x5e,0xca,0x4e,0xd4,0xac,0xe5,0xf3,0x73,0xa7,0x57,
    0xaf,0x58,0xa8,0x50,0xf4,0xea,0xd6,0x74,0x4f,0xae,0xe9,0xd5,0xe7,0xe6,0xad,0xe8,
    0x2c,0xd7,0x75,0x7a,0xeb,0x16,0x0b,0xf5,0x59,0xcb,0x5f,0xb0,0x9c,0xa9,0x51,0xa0,
    0x7f,0x0c,0xf6,0x6f,0x17,0xc4,0x49,0xec,0xd8,0x43,0x1f,0x2d,0xa4,0x76,0x7b,0xb7,
    0xcc,0xbb,0x3e,0x5a,0xfb,0x60,0xb1,0x86,0x3b,0x52,0xa1,0x6c,0xaa,0x55,0x29,0x9d,
    0x97,0xb2,0x87,0x90,0x61,0xbe,0xdc,0xfc,0xbc,0x95,0xcf,0xcd,0x37,0x3f,0x5b,0xd1,
    0x53,0x39,0x84,0x3c,0x41,0xa2,0x6d,0x47,0x14,0x2a,0x9e,0x5d,0x56,0xf2,0xd3,0xab,
    0x44,0x11,0x92,0xd9,0x23,0x20,0x2e,0x89,0xb4,0x7c,0xb8,0x26,0x77,0x99,0xe3,0xa5,
    0x67,0x4a,0xed,0xde,0xc5,0x31,0xfe,0x18,0x0d,0x63,0x8c,0x80,0xc0,0xf7,0x70,0x07
};

const uint8_t PRGM Power2Poly[256] =   // GF(2^8) Power to Polynomial Table
{
    0x01,0x03,0x05,0x0f,0x11,0x33,0x55,0xff,0x1a,0x2e,0x72,0x96,0xa1,0xf8,0x13,0x35,
    0x5f,0xe1,0x38,0x48,0xd8,0x73,0x95,0xa4,0xf7,0x02,0x06,0x0a,0x1e,0x22,0x66,0xaa,
    0xe5,0x34,0x5c,0xe4,0x37,0x59,0xeb,0x26,0x6a,0xbe,0xd9,0x70,0x90,0xab,0xe6,0x31,
    0x53,0xf5,0x04,0x0c,0x14,0x3c,0x44,0xcc,0x4f,0xd1,0x68,0xb8,0xd3,0x6e,0xb2,0xcd,
    0x4c,0xd4,0x67,0xa9,0xe0,0x3b,0x4d,0xd7,0x62,0xa6,0xf1,0x08,0x18,0x28,0x78,0x88,
    0x83,0x9e,0xb9,0xd0,0x6b,0xbd,0xdc,0x7f,0x81,0x98,0xb3,0xce,0x49,0xdb,0x76,0x9a,
    0xb5,0xc4,0x57,0xf9,0x10,0x30,0x50,0xf0,0x0b,0x1d,0x27,0x69,0xbb,0xd6,0x61,0xa3,
    0xfe,0x19,0x2b,0x7d,0x87,0x92,0xad,0xec,0x2f,0x71,0x93,0xae,0xe9,0x20,0x60,0xa0,
    0xfb,0x16,0x3a,0x4e,0xd2,0x6d,0xb7,0xc2,0x5d,0xe7,0x32,0x56,0xfa,0x15,0x3f,0x41,
    0xc3,0x5e,0xe2,0x3d,0x47,0xc9,0x40,0xc0,0x5b,0xed,0x2c,0x74,0x9c,0xbf,0xda,0x75,
    0x9f,0xba,0xd5,0x64,0xac,0xef,0x2a,0x7e,0x82,0x9d,0xbc,0xdf,0x7a,0x8e,0x89,0x80,
    0x9b,0xb6,0xc1,0x58,0xe8,0x23,0x65,0xaf,0xea,0x25,0x6f,0xb1,0xc8,0x43,0xc5,0x54,
    0xfc,0x1f,0x21,0x63,0xa5,0xf4,0x07,0x09,0x1b,0x2d,0x77,0x99,0xb0,0xcb,0x46,0xca,
    0x45,0xcf,0x4a,0xde,0x79,0x8b,0x86,0x91,0xa8,0xe3,0x3e,0x42,0xc6,0x51,0xf3,0x0e,
    0x12,0x36,0x5a,0xee,0x29,0x7b,0x8d,0x8c,0x8f,0x8a,0x85,0x94,0xa7,0xf2,0x0d,0x17,
    0x39,0x4b,0xdd,0x7c,0x84,0x97,0xa2,0xfd,0x1c,0x24,0x6c,0xb4,0xc7,0x52,0xf6,0x01
};
#endif  // USE_FF_TABLE
#endif  // INCLUDE_AES_DECRYPT

/******************************************************************************
 * GLOBAL VARIABLES
 */

void (*pSspAesEncrypt_Sw)( uint8_t *, uint8_t * ) = (void*)0;

/******************************************************************************
 * FUNCTION PROTOTYPES
 */

/******************************************************************************
 * @fn      ssp_KeyInit_Sw
 *
 * @brief   Writes the key into the CC2430
 *
 * input parameters
 *
 * @param   AesKey  - Pointer to AES Key.
 *
 * @return  None
 */
void ssp_KeyInit_Sw( uint8_t *AesKey )
{
}

/******************************************************************************
 * @fn      sspAesEncrypt_Sw
 *
 * @brief   Encrypts 16 byte block using CC2430 HW aes encryption engine
 *
 * input parameters
 *
 * @param   AesKey  - Pointer to AES Key.
 * @param   Cstate  - Pointer to input data.
 *
 * output parameters
 *
 * @param   Cstate  - Pointer to encrypted data.
 *
 * @return  None
 *
 */
void sspAesEncrypt_Sw( uint8_t *AesKey, uint8_t *Cstate )
{
  MAP_sspAesEncryptBasic_Sw( AesKey, Cstate );
}

/******************************************************************************
 * @fn      sspKeyExpansion_Sw
 *
 * @brief   Performs key expansion to create the entire Key Schedule.
 *
 * input parameters
 *
 * @param   AesKey  - Pointer to AES Key.
 * @param   KeyExp  - Pointer Key Expansion buffer.  Size = 176 bytes
 *
 * output parameters
 *
 * @param   KeyExp[]    - Key Schedule
 *
 * @return  None
 *
 */
void sspKeyExpansion_Sw( uint8_t *AesKey, uint8_t *KeyExp )
{
  uint8_t temp[4], i, t, rc;

  osal_memcpy( KeyExp, AesKey, 16 );

  rc = 0;     // index to RCon[] table

  for (i=16; i < KEY_EXP_LENGTH; i+=16)
  {
    //  RotByte, SubByte, Rcon
    t= SBOX(KeyExp[i-4]);
    temp[0] = SBOX(KeyExp[i-3]) ^ RCON(rc++);
    temp[1] = SBOX(KeyExp[i-2]);
    temp[2] = SBOX(KeyExp[i-1]);
    temp[3] = t;

    KeyExp[i]   = KeyExp[i-16] ^ temp[0];
    KeyExp[i+1] = KeyExp[i-15] ^ temp[1];
    KeyExp[i+2] = KeyExp[i-14] ^ temp[2];
    KeyExp[i+3] = KeyExp[i-13] ^ temp[3];

    KeyExp[i+4] = KeyExp[i-12] ^ KeyExp[i];
    KeyExp[i+5] = KeyExp[i-11] ^ KeyExp[i+1];
    KeyExp[i+6] = KeyExp[i-10] ^ KeyExp[i+2];
    KeyExp[i+7] = KeyExp[i-9] ^ KeyExp[i+3];

    KeyExp[i+8] = KeyExp[i-8] ^ KeyExp[i+4];
    KeyExp[i+9] = KeyExp[i-7] ^ KeyExp[i+5];
    KeyExp[i+10] = KeyExp[i-6] ^ KeyExp[i+6];
    KeyExp[i+11] = KeyExp[i-5] ^ KeyExp[i+7];

    KeyExp[i+12] = KeyExp[i-4] ^ KeyExp[i+8];
    KeyExp[i+13] = KeyExp[i-3] ^ KeyExp[i+9];
    KeyExp[i+14] = KeyExp[i-2] ^ KeyExp[i+10];
    KeyExp[i+15] = KeyExp[i-1] ^ KeyExp[i+11];
  }
}

/******************************************************************************
 * @fn      sspAesEncryptKeyExp_Sw
 *
 * @brief   Performs AES-128 encryption using Key Expansion buffer.  The
 *          plaintext input will be overwritten with the ciphertext output.
 *
 * input parameters
 *
 * @param   KeyExp  - Pointer to Key Expansion buffer.
 * @param   Cstate  - Pointer to plaintext input.
 *
 * output parameters
 *
 * @param   Cstate[]    - Ciphertext output
 *
 * @return  None
 *
 */
void sspAesEncryptKeyExp_Sw( uint8_t *KeyExp, uint8_t *Cstate )
{
  uint8_t i, round;

  for (round=0; round < 9; round++)
  {
    MAP_AddRoundKeySubBytes_Sw( KeyExp, Cstate );
    KeyExp += 16;
    MAP_ShiftRows_Sw( Cstate );
    MAP_MixColumns_Sw( Cstate );
  }
  MAP_AddRoundKeySubBytes_Sw( KeyExp, Cstate );
  KeyExp += 16;
  MAP_ShiftRows_Sw( Cstate );
  for (i=0; i < 16; i++)  Cstate[i] ^= KeyExp[i];
}

/******************************************************************************
 * @fn      sspAesEncryptBasic_Sw
 *
 * @brief   Performs AES-128 encryption without using Key Expansion.  The
 *          plaintext input will be overwritten with the ciphertext output.
 *
 * input parameters
 *
 * @param   AesKey  - Pointer to AES Key.
 * @param   Cstate  - Pointer to plaintext input.
 *
 * output parameters
 *
 * @param   Cstate[]    - Ciphertext output
 *
 * @return  None
 *
 */
void sspAesEncryptBasic_Sw( uint8_t *AesKey, uint8_t *Cstate )
{
  uint8_t RKBuff[KEY_BLENGTH];
  uint8_t i, round;

  osal_memcpy( RKBuff, AesKey, 16 );
  
  for (round=0; round < 9; round++)
  {
    MAP_AddRoundKeySubBytes_Sw( RKBuff, Cstate );
    MAP_ShiftRows_Sw( Cstate );
    MAP_MixColumns_Sw( Cstate );
    MAP_RoundKey_Sw( RKBuff, round );
  }
  MAP_AddRoundKeySubBytes_Sw( RKBuff, Cstate );
  MAP_ShiftRows_Sw( Cstate );
  MAP_RoundKey_Sw( RKBuff, round );
  for (i=0; i < 16; i++)  Cstate[i] ^= RKBuff[i];
}

/******************************************************************************
 * @fn      RoundKey_Sw
 *
 * @brief   Generates the next Key Schedule based on the current Key Schedule.
 *
 * input parameters
 *
 * @param   W    - Pointer to the current Key Schedule.
 * @param   rc   - Round counter.
 *
 * output parameters
 *
 * @param   W[]     - Next Key Schedule
 *
 * @return  None
 *
 */
void RoundKey_Sw( uint8_t *W, uint8_t rc )
{
  uint8_t temp[4], t;

  //  RotByte, SubByte, Rcon
  t = SBOX(W[12]);
  temp[0] = SBOX(W[13]) ^ RCON(rc);
  temp[1] = SBOX(W[14]);
  temp[2] = SBOX(W[15]);
  temp[3] = t;

  W[0] ^= temp[0];
  W[1] ^= temp[1];
  W[2] ^= temp[2];
  W[3] ^= temp[3];

  W[4] ^= W[0];
  W[5] ^= W[1];
  W[6] ^= W[2];
  W[7] ^= W[3];

  W[8] ^= W[4];
  W[9] ^= W[5];
  W[10] ^= W[6];
  W[11] ^= W[7];

  W[12] ^= W[8];
  W[13] ^= W[9];
  W[14] ^= W[10];
  W[15] ^= W[11];
}

/******************************************************************************
 * @fn      AddRoundKeySubBytes_Sw
 *
 * @brief   Performs the AddRoundKey and SubBytes function.
 *
 * input parameters
 *
 * @param   KeySch  - Pointer to the Key Schedule.
 * @param   Cstate  - Pointer to cipher state.
 *
 * output parameters
 *
 * @param   Cstate[]    - updated cipher state
 *
 * @return  None
 *
 */
void AddRoundKeySubBytes_Sw( uint8_t *KeySch, uint8_t *Cstate )
{
  uint8_t i;

  for (i=0; i < 16; i++)
  {
    Cstate[i] = SBOX(Cstate[i] ^ KeySch[i]);
  }
}

/******************************************************************************
 * @fn      ShiftRows_Sw
 *
 * @brief   Performs the ShiftRows_Sw function on the cipher state.
 *
 * input parameters
 *
 * @param   Cstate  - The current cipher state
 *
 * output parameters
 *
 * @param   Cstate[]    - Updated cipher state
 *
 * @return  None
 *
 */
void ShiftRows_Sw( uint8_t *Cstate )
{
  uint8_t temp;

  // Row 0 is not shifted

  // Row 1 is shifted down by 1
  temp = Cstate[1];
  Cstate[1] = Cstate[5];
  Cstate[5] = Cstate[9];
  Cstate[9] = Cstate[13];
  Cstate[13] = temp;

  // Row 2 is shifted down by 2
  temp = Cstate[2];
  Cstate[2] = Cstate[10];
  Cstate[10] = temp;
  temp = Cstate[6];
  Cstate[6] = Cstate[14];
  Cstate[14] = temp;

  // Row 3 is shifted down by 3
  temp = Cstate[3];
  Cstate[3] = Cstate[15];
  Cstate[15] = Cstate[11];
  Cstate[11] = Cstate[7];
  Cstate[7] = temp;
}

/******************************************************************************
 * @fn      MixColumns_Sw
 *
 * @brief   Performs the MixColumns_Sw function on the cipher state.
 *
 * input parameters
 *
 * @param   Cstate  - The current cipher state
 *
 * output parameters
 *
 * @param   Cstate[]    - Updated cipher state
 *
 * @return  None
 *
 */
void MixColumns_Sw( uint8_t *Cstate )
{
  uint8_t r, c, t[4];

  for (c=0; c < 16; c+=4)
  {
    for (r=0; r < 4; r++)  t[r] = Cstate[c+r];

    Cstate[c]   = MUL2(t[0]) ^ MUL3(t[1]) ^ t[2] ^ t[3];
    Cstate[c+1] = MUL2(t[1]) ^ MUL3(t[2]) ^ t[3] ^ t[0];
    Cstate[c+2] = MUL2(t[2]) ^ MUL3(t[3]) ^ t[0] ^ t[1];
    Cstate[c+3] = MUL2(t[3]) ^ MUL3(t[0]) ^ t[1] ^ t[2];
  }
}

#ifdef INCLUDE_AES_DECRYPT
/******************************************************************************
 * @fn      FFMult_Sw
 *
 * @brief   Performs Finite-field multiplication over GF(2^8).
 *
 * input parameters
 *
 * @param   a   - field element in polynomial representation
 * @param   b   - field element in polynomial representation
 *
 * output parameters
 *
 * @return  - the field element (a*b) in polynomial representation
 *
 */
uint8_t FFMult_Sw( uint8_t a, uint8_t b )
{
#ifdef USE_FF_TABLE
  uint16_t t;

  if (a && b)
  {
    t = POLY(a) + POLY(b);
    if (t >= 255)  t -= 255;
    return( POWR(t) );
  }
  else  return( 0 );
#else
  uint8_t t;
  uint8_t r = 0;

  while (a)
  {
    if (a & 0x01) r ^= b;

    t = b & 0x80;
    b <<= 1;
    if (t) b ^= 0x1b;

    a >>= 1;
  }
  return( r );
#endif  // USE_FF_TABLE
}

#ifdef USE_KEY_EXPANSION
/******************************************************************************
 * @fn      sspAesDecrypt_Sw
 *
 * @brief   Performs AES-128 decryption.  The ciphertext input will be
 *          overwritten with the plaintext output.
 *
 * input parameters
 *
 * @param   KeyExp  - Pointer to Key Expansion buffer.
 * @param   Cstate  - Pointer to ciphertext input.
 *
 * output parameters
 *
 * @param   Cstate[]    - Plaintext output
 *
 * @return  None
 *
 */
void sspAesDecrypt_Sw( uint8_t *KeyExp, uint8_t *Cstate )
{
  uint8_t round = Nr;

  MAP_InvAddRoundKey_Sw( round, KeyExp, Cstate );

  // Nr-1 rounds
  for (round=(Nr-1); round >= 1; round--)
  {
    MAP_InvShiftRows_Sw( Cstate );
    MAP_InvSubBytes_Sw( Cstate );
    MAP_InvAddRoundKey_Sw( round, KeyExp, Cstate );
    MAP_InvMixColumns_Sw( Cstate );
  }

  // last round
  MAP_InvShiftRows_Sw( Cstate );
  MAP_InvSubBytes_Sw( Cstate );
  MAP_InvAddRoundKey_Sw( round, KeyExp, Cstate );
}

/******************************************************************************
 * @fn      InvAddRoundKey_Sw
 *
 * @brief   Performs the InvAddRoundKey_Sw function using the Key Expansion buffer.
 *
 * input parameters
 *
 * @param   round   - The current encryption round.
 * @param   KeyExp  - Pointer to the Key Expansion buffer.
 * @param   Cstate  - Pointer to cipher state.
 *
 * output parameters
 *
 * @param   Cstate[]    - updated cipher state
 *
 * @return  None
 *
 */
void InvAddRoundKey_Sw( uint8_t round, uint8_t *KeyExp, uint8_t *Cstate )
{
  uint8_t i, t;

  t = round << 4;

  for (i=0; i < 4*Nb; i++)
  {
    Cstate[i] ^= KeyExp[i+t];
  }
}

#else   // ~USE_KEY_EXPANSION

/******************************************************************************
 * @fn      sspAesDecrypt_Sw
 *
 * @brief   Performs AES-128 decryption.  The ciphertext input will be
 *          overwritten with the plaintext output.
 *
 * input parameters
 *
 * @param   AesKey  - Pointer to AES Key.
 * @param   Cstate  - Pointer to ciphertext input.
 *
 * output parameters
 *
 * @param   Cstate[]    - Plaintext output
 *
 * @return  None
 *
 */
void sspAesDecrypt_Sw( uint8_t *AesKey, uint8_t *Cstate )
{
  uint8_t RKBuff[KEY_BLENGTH];
  uint8_t round = Nr;

  MAP_InvAddRoundKey_Sw( round, RKBuff, AesKey, Cstate );

  // Nr-1 rounds
  for (round=(Nr-1); round >= 1; round--)
  {
    MAP_InvShiftRows_Sw( Cstate );
    MAP_InvSubBytes_Sw( Cstate );
    MAP_InvAddRoundKey_Sw( round, RKBuff, AesKey, Cstate );
    MAP_InvMixColumns_Sw( Cstate);
  }

  // last round
  MAP_InvShiftRows_Sw( Cstate );
  MAP_InvSubBytes_Sw( Cstate );
  MAP_InvAddRoundKey_Sw( round, RKBuff, AesKey, Cstate );
}

/******************************************************************************
 * @fn      InvAddRoundKey_Sw
 *
 * @brief   Performs the InvAddRoundKey_Sw function using the Key Schedule created
 *          on-the-fly.
 *
 * input parameters
 *
 * @param   round   - The current encryption round.
 * @param   RKBuff  - Pointer to the current Key Schedule.
 * @param   AesKey  - Pointer to the AES Key.
 * @param   Cstate  - Pointer to cipher state.
 *
 * output parameters
 *
 * @param   Cstate[]    - updated cipher state
 *
 * @return  None
 *
 */
void InvAddRoundKey_Sw( uint8_t round, uint8_t *RKBuff, uint8_t *AesKey, uint8_t *Cstate )
{
  uint8_t i;

  MAP_InvRoundKey_Sw( RKBuff, round, AesKey );

  for (i=0; i < 4*Nb; i++)
  {
    Cstate[i] ^= RKBuff[i];
  }
}

/******************************************************************************
 * @fn      InvRoundKey_Sw
 *
 * @brief   Generates the next the Key Schedule based on the round.
 *
 * input parameters
 *
 * @param   W       - Pointer to Key Schedule.
 * @param   round   - The current encryption round.
 * @param   AesKey  - Pointer to the AES Key.
 *
 * output parameters
 *
 * @param   W[]     - Next Key Schedule
 *
 * @return  None
 *
 */
void InvRoundKey_Sw( uint8_t *W, uint8_t round, uint8_t *AesKey )
{
  uint8_t temp[4], t, r;
  uint8_t i, j;

  osal_memcpy(W, AesKey, 16);
  
  if (round == 0) return;

  for (r=0; r < round; r++)
  {
    for(i=0; i < Nb; i++)
    {
      t = ((i+3) & 0x03) << 2;
      for (j=0; j < 4; j++)
      {
        temp[j] = W[t+j];  // temp = W[i-1]
      }

      if (i == 0)
      {
        //  temp = RotByte(temp)
        t= temp[0];
        temp[0] = temp[1];
        temp[1] = temp[2];
        temp[2] = temp[3];
        temp[3] = t;

        //  temp = SubByte(temp)
        for (j=0; j < 4; j++)
        {
          temp[j] = Sbox[temp[j]];
        }
        //  Rcon
        temp[0] ^= RCon[r];
      }

      t = i << 2;
      for (j=0; j < 4; j++)
      {
        W[t+j] ^= temp[j];    // W[i] = W[i-Nk] ^ temp
      }
    }
  }
}
#endif  // USE_KEY_EXPANSION

/******************************************************************************
 * @fn      InvSubBytes_Sw
 *
 * @brief   Performs the InvSubBytes_Sw function on the cipher state.
 *
 * input parameters
 *
 * @param   Cstate  - The current cipher state
 *
 * output parameters
 *
 * @param   Cstate[]    - Updated cipher state
 *
 * @return  None
 *
 */
void InvSubBytes_Sw( uint8_t *Cstate )
{
  uint8_t i;

  for (i=0; i < 4*Nb; i++)
  {
    Cstate[i] = IBOX(Cstate[i]);
  }
}

/******************************************************************************
 * @fn      InvShiftRows_Sw
 *
 * @brief   Performs the InvShiftRows_Sw function on the cipher state.
 *
 * input parameters
 *
 * @param   Cstate  - The current cipher state
 *
 * output parameters
 *
 * @param   Cstate[]    - Updated cipher state
 *
 * @return  None
 *
 */
void InvShiftRows_Sw( uint8_t *Cstate )
{
  uint8_t temp;

  // Row 0 is not shifted

  // Row 1 is shifted up by 1
  temp = Cstate[1];
  Cstate[1] = Cstate[13];
  Cstate[13] = Cstate[9];
  Cstate[9] = Cstate[5];
  Cstate[5] = temp;

  // Row 2 is shifted up by 2
  temp = Cstate[2];
  Cstate[2] = Cstate[10];
  Cstate[10] = temp;
  temp = Cstate[6];
  Cstate[6] = Cstate[14];
  Cstate[14] = temp;

  // Row 3 is shifted up by 3
  temp = Cstate[3];
  Cstate[3] = Cstate[7];
  Cstate[7] = Cstate[11];
  Cstate[11] = Cstate[15];
  Cstate[15] = temp;
}

/******************************************************************************
 * @fn      InvMixColumns_Sw
 *
 * @brief   Performs the InvMixColumns_Sw function on the cipher state.
 *
 * input parameters
 *
 * @param   Cstate  - The current cipher state
 *
 * output parameters
 *
 * @param   Cstate[]    - Updated cipher state
 *
 * @return  None
 *
 */
void InvMixColumns_Sw( uint8_t *Cstate )
{
  uint8_t r, c, t[4];

  for (c=0; c < 4*Nb; c+=4)
  {
    for (r=0; r < 4; r++) t[r] = Cstate[c+r];
    for (r=0; r < 4; r++)
    {
      Cstate[c+r] = MAP_FFMult_Sw( 0x0e, t[r] )
                  ^ MAP_FFMult_Sw( 0x0b, t[(r+1) & 0x03] )
                  ^ MAP_FFMult_Sw( 0x0d, t[(r+2) & 0x03] )
                  ^ MAP_FFMult_Sw( 0x09, t[(r+3) & 0x03] );
    }
  }
}

#endif  // INCLUDE_AES_DECRYPT
