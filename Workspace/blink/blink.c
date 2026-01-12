/*
 * This template file
 */

#include <ti/devices/msp/msp.h>
#include <blink_helper.h>

#define POWER_STARTUP_DELAY (16)

/* This results in approximately 0.5s of delay assuming 32.768 kHz CPU_CLK */
#define DELAY (16384)

// CLKOUT will be PA31
#define CLK_OUT_PORT                 GPIOA
#define CLK_OUT_PIN                  (0x80000000)
#define CLK_OUT_PINMUX               IOMUX_PINCM6
#define CLK_OUT_PINMUX_FUNCTION      IOMUX_PINCM6_PF_SYSCTL_CLK_OUT

void InitializeProcessor(void) {
    SYSCTL->SOCLOCK.BORTHRESHOLD = SYSCTL_SYSSTATUS_BORCURTHRESHOLD_BORMIN; // Brownout generates a reset.

    update_reg(&SYSCTL->SOCLOCK.MCLKCFG, (uint32_t) SYSCTL_MCLKCFG_UDIV_NODIVIDE, SYSCTL_MCLKCFG_UDIV_MASK); // Set UPCLK divider

    // Set MCLK to LFCLK
    SYSCTL->SOCLOCK.SYSOSCCFG |= SYSCTL_SYSOSCCFG_DISABLE_ENABLE; // Disable SYSOSC. We don't need it!
    SYSCTL->SOCLOCK.MCLKCFG |= SYSCTL_MCLKCFG_USELFCLK_ENABLE;

    // Verify LFCLK -> MCLK
    while ((SYSCTL->SOCLOCK.CLKSTATUS & SYSCTL_CLKSTATUS_CURMCLKSEL_MASK) != SYSCTL_CLKSTATUS_CURMCLKSEL_LFCLK) {
        ;
    }

    // Disable MCLK Divider
    update_reg(&SYSCTL->SOCLOCK.MCLKCFG, (uint32_t) 0x0, SYSCTL_MCLKCFG_MDIV_MASK);

    // Enable external clock out
    update_reg(&SYSCTL->SOCLOCK.GENCLKCFG,
        (uint32_t) SYSCTL_GENCLKCFG_EXCLKDIVEN_PASSTHRU | (uint32_t) SYSCTL_GENCLKCFG_EXCLKSRC_LFCLK,
        SYSCTL_GENCLKCFG_EXCLKDIVEN_MASK | SYSCTL_GENCLKCFG_EXCLKDIVVAL_MASK |
            SYSCTL_GENCLKCFG_EXCLKSRC_MASK);
    SYSCTL->SOCLOCK.GENCLKEN |= SYSCTL_GENCLKEN_EXCLKEN_ENABLE;
}

int main(void)
{
    InitializeProcessor();

    /* Code to initialize GPIO PORT */
    // 1. Reset GPIO port (the one(s) for pins that you will use)
    GPIOA->GPRCM.RSTCTL = (GPIO_RSTCTL_KEY_UNLOCK_W | GPIO_RSTCTL_RESETSTKYCLR_CLR | GPIO_RSTCTL_RESETASSERT_ASSERT);
    GPIOA->GPRCM.PWREN = (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);

    // 2. Enable power on LED GPIO port

    delay_cycles(POWER_STARTUP_DELAY); // delay to enable GPIO to turn on and reset

    /* Code to initialize specific GPIO PINS */
    // 3. Initialize the appropriate pin(s) as digital outputs (Configure the Pinmux!)
    IOMUX->SECCFG.PINCM[(IOMUX_PINCM6)] = (IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM6_PF_SYSCTL_CLK_OUT);
    GPIOA->DOESET31_0 = 0x1u<<31; // PA31 is our output pin for the clock
    IOMUX->SECCFG.PINCM[(IOMUX_PINCM50)] = (IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000001));

    // PA0 is red led gpio
    IOMUX->SECCFG.PINCM[(IOMUX_PINCM1)] = (IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000001));
    GPIOA->DOESET31_0 = (0x00000001); // PA01 is our output pin for the Led
    GPIOA->DOUTSET31_0 = (0x00000000);

    // Functional
    while (1) {
        delay_cycles(DELAY/2);

        GPIOA->DOUTTGL31_0 = (0x00000001);

    }
}

/*
 * Copyright (c) 2023, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
