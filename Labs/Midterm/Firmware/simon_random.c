/*
 * simon_random.h
 *
 * Random number generation code for the simon game.
 * Provides for two functions, rand() and srand().
 * 
 * unsigned int rand() returns a pseudorandom number in the range of 0-3
 * void srand(int seed) is used to initialize (or reset) the random number generation
 *
 *  Created on: Mar 9, 2022
 *      Author: ckemere
 */
/*  */
// -------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include "rgb_interface.h"
#include "simon_random.h"


unsigned int lfsr; // private variable which holds the state of the random number generation system


void srand(unsigned int seed) {
    if (seed == 0)
        lfsr = 1;
    else
        lfsr = seed;
}

unsigned int rand() {
    // Generates a random number between 0-3
    unsigned int bit;
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5))  & 0x01;
    lfsr = (lfsr >> 1) | (bit << 15);
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5))  & 0x01;
    lfsr = (lfsr >> 1) | (bit << 15);
    return lfsr & 0x03;
}

