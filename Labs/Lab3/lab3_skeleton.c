/*
 * Minimum code needed to create a timer-based interrupt. The following code is
 * equivalent to (after all setup code):
 *
 * while(1){/Users/ckemere/Development/elec327.github.io/lab3/lab3_skeleton.c
 *   __delay_cycles(1000);
 *   i++;
 * }
 *
 * You should not, however, use __delay_cycles, as it locks the CPU to the cycles
 * during this time, and can't be used to compute anything else.
 *
 */

#include <msp430g2553.h>		//must include so compiler knows what each variable means

void main(void){
	WDTCTL = WDTPW + WDTHOLD;	//Stop WDT
	CCTL0 = CCIE;				//Puts the timer control on CCR0
	CCR0 = 1000;				//1000 cycles until first interrupt
	TACTL = TASSEL_2 + MC_2;	//right click and "Open Declaration" to find out about each variable
								//(adding a specific setting applies it to that control variable)

	__enable_interrupt();		//global interrupt enable
}

#pragma vector = TIMER0_A0_VECTOR	//says that the interrupt that follows will use the "TIMER0_A0_VECTOR" interrupt
__interrupt void Timer_A(void){		//can name the actual function anything
	CCR0 += 1000;
}
