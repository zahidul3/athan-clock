/******************************************************************************
*  Filename:       rf_patch_mce_slr.h
*  Revised:        $Date: 2017-08-24 11:43:33 +0200 (Thu, 24 Aug 2017) $
*  Revision:       $Revision: 17889 $
*
*  Description: RF core patch for SimpleLink Long Range support in CC13x2 and CC26x2
*
*  Copyright (c) 2015-2017, Texas Instruments Incorporated
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

#ifndef _RF_PATCH_MCE_SLR_H
#define _RF_PATCH_MCE_SLR_H

#include <stdint.h>
#include "../inc/hw_types.h"

#ifndef MCE_PATCH_TYPE
#define MCE_PATCH_TYPE static const uint32_t
#endif

#ifndef PATCH_FUN_SPEC
#define PATCH_FUN_SPEC static inline
#endif

#ifndef RFC_MCERAM_BASE
#define RFC_MCERAM_BASE 0x21008000
#endif

#ifndef MCE_PATCH_MODE
#define MCE_PATCH_MODE 0
#endif

MCE_PATCH_TYPE patchSlrMce[254] = { 
   0x146f6044,
   0x0f9d2fcf,
   0x7f7fdb3e,
   0x333c3c33,
   0x3cc3cccc,
   0x00010000,
   0x00030002,
   0x00030000,
   0x000f000c,
   0x00c300cc,
   0x0033003c,
   0xcc33cccc,
   0x333333cc,
   0x0f0f0f00,
   0x00030000,
   0x001f0007,
   0x00000000,
   0x000f0400,
   0x03870000,
   0x48c00001,
   0x80000043,
   0x00800004,
   0x000006f0,
   0x0524091e,
   0x00050450,
   0x48200800,
   0x00000048,
   0x7f7f001f,
   0x9000004c,
   0x7f7f0000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x00000000,
   0x72637223,
   0x73057303,
   0x73047203,
   0x72047306,
   0x72917391,
   0x8001c7c0,
   0x90010001,
   0x08019010,
   0x720d9001,
   0x720f720e,
   0xb0d07210,
   0xb1107100,
   0x721ba0d0,
   0x10208162,
   0x06703952,
   0x16300020,
   0x14011101,
   0x606b6c01,
   0x608a6076,
   0x1210610a,
   0x73117223,
   0x73137312,
   0x81b17314,
   0x91b00010,
   0x6059b070,
   0xc2b2c1d1,
   0x1820c4e0,
   0x6e236f13,
   0x16121611,
   0xc632687a,
   0x1820c9c0,
   0x12034087,
   0x16126e23,
   0xd0006884,
   0x606b91a0,
   0x73127311,
   0x91f0c000,
   0x81a0c009,
   0x06703980,
   0x1e101610,
   0xc0a74499,
   0xc01ec008,
   0x1e2060a8,
   0xc0e7449f,
   0xc03ec018,
   0x1e4060a8,
   0xc12744a5,
   0xc07ec038,
   0xc16760a8,
   0xc0fec078,
   0x65f5c030,
   0x7100b0e8,
   0xa0e8b128,
   0xb630b230,
   0xb013b003,
   0xb012b002,
   0x39468176,
   0x161606f6,
   0xc004b0e0,
   0xc070c12c,
   0x781192c0,
   0x1012c070,
   0x14c20632,
   0x96436f23,
   0xb1207100,
   0x68bf3921,
   0x1c641614,
   0x781144bd,
   0xc0700bf1,
   0x06321012,
   0x6f2314c2,
   0x71009643,
   0x3921b120,
   0xc4f468ce,
   0xc066c6d5,
   0x64f381df,
   0x22108210,
   0x60da40e0,
   0x1a10c070,
   0x64f3c00f,
   0xa0e168e2,
   0x9910c030,
   0xb1119930,
   0x7100b0d1,
   0xa0d1b111,
   0xa0037291,
   0x7223a002,
   0x061f606b,
   0x00f9316f,
   0x04411091,
   0x898a9971,
   0x1091061a,
   0x99710451,
   0x061b898b,
   0x14ba311b,
   0x147a3919,
   0x96436fa3,
   0x710092ce,
   0x7000b120,
   0x8cb4b118,
   0x9334410e,
   0x9440c1f0,
   0x94507850,
   0x81a7721e,
   0x39871076,
   0x99770677,
   0x16188988,
   0x9a17d040,
   0x786065f5,
   0x78709590,
   0x788095a0,
   0x789095b0,
   0x655695c0,
   0x99c07840,
   0xb0076569,
   0xb124b017,
   0xc205b0e4,
   0x65ddc004,
   0x1c5465ee,
   0x91c04934,
   0x81811614,
   0x412f1e01,
   0x1c411671,
   0xa0e74d2f,
   0x79bf79ae,
   0x97af979e,
   0x65ee1204,
   0x1c541614,
   0x91c04147,
   0xa2356141,
   0x65f5c050,
   0xa0e4a0e7,
   0x72027206,
   0x72037204,
   0x73057204,
   0x73917306,
   0x606b7291,
   0x3181c061,
   0xcff3c002,
   0x16116e12,
   0x16116e12,
   0x6e13c7e0,
   0x6e121611,
   0x695f1611,
   0x6e12cff0,
   0x69651611,
   0xb0067000,
   0xb004b016,
   0xb002b014,
   0x7820b012,
   0x90509030,
   0x90407830,
   0xb2359060,
   0xc090b072,
   0xb13793f0,
   0xb136b0f7,
   0x7100b0f6,
   0xb127b073,
   0xa0f7a0f7,
   0x227080b0,
   0xa0f6459e,
   0xb0e7a0f7,
   0x7100ba3e,
   0xb127b136,
   0xb061b041,
   0x93f0c0f0,
   0x8b14ba3f,
   0x97043114,
   0x7100a044,
   0xb04db127,
   0xb074b06d,
   0x7100b231,
   0x7000b127,
   0xba39b0e7,
   0x31848af4,
   0x98d43d84,
   0x98e0c050,
   0x69a7c050,
   0xc20188f0,
   0x3d601410,
   0x31648334,
   0x18043d64,
   0x3520cff0,
   0x93340404,
   0x93f0c0b0,
   0xd060b069,
   0x65f59a14,
   0xb1277100,
   0x6179a0e7,
   0x120a1209,
   0x140965d3,
   0x1e17141a,
   0x65d341d0,
   0x140a1419,
   0x41d01e37,
   0x140965d3,
   0x65d3141a,
   0x140a1419,
   0x3c8a3c89,
   0x71007000,
   0x7100b127,
   0x89a0b127,
   0x39803180,
   0x398189b1,
   0x65be7000,
   0x10ac109b,
   0x10bd65be,
   0x14db318d,
   0x318d10cd,
   0x318a14dc,
   0x149b14a9,
   0x979b149c,
   0x700097ac,
   0xb079a0e7,
   0xb1247100,
   0x89d0b0e7,
   0x9a007000,
   0x220089f0,
   0xb9e045f6,
   0x00007000
};

PATCH_FUN_SPEC void rf_patch_mce_slr(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 254; i++) {
      HWREG(RFC_MCERAM_BASE + 4 * i) = patchSlrMce[i];
   }
#else
   const uint32_t *pS = patchSlrMce;
   volatile unsigned long *pD = &HWREG(RFC_MCERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 31;

   do {
      t1 = *pS++;
      t2 = *pS++;
      t3 = *pS++;
      t4 = *pS++;
      t5 = *pS++;
      t6 = *pS++;
      t7 = *pS++;
      t8 = *pS++;
      *pD++ = t1;
      *pD++ = t2;
      *pD++ = t3;
      *pD++ = t4;
      *pD++ = t5;
      *pD++ = t6;
      *pD++ = t7;
      *pD++ = t8;
   } while (--nIterations);

   t1 = *pS++;
   t2 = *pS++;
   t3 = *pS++;
   t4 = *pS++;
   t5 = *pS++;
   t6 = *pS++;
   *pD++ = t1;
   *pD++ = t2;
   *pD++ = t3;
   *pD++ = t4;
   *pD++ = t5;
   *pD++ = t6;
#endif
}

#endif
