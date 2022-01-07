#include <msp430.h>

int main(void)
{
//  BCSCTL1 |= DIVA_1;                        // ACLK/2
  BCSCTL3 |= LFXT1S_2;                      // ACLK = VLO
  WDTCTL = WDT_ADLY_1_9;                   // Interval timer
  IE1 |= WDTIE;                             // Enable WDT interrupt

  // Rows are P1.0 through P1.6
  // Columns are P2.0 through p2.4

  P1DIR = 0x7F;                             // Set rows as output
  P2DIR = 0x1F;                             // Set columns as output
  P1OUT = 0;                                // All P1.x reset
  P2OUT = 0x1F;

  // Based on schematic, to light up, a column has to be zero and a row 1.
  // BUT, it appears that PCB is backwards. So

  while(1)
  {
    P1OUT |= 0x7F;                          // Set All LEDs off
    __bis_SR_register(LPM3_bits + GIE);     // Enter LPM3
    P1OUT |= 0x7F;                          // Set All LEDs off
    __bis_SR_register(LPM3_bits + GIE);     // Enter LPM3
    P1OUT |= 0x7F;                          // Set All LEDs off
    __bis_SR_register(LPM3_bits + GIE);     // Enter LPM3
    P1OUT |= 0x7F;                          // Set All LEDs off
    __bis_SR_register(LPM3_bits + GIE);     // Enter LPM3
    P1OUT |= 0x7F;                          // Set All LEDs off
    __bis_SR_register(LPM3_bits + GIE);     // Enter LPM3
    P1OUT &= ~0x7F;                         // Reset All LEDs on
    __bis_SR_register(LPM3_bits + GIE);     // Enter LPM3
  }
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
