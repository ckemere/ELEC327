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
//  MSP430FR24xx Demo - ADC, Lo_Batt, Set P1.0 if AVcc < 2.50V
//
//  Description: A single sample is made on A13 (Vref) with reference set to
//  DVCC. Software sets ADC10SC to start sample and conversion
//  - ADC10SC automatically cleared at EOC. Once conversion is completed, the
//  following equation is used to calculate DVCC = (1023 * 1.5) / adcResult.
//  if DVCC is < 2.5V, P1.0 set indicating a lo_Batt condition, else reset.
//
//                MSP430FR2433
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |A13 (Vref)   P1.0|-->LED
//
//  William Goh
//  Texas Instruments Inc.
//  August 2014
//  Built with IAR Embedded Workbench v6.20 & Code Composer Studio v6.0.1
//******************************************************************************
#include <msp430.h>

unsigned int adcResult;                     // Temporarily stores the ADC value
                                            // that was read from ADCMEM0
int main(void)
{
    volatile unsigned long dvccValue;       // Calculated DVCC value

    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT

    P1DIR |= BIT0;                          // Set P1.0 to output direction

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Configure ADC10
    ADCCTL0 &= ~ADCENC;                     // Disable ADC
    ADCCTL0 = ADCSHT_2 | ADCON;             // ADCON, S&H=16 ADC clks
    ADCCTL1 = ADCSHP;                       // ADCCLK = MODOSC; sampling timer
    ADCCTL2 = ADCRES;                       // 10-bit conversion results
    ADCIE = ADCIE0;                         // Enable ADC conv complete interrupt
    ADCMCTL0 = ADCINCH_13 | ADCSREF_0;      // A13 ADC input select = 1.5V Ref
                                            // Vref = DVCC
    // Configure reference module located in the PMM
    PMMCTL0_H = PMMPW_H;                    // Unlock the PMM registers
    PMMCTL2 |= INTREFEN;                    // Enable internal reference
    while(!(PMMCTL2 & REFGENRDY));          // Poll till internal reference settles

    while(1)
    {
        ADCCTL0 |= ADCENC | ADCSC;          // Sampling and conversion start
        __bis_SR_register(LPM0_bits | GIE); // Enter LPM0, ADC_ISR will force exit
        __no_operation();                   // For debug only

        // To calculate DVCC, the following equation is used
        // DVCC = (1023 * 1.5) / adcResult
        // The following equation is modified to use only integers instead
        // of using float. All results needs to be divided by 100 to obtain
        // the final value.
        // DVCC = (1023 * 150) / adcResult
        dvccValue = ((unsigned long)1023 * (unsigned long)150) / (unsigned long) (adcResult);

        if (dvccValue < 250)                // DVCC < 2.50V ?
            P1OUT |= BIT0;                  // Set P1.0 LED on
        else
            P1OUT &= ~BIT0;                 // Clear P1.0 LED off
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
    switch(__even_in_range(ADCIV, ADCIV_ADCIFG))
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
            adcResult = ADCMEM0;            // Read ADC memory
            __bic_SR_register_on_exit(LPM0_bits);// Exit from LPM
            break;
        default:
            break;
    }
}
