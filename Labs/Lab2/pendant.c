#include <msp430.h>

#define INSIDE 0
#define OUTSIDE 1

/* -------------------------------------------- *
   next_led(unsigned char *v1, unsigned char *v2, int inside)

Arguments:
    - *v1 and *v2 are pointers to the values for port 1 and port 2
        the function updates them to the next proper value in sequence
        
    - inside - INSIDE or OUTSIDE


 * -------------------------------------------- */

void next_led(unsigned char *v1, unsigned char *v2, int inside)
{
  static unsigned int led = 0x00;
  static unsigned int first = 1;

  // CODE

  led = 0x3F;

  *v1 = ~led;

  if (first) {
    *v2 ^= 0x36;
    first = 1;
  }
  else {
    first = 0;
  }
  
  return;
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
  P1DIR = 0x3F;                            // ?
  P2DIR = 0x36;                            // ?
  

  int inside = 0;
  unsigned char p1, p2;
  int i;

  p1 = 0x3F; // For my LEDs, this is the "OFF" condition. If yours are backwards, it will be different!
  p2 = 0x14; //? (what does changing this do to the default pattern?)


  P1OUT = (p1 & 0x3F);
  P2OUT = (p2 & 0x36);
  
  while(1) {
    for (i=0; i < 12; i++) {             // ?
      __bis_SR_register(LPM0_bits + GIE); // ?

      next_led(&p1, &p2, inside);
      P1OUT = (p1 & 0x3F);
      P2OUT = (p2 & 0x36);
    }
    inside = !inside;
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
