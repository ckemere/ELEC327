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
//  MSP430FR24xx Demo - ADC, Sample A1, internal 1.5V Ref, TA1.1 Trig, Set P1.0 if A1>0.5V
//
//  Description: This example works on Repeat-Single-Channel Mode.
//  A1 is sampled 16/second (ACLK/1024) with reference to 1.5V.
//  Timer_A is run in upmode and TA1.1B is used to automatically trigger
//  ADC conversion. Internal oscillator times sample (16x) and conversion(13x).
//  Inside ADC_ISR if A1 > 0.5V, P1.0 is set, else reset. Normal mode is LPM3.
//  ACLK = XT1 = 32768Hz, MCLK = SMCLK = default DCODIV ~1MHz.
//  //* An external watch crystal on XIN XOUT is required for ACLK *//
//
//                MSP430FR2433
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//        >---|P1.1/A1      P1.0|--> LED
//
//
//  Ling Zhu
//  Texas Instruments Inc.
//  Jan 2014
//  Built with IAR Embedded Workbench v6.20 & Code Composer Studio v6.0.1
//******************************************************************************

#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                                 // Stop WDT

    // Configure GPIO
    P1DIR |= BIT0;                                            // Set P1.0 to output direction
    P1OUT &= ~BIT0;                                           // Clear P1.0
  
    // Configure ADC A1 pin
    SYSCFG2 |= ADCPCTL1;    

    // Configure XT1 oscillator
    P2SEL0 |= BIT0 | BIT1;                                    // P2.0~P2.1: crystal pins
    
    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;
    
    CSCTL4 = SELA__XT1CLK;                                    // Set ACLK = XT1; MCLK = SMCLK = DCO
    do
    {
        CSCTL7 &= ~(XT1OFFG | DCOFFG);                        // Clear XT1 and DCO fault flag
        SFRIFG1 &= ~OFIFG;                                    
    }while (SFRIFG1 & OFIFG);                                 // Test oscillator fault flag

    // Configure ADC 
    ADCCTL0 |= ADCON | ADCMSC;                                // ADCON
    ADCCTL1 |= ADCSHP | ADCSHS_2 | ADCCONSEQ_2;                        // repeat single channel; TA1.1 trig sample start
    ADCCTL2 |= ADCRES;                                        // 10-bit conversion results
    ADCMCTL0 |= ADCINCH_1 | ADCSREF_1;                        // A1 ADC input select; Vref=1.5V
    ADCIE |= ADCIE0;                                          // Enable ADC conv complete interrupt
    
    // Configure reference
    PMMCTL0_H = PMMPW_H;                                      // Unlock the PMM registers
    PMMCTL2 |= INTREFEN;                                      // Enable internal reference
    __delay_cycles(400);                                      // Delay for reference settling    

    ADCCTL0 |= ADCENC;                                        // ADC Enable
    
  
    // ADC conversion trigger signal - TimerA1.1 (32ms ON-period)
    TA1CCR0 = 1024-1;                                         // PWM Period
    TA1CCR1 = 512-1;                                          // TA1.1 ADC trigger
    TA1CCTL1 = OUTMOD_4;                                      // TA1CCR0 toggle
    TA1CTL = TASSEL__ACLK | MC_1 | TACLR;                     // ACLK, up mode

    __bis_SR_register(LPM0_bits | GIE);                       // Enter LPM3 w/ interrupts
}

// ADC interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC_VECTOR))) ADC_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(ADCIV,ADCIV_ADCIFG))
    {
        case ADCIV_NONE: 
            break;                            
        case ADCIV_ADCOVIFG: 
            break;                        
        case ADCIV_ADCTOVIFG: 
            break;                       
        case ADCIV_ADCHIIFG: 
            break;                        
        case ADCIV_ADCLOIFG: 
            break;                        
        case ADCIV_ADCINIFG: 
            break;                        
        case ADCIV_ADCIFG: 
            if (ADCMEM0 < 0x155)                             // ADCMEM = A0 < 0.5V?
                P1OUT &= ~BIT0;                              // Clear P1.0 LED off
            else
                P1OUT |= BIT0;                               // Set P1.0 LED on
            ADCIFG = 0;
            break;                                           // Clear CPUOFF bit from 0(SR)       
        default: 
            break; 
    }  
}
