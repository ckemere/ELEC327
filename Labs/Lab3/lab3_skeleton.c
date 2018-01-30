/* Based on msp430g2xx3_wdt_02.c from the TI Examples */

#include <msp430.h>

int main(void)
{
    WDTCTL = WDT_ADLY_250;                    // WDT 250ms, ACLK, interval timer
    IE1 |= WDTIE;                             // Enable WDT interrupt
    P1DIR |= BIT0;                            // Set P1.0 to output direction

    __bis_SR_register(GIE);                   // Enable interrupts
    while (1) {
        __bis_SR_register(LPM3_bits);   // Enter LPM3 w/interrupt
        P1OUT ^= BIT0;
    }
}

// Watchdog Timer interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
#else
#error Compiler not supported!
#endif
{
    __bic_SR_register_on_exit(LPM3_bits);
}
