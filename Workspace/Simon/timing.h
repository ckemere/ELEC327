#ifndef timing_include
#define timing_include

#include <stdint.h>
#include <stdbool.h>

void InitializeTimerG0(void);

void SetTimerG0Delay(uint16_t delay);

void EnableTimerG0(void);

extern bool timer_wakeup;

#endif /* timing_include */
