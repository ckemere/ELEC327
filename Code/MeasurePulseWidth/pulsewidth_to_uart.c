/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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

#include "ti_msp_dl_config.h"
#include <stdio.h>

uint8_t gWelcomeMsg[] = "\r\n==== MSPM0 Console Test ====\r\n";
uint8_t gNoPulseMsg[] = "No pulse detected in last second. Generating pulse.\r\n";
uint8_t gTimingMsg[15];

volatile bool gConsoleTxTransmitted, gConsoleTxDMATransmitted, timerExpired;


#define TIMER_CAPTURE_DURATION (CAPTURE_0_INST_LOAD_VALUE)
volatile bool pulseCaptureDetected;


void UART_Console_write(const uint8_t *data, uint16_t size)
{
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)(data));
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)(&UART_0_INST->TXDATA));
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, size);

    DL_SYSCTL_disableSleepOnExit();

    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);

    while (false == gConsoleTxDMATransmitted) {
        __WFE();
    }

    while (false == gConsoleTxTransmitted) {
        __WFE();
    }

    gConsoleTxTransmitted    = false;
    gConsoleTxDMATransmitted = false;
}

__attribute__((always_inline))
void blink_led(void) {
//    DL_GPIO_setPins(GPIO_LEDS_PORT,
//        GPIO_LEDS_USER_TEST_PIN);
    GPIO_LEDS_PORT->DOUTSET31_0 = GPIO_LEDS_USER_TEST_PIN;
//            delay_cycles(10000000);
    __NOP();
    __NOP();
    __NOP();

//    DL_GPIO_clearPins(GPIO_LEDS_PORT,
//        GPIO_LEDS_USER_TEST_PIN);
    GPIO_LEDS_PORT->DOUTCLR31_0 = GPIO_LEDS_USER_TEST_PIN;
}

int main(void)
{
    timerExpired           = false;
    gConsoleTxTransmitted    = false;
    gConsoleTxDMATransmitted = false;

    bool pulseCapturedRecently = false;

    uint32_t pulseWidth = 0;


    SYSCFG_DL_init();
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);



    NVIC_EnableIRQ(CAPTURE_0_INST_INT_IRQN);

    pulseCaptureDetected = false;
    DL_TimerG_startCounter(CAPTURE_0_INST);

    /* Write welcome message */
    UART_Console_write(&gWelcomeMsg[0], sizeof(gWelcomeMsg));

    while (1) {
        if (timerExpired == true) {
            timerExpired  = false;
            if (false == pulseCapturedRecently) {
                UART_Console_write(&gNoPulseMsg[0], sizeof(gNoPulseMsg));
                blink_led();
            }
            else {
                pulseCapturedRecently = false;
            }
            blink_led();
        }
        else if (true == pulseCaptureDetected) {
            pulseCaptureDetected = false;
            pulseCapturedRecently = true;

            pulseWidth =
                (DL_Timer_getCaptureCompareValue(CAPTURE_0_INST, DL_TIMER_CC_1_INDEX)) -
                (DL_Timer_getCaptureCompareValue(CAPTURE_0_INST, DL_TIMER_CC_0_INDEX));

            GPIO_LEDS_PORT->DOUTSET31_0 = GPIO_LEDS_USER_LED_1_PIN;
            delay_cycles(100);
            GPIO_LEDS_PORT->DOUTCLR31_0 = GPIO_LEDS_USER_LED_1_PIN;

//            __BKPT(0);
            sprintf(gTimingMsg, "=%10u\r\n", pulseWidth);
            UART_Console_write(&gTimingMsg[0], sizeof(gTimingMsg));
        }
        else {
            __WFI(); // __WFE();?
        }

    }
}

void UART_0_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_0_INST)) {
        case DL_UART_MAIN_IIDX_EOT_DONE:
            gConsoleTxTransmitted = true;
            break;
        case DL_UART_MAIN_IIDX_DMA_DONE_TX:
            gConsoleTxDMATransmitted = true;
            break;
        default:
            break;
    }
}

void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_Timer_getPendingInterrupt(TIMER_0_INST)) {
        case DL_TIMER_IIDX_ZERO:
            timerExpired = true;
            break;
        default:
            break;
    }
}


void CAPTURE_0_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(CAPTURE_0_INST)) {
        case DL_TIMER_IIDX_CC1_UP:
            pulseCaptureDetected = true;
            break;
        default:
            break;
    }
}
