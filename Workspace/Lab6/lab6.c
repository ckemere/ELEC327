
/*
 * Copyright (c) 2026, Caleb Kemere
 * All rights reserved, see LICENSE.md
 *
 */

#include <ti/devices/msp/msp.h>
#include "delay.h"
#include "buttons.h"
#include "timing.h"
#include "buzzer.h"
#include "leds.h"

uint16_t onTxPacket[] =  {0x0, 0x0, 0xE5F0, 0x1010, 0xE510, 0x10F0, 0xE510, 0xF010, 0xE510, 0x0010, 0xFFFF, 0xFFFF};
uint16_t offTxPacket[] = {0x0, 0x0, 0xE000, 0x0000, 0xE000, 0x0000, 0xE000, 0x0000, 0xE000, 0x0000, 0xFFFF, 0xFFFF};
int message_len = sizeof(onTxPacket) / sizeof(onTxPacket[0]);

int main(void)
{
    InitializeButtonGPIO();
    InitializeBuzzer();
    InitializeLEDInterface();
    InitializeTimerG0();

    // let the buzzer run for 0.1 s just so we know it's there!
    delay_cycles(1600000);
    DisableBuzzer();

    SetTimerG0Delay(20); // 20 ticks at 32 kHz is 0.6 ms
    EnableTimerG0();

    // VERY BASIC LOOP - If button 1 signals a 0, enable the PWM
    while (1) {
        if (timer_wakeup) { // Ignore SPI wakeups
            uint32_t input = GPIOA->DIN31_0 & (SW1 + SW2 + SW3 + SW4);
            if ((input & SW1) == 0) { // active low!
                EnableBuzzer();
                while (!SendSPIMessage(onTxPacket, message_len)) 
                {
                    // Block until previous message is complete                
                }
            }
            else {
                DisableBuzzer();
                while (!SendSPIMessage(offTxPacket, message_len)) 
                {
                    // Block until previous message is complete
                }
            }
        }
        
        // The above is just a basic example I expect you to implement functions that look something like this:
        // SetStateMachineOutput(state);
        // state = GetNextState(state, input);

        __WFI(); // Go to sleep until timer counts down again.
    }

}

