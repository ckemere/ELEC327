/*
 * This template file implements an interrupt-driven infinite loop which
 * cycles a state machine.
 */

#include <ti/devices/msp/msp.h>
#include "hw_interface.h"
#include "state_machine_logic.h"

int main(void)
{
    InitializeLEDs();

    InitializeButton();

    InitializeTimerG0();

    state_t state; // initialize state machine
    state.hour = 0;
    state.minute = 0;

    uint32_t input;

    SetTimerG0Delay(32000); // Once per second interrupts
    EnableTimerG0();

    while (1) {
        // Need to get input from GPIO here
        input = GetButtonState();

        // Read-Modify-Write
        int current_gpio_state = GPIOA->DOUT31_0;
        current_gpio_state &= ~(led_mask); // Make the pins that we might want to edit zeros
        int output = GetStateOutput(state);
        GPIOA->DOUT31_0 = current_gpio_state + output;

        state = GetNextState(state, input);
        __WFI(); // Go to sleep until timer counts down again.
    }

}


/*
 * Copyright (c) 2026, Caleb Kemere
 * Derived from example code which is
 *
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
