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
//******************************************************************************
//  MSP430FR24xx Demo - Timer0_A3, Toggle P1.0, Overflow ISR, 32kHz ACLK
//
//  Description: Toggle P1.0 using software and the Timer0_A overflow ISR.
//  In this example an ISR triggers when TA overflows. Inside the ISR P1.0
//  is toggled. Toggle rate is exactly 0.5Hz. Proper use of the TAIV interrupt
//  vector generator is demonstrated.
//
//  ACLK = TACLK = 32768Hz, MCLK = SMCLK = 8MHz/2
//
//
//                MSP430FR2433
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//            |             P1.0|--> LED
//
//
//  Wei Zhao
//  Texas Instruments Inc.
//  Jan 2014
//  Built with IAR Embedded Workbench v6.20 & Code Composer Studio v6.0.1
//******************************************************************************
#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                    // Stop WDT

    // Configure clock
    __bis_SR_register(SCG0);                     // disable FLL
    CSCTL3 = SELREF__REFOCLK;                    // Set REFOCLK as FLL reference source
    CSCTL0 = 0;                                  // clear DCO and MOD registers
    CSCTL1 &= ~(DCORSEL_7);                      // Clear DCO frequency select bits first
    CSCTL1 |= DCORSEL_3;                         // Set DCOCLK = 8MHz
    CSCTL2 = FLLD_1 + 121;                       // FLLD = 1, by default, DCODIV = DCO/2 = 4MHz
    __delay_cycles(3);  
    __bic_SR_register(SCG0);                     // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));   // Poll until FLL is locked

    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;   // set ACLK = REFOCLK = 32768Hz, DCOCLK as MCLK and SMCLK source
    CSCTL5 |= DIVM0 | DIVS0;                     // SMCLK = MCLK = DCODIV = 4MHz

    // Configure GPIO
    P1DIR |= BIT0;
    P1OUT |= BIT0;
    
    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5; 

    // Configure Timer_A
    TA0CTL = TASSEL_1 | MC_2 | TACLR | TAIE;      // ACLK, count mode, clear TAR, enable interrupt

    __bis_SR_register(LPM3_bits | GIE);           // Enter LPM3, enable interrupts
    __no_operation();                             // For debugger
}

// Timer0_A3 Interrupt Vector (TAIV) handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A1_VECTOR
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
            break;                               // No interrupt
        case TA0IV_TACCR1:
            break;                               // CCR1 not used
        case TA0IV_TACCR2: 
            break;                               // CCR2 not used
        case TA0IV_TAIFG:
            P1OUT ^= BIT0;                       // overflow
            break;
        default: 
            break;
    }
}
