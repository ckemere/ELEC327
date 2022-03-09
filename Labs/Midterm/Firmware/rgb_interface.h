/*
 * rgb_interface.h
 *
 *  Created on: Mar 12, 2021
 *      Author: ckemere
 */

#ifndef RGB_INTERFACE_H_
#define RGB_INTERFACE_H_

void rgb_init_spi(void);
void rgb_set_LEDs(const uint8_t *LED1, const uint8_t *LED2, const uint8_t *LED3, const uint8_t *LED4);


#endif /* RGB_INTERFACE_H_ */
