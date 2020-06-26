/******************************************************************************

 @file main.c

 @brief main entry of the example application

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

#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#include <ioc.h>

#include "sys_ctrl.h"

#include "board.h"
#include "board_led.h"
#if defined(CC13XX_LAUNCHXL) || defined(CC13X2R1_LAUNCHXL)
#include "board_gpio.h"
#endif
#include "timac_board.h"
#include <inc/hw_ccfg.h>
#include <inc/hw_ccfg_simple_struct.h>

/* Header files required for the temporary idle task function */
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <aon_rtc.h>
#include <prcm.h>

#if defined(FEATURE_BLE_OAD) || defined(FEATURE_NATIVE_OAD)
#include <ti/drivers/SPI.h>
#endif

#ifdef FEATURE_UBLE
#include "bleAdv.h"
#endif /* FEATURE_UBLE */


#if defined(FEATURE_BLE_OAD)
    #include <oad_switch.h>
#endif

#include "ti/sysbios/family/arm/m3/Hwi.h"


#include "cpu.h"

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
#include "ssf.h"
#include "uart_printf.h"

#include "sensor.h"
#if defined(ASSERT_LEDS)
#include "board_led.h"
#endif

#ifndef USE_DEFAULT_USER_CFG

#include "mac_user_config.h"

/* MAC user defined configuration */
macUserCfg_t macUser0Cfg[] = MAC_USER_CFG;

#endif /* USE_DEFAULT_USER_CFG */

#if ((CONFIG_RANGE_EXT_MODE == APIMAC_HIGH_GAIN_MODE) && \
    defined(CC13XX_LAUNCHXL) && !defined(FREQ_2_4G))
#include "board_palna.h"
#endif

/******************************************************************************
 Constants
 *****************************************************************************/

/* Assert Reasons */
#define MAIN_ASSERT_ICALL        2
#define MAIN_ASSERT_MAC          3
#define MAIN_ASSERT_HWI_TIRTOS   4

#define MAX_ASSERT_TOGGLE_COUNT  500000

#define RFC_MODE_BLE                 PRCM_RFCMODESEL_CURR_MODE1
#define RFC_MODE_IEEE                PRCM_RFCMODESEL_CURR_MODE2
#define RFC_MODE_ANT                 PRCM_RFCMODESEL_CURR_MODE4
#define RFC_MODE_EVERYTHING_BUT_ANT  PRCM_RFCMODESEL_CURR_MODE5
#define RFC_MODE_EVERYTHING          PRCM_RFCMODESEL_CURR_MODE6

/* Extended Address offset in FCFG (LSB..MSB) */
#define EXTADDR_OFFSET 0x2F0
#if defined(CC13X2R1_LAUNCHXL) || (CC26X2R1_LAUNCHXL)
#define APP_TASK_STACK_SIZE 1536
#else
#define APP_TASK_STACK_SIZE 900
#endif

#define SET_RFC_MODE(mode) HWREG( PRCM_BASE + PRCM_O_RFCMODESEL ) = (mode)

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
mac_Config_t Main_user1Cfg = { 0 };
#endif

#if defined(BOARD_DISPLAY_USE_UART)
UART_Params uartParams;
#endif

/******************************************************************************
 Local Variables
 *****************************************************************************/
/* Used to check for a valid extended address */
static const uint8_t dummyExtAddr[] =
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

#ifdef NV_RESTORE
#ifdef ONE_PAGE_NV
/* NVOCOP load API pointers */
static void NVOCOP_loadApiPtrs(NVINTF_nvFuncts_t *pfn)
{
    // Load caller's structure with pointers to the NV API functions
    pfn->initNV      = &NVOCOP_initNV;
    pfn->compactNV   = &NVOCOP_compactNV;
    pfn->createItem  = NULL;
    pfn->deleteItem  = &NVOCOP_deleteItem;
    pfn->readItem    = &NVOCOP_readItem;
    pfn->writeItem   = &NVOCOP_writeItem;
    pfn->writeItemEx = NULL;
    pfn->getItemLen  = NULL;
}
#endif
#endif

/*!
 * @brief       Fill in your own assert function.
 *
 * @param       assertReason - reason: MAIN_ASSERT_HWI_TIRTOS,
 *                                     MAIN_ASSERT_ICALL, or
 *                                     MAIN_ASSERT_MAC
 */
void Main_assertHandler(uint8 assertReason)
{
#if defined(ASSERT_LEDS)
    int toggleCount = 0;
    bool toggle = true;

    Hwi_disable();

    while(1)
    {
        if(toggleCount == 0)
        {
            if(toggle == false)
            {
                Board_Led_control(board_led_type_LED1, board_led_state_OFF);
                Board_Led_control(board_led_type_LED2, board_led_state_OFF);
#if !defined(CC13XX_LAUNCHXL) && !defined(CC26XX_LAUNCHXL) && !defined(CC13X2R1_LAUNCHXL) && !defined(CC26X2R1_LAUNCHXL)
                Board_Led_control(board_led_type_LED3, board_led_state_OFF);
                Board_Led_control(board_led_type_LED4, board_led_state_OFF);
#endif
            }
            else if(toggle == true)
            {
                Board_Led_control(board_led_type_LED1, board_led_state_ON);
                Board_Led_control(board_led_type_LED2, board_led_state_ON);
#if !defined(CC13XX_LAUNCHXL) && !defined(CC26XX_LAUNCHXL) && !defined(CC13X2R1_LAUNCHXL) && !defined(CC26X2R1_LAUNCHXL)
                Board_Led_control(board_led_type_LED3, board_led_state_ON);
                Board_Led_control(board_led_type_LED4, board_led_state_ON);
#endif
            }
        }

        toggleCount++;
        if(toggleCount >= MAX_ASSERT_TOGGLE_COUNT)
        {
            toggleCount = 0;
            if(toggle == true)
            {
                toggle = false;
            }
            else
            {
                toggle = true;
            }
        }
    }
#else  /* ASSERT_LEDS */
    Ssf_assertInd(assertReason);

    /* Pull the plug and start over */
    SysCtrlSystemReset();
#endif /* !ASSERT_LEDS */
}


/*!
 * @brief       Main task function
 *
 * @param       a0 -
 * @param       a1 -
 */
Void taskFxn(UArg a0, UArg a1)
{
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
     * Copy the extended address from the CCFG area
     * Assumption: the memory in CCFG_IEEE_MAC_0 and CCFG_IEEE_MAC_1
     * is contiguous and LSB first.
     */
    memcpy(ApiMac_extAddr, (uint8_t *)&(__ccfg.CCFG_IEEE_MAC_0),
           (APIMAC_SADDR_EXT_LEN));

    /* Check to see if the CCFG IEEE is valid */
    if(memcmp(ApiMac_extAddr, dummyExtAddr, APIMAC_SADDR_EXT_LEN) == 0)
    {
        /* No, it isn't valid.  Get the Primary IEEE Address */
        memcpy(ApiMac_extAddr, (uint8_t *)(FCFG1_BASE + EXTADDR_OFFSET),
               (APIMAC_SADDR_EXT_LEN));
    }


#ifdef NV_RESTORE
    /* Setup the NV driver */
#ifdef ONE_PAGE_NV
    NVOCOP_loadApiPtrs(&Main_user1Cfg.nvFps);
#else
    NVOCTP_loadApiPtrs(&Main_user1Cfg.nvFps);
#endif

    if(Main_user1Cfg.nvFps.initNV)
    {
        Main_user1Cfg.nvFps.initNV( NULL);
    }
#endif

    /* Start tasks of external images */
    ICall_createRemoteTasks();

    /* Initialize the application */
    Sensor_init();

#ifdef FEATURE_BLE_OAD
    uint8_t blinkCnt = 8;

    /* blink to let user know sensor is initializing */
    while(blinkCnt)
    {
        Board_Led_toggle(board_led_type_LED1);
        Task_sleep(10000);
        blinkCnt--;
    }

    /* reset led back to known state */
    Board_Led_control(board_led_type_LED1, board_led_state_OFF);
#endif /* FEATURE_BLE_OAD */

    /* Kick off application - Forever loop */
    while(1)
    {
        Sensor_process();
    }
}

/*!
 * @brief       TIRTOS HWI Handler.  The name of this function is set to
 *              M3Hwi.excHandlerFunc in app.cfg, you can disable this by
 *              setting it to null.
 *
 * @param       excStack - TIROS variable
 * @param       lr - TIROS variable
 */
xdc_Void Main_excHandler(UInt *excStack, UInt lr)
{
    /* User defined function */
    Main_assertHandler(MAIN_ASSERT_HWI_TIRTOS);
}

/*!
 * @brief       HAL assert handler required by OSAL memory module.
 */
void halAssertHandler(void)
{
    /* User defined function */
    Main_assertHandler(MAIN_ASSERT_ICALL);
}

/*!
 * @brief       MAC HAL assert handler.
 */
void macHalAssertHandler(void)
{
    /* User defined function */
    Main_assertHandler(MAIN_ASSERT_MAC);
}

#if defined(CC13X2R1_LAUNCHXL) && !defined(NO_CC1312R1_SUPPORT)
extern void CC13X2R1_LAUNCHXL_Pin_init(void);
#endif

/*!
 * @brief       "main()" function - starting point
 */
Void main()
{
    Task_Params taskParams;

#ifndef USE_DEFAULT_USER_CFG
    macUser0Cfg[0].pAssertFP = macHalAssertHandler;
#endif

#if ((CONFIG_RANGE_EXT_MODE == APIMAC_HIGH_GAIN_MODE) && \
     defined(CC13XX_LAUNCHXL) && !defined(FREQ_2_4G))
    macUser0Cfg[0].pSetRE = Board_Palna_initialize;
#endif

    /*
     Initialization for board related stuff such as LEDs
     following TI-RTOS convention
     */
#if defined(CC13X2R1_LAUNCHXL) && !defined(NO_CC1312R1_SUPPORT)
    CC13X2R1_LAUNCHXL_Pin_init();
#else
    PIN_init(BoardGpioInitTable);
#endif

#ifdef FEATURE_BLE_OAD
    /* If FEATURE_BLE_OAD is enabled, look for a left button
     *  press on reset. This indicates to revert to some
     *  factory image
     */
    uint32_t btnPin = Board_PIN_BUTTON0;

#if defined(CC13X2R1_LAUNCHXL) && !defined(NO_CC1312R1_SUPPORT)
    if(ChipInfo_GetChipType() == CHIP_TYPE_CC1312)
    {
        btnPin = Board_PIN_BUTTON0_CC1312R1;
    }
#endif

    if(!PIN_getInputValue(btnPin))
    {
        OAD_markSwitch();
    }
#endif /* FEATURE_BLE_OAD */

#if defined(POWER_MEAS)
    /* Disable external flash for power measurements */
    Board_shutDownExtFlash();
#endif

#if defined(FEATURE_BLE_OAD) || defined(FEATURE_NATIVE_OAD)
    SPI_init();
#endif

#ifndef POWER_MEAS
#if defined(BOARD_DISPLAY_USE_UART)
    /* Enable System_printf(..) UART output */
    UART_init();
    UART_Params_init(&uartParams);
#ifndef TIMAC_AGAMA_FPGA
    uartParams.baudRate = 115200;
#else
    uartParams.baudRate = 460800;
#endif
    UartPrintf_init(UART_open(Board_UART0, &uartParams));
#endif /* BOARD_DISPLAY_USE_UART */
#endif

    /* Configure task. */
    Task_Params_init(&taskParams);
    taskParams.stack = myTaskStack;
    taskParams.stackSize = APP_TASK_STACK_SIZE;
    taskParams.priority = 1;
    Task_construct(&myTask, taskFxn, &taskParams, NULL);

#ifdef DEBUG_SW_TRACE
    IOCPortConfigureSet(IOID_8, IOC_PORT_RFC_TRC, IOC_STD_OUTPUT
                    | IOC_CURRENT_4MA | IOC_SLEW_ENABLE);
#endif /* DEBUG_SW_TRACE */

    BIOS_start(); /* enable interrupts and start SYS/BIOS */
}

