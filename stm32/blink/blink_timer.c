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
// prescale the system clock. Therefore, the default system clock frequence is
// just the default HSI frequence, which is 16 MHz.
#define HSI_VALUE    ((uint32_t)16000000) /*!< Value of the Internal oscillator in Hz*/
#define SystemCoreClock HSI_VALUE

// TIM2 (general purpose timer)
#define APB1PERIPH_BASE       (PERIPH_BASE + 0x00000000UL)
#define TIM2_BASE             (APB1PERIPH_BASE + 0x0000UL)
#define TIM2_CR1              *(volatile uint32_t *)(TIM2_BASE + 0x00)
#define TIM_CR1_CEN           (1UL << 0)
#define TIM2_DIER             *(volatile uint32_t *)(TIM2_BASE + 0x0C)
#define TIM_DIER_UIE          (1UL << 0)
#define TIM2_SR               *(volatile uint32_t *)(TIM2_BASE + 0x10)
#define TIM_SR_UIF            (1UL << 0)
// TIM2_CNT useful for debugging
#define TIM2_CNT              *(volatile uint32_t *)(TIM2_BASE + 0x24)
#define TIM2_PSC              *(volatile uint32_t *)(TIM2_BASE + 0x28)
#define TIM2_ARR              *(volatile uint32_t *)(TIM2_BASE + 0x2C)

// NVIC (Nested Vector Interrupt Controller). This is actually defined at the
// Cortex M4 level, not the STM32 level, so we need to look at the STM32 Cortex
// M4 Manual. These definitions were taken from core_cm4.h.
#define SCS_BASE            (0xE000E000UL)                           /*!< System Control Space Base Address */
#define NVIC_BASE           (SCS_BASE + 0x0100UL)                    /*!< NVIC Base Address */
#define NVIC_ISER(i)        *(volatile uint32_t *)(NVIC_BASE + 0x000UL + (i))
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

// Need to enable the AHB1 peripheral clock. See Section 6.3.9 RCC AHB1
// peripheral clock enable register (RCC_AHB1ENR) in the Reference Manual. AHB1
// is offset 0x30 from RCC, and GPIOA is bit 0 in register.
#define RCC_AHB1ENR         *(volatile uint32_t *)(RCC_BASE + 0x30)
#define RCC_AHB1ENR_GPIOAEN (1UL << 0)

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

// This could be a #define if we really wanted, but I like having some data here
// to test the linker script.
const uint32_t loop_length = 200000;

void Reset_Handler(void)
{
	// Enable GPIOA clock.
	RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	// Enable PA5 (pin 5 of GPIOA) as an output bit, which controls the LED.
	// (All of GPIOA_MODER is set to 0 on reset, so we just need to set our
	// one bit to set MODER5 to 01.)
	GPIOA_MODER |= GPIO_MODER_MODER5_0;

	// Enable TIM2 clock
	RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;

	// Set TIM2 ARR. This is what the counter will count up to before it
	// triggers the interrupt. This value is actually 32 bits for TIM2 (and
	// TIM5).
	TIM2_ARR = SystemCoreClock / 10000 - 1;

	// Set TIM2 prescaler. Must be <= 65536. -1 is needed because prescale
	// value adds one.
	TIM2_PSC = 10000 - 1;

	// Enable the hardware interrupt.
	TIM2_DIER |= TIM_DIER_UIE;

	// Enable TIM2 interrupt line
	NVIC_EnableIRQ(TIM2_IRQn);

	// Enable TIM2 counter (should be done after reset counter set and interrupt enabled)
	TIM2_CR1 |= TIM_CR1_CEN;

	// While loop is useful if debugging w/ GDB because we can immediately
	// have a stack frame and backtrace, so we can e.g. query for variable
	// values.
	// while (1);
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
