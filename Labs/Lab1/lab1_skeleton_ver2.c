#include <msp430.h> 



int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings  P1DIR |= 0x01;                            // P1.0 output
    TA0CCTL0 = CCIE;                        // TA0CCR0 interrupt enabled
    TA0CCR0 = 0x2000;
    TA0CTL = TASSEL_2 + MC_1 + ID_3; // SMCLK, up-mode, Divide clock by 8

    P1DIR |= 0x01;                          // Set P1.0 to output direction

    __bis_SR_register(GIE);       // Enable interrupt
    while (1) {
        __bis_SR_register(LPM0_bits);       // Enter LPM0
        P1OUT ^= 0x01;                            // Toggle P1.0
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
