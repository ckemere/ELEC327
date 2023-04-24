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
//  MSP430FR243x Demo  - eUSCI_B0 I2C Master RX multiple bytes from MSP430 Slave
//
//  Description: This demo connects two MSP430's via the I2C bus. The master
//  reads 5 bytes from the slave. This is the MASTER CODE. The data from the slave
//  transmitter begins at 0 and increments with each transfer.
//  The USCI_B0 RX interrupt is used to know when new data has been received.
//  ACLK = default REFO ~32768Hz, MCLK = SMCLK = BRCLK = DCODIV ~1MHz.
//
//    *****used with "msp430fr243x_euscib0_i2c_11.c"****
//
//                                /|\  /|\
//               MSP430FR2433      10k  10k     MSP430FR2433
//                   slave         |    |        master
//             -----------------   |    |   -----------------
//            |     P1.2/UCB0SDA|<-|----|->|P1.2/UCB0SDA     |
//            |                 |  |       |                 | 
//            |                 |  |       |                 |
//            |     P1.3/UCB0SCL|<-|------>|P1.3/UCB0SCL     |
//            |                 |          |             P1.0|--> LED
//
//   Cen Fang
//   Texas Instruments Inc.
//   June 2013
//   Built with IAR Embedded Workbench v6.20 & Code Composer Studio v6.0.1
//******************************************************************************
#include <msp430.h>

volatile unsigned char RXData;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    // Configure GPIO
    P1OUT &= ~BIT0;                         // Clear P1.0 output latch
    P1DIR |= BIT0;                          // For LED
    P1SEL0 |= BIT2 | BIT3;                  // I2C pins

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Configure USCI_B0 for I2C mode
    UCB0CTLW0 |= UCSWRST;                   // Software reset enabled
    UCB0CTLW0 |= UCMODE_3 | UCMST | UCSYNC; // I2C mode, Master mode, sync
    UCB0CTLW1 |= UCASTP_2;                  // Automatic stop generated
                                            // after UCB0TBCNT is reached
    UCB0BRW = 0x0008;                       // baudrate = SMCLK / 8
    UCB0TBCNT = 0x0005;                     // number of bytes to be received
    UCB0I2CSA = 0x0048;                     // Slave address
    UCB0CTL1 &= ~UCSWRST;
    UCB0IE |= UCRXIE | UCNACKIE | UCBCNTIE;

    while (1)
    {
        __delay_cycles(2000);
        while (UCB0CTL1 & UCTXSTP);         // Ensure stop condition got sent
        UCB0CTL1 |= UCTXSTT;                // I2C start condition

        __bis_SR_register(LPM0_bits|GIE);   // Enter LPM0 w/ interrupt
    }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCIB0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCIB0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG))
  {
    case USCI_NONE: break;                  // Vector 0: No interrupts
    case USCI_I2C_UCALIFG: break;           // Vector 2: ALIFG
    case USCI_I2C_UCNACKIFG:                // Vector 4: NACKIFG
      UCB0CTL1 |= UCTXSTT;                  // I2C start condition
      break;
    case USCI_I2C_UCSTTIFG: break;          // Vector 6: STTIFG
    case USCI_I2C_UCSTPIFG: break;          // Vector 8: STPIFG
    case USCI_I2C_UCRXIFG3: break;          // Vector 10: RXIFG3
    case USCI_I2C_UCTXIFG3: break;          // Vector 14: TXIFG3
    case USCI_I2C_UCRXIFG2: break;          // Vector 16: RXIFG2
    case USCI_I2C_UCTXIFG2: break;          // Vector 18: TXIFG2
    case USCI_I2C_UCRXIFG1: break;          // Vector 20: RXIFG1
    case USCI_I2C_UCTXIFG1: break;          // Vector 22: TXIFG1
    case USCI_I2C_UCRXIFG0:                 // Vector 24: RXIFG0
      RXData = UCB0RXBUF;                   // Get RX data
      __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
      break;
    case USCI_I2C_UCTXIFG0: break;          // Vector 26: TXIFG0
    case USCI_I2C_UCBCNTIFG:                // Vector 28: BCNTIFG
      P1OUT ^= BIT0;                        // Toggle LED on P1.0
      break;
    case USCI_I2C_UCCLTOIFG: break;         // Vector 30: clock low timeout
    case USCI_I2C_UCBIT9IFG: break;         // Vector 32: 9th bit
    default: break;
  }
}
