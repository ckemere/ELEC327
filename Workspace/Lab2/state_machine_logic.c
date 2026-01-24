#include "state_machine_logic.h"
#include <ti/devices/msp/msp.h>


int GetNextState(int current_state)
{
    if (current_state == OFF) {
        return ON1; // If the LED is off -> on
    }
    else {
        if (current_state == ON1)
            return ON2;
        else
            return OFF; // If the LED is on -> off
    }
}

int GetStateOutputGPIOA(int current_state) {
    if ((current_state == ON1) || (current_state == ON2)) {
        return 0x00000000;
    }
    else {
        return 0x00000001;
    }
}

int GetStateOutputGPIOB(int current_state) {
    return 0;
};
