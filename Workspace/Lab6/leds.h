#ifndef leds_include
#define leds_include

#include <stdbool.h>
#include <stdint.h>

extern bool spi_wakeup;
extern bool spi_transmission_in_progress;

void InitializeLEDInterface(void);
bool SendSPIMessage(uint16_t *message_ptr, int message_len);

#endif // leds_include