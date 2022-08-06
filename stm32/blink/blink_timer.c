#include <stdbool.h>
#include <stdint.h>

// From User Manual section 6.4 LEDs: "User LD2: the green LED is a user LED
// connected to ARDUINO® signal D13 corresponding to STM32 I/O PA5 (pin 21) or
// PB13 (pin 34) depending on the STM32 target. Refer to Table 11 to Table 23
// when:
// - the I/O is HIGH value, the LED is on
// - the I/O is LOW, the LED is off"
//
// Figure 18. NUCLEO-F401RE and Table 16. ARDUINO® connectors on NUCLEO-F401RE
// and NUCLEO-F411RE have PA5 next to D13, and PA5 == Pin 5 of GPIOA

// Register addresses (page 52 of STM32F401RE data sheet, page 38 of reference
// manual)
#define PERIPH_BASE           0x40000000UL /*!< Peripheral base address in the alias region */
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000UL)
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000UL)
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800UL)

// By default, the system clock is set to the HSI clock. See 6.3.3 RCC clock
// configuration register (RCC_CFGR) on page 107, where default value of 00 for
// SW sets to HSI.
//
// Also, by default, the HPRE bits 7:4 are set to 0000, which means don't
// prescale the system clock. Therefore, the default system clock frequency is
// just the default HSI frequence, which is 16 MHz.
#define HSI_VALUE    ((uint32_t)16000000) /*!< Value of the Internal oscillator in Hz*/
#define SystemCoreClock HSI_VALUE

// Set up prescaler values. Note that the TIMx_PSC registers are 16 bit values,
// so the prescaler values need to be in between 1 and 65536. Also note that the
// chip adds 1 to TIMx_PSC to avoid divide by zero.
//
// We set the prescalar value to 16000, which takes our 16 MHz clock down to
// 1000 Hz ticks. This also lets us express the blink interval in integer
// milliseconds.
#define PRESCALER_VALUE         (16000 - 1)
#define FAST_BLINK_INTERVAL_MS  100
#define SLOW_BLINK_INTERVAL_MS  500
static volatile bool clock_fast = false;

// TIM2 (general purpose timer)
#define APB1PERIPH_BASE       (PERIPH_BASE + 0x00000000UL)
#define TIM2_BASE             (APB1PERIPH_BASE + 0x0000UL)
#define TIM2_CR1              *(volatile uint32_t *)(TIM2_BASE + 0x00)
#define TIM_CR1_CEN           (1UL << 0)
#define TIM2_DIER             *(volatile uint32_t *)(TIM2_BASE + 0x0C)
#define TIM_DIER_UIE          (1UL << 0)
#define TIM2_SR               *(volatile uint32_t *)(TIM2_BASE + 0x10)
#define TIM_SR_UIF            (1UL << 0)
#define TIM2_EGR              *(volatile uint32_t *)(TIM2_BASE + 0x14)
#define TIM_EGR_UG            (1UL << 0)
// TIM2_CNT useful for debugging
#define TIM2_CNT              *(volatile uint32_t *)(TIM2_BASE + 0x24)
#define TIM2_PSC              *(volatile uint32_t *)(TIM2_BASE + 0x28)
#define TIM2_ARR              *(volatile uint32_t *)(TIM2_BASE + 0x2C)

// NVIC (Nested Vector Interrupt Controller). This is actually defined at the
// Cortex M4 level, not the STM32 level, so we need to look at the STM32 Cortex
// M4 Manual. These definitions were taken from core_cm4.h.
#define SCS_BASE            (0xE000E000UL)                           /*!< System Control Space Base Address */
#define NVIC_BASE           (SCS_BASE + 0x0100UL)                    /*!< NVIC Base Address */
#define NVIC_ISER(i)        *(volatile uint32_t *)(NVIC_BASE + 0x000UL + ((i) * 4))
#define TIM2_IRQn           28

// Cribbed from core_cm4.h.
static void NVIC_EnableIRQ(uint32_t IRQn)
{
    NVIC_ISER(IRQn >> 5UL) = (1UL << ((IRQn) & 0x1FUL));
}

// Need to enable the APB1 peripheral clock. See Section 6.3.11 RCC APB1
// peripheral clock enable register (RCC_APB1ENR) in the Reference Manual. APB1
// is offset 0x40 from RCC, and TIME2EN is bit 0 in register.
#define RCC_APB1ENR        *(volatile uint32_t *)(RCC_BASE + 0x40)
#define RCC_APB1ENR_TIM2EN (1UL << 0)

// Reset APB1
#define RCC_APB1RSTR         *(volatile uint32_t *)(RCC_BASE + 0x20)
#define RCC_APB1RSTR_TIM2RST (1UL << 0)

// Enable APB2 for SYSCFG
#define RCC_APB2ENR          *(volatile uint32_t *)(RCC_BASE + 0x44)
#define RCC_APB2ENR_SYSCFGEN (1UL << 14)

// Need to enable the AHB1 peripheral clock. See Section 6.3.9 RCC AHB1
// peripheral clock enable register (RCC_AHB1ENR) in the Reference Manual. AHB1
// is offset 0x30 from RCC, and GPIOA is bit 0 in register.
#define RCC_AHB1ENR         *(volatile uint32_t *)(RCC_BASE + 0x30)
#define RCC_AHB1ENR_GPIOAEN (1UL << 0)
#define RCC_AHB1ENR_GPIOCEN (1UL << 2)

// MODER is offset of 0x0 from register base. MODER5 is bits 10/11 (see 8.4.1
// GPIO port mode register (GPIOx_MODER) (x = A..E and H) on page 158 of
// Reference Manual). Need to set bits 10/11 to 01 for output mode.
#define GPIOA_MODER         *(volatile uint32_t *)(GPIOA_BASE + 0x00)
#define GPIO_MODER_MODER5_0 (1UL << 10)

// ODR is offset 0x14 from GPIO base, and we want pin 5, which is the 5th bit.
// See 8.4.6 GPIO port output data register (GPIOx_ODR) (x = A..E and H) on page
// 160 of reference manual.
#define GPIOA_ODR     *(volatile uint32_t *)(GPIOA_BASE + 0x14)
#define GPIO_ODR_OD5  (1UL << 5)

// GPIOC stuff so we can debug button values (button is on PC15)
#define GPIOC_BASE      (AHB1PERIPH_BASE + 0x0800UL)
#define GPIOC_MODER     *(volatile uint32_t *)(GPIOC_BASE + 0x00)
#define GPIOC_IDR       *(volatile uint32_t *)(GPIOC_BASE + 0x10)
#define GPIO_IDR_IDR13  (1UL << 13)

// EXTI for button is configured in SYSCFG. User button (blue button) on
// STM32F401RE is located in PC13, so we configure EXTI13, which is on EXTICR4
// (for pins 12 through 15). Note that the 4 EXTI13 bits are bits 4-7 of
// EXTICR4.
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000UL)
#define SYSCFG_BASE           (APB2PERIPH_BASE + 0x3800UL)
#define SYSCFG_EXTICR4        *(volatile uint32_t *)(SYSCFG_BASE + 0x14)
#define SYSCFG_EXTICR4_EXTI13_MASK    (0b1111 << 4)
#define SYSCFG_EXTICR4_EXTI13_PC      (0b0010 << 4)

#define EXTI_BASE             (APB2PERIPH_BASE + 0x3C00UL)
#define EXTI_IMR              *(volatile uint32_t *)(EXTI_BASE + 0x00)
#define EXTI_RTSR             *(volatile uint32_t *)(EXTI_BASE + 0x08)
#define EXTI_FTSR             *(volatile uint32_t *)(EXTI_BASE + 0x0C)
#define EXTI_PR               *(volatile uint32_t *)(EXTI_BASE + 0x14)
#define BUTTON_PIN            13
#define EXTI15_10_IRQn        40

void reset_TIM2()
{
	// Disable time counter
	TIM2_CR1 &= ~(TIM_CR1_CEN);

	// Next, reset the peripheral
	RCC_APB1RSTR |=  (RCC_APB1RSTR_TIM2RST);
	RCC_APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST);

	// Set TIM2 ARR. This is what the counter will count up to before it
	// triggers the interrupt. This value is actually 32 bits for TIM2 (and
	// TIM5). The "- 1" is because the counter starts at zero.
	if (clock_fast) {
		TIM2_ARR = FAST_BLINK_INTERVAL_MS;
	} else {
		TIM2_ARR = SLOW_BLINK_INTERVAL_MS;
	}

	// Set TIM2 prescaler (computed above)
	TIM2_PSC = PRESCALER_VALUE;

	// Send an update event to reset the timer and apply settings.
	TIM2_EGR  |= TIM_EGR_UG;

	// Enable the hardware interrupt.
	TIM2_DIER |= TIM_DIER_UIE;

	// Enable TIM2 counter (should be done at the very end)
	TIM2_CR1 |= TIM_CR1_CEN;
}

int start(void)
{
	// Enable GPIOA clock for LED
	RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	// Enable GPIOC clock for button
	RCC_AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	// Enable PA5 (pin 5 of GPIOA) as an output bit, which controls the LED.
	// (All of GPIOA_MODER is set to 0 on reset, so we just need to set our
	// one bit to set MODER5 to 01.)
	GPIOA_MODER |= GPIO_MODER_MODER5_0;

	// N.B. GPIOC13 is configured correctly by default. MODER defaults to 00
	// (input), and PUPDR defaults to no pull-up/pull-down.

	// Enable SYSCFG, which is in APB2, so we can configure EXTI (extended
	// interrupts) so we can listen to the User button on the board.
	RCC_APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// Set SYSCFG to connect the button EXTI line to GPIOC (button is on pin
	// 2, which is PC13).
	SYSCFG_EXTICR4 &= ~(SYSCFG_EXTICR4_EXTI13_MASK);
	SYSCFG_EXTICR4 |=  (SYSCFG_EXTICR4_EXTI13_PC);

	// Setup the button's EXTI line as an interrupt.
	EXTI_IMR  |=  (1 << BUTTON_PIN);
	// Disable the 'rising edge' trigger (button release).
	EXTI_RTSR &= ~(1 << BUTTON_PIN);
	// Enable the 'falling edge' trigger (button press).
	EXTI_FTSR |=  (1 << BUTTON_PIN);

	// Enable EXTI15_10 interrupt line
	NVIC_EnableIRQ(EXTI15_10_IRQn);

	// Enable TIM2 clock
	RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;

	// Enable TIM2 interrupt line
	NVIC_EnableIRQ(TIM2_IRQn);

	reset_TIM2();

	// While loop is useful if debugging w/ GDB because we can immediately
	// have a stack frame and backtrace, so we can e.g. query for variable
	// values.
	// while (1);

	return 0;
}

void TIM2_IRQHandler(void)
{
        // Check the update flag. It'll be set every time the timer overflows
        if(TIM2_SR & TIM_SR_UIF) {
            // Reset the flag, so that we can catch the next overflow
            TIM2_SR &= ~TIM_SR_UIF;

            // Toggle LED pin
	    GPIOA_ODR ^= GPIO_ODR_OD5;
        }
}

void EXTI15_10_IRQHandler(void) {
	if (EXTI_PR & (1 << BUTTON_PIN)) {
		// Clear the EXTI status flag.
		EXTI_PR |= (1 << BUTTON_PIN);

		// Toggle the clock speed variable and reset clock
		clock_fast = !clock_fast;
		reset_TIM2();
	}
}
