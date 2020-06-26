/******************************************************************************

 @file  main.c

 @brief Main entry of the MAC-CoProcessor application

 Group: WCS LPC
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2016-2018, Texas Instruments Incorporated
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
 Includes
 *****************************************************************************/
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/Error.h>

#include "icall.h"
#include "board.h"
#include "board_led.h"
#if defined(CC13XX_LAUNCHXL) || defined(CC13X2R1_LAUNCHXL)
#include "board_gpio.h"
#endif

#include <inc/hw_ccfg.h>
#include <inc/hw_ccfg_simple_struct.h>

/* Required for the idle task function */
#include <ti/drivers/power/PowerCC26XX.h>

/* Required to enable instruction fetch cache */
#include <vims.h>

#ifdef NV_RESTORE
#include "macconfig.h"
#ifdef ONE_PAGE_NV
#include "nvocop.h"
#else
#include "nvoctp.h"
#endif
#endif

#include <string.h>

#include "api_mac.h"
#include "mt_sys.h"
#include "mcp.h"

#ifndef USE_DEFAULT_USER_CFG
#include "_hal_types.h"
#include "macs.h"
/* MAC user defined configuration */
macUserCfg_t macUser0Cfg[] = MAC_USER_CFG;
#endif /* USE_DEFAULT_USER_CFG */

#if defined(CC13XX_LAUNCHXL) && !defined(FREQ_2_4G)
#include "board_palna.h"
#endif

/******************************************************************************
 Constants
 *****************************************************************************/
/*! Extended Address offset in FCFG (LSB..MSB) */
#define EXTADDR_OFFSET 0x2F0

/*! Memory location of cutomer-configured IEEE address */
#define CCFG_IEEE ((uint8_t *)&(__ccfg.CCFG_IEEE_MAC_0))

/*! Memory location of unique factory-programmed IEEE address */
#define PRIM_IEEE ((uint8_t *)(FCFG1_BASE + EXTADDR_OFFSET))

/*! Size of stack for MNP application */
#define APP_TASK_STACK_SIZE  800

/******************************************************************************
 External Variables
 *****************************************************************************/
extern ApiMac_sAddrExt_t ApiMac_extAddr;

/******************************************************************************
 Global Variables
 *****************************************************************************/
Task_Struct myTask;
Char myTaskStack[APP_TASK_STACK_SIZE];

#ifdef NV_RESTORE
mac_Config_t Main_user1Cfg = {0};
#endif

/******************************************************************************
 Local Variables
 *****************************************************************************/
/*! Used to check for a valid extended address */
static const uint8_t dummyExtAddr[] =
{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

/******************************************************************************
 Local Function Prototypes
 *****************************************************************************/
#ifdef NV_RESTORE
#ifdef ONE_PAGE_NV
/* NVOCOP load API pointers */
static void NVOCOP_loadApiPtrs(NVINTF_nvFuncts_t *pfn)
{
    // Load caller's structure with pointers to the NV API functions
    pfn->initNV      = &NVOCOP_initNV;
    pfn->compactNV   = &NVOCOP_compactNV;
    pfn->createItem  = NULL;
    pfn->deleteItem  = NULL;
    pfn->readItem    = &NVOCOP_readItem;
    pfn->writeItem   = &NVOCOP_writeItem;
    pfn->writeItemEx = NULL;
    pfn->getItemLen  = NULL;
}
#endif
#endif
static void taskFxn(UArg a0, UArg a1);

/*! USER assert handler */
static void userAssertHandler(uint8_t reason);

/*!
 * @brief       MAC HAL assert handler.
 */
static void macHalAssertHandler(void)
{
}

void halAssertHandler(void)
{
}

/******************************************************************************
 Public Functions
 *****************************************************************************/
#if defined(CC13X2R1_LAUNCHXL) && !defined(NO_CC1312R1_SUPPORT)
extern void CC13X2R1_LAUNCHXL_Pin_init(void);
#endif
 
/*!
 * @brief  "main()" function - starting point
 */
void main()
{
    uint8_t i = 0;
    Task_Params taskParams;

#ifndef USE_DEFAULT_USER_CFG
    /* Register our MAC assert handler */
    do
    {
      macUser0Cfg[i].pAssertFP = macHalAssertHandler;
    } while (!macUser0Cfg[i++].last);
#endif

#ifndef FREQ_2_4G
    /* Register our MAC assert handler */
#if defined (CC13XX_LAUNCHXL)
    i = 0;
    do
    {
      macUser0Cfg[i].pSetRE = Board_Palna_initialize;
    } while (!macUser0Cfg[i++].last);
#endif
#endif

    /*
     Initialization for board related stuff, such as LEDs
     following TI-RTOS convention
     */
#if defined(CC13X2R1_LAUNCHXL) && !defined(NO_CC1312R1_SUPPORT)
    CC13X2R1_LAUNCHXL_Pin_init();
#else
    PIN_init(BoardGpioInitTable);
#endif

#if defined(POWER_MEAS)
    /* Disable external flash for power measurements */
    Board_shutDownExtFlash();
#endif

    /* Configure task */
    Task_Params_init(&taskParams);
    taskParams.stack = myTaskStack;
    taskParams.stackSize = APP_TASK_STACK_SIZE;
    taskParams.priority = 1;
    Task_construct(&myTask, taskFxn, &taskParams, NULL);

    BIOS_start(); /* enable interrupts and start SYS/BIOS */
}

/*!
 * @brief  TIRTOS HWI Handler. The name of this function is set to
 *         M3Hwi.excHandlerFunc in app.cfg, you can disable this by
 *         setting it to NULL.
 *
 * @param  excStack - TIROS variable
 * @param  lr - TIROS variable
 */
xdc_Void Main_excHandler(UInt *excStack, UInt lr)
{
    /* Intentionally not used */
    (void)excStack;
    (void)lr;

    /* Go to user-defined handler */
    userAssertHandler(MTSYS_ASSERT_RTOS);
}

/*!
 * @brief  HAL assert handler required by OSAL memory module.
 */
void Main_halAssertHandler(void)
{
    /* Go to user-defined handler */
    userAssertHandler(MTSYS_ASSERT_HAL);
}

/******************************************************************************
 Local Functions
 *****************************************************************************/
/*!
 * @brief  Function to initialize and run main applicatiion task
 *
 * @param  a0 - TIROS variable
 * @param  a1 - TIROS variable
 */
static void taskFxn(UArg a0, UArg a1)
{
    /* Intentionally not used */
    (void)a0;
    (void)a1;
#ifdef TIMAC_AGAMA_FPGA
    /* FPGA build disables POWER constraints */
    Power_setConstraint(PowerCC26XX_IDLE_PD_DISALLOW);
    Power_setConstraint(PowerCC26XX_SB_DISALLOW);

    IOCPortConfigureSet(IOID_20, IOC_PORT_RFC_GPO0, IOC_STD_OUTPUT);
    IOCPortConfigureSet(IOID_18, IOC_PORT_RFC_GPI0, IOC_STD_INPUT);
    // configure RF Core SMI Command Link
    IOCPortConfigureSet(IOID_22, IOC_IOCFG0_PORT_ID_RFC_SMI_CL_OUT, IOC_STD_OUTPUT);
    IOCPortConfigureSet(IOID_21, IOC_IOCFG0_PORT_ID_RFC_SMI_CL_IN, IOC_STD_INPUT);

#endif

    /* Initialize ICall module */
    ICall_init();

    /*
     * Copy the extended address from the CCFG area in flash memory
     * Assumption: CCFG_IEEE_MAC_0 and CCFG_IEEE_MAC_1 are contiguous
     * 4-byte locations with data stored first.
     */
    memcpy(ApiMac_extAddr, CCFG_IEEE, APIMAC_SADDR_EXT_LEN);

    /* Check to see if the CCFG IEEE is valid */
    if(memcmp(ApiMac_extAddr, dummyExtAddr, APIMAC_SADDR_EXT_LEN) == 0)
    {
        /* Nothing at CCFG IEEE location, get the Primary IEEE */
        memcpy(ApiMac_extAddr, PRIM_IEEE, APIMAC_SADDR_EXT_LEN);
    }

#ifdef NV_RESTORE
    /* Setup the NV driver */
#ifdef ONE_PAGE_NV
    NVOCOP_loadApiPtrs(&Main_user1Cfg.nvFps);
#else
    NVOCTP_loadApiPtrs(&Main_user1Cfg.nvFps);
#endif

    if (Main_user1Cfg.nvFps.initNV)
    {
        Main_user1Cfg.nvFps.initNV(NULL);
    }
#endif

    /* Start tasks of external images */
    ICall_createRemoteTasks();

    /* Kick off co-processor application task */
    MCP_task();
}

/*!
 * @brief  User-defined assert handler
 *
 * @param  Reason for the assertion
 */
static void userAssertHandler(uint8_t reason)
{
    while(1)
    {
        /* Record assert reason, then reset device */
        MtSys_resetReq(reason);
    }
}
