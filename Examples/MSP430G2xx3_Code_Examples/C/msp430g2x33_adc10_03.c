/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2012, Texas Instruments Incorporated
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
//  MSP430G2x33/G2x53 Demo - ADC10, Sample A10 Temp, Set P1.0 if Temp ++ ~2C
//
//  Description: se ADC10 and the integrated temperature sensor to detect
//  temperature gradients. The temperature sensor output voltage is sampled
//  ~ every 120ms and compared with the defined delta values using an ISR.
//  (ADC10OSC/4)/64 determines sample time which needs to be greater than
//  30us for temperature sensor.
//  ADC10 is operated in repeat-single channel mode with the sample and
//  convert trigger sourced from Timer_A CCR1. The ADC10IFG at the end
//  of each converstion will trigger an ISR.
//  ACLK = n/a, MCLK = SMCLK = default DCO ~ 1.2MHz, ADC10CLK = ADC10OSC
//
//                MSP430G2x33/G2x53
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |A10          P1.0|-->LED
//
//  D. Dang
//  Texas Instruments Inc.
//  December 2010
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************
#include <msp430.h>

static unsigned int FirstADCVal;            // holds 1st ADC result

#define ADCDeltaOn       3                  // ~ 2 Deg C delta for LED on

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog
  ADC10CTL1 = ADC10DIV_3 + INCH_10 + SHS_1 + CONSEQ_2;  // TA trig., rpt, A10
  ADC10CTL0 = SREF_1 + ADC10SHT_3 + REF2_5V + ADC10IE + REFON + ADC10ON;
  __enable_interrupt();                     // Enable interrupts.
  TACCR0 = 30;                              // Delay to allow Ref to settle
  TACCTL0 |= CCIE;                          // Compare-mode interrupt.
  TACTL = TASSEL_2 | MC_1;                  // TACLK = SMCLK, Up mode.
  LPM0;                                     // Wait for delay.
  TACCTL0 &= ~CCIE;                         // Disable timer Interrupt
  __disable_interrupt();
  ADC10CTL0 |= ENC;
  TACCTL1 = OUTMOD_4;                       // Toggle on EQU1 (TAR = 0)
  TACTL = TASSEL_2 + MC_2;                  // SMCLK, cont-mode
  while (!(ADC10IFG & ADC10CTL0));          // First conversion?
  FirstADCVal = ADC10MEM;                   // Read out 1st ADC value
  P1OUT = 0x00;                             // Clear P1
  P1DIR = 0x01;                             // P1.0 as output
  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC10_VECTOR))) ADC10_ISR (void)
#else
#error Compiler not supported!
#endif
{
  if (ADC10MEM >= FirstADCVal + ADCDeltaOn)
    P1OUT |= 0x01;                          // LED on
  else
    P1OUT &= ~0x01;                         // LED off
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void ta0_isr(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) ta0_isr (void)
#else
#error Compiler not supported!
#endif
{
  TACTL = 0;
  LPM0_EXIT;                                // Exit LPM0 on return
}
