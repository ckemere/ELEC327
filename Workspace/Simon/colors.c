#include "colors.h"
#include "leds.h"

/* 
    Note that the C standard specifies that if we leave things
   out of our struct initializer, they are set to zero. That's
   convenient for us!
*/
const leds_message_t leds_off = {
    .start = {0x0000, 0x0000},
    .led = {
        {.brightness = 0, ._header = 7}, // initializing other fields to zero
        {.brightness = 0, ._header = 7},
        {.brightness = 0, ._header = 7},
        {.brightness = 0, ._header = 7},
    },
    .end = {0xFFFF, 0xFFFF},
};


const leds_message_t leds_on = {
    .start = {0x0000, 0x0000},
    .led = {
        {.brightness = 5, ._header = 7, .red = 0x10, .green = 0x10, .blue = 0xF0},  // Blue
        {.brightness = 5, ._header = 7, .red = 0xF0, .green = 0x10, .blue = 0x10},  // Red
        {.brightness = 5, ._header = 7, .red = 0x10, .green = 0xF0, .blue = 0x10},  // Green
        {.brightness = 5, ._header = 7, .red = 0x10, .green = 0x00, .blue = 0x10},  // Magenta
    },
    .end = {0xFFFF, 0xFFFF},
};
