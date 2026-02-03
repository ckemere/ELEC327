#include <ti/devices/msp/msp.h>
#include "hw_interface.h"

#define POWER_STARTUP_DELAY (32)

int led_mask; // Used to remember which pins are used for LEDs

pin_config_t hour_leds[12] = {
    { .pin_number = 0,  .iomux = 1  },  // PA0
    { .pin_number = 26, .iomux = 59 },  // PA26
    { .pin_number = 24, .iomux = 54 },  // PA24
    { .pin_number = 22, .iomux = 47 },  // PA22
    { .pin_number = 18, .iomux = 40 },  // PA18
    { .pin_number = 16, .iomux = 38 },  // PA16
    { .pin_number = 14, .iomux = 36 },  // PA14
    { .pin_number = 12, .iomux = 34 },  // PA12
    { .pin_number = 10, .iomux = 21 },  // PA10
    { .pin_number = 8,  .iomux = 19 },  // PA8
    { .pin_number = 6,  .iomux = 11 },  // PA6
    { .pin_number = 28, .iomux = 3  },  // PA28
};

pin_config_t minute_leds[12] = {
    { .pin_number = 27, .iomux = 60 },  // PA27
    { .pin_number = 25, .iomux = 55 },  // PA25
    { .pin_number = 23, .iomux = 53 },  // PA23
    { .pin_number = 21, .iomux = 46 },  // PA21
    { .pin_number = 17, .iomux = 39 },  // PA17
    { .pin_number = 15, .iomux = 37 },  // PA15
    { .pin_number = 13, .iomux = 35 },  // PA13
    { .pin_number = 11, .iomux = 22 },  // PA11
    { .pin_number = 9,  .iomux = 20 },  // PA9
    { .pin_number = 7,  .iomux = 14 },  // PA7
    { .pin_number = 5,  .iomux = 10 },  // PA5
    { .pin_number = 1,  .iomux = 2  },  // PA1
};


void delay_cycles(uint32_t cycles)
{
    /* this is a scratch register for the compiler to use */
    uint32_t scratch;

    /* There will be a 2 cycle delay here to fetch & decode instructions
     * if branch and linking to this function */

    /* Subtract 2 net cycles for constant offset: +2 cycles for entry jump,
     * +2 cycles for exit, -1 cycle for a shorter loop cycle on the last loop,
     * -1 for this instruction */

    __asm volatile(
#ifdef __GNUC__
        ".syntax unified\n\t"
#endif
        "SUBS %0, %[numCycles], #2; \n"
        "%=: \n\t"
        "SUBS %0, %0, #4; \n\t"
        "NOP; \n\t"
        "BHS  %=b;" /* branches back to the label defined above if number > 0 */
        /* Return: 2 cycles */
        : "=&r"(scratch)
        : [ numCycles ] "r"(cycles));
}


// TODO: Fill in the implementation here!
void InitializeButton() {
    
}

// TODO: Fill in the implementation here!
inline uint32_t GetButtonState() {
    return 0;
}


void InitializeLEDs() {
    // 1. Reset GPIO port (the one(s) for pins that you will use)
    GPIOA->GPRCM.RSTCTL = (GPIO_RSTCTL_KEY_UNLOCK_W | GPIO_RSTCTL_RESETSTKYCLR_CLR | GPIO_RSTCTL_RESETASSERT_ASSERT);

    // 2. Enable power on LED GPIO port
    GPIOA->GPRCM.PWREN = (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);

    delay_cycles(POWER_STARTUP_DELAY); // delay to enable GPIO to turn on and reset

    // initialize pins for hours and minutes
    led_mask = 0;
    for (int i = 0; i < 12; i++) {
        // hours
        int iomux_entry = hour_leds[i].iomux - 1;
        IOMUX->SECCFG.PINCM[iomux_entry] = (IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000001));
        GPIOA->DOUTSET31_0 = (0x1 << hour_leds[i].pin_number);
        GPIOA->DOESET31_0 = (0x1 << hour_leds[i].pin_number);

        led_mask |= (0x1 << hour_leds[i].pin_number);
        // minutes
        iomux_entry = minute_leds[i].iomux - 1;
        IOMUX->SECCFG.PINCM[iomux_entry] = (IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000001));
        GPIOA->DOUTSET31_0 = (0x1 << minute_leds[i].pin_number);
        GPIOA->DOESET31_0 = (0x1 << minute_leds[i].pin_number);

        led_mask |= (0x1 << minute_leds[i].pin_number);
    }
}


void InitializeTimerG0() {

    /* Timer module and Sleep Mode Initialization */
    // 1. Step 1 is always to reset and enable
    TIMG0->GPRCM.RSTCTL = (GPIO_RSTCTL_KEY_UNLOCK_W | GPIO_RSTCTL_RESETSTKYCLR_CLR | GPIO_RSTCTL_RESETASSERT_ASSERT);
    TIMG0->GPRCM.PWREN = (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);
    delay_cycles(16); // delay to enable module to turn on and reset

    // 2. Step 2 is to choose the desired clock. We want UPCLK (LFCLK) so we can use a LPM
    // TIMG0->CLKSEL = GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE;
    TIMG0->CLKSEL = GPTIMER_CLKSEL_LFCLK_SEL_ENABLE;

    // 3. By default, the timer counts down to zero and then stops. Configure it to repeat.
    TIMG0->COUNTERREGS.CTRCTL = GPTIMER_CTRCTL_REPEAT_REPEAT_1;

    // 4. Enable timer interrupt when we reach 0
    TIMG0->CPU_INT.IMASK |= GPTIMER_CPU_INT_IMASK_Z_SET;

    // 5. Enable the clock input to the timer. (The timer itself is still not enabled!)
    TIMG0->COMMONREGS.CCLKCTL = GPTIMER_CCLKCTL_CLKEN_ENABLED;
    // 6. Set sleep to be STANDBY (Rev 2025 TRM p. 227, also 2.4.2 Operating Mode Selection)
    SYSCTL->SOCLOCK.PMODECFG = SYSCTL_PMODECFG_DSLEEP_STANDBY;
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    // Set sleep to be STANDBY1 (Rev 2025 TRM Table 2-9. MSPM0Gxx ULPCLK by Operating Mode)
    SYSCTL->SOCLOCK.MCLKCFG |= SYSCTL_MCLKCFG_STOPCLKSTBY_ENABLE;

}

inline void SetTimerG0Delay(uint16_t delay) {
    TIMG0->COUNTERREGS.LOAD = delay; // This will load as soon as timer is enabled
}

inline void EnableTimerG0() {
    TIMG0->COUNTERREGS.CTRCTL |= (GPTIMER_CTRCTL_EN_ENABLED);
    NVIC_EnableIRQ(TIMG0_INT_IRQn); // Enable the timer interrupt
}


// The function needs to be put into the interrupt table!!!!
void TIMG0_IRQHandler(void)
{
    // This wakes up the processor!
    switch (TIMG0->CPU_INT.IIDX) {
        case GPTIMER_CPU_INT_IIDX_STAT_Z: // Counted down to zero event.
            // If we wanted to execute code in the ISR, it would go here.
            break;
        default:
            break;
    }
}

