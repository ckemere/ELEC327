#ifndef buttons_include
#define buttons_include

#include <stdint.h>

void InitializeButtonGPIO(void);

// Pin numbers on GPIOA from Simon PCB
// Kicad here - https://github.com/ckemere/ELEC327/tree/master/PCBs/Simon-2026-JLC
#define SW1 ((uint32_t) 0x1 << 23)
#define SW2 ((uint32_t) 0x1 << 24)
#define SW3 ((uint32_t) 0x1 << 25)
#define SW4 ((uint32_t) 0x1 << 26)


#endif /* buttons_include */
