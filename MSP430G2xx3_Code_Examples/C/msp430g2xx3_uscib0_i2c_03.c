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
//  MSP430G2xx3 Demo - USCI_B0 I2C Master Interface to DAC8571, Write
//
//  Description: Using UCB0TXIE, a continuous sine wave is output to the
//  external DAC using a 16-point look-up table. Only one start condition
//  is executed. Data is handled by the ISR and the CPU is normally in LPM0.
//  ACLK = n/a, MCLK = SMCLK = BRCLK = default DCO = ~1.2MHz
//
//               MSP430G2xx3                       DAC8571
//           ------------------                   ------------
//         -|XIN   P1.7/UCB0SDA|<--------------->|SDA         |
//          |      P1.6/UCB0SCL|---------------->|SCL  I2C    |
//         -|XOUT              |                 |    SLAVE   |
//          |     I2C MASTER   |              GND|A0          |
//
//
//  DAC8571 I2C address = 0x4C (A0 = GND)
//
//  D. Dang
//  Texas Instruments Inc.
//  February 2011
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************
#include <msp430.h>

const unsigned char Sine_Tab[] =            // 16 Point 16-bit Sine Table
{
  0xFF,                                     // MSB Word 0
  0xFF,                                     // LSB
  0xF6,                                     // MSB Word 1
  0x40,                                     // LSB
  0xDA,                                     // MSB Word 2
  0x81,                                     // LSB
  0xB0,                                     // MSB Word 3
  0xFA,                                     // LSB
  0x7F,                                     // MSB Word 4
  0xFF,                                     // LSB
  0x4F,                                     // MSB Word 5
  0x03,                                     // LSB
  0x25,                                     // MSB Word 6
  0x7C,                                     // LSB
  0x09,                                     // MSB Word 7
  0xBD,                                     // LSB
  0x00,                                     // MSB Word 8
  0x00,                                     // LSB
  0x09,                                     // MSB Word 9
  0xBD,                                     // LSB
  0x25,                                     // MSB Word 10
  0x7C,                                     // LSB
  0x4F,                                     // MSB Word 11
  0x03,                                     // LSB
  0x7F,                                     // MSB Word 12
  0xFE,                                     // LSB
  0xB0,                                     // MSB Word 13
  0xFA,                                     // LSB
  0xDA,                                     // MSB Word 14
  0x81,                                     // LSB
  0xF6,                                     // MSB Word 15
  0x40                                      // LSB
};

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop Watchdog Timer 
  P1SEL |= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0
  P1SEL2|= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0
  UCB0CTL1 |= UCSWRST;                      // Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
  UCB0BR0 = 12;                             // fSCL = SMCLK/12 = ~100kHz
  UCB0BR1 = 0;
  UCB0I2CSA = 0x4c;                         // Set slave address
  UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
  IE2 |= UCB0TXIE;                          // Enable TX ready interrupt
  UCB0CTL1 |= UCTR + UCTXSTT;               // I2C TX, start condition
  UCB0TXBUF = 0x010;                        // Write DAC control byte
  __bis_SR_register(CPUOFF + GIE);          // Enter LPM0 w/ interrupts
}

// USCI_B0 Data ISR
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCIAB0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  static unsigned char ByteCtr;

  UCB0TXBUF = Sine_Tab[ByteCtr++];          // Transmit data byte
  ByteCtr &= 0x1f;                          // Do not exceed table
}
