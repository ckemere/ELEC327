#include "initialize_leds.h"
#include "delay.h"
#include <ti/devices/msp/msp.h>

#define POWER_STARTUP_DELAY (16)

void InitializeGPIO() {
    // 1. Reset GPIO port (the one(s) for pins that you will use)
    GPIOA->GPRCM.RSTCTL = (GPIO_RSTCTL_KEY_UNLOCK_W | GPIO_RSTCTL_RESETSTKYCLR_CLR | GPIO_RSTCTL_RESETASSERT_ASSERT);

    // 2. Enable power on LED GPIO port
    GPIOA->GPRCM.PWREN = (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);

    delay_cycles(POWER_STARTUP_DELAY); // delay to enable GPIO to turn on and reset

    /* Code to initialize specific GPIO PIN */
    // PA0 is red led gpio
    IOMUX->SECCFG.PINCM[(IOMUX_PINCM1)] = (IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000001));
    GPIOA->DOUTSET31_0 = (0x00000000);
    GPIOA->DOESET31_0 = (0x00000001); // PA0 is our output pin for the Led
}
