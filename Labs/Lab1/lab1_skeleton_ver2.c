
#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  P1DIR |= 0x01;                            // P1.0 output
  TA0CCTL0 = CCIE;                          // TA0CCR0 interrupt enabled
  TA0CCR0 = 0x100;
  TA0CTL = TASSEL_2 + MC_2; // SMCLK, contmode

  while (1) {
    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
    P1OUT ^= 0x01;                            // Toggle P1.0
    TA0CCR0 += 0x100;                         // Add Offset to TA0CCR0
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
  __bic_SR_register_on_exit(LPM0_bits);
}
