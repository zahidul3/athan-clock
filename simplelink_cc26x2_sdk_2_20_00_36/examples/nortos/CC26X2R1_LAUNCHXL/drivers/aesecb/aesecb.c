/*
 * Copyright (c) 2017, Texas Instruments Incorporated
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
 *  ======== aesecb.c ========
 */
#include <stdint.h>
#include <stdio.h>

/* For sleep */
#include <unistd.h>

/* Driver Header files */
#include <ti/drivers/UART.h>
#include <ti/drivers/AESECB.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>

/* Example/Board Header files */
#include "Board.h"

/* Defines */
#define MAX_MSG_LENGTH 100

/* UART pre-formated strings */
const uint8_t  promptStartup[]      = "\n\n\rAES-ECB Driver encryption and decryption demo.\n\n\rEncrypting:\n\n\r";
const uint8_t  promptEncrypted[]    = "\n\n\rThe encrypted data is:\n\n\r";
const uint8_t  promptDecrypted[]    = "\n\n\rThe decrypted data is:\n\n\r";
const uint8_t  decryptError[]       = "\n\n\rDecryption was unsuccessful.";
const uint8_t  encryptError[]       = "\n\n\rDecryption was unsuccessful.";

/* Plain text to encrypt (size is multiple of the block size) */
const uint8_t  plaintext[]          = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
                                       0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
                                       0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
                                       0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51};

const uint8_t  plaintextLength      = sizeof(plaintext);

/* AES key material */
const uint8_t  keyMaterial[16]      = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                                       0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};

/* Message buffers */
uint8_t        cipherText[MAX_MSG_LENGTH];
uint8_t        formatedText[MAX_MSG_LENGTH*2];

/*
 *  ======== printAsHex ========
 */
void printAsHex(UART_Handle handle, uint8_t* msg, uint8_t size)
{
    uint32_t i;
    uint32_t tmpLength = 0;

    /* Format the message as printable hex */
    for (i = 0; i < size; i++) {
        tmpLength += sprintf((char*) formatedText + (i * 2), "%02X", *(msg + i));
    }

    /* Print result */
    UART_write(handle, formatedText, tmpLength);
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    int_fast16_t result;

    /* Driver handles */
    UART_Handle         uartHandle;
    AESECB_Handle       aesecbHandle;

    /* UART variables */
    UART_Params         uartParams;

    /* CryptoKey storage */
    CryptoKey           cryptoKey;

    /* AESECB variables */
    AESECB_Operation    operationOneStepEncrypt;
    AESECB_Operation    operationOneStepDecrypt;

    /* Call driver Initialization functions */
    UART_init();
    AESECB_init();

    /* Open UART for console output */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;

    uartHandle = UART_open(Board_UART0, &uartParams);

    if (!uartHandle) {
        /* UART_open() failed */
        while (1);
    }

    /* Open AESCCM_open */
    aesecbHandle = AESECB_open(0, NULL);

    if (!aesecbHandle) {
        /* AESECM_open_open() failed */
        while(1);
    }

    /* Initialize the key structure */
    CryptoKeyPlaintext_initKey(&cryptoKey, (uint8_t*) keyMaterial, sizeof(keyMaterial));

    /* Prompt startup message */
    UART_write(uartHandle, promptStartup, sizeof(promptStartup));

    /* Prompt the message that is to be encrypted */
    printAsHex(uartHandle, (uint8_t*) plaintext, plaintextLength);

    /* Perform a single step encrypt operation of the plain text */
    AESECB_Operation_init(&operationOneStepEncrypt);
    operationOneStepEncrypt.key            = &cryptoKey;
    operationOneStepEncrypt.input          = (uint8_t*) plaintext;
    operationOneStepEncrypt.output         = cipherText;
    operationOneStepEncrypt.inputLength    = plaintextLength;

    result = AESECB_oneStepEncrypt(aesecbHandle, &operationOneStepEncrypt);

    if (result != AESECB_STATUS_SUCCESS) {
        /* Error while encrypting */
        UART_write(uartHandle, encryptError, sizeof(encryptError));
    }
    else {
        /* Print prompt and the encrypted result */
        UART_write(uartHandle, promptEncrypted, sizeof(promptEncrypted));
        printAsHex(uartHandle, cipherText, plaintextLength);

        /* Perform a single step decrypt operation of the cipher text */
        AESECB_Operation_init(&operationOneStepDecrypt);
        operationOneStepDecrypt.key            = &cryptoKey;
        operationOneStepDecrypt.input          = cipherText;
        operationOneStepDecrypt.output         = cipherText;
        operationOneStepDecrypt.inputLength    = plaintextLength;

        result = AESECB_oneStepDecrypt(aesecbHandle, &operationOneStepDecrypt);

        if (result != AESECB_STATUS_SUCCESS) {
            /* Error while decrypting */
            UART_write(uartHandle, decryptError, sizeof(decryptError));
        }
        else {
            /*
             * Print out the decrypted result, the resultant cipher text should be:
             * 0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
             * 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
             * 0xf5, 0xd3, 0xd5, 0x85, 0x03, 0xb9, 0x69, 0x9d,
             * 0xe7, 0x85, 0x89, 0x5a, 0x96, 0xfd, 0xba, 0xaf
             */
            /* Print prompt and the decrypted text*/
            UART_write(uartHandle, promptDecrypted, sizeof(promptDecrypted));
            printAsHex(uartHandle, cipherText, plaintextLength);
        }
    }

    /* Sleep forever */
    while(1) {
        sleep(5000);
    }
}
