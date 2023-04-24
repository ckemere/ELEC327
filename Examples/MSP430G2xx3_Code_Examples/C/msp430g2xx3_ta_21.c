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
//  MSP430G2xx3 Demo - Timer_A0, Normal Timer Mode Input Period and Dutycycle
//                     measurement
//  Description: This code example implements input capture in single capture
//  mode using TimerA in normal timer mode. TA1.1 is configured to output PWM
//  of 25% dutycycle, that is used as capture input at TA0.1. TA0.1 is
//  configured as timer input capture that is triggered by both the rising and
//  the falling edges. An external connection between TA1.1 and TA0.1 is
//  required in this example. Rising and Falling edges are captured and the
//  dutycycle is computed. If the measured dutycycle is != 25%, then LED on
//  P1.0 is set.
//
//  ACLK = LFXT1 = 32kHz crystal ; SMCLK = MCLK = 8 MHz;
//
//                 MSP430F51x2
//             -----------------
//         /|\|              XIN|-
//          | |                 | 32kHz
//          --|RST          XOUT|-
//            |                 |
//            |       P1.2/TA0.1|<-- CCI1A <-|
//            |       P2.1/TA1.1|--> CCR1 -->|
//            |                 |
//            |             P1.0|--> LED "ON" when dutycycle != 25%
//            |                 |

//
//  W. Goh
//  Texas Instruments Inc.
//  March 2012
//  Built with CCS v5.2 and IAR Embedded Workbench Version: 5.40.3
//******************************************************************************
#include <msp430.h>

unsigned char Count, First_Time;
unsigned int REdge1, REdge2, FEdge;

int main(void)
{
  unsigned int Period, ON_Period;
  unsigned char DutyCycle;

  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer

//  P1SEL |= BIT0;
  P1DIR |= BIT0;                            // P1.0/LED Output
  P1OUT &= ~BIT0;                           // LED off
  if (CALBC1_8MHZ==0xFF)					// If calibration constant erased
  {											
    while(1);                               // do not load, trap CPU!!	
  }
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_8MHZ;                    // Set DCO to 8MHz
  DCOCTL = CALDCO_8MHZ;

  // Configure Port Pins
  P2DIR |= BIT1;                            // P2.1/TA1.1 Output
  P2SEL |= BIT1;                            // TA1.1 Option select
  P1DIR &= ~BIT2;                           // P1.1/TA0.1 Input Capture
  P1SEL |= BIT2;                            // TA0.1 option select

  // Configure TA1.1 to output PWM signal
  // Period = 82/32khz = 2.5ms ~ 400Hz Freq
  TA1CCR0 = 82-1;                          // Period Register
  TA1CCR1 = 21;                            // TA1.1 25% dutycycle
  TA1CCTL1 |= OUTMOD_7;                    // TA1CCR1, Reset/Set
  TA1CTL = TASSEL_1 + MC_1 + TACLR;        // ACLK, upmode, clear TAR

  // Configure the TA0CCR1 to do input capture
  TA0CCTL1 = CAP + CM_3 + CCIE + SCS + CCIS_0;
                                            // TA0CCR1 Capture mode; CCI1A; Both
                                            // Rising and Falling Edge; interrupt enable
  TA0CTL |= TASSEL_2 + MC_2 + TACLR;        // SMCLK, Cont Mode; start timer

  // Variable Initialization
  Count = 0x0;
  First_Time = 0x01;

  while(1)
  {
      __bis_SR_register(LPM0_bits + GIE);   // Enter LPM0
      __no_operation();                     // For debugger
      // On exiting LPM0
      if (TA0CCTL1 & COV)                   // Check for Capture Overflow
          while(1);                         // Loop Forever

      Period = REdge2 - REdge1;             // Calculate Period
      ON_Period = FEdge-REdge1;             // On period
      DutyCycle = ((unsigned long)ON_Period*100/Period);
      if(DutyCycle!= 25)
      {
          P1OUT |= BIT0;
      }
      else
      {
          P1OUT &= ~BIT0;
      }
  }
}

// TA0_A1 Interrupt vector
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A1_VECTOR))) TIMER0_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(TA0IV,0x0A))
  {
      case  TA0IV_NONE: break;              // Vector  0:  No interrupt
      case  TA0IV_TACCR1:                   // Vector  2:  TACCR1 CCIFG
        if (TA0CCTL1 & CCI)                 // Capture Input Pin Status
        {
            // Rising Edge was captured
            if (!Count)
            {
                REdge1 = TA0CCR1;
                Count++;
            }
            else
            {
                REdge2 = TA0CCR1;
                Count=0x0;
                __bic_SR_register_on_exit(LPM0_bits + GIE);  // Exit LPM0 on return to main
            }

            if (First_Time)
                First_Time = 0x0;
        }
        else
        {
            // Falling Edge was captured
            if(!First_Time)
            {
                FEdge = TA0CCR1;
            }
        }
        break;
      case TA0IV_TACCR2: break;             // Vector  4:  TACCR2 CCIFG
      case TA0IV_6: break;                  // Vector  6:  Reserved CCIFG
      case TA0IV_8: break;                  // Vector  8:  Reserved CCIFG
      case TA0IV_TAIFG: break;              // Vector 10:  TAIFG
      default: 	break;
  }
}

