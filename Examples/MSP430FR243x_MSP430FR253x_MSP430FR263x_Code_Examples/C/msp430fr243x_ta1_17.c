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
//*******************************************************************************
//  MSP430FR243x Demo - Timer1_A3, PWM TA1.1-2, Up Mode, 32KHz ACLK
//
//  Description: This program generates two PWM outputs on P1.4,P1.5 using
//  Timer1_A configured for up mode. The value in CCR0, 100-1, defines the PWM
//  period and the values in CCR1 and CCR2 the PWM duty cycles. Using 32768Hz
//  ACLK as TACLK, the timer period is ~3ms with a 75% duty cycle on P1.4
//  and 25% on P1.5.
//  ACLK = TACLK =XT1~32768Hz, SMCLK = MCLK = 4MHz
//
//
//          MSP430FR2433
//         ---------------
//     /|\|            XIN|--
//      | |               |  ~32768Hz
//      --|RST        XOUT|--
//        |               |
//        |     P1.5/TA1.1|--> CCR1 - 75% PWM
//        |     P1.4/TA1.2|--> CCR2 - 25% PWM
//
//  Ling Zhu
//  Texas Instruments Inc.
//  Feb 2015
//  Built with IAR Embedded Workbench v6.20 & Code Composer Studio v6.0.1
//******************************************************************************
#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                         // Stop WDT
    
    // Configure GPIO
    P1DIR |= BIT4 + BIT5;
    P1SEL1 |= BIT4 + BIT5;
    
    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;
    
    // Configure clock
    P2SEL0 |= BIT0 | BIT1;                  // set XT1 pin as second function
    do
    {
        CSCTL7 &= ~(XT1OFFG | DCOFFG);      // Clear XT1 and DCO fault flag
        SFRIFG1 &= ~OFIFG;
    }while (SFRIFG1 & OFIFG);               // Test oscillator fault flag

    __bis_SR_register(SCG0);                          // disable FLL
    CSCTL3 |= SELREF__REFOCLK;                        // Set REFOCLK as FLL reference source
    CSCTL0 = 0;                                       // clear DCO and MOD registers
    CSCTL1 &= ~(DCORSEL_7);                           // Clear DCO frequency select bits first
    CSCTL1 |= DCORSEL_3;                              // Set DCOCLK = 8MHz
    CSCTL2 = FLLD_1 + 121;                            // FLLD = 1, DCODIV = 4MHz
    __delay_cycles(3);  
    __bic_SR_register(SCG0);                          // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));        // Poll until FLL is locked    
    CSCTL4 = SELMS__DCOCLKDIV | SELA__XT1CLK;         // set ACLK = XT1 = 32768Hz, DCOCLK as MCLK and SMCLK source
    CSCTL5 |= DIVM0 | DIVS0;                          // SMCLK = MCLK = DCODIV = 1MHz, by default

    // Setup Timer1_A
    TA1CCR0 = 100-1;                                  // PWM Period
    TA1CCTL1 = OUTMOD_7;                              // CCR1 reset/set
    TA1CCR1 = 75;                                     // CCR1 PWM duty cycle
    TA1CCTL2 = OUTMOD_7;                              // CCR2 reset/set
    TA1CCR2 = 25;                                     // CCR2 PWM duty cycle
    TA1CTL = TASSEL_1 | MC_1 | TACLR;                 // ACLK, up mode, clear TAR
    
    __bis_SR_register(LPM3_bits);                     // Enter LPM3
    __no_operation();                                 // For debugger
}
