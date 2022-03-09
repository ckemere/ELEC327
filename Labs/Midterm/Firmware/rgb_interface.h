/*
 * rgb_interface.h
 *
 *  Created on: Mar 12, 2021
 *      Author: ckemere
 */

#ifndef RGB_INTERFACE_H_
#define RGB_INTERFACE_H_

void rgb_init_spi(void);
void rgb_send_frame(const uint8_t *frame1, bool wait_for_completion);
void rgb_send_start();
void rgb_send_end();

#endif /* RGB_INTERFACE_H_ */
