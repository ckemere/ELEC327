#ifndef state_machine_logic_include
#define state_machine_logic_include

#include <stdint.h>

/* Define states as a tuple */
typedef struct {
    int hour; 
    int minute; 
} state_t;

state_t GetNextState(state_t current_state, uint32_t input);

int GetStateOutput(state_t current_state);


#endif /* state_machine_logic_include */
