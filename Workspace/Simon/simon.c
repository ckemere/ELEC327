
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
#include "colors.h"
#include "state_machine_logic.h"

int message_len = sizeof(leds_message_t)/sizeof(uint16_t);

int main(void)
{
    
    InitializeButtonGPIO();
    InitializeBuzzer();
    InitializeLEDInterface();
    InitializeTimerG0();

    DisableBuzzer();

    state_t state; // initialize state machine
    for (int i = 0; i < 4; i++) {
        state.buttons[i].state = BUTTON_IDLE;
        state.buttons[i].depressed_counter = 0;
    }
    state.buzzer.period = 3999;
    state.buzzer.sound_on = false;
    state.leds = &leds_off;
    state.mode = MODE_BUTTONS;

    uint32_t input;

    SetTimerG0Delay(PERIOD); // 20 ticks at 32 kHz is 0.6 ms
    EnableTimerG0();

    while (1) {
        if (timer_wakeup) { // Ignore SPI wakeups
            SetBuzzerState(state.buzzer);
            while (!SendSPIMessage((uint16_t *)state.leds, message_len)) {
                // Block until previous message is complete                
            }

            // Get input from GPIO here
            uint32_t input = GPIOA->DIN31_0 & (SW1 + SW2 + SW3 + SW4);

            state = GetNextState(state, input);

            timer_wakeup = false;
            __WFI(); // Go to sleep until timer counts down again.
        }
    }

}

