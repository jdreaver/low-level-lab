/**
 * Enable USART2 interface.
 *
 * To use this module, you must call `usart2_enable()`. You probably also want
 * to override _write so newlib knows where to send e.g. puts() and printf()
 * output. (TODO: Can we do this with some kind of option?)
 *
 *     // Override the 'write' newlib method to implement 'printf' over USART.
 *     int _write(int handle, char* data, int size) {
 *         return usart2_write(handle, data, size);
 *     }
 */

#pragma once

void usart2_enable(void);
int usart2_write(int handle, char* data, int size);
