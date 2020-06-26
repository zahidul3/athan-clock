/******************************************************************************
*  Filename:       rf_patch_mce_ieee_802_15_4.h
*  Revised:        2017-08-22 20:30:41 +0200 (Tue, 22 Aug 2017)
*  Revision:       17885
*
*  Description: RF core patch for IEEE 802.15.4-2006 support in CC13x2 and CC26x2
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
#ifndef _RF_PATCH_MCE_IEEE_802_15_4_H
#define _RF_PATCH_MCE_IEEE_802_15_4_H

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

MCE_PATCH_TYPE patchIeee_802_15_4Mce[367] = { 
   0xf7036064,
   0x70399b3a,
   0x039bb3af,
   0x39b33af7,
   0x9b3aaf70,
   0xb3aff703,
   0x3af77039,
   0xaf70039b,
   0x08fcb9b3,
   0x8fc664c5,
   0xfc644c50,
   0xc64cc508,
   0x64c5508f,
   0x4c5008fc,
   0xc5088fc6,
   0x508ffc64,
   0x2fc0c64c,
   0x079c0fcf,
   0x7f7f01e0,
   0x0000dead,
   0x00000003,
   0x0000001f,
   0x80000000,
   0x0004000c,
   0x00011400,
   0x00000009,
   0x00008000,
   0x06700000,
   0x16aa002b,
   0x00000a11,
   0x00000b60,
   0x00404010,
   0x00060000,
   0x001e1e1e,
   0x00000000,
   0x00000003,
   0x0000001f,
   0x80000000,
   0x0004000c,
   0x000114c4,
   0x00000009,
   0x00008000,
   0x0f900000,
   0x121d0000,
   0x00000a11,
   0x00000b60,
   0x00404010,
   0x00060000,
   0x00202020,
   0x00000000,
   0x720d7223,
   0x720f720e,
   0x72637210,
   0x7203a35d,
   0x73057204,
   0x73917306,
   0xc7c07291,
   0xb0d09010,
   0xa0d07100,
   0x721bb110,
   0x10208162,
   0x06703952,
   0x16300020,
   0x14011101,
   0x60a16c01,
   0x60ca60b2,
   0x61456119,
   0x60a160a1,
   0x60a160a1,
   0x60ca60ad,
   0x61456119,
   0x60a160a1,
   0x60a160a1,
   0x60ca60b2,
   0x61456119,
   0x60a160a1,
   0x60a160a1,
   0x60ca60ad,
   0x61456119,
   0x60a160a1,
   0x60a460a1,
   0x60a51220,
   0x73111210,
   0x73137312,
   0x001081b1,
   0xb07091b0,
   0xc4616073,
   0xc460c2b2,
   0x60b61820,
   0xc2b2c281,
   0x1820c460,
   0x6e236f13,
   0x16121611,
   0xc63268b6,
   0x1820c9c0,
   0x120340c3,
   0x16126e23,
   0x7a4068c0,
   0xc1409c80,
   0x12209c90,
   0x60a49990,
   0xc1307291,
   0xb11391f0,
   0x81621213,
   0xd0303982,
   0x66d79a12,
   0x40e41e02,
   0x40e41e12,
   0x40dd1e22,
   0x00238992,
   0xb00560e4,
   0x1e208990,
   0x60e444e2,
   0xa005b270,
   0xb0e8b128,
   0xb1287100,
   0x9233a0e8,
   0x1e008c80,
   0x993040f2,
   0xb111b910,
   0x7100b0d1,
   0xb002b012,
   0xb013b633,
   0xb111b003,
   0x7291a0d1,
   0x7100b0d3,
   0xa0d3b113,
   0xc0301000,
   0xc0209910,
   0xb0d19930,
   0xb1117100,
   0x7291a0d1,
   0x39828162,
   0x490d1a22,
   0x726366d0,
   0xa002a003,
   0x73057263,
   0x73917306,
   0xa2307291,
   0x9010c7c0,
   0x781060a4,
   0x95b094c0,
   0x94d07820,
   0x819095c0,
   0x0410c0f1,
   0x3110c012,
   0x6f201402,
   0x959094a0,
   0x6f201612,
   0x95a094b0,
   0x22308240,
   0xb0d54534,
   0xb1157100,
   0x612ca0d5,
   0xc0307291,
   0xb00291e0,
   0xb006b004,
   0x22808160,
   0x7a20413f,
   0x7a106140,
   0x90309050,
   0x90607a30,
   0xb2359040,
   0x31808460,
   0x31818461,
   0x00103981,
   0x81959450,
   0x120e06f5,
   0x12061217,
   0x944012f0,
   0x9440c190,
   0xb139b011,
   0xa0fcb0f9,
   0x1203b089,
   0x81621204,
   0xd0403982,
   0x66d79a12,
   0x42261e22,
   0x421c1e32,
   0x72917100,
   0xb139a0d1,
   0xb13ca0f9,
   0xa444b0fc,
   0x3140c600,
   0xb9109930,
   0xb0d1b111,
   0xa0d17100,
   0x80907291,
   0x46d42210,
   0x65bdb13c,
   0x41931e76,
   0x39408190,
   0x1c0f06f0,
   0x8c904545,
   0x4d451c90,
   0x12021205,
   0xc3001276,
   0xb2309070,
   0x66d7c050,
   0x9440c0c0,
   0x94507a50,
   0x1647619a,
   0x818091cf,
   0x419a1e00,
   0x499f1c70,
   0x80907100,
   0x44762200,
   0xa235617a,
   0x72037291,
   0x73057204,
   0xa0027306,
   0xa006a004,
   0x73057263,
   0x73917306,
   0xc7c07291,
   0x81609010,
   0x1a303980,
   0x80004dba,
   0x45b82250,
   0x61bb1220,
   0x61bb1240,
   0x999089c0,
   0x120960a4,
   0xc0cc120a,
   0xba3e120d,
   0x1c898ad8,
   0x1c8a49c8,
   0x61d54dce,
   0x12001089,
   0x100a1880,
   0x61d510db,
   0x1200108a,
   0x10091880,
   0x168b10db,
   0x161d61d5,
   0x41da1e8d,
   0x61c1908c,
   0x1e8210bf,
   0x149541e5,
   0x1e821612,
   0x312545e5,
   0x000581b0,
   0x1e8b91b5,
   0x1a8b49e8,
   0x18b0c070,
   0x11011630,
   0x6c011401,
   0x908c908c,
   0x908c908c,
   0x908c908c,
   0x908c908c,
   0x1000b082,
   0x8ad3ba3e,
   0xb083b083,
   0xba3e1000,
   0x1e8f8ad4,
   0x1ca34a06,
   0x1ca44a0b,
   0x70004a0d,
   0x4e0b1c93,
   0x4e0d1c94,
   0x1a1e7000,
   0x161e620f,
   0xc040620f,
   0x4a161ce0,
   0x164010e0,
   0x70004a19,
   0xb085c00e,
   0xc00e7000,
   0x7000b084,
   0xb3a5a3a7,
   0xcbe066ab,
   0x66ab6a20,
   0x6a23c7f0,
   0xa3a5b3a7,
   0x16408460,
   0x84613180,
   0x31811641,
   0x00103981,
   0x78109450,
   0x95a094b0,
   0x94a07820,
   0xa0f99590,
   0xb005b0d1,
   0x7291b258,
   0x9910c030,
   0xb133b130,
   0x9440c290,
   0x223080b0,
   0x2200465d,
   0xc040465d,
   0xa9109930,
   0xa0f3b910,
   0xb111a0f0,
   0xb0647100,
   0x9930c2f0,
   0xb133b130,
   0x7100b111,
   0x223080b0,
   0x2200465b,
   0x66ab465b,
   0x66ab623e,
   0x8248625d,
   0x9930c040,
   0xb910a910,
   0xd0603988,
   0x66d79a18,
   0x7100b111,
   0x9930c2f0,
   0xb130b064,
   0xb111b133,
   0x80b07100,
   0x46752200,
   0x46752230,
   0x8241627c,
   0xd0703981,
   0x66d79a11,
   0x4a851c18,
   0xc04066ab,
   0xa9109930,
   0xc080b910,
   0x710066d7,
   0xc190b064,
   0xb0f99440,
   0xa0f3b139,
   0x8190a0f0,
   0xc01206f0,
   0x14023110,
   0x94a06f20,
   0x16129590,
   0x94b06f20,
   0xc5f095a0,
   0x84609930,
   0x84613180,
   0x39813181,
   0x94500010,
   0xb910a910,
   0xb133b111,
   0x80b07100,
   0x45672290,
   0x46a02230,
   0x81606226,
   0x1a303980,
   0x80004eb7,
   0x46b42250,
   0x62b6b005,
   0xa005b270,
   0x82327000,
   0x46bf2212,
   0x2a222612,
   0x99c29232,
   0x2a1262c3,
   0x92322622,
   0x969399c2,
   0xc03096a4,
   0x8a439a30,
   0x06628a54,
   0x06608240,
   0x46ca1c02,
   0x7000b053,
   0x0a608990,
   0x70009990,
   0x66d7c090,
   0x9a006152,
   0x220089f0,
   0xb9e046d8,
   0x00007000
};

PATCH_FUN_SPEC void rf_patch_mce_ieee_802_15_4(void)
{
#ifdef __PATCH_NO_UNROLLING
   uint32_t i;
   for (i = 0; i < 367; i++) {
      HWREG(RFC_MCERAM_BASE + 4 * i) = patchIeee_802_15_4Mce[i];
   }
#else
   const uint32_t *pS = patchIeee_802_15_4Mce;
   volatile unsigned long *pD = &HWREG(RFC_MCERAM_BASE);
   uint32_t t1, t2, t3, t4, t5, t6, t7, t8;
   uint32_t nIterations = 45;

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
   t7 = *pS++;
   *pD++ = t1;
   *pD++ = t2;
   *pD++ = t3;
   *pD++ = t4;
   *pD++ = t5;
   *pD++ = t6;
   *pD++ = t7;
#endif
}

#endif
