
#include "timing.h"
#include "delay.h"
#include <ti/devices/msp/msp.h>

bool timer_wakeup;

inline void SetTimerG0Delay(uint16_t delay) {
    TIMG0->COUNTERREGS.LOAD = delay; // This will load as soon as timer is enabled
}

inline void EnableTimerG0() {
    TIMG0->COUNTERREGS.CTRCTL |= (GPTIMER_CTRCTL_EN_ENABLED);
    NVIC_EnableIRQ(TIMG0_INT_IRQn); // Enable the timer interrupt
}

void InitializeTimerG0() {
    /* Timer module and Sleep Mode Initialization */
    // 1. Step 1 is always to reset and enable
    TIMG0->GPRCM.RSTCTL = (GPTIMER_RSTCTL_KEY_UNLOCK_W | GPTIMER_RSTCTL_RESETSTKYCLR_CLR | GPTIMER_RSTCTL_RESETASSERT_ASSERT);
    TIMG0->GPRCM.PWREN = (GPTIMER_PWREN_KEY_UNLOCK_W | GPTIMER_PWREN_ENABLE_ENABLE);
    delay_cycles(POWER_STARTUP_DELAY); // delay to enable module to turn on and reset

    // 2. Step 2 is to choose the desired clock. We want UPCLK (LFCLK) so we can use a LPM
    // TIMG0->CLKSEL = GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE;
    TIMG0->CLKSEL = GPTIMER_CLKSEL_LFCLK_SEL_ENABLE;

    // 3. By default, the timer counts down to zero and then stops. Configure it to repeat.
    TIMG0->COUNTERREGS.CTRCTL = GPTIMER_CTRCTL_REPEAT_REPEAT_1;

    // 4. Enable timer interrupt when we reach 0
    TIMG0->CPU_INT.IMASK |= GPTIMER_CPU_INT_IMASK_Z_SET;

    // 5. Enable the clock input to the timer. (The timer itself is still not enabled!)
    TIMG0->COMMONREGS.CCLKCTL = GPTIMER_CCLKCTL_CLKEN_ENABLED;
    
    timer_wakeup = false;
}



// The function needs to be put into the interrupt table!!!!
void TIMG0_IRQHandler(void)
{
    // This wakes up the processor!
    switch (TIMG0->CPU_INT.IIDX) {
        case GPTIMER_CPU_INT_IIDX_STAT_Z: // Counted down to zero event.
            timer_wakeup = true; // set a flag so we can know what woke us up.
            break;
        default:
            break;
    }
}


