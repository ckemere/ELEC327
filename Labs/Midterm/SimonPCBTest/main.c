#include <msp430.h>

void button_setup(void) {
    /* BUTTON LAYOUT */
    /* (P2.2) S2 --------- S3 (P2.3) */
    /*         |            |        */
    /*         |            |        */
    /* (P2.0) S1 --------- S4 (P2.4) */



    // ------ Setup pins for P1 and P2 as outputs (For the LEDS) and clear them
    P2DIR &= ~(BIT0 + BIT2 + BIT3 + BIT4);                           // Make P2.0, 2.2, 2.3, and 2.4 input pins
    P2REN |= (BIT0 + BIT2 + BIT3 + BIT4);                            // Pull-up
    P2OUT |= (BIT0 + BIT2 + BIT3 + BIT4);                            // Init P2 pins to 1 (Pull up)
}

void buzzer_setup(void) {
    P2DIR |= BIT1 + BIT5;                         // Make P2.1, 2.5 output pins (Piezo)
    P2OUT &= ~(BIT1 + BIT5);                      // set to 0 output (ground)    
}

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


void wdt_disable(void) {
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
}



void main() {

    // LED Code
    int i;
    int currentOff = 0;
    int counter = 0;

    char leds[] = {0x00, 0x00, 0x00, 0x00,
                   0xFF, 0xFF, 0x00, 0x00, // blue
                   0xFF, 0x00, 0xFF, 0x00, // green
                   0xFF, 0x00, 0x00, 0xFF, // red
                   0xFF, 0x70, 0x00, 0x70, // dimmer magenta
                   0x00, 0x00, 0x00, 0x00};

    int beep = 0;
    
    button_setup();
    buzzer_setup();
    SPI_setup();
    wdt_disable();

    
    while(1) {
      beep = 1;
      if ((P2IN & 0x01) == 0) {
        leds[4] = 0xE0;
        beep = 0;
      }
      else {
        leds[4] = 0xFF;
      }
      if ((P2IN & 0x04) == 0) {
        leds[8] = 0xE0;
        beep = 0;
      }
      else {
        leds[8] = 0xFF;
      }
      if ((P2IN & 0x08) == 0) {
        leds[12] = 0xE0;
        beep = 0;
      }
      else {
        leds[12] = 0xFF;
      }
      if ((P2IN & 0x10) == 0) {
        leds[16] = 0xE0;
        beep = 0;
      }
      else {
        leds[16] = 0xFF;
      }
      
      if ( beep == 1) { // Default on (1) or off (== 0)
        P2OUT |= BIT1;
        __delay_cycles(1000);
        P2OUT &= ~BIT1;
        __delay_cycles(1000);
      }
            
      for (i = 0; i <= sizeof(leds); i++) { // Send Color Frame
          while(!(IFG2 & UCA0TXIFG)); // Buffer ready?
              UCA0TXBUF = leds[i];
      }

    }
}
