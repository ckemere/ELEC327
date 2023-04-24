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
//  MSP430FR24xx Demo - ADC, Sample A2/A1/A0, internal 1.5V Ref.
//
//  Description: This example works on Repeat -sequence-of-channels Mode
//  with TA1.1B as the trigger signal.
//  A2/A1/A0 is sampled 16ADCclks with reference to 1.5V.
//  Internal oscillator times sample (16x) and conversion(13x).
//  Inside ADC_ISR A2/A1/A0 sample value put into array ADC_Result[3].
//  ACLK = default REFO ~32768Hz, MCLK = SMCLK = default DCODIV ~1MHz.
//  
//  Note: The TA1.1B is configured for 200us 50% PWM, which will trigger ADC
//  sample-and-conversion every 200us. The period of TA1.1B trigger event
//  should be more than the time period taken for ADC sample-and-conversion
//  and ADC interrupt service routine of each channel, which is about 57us in
//  this code
//
//
//                MSP430FR2433
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//        >---|P1.2/A2          |
//        >---|P1.1/A1          |
//        >---|P1.0/A0          |
//
//
//  Ling Zhu
//  Texas Instruments Inc.
//  July 2018
//  Built with IAR Embedded Workbench v7.11 & Code Composer Studio v8.0
//******************************************************************************
#include <msp430.h>

unsigned char ADC_Result[3];                                    // 8-bit ADC conversion result array
unsigned char i;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                                   // Stop WDT
    
    // Configure ADC A0~2 pins
    SYSCFG2 |= ADCPCTL0 | ADCPCTL1 | ADCPCTL2;
  
    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Configure ADC 
    ADCCTL0 |= ADCSHT_2 | ADCON;                                // 16ADCclks, ADC ON
    ADCCTL1 |= ADCSHP | ADCSHS_2 | ADCCONSEQ_3;                 // ADC clock MODCLK, sampling timer, TA1.1B trig.,repeat sequence
    ADCCTL2 &= ~ADCRES;                                         // 8-bit conversion results
    ADCMCTL0 |= ADCINCH_2 | ADCSREF_1;                          // A0~2(EoS); Vref=1.5V
    ADCIE |= ADCIE0;                                            // Enable ADC conv complete interrupt
    
    // Configure reference
    PMMCTL0_H = PMMPW_H;                                        // Unlock the PMM registers
    PMMCTL2 |= INTREFEN;                                        // Enable internal reference
    __delay_cycles(400);                                        // Delay for reference settling

    // Configure TA1.1B as ADC trigger signal
    // Note: The TA1.1B is configured for 200us 50% PWM, which will trigger ADC
    // sample-and-conversion every 200us. The period of TA1.1B trigger event
    // should be more than the time period taken for ADC sample-and-conversion
    // and ADC interrupt service routine of each channel, which is about 57us in this code
    TA1CCR0 = 200-1;                                            // PWM Period, 200us
    TA1CCTL1 = OUTMOD_7;                                        // CCR1 reset/set
    TA1CCR1 = 100;                                              // CCR1 PWM duty cycle, 50%
    TA1CTL = TASSEL__SMCLK | MC__UP | TACLR;                    // SMCLK, up mode, clear TAR

    i = 2;

    while(1)                                
    {
        ADCCTL0 |= ADCENC;                                       // Enable ADC
        TA1CTL |= TACLR;                                         // Clear TAR to start the ADC sample
        __bis_SR_register(LPM0_bits | GIE);                      // Enter LPM0 w/ interrupts
        __no_operation();                                        // Only for debug
    }
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
            ADC_Result[i] = ADCMEM0;
            if(i == 0)
            {
                __no_operation();   // Only for debug
                i = 2;
            }
            else
            {
                i--;
            }
            break;                                             
        default: 
            break; 
    }  
}
