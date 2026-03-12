#ifndef random_include
#define random_include

#include <stdint.h>

/* Random number generation */
// -------------------------------------
void srand(uint16_t seed);
uint16_t rand();

#endif // random_include