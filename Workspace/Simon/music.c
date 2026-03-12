#include "music.h"
#include "colors.h"
#include "state_machine_logic.h"


// Define the melody as a const array 
const animation_note_t animation[] = {
    {.note = {G5_LOAD, true},  .leds = &leds_on, .duration = 4},
    {.note = {G5_LOAD, true},  .leds = &leds_on, .duration = 8},
    {.note = {G5_LOAD, true},  .leds = &leds_on, .duration = 16},
    {.note = {G5_LOAD, false}, .leds = &leds_off, .duration = 16},
};

const int animation_length = sizeof(animation) / sizeof(animation_note_t);
