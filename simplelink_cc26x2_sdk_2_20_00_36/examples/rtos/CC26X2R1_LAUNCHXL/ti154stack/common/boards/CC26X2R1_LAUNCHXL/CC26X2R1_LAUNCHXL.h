/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file       CC26X2R1_LAUNCHXL.h
 *
 *  @brief      CC26X2R1_LAUNCHXL Board Specific header file.
 *
 *  The CC26X2R1_LAUNCHXL header file should be included in an application as
 *  follows:
 *  @code
 *  #include "CC26X2R1_LAUNCHXL.h"
 *  @endcode
 *
 *  ============================================================================
 */
#ifndef __CC26X2R1_LAUNCHXL_BOARD_H__
#define __CC26X2R1_LAUNCHXL_BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <ti/drivers/PIN.h>
#ifdef TIMAC_AGAMA_FPGA
#include <ti/devices/cc13x2_cc26x2_v2/driverlib/ioc.h>
#else
#include <ti/devices/cc13x2_cc26x2_v1/driverlib/ioc.h>
#endif

/* Externs */
extern const PIN_Config BoardGpioInitTable[];

/* Defines */

/* Mapping of pins to board signals using general board aliases
 *      <board signal alias>                <pin mapping>
 */

/* Mapping of pins to board signals using general board aliases
 *      <board signal alias>                  <pin mapping>
 */

/* Analog Capable DIOs */
#define CC26X2R1_LAUNCHXL_DIO23_ANALOG          IOID_23
#define CC26X2R1_LAUNCHXL_DIO24_ANALOG          IOID_24
#define CC26X2R1_LAUNCHXL_DIO25_ANALOG          IOID_25
#define CC26X2R1_LAUNCHXL_DIO26_ANALOG          IOID_26
#define CC26X2R1_LAUNCHXL_DIO27_ANALOG          IOID_27
#define CC26X2R1_LAUNCHXL_DIO28_ANALOG          IOID_28
#define CC26X2R1_LAUNCHXL_DIO29_ANALOG          IOID_29
#define CC26X2R1_LAUNCHXL_DIO30_ANALOG          IOID_30

/* Digital IOs */
#define CC26X2R1_LAUNCHXL_DIO0                  IOID_0
#define CC26X2R1_LAUNCHXL_DIO1_RFSW             IOID_1
#define CC26X2R1_LAUNCHXL_DIO12                 IOID_12
#define CC26X2R1_LAUNCHXL_DIO15                 IOID_15
#define CC26X2R1_LAUNCHXL_DIO16_TDO             IOID_16
#define CC26X2R1_LAUNCHXL_DIO17_TDI             IOID_17
#define CC26X2R1_LAUNCHXL_DIO21                 IOID_21
#define CC26X2R1_LAUNCHXL_DIO22                 IOID_22

/* Discrete Inputs */
#define CC26X2R1_LAUNCHXL_PIN_BTN1              IOID_13
#define CC26X2R1_LAUNCHXL_PIN_BTN2              IOID_14

/* GPIO */
#define CC26X2R1_LAUNCHXL_GPIO_LED_ON           1
#define CC26X2R1_LAUNCHXL_GPIO_LED_OFF          0

/* I2C */
#define CC26X2R1_LAUNCHXL_I2C0_SCL0             IOID_4
#define CC26X2R1_LAUNCHXL_I2C0_SDA0             IOID_5

/* LCD (430BOOST - Sharp96 Rev 1.1) */
#define CC26X2R1_LAUNCHXL_LCD_CS                IOID_24 /* SPI chip select */
#define CC26X2R1_LAUNCHXL_LCD_EXTCOMIN          IOID_12 /* External COM inversion */
#define CC26X2R1_LAUNCHXL_LCD_ENABLE            IOID_22 /* LCD enable */
#define CC26X2R1_LAUNCHXL_LCD_POWER             IOID_23 /* LCD power control */
#define CC26X2R1_LAUNCHXL_LCD_CS_ON             1
#define CC26X2R1_LAUNCHXL_LCD_CS_OFF            0

/* LEDs */
#define CC26X2R1_LAUNCHXL_PIN_LED_ON            1
#define CC26X2R1_LAUNCHXL_PIN_LED_OFF           0
#define CC26X2R1_LAUNCHXL_PIN_RLED              IOID_6
#define CC26X2R1_LAUNCHXL_PIN_GLED              IOID_7

/* PWM Outputs */
#define CC26X2R1_LAUNCHXL_PWMPIN0               CC26X2R1_LAUNCHXL_PIN_RLED
#define CC26X2R1_LAUNCHXL_PWMPIN1               CC26X2R1_LAUNCHXL_PIN_GLED
#define CC26X2R1_LAUNCHXL_PWMPIN2               PIN_UNASSIGNED
#define CC26X2R1_LAUNCHXL_PWMPIN3               PIN_UNASSIGNED
#define CC26X2R1_LAUNCHXL_PWMPIN4               PIN_UNASSIGNED
#define CC26X2R1_LAUNCHXL_PWMPIN5               PIN_UNASSIGNED
#define CC26X2R1_LAUNCHXL_PWMPIN6               PIN_UNASSIGNED
#define CC26X2R1_LAUNCHXL_PWMPIN7               PIN_UNASSIGNED

/* SPI */
#define CC26X2R1_LAUNCHXL_SPI_FLASH_CS          IOID_20
#define CC26X2R1_LAUNCHXL_FLASH_CS_ON           0
#define CC26X2R1_LAUNCHXL_FLASH_CS_OFF          1

/* SPI Board */
#define CC26X2R1_LAUNCHXL_SPI0_MISO             IOID_8          /* RF1.20 */
#define CC26X2R1_LAUNCHXL_SPI0_MOSI             IOID_9          /* RF1.18 */
#define CC26X2R1_LAUNCHXL_SPI0_CLK              IOID_10         /* RF1.16 */
#define CC26X2R1_LAUNCHXL_SPI0_CSN              PIN_UNASSIGNED
#define CC26X2R1_LAUNCHXL_SPI1_MISO             PIN_UNASSIGNED
#define CC26X2R1_LAUNCHXL_SPI1_MOSI             PIN_UNASSIGNED
#define CC26X2R1_LAUNCHXL_SPI1_CLK              PIN_UNASSIGNED
#define CC26X2R1_LAUNCHXL_SPI1_CSN              PIN_UNASSIGNED

/* UART Board */
#define CC26X2R1_LAUNCHXL_UART0_RX              IOID_2          /* RXD */
#define CC26X2R1_LAUNCHXL_UART0_TX              IOID_3          /* TXD */
#ifndef TIMAC_AGAMA_FPGA
#define CC26X2R1_LAUNCHXL_UART0_CTS             IOID_19         /* CTS */
#define CC26X2R1_LAUNCHXL_UART0_RTS             IOID_18         /* RTS */
#else
#define CC26X2R1_LAUNCHXL_UART0_CTS             IOID_0          /* CTS */
#define CC26X2R1_LAUNCHXL_UART0_RTS             IOID_26         /* RTS */
#endif
#define CC26X2R1_LAUNCHXL_UART1_RX              PIN_UNASSIGNED
#define CC26X2R1_LAUNCHXL_UART1_TX              PIN_UNASSIGNED
#define CC26X2R1_LAUNCHXL_UART1_CTS             PIN_UNASSIGNED
#define CC26X2R1_LAUNCHXL_UART1_RTS             PIN_UNASSIGNED
/* For backward compatibility */
#define CC26X2R1_LAUNCHXL_UART_RX               CC26X2R1_LAUNCHXL_UART0_RX
#define CC26X2R1_LAUNCHXL_UART_TX               CC26X2R1_LAUNCHXL_UART0_TX
#define CC26X2R1_LAUNCHXL_UART_CTS              CC26X2R1_LAUNCHXL_UART0_CTS
#define CC26X2R1_LAUNCHXL_UART_RTS              CC26X2R1_LAUNCHXL_UART0_RTS

/*!
 *  @brief  Initialize the general board specific settings
 *
 *  This function initializes the general board specific settings.
 */
void CC26X2R1_LAUNCHXL_initGeneral(void);

/*!
 *  @brief  Shut down the external flash present on the board files
 *
 *  This function bitbangs the SPI sequence necessary to turn off
 *  the external flash on LaunchPads.
 */
void CC26X2R1_LAUNCHXL_shutDownExtFlash(void);

/*!
 *  @brief  Wake up the external flash present on the board files
 *
 *  This function toggles the chip select for the amount of time needed
 *  to wake the chip up.
 */
void CC26X2R1_LAUNCHXL_wakeUpExtFlash(void);


/*!
 *  @def    CC26X2R1_LAUNCHXL_ADCBufName
 *  @brief  Enum of ADCs
 */
typedef enum CC26X2R1_LAUNCHXL_ADCBufName {
    CC26X2R1_LAUNCHXL_ADCBUF0 = 0,

    CC26X2R1_LAUNCHXL_ADCBUFCOUNT
} CC26X2R1_LAUNCHXL_ADCBufName;

/*!
 *  @def    CC26X2R1_LAUNCHXL_ADCBuf0SourceName
 *  @brief  Enum of ADCBuf channels
 */
typedef enum CC26X2R1_LAUNCHXL_ADCBuf0ChannelName {
    CC26X2R1_LAUNCHXL_ADCBUF0CHANNEL0 = 0,
    CC26X2R1_LAUNCHXL_ADCBUF0CHANNEL1,
    CC26X2R1_LAUNCHXL_ADCBUF0CHANNEL2,
    CC26X2R1_LAUNCHXL_ADCBUF0CHANNEL3,
    CC26X2R1_LAUNCHXL_ADCBUF0CHANNEL4,
    CC26X2R1_LAUNCHXL_ADCBUF0CHANNEL5,
    CC26X2R1_LAUNCHXL_ADCBUF0CHANNEL6,
    CC26X2R1_LAUNCHXL_ADCBUF0CHANNEL7,
    CC26X2R1_LAUNCHXL_ADCBUF0CHANNELVDDS,
    CC26X2R1_LAUNCHXL_ADCBUF0CHANNELDCOUPL,
    CC26X2R1_LAUNCHXL_ADCBUF0CHANNELVSS,

    CC26X2R1_LAUNCHXL_ADCBUF0CHANNELCOUNT
} CC26X2R1_LAUNCHXL_ADCBuf0ChannelName;

/*!
 *  @def    CC26X2R1_LAUNCHXL_ADCName
 *  @brief  Enum of ADCs
 */
typedef enum CC26X2R1_LAUNCHXL_ADCName {
    CC26X2R1_LAUNCHXL_ADC0 = 0,
    CC26X2R1_LAUNCHXL_ADC1,
    CC26X2R1_LAUNCHXL_ADC2,
    CC26X2R1_LAUNCHXL_ADC3,
    CC26X2R1_LAUNCHXL_ADC4,
    CC26X2R1_LAUNCHXL_ADC5,
    CC26X2R1_LAUNCHXL_ADC6,
    CC26X2R1_LAUNCHXL_ADC7,
    CC26X2R1_LAUNCHXL_ADCDCOUPL,
    CC26X2R1_LAUNCHXL_ADCVSS,
    CC26X2R1_LAUNCHXL_ADCVDDS,

    CC26X2R1_LAUNCHXL_ADCCOUNT
} CC26X2R1_LAUNCHXL_ADCName;

/*!
 *  @def    CC26X2R1_LAUNCHXL_ECDHName
 *  @brief  Enum of ECDH names
 */
typedef enum CC26X2R1_LAUNCHXL_ECDHName {
    CC26X2R1_LAUNCHXL_ECDH0 = 0,

    CC26X2R1_LAUNCHXL_ECDHCOUNT
} CC26X2R1_LAUNCHXL_ECDHName;

/*!
 *  @def    CC26X2R1_LAUNCHXL_ECDSAName
 *  @brief  Enum of ECDSA names
 */
typedef enum CC26X2R1_LAUNCHXL_ECDSAName {
    CC26X2R1_LAUNCHXL_ECDSA0 = 0,

    CC26X2R1_LAUNCHXL_ECDSACOUNT
} CC26X2R1_LAUNCHXL_ECDSAName;

/*!
 *  @def    CC26X2R1_LAUNCHXL_ECJPAKEName
 *  @brief  Enum of ECJPAKE names
 */
typedef enum CC26X2R1_LAUNCHXL_ECJPAKEName {
    CC26X2R1_LAUNCHXL_ECJPAKE0 = 0,

    CC26X2R1_LAUNCHXL_ECJPAKECOUNT
} CC26X2R1_LAUNCHXL_ECJPAKEName;

/*!
 *  @def    CC26X2R1_LAUNCHXL_AESCCMName
 *  @brief  Enum of AESCCM names
 */
typedef enum CC26X2R1_LAUNCHXL_AESCCMName {
    CC26X2R1_LAUNCHXL_AESCCM0 = 0,

    CC26X2R1_LAUNCHXL_AESCCMCOUNT
} CC26X2R1_LAUNCHXL_AESCCMName;

/*!
 *  @def    CC26X2R1_LAUNCHXL_AESECBName
 *  @brief  Enum of AESECB names
 */
typedef enum CC26X2R1_LAUNCHXL_AESECBName {
    CC26X2R1_LAUNCHXL_AESECB0 = 0,

    CC26X2R1_LAUNCHXL_AESECBCOUNT
} CC26X2R1_LAUNCHXL_AESECBName;

/*!
 *  @def    CC26X2R1_LAUNCHXL_SHA2Name
 *  @brief  Enum of SHA2 names
 */
typedef enum CC26X2R1_LAUNCHXL_SHA2Name {
    CC26X2R1_LAUNCHXL_SHA20 = 0,

    CC26X2R1_LAUNCHXL_SHA2COUNT
} CC26X2R1_LAUNCHXL_SHA2Name;

/*!
 *  @def    CC26X2R1_LAUNCHXL_GPIOName
 *  @brief  Enum of GPIO names
 */
typedef enum CC26X2R1_LAUNCHXL_GPIOName {
    CC26X2R1_LAUNCHXL_GPIO_S1 = 0,
    CC26X2R1_LAUNCHXL_GPIO_S2,
    CC26X2R1_LAUNCHXL_SPI_MASTER_READY,
    CC26X2R1_LAUNCHXL_SPI_SLAVE_READY,
    CC26X2R1_LAUNCHXL_GPIO_LED_GREEN,
    CC26X2R1_LAUNCHXL_GPIO_LED_RED,
    CC26X2R1_LAUNCHXL_GPIO_SPI_FLASH_CS,
    CC26X2R1_LAUNCHXL_SDSPI_CS,
    CC26X2R1_LAUNCHXL_GPIO_LCD_CS,
    CC26X2R1_LAUNCHXL_GPIO_LCD_POWER,
    CC26X2R1_LAUNCHXL_GPIO_LCD_ENABLE,
    CC26X2R1_LAUNCHXL_GPIOCOUNT
} CC26X2R1_LAUNCHXL_GPIOName;

/*!
 *  @def    CC26X2R1_LAUNCHXL_GPTimerName
 *  @brief  Enum of GPTimer parts
 */
typedef enum CC26X2R1_LAUNCHXL_GPTimerName {
    CC26X2R1_LAUNCHXL_GPTIMER0A = 0,
    CC26X2R1_LAUNCHXL_GPTIMER0B,
    CC26X2R1_LAUNCHXL_GPTIMER1A,
    CC26X2R1_LAUNCHXL_GPTIMER1B,
    CC26X2R1_LAUNCHXL_GPTIMER2A,
    CC26X2R1_LAUNCHXL_GPTIMER2B,
    CC26X2R1_LAUNCHXL_GPTIMER3A,
    CC26X2R1_LAUNCHXL_GPTIMER3B,

    CC26X2R1_LAUNCHXL_GPTIMERPARTSCOUNT
} CC26X2R1_LAUNCHXL_GPTimerName;

/*!
 *  @def    CC26X2R1_LAUNCHXL_GPTimers
 *  @brief  Enum of GPTimers
 */
typedef enum CC26X2R1_LAUNCHXL_GPTimers {
    CC26X2R1_LAUNCHXL_GPTIMER0 = 0,
    CC26X2R1_LAUNCHXL_GPTIMER1,
    CC26X2R1_LAUNCHXL_GPTIMER2,
    CC26X2R1_LAUNCHXL_GPTIMER3,

    CC26X2R1_LAUNCHXL_GPTIMERCOUNT
} CC26X2R1_LAUNCHXL_GPTimers;

/*!
 *  @def    CC26X2R1_LAUNCHXL_I2CName
 *  @brief  Enum of I2C names
 */
typedef enum CC26X2R1_LAUNCHXL_I2CName {
    CC26X2R1_LAUNCHXL_I2C0 = 0,

    CC26X2R1_LAUNCHXL_I2CCOUNT
} CC26X2R1_LAUNCHXL_I2CName;

/*!
 *  @def    CC26X2R1_LAUNCHXL_NVSName
 *  @brief  Enum of NVS names
 */
typedef enum CC26X2R1_LAUNCHXL_NVSName {
    CC26X2R1_LAUNCHXL_NVSCC26XX0 = 0,
    CC26X2R1_LAUNCHXL_NVSSPI25X0,

    CC26X2R1_LAUNCHXL_NVSCOUNT
} CC26X2R1_LAUNCHXL_NVSName;

/*!
 *  @def    CC26X2R1_LAUNCHXL_PWM
 *  @brief  Enum of PWM outputs
 */
typedef enum CC26X2R1_LAUNCHXL_PWMName {
    CC26X2R1_LAUNCHXL_PWM0 = 0,
    CC26X2R1_LAUNCHXL_PWM1,
    CC26X2R1_LAUNCHXL_PWM2,
    CC26X2R1_LAUNCHXL_PWM3,
    CC26X2R1_LAUNCHXL_PWM4,
    CC26X2R1_LAUNCHXL_PWM5,
    CC26X2R1_LAUNCHXL_PWM6,
    CC26X2R1_LAUNCHXL_PWM7,

    CC26X2R1_LAUNCHXL_PWMCOUNT
} CC26X2R1_LAUNCHXL_PWMName;

/*!
 *  @def    CC26X2R1_LAUNCHXL_SDName
 *  @brief  Enum of SD names
 */
typedef enum CC26X2R1_LAUNCHXL_SDName {
    CC26X2R1_LAUNCHXL_SDSPI0 = 0,

    CC26X2R1_LAUNCHXL_SDCOUNT
} CC26X2R1_LAUNCHXL_SDName;

/*!
 *  @def    CC26X2R1_LAUNCHXL_SPIName
 *  @brief  Enum of SPI names
 */
typedef enum CC26X2R1_LAUNCHXL_SPIName {
    CC26X2R1_LAUNCHXL_SPI0 = 0,
    CC26X2R1_LAUNCHXL_SPI1,

    CC26X2R1_LAUNCHXL_SPICOUNT
} CC26X2R1_LAUNCHXL_SPIName;

/*!
 *  @def    CC26X2R1_LAUNCHXL_UARTName
 *  @brief  Enum of UARTs
 */
typedef enum CC26X2R1_LAUNCHXL_UARTName {
    CC26X2R1_LAUNCHXL_UART0 = 0,
    CC26X2R1_LAUNCHXL_UART1,

    CC26X2R1_LAUNCHXL_UARTCOUNT
} CC26X2R1_LAUNCHXL_UARTName;

/*!
 *  @def    CC26X2R1_LAUNCHXL_UDMAName
 *  @brief  Enum of DMA buffers
 */
typedef enum CC26X2R1_LAUNCHXL_UDMAName {
    CC26X2R1_LAUNCHXL_UDMA0 = 0,

    CC26X2R1_LAUNCHXL_UDMACOUNT
} CC26X2R1_LAUNCHXL_UDMAName;

/*!
 *  @def    CC26X2R1_LAUNCHXL_WatchdogName
 *  @brief  Enum of Watchdogs
 */
typedef enum CC26X2R1_LAUNCHXL_WatchdogName {
    CC26X2R1_LAUNCHXL_WATCHDOG0 = 0,

    CC26X2R1_LAUNCHXL_WATCHDOGCOUNT
} CC26X2R1_LAUNCHXL_WatchdogName;

#ifdef __cplusplus
}
#endif

#endif /* __CC26X2R1_LAUNCHXL_BOARD_H__ */
