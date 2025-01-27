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
#include <stdio.h>
#include <sys/types.h>
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#include "ti_msp_dl_config.h"


#define false 0
#define true 1

char gWelcomeMsg[] = "\r\n==== MSPM0 Console Test ====\r\n";
char gNoPulseMsg[] = "No pulse detected in last second.\r\n";
char gTimingMsg[25];

typedef struct {
    uint32_t spaceWidth;
    uint32_t pulseWidth;
} PulseData;

PulseData dataBuffer[1000];
int maxBufferedData = 100;
int bufferIndex = 0;
bool bufferFull = false;

volatile int gConsoleTxTransmitted, gConsoleTxDMATransmitted, timerExpired;


#define TIMER_CAPTURE_DURATION (CAPTURE_0_INST_LOAD_VALUE)
volatile int pulseCaptureDetected;


void UART_Console_write(const char *data, uint16_t size)
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
    GPIO_LEDS_PORT->DOUTSET31_0 = GPIO_LEDS_USER_TEST_PIN;
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    GPIO_LEDS_PORT->DOUTCLR31_0 = GPIO_LEDS_USER_TEST_PIN;
}

volatile uint32_t tRise, tFall, previousTFall = 0;


int main(void)
{
    bool bufferTransmitInProgress = false;
    int bufferTransmitIndex = 0;
    timerExpired           = false;
    gConsoleTxTransmitted    = false;
    gConsoleTxDMATransmitted = false;

    SYSCFG_DL_init();

    DL_TimerG_setLoadValue(CAPTURE_0_INST, (uint32_t) (0xFFFFFFFF)); // Override config (100s) to set to 32bit max

    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);


    NVIC_EnableIRQ(CAPTURE_0_INST_INT_IRQN);

    pulseCaptureDetected = false;
    DL_TimerG_startCounter(CAPTURE_0_INST);

    /* Write welcome message */
    UART_Console_write(&gWelcomeMsg[0], sizeof(gWelcomeMsg));

    while (1) {
        if (true == bufferTransmitInProgress) {
            sprintf(gTimingMsg, "=%10u,%10u\r\n",
                    dataBuffer[bufferTransmitIndex].spaceWidth,
                    dataBuffer[bufferTransmitIndex].pulseWidth);
            UART_Console_write(&gTimingMsg[0], 25); // This function is blocking!!!
            bufferTransmitIndex = bufferTransmitIndex+1;

            if (bufferTransmitIndex == maxBufferedData) {
              // Now we can reset the buffer and start buffering again!
              bufferTransmitInProgress = false;
              bufferIndex = 0;
              bufferFull = false; // This is going to start the interrupt triggering again!
            }
        }
        else if (true == timerExpired) {
            timerExpired  = false;
            if (false == pulseCaptureDetected) {
                UART_Console_write(&gNoPulseMsg[0], sizeof(gNoPulseMsg));
            }
            else {
                pulseCaptureDetected = false;
            }
        }
        else if (true == bufferFull) {
            bufferTransmitIndex = 0;
            bufferTransmitInProgress = true;
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
            previousTFall = tFall;
            tRise = (TIMG12->COUNTERREGS.CC_01[0]);
            tFall = (TIMG12->COUNTERREGS.CC_01[1]);
            if (false == bufferFull) {
                dataBuffer[bufferIndex].pulseWidth = tFall - tRise;
                dataBuffer[bufferIndex].spaceWidth = tRise - previousTFall;
                bufferIndex += 1;
                if (bufferIndex == maxBufferedData) {
                    bufferFull = true;
                }
                GPIO_LEDS_PORT->DOUTSET31_0 = GPIO_LEDS_USER_LED_1_PIN;
                GPIO_LEDS_PORT->DOUTCLR31_0 = GPIO_LEDS_USER_LED_1_PIN;
            }
            pulseCaptureDetected = true;
            break;
        default:
            break;
    }
}
