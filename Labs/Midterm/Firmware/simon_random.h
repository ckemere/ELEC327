/*
 * rgb_interface.h
 *
 *  Created on: Mar 9, 2022
 *      Author: ckemere
 */
#ifndef SIMON_RANDOM_H_
#define SIMON_RANDOM_H_

#include <stdint.h>

/* Random number generation */
// -------------------------------------
void srand(uint16_t seed);
uint16_t rand();

#endif
