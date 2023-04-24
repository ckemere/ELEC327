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
//  MSP430FR243x Demo - ADC, Sample A12 Temp and Convert to oC and oF
//
//  Description: This example works on Single-Channel Single-Conversion Mode.
//  A single sample is made on A12 (temperature) with reference to
//  internal 1.5V Vref. Software sets ADCSC to start sample and conversion
//  periodically
//  - ADCSC automatically cleared at EOC. ADC internal oscillator times
//  sample and conversion. In Mainloop MSP430 waits in LPM3 to save power until
//  ADC conversion complete, ADC12_ISR will convert the result to oC and oF
//
//  ACLK = default REFO ~32768Hz, MCLK = SMCLK = default DCODIV ~1MHz.
//
//  Non-calibrated temperature measured from device to device will vary due to
//  slope and offset variance from device to device - please see datasheet.
//
//  This code example show how to use the TLV calibrated data to measure temperature.
//
//
//                MSP430FR2433
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//        >---|A12(temp)        |
//
//
//  Ling Zhu
//  Texas Instruments Inc.
//  Nov 2015
//  Built with IAR Embedded Workbench v6.20 & Code Composer Studio v6.0.1
//******************************************************************************
#include <msp430.h>

#define CALADC_15V_30C  *((unsigned int *)0x1A1A)                 // Temperature Sensor Calibration-30 C
                                                                  // See device datasheet for TLV table memory mapping
#define CALADC_15V_85C  *((unsigned int *)0x1A1C)                 // Temperature Sensor Calibration-85 C

volatile long temp;
volatile long IntDegF;
volatile long IntDegC;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                                     // Stop WDT

    TA0CCTL0 |= CCIE;                                             // TACCR0 interrupt enabled
    TA0CCR0 = 65535;
    TA0CTL = TASSEL__ACLK | MC__UP;                               // ACLK, UP mode

    // Configure ADC - Pulse sample mode; ADCSC trigger
    ADCCTL0 |= ADCSHT_8 | ADCON;                                  // ADC ON,temperature sample period>30us
    ADCCTL1 |= ADCSHP;                                            // s/w trig, single ch/conv, MODOSC
    ADCCTL2 |= ADCRES;                                            // 10-bit conversion results
    ADCMCTL0 |= ADCSREF_1 | ADCINCH_12;                           // ADC input ch A12 => temp sense
    ADCIE |=ADCIE0;                                               // Enable the Interrupt request for a completed ADC_B conversion

    // Configure reference
    PMMCTL0_H = PMMPW_H;                                          // Unlock the PMM registers
    PMMCTL2 |= INTREFEN | TSENSOREN;                              // Enable internal reference and temperature sensor
    __delay_cycles(400);                                          // Delay for reference settling

    __bis_SR_register(LPM0_bits | GIE);                           // LPM0 with interrupts enabled
    __no_operation();                                             // Only for debugger
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
            temp = ADCMEM0;
            // Temperature in Celsius
            // The temperature (Temp, C)=
            IntDegC = (temp-CALADC_15V_30C)*(85-30)/(CALADC_15V_85C-CALADC_15V_30C)+30;

            // Temperature in Fahrenheit
            // Tf = (9/5)*Tc | 32
            IntDegF = 9*IntDegC/5+32;
            //__bic_SR_register_on_exit(LPM0_bits);               // Exit LPM0

            break;
        default:
            break;
    }
}

// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
    ADCCTL0 |= ADCENC | ADCSC;                                    // Sampling and conversion start
}
