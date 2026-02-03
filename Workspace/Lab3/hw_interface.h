#ifndef hw_interface_include
#define hw_interface_include

#include <stdint.h>

void InitializeLEDs();
void InitializeButton();
void InitializeTimerG0();

void SetTimerG0Delay(uint16_t delay);
void EnableTimerG0();
uint32_t GetButtonState();

typedef struct {
    int pin_number;  /* PAx → x */
    int iomux;       /* PINCMx index */
} pin_config_t;

extern pin_config_t hour_leds[12];
extern pin_config_t minute_leds[12];

extern int led_mask; // we'll use this to remember which pins are output

#endif /* hw_interface_include */
