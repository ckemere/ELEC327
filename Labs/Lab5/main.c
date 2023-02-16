#include <msp430.h> 
#include "Numbers.h"

/**
 * main.c
 */
int second_flag = 0; // Timer ISR will use this to signal if a second has elapsed

#define SWITCH_1_MASK 0x20 // Port 2.5
#define SWITCH_2_MASK 0x80 // Port 1.7

int main(void)
{
    int num_to_display = 0;

    BCSCTL3 |= LFXT1S_2;                    // ACLK = VLO

    // Set up WDT interrupt for helpfulness
//	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    WDTCTL = WDT_ADLY_16;       // WDT interrupt
    IE1 |= WDTIE;                             // Enable WDT interrupt

    // Set up a timer for seconds!
    TA0CTL = TASSEL_1 + MC_1;     // ACLK, upmode
    TA0CCR0 = 12000;              // Interrupt should happen every ~12 kHz / 12000
    TA0CCTL0 = CCIE;              // CCR0 interrupt enabled

	// Switches are pins 2.5 and 1.7. They pull low when depressed.
    // SW1
    P2DIR &= ~SWITCH_1_MASK; // Set pin 2.5 to input
    P2REN = SWITCH_1_MASK;   // Set pull-up/pull-down resistor
    P2OUT |= SWITCH_1_MASK;  // Set resistor to be a pull-up

    // SW2
    P1DIR &= ~SWITCH_2_MASK;
    P1REN = SWITCH_2_MASK;
    P1OUT |= SWITCH_2_MASK;


	initialize_display();

	while (1) {
	  __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3 w/ interrupt
	  // We'll be woken up EITHER by WDT or by Timer0 at 1 second

	  // You will replace this with your own logic to process button
	  //    presses. You'll need some sort of state machine to keep
	  //    track of whether you're in (A) timing mode (counting down)
	  //    or (B) in setting mode. In setting mode, SW1 should increment
	  //    the display and SW2 should decrement it. In timing mode, the
	  //    display should count down to zero, and then stop at zero.
	  //    To enter and exit setting mode you should push both buttons at
	  //    the same time (and the mode change should happen after both
	  //    are released). In setting mode, holding down a button should
	  //    cause the numbers to continue to increment or decrement.
	  // ----------------------------------------------------------->

	  if (second_flag) { // Did we wake up because of 1 second tick?
	      second_flag = 0; // Reset flag

	      // Switches are active low, so this will be false if either switch is pressed
          if ((P1IN & SWITCH_2_MASK) && (P2IN & SWITCH_1_MASK)) {

            num_to_display = num_to_display + 1;
            if (num_to_display > 99) {
              num_to_display = 0;
            }
            display_number(num_to_display); // Change the display
         }
	  }

	  // <------------------------------------------------------------
	} // end of infinite loop
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
#else
#error Compiler not supported!
#endif
{
  __bic_SR_register_on_exit(LPM3_bits);     // Clear LPM3 bits from 0(SR)
}

// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0_0 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
    second_flag = 1; // This flag will signal to our main loop what woke us up!
    __bic_SR_register_on_exit(LPM3_bits);     // Clear LPM3 bits from 0(SR)
}
