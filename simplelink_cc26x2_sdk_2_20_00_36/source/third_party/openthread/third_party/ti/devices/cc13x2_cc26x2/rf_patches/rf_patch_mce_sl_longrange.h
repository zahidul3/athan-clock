/******************************************************************************
*  Filename:       rf_patch_mce_sl_longrange.h
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

#ifndef _RF_PATCH_MCE_SL_LONGRANGE_H
#define _RF_PATCH_MCE_SL_LONGRANGE_H

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

MCE_PATCH_TYPE patchSimplelinkLrmMce[226] = { 
   0x146f6049,
   0x0f9d2fcf,
   0x7f7fdb3e,
   0x00010000,
   0x00030002,
   0x00030000,
   0x000f000c,
   0x00c300cc,
   0x0033003c,
   0xcc33cccc,
   0x333333cc,
   0x0f0f0f00,
   0x0060005f,
   0x005e005d,
   0x005c005b,
   0x005a0059,
   0x00070003,
   0x0000001f,
   0x04000000,
   0x0000000f,
   0x00010387,
   0x004340c0,
   0x00048000,
   0x06f00100,
   0x091e0000,
   0x00540524,
   0x08000005,
   0x00484820,
   0x001f0000,
   0x004c7f7f,
   0x00008004,
   0x00002e00,
   0x00000000,
   0x00000000,
   0x00001020,
   0x00000000,
   0x72230000,
   0x73037263,
   0x72037305,
   0x73067304,
   0x73917204,
   0xc7c07291,
   0x00018001,
   0x90109001,
   0x90010801,
   0x720e720d,
   0x7210720f,
   0x7100b0d0,
   0xa0d0b110,
   0x8162721b,
   0x39521020,
   0x00200670,
   0x11011630,
   0x6c011401,
   0x60826081,
   0x60986098,
   0x60816081,
   0x60816081,
   0x60771220,
   0x72231210,
   0x73127311,
   0x73147313,
   0x001081b1,
   0xb07091b0,
   0x6076605e,
   0xc2b2c201,
   0x1820c4f0,
   0x6e236f13,
   0x16121611,
   0xc6326886,
   0x1820c9c0,
   0x12034093,
   0x16126e23,
   0xd0006890,
   0xc0c091a0,
   0x60769990,
   0x8cb4b118,
   0x933440a0,
   0xb008b009,
   0xb018b019,
   0x78506597,
   0x721e9450,
   0x107681a7,
   0x06773987,
   0x89889977,
   0xd0301618,
   0x65be9a17,
   0x784064df,
   0x64f299c0,
   0xb017b007,
   0xb0e4b124,
   0xc004c205,
   0x6590657f,
   0x48bd1c54,
   0x161491c0,
   0x1e018181,
   0x167140b8,
   0x4cb81c41,
   0x796ea0e7,
   0x979e797f,
   0x120497af,
   0x16146590,
   0x40d01c54,
   0x60ca91c0,
   0xc040a235,
   0xa0e765be,
   0x7206a0e4,
   0x72047202,
   0x72047203,
   0x73067305,
   0x72917391,
   0xc0616076,
   0xc0023181,
   0x6e12cff3,
   0x6e121611,
   0xc7e01611,
   0x16116e13,
   0x16116e12,
   0xcff068e8,
   0x16116e12,
   0x700068ee,
   0xb016b006,
   0xb014b004,
   0xb012b002,
   0x90307820,
   0x78309050,
   0x90609040,
   0xb072b235,
   0x93f0c090,
   0x3150c600,
   0xc0609930,
   0xb1379910,
   0xb14cb0f7,
   0xb111b10c,
   0x7100b0d1,
   0x227080b0,
   0xb073452a,
   0xa0f7a10c,
   0xa0d1b111,
   0xb0e7b127,
   0x7100ba3e,
   0xb127b14c,
   0xb061b041,
   0xa009a044,
   0x7100a008,
   0xb04db127,
   0xb074b06d,
   0x7100b231,
   0x7000b127,
   0x22108090,
   0xb1114136,
   0x8cb0a910,
   0xc0909330,
   0xb00993f0,
   0x6107b008,
   0xa0f7b0e7,
   0xa008a009,
   0xc0b0ba39,
   0x8af493f0,
   0x3d843184,
   0x899098d4,
   0xc05098e0,
   0x88f06944,
   0x1410c401,
   0x83343d70,
   0x3d643164,
   0xcff01804,
   0x04043520,
   0x80c19334,
   0x451122c1,
   0x7100c060,
   0x22c180c1,
   0xb1274511,
   0xb0696955,
   0xb127b910,
   0x6107a0e7,
   0x120a1209,
   0x14096575,
   0x1e17141a,
   0x65754172,
   0x140a1419,
   0x41721e37,
   0x14096575,
   0x6575141a,
   0x140a1419,
   0x3c8a3c89,
   0x71007000,
   0x7100b127,
   0x89a0b127,
   0x39803180,
   0x398189b1,
   0x65607000,
   0x10ac109b,
   0x10bd6560,
   0x14db318d,
   0x318d10cd,
   0x318a14dc,
   0x149b14a9,
   0x979b149c,
   0x700097ac,
   0xb079a0e7,
   0xb1247100,
   0x89d0b0e7,
   0xc0507000,
   0xc18b65be,
   0x84a1c0ec,
   0x1012c070,
   0x14c20632,
   0x22006f23,
   0x103441a5,
   0x318361aa,
   0x6fb70034,
   0x161b6e74,
   0x699d3921,
   0xc07084b1,
   0x06321012,
   0x6f2314c2,
   0x41b62200,
   0x61bb1034,
   0x00343183,
   0x6e746fb7,
   0x3921161b,
   0x700069ae,
   0x89f09a00,
   0x45bf2200,
   0x7000b9e0
};

PATCH_FUN_SPEC void rf_patch_mce_sl_longrange(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 226; i++) {
      HWREG(RFC_MCERAM_BASE + 4 * i) = patchSimplelinkLrmMce[i];
   }
#else
   const uint32_t *pS = patchSimplelinkLrmMce;
   volatile unsigned long *pD = &HWREG(RFC_MCERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 28;

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
   *pD++ = t1;
   *pD++ = t2;
#endif
}

#endif
