/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2014, Texas Instruments Incorporated
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
 *
 *******************************************************************************
 * 
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//***************************************************************************************
//  MSP430FR24xx Demo - Timer0_A3 Capture of ACLK
//
//  Description: Capture a number of periods of the ACLK clock and store them in an array.
//  When the set number of periods is captured the program is trapped and the LED on
//  P1.0 is toggled. At this point halt the program execution read out the values using
//  the debugger.
//  ACLK = REFOCLK = 32kHz, MCLK = SMCLK = default DCODIV = 1MHz.
//
//                MSP430FR2433
//             -----------------
//         /|\|                 |
//          | |             P1.2|<-- TA0.CCI2A
//          --|RST              |  |
//            |             P2.2|--> ACLK
//            |                 |
//            |             P1.0|-->LED
//
//
//  Wei Zhao
//  Texas Instruments Inc.
//  Jan 2014
//  Built with IAR Embedded Workbench v6.20 & Code Composer Studio v6.0.1
//***************************************************************************************
#include <msp430.h>

#define NUMBER_TIMER_CAPTURES        20

volatile unsigned int timerAcaptureValues[NUMBER_TIMER_CAPTURES];
unsigned int timerAcapturePointer = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                       // Stop watchdog timer

    // Configure GPIO
    P1DIR |= BIT0;                                  // Set P1.0 as output
    P1OUT |= BIT0;                                  // P1.0 high
    P1SEL1 |= BIT2;                                 // TA0.CCI2A input capture pin, second function
    P1REN |= BIT2;                                  // enable internal pull-down resistor
    P1OUT &= ~BIT2;
    
    P2SEL1 |= BIT2;                                 // Set as ACLK pin, second function
    P2DIR |= BIT2;

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Configure clock
    __bis_SR_register(SCG0);                        // disable FLL
    CSCTL3 |= SELREF__REFOCLK;                      // Set REFO as FLL reference source
    CSCTL0 = 0;                                     // clear DCO and MOD registers
    CSCTL1 &= ~(DCORSEL_7);                         // Clear DCO frequency select bits first
    CSCTL1 |= DCORSEL_2;                            // Set DCO = 4MHz
    CSCTL2 = FLLD_1 + 60;                           // DCODIV = 2MHz
    __delay_cycles(3);  
    __bic_SR_register(SCG0);                        // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));      // Poll until FLL is locked

    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;      // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
                                                    // default DCODIV as MCLK and SMCLK source
    CSCTL5 |= DIVM__1 | DIVS__2;                    // SMCLK = 1MHz, MCLK = 2MHz

    // Timer0_A3 Setup
    TA0CCTL2 |= CM_1 | CCIS_0 | CCIE | CAP | SCS;
                                                    // Capture rising edge,
                                                    // Use CCI2A=ACLK,
                                                    // Synchronous capture,
                                                    // Enable capture mode,
                                                    // Enable capture interrupt

    TA0CTL |= TASSEL_2 | MC_2 | TACLR;              // Use SMCLK as clock source, clear TA0R
                                                    // Start timer in continuous mode

    __bis_SR_register(LPM0_bits | GIE);
    __no_operation();
}

// Timer0_A3 CC1-2, TA Interrupt Handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A1_VECTOR))) TIMER0_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(TA0IV,TA0IV_TAIFG))
    {
        case TA0IV_NONE:   
            break;                                  // No interrupt
        case TA0IV_TACCR1: 
            break;                                  // CCR1 not used
        case TA0IV_TACCR2:
            timerAcaptureValues[timerAcapturePointer++] = TA0CCR2;
            if (timerAcapturePointer >= 20)
            {
                while (1) 
                {
                    P1OUT ^= 0x01;                  // Toggle P1.0 (LED)
                    __delay_cycles(100000);
                }
            }        
            break;                                  // CCR2 not used
        case TA0IV_TAIFG:
            break;                                  // overflow 
        default: 
            break;
    }
}
