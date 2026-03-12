#ifndef buzzer_include
#define buzzer_include

#include <stdbool.h>
#include <stdint.h>

void InitializeBuzzer(void);

void SetBuzzerPeriod(uint16_t period);
void EnableBuzzer(void);
void DisableBuzzer(void);

#endif // buzzer_include