/*
 * uart_cc.c
 *
 *  Created on: Dec 12, 2019
 *      Author: Jahid's Desktop
 */

/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "IoDef.h"

#include "uart_cc.h"

const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_ACLK,          // SMCLK Clock Source
        13,                                     // BRDIV = 78
        0,                                       // UCxBRF = 2
        3,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION  // Oversampling
};

void initUART_CC(void)
{
    /* Selecting P3.2/PM_UCA2RXD/PM_UCA2SOMI and P3.3/PM_UCA2TXD/PM_UCA2SIMO in UART mode */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    //![Simple UART Example]
    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A2_BASE, &uartConfig);

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A2_BASE);

    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT | EUSCI_A_UART_TRANSMIT_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA2);
    GPIO_setOutputLowOnPin(CC_INT_PORT, CC_INT_PIN);
}
