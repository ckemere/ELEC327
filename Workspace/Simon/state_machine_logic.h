#ifndef state_machine_logic_include
#define state_machine_logic_include

#include <stdint.h>
#include <stdbool.h>
#include "leds.h" // For LED state type

// PERIOD * MAX_COUNTER = 32000
#define PERIOD 20 // This means updates every 0.625 ms
#define MAX_COUNTER 1600 // This needs to be a multiple of 4 for modulo PWM to work!
#define FLASH_COUNTER 400 // 2x per second flashing

#define BUTTON_BOUNCE_LIMIT 3

#define SIXTEENTH_NOTE 200

/* For our state machine, we need to think about the MODE, the SOUND, and the BUTTONS */

/* Let's start by defining a enum and a struct that will be useful for tracking the state of  
 *  the buttons. */
typedef enum {
    BUTTON_IDLE = 0,
    BUTTON_BOUNCING,
    BUTTON_PRESS
} button_state_t;

typedef struct {
   button_state_t state; 
   uint32_t       depressed_counter;
} button_t;

/* Next, let's define the state of the sound*/
// Basic info for the buzzer's state
typedef struct {
    uint16_t period;
    bool     sound_on;
} buzzer_state_t;

// More complex info for playing songs.
// We'll define a song as a linked list of notes and durations.
// Rests would be defined with sound_on = false.
typedef struct music_note {
    buzzer_state_t     note;
    uint16_t           duration;
} music_note_t;

typedef struct animation_note {
    buzzer_state_t note;
    const leds_message_t *leds;
    uint16_t       duration;
} animation_note_t;

typedef enum {
    PLAYING_NOTE,
    INTERNOTE
} internote_t;

typedef struct {
    const animation_note_t *song;         // this is an array
    int          song_length;
    int          index;
    internote_t  note_state;  // allows us to keep track of inter-note breaks
    uint32_t     music_counter; // this will actually be used with the tick counter to achieve durations
    uint32_t     note_counter; // This will be used for quarter notes, whole notes etc
} song_state_t;

/* And last, let's define the possible modes */
typedef enum {
    MODE_SONG = 0,
    MODE_BUTTONS
} mode_t;


/* Finally, we can define our state machine state*/
typedef struct {
    button_t buttons[4];
    buzzer_state_t buzzer;
    const leds_message_t *leds;
    mode_t mode;
    song_state_t song_state; 
} state_t;



state_t GetNextState(state_t current_state, uint32_t input);
void SetBuzzerState(buzzer_state_t);

extern const animation_note_t animation[];
extern const int animation_length;

#endif /* state_machine_logic_include */
