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

#include <ti/devices/msp/msp.h>
#include <elec327.h>

#define POWER_STARTUP_DELAY                                                (16)
#define CPUCLK_FREQ                                                     32000000

/* This results in approximately 0.5s of delay assuming 32MHz CPU_CLK */
#define DELAY (16000000)

/* 
#define LED_PORT GPIOA // GPIOA is declared in mspm0g350x.h
#define LED_PINS (0x00000008) | (0x00000010) | (0x00000020) | (0x00000040)  // 4rd, 4th, 5th, 6th bits, corresponding to pins 3-6!

// PinMux indices for each pin. See Datasheet Table 6.1
#define LED_1_IOMUX (IOMUX_PINCM8)
#define LED_2_IOMUX (IOMUX_PINCM9)
#define LED_3_IOMUX (IOMUX_PINCM10)
#define LED_4_IOMUX (IOMUX_PINCM11)
//#define LED_IOMUX_IDX 9 // Datasheet, page 10

*/

#define LED_PORT GPIOB // GPIOB is a variable declared in mspm0g350x.h
#define LED_PINS (0x00400000) // LED1 on the launchpad is Pin PB22
#define LED_IOMUX (IOMUX_PINCM50)

int main(void)
{
    // Power on and reset GPIOA
    LED_PORT->GPRCM.RSTCTL =
    (GPIO_RSTCTL_KEY_UNLOCK_W | GPIO_RSTCTL_RESETSTKYCLR_CLR |
        GPIO_RSTCTL_RESETASSERT_ASSERT);

    // Enable power in GPIOA
    LED_PORT->GPRCM.PWREN = (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);
    delay_cycles(POWER_STARTUP_DELAY);



    // Initialize pins as digital outputs
    // Configure the PinMux for each pin (Datasheet p. 10 for values for each pin)
    IOMUX->SECCFG.PINCM[LED_IOMUX] =
        (IOMUX_PINCM_PC_CONNECTED | ((uint32_t) 0x00000001));

    // Clear pins (set values to 0)
    LED_PORT->DOUTCLR31_0 = LED_PINS;

    // Enable GPIO output
    LED_PORT->DOESET31_0 = LED_PINS;


    SYSCTL->SOCLOCK.BORTHRESHOLD = (uint32_t) ((uint32_t)0x00000000U); // Brownout generates a powercycle. Could also be configured 1, 2, or 3

    // DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
    update_reg(&SYSCTL->SOCLOCK.SYSOSCCFG, (uint32_t) (0x00000000U),
        SYSCTL_SYSOSCCFG_FREQ_MASK);

    /* Set default configuration */
    // Disable HFXT
    SYSCTL->SOCLOCK.HSCLKEN &= ~(SYSCTL_HSCLKEN_HFXTEN_MASK);

    // Disable SYSPLL
    SYSCTL->SOCLOCK.HSCLKEN &= ~(SYSCTL_HSCLKEN_SYSPLLEN_MASK);



    // CODE STARTS HERE

    /* Default: LED1 ON*/
    LED_PORT->DOUTSET31_0 = LED_PINS;
    while (1) {
        /*
         * Call togglePins API to flip the current value of LEDs 1-3. This
         * API causes the corresponding HW bits to be flipped by the GPIO HW
         * without need for additional R-M-W cycles by the processor.
         */
        delay_cycles(DELAY);

        // Toggle GPIO
        LED_PORT->DOUTTGL31_0 = LED_PINS;

    }
}
