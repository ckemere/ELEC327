#include "state_machine_logic.h"
#include "hw_interface.h"
#include <ti/devices/msp/msp.h>

state_t GetNextState(state_t current_state, uint32_t input)
{
    state_t new_state;
    new_state.minute = current_state.minute + 1;
    new_state.hour = current_state.hour;

    if (current_state.minute == 11) {
        new_state.minute = 0;
        new_state.hour = current_state.hour + 1;
        if (new_state.hour > 11) {
            new_state.hour = 0;
        }
    }

    return new_state;
}

int GetStateOutput(state_t current_state) {
    int output = led_mask;

    output &= ~(0x1 << minute_leds[current_state.minute].pin_number);
    output &= ~(0x1 << hour_leds[current_state.hour].pin_number);

    return output;
}