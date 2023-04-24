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
//  MSP430G2xx3 Demo - USCI_B0 I2C Master to TMP100, Set P1.0 if Temp > 28C
//
//  Description: I2C interface to TMP100 temperature sensor in 9-bit mode.
//  Timer_A CCR0 interrupt is used to wake up and read the two bytes of
//  the TMP100 temperature register every 62ms. If the temperature is greater
//  than 28C, P1.0 is set, else reset. CPU is operated in LPM0. I2C speed
//  is ~100kHz.
//  ACLK = n/a, MCLK = SMCLK = TACLK = BRCLK = default DCO = ~1.2MHz
//
//         /|\           /|\ /|\
//          |   TMP100   10k 10k     MSP430G2xx3
//          |   -------   |   |   -------------------
//          +--|Vcc SDA|<-|---+->|P1.7/UCB0SDA    XIN|-
//          |  |       |  |      |                   |
//          +--|A1,A0  |  |      |               XOUT|-
//             |       |  |      |                   |
//          +--|Vss SCL|<-+------|P1.6/UCB0SCL   P1.0|---> LED
//         \|/  -------          |                   |
//
//  D. Dang
//  Texas Instruments Inc.
//  February 2011
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************
#include <msp430.h>

unsigned int RxByteCtr;
unsigned int RxWord;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  P1DIR |= BIT0;                            // P1.0 output
  P1SEL |= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0
  P1SEL2|= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0
  UCB0CTL1 |= UCSWRST;                      // Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
  UCB0BR0 = 12;                             // fSCL = SMCLK/12 = ~100kHz
  UCB0BR1 = 0;
  UCB0I2CSA = 0x4e;                         // Set slave address
  UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
  IE2 |= UCB0RXIE;                          // Enable RX interrupt
  TACTL = TASSEL_2 + MC_2;                  // SMCLK, contmode

  while (1)
  {
    RxByteCtr = 2;                          // Load RX byte counter
    UCB0CTL1 |= UCTXSTT;                    // I2C start condition
    __bis_SR_register(CPUOFF + GIE);        // Enter LPM0, enable interrupts
                                            // Remain in LPM0 until all data
                                            // is RX'd

    if (RxWord < 0x1d00)                    // >28C?
      P1OUT &= ~0x01;                       // No, P1.0 = 0
    else
      P1OUT |= 0x01;                        // Yes, P1.0 = 1

    __disable_interrupt();
    TACCTL0 |= CCIE;                        // TACCR0 interrupt enabled
    __bis_SR_register(CPUOFF + GIE);        // Enter LPM0, enable interrupts
                                            // Remain in LPM0 until TACCR0
                                            // interrupt occurs
    TACCTL0 &= ~CCIE;                       // TACCR0 interrupt disabled
  }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) TA0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  __bic_SR_register_on_exit(CPUOFF);        // Exit LPM0
}

// The USCIAB0TX_ISR is structured such that it can be used to receive any
// 2+ number of bytes by pre-loading RxByteCtr with the byte count.
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCIAB0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  RxByteCtr--;                              // Decrement RX byte counter

  if (RxByteCtr)
  {
    RxWord = (unsigned int)UCB0RXBUF << 8;  // Get received byte
    if (RxByteCtr == 1)                     // Only one byte left?
      UCB0CTL1 |= UCTXSTP;                  // Generate I2C stop condition
  }
  else
  {
    RxWord |= UCB0RXBUF;                    // Get final received byte,
                                            // Combine MSB and LSB
    __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
  }
}

