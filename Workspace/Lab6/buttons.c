
#include "buttons.h"
#include "delay.h"
#include <ti/devices/msp/msp.h>

void InitializeButtonGPIO(void) {
    // ===============================================================================================================
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

    // ===============================================================================================================
    // Initialize IOMUX for Button inputs
    // We have to do this multiple times, so let's define it once and then re-use the code
    const uint32_t input_configuration = IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM_INENA_ENABLE |
            ((uint32_t) 0x00000001) | // GPIO function is always MUX entry 1
            IOMUX_PINCM_INV_DISABLE | // TODO: experiment with setting this to invert our logic
            IOMUX_PINCM_PIPU_ENABLE | IOMUX_PINCM_PIPD_DISABLE | // pull up resistor - switch connects to ground
            IOMUX_PINCM_HYSTEN_DISABLE | // disable input pin hysteresis (TODO: experiment with this)
            IOMUX_PINCM_WUEN_DISABLE;  // wake-up disable (TODO: experiment with this for ultra low power!)

    IOMUX->SECCFG.PINCM[(IOMUX_PINCM53)] = input_configuration; // PA23
    IOMUX->SECCFG.PINCM[(IOMUX_PINCM54)] = input_configuration; // PA24
    IOMUX->SECCFG.PINCM[(IOMUX_PINCM55)] = input_configuration; // PA25
    IOMUX->SECCFG.PINCM[(IOMUX_PINCM59)] = input_configuration; // PA26
    // ===============================================================================================================

    delay_cycles(POWER_STARTUP_DELAY); // delay to enable GPIO to turn on and reset
}


