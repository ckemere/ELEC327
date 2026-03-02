
#include "leds.h"
#include "delay.h"
#include <ti/devices/msp/msp.h>

// PUBLIC
// Flags for interface to main code
bool spi_wakeup;

// PRIVATE
// Data buffer and length for SPI TX - we'll load this with a function
uint16_t *spi_message;
int      spi_message_len;
int      spi_message_idx;
bool     spi_transmission_in_progress;


void InitializeLEDInterface(void) {
    // 1. Initialize GPIO IOMUX for SPI
    // Check if module needs full initialization
    if (GPIOA->GPRCM.STAT & GPIO_STAT_RESETSTKY_MASK) {
        // Sticky bit is set → module was reset (or never initialized)
        // Do full reset sequence and clear the sticky bit
        GPIOA->GPRCM.RSTCTL = (GPIO_RSTCTL_KEY_UNLOCK_W |
                                GPIO_RSTCTL_RESETSTKYCLR_CLR |
                                GPIO_RSTCTL_RESETASSERT_ASSERT);
        GPIOA->GPRCM.PWREN  = (GPIO_PWREN_KEY_UNLOCK_W |
                                GPIO_PWREN_ENABLE_ENABLE);
        delay_cycles(POWER_STARTUP_DELAY);
    } 

    // Initialize SPI0 connections!!
    IOMUX->SECCFG.PINCM[(IOMUX_PINCM22)] = IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM22_PF_SPI0_SCLK;  // SPI0_SCLK on PA11
    IOMUX->SECCFG.PINCM[(IOMUX_PINCM20)] = IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM20_PF_SPI0_PICO;  // SPI0_PCIO on PA9


    SPI0->GPRCM.RSTCTL = (SPI_RSTCTL_KEY_UNLOCK_W | SPI_RSTCTL_RESETSTKYCLR_CLR | SPI_RSTCTL_RESETASSERT_ASSERT);
    SPI0->GPRCM.PWREN = (SPI_PWREN_KEY_UNLOCK_W | SPI_PWREN_ENABLE_ENABLE);
    delay_cycles(POWER_STARTUP_DELAY); // delay to enable SPI to turn on and reset

    // Configure clocking for SPI0
    SPI0->CLKSEL = (uint32_t) SPI_CLKSEL_SYSCLK_SEL_ENABLE; // use the SYSOSC
    SPI0->CLKDIV = (uint32_t) SPI_CLKDIV_RATIO_DIV_BY_1; // actually 0x0, which is going to be default, but here for completeness

    // Configure the module
    SPI0->CTL0 = SPI_CTL0_SPO_HIGH | SPI_CTL0_SPH_SECOND | // Clock edges and phases for data
            SPI_CTL0_FRF_MOTOROLA_3WIRE |  // Don't use a chip select pin to bound frames
            SPI_CTL0_DSS_DSS_16;

    SPI0->CTL1 = SPI_CTL1_CP_ENABLE | // Microcontroller is CONTROLLER
            SPI_CTL1_PREN_DISABLE | SPI_CTL1_PTEN_DISABLE | // Disable parity on RX and TX
            SPI_CTL1_MSB_ENABLE; // Bit order is MSB first

    /* Configure Controller mode */
    /*
     * Set the bit rate clock divider to generate the serial output clock
     *     outputBitRate = (spiInputClock) / ((1 + SCR) * 2)
     *     2000000 = (32000000)/((1 + 7) * 2)
     */

    SPI0->CLKCTL = 7; // 10 bits

    // LOOK HERE!
    /* Set RX and TX FIFO threshold levels */
    SPI0->IFLS = SPI_IFLS_RXIFLSEL_LEVEL_1 | // Trigger an RX interrupt when FIFO contains >=1 sample (included for reference)
            SPI_IFLS_TXIFLSEL_LVL_EMPTY;     // Trigger an TX interrupt when the FIFO is empty

    /* Enable Transmit FIFO interrupt */
    SPI0->CPU_INT.IMASK |= SPI_CPU_INT_IMASK_TX_SET; // Only enable TX interrupt

    /* Enable module */
    SPI0->CTL1 |= SPI_CTL1_ENABLE_ENABLE;
}

bool SendSPIMessage(uint16_t *message_ptr, int message_len) {
    if (spi_transmission_in_progress) {
        return false;
    }
    else {
        spi_message = message_ptr;
        spi_message_len = message_len;
        spi_transmission_in_progress = true;
        spi_message_idx = 1; // We'll load the first item in a moment, so the ISR should start at the second
        NVIC_ClearPendingIRQ(SPI0_INT_IRQn);
        NVIC_EnableIRQ(SPI0_INT_IRQn);
        SPI0->TXDATA = spi_message[0]; // This actually initiates transmission
        return true;
    }
}

void SPI0_IRQHandler(void)
{
    switch (SPI0->CPU_INT.IIDX) {
        case SPI_CPU_INT_IIDX_STAT_TX_EVT: // SPI interrupt index for transmit FIFO
            SPI0->TXDATA = spi_message[spi_message_idx];
            spi_message_idx++;
            spi_wakeup = 1;
            if (spi_message_idx == spi_message_len) { // We're done!
               spi_transmission_in_progress = false; 
               NVIC_DisableIRQ(SPI0_INT_IRQn);
            }
            break;
        default:
            break;
    }
}