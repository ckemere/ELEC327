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
//   MSP430G2xx3 Demo - USCI_A0, UART 9600 Full-Duplex Transceiver, 32kHz ACLK
//
//   Description: USCI_A0 communicates continously as fast as possible full-
//   duplex with another device. Normal mode is LPM3, with activity only during
//   RX and TX ISR's. The TX ISR indicates the USCI_A0 is ready to send another
//   character. The RX ISR indicates the USCI_A0 has received a character. At
//   9600 baud, a full character is tranceived ~1ms.
//   The levels on P1.3/P1.4 are TX'ed. RX'ed value is displayed on P1.6/0.  
//   ACLK = BRCLK = LFXT1 = 32768Hz, MCLK = SMCLK = DCO ~1.2MHz
//   Baud rate divider with 32768Hz XTAL @9600 = 32768Hz/9600 = 3.41
//* An external watch crystal is required on XIN XOUT for ACLK *//
//
//                 MSP430G2xx3                  MSP430G2xx3
//              -----------------            -----------------
//             |              XIN|-      /|\|              XIN|-
//             |                 | 32kHz  | |                 | 32kHz
//             |             XOUT|-       --|RST          XOUT|-
//             |                 | /|\      |                 |
//             |              RST|---       |                 |
//             |                 |          |                 |
//           ->|P1.3             |          |             P1.0|-> LED
//           ->|P1.4             |          |             P1.6|-> LED
//       LED <-|P1.0             |          |             P1.3|<-
//       LED <-|P1.6             |          |             P1.4|<-
//             |     UCA0TXD/P1.2|--------->|P1.1             |
//             |                 |   9600   |                 |
//             |     UCA0RXD/P1.1|<---------|P1.2             |
//
//
//   D. Dang
//   Texas Instruments Inc.
//   February 2011
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************
#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  P1OUT = 0x00;                             // P1.0/6 setup for LED output
  P1DIR = BIT0 + BIT6;
  P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
  P1SEL2 = BIT1 + BIT2;                      
  UCA0CTL1 |= UCSSEL_1;                     // CLK = ACLK
  UCA0BR0 = 0x03;                           // 32kHz/9600 = 3.41
  UCA0BR1 = 0x00;
  UCA0MCTL = UCBRS1 + UCBRS0;               // Modulation UCBRSx = 3
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCA0RXIE + UCA0TXIE;               // Enable USCI_A0 TX/RX interrupt

  __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3 w/ interrupts enabled
}

// USCI A0/B0 Transmit ISR
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  unsigned char TxByte=0;
  if (P1IN & BIT3)
    TxByte |= BIT6;
  if (P1IN & BIT4)
    TxByte |= BIT0;
  UCA0TXBUF = TxByte;                       // Read, justify, and transmit
}

// USCI A0/B0 Receive ISR
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  P1OUT = UCA0RXBUF;                        // Display RX'ed charater
}
