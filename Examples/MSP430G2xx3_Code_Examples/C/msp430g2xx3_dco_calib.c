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
//  MSP430G2xx3 Demo - Basic Clock, Output Buffered clocks with preloaded DCO
//                     calibration constants for BCSCTL1 and DCOCTL.
//  Description: Buffer ACLK on P1.0, default SMCLK(DCO) on P1.4 and MCLK/10 on
//  P1.1. DCO is software selectable to 1, 8, 12, or 16Mhz using calibration
//  contstants in INFOA.
//
//  ACLK = LFXT1 = 32768, MCLK = SMCLK = Selectable at 1, 8, 12 or 16Mhz
//  //* External watch crystal installed on XIN XOUT is required for ACLK *//	
//	//* By default, the MSP430 uses XT1 to source ACLK; P2.6/7 configured 
//  //* automatically.
//               MSP430G2xx3
//             -----------------
//         /|\|         P2.6/XIN|-
//          | |                 | 32kHz
//          --|RST     P2.7/XOUT|-
//            |                 |
//            |       P1.4/SMCLK|-->SMCLK = Default DCO
//            |             P1.1|-->MCLK/10 = DCO/10
//            |        P1.0/ACLK|-->ACLK = 32kHz
//  D. Dang
//  Texas Instruments Inc.
//  December 2010
//   Built with IAR Embedded Workbench Version: 3.42A
//******************************************************************************

#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW +WDTHOLD;                  // Stop Watchdog Timer

 //1Mhz
  if (CALBC1_1MHZ==0xFF)					// If calibration constant erased
  {											
    while(1);                               // do not load, trap CPU!!	
  } 
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                    // Set range
  DCOCTL = CALDCO_1MHZ;                     // Set DCO step + modulation */

/* //8Mhz
  if (CALBC1_8MHZ==0xFF)					// If calibration constant erased
  {											
    while(1);                               // do not load, trap CPU!!	
  }
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_8MHZ;                    // Set range
  DCOCTL = CALDCO_8MHZ;                     // Set DCO step + modulation */

/* //12Mhz
  if (CALBC1_12MHZ==0xFF)					// If calibration constant erased
  {											
    while(1);                               // do not load, trap CPU!!	
  }
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_12MHZ;                   // Set range
  DCOCTL = CALDCO_12MHZ;                    // Set DCO step + modulation*/

/* //16Mhz
  if (CALBC1_16MHZ==0xFF)					// If calibration constant erased
  {											
    while(1);                               // do not load, trap CPU!!	
  }
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_16MHZ;                   // Set range
  DCOCTL = CALDCO_16MHZ;                    // Set DCO step + modulation*/

  P1DIR |= 0x13;                            // P1.0,1 and P1.4 outputs
  P1SEL |= 0x11;                            // P1.0,4 ACLK, SMCLK output

  while(1)
  {
    P1OUT |= 0x02;    	                    // P1.1 = 1
    P1OUT &= ~0x02;                         // P1.1 = 0
  }
}

