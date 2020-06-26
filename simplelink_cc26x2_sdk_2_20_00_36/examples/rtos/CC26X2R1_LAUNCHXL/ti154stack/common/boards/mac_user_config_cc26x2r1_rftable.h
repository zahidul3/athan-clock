/******************************************************************************

 @file mac_user_config_cc26x2r1_2_4g.h

 @brief override and power table

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

#ifndef _CONFIG_CC26X2R1_RF_TABLE_H
#define _CONFIG_CC26X2R1_RF_TABLE_H

/******************************************************************************
 Includes
 *****************************************************************************/

/******************************************************************************
 Constants and definitions
 *****************************************************************************/

/******************************************************************************
 External Variables
 *****************************************************************************/

/******************************************************************************
 Global Variables
 *****************************************************************************/
// Overrides for CMD_RADIO_SETUP
// 2.4GHz

static uint32_t pOverrides_ieee[] =
{
#if !defined (TIMAC_AGAMA_FPGA)
    // override_ieee_802_15_4.xml
    // PHY: Use MCE RAM patch, RFE ROM bank 1
    MCE_RFE_OVERRIDE(1,0,0,0,1,0),
    // Synth: Use 48 MHz crystal, enable extra PLL filtering
    (uint32_t)0x02400403,
    // Synth: Configure extra PLL filtering
    (uint32_t)0x001C8473,
    // Synth: Configure synth hardware
    (uint32_t)0x00088433,
    // Synth: Set minimum RTRIM to 3
    (uint32_t)0x00038793,
    // Synth: Configure faster calibration
    HW32_ARRAY_OVERRIDE(0x4004,1),
    // Synth: Configure faster calibration
    (uint32_t)0x1C0C0618,
    // Synth: Configure faster calibration
    (uint32_t)0xC00401A1,
    // Synth: Configure faster calibration
    (uint32_t)0x00010101,
    // Synth: Configure faster calibration
    (uint32_t)0xC0040141,
    // Synth: Configure faster calibration
    (uint32_t)0x00214AD3,
    // Synth: Decrease synth programming time-out (0x0298 RAT ticks = 166 us)
    (uint32_t)0x02980243,
    // DC/DC regulator: In Tx, use DCDCCTL5[3:0]=0xC (DITHER_EN=1 and IPEAK=4). In Rx, use DCDCCTL5[3:0]=0xC (DITHER_EN=1 and IPEAK=4).
    (uint32_t)0xFCFC08C3,
    // Rx: Set LNA bias current offset to +15 to saturate trim to max (default: 0)
    (uint32_t)0x000F8883,
    // override_frontend_id.xml
#else
    0x000151D0, // Enable demodulator wired input
    0x00041110, // Output digital baseband signal on RF_GPO0
    0x00000083, // Disable digital transmitter
    0x02CF02A3, // Adjust sync found timing to match FPGA
    0x00800403, // Disable synth programming
#endif
    (uint32_t)0xFFFFFFFF,
};

const macTxPwrVal_t txPowerTable_ieee[] =
{
    {-21, RF_TxPowerTable_DEFAULT_PA_ENTRY(7, 3, 0, 3) },
    {-18, RF_TxPowerTable_DEFAULT_PA_ENTRY(9, 3, 0, 3) },
    {-15, RF_TxPowerTable_DEFAULT_PA_ENTRY(12, 2, 0, 100) },
    {-12, RF_TxPowerTable_DEFAULT_PA_ENTRY(40, 2, 0, 8) },
    {-10, RF_TxPowerTable_DEFAULT_PA_ENTRY(12, 2, 0, 11) },
    {-9, RF_TxPowerTable_DEFAULT_PA_ENTRY(13, 2, 0, 5) },
    {-6, RF_TxPowerTable_DEFAULT_PA_ENTRY(13, 1, 0, 16) },
    {-5, RF_TxPowerTable_DEFAULT_PA_ENTRY(14, 1, 0, 17) },
    {-3, RF_TxPowerTable_DEFAULT_PA_ENTRY(17, 1, 0, 20) },
    {0, RF_TxPowerTable_DEFAULT_PA_ENTRY(25, 1, 0, 26) },
    {1, RF_TxPowerTable_DEFAULT_PA_ENTRY(28, 1, 0, 28) },
    {2, RF_TxPowerTable_DEFAULT_PA_ENTRY(13, 0, 0, 34) },
    {3, RF_TxPowerTable_DEFAULT_PA_ENTRY(17, 0, 0, 42) },
    {4, RF_TxPowerTable_DEFAULT_PA_ENTRY(22, 0, 0, 54) },
    {5, RF_TxPowerTable_DEFAULT_PA_ENTRY(30, 0, 0, 74) },
    RF_TxPowerTable_TERMINATION_ENTRY
};

#endif /* _CONFIG_CC26X2R1_RF_TABLE_H */
