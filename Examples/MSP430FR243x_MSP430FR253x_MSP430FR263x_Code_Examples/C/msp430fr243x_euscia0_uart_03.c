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
//   MSP430FR24xx Demo - USCI_A0 External Loopback test @ 115200 baud
//
//  Description: This demo connects TX to RX of the MSP430 UART
//  The example code shows proper initialization of registers
//  and interrupts to receive and transmit data. If data is incorrect P1.0 LED is
//  turned ON.
//  ACLK = n/a, MCLK = SMCLK = BRCLK = DCODIV ~1MHz.
//
//                MSP430FR2433
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//            |                 |
//            |     P1.4/UCA0TXD|----
//            |                 |   |
//            |     P1.5/UCA0RXD|----
//            |                 |
//            |            P1.0 |--> LED
//            |                 |

//   Ling Zhu
//   Texas Instruments Inc.
//   July 2015
//   Built with IAR Embedded Workbench v6.20 & Code Composer Studio v6.0.1
//******************************************************************************
#include <msp430.h>

unsigned char RXData = 0;
unsigned char TXData = 1;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                 // Stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5;                     // Disable the GPIO power-on default high-impedance mode
                                              // to activate previously configured port settings
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;                           // P1.0 out low

    // Configure UART pins
    P1SEL0 |= BIT4 | BIT5;                    // set 2-UART pin as second function

    // Configure UART
    UCA0CTLW0 |= UCSWRST;                     // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__SMCLK;               
    // Baud Rate calculation
    UCA0BR0 = 8;                              // 1000000/115200 = 8.68
    UCA0MCTLW = 0xD600;                       // 1000000/115200 - INT(1000000/115200)=0.68
                                              // UCBRSx value = 0xD6 (See UG)
    UCA0BR1 = 0;
    UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

    while (1)
    {
        while(!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = TXData;                   // Load data onto buffer

        __bis_SR_register(LPM0_bits|GIE);     // Enter LPM0
        __no_operation();                     // For debugger
    }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(UCA0IV,USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE: break;
        case USCI_UART_UCRXIFG:
              UCA0IFG &=~ UCRXIFG;            // Clear interrupt
              RXData = UCA0RXBUF;             // Clear buffer
              if(RXData != TXData)            // Check value
              {
                  P1OUT |= BIT0;              // If incorrect turn on P1.0
                  while(1);                   // trap CPU
              }
              TXData++;                             // increment data byte
              __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0 on reti
              break;
        case USCI_UART_UCTXIFG: break;
        case USCI_UART_UCSTTIFG: break;
        case USCI_UART_UCTXCPTIFG: break;
    }
}
