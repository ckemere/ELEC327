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
#include <stdbool.h>
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#include "ti_msp_dl_config.h"


volatile uint32_t externalClk = 0;
volatile uint32_t ioValue = 0;
volatile uint32_t tEdge = 0;
volatile uint32_t whichEdgeDetected = 0;

#define false 0
#define true 1

char gWelcomeMsg[] = "\r\n==== MSPM0 Console Test ====\r\n";
char gNoPulseMsg[] = "No pulse detected in last second. Generating pulse.\r\n";
char gTimingMsg[60];
char gCharDetectedMsg[13] = "Received  \r\n";
char gRxData;

volatile int gConsoleTxTransmitted, gConsoleTxDMATransmitted, timerExpired, uartRxDetected=0;


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


int main(void)
{
    timerExpired           = false;
    gConsoleTxTransmitted    = false;
    gConsoleTxDMATransmitted = false;

    int pulseCapturedRecently = false;

    uint32_t pulseWidth = 0;
    uint32_t spaceWidth = 0;

    SYSCFG_DL_init();

    // Enable the SysTick counter. Systick is a 24-bit counter.
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk; // Disable Systick for configuration
    SysTick->LOAD = 0x00FFFFFF;                 // Set the value that the Systick should count down to to the maximum for 24 bits
    SysTick->VAL  = 0;                          // Set the current value of the counter to zero. When we execute the next line, it will loop around back to LOAD
    SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk); // Re-enable systick


    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(GPIO_CLK_GRP_INT_IRQN);

    pulseCaptureDetected = false;

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
        else if (uartRxDetected) {
            uartRxDetected = 0;
            sprintf(gCharDetectedMsg, "Received %c\r\n", gRxData);
            UART_Console_write(&gCharDetectedMsg[0], 13);
            GPIO_CLK_GRP_PORT->DOUTSET31_0 = GPIO_CLK_GRP_GPIO_NRST_OUT_PIN;
            GPIO_CLK_GRP_PORT->DOESET31_0 = GPIO_CLK_GRP_GPIO_NRST_OUT_PIN;
            GPIO_CLK_GRP_PORT->DOUTCLR31_0 = GPIO_CLK_GRP_GPIO_NRST_OUT_PIN;
            GPIO_LEDS_PORT->DOUTSET31_0 = GPIO_LEDS_USER_LED_1_PIN;
            delay_cycles(1000);
            GPIO_LEDS_PORT->DOUTCLR31_0 = GPIO_LEDS_USER_LED_1_PIN;
            GPIO_CLK_GRP_PORT->DOUTSET31_0 = GPIO_CLK_GRP_GPIO_NRST_OUT_PIN;
            GPIO_CLK_GRP_PORT->DOESET31_0 = GPIO_CLK_GRP_GPIO_NRST_OUT_PIN;
        }
        else if (true == pulseCaptureDetected) {
            pulseCaptureDetected = false;
            pulseCapturedRecently = true;

            GPIO_LEDS_PORT->DOUTSET31_0 = GPIO_LEDS_USER_LED_1_PIN;
            delay_cycles(100);
            GPIO_LEDS_PORT->DOUTCLR31_0 = GPIO_LEDS_USER_LED_1_PIN;

//            __BKPT(0);
            sprintf(gTimingMsg, ">%1u,%10u\r\n", whichEdgeDetected, tEdge);
            UART_Console_write(&gTimingMsg[0], 16);
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
        case DL_UART_MAIN_IIDX_RX:
            gRxData = DL_UART_Main_receiveData(UART_0_INST);
            uartRxDetected = 1;
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


//    __STATIC_INLINE uint32_t DL_GPIO_getEnabledInterruptStatus(
//        GPIO_Regs* gpio, uint32_t pins)
//    {
//        return (gpio->CPU_INT.MIS & pins);
//    }


void GROUP1_IRQHandler(void)
{

    externalClk++;

    uint32_t newIOValue = DL_GPIO_readPins(GPIO_CLK_GRP_PORT, GPIO_CLK_GRP_GPIO_PULSES_IN_PIN);

    if (newIOValue == ioValue) { // unchanged. do nothing
//        SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    }
    else if (ioValue) { // old value was true (1-ish), now we're false, so falling edge
        whichEdgeDetected = 0;
        tEdge = externalClk;
        ioValue = newIOValue;
        pulseCaptureDetected = true;
        SCB->SCR &= ~(SCB_SCR_SLEEPONEXIT_Msk); // disable sleep on exit
        // wake up from sleep
    }
    else {
        whichEdgeDetected = 1;
        tEdge = externalClk;
        ioValue = newIOValue;
        pulseCaptureDetected = true;
        SCB->SCR &= ~(SCB_SCR_SLEEPONEXIT_Msk); // disable sleep on exit
        // wake up from sleep
    }

}

