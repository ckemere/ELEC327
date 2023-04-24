//******************************************************************************
//   MSP430FR243x Demo - eUSCI_B0, I2C Slave multiple byte TX/RX
//
//   Description: I2C master communicates to I2C slave sending and receiving
//   3 different messages of different length. (This is the slave code). The
//   slave will be in LPM0 mode, waiting for the master to initiate the
//   communication. The slave will send/receive bytes based on the master's
//   request. The slave will handle I2C bytes sent/received using the
//   I2C interrupt.
//   ACLK = NA, MCLK = SMCLK = DCO 16MHz.
//
//                                     /|\ /|\
//                   MSP430FR2633      4.7k |
//                 -----------------    |  4.7k
//            /|\ |             P1.3|---+---|-- I2C Clock (UCB0SCL)
//             |  |                 |       |
//             ---|RST          P1.2|-------+-- I2C Data (UCB0SDA)
//                |                 |
//                |                 |
//                |                 |
//                |                 |
//                |                 |
//                |                 |
//
//   Nima Eskandari and Ryan Meredith
//   Texas Instruments Inc.
//   November 2017
//   Built with CCS V7.3
//******************************************************************************

#include <msp430.h>
#include <stdint.h>

//******************************************************************************
// Example Commands ************************************************************
//******************************************************************************

#define LED_OUT     P1OUT
#define LED_DIR     P1DIR
#define LED0_PIN    BIT0
#define LED1_PIN    BIT1

#define SLAVE_ADDR  0x48

/* CMD_TYPE_X_SLAVE are example commands the master sends to the slave.
 * The slave will send example SlaveTypeX buffers in response.
 *
 * CMD_TYPE_X_MASTER are example commands the master sends to the slave.
 * The slave will initialize itself to receive MasterTypeX example buffers.
 * */

#define CMD_TYPE_0_SLAVE      0
#define CMD_TYPE_1_SLAVE      1
#define CMD_TYPE_2_SLAVE      2

#define CMD_TYPE_0_MASTER      3
#define CMD_TYPE_1_MASTER      4
#define CMD_TYPE_2_MASTER      5

#define TYPE_0_LENGTH   1
#define TYPE_1_LENGTH   2
#define TYPE_2_LENGTH   6

#define MAX_BUFFER_SIZE     20

/* MasterTypeX are example buffers initialized in the master, they will be
 * sent by the master to the slave.
 * SlaveTypeX are example buffers initialized in the slave, they will be
 * sent by the slave to the master.
 * */

uint8_t MasterType2 [TYPE_2_LENGTH] = {0};
uint8_t MasterType1 [TYPE_1_LENGTH] = { 0, 0};
uint8_t MasterType0 [TYPE_0_LENGTH] = { 0};

uint8_t SlaveType2 [TYPE_2_LENGTH] = {'A', 'B', 'C', 'D', '1', '2'};
uint8_t SlaveType1 [TYPE_1_LENGTH] = {15, 16};
uint8_t SlaveType0 [TYPE_0_LENGTH] = {12};

//******************************************************************************
// General I2C State Machine ***************************************************
//******************************************************************************

typedef enum I2C_ModeEnum{
    IDLE_MODE,
    NACK_MODE,
    TX_REG_ADDRESS_MODE,
    RX_REG_ADDRESS_MODE,
    TX_DATA_MODE,
    RX_DATA_MODE,
    SWITCH_TO_RX_MODE,
    SWITHC_TO_TX_MODE,
    TIMEOUT_MODE
} I2C_Mode;

/* Used to track the state of the software state machine*/
I2C_Mode SlaveMode = RX_REG_ADDRESS_MODE;

/* The Register Address/Command to use*/
uint8_t ReceiveRegAddr = 0;

/* ReceiveBuffer: Buffer used to receive data in the ISR
 * RXByteCtr: Number of bytes left to receive
 * ReceiveIndex: The index of the next byte to be received in ReceiveBuffer
 * TransmitBuffer: Buffer used to transmit data in the ISR
 * TXByteCtr: Number of bytes left to transfer
 * TransmitIndex: The index of the next byte to be transmitted in TransmitBuffer
 * */
uint8_t ReceiveBuffer[MAX_BUFFER_SIZE] = {0};
uint8_t RXByteCtr = 0;
uint8_t ReceiveIndex = 0;
uint8_t TransmitBuffer[MAX_BUFFER_SIZE] = {0};
uint8_t TXByteCtr = 0;
uint8_t TransmitIndex = 0;


/* Initialized the software state machine according to the received cmd
 *
 * cmd: The command/register address received
 * */
void I2C_Slave_ProcessCMD(uint8_t cmd);

/* The transaction between the slave and master is completed. Uses cmd
 * to do post transaction operations. (Place data from ReceiveBuffer
 * to the corresponding buffer based in the last received cmd)
 *
 * cmd: The command/register address corresponding to the completed
 * transaction
 */
void I2C_Slave_TransactionDone(uint8_t cmd);
void CopyArray(uint8_t *source, uint8_t *dest, uint8_t count);

void I2C_Slave_ProcessCMD(uint8_t cmd)
{
    ReceiveIndex = 0;
    TransmitIndex = 0;
    RXByteCtr = 0;
    TXByteCtr = 0;

    switch (cmd)
    {
        case (CMD_TYPE_0_SLAVE):                        //Send slave device id (This device's id)
            SlaveMode = TX_DATA_MODE;
            TXByteCtr = TYPE_0_LENGTH;
            //Fill out the TransmitBuffer
            CopyArray(SlaveType0, TransmitBuffer, TYPE_0_LENGTH);
            UCB0IE &= ~UCRXIE;                       // Disable RX interrupt
            UCB0IE |= UCTXIE;                        // Enable TX interrupt
            break;
        case (CMD_TYPE_1_SLAVE):                      //Send slave device time (This device's time)
            SlaveMode = TX_DATA_MODE;
            TXByteCtr = TYPE_1_LENGTH;
            //Fill out the TransmitBuffer
            CopyArray(SlaveType1, TransmitBuffer, TYPE_1_LENGTH);
            UCB0IE &= ~UCRXIE;                       // Disable RX interrupt
            UCB0IE |= UCTXIE;                        // Enable TX interrupt
            break;
        case (CMD_TYPE_2_SLAVE):                  //Send slave device location (This device's location)
            SlaveMode = TX_DATA_MODE;
            TXByteCtr = TYPE_2_LENGTH;
            //Fill out the TransmitBuffer
            CopyArray(SlaveType2, TransmitBuffer, TYPE_2_LENGTH);
            UCB0IE &= ~UCRXIE;                       // Disable RX interrupt
            UCB0IE |= UCTXIE;                        // Enable TX interrupt
            break;
        case (CMD_TYPE_0_MASTER):
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = TYPE_0_LENGTH;
            UCB0IE &= ~UCTXIE;                       // Disable RX interrupt
            UCB0IE |= UCRXIE;                        // Enable TX interrupt
            break;
        case (CMD_TYPE_1_MASTER):
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = TYPE_1_LENGTH;
            UCB0IE &= ~UCTXIE;                       // Disable RX interrupt
            UCB0IE |= UCRXIE;                        // Enable TX interrupt
            break;
        case (CMD_TYPE_2_MASTER):
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = TYPE_2_LENGTH;
            UCB0IE &= ~UCTXIE;                       // Disable RX interrupt
            UCB0IE |= UCRXIE;                        // Enable TX interrupt
            break;
        default:
            __no_operation();
            break;
    }
}


void I2C_Slave_TransactionDone(uint8_t cmd)
{
    switch (cmd)
    {
        case (CMD_TYPE_0_SLAVE):                        //Slave device id was sent(This device's id)
            break;
        case (CMD_TYPE_1_SLAVE):                      //Slave device time was sent(This device's time)
            break;
        case (CMD_TYPE_2_SLAVE):                  //Send slave device location (This device's location)
            break;
        case (CMD_TYPE_0_MASTER):
            CopyArray(ReceiveBuffer, MasterType0, TYPE_0_LENGTH);
            break;
        case (CMD_TYPE_1_MASTER):
            CopyArray(ReceiveBuffer, MasterType1, TYPE_1_LENGTH);
            break;
        case (CMD_TYPE_2_MASTER):
            CopyArray(ReceiveBuffer, MasterType2, TYPE_2_LENGTH);
            break;
        default:
            __no_operation();
            break;
    }
}

void CopyArray(uint8_t *source, uint8_t *dest, uint8_t count)
{
    uint8_t copyIndex = 0;
    for (copyIndex = 0; copyIndex < count; copyIndex++)
    {
        dest[copyIndex] = source[copyIndex];
    }
}


//******************************************************************************
// Device Initialization *******************************************************
//******************************************************************************

void initGPIO()
{
    // Configure GPIO
    LED_OUT &= ~(LED0_PIN | LED1_PIN); // P1 setup for LED & reset output
    LED_DIR |= (LED0_PIN | LED1_PIN);

    P1SEL0 |= BIT2 | BIT3;                    // I2C pins
    P1SEL1 &= ~(BIT2 | BIT3);

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;
}

void initI2C()
{
    UCB0CTLW0 = UCSWRST;                      // Software reset enabled
    UCB0CTLW0 |= UCMODE_3 | UCSYNC;           // I2C mode, sync mode
    UCB0I2COA0 = SLAVE_ADDR | UCOAEN;         // Own Address and enable
    UCB0CTLW0 &= ~UCSWRST;                    // clear reset register
    UCB0IE |= UCRXIE + UCSTPIE;
}

void initClockTo16MHz()
{
    // Configure one FRAM waitstate as required by the device datasheet for MCLK
    // operation beyond 8MHz _before_ configuring the clock system.
    FRCTL0 = FRCTLPW | NWAITS_1;

    __bis_SR_register(SCG0);    // disable FLL
    CSCTL3 |= SELREF__REFOCLK;  // Set REFO as FLL reference source
    CSCTL0 = 0;                 // clear DCO and MOD registers
    CSCTL1 &= ~(DCORSEL_7);     // Clear DCO frequency select bits first
    CSCTL1 |= DCORSEL_5;        // Set DCO = 16MHz
    CSCTL2 = FLLD_0 + 487;      // set to fDCOCLKDIV = (FLLN + 1)*(fFLLREFCLK/n)
                                //                   = (487 + 1)*(32.768 kHz/1)
                                //                   = 16 MHz

    __delay_cycles(3);
    __bic_SR_register(SCG0);                        // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));      // FLL locked

    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;
}


//******************************************************************************
// Main ************************************************************************
// Enters LPM0 and waits for I2C interrupts. The data sent from the master is  *
// then interpreted and the device will respond accordingly                    *
//******************************************************************************


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    initClockTo16MHz();
    initGPIO();
    initI2C();

    __bis_SR_register(LPM0_bits + GIE);
	return 0;
}

//******************************************************************************
// I2C Interrupt ***************************************************************
//******************************************************************************

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  //Must read from UCB0RXBUF
  uint8_t rx_val = 0;
  switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG))
  {
    case USCI_NONE:          break;         // Vector 0: No interrupts
    case USCI_I2C_UCALIFG:   break;         // Vector 2: ALIFG
    case USCI_I2C_UCNACKIFG:                // Vector 4: NACKIFG
      break;
    case USCI_I2C_UCSTTIFG:  break;         // Vector 6: STTIFG
    case USCI_I2C_UCSTPIFG:
        UCB0IFG &= ~(UCTXIFG0);
        break;         // Vector 8: STPIFG
    case USCI_I2C_UCRXIFG3:  break;         // Vector 10: RXIFG3
    case USCI_I2C_UCTXIFG3:  break;         // Vector 12: TXIFG3
    case USCI_I2C_UCRXIFG2:  break;         // Vector 14: RXIFG2
    case USCI_I2C_UCTXIFG2:  break;         // Vector 16: TXIFG2
    case USCI_I2C_UCRXIFG1:  break;         // Vector 18: RXIFG1
    case USCI_I2C_UCTXIFG1:  break;         // Vector 20: TXIFG1
    case USCI_I2C_UCRXIFG0:                 // Vector 22: RXIFG0
        rx_val = UCB0RXBUF;
        switch (SlaveMode)
        {
          case (RX_REG_ADDRESS_MODE):
              ReceiveRegAddr = rx_val;
              I2C_Slave_ProcessCMD(ReceiveRegAddr);
              break;
          case (RX_DATA_MODE):
              ReceiveBuffer[ReceiveIndex++] = rx_val;
              RXByteCtr--;
              if (RXByteCtr == 0)
              {
                  //Done Receiving MSG
                  SlaveMode = RX_REG_ADDRESS_MODE;
                  UCB0IE &= ~(UCTXIE);
                  UCB0IE |= UCRXIE;                          // Enable RX interrupt
                  I2C_Slave_TransactionDone(ReceiveRegAddr);
              }
              break;
          default:
              __no_operation();
              break;
        }
        break;
    case USCI_I2C_UCTXIFG0:                 // Vector 24: TXIFG0
        switch (SlaveMode)
        {
          case (TX_DATA_MODE):
              UCB0TXBUF = TransmitBuffer[TransmitIndex++];
              TXByteCtr--;
              if (TXByteCtr == 0)
              {
                  //Done Transmitting MSG
                  SlaveMode = RX_REG_ADDRESS_MODE;
                  UCB0IE &= ~(UCTXIE);
                  UCB0IE |= UCRXIE;                          // Enable RX interrupt
                  I2C_Slave_TransactionDone(ReceiveRegAddr);
              }
              break;
          default:
              __no_operation();
              break;
        }
        break;                      // Interrupt Vector: I2C Mode: UCTXIFG
    default: break;
  }
}
