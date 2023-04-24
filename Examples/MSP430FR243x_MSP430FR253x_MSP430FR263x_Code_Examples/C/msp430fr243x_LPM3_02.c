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
//  MSP430FR243x Demo - LPM3 entry with MCLK 16MHz
//
//  Description: When MCLK is 16MHz, and if the temperature changes significantly 
//                    for entering LPM3 and exiting from LPM3, the MCLK frequency at 
//                    wake-up may be significantly different from when the LPM3 is 
//                    entered, and may be out of the specified operation range. To avoid
//                    this, the MCLK can be set to be divided from DCOCLKDIV. 
// 
//           MSP430FR2433
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST       P1.0 |---> LED
//        |               |

//
//
//  Ling Zhu
//  Texas Instruments Inc.
//  Oct 2015
//  Built with IAR Embedded Workbench v6.20 & Code Composer Studio v6.0.1
//******************************************************************************
#include <msp430.h>

void Init_GPIO(void);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                                              // Stop watchdog timer
    
    // Configure one FRAM waitstate as required by the device datasheet for MCLK
    // operation beyond 8MHz _before_ configuring the clock system.
    FRCTL0 = FRCTLPW | NWAITS_1;
    
    // Configure GPIO
    Init_GPIO();
    
    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    __bis_SR_register(SCG0);                                               // disable FLL
    CSCTL3 |= SELREF__REFOCLK;                                             // Set REFO as FLL reference source
    CSCTL0 = 0;                                                            // clear DCO and MOD registers
    CSCTL1 &= ~(DCORSEL_7);                                                // Clear DCO frequency select bits first
    CSCTL1 |= DCORSEL_5;                                                   // Set DCO = 16MHz
    CSCTL2 = FLLD_0 + 487;                                                 // DCOCLKDIV = 16MHz
    __delay_cycles(3);   
    __bic_SR_register(SCG0);                                               // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));                             // FLL locked
    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;                             // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
                                                                           // default DCOCLKDIV as MCLK and SMCLK source
   
    // Configure RTC
    RTCMOD = 10-1;                                                         // Interrupt and reset happen every 10*1000*(1/10KHz) = ~1S
    RTCCTL |= RTCSS__VLOCLK | RTCSR | RTCPS__1000;                          
    RTCCTL |= RTCIE;
   
    while(1)
    {  
        CSCTL5 |= DIVM_1;                                                  // MCLK = 0.5* DCOCLKDIV = 8MHz - only for case of temperature changes significantly for LPM3 entry and LPM3 exit

        P1OUT ^= BIT0;
        
        __bis_SR_register(LPM3_bits | GIE);                                // Enter LPM3, Stop all clocks
        __no_operation();                                                  // For debug
    }
}

// RTC interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=RTC_VECTOR
__interrupt void RTC_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(RTC_VECTOR))) RTC_ISR (void)
#else
#error Compiler not supported!
#endif
{
    // Configure the CS
    __bic_SR_register(SCG0);                                               // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));                             // FLL locked
  
    CSCTL5 &= DIVM_1;                                                      // MCLK = DCOCLKDIV = 16MHz - only for case of temperature changes significantly for LPM3 entry and LPM3 exit

    switch(__even_in_range(RTCIV,RTCIV_RTCIF))
    {
        case RTCIV_NONE :  break;                                          // No interrupt
        case RTCIV_RTCIF:  __bic_SR_register_on_exit(LPM3_bits);           // RTC Overflow
                                                                           // Exit LPM3 on reti
                           break;
        default: break;
    }
}

void Init_GPIO()
{
    P1DIR = 0xFF; P2DIR = 0xFF; P3DIR = 0xFF; 
    P1REN = 0xFF; P2REN = 0xFF; P3REN = 0xFF;
    P1OUT = 0x00; P2OUT = 0x00; P3OUT = 0x00;
}
