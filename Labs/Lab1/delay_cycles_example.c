/*
 * ELEC 327
 * Lab 1 skeleton code.
 *
 * In general, you should not use __delay_cycles, as it locks/uses the CPU
 * during this time, and can't be used to compute anything else (while consuming
 * power).
 *
 * However, in Lab 1, the goal is for you to focus on the concept of the basic
 * non-terminating architecture of all embedded systems - there is some setup code,
 * and then the CPU essentially runs in an infinite loop.
 *
 */

#include "msp430g2553.h"		//must include so compiler knows what each variable means

void main(void){
    WDTCTL = WDTPW + WDTHOLD;	// Stop WDT
    
    if (CALBC1_1MHZ == 0xFF || CALDCO_1MHZ == 0xff)
        while(1); // Erased calibration data? Trap!
    
    BCSCTL1 = CALBC1_1MHZ; // Set the DCO to 1 MHz
    DCOCTL = CALDCO_1MHZ; // And load calibration data
    
    P1OUT &= BIT0; // ADD COMMENT
    P1DIR |= BIT0; // ADD COMMENT
    
    while (1) {
        __delay_cycles(500000);
        P1OUT ^= BIT0; // ADD COMMENT
    }
    
}
