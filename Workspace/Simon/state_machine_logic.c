#include <ti/devices/msp/msp.h>
#include "state_machine_logic.h"
#include "buzzer.h"
#include "music.h"
#include "buttons.h"
#include "leds.h"
#include "colors.h"

const uint32_t button_mask[] = {SW1, SW2, SW3, SW4};

button_t UpdateButton(button_t button, uint32_t input, uint32_t mask) {
    button_t new_button = button;

    if ((input & mask) == 0) {
        switch (button.state) {
            case BUTTON_IDLE:
                new_button.state = BUTTON_BOUNCING;
                new_button.depressed_counter = 1;
                break;
            case BUTTON_BOUNCING:
                new_button.depressed_counter = button.depressed_counter + 1;
                if (new_button.depressed_counter > BUTTON_BOUNCE_LIMIT) {
                    new_button.state = BUTTON_PRESS;
                }
                break;
            case BUTTON_PRESS:
            default:
                // The button counter stops here, so it never overflows
                break;
        }
    }
    else {
        // Before, we wanted to take action on the release. Here we don't care!
        new_button.state = BUTTON_IDLE;
        new_button.depressed_counter = 0;
    }

    return new_button;
}

state_t GetNextState(state_t current_state, uint32_t input)
{
    state_t new_state = current_state;

    // Update buttons
    int button_pressed = 0; // helper

    for (int i =0; i < 4; i++) {
        new_state.buttons[i] = UpdateButton(current_state.buttons[i], input, button_mask[i]);
        if (new_state.buttons[i].state == BUTTON_PRESS)
            button_pressed = button_pressed + 1;
    }

    // Update buzzer state
    new_state.buzzer.sound_on = false;
    new_state.leds = &leds_off;

    for (int i =0; i < 4; i++) {
        if (new_state.buttons[i].state == BUTTON_PRESS) {
            new_state.buzzer.period = G5_LOAD;
            new_state.buzzer.sound_on = true;
            new_state.leds = &leds_on;
            break;
        }
    }


    return new_state;
}

void SetBuzzerState(buzzer_state_t buzzer) {
    if (buzzer.sound_on) {
        EnableBuzzer();
    }
    else {
        DisableBuzzer();
    }

    SetBuzzerPeriod(buzzer.period);
}
