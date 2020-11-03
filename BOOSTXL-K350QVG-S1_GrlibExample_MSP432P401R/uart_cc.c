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

#include "athanTransport.h"
#include "CircularBuffer.h"
#include "IoDef.h"
#include "uart_cc.h"

static TsCircularBuffer txCCBuffer;
volatile uint8_t TxDataLen = 0;
bool TxDone = false;

uint8_t index = 0;
uint8_t rxBuffer[256] = {0};
volatile uint8_t dataLenRx = 0;

UARTLCDSTATE UartLcdState = UART_LCD_IDLE;

// configured to 9600(low power mode) 8N1
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

void resetUartStateRx(void)
{
    index = 0; // reset index
    UartLcdState = UART_LCD_IDLE;
}

/* EUSCI A0 UART ISR - Receives one byte at a time */
void EUSCIA2_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);

    MAP_UART_clearInterruptFlag(EUSCI_A2_BASE, status);

    // RX Data
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        if(UartLcdState != UART_RECEIVING_ATHAN_PACKET)
            index = 0;

        rxBuffer[index] = MAP_UART_receiveData(EUSCI_A2_BASE);

        if(rxBuffer[index] == ATHAN_PACKET_VERSION && (index == 0))
            UartLcdState = UART_RECEIVING_ATHAN_PACKET;

        index++;

        if(UartLcdState == UART_RECEIVING_ATHAN_PACKET)
        {
            if(index == 3)
            {
                dataLenRx = rxBuffer[index-1];
            }
            else if(index >= (ATHAN_PACKET_HEADER_LEN + dataLenRx))
            {
                handleRxMessage(rxBuffer);
                resetUartStateRx();
            }
        }
    }

    // TX Data, send one byte a time
    if(status & EUSCI_A_UART_TRANSMIT_INTERRUPT)
    {
        if(TxDone != true)
        {
            TxDataLen--;
            if(TxDataLen == 0)
            {
                TxDone = true;
                GPIO_setOutputLowOnPin(CC_INT_PORT, CC_INT_PIN);
            }
            else
            {
                uint8_t dataByte = GetUARTData();
                MAP_UART_transmitData(EUSCI_A2_BASE, dataByte);
            }
        }
    }
}

uint8_t GetUARTData()
{
    uint16_t nextChar = GetCharCircBuf(&txCCBuffer);
    if(nextChar != 0)
    {
        return (nextChar & 0xff);
    }
    else
    {
        return 0;
    }
}

// Sends data to CC device
void SendUARTCmd(uint8_t* data, uint8_t len)
{
    uint8_t dataByte=0, index=0;
    TxDone = false;
    GPIO_setOutputHighOnPin(CC_INT_PORT, CC_INT_PIN);
    polling_delay_ms(7); //wait for CC to wake and stabilize UART
    TxDataLen = len;
    for(index=0; index<TxDataLen; index++)
    {
        PutCharCircBuf(&txCCBuffer, *data);
        data++;
    }
    dataByte = GetUARTData();
    //printDebugInt(dataByte);
    MAP_UART_transmitData(EUSCI_A2_BASE, dataByte);
}

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

    InitCircBuf(&txCCBuffer, 512);
}
