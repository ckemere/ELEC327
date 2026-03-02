
#include "buzzer.h"
#include "delay.h"
#include <ti/devices/msp/msp.h>

// ====================================================================================================================
// ====================================================================================================================
/* TODO - write these functions! */

void SetBuzzerPeriod(uint16_t period) {

    // HINT: This function should change both the PWM period
    //   AND the PWM duty cycle to be 50% of the period!!!
    //   It probably would be useful to #define the set of periods that correspond to the tones you  want to use!
    //   Those sorts of constants should go in the HEADER file!

    return;
}

void EnableBuzzer(void) {
    TIMA1->COUNTERREGS.CTRCTL |= (GPTIMER_CTRCTL_EN_ENABLED); // Enable the buzzer
    return;
}

void DisableBuzzer(void) {
    TIMA1->COUNTERREGS.CTRCTL &= ~(GPTIMER_CTRCTL_EN_ENABLED); // Disable the buzzer
    return;
}


void InitializeBuzzer(void) {
    // ===============================================================================================================
    // 1. Initialize GPIO IOMUX for Buzzer
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

    // IOMUX for PWM Mode on Buzzer Pin
    IOMUX->SECCFG.PINCM[(IOMUX_PINCM37)] = IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM37_PF_TIMA1_CCP0; // TIMA1-CC0 on PA15

    // ===============================================================================================================
    // 2. Initialize Timer Module for PWM
    TIMA1->GPRCM.RSTCTL = (GPTIMER_RSTCTL_KEY_UNLOCK_W | GPTIMER_RSTCTL_RESETSTKYCLR_CLR | GPTIMER_RSTCTL_RESETASSERT_ASSERT);
    TIMA1->GPRCM.PWREN = (GPTIMER_PWREN_KEY_UNLOCK_W | GPTIMER_PWREN_ENABLE_ENABLE);
    delay_cycles(POWER_STARTUP_DELAY); // delay to enable module to turn on and reset

    // Configure clocking for Timer Module
    TIMA1->CLKSEL = GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE; // BUSCLK will be SYSOSC / 32 MHz
    TIMA1->CLKDIV = GPTIMER_CLKDIV_RATIO_DIV_BY_4; // Divide by 4 to make PWM clock frequency 8 MHz

    TIMA1->COUNTERREGS.CCACT_01[0] = GPTIMER_CCACT_01_ZACT_CCP_HIGH | GPTIMER_CCACT_01_CUACT_CCP_LOW;
    TIMA1->COUNTERREGS.CTRCTL = GPTIMER_CTRCTL_REPEAT_REPEAT_1 | GPTIMER_CTRCTL_CM_UP |
            GPTIMER_CTRCTL_CVAE_ZEROVAL | GPTIMER_CTRCTL_EN_DISABLED;
    TIMA1->COMMONREGS.CCPD = GPTIMER_CCPD_C0CCP0_OUTPUT | GPTIMER_CCPD_C0CCP1_OUTPUT;
    TIMA1->COMMONREGS.CCLKCTL = (GPTIMER_CCLKCTL_CLKEN_ENABLED);

    TIMA1->COUNTERREGS.LOAD = 3999; // Period is LOAD+1 - this is 8000000/4000 = 2kHz
    // HEADS UP: This sets the frequency of the buzzer!

    TIMA1->COUNTERREGS.CC_01[0] = (TIMA1->COUNTERREGS.LOAD  + 1) / 2; // half of load to make this a square wave
    TIMA1->COUNTERREGS.CTRCTL |= (GPTIMER_CTRCTL_EN_ENABLED);

}
