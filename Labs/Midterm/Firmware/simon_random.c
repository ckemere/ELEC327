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
#include "simon_random.h"

uint16_t lfsr; // private variable which holds the state of the random number generation system

void srand(uint16_t seed) {
    int i;
    if (seed == 0)
        lfsr = 1;
    else
        lfsr = seed;

    // Churn the random number generator enough times to scramble the state
    // This is helpful if initializers are often small values, since rand()
    // is grabbing the least significant bits and the pseudorandom sequence
    // gets built from the most significant side.
    for (i = 0; i < 32; i++)
        rand();
}

uint16_t rand() {
    // Generates a random number between 0-3
    unsigned int bit;
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5))  & 0x01;
    lfsr = (lfsr >> 1) | (bit << 15);
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5))  & 0x01;
    lfsr = (lfsr >> 1) | (bit << 15);
    return lfsr & 0x03;
}

