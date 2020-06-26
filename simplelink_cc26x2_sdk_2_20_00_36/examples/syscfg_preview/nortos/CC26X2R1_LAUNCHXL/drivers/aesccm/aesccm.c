/*
 * Copyright (c) 2018, Texas Instruments Incorporated
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
/*
 *  ======== aesccm.c ========
 */

/* ======== Includes ======== */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/* Driver Header files */
#include <ti/drivers/UART.h>
#include <ti/drivers/AESCCM.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>

/* Example/Board Header files */
#include "Board.h"

/* ======== Defines ======== */
#define GET_TEXT        0
#define GET_NONCE       1
#define GET_MAC         2

#define MAX_MSG_LENGTH  100

/* ======== UART pre-formated strings ======== */
const uint8_t  promptStartup[]      = "\n\n\rAES-CCM Driver encryption and decryption demo.";
const uint8_t  promptEnter[]        = "\n\n\rEnter 'e' to encrypt a string or 'd' to decrypt a HEX coded string (e.g. 'A3B4B1'):";
const uint8_t  promptAadEncrypt[]   = "\n\n\rEnter a string to authenticate but not encrypt: ";
const uint8_t  promptAadDecrypt[]   = "\n\n\rEnter a string to verify but not decrypt: ";
const uint8_t  promptPlainText[]    = "\n\n\rEnter a string to encrypt and authenticate: ";
const uint8_t  promptCipherText[]   = "\n\n\rEnter a string to decrypt and verify: ";
const uint8_t  promptNonce[]        = "\n\n\rEnter a nonce (Do not use a nonce more then once for any AES key as this will break security):\n\r";
const uint8_t  promptMac[]          = "\n\n\rEnter a MAC: ";
const uint8_t  promptEncrypted[]    = "\n\n\rThe encrypted data is:\n\n\r";
const uint8_t  promptDecrypted[]    = "\n\n\rThe decrypted data is:\n\n\r";
const uint8_t  promptGeneratedMac[] = "\n\n\rThe MAC is:\n\n\r";
const uint8_t  decryptError[]       = "\n\n\rDecryption was unsuccessful.";
const uint8_t  encryptError[]       = "\n\n\rEncryption was unsuccessful.";
const uint8_t  shortNonce[]         = "\n\n\rThe nonce entered is too short, try again and make sure it is between 7-13 characters.";
const uint8_t  longNonce[]          = "\n\n\rThe nonce entered is too long, try again and make sure it is between 7-13 characters.";
const uint8_t  shortMac[]           = "\n\n\rThe MAC entered is too short, try again.";
const uint8_t  longMac[]            = "\n\n\rThe MAC entered is too long, try again.";

/* ======== AES key material ======== */
const uint8_t  keyMaterial[16]      = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                                       0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
/* ======== Message buffers ======== */
uint8_t        aad[MAX_MSG_LENGTH];
uint8_t        plainText[MAX_MSG_LENGTH];
uint8_t        cipherText[MAX_MSG_LENGTH];
uint8_t        formatedText[(MAX_MSG_LENGTH*2)+25];
uint8_t        nonce[13];
uint8_t        macHex[16];
uint8_t        macAlpha[32];

/* Length variables */
uint32_t       aadLength;
uint32_t       textLength;
uint32_t       nonceLength;
uint32_t       macLength;

/* ======== Helper function prototypes ======== */
/* Reads input needed for decryption/encryption from the console */
void getDataToProcess(UART_Handle handle, bool isDecryption);

/* Convert a alphanumeric hex representation to a binary representation */
uint8_t stringToHex(uint8_t* msg, uint8_t msgLength);

/* Print binary represented hex as text */
void printAsHex(UART_Handle handle, uint8_t* msg, uint16_t size);

/*  ======== mainThread ======== */
void *mainThread(void *arg0)
{
    int_fast16_t result;

    /* Driver handles */
    UART_Handle         uartHandle;
    AESCCM_Handle       aesccmHandle;

    /* UART variables */
    UART_Params         uartParams;
    uint8_t             input;

    /* CryptoKey storage */
    CryptoKey           cryptoKey;

    /* AESCCM variables */
    AESCCM_Operation    operationOneStepEncrypt;
    AESCCM_Operation    operationOneStepDecrypt;

    /* Call driver Initialization functions */
    UART_init();
    AESCCM_init();

    /* Open UART for console output */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode    = UART_DATA_BINARY;
    uartParams.readDataMode     = UART_DATA_BINARY;
    uartParams.readReturnMode   = UART_RETURN_FULL;
    uartParams.readEcho         = UART_ECHO_OFF;
    uartParams.baudRate         = 115200;

    uartHandle = UART_open(Board_UART0, &uartParams);

    if (!uartHandle) {
        /* UART_open() failed */
        while (1);
    }

    /* Open AESCCM_open */
    aesccmHandle = AESCCM_open(0, NULL);

    if (!aesccmHandle) {
        /* AESCCM_open_open() failed */
        while(1);
    }

    /* Initialize the key structure */
    CryptoKeyPlaintext_initKey(&cryptoKey, (uint8_t*) keyMaterial, sizeof(keyMaterial));

    /* Prompt startup message */
    UART_write(uartHandle, promptStartup, sizeof(promptStartup));

    /* Loop forever */
    while(1) {
        /* Prompt for action and wait for input */
        UART_write(uartHandle, promptEnter, sizeof(promptEnter));
        UART_read(uartHandle, &input, 1);

        /* If encryption */
        if (input == 'e') {
            /* Prompt the user to enter the values used during the encryption */
            getDataToProcess(uartHandle, false);

            /* Perform a single step encrypt operation of the message */
            AESCCM_Operation_init(&operationOneStepEncrypt);
            operationOneStepEncrypt.key            = &cryptoKey;
            operationOneStepEncrypt.aad            = aad;
            operationOneStepEncrypt.aadLength      = aadLength;
            operationOneStepEncrypt.input          = plainText;
            operationOneStepEncrypt.output         = cipherText;
            operationOneStepEncrypt.inputLength    = textLength;
            operationOneStepEncrypt.nonce          = nonce;
            operationOneStepEncrypt.nonceLength    = nonceLength;
            operationOneStepEncrypt.mac            = macHex;
            operationOneStepEncrypt.macLength      = sizeof(macHex);

            result = AESCCM_oneStepEncrypt(aesccmHandle, &operationOneStepEncrypt);

            if (result != AESCCM_STATUS_SUCCESS) {
                /* Error while encrypting */
                UART_write(uartHandle, encryptError, sizeof(encryptError));
            }
            else {
                /* Print prompt and the encrypted result and mac*/
                UART_write(uartHandle, promptEncrypted, sizeof(promptEncrypted));
                printAsHex(uartHandle, cipherText, textLength);

                UART_write(uartHandle, promptGeneratedMac, sizeof(promptMac));
                printAsHex(uartHandle, macHex, sizeof(macHex));
            }
        }
        else if (input == 'd') {
            /* Prompt the user to enter the values used during the decryption */
            getDataToProcess(uartHandle, true);

            /* Squash cipherText from string to hex */
            textLength = stringToHex(cipherText, textLength);
            /* Squash MAC from string to hex */
            macLength = stringToHex(macAlpha, sizeof(macAlpha));

            /* Perform a single step decrypt operation of the message */
            AESCCM_Operation_init(&operationOneStepDecrypt);
            operationOneStepDecrypt.key            = &cryptoKey;
            operationOneStepDecrypt.aad            = aad;
            operationOneStepDecrypt.aadLength      = aadLength;
            operationOneStepDecrypt.input          = cipherText;
            operationOneStepDecrypt.output         = plainText;
            operationOneStepDecrypt.inputLength    = textLength;
            operationOneStepDecrypt.nonce          = nonce;
            operationOneStepDecrypt.nonceLength    = nonceLength;
            operationOneStepDecrypt.mac            = macAlpha;
            operationOneStepDecrypt.macLength      = macLength;

            result = AESCCM_oneStepDecrypt(aesccmHandle, &operationOneStepDecrypt);

            if (result != AESCCM_STATUS_SUCCESS) {
                /* Error while decrypting */
                UART_write(uartHandle, decryptError, sizeof(decryptError));
            }
            else {
                /* Print out the decrypted result */
                UART_write(uartHandle, promptDecrypted, sizeof(promptDecrypted));
                UART_write(uartHandle, plainText, textLength);
            }
        }
    }
}

/* ======== Helper function declaration ======== */

/* Reads in a single entry from the console */
uint8_t getConsoleInput(UART_Handle handle, uint8_t* msg, uint8_t size, int8_t inputType)
{
    uint16_t count = 0;
    uint8_t  input;
    bool     bufferOverflow = false;

    /* Read in from the console until carriage-return is detected */
    while (1) {
        UART_read(handle, &input, 1);

        /* If not carriage-return, echo input and continue to read */
        if (input != 0x0D) {
            /* Check if message buffer is full */
            if (count == size) {
                /* We can't read in any more data as the buffer is full, it will become an overflow */
                bufferOverflow = true;
                break;
            }
            UART_write(handle, &input, 1);
        }
        else {
            /* The string was terminated */
            break;
        }

        /* Store the received message */
        msg[count] = input;
        count++;
    }

    /* If we are reading in a nonce, make sure it is between 7-13 characters. */
    if ((inputType == GET_NONCE) && (count < 7)) {
        UART_write(handle, shortNonce, sizeof(shortNonce));
        return 0;
    }
    else if ((inputType == GET_NONCE) && bufferOverflow) {
        UART_write(handle, longNonce, sizeof(longNonce));
        return 0;
    }

    /* If we are reading a MAC the entered string must be 2*16 (16 hex values in string representation) */
    if ((inputType == GET_MAC) && (count < size)) {
        UART_write(handle, shortMac, sizeof(shortMac));
        return 0;
    }
    else if ((inputType == GET_MAC) && bufferOverflow) {
        UART_write(handle, longMac, sizeof(longMac));
        return 0;
    }

    return count;
}

/* Reads input needed for decryption/encryption from the console */
void getDataToProcess(UART_Handle handle, bool isDecryption)
{
    nonceLength = 0;
    macLength   = 0;

    /* Read in string to authenticate from console */
    if (isDecryption) {
        UART_write(handle, promptAadDecrypt, sizeof(promptAadDecrypt));
    }
    else {
        UART_write(handle, promptAadEncrypt, sizeof(promptAadEncrypt));
    }
    aadLength = getConsoleInput(handle, aad, sizeof(aad), GET_TEXT);

    /* Read in string to encrypt/decrypt from console */
    if (isDecryption) {
        UART_write(handle, promptCipherText, sizeof(promptCipherText));
        textLength = getConsoleInput(handle, cipherText, sizeof(cipherText), GET_TEXT);
    }
    else {
        UART_write(handle, promptPlainText, sizeof(promptPlainText));
        textLength = getConsoleInput(handle, plainText, sizeof(plainText), GET_TEXT);
    }

    /*
     * Read in a nonce from the console.
     * Ask for a new nonce until a valid nonce has been entered.
     */
    while (!nonceLength) {
        UART_write(handle, promptNonce, sizeof(promptNonce));
        nonceLength = getConsoleInput(handle, nonce, sizeof(nonce), GET_NONCE);
    }

    /* If decryption ... */
    if (isDecryption) {
        /* ... ask for a nonce until a valid nonce is entered */
        while (!macLength) {
            UART_write(handle, promptMac, sizeof(promptMac));
            macLength = getConsoleInput(handle, macAlpha, sizeof(macAlpha), GET_MAC);
        }
    }
}

/* Convert a alphanumeric hex representation to a binary representation */
uint8_t stringToHex(uint8_t* msg, uint8_t msgLength)
{
    /* Squash string representation of hex values to single byte hex */
    uint32_t i;
    uint32_t j = 0;
    for (i = 0; i < msgLength; i += 2) {
        uint8_t upper = (*(msg + i) > 57)     ? (*(msg + i) - 55)     : (*(msg + i) - 48);
        uint8_t lower = (*(msg + i + 1) > 57) ? (*(msg + i + 1) - 55) : (*(msg + i + 1) - 48);
        *(msg + j) = ((upper << 4) + lower) & 0xFF;
        j++;
    }

    /* Return length of the squashed message */
    return j;
}

/* Print binary represented hex as text */
void printAsHex(UART_Handle handle, uint8_t* msg, uint16_t size)
{
    uint32_t i;
    uint16_t formatedLength = 0;

    /* Format the message as printable hex */
    for (i = 0; i < size; i++) {
        formatedLength += sprintf((char*) formatedText + (i * 2), "%02X", *(msg + i));
    }

    /* Print result */
    UART_write(handle, formatedText, formatedLength);
}
