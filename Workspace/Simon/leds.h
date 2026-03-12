#ifndef leds_include
#define leds_include

#include <stdbool.h>
#include <stdint.h>

extern bool spi_wakeup;
extern bool spi_transmission_in_progress;

void InitializeLEDInterface(void);
bool SendSPIMessage(uint16_t *message_ptr, int message_len);

// We're going to define the state of the 4 LEDs in a cool way that will allow the struct 
// to memory map precisely to an SPI message.
// NOTE: SPI is configured for 16 bit transfers, with most significant bit first.
//       **BUT** Cortex M0+ is little endian by default. That means that the least
//       significant BYTE is stored before the most significant BYTE in the memory
//       representation of a 2-byte integer. The naive ordering of bytes in our struct
//       would be [HEADER_ZEROSx2][Bright][Bl][Gr][Rd].(repeat x3 LEDs)..[FOOTER]. 
//       But when we cast this as a uint16_t, interpreting the bytes as 16 bit ints would
//       mean they were sent as [STILL_ZEROS][Bl][Bright][Rd][Gr]. Because the Brightness
//       byte requires leading 111b, the messages will not only be scrambled, they will
//       likely be not-functional.
//       THE SOLUTION is simply to pre-reorder the bytes in our struct representation of
//       the LEDs. (Or we could alternatively configure SPI to transmit bytes, but 16 bits
//       is more efficient.)
typedef struct __attribute__((packed)) {
    uint8_t blue;
    uint8_t brightness : 5;
    uint8_t _header    : 3; // Always b111
    uint8_t red;
    uint8_t green;
} apa102_led_t;

typedef struct __attribute__((packed)) {
    uint16_t     start[2];   // all 0x00
    apa102_led_t led[4];
    uint16_t     end[2];     // all 0xFF
} leds_message_t;

#endif // leds_include