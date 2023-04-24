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
//  MSP430FR243x Demo - eUSCI_B0 I2C 4 Hardware I2C slaves
//
//  Description: This demo connects two MSP430's via the I2C bus.
//  This code configures the MSP430 USCI to be addressed as 4 independent I2C
//  slaves. Each slave has its own interrupt flag and data variable to store
//  incoming data.
//  Use with msp430fr243x_uscib0_i2c_15.c
//  ACLK = REFO = 32768Hz, MCLK = SMCLK = default  = DCO = ~1MHz
//
//                                /|\  /|\
//               MSP430FR2433      10k  10k     MSP430FR2433
//                   slave         |    |        master
//             -----------------   |    |   -----------------
//            |     P1.2/UCB0SDA|<-|----|->|P1.2/UCB0SDA     |
//            |                 |  |       |                 |
//            |                 |  |       |                 |
//            |     P1.3/UCB0SCL|<-|------>|P1.3/UCB0SCL     |
//            |                 |          |                 |
//
//  Wei Zhao
//  Texas Instruments Inc.
//  Jan 2014
//  Built with IAR Embedded Workbench v6.20 & Code Composer Studio v6.0.1
//******************************************************************************

#include <msp430.h>

unsigned char RXData0=0;
unsigned char RXData1=0;
unsigned char RXData2=0;
unsigned char RXData3=0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                             // Stop watchdog timer

    // Configure Pins for I2C
    P1SEL0 |= BIT2 | BIT3;                                // I2C pins

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Configure USCI_B0 for I2C mode
    UCB0CTLW0 |= UCSWRST;                                 //Software reset enabled
    UCB0CTLW0 |= UCMODE_3;                                //I2C slave mode, SMCLK
    UCB0I2COA0 = 0x0A | UCOAEN;                           //SLAVE0 own address is 0x0A| enable
    UCB0I2COA1 = 0x0B | UCOAEN;                           //SLAVE1 own address is 0x0B| enable
    UCB0I2COA2 = 0x0C | UCOAEN;                           //SLAVE2 own address is 0x0C| enable
    UCB0I2COA3 = 0x0D | UCOAEN;                           //SLAVE3 own address is 0x0D| enable
    UCB0CTLW0 &=~UCSWRST;                                 //clear reset register

    UCB0IE |=  UCRXIE0 | UCRXIE1| UCRXIE2 | UCRXIE3;      //receive interrupt enable

    __bis_SR_register(LPM0_bits | GIE);                   // Enter LPM0 w/ interrupts
    __no_operation();
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
   switch(__even_in_range(UCB0IV,USCI_I2C_UCBIT9IFG))
    {
      case USCI_NONE: break;                                   // Vector 0: No interrupts break;
      case USCI_I2C_UCALIFG: break;                                   // Vector 2: ALIFG break;
      case USCI_I2C_UCNACKIFG: break;                                   // Vector 4: NACKIFG break;
      case USCI_I2C_UCSTTIFG: break;                                   // Vector 6: STTIFG break;
      case USCI_I2C_UCSTPIFG: break;                                   // Vector 8: STPIFG break;
      case USCI_I2C_UCRXIFG3:                                          // SLAVE3
        RXData3 = UCB0RXBUF;
        break;                                            // Vector 10: RXIFG3 break;
      case USCI_I2C_UCTXIFG3: break;                                   // Vector 14: TXIFG3 break;
      case USCI_I2C_UCRXIFG2:                                          // SLAVE2
        RXData2 = UCB0RXBUF;
        break;                                            // Vector 16: RXIFG2 break;
      case USCI_I2C_UCTXIFG2: break;                                   // Vector 18: TXIFG2 break;
      case USCI_I2C_UCRXIFG1:                                          // SLAVE1
        RXData1 = UCB0RXBUF;
        break;                                            // Vector 20: RXIFG1 break;
      case USCI_I2C_UCTXIFG1: break;                                   // Vector 22: TXIFG1 break;
      case USCI_I2C_UCRXIFG0:                                          // SLAVE0
        RXData0 = UCB0RXBUF;                              // Get RX data
        break;                                            // Vector 24: RXIFG0 break;
      case USCI_I2C_UCTXIFG0: break;                                   // Vector 26: TXIFG0 break;
      case USCI_I2C_UCBCNTIFG: break;                                   // Vector 28: BCNTIFG break;
      case USCI_I2C_UCCLTOIFG: break;                                   // Vector 30: clock low timeout break;
      case USCI_I2C_UCBIT9IFG: break;                                   // Vector 32: 9th bit break;
      default: break;
    }

}

