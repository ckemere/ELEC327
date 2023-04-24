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
//  MSP430G2xx3 Demo - 4-button Capacitive Touch, Pin Oscillator Method, ACLK CCR 
//
//  Description: Basic 4-button input using the built-in pin oscillation feature
//  on GPIO input structure. PinOsc signal feed into TA0CLK. ACLK [32kHz] is fed 
//  into the CCR1 register, triggering the CCR flag in timer capture mode. 
//  Difference in measurements indicate button touch. LEDs flash according to the
//  input touched:
//
//  Input 1: LED1 (LED2 off)
//  Input 2: LED2 (LED1 off)
//  Input 3: Both LEDs on
//  Input 4: Both LEDs flash on/off
//
//  ACLK = LFXT1 = 32768Hz, MCLK = SMCLK = 1MHz DCO
//
//               MSP430G2xx3
//             -----------------
//         /|\|              XIN|-
//          | |                 |  32kHz xtal
//          --|RST          XOUT|-
//            |                 |
//            |             P2.1|<--Capacitive Touch Input 1
//            |                 |
//  LED 2  <--|P1.6         P2.2|<--Capacitive Touch Input 2
//            |                 |
//  LED 1  <--|P1.0         P2.4|<--Capacitive Touch Input 3
//            |                 |
//            |             P2.5|<--Capacitive Touch Input 4
//
//   Brandon Elliott/D. Dang
//  Texas Instruments Inc.
//  November 2010
//   Built with IAR Embedded Workbench Version: 5.10
//******************************************************************************


#include <msp430.h>

/* Sensor settings*/
#define NUM_SEN     4                       // Defines number of sensors
#define KEY_LVL     1000                     // Defines threshold for a key press
/*Set to ~ half the max delta expected*/

#define LED_1   (0x01)                      // P1.0 LED output
#define LED_2   (0x40)                      // P1.6 LED output

// Global variables for sensing
unsigned int base_cnt[NUM_SEN];
unsigned int meas_cnt[NUM_SEN];
int delta_cnt[NUM_SEN];
unsigned char key_press[NUM_SEN];
char key_pressed;
int cycles;
const unsigned char electrode_bit[NUM_SEN]={BIT1, BIT2, BIT4, BIT5};
/* System Routines*/
void measure_count(void);                   // Measures each capacitive sensor
void pulse_LED(void);                       // LED gradient routine

/* Main Function*/
int main(void)
{ 
  unsigned int i,j;
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  if (CALBC1_1MHZ==0xFF)					// If calibration constant erased
  {											
    while(1);                               // do not load, trap CPU!!	
  }
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO to 1, 8, 12 or 16MHz
  DCOCTL = CALDCO_1MHZ; 
  BCSCTL2 |= DIVS_2;                        // SMCLK/(0:1,1:2,2:4,3:8)
  BCSCTL1 |= DIVA_1;                        // ACLK/(0:1,1:2,2:4,3:8)
  BCSCTL3 |= XCAP_1;                        // Configure Load Caps
  IE1 |= WDTIE;                             // enable WDT interrupt
  P1DIR = LED_1 + LED_2;                    // P1.0 & P1.6 = LEDs
  P1OUT = 0x00;                             

 do
 {
    IFG1 &= ~OFIFG;                         // Clear OSCFault flag
    for (i = 0xFF; i > 0; i--);             // Time for flag to set
  }
  while (IFG1 & OFIFG);                     // OSCFault flag still set?
  
  
   __bis_SR_register(GIE);                  // Enable interrupts

  measure_count();                          // Establish baseline capacitance
  for (i = 0; i<NUM_SEN; i++)
    base_cnt[i] = meas_cnt[i];

  for(i=15; i>0; i--)                       // Repeat and avg base measurement
  { 
    measure_count();
    for (j = 0; j<NUM_SEN; j++)
      base_cnt[j] = (meas_cnt[j]+base_cnt[j])/2;
  }
  

  /* Main loop starts here*/
  while (1)
  {
    j = KEY_LVL;
    key_pressed = 0;                        // Assume no keys are pressed

    measure_count();                        // Measure all sensors

    for (i = 0; i<NUM_SEN; i++)
    { 
      delta_cnt[i] = base_cnt[i] - meas_cnt[i];  // Calculate delta: c_change

      /* Handle baseline measurment for a base C decrease*/
      if (delta_cnt[i] < 0)                 // If negative: result increased
      {                                     // beyond baseline, i.e. cap dec
          base_cnt[i] = (base_cnt[i]+meas_cnt[i]) >> 1; // Re-average quickly
          delta_cnt[i] = 0;                 // Zero out for pos determination
      }
      if (delta_cnt[i] > j)                 // Determine if each key is pressed 
      {                                     // per a preset threshold
        key_press[i] = 1;                   // Specific key pressed
        j = delta_cnt[i];
        key_pressed = i+1;                  // key pressed
      }
      else
        key_press[i] = 0;
    }

    /* Delay to next sample, sample more slowly if no keys are pressed*/
    if (key_pressed)
    {
      BCSCTL1 = (BCSCTL1 & 0x0CF) + DIVA_0; // ACLK/(0:1,1:2,2:4,3:8)
      cycles = 20;
    }
    else
    {
      cycles--;
      if (cycles > 0)
        BCSCTL1 = (BCSCTL1 & 0x0CF) + DIVA_0; // ACLK/(0:1,1:2,2:4,3:8)
      else
      {
        BCSCTL1 = (BCSCTL1 & 0x0CF) + DIVA_3; // ACLK/(0:1,1:2,2:4,3:8)
        cycles = 0;
      }
    }

    /* Handle baseline measurment for a base C increase*/
    if (!key_pressed)                       // Only adjust baseline down 
    {                                       // if no keys are touched
      for (i = 0; i<NUM_SEN; i++)
        base_cnt[i] = base_cnt[i] - 1;      // Adjust baseline down, should be 
    }                                       // slow to accomodate for genuine 
     pulse_LED();                           // changes in sensor C
   
    __delay_cycles(20000);
  }
}                                           // End Main

/* Measure count result (capacitance) of each sensor*/
/* Routine setup for four sensors, not dependent on NUM_SEN value!*/

void measure_count(void)
{        
  unsigned int i, j;
  __disable_interrupt();                // Disable interrupts                                       
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = (BCSCTL1 & 0x0CF) + DIVA_3; // ACLK/(0:1,1:2,2:4,3:8)                     
  for (i = 0; i<NUM_SEN; i++)
  {  	
    // Configure Ports for relaxation oscillator
    P2DIR &= ~ electrode_bit[i];        // 
    P2SEL &= ~ electrode_bit[i];        //  
    P2SEL2 |= electrode_bit[i];         // Set target Pin Oscillator    
    TA0CTL = TASSEL_3 + MC_2 + TACLR;   // PinOsc Clock source, cont mode
    TA0CCTL0 = CM_1 + CCIS_1 + CAP;     // Capture on Pos Edges, ACLK, Cap, Interrupt
    TA0CCTL0 |= CCIE;                   // Enable Interrupt        
    
    __bis_SR_register(LPM3_bits+GIE);   // Wait for TIMER interrupt
    __bis_SR_register(LPM3_bits+GIE);   // Wait for TIMER interrupt
    meas_cnt[i] = TACCR0;               // Save result
    for (j=0;j<15;j++) {
      __bis_SR_register(LPM3_bits+GIE); // Wait for TIMER interrupt
    }
    TA0CTL &= MC_2;                     // Halt Timer    
    TA0CCTL0 &= ~CCIE;                  // Disable Interrupt       
    meas_cnt[i] = TACCR0 - meas_cnt[i]; // Save Measured        
    P2SEL2 &= ~electrode_bit[i];        // Clear target Pin Oscillator
  }  
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = (BCSCTL1 & 0x0CF) + DIVA_0; // ACLK/(0:1,1:2,2:4,3:8)  
}


void pulse_LED(void)
{ 
  switch (key_pressed){
  case 0: P1OUT &= ~(LED_1 + LED_2);
          break;
  case 1: P1OUT = LED_1;
          break;
  case 2: P1OUT = LED_2;
          break;
  case 3: P1OUT = LED_1 + LED_2;
          break;
  case 4: P1OUT ^= LED_1 + LED_2;
          break;
          
    }
}
/* Timer A1 interrupt service routine*/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A0 (void)
#else
#error Compiler not supported!
#endif
{
   __bic_SR_register_on_exit(LPM3_bits);     // Exit LPM3 on reti
}
