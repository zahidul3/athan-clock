/*
 * uart_debug.c
 *
 *  Created on: Jul 31, 2019
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

#include "CircularBuffer.h"

#include "uart_debug.h"

char * DebugString;
uint8_t DebugStringLen;
uint8_t DebugStringIndex;
bool printDebugInProgress = false;

static TsCircularBuffer tx0Buffer;

//![Simple UART Config]
/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 *http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 Note the processor clock runs at 48000000 Hz
 */
const eUSCI_UART_Config uartConfigDebug =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        26,                                     // BRDIV = 78
        1,                                       // UCxBRF = 2
        0,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};


/* EUSCI A0 UART ISR - DEBUG UART */
void EUSCIA0_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);

    MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        //echo back chars to PC terminal
        MAP_UART_transmitData(EUSCI_A0_BASE, MAP_UART_receiveData(EUSCI_A0_BASE));
    }
    else if(status & EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
    {
        uint16_t nextChar = GetCharCircBuf(&tx0Buffer);
        if(nextChar != 0)
        {
            //DebugStringIndex++;
            MAP_UART_transmitData(EUSCI_A0_BASE, (nextChar & 0xff));
            //DebugStringLen--;
        }
    }
}

char bufferDebug[DEBUG_BUFFER_SIZE];

//print a number to the debug terminal
void printDebugInt(uint32_t debugInt)
{
    memset(bufferDebug, 0, DEBUG_BUFFER_SIZE);
    ltoa(debugInt, bufferDebug, 10);
    uint8_t strLen = strlen(bufferDebug);
    bufferDebug[strLen] = '.';
    bufferDebug[strLen+1] = '\n';
    bufferDebug[strLen+2] = '\r';
    bufferDebug[strLen+3] = 0;
    //if(printDebugInProgress == false)
    {
        int index;
        DebugStringIndex = 0;
        //printDebugInProgress = true;
        DebugStringLen = strlen(bufferDebug);
        DebugString = bufferDebug;
        //memcpy(DebugString, debugString, DebugStringLen);
        for(index=0; index<DebugStringLen; index++)
        {
            PutCharCircBuf(&tx0Buffer, *DebugString);
            DebugString++;
        }

        //start transmission if not busy
        while(UCA0STATW & BIT0);
        MAP_UART_transmitData(EUSCI_A0_BASE, (GetCharCircBuf(&tx0Buffer) & 0xff));
        //DebugStringLen--;
    }
}

void printDebugChar(const char ch)
{
    MAP_UART_transmitData(EUSCI_A0_BASE, ch & 0xff);
}

//print a string to the debug terminal
void printDebugString(const char * debugString)
{
    //if(printDebugInProgress == false)
    {
        int index;
        DebugStringIndex = 0;
        //printDebugInProgress = true;
        DebugStringLen = strlen(debugString);
        DebugString = debugString;
        //memcpy(DebugString, debugString, DebugStringLen);
        for(index=0; index<DebugStringLen; index++)
        {
            PutCharCircBuf(&tx0Buffer, *DebugString);
            DebugString++;
        }

        //start transmission if not busy
        while(UCA0STATW & BIT0);
        MAP_UART_transmitData(EUSCI_A0_BASE, (GetCharCircBuf(&tx0Buffer) & 0xff));
        //DebugStringLen--;
    }
}


void initUART0Debug(void)
{
    /* Selecting P1.2 and P1.3 in UART mode */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    //![Simple UART Example]
    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfigDebug);

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A0_BASE);

    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT | EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA0);

    InitCircBuf(&tx0Buffer, 1024);
}
