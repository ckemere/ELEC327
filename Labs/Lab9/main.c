#include <msp430.h>
#include <stdint.h>
#include "rainbow.h"


void SPI_setup(void) {
    UCA0CTL1 |= UCSWRST;                          // Enable SPI

    P1SEL  |= BIT2;                               // Just setup for the pins (1.2 is the SIMO data line)
    P1SEL2 |= BIT2;

    P1SEL  |= BIT4;                               // Just setup for the pins (1.4 is the USCI clock)
    P1SEL2 |= BIT4;

    UCA0CTL0 |= UCCKPH + UCSYNC + UCMST + UCMSB;  // Clock phase sync mode, Synchronous  (SPI), Master Mode on, MSB first direction for the RX/TX shifter register
    UCA0CTL1 |= UCSSEL_2;                         // Select SMCLK as source

    UCA0BR1 |= 0;
    UCA0BR0 |= 0x02;                              // Baud rate settings, clock divide by 2
    UCA0CTL1 &= ~UCSWRST;
}


void wdt_setup(void) {
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    WDTCTL = WDT_ADLY_250;                    // WDT 250ms, ACLK, interval timer
    IE1 |= WDTIE;                             // Enable WDT interrupt

}

uint8_t spi_array[] = {0x00, 0x00, 0x00, 0x00,
               0xEF, 0x70, 0x00, 0x00, // blue
               0xEF, 0x00, 0x70, 0x00, // green
               0xEF, 0x00, 0x00, 0x70, // red
               0xEF, 0x70, 0x00, 0x70, // dimmer magenta/yellow
               0x00, 0x00, 0x00, 0x00};


uint8_t led_history[] = {0xEF, 0x00, 0x00, 0x00,
                         0xEF, 0x00, 0x00, 0x00,
                         0xEF, 0x00, 0x00, 0x00,
                         0xEF, 0x00, 0x00, 0x00};
                        // Note that size is a nice power of 2!!!!

int rainbow_idx, history_idx;

void update_colors(int direction) {
    int i;
    uint8_t *lptr;

    rainbow_idx = rainbow_idx + 3*direction; // shift one LED

    // Check over or underflow of ptr
    if (rainbow_idx >= sizeof(rainbow_rbg)) // loop at the end if needed!
        rainbow_idx = rainbow_idx - sizeof(rainbow_rbg);
    if (rainbow_idx < 0)
        rainbow_idx = rainbow_idx + sizeof(rainbow_rbg);

    // Update circular buffer
    history_idx = (history_idx  - 4) & 0x000F; // go to next entry in circular buffer
            // Move backwards so that history makes sense

    for (i = 0; i < 3; i++) {
        led_history[history_idx + i + 1] = rainbow_rbg[rainbow_idx + i];
        // copy from rainbow array to LED history array skipping the first intensity entry
    }

    // Copy LED values from history circular buffer
    lptr = spi_array + 4; // point LED pointer at the beginning of the LED array
    for (i = 0; i < sizeof(led_history); i++) {
        *lptr++ = led_history[(history_idx + i) & 0x000F]; // note circular indexing
    }

}

void main() {

    // LED Code
    int i;

    BCSCTL3 |= LFXT1S_2;
    SPI_setup();
    wdt_setup();


    while(1) {

        update_colors(3); // Example - increase temperature in steps of 3

        for (i = 0; i <= sizeof(spi_array); i++) { // Send Color Frame
          while(!(IFG2 & UCA0TXIFG)); // Buffer ready?
              UCA0TXBUF = spi_array[i];
        }

        __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3 w/interrupt

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
    __bic_SR_register_on_exit(LPM3_bits);       // Enter LPM3 w/interrupt

}

