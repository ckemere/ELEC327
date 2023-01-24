#include <msp430.h> 



int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

#ifdef __MSP430FR2XX_4XX_FAMILY__
    // Code needed for FR2433
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings  P1DIR |= 0x01;                            // P1.0 output
#endif

    TA0CCTL0 = CCIE;                        // TA0CCR0 interrupt enabled
    TA0CCR0 = 32768;
    TA0CTL = TASSEL_1 + MC_1;               // ACLK, up-mode  - Note! Default ACLK differs by model!


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
