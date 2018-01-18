#include <msp430.h>

#define CLOCKWISE 0
#define COUNTERCLOCKWISE 1

/* -------------------------------------------- *
   next_led(int direction)

Arguments:
    - direction - CLOCKWISE or COUNTERCLOCKWISE

Returns: the next LED in sequence in the given direction
    - unsigned int - lower byte maps to Port 2 pins
        and upper byte maps to Port 1 pins.

 * -------------------------------------------- */

unsigned int next_led(int direction)
{
  static unsigned int led = 0x00;

  // CODE
  led = led ^ 0xC0FF; // replace this line!!

  return led;
}

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                    // Set range
  DCOCTL = CALDCO_1MHZ;                     // Set DCO step + modulation */
  BCSCTL2 |= DIVS_3;                        // Divide MCLK by 8

  CCTL0 = CCIE;                             // ?
  CCR0 = 0x1458;                            // ?
  TACTL = TASSEL_2 + MC_1 + ID_3;           // ?


  // ------ ???
  P1DIR = 0xC0;                            // ?
  P2DIR = ;                                // ?
  P1OUT = 0;
  P2OUT = 0;


  // ------ ???
  P1IE  |= BIT2;     // ?
  P1IES |= BIT2;     // ?
  P1IFG &= ~BIT2;    // ?


  int value;
  int p1, p2;
  int direction = CLOCKWISE;
  int i;

  while(1) {
    for (i=0; i < 8*3; i++) {             // ?
      __bis_SR_register(LPM0_bits + GIE); // ?

      P1OUT &= ~0xC0;                     // ?
      P2OUT &= ;                          // ?
      value = next_led(direction);
      p1 = (value & 0xFF00) >> 8;         // ?
      p2 = value & 0x00FF;                // ?
      P1OUT |= p1;
      P2OUT |= p2;

    }
    if (direction == CLOCKWISE)
      direction = COUNTERCLOCKWISE;
    else direction = CLOCKWISE;
  }
}

// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
  __bic_SR_register_on_exit(LPM0_bits);  // ?
}


int asleep = 0;

// Port 1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void Port_1 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
#else
#error Compiler not supported!
#endif
{
  P1IE  &= ~BIT2;   // ?
  P1IFG &= ~BIT2;   // ? (what and why?)
  if (asleep) {
    asleep = 0;
    P1IE  |= BIT2;
    __bic_SR_register_on_exit(LPM4_bits);  // ?
  }
  else {
    P1OUT &= ~0xC0;  // ?
    P2OUT &= ~0xFF;  // ?
    asleep = 1;
    P1IE  |= BIT2;
    __bis_SR_register_on_exit(LPM4_bits + GIE);
  }
}
