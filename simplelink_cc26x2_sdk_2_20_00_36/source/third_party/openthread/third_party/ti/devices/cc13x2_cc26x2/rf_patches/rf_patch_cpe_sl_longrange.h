/******************************************************************************
*  Filename:       rf_patch_cpe_sl_longrange.h
*  Revised:        $Date: 2017-08-24 11:43:33 +0200 (Thu, 24 Aug 2017) $
*  Revision:       $Revision: 17889 $
*
*  Description: RF core patch for SimpleLink Long Range support ("PROP" API command set) in CC13x2 and CC26x2
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
#ifndef _RF_PATCH_CPE_SL_LONGRANGE_H
#define _RF_PATCH_CPE_SL_LONGRANGE_H

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

#include <stdint.h>
#include <string.h>

#ifndef CPE_PATCH_TYPE
#define CPE_PATCH_TYPE static const uint32_t
#endif

#ifndef SYS_PATCH_TYPE
#define SYS_PATCH_TYPE static const uint32_t
#endif

#ifndef PATCH_FUN_SPEC
#define PATCH_FUN_SPEC static inline
#endif

#ifndef _APPLY_PATCH_TAB
#define _APPLY_PATCH_TAB
#endif


CPE_PATCH_TYPE patchImageSlLongrange[] = {
   0x210040f5,
   0x2100404d,
   0x21004109,
   0x21004123,
   0x21004075,
   0x21004091,
   0x210040a1,
   0x4801b510,
   0x00004700,
   0x000009df,
   0x460db5f8,
   0x4b054616,
   0x290c6819,
   0x4b04d104,
   0x78192408,
   0x70194321,
   0x47084902,
   0x40045054,
   0x21000340,
   0x0000699d,
   0x49044803,
   0x05c068c0,
   0x47880fc0,
   0x47084902,
   0x21000340,
   0x000087d1,
   0x000053cd,
   0xf0002000,
   0x4604f855,
   0x47004800,
   0x0000545b,
   0xf0002004,
   0x4605f84d,
   0x47004800,
   0x0000533b,
   0x480cb570,
   0x4c0c6ac0,
   0x0f800700,
   0x28012502,
   0x2803d008,
   0x6960d105,
   0x616043a8,
   0x15204907,
   0xbd706008,
   0x20014906,
   0x69604788,
   0x61604328,
   0x0000bd70,
   0x40046000,
   0x40041100,
   0xe000e180,
   0x00007d05,
   0x4c03b510,
   0xffa2f7ff,
   0x28006820,
   0xbd10d1fa,
   0x40041100,
   0x780a490b,
   0xd1042aff,
   0x7d924a0a,
   0x0f120712,
   0x4908700a,
   0x75883140,
   0x49054770,
   0x29ff7809,
   0x0900d005,
   0x43080100,
   0x31404902,
   0x47707588,
   0x210002a5,
   0x40086200,
   0x4c19b570,
   0x7ba14606,
   0xf820f000,
   0x7be14605,
   0xf0004630,
   0x4915f81b,
   0x78094604,
   0x070a2028,
   0x2d01d401,
   0x2038d100,
   0xd40106c9,
   0xd1012c01,
   0x43082140,
   0x4788490e,
   0xd0012dff,
   0x6145480d,
   0xd0012cff,
   0x61c4480c,
   0xbd704808,
   0xd0082900,
   0xd00629ff,
   0x070840c1,
   0x281c0ec0,
   0x2001d100,
   0x20ff4770,
   0x00004770,
   0x210000a8,
   0x21000340,
   0x000040e5,
   0x40045040,
   0x40046000,
};
#define _NWORD_PATCHIMAGE_SL_LONGRANGE 102

#define _NWORD_PATCHSYS_SL_LONGRANGE 0

#define _IRQ_PATCH_0 0x210040b1


#ifndef _SL_LONGRANGE_SYSRAM_START
#define _SL_LONGRANGE_SYSRAM_START 0x20000000
#endif

#ifndef _SL_LONGRANGE_CPERAM_START
#define _SL_LONGRANGE_CPERAM_START 0x21000000
#endif

#define _SL_LONGRANGE_SYS_PATCH_FIXED_ADDR 0x20000000

#define _SL_LONGRANGE_PARSER_PATCH_TAB_OFFSET 0x0390
#define _SL_LONGRANGE_PATCH_TAB_OFFSET 0x0398
#define _SL_LONGRANGE_IRQPATCH_OFFSET 0x0434
#define _SL_LONGRANGE_PATCH_VEC_OFFSET 0x4024

#ifndef _SL_LONGRANGE_NO_PROG_STATE_VAR
static uint8_t bSlLongrangePatchEntered = 0;
#endif

PATCH_FUN_SPEC void enterSlLongrangeCpePatch(void)
{
   uint32_t *pPatchVec = (uint32_t *) (_SL_LONGRANGE_CPERAM_START + _SL_LONGRANGE_PATCH_VEC_OFFSET);

#if (_NWORD_PATCHIMAGE_SL_LONGRANGE > 0)
   memcpy(pPatchVec, patchImageSlLongrange, sizeof(patchImageSlLongrange));
#endif
}

PATCH_FUN_SPEC void enterSlLongrangeSysPatch(void)
{
}

PATCH_FUN_SPEC void configureSlLongrangePatch(void)
{
   uint8_t *pPatchTab = (uint8_t *) (_SL_LONGRANGE_CPERAM_START + _SL_LONGRANGE_PATCH_TAB_OFFSET);
   uint32_t *pIrqPatch = (uint32_t *) (_SL_LONGRANGE_CPERAM_START + _SL_LONGRANGE_IRQPATCH_OFFSET);


   pPatchTab[28] = 0;
   pPatchTab[104] = 1;
   pPatchTab[105] = 2;
   pPatchTab[106] = 3;
   pPatchTab[70] = 4;
   pPatchTab[71] = 5;
   pPatchTab[69] = 6;

   pIrqPatch[21] = _IRQ_PATCH_0;
}

PATCH_FUN_SPEC void applySlLongrangePatch(void)
{
#ifdef _SL_LONGRANGE_NO_PROG_STATE_VAR
   enterSlLongrangeSysPatch();
   enterSlLongrangeCpePatch();
#else
   if (!bSlLongrangePatchEntered)
   {
      enterSlLongrangeSysPatch();
      enterSlLongrangeCpePatch();
      bSlLongrangePatchEntered = 1;
   }
#endif
   configureSlLongrangePatch();
}

PATCH_FUN_SPEC void refreshSlLongrangePatch(void)
{
   configureSlLongrangePatch();
}

#ifndef _SL_LONGRANGE_NO_PROG_STATE_VAR
PATCH_FUN_SPEC void cleanSlLongrangePatch(void)
{
   bSlLongrangePatchEntered = 0;
}
#endif

PATCH_FUN_SPEC void rf_patch_cpe_sl_longrange(void)
{
   applySlLongrangePatch();
}

#undef _IRQ_PATCH_0

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  _RF_PATCH_CPE_SL_LONGRANGE_H

