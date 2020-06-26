/******************************************************************************
*  Filename:       rf_patch_cpe_bt5.h
*  Revised:        $Date: 2017-08-24 11:43:33 +0200 (Thu, 24 Aug 2017) $
*  Revision:       $Revision: 17889 $
*
*  Description: RF core patch for Bluetooth 5 support ("BLE" and "BLE5" API command sets) in CC13x2 and CC26x2
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
#ifndef _RF_PATCH_CPE_BT5_H
#define _RF_PATCH_CPE_BT5_H

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


CPE_PATCH_TYPE patchImageBt5[] = {
   0x210041db,
   0x2100422b,
   0x21004341,
   0x21004355,
   0x2100436f,
   0x21004055,
   0x461db570,
   0x47204c00,
   0x00022281,
   0x4801b510,
   0x00004700,
   0x000009df,
   0x49044803,
   0x05c068c0,
   0x47880fc0,
   0x47084902,
   0x21000340,
   0x000087d1,
   0x000053cd,
   0xb430488f,
   0x31404601,
   0x2a027c0a,
   0x6802d116,
   0x754a79d2,
   0x68936802,
   0x32804602,
   0x7d486093,
   0xd00b2802,
   0xd0092800,
   0x2c061ec4,
   0x4985d809,
   0x18400080,
   0x6b803840,
   0x60901818,
   0xbc304882,
   0x20034700,
   0x80c802c0,
   0x72082002,
   0x2003bc30,
   0xb5704770,
   0x4604487a,
   0x7c213440,
   0x29007da5,
   0x2d01d10c,
   0x3060d00a,
   0x08807840,
   0xd00207c0,
   0x07007ce0,
   0x2001d502,
   0xe00075a0,
   0x73a02000,
   0x47804872,
   0xbd7075a5,
   0x4c6db570,
   0x46254870,
   0x47803550,
   0xd1092802,
   0x88e13440,
   0x42914a6d,
   0x7ce9d104,
   0xd4010689,
   0x80e11d91,
   0xb570bd70,
   0x4a694963,
   0x30504608,
   0x2b007803,
   0x7983d010,
   0xd10d2b01,
   0x095b7c03,
   0xd00907db,
   0x4d637d09,
   0x7f6c74c1,
   0x43202020,
   0x47907768,
   0xbd70776c,
   0x74c121ff,
   0xbd704790,
   0x485db510,
   0x47804c54,
   0xd10e2802,
   0x31404621,
   0x4b5688ca,
   0xd108429a,
   0x09527cca,
   0xd00407d2,
   0x06927d22,
   0x1d9ad401,
   0xbd1080ca,
   0x484ab570,
   0x30404952,
   0x09407cc0,
   0xd00707c0,
   0x48504d4d,
   0x43208b2c,
   0x47888328,
   0xbd70832c,
   0xbd704788,
   0x4d41b570,
   0x3540494b,
   0x00a0792c,
   0x6b401840,
   0x493d4780,
   0xd00a2c0a,
   0x780a3154,
   0x07db0993,
   0x73aad004,
   0x2303780a,
   0x700a431a,
   0x3153bd70,
   0x4935e7f3,
   0x71083140,
   0xd01a2825,
   0x280adc08,
   0x280bd011,
   0x2818d011,
   0x281ed011,
   0xe014d106,
   0xd010282a,
   0xd006283c,
   0xd010283d,
   0x00804935,
   0x6b401840,
   0x48344770,
   0x48344770,
   0x48344770,
   0x48344770,
   0x48344770,
   0x48344770,
   0x48344770,
   0xb5f84770,
   0x4c204607,
   0x5d00204e,
   0x07ee0985,
   0x2e0025fb,
   0x7d26d017,
   0x0f240734,
   0xd0032c05,
   0x42202401,
   0xe012d002,
   0xe7fa2402,
   0xd00509c0,
   0xd5030670,
   0x0f806848,
   0xd0082801,
   0x005b085b,
   0x00520852,
   0x2800e003,
   0x402bd001,
   0x2b06402a,
   0x2010d003,
   0xd0102b02,
   0x2302e010,
   0x4638402a,
   0xfed8f7ff,
   0xda072800,
   0x1ab900c2,
   0x7e493920,
   0x42112214,
   0x2000d100,
   0x4302bdf8,
   0x46384303,
   0xfec8f7ff,
   0x0000bdf8,
   0x21000160,
   0x21000108,
   0x000245a5,
   0x00022cfd,
   0x00023b49,
   0x00001404,
   0x000238fd,
   0x210000a8,
   0x000224fb,
   0x00021d4f,
   0x00002020,
   0x00024fc0,
   0x210041a9,
   0x21004185,
   0x21004159,
   0x2100411f,
   0x210040f9,
   0x210040c3,
   0x21004071,
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
   0xfe80f7ff,
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
};
#define _NWORD_PATCHIMAGE_BT5 218

#define _NWORD_PATCHSYS_BT5 0

#define _IRQ_PATCH_0 0x210042fd


#ifndef _BT5_SYSRAM_START
#define _BT5_SYSRAM_START 0x20000000
#endif

#ifndef _BT5_CPERAM_START
#define _BT5_CPERAM_START 0x21000000
#endif

#define _BT5_SYS_PATCH_FIXED_ADDR 0x20000000

#define _BT5_PARSER_PATCH_TAB_OFFSET 0x0390
#define _BT5_PATCH_TAB_OFFSET 0x0398
#define _BT5_IRQPATCH_OFFSET 0x0434
#define _BT5_PATCH_VEC_OFFSET 0x4024

#ifndef _BT5_NO_PROG_STATE_VAR
static uint8_t bBt5PatchEntered = 0;
#endif

PATCH_FUN_SPEC void enterBt5CpePatch(void)
{
   uint32_t *pPatchVec = (uint32_t *) (_BT5_CPERAM_START + _BT5_PATCH_VEC_OFFSET);

#if (_NWORD_PATCHIMAGE_BT5 > 0)
   memcpy(pPatchVec, patchImageBt5, sizeof(patchImageBt5));
#endif
}

PATCH_FUN_SPEC void enterBt5SysPatch(void)
{
}

PATCH_FUN_SPEC void configureBt5Patch(void)
{
   uint8_t *pPatchTab = (uint8_t *) (_BT5_CPERAM_START + _BT5_PATCH_TAB_OFFSET);
   uint32_t *pIrqPatch = (uint32_t *) (_BT5_CPERAM_START + _BT5_IRQPATCH_OFFSET);


   pPatchTab[1] = 0;
   pPatchTab[18] = 1;
   pPatchTab[28] = 2;
   pPatchTab[105] = 3;
   pPatchTab[106] = 4;
   pPatchTab[70] = 5;

   pIrqPatch[21] = _IRQ_PATCH_0;
}

PATCH_FUN_SPEC void applyBt5Patch(void)
{
#ifdef _BT5_NO_PROG_STATE_VAR
   enterBt5SysPatch();
   enterBt5CpePatch();
#else
   if (!bBt5PatchEntered)
   {
      enterBt5SysPatch();
      enterBt5CpePatch();
      bBt5PatchEntered = 1;
   }
#endif
   configureBt5Patch();
}

PATCH_FUN_SPEC void refreshBt5Patch(void)
{
   configureBt5Patch();
}

#ifndef _BT5_NO_PROG_STATE_VAR
PATCH_FUN_SPEC void cleanBt5Patch(void)
{
   bBt5PatchEntered = 0;
}
#endif

PATCH_FUN_SPEC void rf_patch_cpe_bt5(void)
{
   applyBt5Patch();
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

#endif //  _RF_PATCH_CPE_BT5_H

