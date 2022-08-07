/**
 * Work with the blue User button on the STM32F401RE
 *
 * To use this module, you must call `user_button_enable()`, and then implement
 * `EXTI15_10_IRQHandler`. Inside that handler, you can check if the user button
 * is the source of the interrupt.
 *
 *     if (EXTI->PR & (1 << USER_BUTTON_PIN)) {
 *         // Clear the EXTI status flag.
 *         EXTI->PR |= (1 << USER_BUTTON_PIN);
 *         ...
 */

#pragma once

#ifdef STM32F401xE
  // User button (blue button) on STM32F401RE is located in PC13, which is pin 13,
  // so we configure EXTI13, which is on EXTICR4 (for pins 12 through 15).
  #define USER_BUTTON_PIN 13
#else
  #error "Unknown board. Can't define USER_BUTTON_PIN."
#endif

void user_button_enable(void);
