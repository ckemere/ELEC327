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
//  MSP430G2xx3 Demo - USCI_A0 IrDA External Loopback Test, 8MHz SMCLK
//
//  Description: This example transmits bytes through the USCI module
//  configured for IrDA mode, and receives them using an external loopback
//  connection. The transfered sequence is 00h, 01h, 02h, ..., ffh. The
//  received bytes are also stored in memory starting at address RxData.
//  In the case of an RX error the LED is lighted and program execution stops.
//  An external loopback connection has been used as it allows for the
//  connection of a scope to monitor the communication, which is not possible
//  when using the internal loopback.
//  ACLK = n/a, MCLK = SMCLK = BRCLK = CALxxx_8MHZ = 8MHz
//
//              MSP430G2xx3
//            -----------------
//        /|\|              XIN|-
//         | |                 |
//         --|RST          XOUT|-
//           |                 |
//           |     P1.1/UCA0RXD|--+   external
//           |     P1.2/UCA0TXD|--+   loopback connection
//           |                 |
//           |                 |
//           |             P1.0|--->  LED
//           |                 |
//
//  D. Dang
//  Texas Instruments Inc.
//  February 2011
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************
#include <msp430.h>

unsigned char RxByte;
volatile unsigned char RxData[256];
unsigned char TxByte;
volatile unsigned int i;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  if (CALBC1_8MHZ==0xFF)					// If calibration constant erased
  {											
    while(1);                               // do not load, trap CPU!!	
  }
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_8MHZ;
  DCOCTL = CALDCO_8MHZ;                     // Load 8MHz constants
  P1OUT &= ~BIT0;                           // Clear P1.0
  P1DIR |= BIT0;                            // P1.0 output
  P1SEL |= BIT1 + BIT2;                     // Use P3.4/P3.5 for USCI_A0
  P1SEL2 |= BIT1 + BIT2;					// Both P1SEL & P1SEL2 bits must be set for UCA functions
  UCA0CTL1 |= UCSWRST;                      // Set SW Reset
  UCA0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
  UCA0BR0 = 52;                             // 8MHz/52=153.8KHz
  UCA0BR1 = 0;
  UCA0MCTL = UCBRF_1 + UCOS16;              // Set 1st stage modulator to 1
                                            // 16-times oversampling mode
  UCA0IRTCTL = UCIRTXPL2 + UCIRTXPL0 + UCIRTXCLK + UCIREN;
                                            // Pulse length = 6 half clock cyc
                                            // Enable BITCLK16, IrDA enc/dec
  UCA0CTL1 &= ~UCSWRST;                     // Resume operation

  TxByte = 0x00;                            // TX data and pointer, 8-bit

  while (1)
  {
    for (i = 1000; i; i--);                 // Small delay
    while (!(IFG2 & UCA0TXIFG));            // USCI_A0 TX buffer ready?
    UCA0TXBUF = TxByte;                     // TX character

    __disable_interrupt();
    IE2 |= UCA0RXIE;                        // Enable RX int
    __bis_SR_register(CPUOFF + GIE);        // Enter LPM0 w/ interrupts

    RxData[TxByte] = RxByte;                // Store RXed character in RAM
    if (TxByte != RxByte)                   // RX OK?
    {
      P1OUT |= BIT0;                        // LED P1.0 on
      while (1);                            // Trap PC here
    }
    TxByte++;                               // Next character to TX
  }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCIAB0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  RxByte = UCA0RXBUF;                       // Get RXed character
  IE2 &= ~UCA0RXIE;                         // Disable RX int
  __bic_SR_register_on_exit(CPUOFF);        // Exit LPM0
}
