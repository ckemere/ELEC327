
#ifndef music_include
#define music_include

#define G5  783.99

#define MCLK_FREQUENCY 8000000.0  // Use .0 to ensure floating point math

// Helper macro for Load Value calculation
// We subtract 1 from the final value because we are always counting from 0!
// We add 0.5 to ensure rounding rather than truncation in the divide.
#define CALC_LOAD(freq) ((uint16_t)((MCLK_FREQUENCY / (freq)) + 0.5) - 1)

#define G5_LOAD    CALC_LOAD(G5)

#endif /* music_include */

/*
 *
 * Copyright (c) 2026, Caleb Kemere
 * All rights reserved, see LICENSE.md
 *
 */