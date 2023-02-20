/*
 * Numbers.c
 *
 */
#include <msp430.h>


char font[10][3] = {
   {0x1F, 0x11, 0x1F},
   {0x00, 0x00, 0x1F},
   {0x1D, 0x15, 0x17},
   {0x11, 0x15, 0x1F},
   {0x07, 0x04, 0x1F},
   {0x17, 0x15, 0x1D},
   {0x1F, 0x14, 0x1C},
   {0x01, 0x01, 0x1F},
   {0x1B, 0x15, 0x1B},
   {0x07, 0x05, 0x1F},
};

int col = 0x01; // start with the first column
int col_idx = 0;
volatile unsigned int left_num = 0;
volatile unsigned int right_num = 0;


#define ALL_COLUMNS 0x7F
#define ALL_ROWS 0x1F

void initialize_display() {
    TA1CTL = TASSEL_1 + MC_1;     // ACLK, upmode
    TA1CCR0 = 10;                 // Count to 10 => Interrupt should happen every ~12 kHz / 10
    TA1CCTL0 = CCIE;              // CCR0 interrupt enabled
    TA1CCR1 = 9;                 // Brightness
    TA1CCTL1 = CCIE;              // CCR1 interrupt enabled

    // Columns are P1.0 through P1.6
    // Rows are P2.0 through p2.4

    P1DIR |= ALL_COLUMNS;                          // Set columns as output
    P2DIR |= ALL_ROWS;                          // Set rows as output

    // Based on schematic, to light up, a row has to be high (1) and a column low (0).
    P1OUT &= ~ALL_COLUMNS;                        // Default rows to off (low)
    P2OUT |= ALL_ROWS;                 // Default columns to on (high)

}

void display_number(int number) {
    left_num = number / 10;
    right_num = number - left_num * 10;
}


// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A1_0 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
    P1OUT |= ALL_COLUMNS;
    P1OUT &= (~col);
    P2OUT &= ~ALL_ROWS; // space (turn off LEDs)
    if (col_idx < 3)
        P2OUT += font[left_num][col_idx];
    else if (col_idx > 3)
        P2OUT += font[right_num][col_idx-4];

    col_idx = col_idx + 1;
    col = col >> 1; // move to the next column

    if (col_idx > 6) {
        col = 0x40;
        col_idx = 0;
    }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A1_VECTOR
__interrupt void Timer_A1_Multiplexed (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A1_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
    if (TA1IV == 2)
        P2OUT &= ~ALL_ROWS; // turn off LEDs

}
