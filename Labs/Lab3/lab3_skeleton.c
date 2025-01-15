
#include <msp430.h>				

// For pixels to be turned on, the column value has
//   to be 1, and the row has to be zero. So a blank
//   display looks like this:
char blank_screen[5] = { 0x7F, 0x7F, 0x7F, 0x7F, 0x7F};


// TODO!!!!
// You need to define your font for the numbers 0-9
char zero[5], one[5], two[5], three[5], four[5], five[5],
     six[5], seven[5], eight[5], nine[5];

char *numbers[10];

char *current_character;

int current_column = 0;

// TODO!!!!
void refresh(void) {
    // We'll write code in class to implement this function
    // Iterate through columns
    // P2OUT = 0x01 << current_column;
    // P1OUT = current_character[current_column++];

    // NOTE - outputs are inverted. You could make your life easier by doing the inversion here!

    // wrap current column appropriately
}

void update_display(int value) {
    current_character = numbers[value];
}


void main(void)
{
    BCSCTL3 |= LFXT1S_2;         // ACLK = VLO
    WDTCTL = WDTPW | WDTHOLD;   // Stop WDT


    // Configure Timer A0 to give us an interrupt being driven by ACLK
    TA0CTL = TASSEL_1 + MC_1;  // ACLK, upmode
    // TODO!!!!
    TA0CCR0 = 1000;     // You'll need to set this to achieve 1 second interrupts!
    TA0CCTL0 = CCIE;    // CCR0 interrupt enabled

    TA1CTL = TASSEL_1 + MC_1;  // ACLK, upmode
//     TODO!!!!
    TA1CCR0 = 60;     // You'll need to set this to for refresh!
    TA1CCTL0 = CCIE;    // CCR0 interrupt enabled

    P1DIR |= 0x7F;	// configure P1 as output
    P2DIR |= 0x1F;  // configure P2 as output

    P1OUT = 0x7F;   // Rows will default to high
    P2OUT = 0x1F;   // Columns will default to high

    numbers[0] = zero;
    numbers[1] = one;
    numbers[2] = two;
    numbers[3] = three;
    numbers[4] = four;
    numbers[5] = five;
    numbers[6] = six;
    numbers[7] = seven;
    numbers[8] = eight;
    numbers[9] = nine;

    current_character = numbers[0];

	int value = 0;

	__enable_interrupt();   // equivalent to __bis_SR_register(GIE)

	while(1)
	{
	    // This loop executes once per second as controlled by TA0
            update_display(value);
            value = value + 1;
            if (value == 10)
                value = 0;

	    __low_power_mode_3() // equivalent to __bis_SR_register(LPM3_bits);     // Enter LPM3

	}
}


// Timer 1 interrupt service routine (refresh)
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
    refresh();
}

// Timer 0 interrupt service routine (count)
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
    __low_power_mode_off_on_exit(); // equivalent to __bic_SR_register_on_exit(LPM3_bits);
}





