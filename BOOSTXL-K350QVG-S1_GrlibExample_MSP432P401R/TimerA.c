/*
 * Timer_A.c
 *
 *  Created on: Oct 11, 2020
 *      Author: zahidhaq
 */

#include "IoDef.h"
#include <msp432p401r.h>
#include "TimerA.h"

uint32_t RunningTime = 0;

void TimerAInit(void)
{
    TIMER_A1->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A1->CCR[0] = 32767;
    TIMER_A1->CTL = TIMER_A_CTL_SSEL__ACLK | // SMCLK, continuous mode
            TIMER_A_CTL_MC__UPDOWN;

    TIMER_A1->CCR[0] = 32767;              // Add Offset to TACCR0
    NVIC->ISER[0] = 1 << ((TA1_0_IRQn) & 31);
}

// Timer A0 interrupt service routine
void TA1_0_IRQHandler(void)
{
    TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    P1->OUT ^= BIT0;
    RunningTime++;
}

uint32_t GetRunningTime()
{
    return RunningTime;
}

