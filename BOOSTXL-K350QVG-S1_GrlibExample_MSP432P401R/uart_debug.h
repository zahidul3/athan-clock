/*
 * uart_debug.h
 *
 *  Created on: Jul 31, 2019
 *      Author: Jahid's Desktop
 */

#ifndef UART_DEBUG_H_
#define UART_DEBUG_H_

#define DEBUG_BUFFER_SIZE 64

void printDebug(char * debugString);
void printDebugChar(const char ch);
void printDebugInt(uint32_t debugInt);
void initUART0Debug(void);

#endif /* UART_DEBUG_H_ */
