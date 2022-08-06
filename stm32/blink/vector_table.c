#include <stdint.h>
#include <string.h>

// start() is provided by the user. It is kind of like main()
extern void start(void);

// Variables from linker script
extern uint8_t _sdata;
extern uint8_t _edata;
extern uint8_t _sidata;

extern uint8_t _sbss;
extern uint8_t _ebss;

void Reset_Handler(void)
{
	// Copy pre-initialized data into .data section in RAM
	uint32_t data_size = &_edata - &_sdata;
	memcpy(&_sdata, &_sidata, data_size);

	// Zero out bss
	uint32_t bss_size = &_ebss - &_sbss;
 	memset(&_sbss, 0x00, bss_size);

	// Run user start function
	start();
}

// Default handler does nothing;
void default_handler(void) {}

void NMI_Handler(void) __attribute__((weak, alias("default_handler")));
void HardFault_Handler(void) __attribute__((weak, alias("default_handler")));
void MemManage_Handler(void) __attribute__((weak, alias("default_handler")));
void BusFault_Handler(void) __attribute__((weak, alias("default_handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("default_handler")));
void SVC_Handler(void) __attribute__((weak, alias("default_handler")));
void DebugMon_Handler(void) __attribute__((weak, alias("default_handler")));
void PendSV_Handler(void) __attribute__((weak, alias("default_handler")));
void SysTick_Handler(void) __attribute__((weak, alias("default_handler")));
void WWDG_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void PVD_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void TAMP_STAMP_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void RTC_WKUP_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void FLASH_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void RCC_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void EXTI0_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void EXTI1_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void EXTI2_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void EXTI3_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void EXTI4_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void DMA1_Stream0_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void DMA1_Stream1_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void DMA1_Stream2_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void DMA1_Stream3_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void DMA1_Stream4_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void DMA1_Stream5_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void DMA1_Stream6_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void ADC_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void EXTI9_5_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void TIM1_BRK_TIM9_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void TIM1_UP_TIM10_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void TIM1_TRG_COM_TIM11_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void TIM1_CC_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void TIM2_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void TIM3_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void TIM4_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void I2C1_EV_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void I2C1_ER_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void I2C2_EV_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void I2C2_ER_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void SPI1_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void SPI2_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void USART1_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void EXTI15_10_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void RTC_Alarm_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void OTG_FS_WKUP_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void DMA1_Stream7_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void SDIO_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void TIM5_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void SPI3_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void DMA2_Stream0_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void DMA2_Stream1_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void DMA2_Stream2_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void DMA2_Stream3_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void DMA2_Stream4_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void OTG_FS_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void DMA2_Stream5_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void DMA2_Stream6_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void DMA2_Stream7_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void USART6_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void I2C3_EV_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void I2C3_ER_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void FPU_IRQHandler(void) __attribute__((weak, alias("default_handler")));
void SPI4_IRQHandler(void) __attribute__((weak, alias("default_handler")));

// Vector table. This needs to be stored at 0x00000004, which with the default
// BOOT pin configuration is aliased to flash memory at 0x08000004. Note that
// the first element (stack pointer location) is set in the linker script.
void (* vector_table[])(void) __attribute__((section(".vector_table"))) =
{
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0,
    0,
    0,
    0,
    SVC_Handler,
    DebugMon_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,
    WWDG_IRQHandler,                   /* Window WatchDog              */
    PVD_IRQHandler,                    /* PVD through EXTI Line detection */
    TAMP_STAMP_IRQHandler,             /* Tamper and TimeStamps through the EXTI line */
    RTC_WKUP_IRQHandler,               /* RTC Wakeup through the EXTI line */
    FLASH_IRQHandler,                  /* FLASH                        */
    RCC_IRQHandler,                    /* RCC                          */
    EXTI0_IRQHandler,                  /* EXTI Line0                   */
    EXTI1_IRQHandler,                  /* EXTI Line1                   */
    EXTI2_IRQHandler,                  /* EXTI Line2                   */
    EXTI3_IRQHandler,                  /* EXTI Line3                   */
    EXTI4_IRQHandler,                  /* EXTI Line4                   */
    DMA1_Stream0_IRQHandler,           /* DMA1 Stream 0                */
    DMA1_Stream1_IRQHandler,           /* DMA1 Stream 1                */
    DMA1_Stream2_IRQHandler,           /* DMA1 Stream 2                */
    DMA1_Stream3_IRQHandler,           /* DMA1 Stream 3                */
    DMA1_Stream4_IRQHandler,           /* DMA1 Stream 4                */
    DMA1_Stream5_IRQHandler,           /* DMA1 Stream 5                */
    DMA1_Stream6_IRQHandler,           /* DMA1 Stream 6                */
    ADC_IRQHandler,                    /* ADC1, ADC2 and ADC3s         */
    0,               				  /* Reserved                      */
    0,              					  /* Reserved                     */
    0,                                 /* Reserved                     */
    0,                                 /* Reserved                     */
    EXTI9_5_IRQHandler,                /* External Line[9:5]s          */
    TIM1_BRK_TIM9_IRQHandler,          /* TIM1 Break and TIM9          */
    TIM1_UP_TIM10_IRQHandler,          /* TIM1 Update and TIM10        */
    TIM1_TRG_COM_TIM11_IRQHandler,     /* TIM1 Trigger and Commutation and TIM11 */
    TIM1_CC_IRQHandler,                /* TIM1 Capture Compare         */
    TIM2_IRQHandler,                   /* TIM2                         */
    TIM3_IRQHandler,                   /* TIM3                         */
    TIM4_IRQHandler,                   /* TIM4                         */
    I2C1_EV_IRQHandler,                /* I2C1 Event                   */
    I2C1_ER_IRQHandler,                /* I2C1 Error                   */
    I2C2_EV_IRQHandler,                /* I2C2 Event                   */
    I2C2_ER_IRQHandler,                /* I2C2 Error                   */
    SPI1_IRQHandler,                   /* SPI1                         */
    SPI2_IRQHandler,                   /* SPI2                         */
    USART1_IRQHandler,                 /* USART1                       */
    USART2_IRQHandler,                 /* USART2                       */
    0,               				  /* Reserved                       */
    EXTI15_10_IRQHandler,              /* External Line[15:10]s        */
    RTC_Alarm_IRQHandler,              /* RTC Alarm (A and B) through EXTI Line */
    OTG_FS_WKUP_IRQHandler,            /* USB OTG FS Wakeup through EXTI line */
    0,                                 /* Reserved     				  */
    0,                                 /* Reserved       			  */
    0,                                 /* Reserved 					  */
    0,                                 /* Reserved                     */
    DMA1_Stream7_IRQHandler,           /* DMA1 Stream7                 */
    0,                                 /* Reserved                     */
    SDIO_IRQHandler,                   /* SDIO                         */
    TIM5_IRQHandler,                   /* TIM5                         */
    SPI3_IRQHandler,                   /* SPI3                         */
    0,                                 /* Reserved                     */
    0,                                 /* Reserved                     */
    0,                                 /* Reserved                     */
    0,                                 /* Reserved                     */
    DMA2_Stream0_IRQHandler,           /* DMA2 Stream 0                */
    DMA2_Stream1_IRQHandler,           /* DMA2 Stream 1                */
    DMA2_Stream2_IRQHandler,           /* DMA2 Stream 2                */
    DMA2_Stream3_IRQHandler,           /* DMA2 Stream 3                */
    DMA2_Stream4_IRQHandler,           /* DMA2 Stream 4                */
    0,                    			  /* Reserved                     */
    0,              					  /* Reserved                     */
    0,              					  /* Reserved                     */
    0,             					  /* Reserved                     */
    0,              					  /* Reserved                     */
    0,              					  /* Reserved                     */
    OTG_FS_IRQHandler,                 /* USB OTG FS                   */
    DMA2_Stream5_IRQHandler,           /* DMA2 Stream 5                */
    DMA2_Stream6_IRQHandler,           /* DMA2 Stream 6                */
    DMA2_Stream7_IRQHandler,           /* DMA2 Stream 7                */
    USART6_IRQHandler,                 /* USART6                       */
    I2C3_EV_IRQHandler,                /* I2C3 event                   */
    I2C3_ER_IRQHandler,                /* I2C3 error                   */
    0,                                 /* Reserved                     */
    0,                                 /* Reserved                     */
    0,                                 /* Reserved                     */
    0,                                 /* Reserved                     */
    0,                                 /* Reserved                     */
    0,                                 /* Reserved                     */
    0,                                 /* Reserved                     */
    FPU_IRQHandler,                    /* FPU                          */
    0,                                 /* Reserved                     */
    0,                                 /* Reserved                     */
    SPI4_IRQHandler,                   /* SPI4                         */
};
