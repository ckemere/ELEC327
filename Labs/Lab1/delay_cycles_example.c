/*
 * ELEC 327
 * Lab 2 skeleton code.
 *
 * In general, you should not use __delay_cycles, as it locks/uses the CPU
 * during this time, and can't be used to compute anything else (while consuming
 * power).
 *
 * However, in Lab 2, the goal is for you to focus on the concept of the basic
 * non-terminating architecture of all embedded systems - there is some setup code,
 * and then the CPU essentially runs in an infinite loop.
 *
 */

#include "msp430fr2433.h" //must include so compiler knows what each variable means

void main(void){
    WDTCTL = WDTPW | WDTHOLD;   // Stop WDT

    __bis_SR_register(SCG0);   // disable frequency-locked loop (FLL)
    CSCTL3 |= SELREF__REFOCLK; // Set REFO as FLL reference source
    CSCTL0 = 0;                // Clear DCO and MOD registers
    CSCTL1 = DCORSEL_0;        // Set the DCO to 1 MHz
    CSCTL2 = 30;               // And load calibration data
    __delay_cycles(3);         // Wait 3 cycles to set
    __bic_SR_register(SCG0);   // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));         // Wait for FLL to lock

    PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode

    P1OUT &= BIT0; // ADD COMMENT
    P1DIR |= BIT0; // ADD COMMENT
    
    while (1) {
        __delay_cycles(500000);
        P1OUT ^= BIT0; // ADD COMMENT
    }
    
}
