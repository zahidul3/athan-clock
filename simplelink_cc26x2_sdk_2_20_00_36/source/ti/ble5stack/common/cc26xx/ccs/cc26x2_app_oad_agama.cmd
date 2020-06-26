/******************************************************************************

 @file  cc26x2_app_oad_agama.cmd

 @brief CC26X2 or CC13X2 OAD specific linker configuration file for TI-RTOS with Code Composer
        Studio.

        Imported Symbols
        Note: Linker defines are located in the CCS IDE project by placing them
        in
        Properties->Build->Linker->Advanced Options->Command File Preprocessing.

        CACHE_AS_RAM:       Disable system cache to be used as GPRAM for
                            additional volatile memory storage.
        CCxxxxROM or
        CCxxxxFLASH:        Device Name (e.g. CC2650). In order to define this
                            symbol, the tool chain requires that it be set to
                            a specific value, but in fact, the actual value does
                            not matter as it is not used in the linker control
                            file. The only way this symbol is used is based on
                            whether it is defined or not, not its actual value.
                            There are other linker symbols that do specifically
                            set their value to 1 to indicate R1, and 2 to
                            indicate R2, and these values are checked and do make
                            a difference. However, it would appear confusing if
                            the device name's value did not correspond to the
                            value set in other linker symbols. In order to avoid
                            this confusion, when the symbol is defined, it should
                            be set to the value of the device's ROM that it
                            corresponds so as to look and feel consistent. Please
                            note that a device name symbol should always be
                            defined to avoid side effects from default values
                            that may not be correct for the device being used.
        ICALL_RAM0_START:   RAM start of BLE stack.
        ICALL_STACK0_START: Flash start of BLE stack.
        PAGE_AlIGN:         Align BLE stack boundary to a page boundary.
                            Aligns to Flash word boundary by default.
        ENCRYPTION_ROM:     Reserve ECC Engine RAM space (necessary for Secure
                            Connections when CC26X0ROM is not define).

 Group: CMCU, LPRF
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2013-2018, Texas Instruments Incorporated
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

/*******************************************************************************
 * CCS Linker configuration
 */

/* Retain interrupt vector table variable                                    */
--retain=g_pfnVectors
/* Override default entry point.                                             */
--entry_point ResetISR
/* Suppress warnings and errors:                                             */
/* - 10063: Warning about entry point not being _c_int00                     */
/* - 16011, 16012: 8-byte alignment errors. Observed when linking in object  */
/*   files compiled using Keil (ARM compiler)                                */
--diag_suppress=10063,16011,16012

/* The following command line options are set as part of the CCS project.    */
	/* If you are building using the command line, or for some reason want to    */
/* define them here, you can uncomment and modify these lines as needed.     */
/* If you are using CCS for building, it is probably better to make any such */
/* modifications in your CCS project and leave this file alone.              */
/*                                                                           */
/* --heap_size=0                                                             */
/* --stack_size=256                                                          */
/* --library=rtsv7M3_T_le_eabi.lib                                           */

/* The starting address of the application.  Normally the interrupt vectors  */
/* must be located at the beginning of the application. Flash is 128KB, with */
/* sector length of 4KB                                                      */
/*******************************************************************************
 * Memory Sizes
 */
#define FLASH_BASE   0x00000000
#define GPRAM_BASE   0x11000000
#define RAM_BASE     0x20000000
#define ROM_BASE     0x10000000

#if defined(CC26X0ROM) || defined(CC26X0FLASH)
#define FLASH_SIZE 0x00020000
#define GPRAM_SIZE 0x00002000
#define RAM_SIZE   0x00005000
#define ROM_SIZE   0x0001C000
#else if defined(CC26X2ROM) || defined(CC26X2FLASH) || defined(CC13X2FLASH) || defined(CC13X2ROM)
#define FLASH_SIZE 0x00058000
#define GPRAM_SIZE 0x00002000
#define RAM_SIZE   0x00014000
#define ROM_SIZE   0x00040000
#endif /* CC26X0ROM || CC26X0FLASH */

/*******************************************************************************
 * Memory Definitions
 ******************************************************************************/

/*******************************************************************************
 * RAM
 */
#ifdef CC26X0ROM
  #if CC26X0ROM == 2
    #define RESERVED_RAM_SIZE 0x00000C00
  #elif CC26X0ROM == 1
    #define RESERVED_RAM_SIZE 0x00000718
  #endif /* CC26X0ROM */
#elif ((defined(CC26X2ROM)) && (CC26X2ROM == 1)) || ((defined(CC13X2ROM)) && (CC13X2ROM == 1))
  #define RESERVED_RAM_SIZE 0x00000B08
#else /* Flash Only */
  #ifdef ENCRYPTION_ROM
      #if ENCRYPTION_ROM == 1  // R1
        #error "CCS: source build not support on CC2650R1"
      #elif ENCRYPTION_ROM == 2  // R2
        #define RESERVED_RAM_SIZE 0x80
      #else
        #define RESERVED_RAM_SIZE 0x00
      #endif
  #else
    #define RESERVED_RAM_SIZE 0x00
  #endif
#endif /* CC26X0ROM */

#define RAM_START             RAM_BASE

#ifdef ICALL_RAM0_START
  #define RAM_END             (ICALL_RAM0_START - 1)
#else
  #define RAM_END             (RAM_START + RAM_SIZE - RESERVED_RAM_SIZE - 1)
#endif /* ICALL_RAM0_START */

/*******************************************************************************
 * Flash
 */

#define WORD_SIZE                  4

#if defined(CC26X0ROM) || defined(CC26X0FLASH)
  #define PAGE_SIZE                0x1000
#else if defined(CC26X2ROM) || defined(CC26X2FLASH) || defined(CC13X2FLASH) || defined(CC13X2ROM)
  #define PAGE_SIZE                0x2000
#endif /* CC26X0ROM || CC26X0FLASH */

#ifdef PAGE_ALIGN
  #define FLASH_MEM_ALIGN          PAGE_SIZE
#else
  #define FLASH_MEM_ALIGN          WORD_SIZE
#endif /* PAGE_ALIGN */

#if defined(CC26X0ROM) || defined(CC26X0FLASH)
  #define PAGE_MASK                0xFFFFF000
#else if defined(CC26X2ROM) || defined(CC26X2FLASH) || defined(CC13X2FLASH) || defined(CC13X2ROM)
  #define PAGE_MASK                0xFFFFE000
#endif /* CC26X0ROM || CC26X0FLASH */

/* The last Flash page is reserved for the application. */
#define NUM_RESERVED_FLASH_PAGES   1
#define RESERVED_FLASH_SIZE        (NUM_RESERVED_FLASH_PAGES * PAGE_SIZE)


/* Check if page alingment with the Stack image is required.  If so, do not link
 * into a page shared by the Stack.
 */
#define  OAD_HDR_START             FLASH_BASE
#define  OAD_HDR_SIZE              0x50
#define  OAD_HDR_END               OAD_HDR_START + OAD_HDR_SIZE - 1

#define ENTRY_START                OAD_HDR_END + 1
#define ENTRY_SIZE                 0x40
#define ENTRY_END                  ENTRY_START + ENTRY_SIZE - 1

#define FLASH_START                ENTRY_END + 1

#ifdef ICALL_STACK0_START
  #ifdef PAGE_ALIGN
    #define ADJ_ICALL_STACK0_START (ICALL_STACK0_START * PAGE_MASK)
  #else
    #define ADJ_ICALL_STACK0_START ICALL_STACK0_START
  #endif /* PAGE_ALIGN */

  #define FLASH_END                (ADJ_ICALL_STACK0_START - 1)
#else
  #define FLASH_END                (FLASH_BASE + FLASH_SIZE - RESERVED_FLASH_SIZE - 1)
#endif /* ICALL_STACK0_START */

#define FLASH_LAST_PAGE_START      FLASH_END + 1

/*******************************************************************************
 * Stack
 */

/* Create global constant that points to top of stack */
/* CCS: Change stack size under Project Properties    */
__STACK_TOP = __stack + __STACK_SIZE;

/*******************************************************************************
 * GPRAM
 */

#ifdef CACHE_AS_RAM
  #define GPRAM_START GPRAM_BASE
  #define GPRAM_END   (GPRAM_START + GPRAM_SIZE - 1)
#endif /* CACHE_AS_RAM */

/*******************************************************************************
 * Main arguments
 */

/* Allow main() to take args */
/* --args 0x8 */

/*******************************************************************************
 * System Memory Map
 ******************************************************************************/
MEMORY
{
  /* EDITOR'S NOTE:
   * the FLASH and SRAM lengths can be changed by defining
   * ICALL_STACK0_START or ICALL_RAM0_START in
   * Properties->ARM Linker->Advanced Options->Command File Preprocessing.
   */

  /* Application stored in and executes from internal flash */
  FLASH (RX) : origin = FLASH_START, length = (FLASH_END - FLASH_START + 1)

  ENTRY (RX) : origin = ENTRY_START, length = ENTRY_SIZE
  /* CCFG Page, contains .ccfg code section and some application code. */
  FLASH_IMG_HDR (RX) : origin = OAD_HDR_START, length = OAD_HDR_SIZE

  /* Application uses internal RAM for data */
  SRAM (RWX) : origin = RAM_START, length = (RAM_END - RAM_START + 1)

  #ifdef CACHE_AS_RAM
      GPRAM(RWX) : origin = GPRAM_START, length = GPRAM_SIZE
  #endif /* CACHE_AS_RAM */
}

/*******************************************************************************
 * Section Allocation in Memory
 ******************************************************************************/
SECTIONS
{
  GROUP > ENTRY
  {
    .resetVecs
    .intvecs
    EntrySection  LOAD_START(prgEntryAddr)
  }

 // .intvecs        :   >  FLASH_START
  GROUP >>  FLASH
  {
    .text
    .const
    .constdata
    .rodata
    .emb_text
    .pinit
  }

  .cinit        : > FLASH LOAD_END(flashEndAddr)

  GROUP > FLASH_IMG_HDR
  {
    .image_header //align PAGE_SIZE
  }
  //.ccfg           :   >  FLASH_LAST_PAGE (HIGH)

  GROUP > SRAM
  {
    .data LOAD_START(ramStartHere)
    #ifndef CACHE_AS_RAM
    .bss
    #endif /* CACHE_AS_RAM */
    .vtable
    .vtable_ram
    vtable_ram
    .sysmem
    .nonretenvar
    /*This keeps ll.o objects out of GPRAM, if no ll.o would be placed here
      the warning #10068 is supressed.*/
    #ifdef CACHE_AS_RAM
    ll_bss
    {
      --library=*ll_*.a<ll.o> (.bss)
      --library=*ll_*.a<ll_ae.o> (.bss)
    }
    #endif /* CACHE_AS_RAM */
  } LOAD_END(heapStart)

  .stack            :   >  SRAM (HIGH) LOAD_START(heapEnd)

  #ifdef CACHE_AS_RAM

  .bss :
  {
    *(.bss)
  } > GPRAM
  #endif /* CACHE_AS_RAM */
}
