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
//  MSP430G2xx3 Demo - USCI_A0 IrDA Physical Layer Comm, 8MHz SMCLK
//
//  Description: This example receives bytes through the USCI module
//  configured for IrDA mode, and sends them out using the Timer_A UART
//  to a PC running a terminal software. Likewise, data received from the PC
//  through the Timer_A UART link is transmitted via IrDA.
//
//  ACLK = n/a, MCLK = SMCLK = BRCLK = CALxxx_8MHZ = 8MHz
//
//                                     MSP430G2xx3
//                               -----------------------
//                              |                       |
//                           /|\|                    XIN|-
//                            | |                       |
//                            --|RST                XOUT|-
//                              |                       |
//    GP2W0116YPS   /|\         |                       |
//      -------      |          |                       |
//     |    Vcc|-----+  IrDA    |               P2.4/TA2|--> 115,200 8N1
//     #    LED|-----+ 9600 8N1 |               P2.3/TA1|<-- Terminal SW
//     #    TxD|<---------------|P3.4/UCA0TXD           |
//     #    RxD|--------------->|P3.5/UCA0RXD           |
//     #     SD|-----+          |                       |
//     |    GND|-----+          |                       |
//      -------      |           -----------------------
//                  ---
//
//  D. Dang
//  Texas Instruments Inc.
//  February 2011
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************
#include <msp430.h>

#define BITTIME     69                      // UART bit time = 8MHz / 115,200
#define BITTIME_5   35                      // UART half bit time

#define FLAG_USCI   0x01                    // USCI data received
#define FLAG_UART   0x02                    // Timer_A UART data received

unsigned int TXData;                        // Timer_A UART TX data
unsigned char TxBitCnt;                     // Timer_A UART TX bit counter
unsigned char RXData;                       // Timer_A UART RX data
unsigned char RxBitCnt;                     // Timer_A UART RX bit counter
unsigned char Flags;                        // Flag register
unsigned char RXDataIR;                     // Received IrDA data

// Function prototypes
void RX_Ready(void);
void TX_Byte(unsigned char Data);

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
  P2SEL |= 0x18;                            // Use P2.3/P2.4 for Timer_A
  P2DIR |= 0x10;                            // P2.4 output
  P3SEL |= 0x30;                            // Use P3.4/P3.5 for USCI_A0
  UCA0CTL1 |= UCSWRST;                      // Set SW Reset
  UCA0CTL1 = UCSSEL_2 + UCSWRST;
  UCA0BR0 = 52;                             // 8MHz/52=153.8KHz
  UCA0BR1 = 0;
  UCA0MCTL = UCBRF_1 + UCOS16;
  UCA0IRTCTL = UCIRTXPL2 + UCIRTXPL0 + UCIRTXCLK + UCIREN;
                                            // Pulse length = 6 half clock cyc
                                            // Enable BITCLK16, IrDA enc/dec
  UCA0IRRCTL = UCIRRXPL;                    // Light = low pulse
  UCA0CTL1 &= ~UCSWRST;                     // Resume operation
  IE2 |= UCA0RXIE;                          // Enable RX int
  TACCTL2 = OUT;                            // TXD Idle as Mark
  TACTL = TASSEL_2 + MC_2;                  // SMCLK, continuous mode
  RX_Ready();                               // Ready Timer_A UART for RX

  while (1)
  {
    __disable_interrupt();                  // Disable interrupts
    if (!Flags)                             // Any events pending?
      __bis_SR_register(CPUOFF + GIE);      // Enter LPM0 w/ interrupts

    __enable_interrupt();                   // Enable interrupts

    if (Flags & FLAG_USCI)                  // USCI_A0 character received?
    {
      while (TACCTL2 & CCIE);               // Yes, ensure Timer_A UART is ready
      TX_Byte(RXDataIR);                    // Transmit using Timer_A UART
      Flags &= ~FLAG_USCI;                  // Clear flag
      IE2 |= UCA0RXIE;                      // Re-enable RX int
    }

    if (Flags & FLAG_UART)                  // Timer_A UART character received?
    {
      while (!(IFG2 & UCA0TXIFG));          // Ensure TX buffer is ready
      UCA0TXBUF = RXData;                   // Move RX'd character to USCI_A0
      Flags &= ~FLAG_UART;                  // Clear flag
      RX_Ready();                           // Ready Timer_A UART for RX
    }
  }
}

//------------------------------------------------------------------------------
// Read RXed character from USCI_A0, return from LPM0
//------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCIAB0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  RXDataIR = UCA0RXBUF;                     // Get RXed character
  IE2 &= ~UCA0RXIE;                         // Disable RX int
  Flags |= FLAG_USCI;                       // Indicate received character
  __bic_SR_register_on_exit(CPUOFF);        // Return active after receiption
}

//------------------------------------------------------------------------------
// Readies the Timer_A UART to receive on byte
//------------------------------------------------------------------------------
void RX_Ready(void)
{
  RxBitCnt = 8;                             // Load Bit counter
  TACCTL1 = SCS + CCIS0 + CM1 + CAP + CCIE; // Sync, Neg Edge, Capture
}

//------------------------------------------------------------------------------
// TX the byte 'Data' using Timer_A UART
//------------------------------------------------------------------------------
void TX_Byte(unsigned char Data)
{
  TxBitCnt = 10;                            // Load Bit counter, 8 data + ST/SP
  TACCR2 = TAR;                             // Current state of TA counter
  TACCR2 += BITTIME;                        // Some time till first bit
  TXData = Data;                            // Load global variable
  TXData |= 0x0100;                         // Add mark stop bit
  TXData <<= 1;                             // Add space start bit
  TACCTL2 = OUTMOD0 + CCIE;                 // TXD = mark = idle
}

//------------------------------------------------------------------------------
// Timer_A UART RX and TX
//------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A1_VECTOR
__interrupt void Timer_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A1_VECTOR))) Timer_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch (TAIV)        // Use calculated branching
  {
    case  2 :                               // TACCR1 CCIFG - UART RX
      TACCR1 += BITTIME;                    // Add Offset to TACCR1
      if (TACCTL1 & CAP)                    // Capture mode = start bit edge
      {
        TACCTL1 &= ~CAP;                    // Capture to compare mode
        TACCR1 += BITTIME_5;
      }
      else
      {
        RXData >>= 1;
        if (TACCTL1 & SCCI)                 // Get bit waiting in receive latch
          RXData |= 0x80;
        RxBitCnt--;
        if (RxBitCnt == 0)                  // All bits RXed?
        {
          TACCTL1 &= ~CCIE;                 // All bits RXed, disable interrupt
          Flags |= FLAG_UART;               // Indicate received character
          __bic_SR_register_on_exit(CPUOFF);// Clear LPM0 bits from 0(SR)
        }
      }
      break;
    case  4 :                               // TACCR2 CCIFG - UART TX
      TACCR2 += BITTIME;                    // Add Offset to TACCR2
      if (TxBitCnt == 0)                    // All bits TXed?
      {
        TACCTL2 &= ~CCIE;                   // All bits TXed, disable interrupt
        __bic_SR_register_on_exit(CPUOFF);  // Clear LPM0 bits from 0(SR)
      }
      else
      {
        if (TXData & 0x01)
          TACCTL2 &= ~OUTMOD2;              // TX Mark
        else
          TACCTL2 |= OUTMOD2;               // TX Space
        TXData >>= 1;
        TxBitCnt--;
      }
      break;
  }
}
