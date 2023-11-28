/*
 *
 * Embedded System Architecture - Second Edition
 * Copyright (c) 2018-2022 Packt
 *
 * Author: Daniele Lacamera <root@danielinux.net>
 *
 * MIT License
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal 
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 */
#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

/* System specific: PLL with (8 MHz internal oscillator), CPU at 120MHz*/
#define CPU_FREQ (120000000)
#define PLL_FULL_MASK (0xFFFFFFFF)

/*** FLASH ***/
#define FLASH_BASE (0x40022000)
#define FLASH_ACR  (*(volatile uint32_t *)(FLASH_BASE + 0x00))
#define FLASH_ACR_ENABLE_DATA_CACHE (1 << 10)
#define FLASH_ACR_ENABLE_INST_CACHE (1 << 9)

/*** RCC ***/
#define RCC_BASE (0x40021000)
#define RCC_CR                  (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_PLLCFGR             (*(volatile uint32_t *)(RCC_BASE + 0x0c))
#define RCC_CFGR                (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_BDCR                (*(volatile uint32_t *)(RCC_BASE + 0x90))
#define APB1_CLOCK_RST          (*(volatile uint32_t *)(RCC_BASE + 0x38))
#define APB1_CLOCK_ER           (*(volatile uint32_t *)(RCC_BASE + 0x58))
#define APB2_CLOCK_RST          (*(volatile uint32_t *)(RCC_BASE + 0x40))
#define APB2_CLOCK_ER           (*(volatile uint32_t *)(RCC_BASE + 0x60))
#define AHB2_CLOCK_ER           (*(volatile uint32_t *)(RCC_BASE + 0x4c))

/*** RCC SETTINGS ***/
#define TIM2_APB1_CLOCK_ER_VAL 	(1 << 0)
#define PWR_APB1_CLOCK_ER_VAL   (1 << 28)
#define SYSCFG_APB2_CLOCK_ER    (1 << 0)
#define GPIOB_AHB2_CLOCK_ER     (1 << 1)
#define GPIOC_AHB2_CLOCK_ER     (1 << 2)

#define RCC_CR_PLLRDY               (1 << 25)
#define RCC_CR_PLLON                (1 << 24)
#define RCC_CR_HSIRDY               (1 << 10)
#define RCC_CR_HSION                (1 << 8)
#define RCC_CR_MSIRDY               (1 << 1)
#define RCC_CR_MSION                (1 << 0)
#define RCC_CR_MSIPLLEN             (1 << 2)
#define RCC_CR_MSIRGSEL             (1 << 3)
#define RCC_CR_MSIRANGE             (6 << 4)

#define RCC_BDCR_LSERDY             (1 << 1)
#define RCC_BDCR_LSEON              1

#define RCC_CFGR_SW_HSI             0x1
#define RCC_CFGR_SW_HSE             0x2
#define RCC_CFGR_SW_PLL             0x3


#define RCC_PLLCFGR_PLLSRC          1 // MSI clock as PLL clock entry

#define RCC_PRESCALER_DIV_NONE 0
#define RCC_PRESCALER_DIV_2    4
#define RCC_PRESCALER_DIV_4    5

/* GPIO SETTINGS */
#define GPIOB_BASE 0x48000400
#define GPIOB_MODE (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIOB_OTYPE (*(volatile uint32_t *)(GPIOB_BASE + 0x04))
#define GPIOB_PUPD (*(volatile uint32_t *)(GPIOB_BASE + 0x0c))
#define GPIOB_ODR  (*(volatile uint32_t *)(GPIOB_BASE + 0x14))
#define GPIOB_BSRR (*(volatile uint32_t *)(GPIOB_BASE + 0x18))
#define BLUE_LED_PIN (7) // Pin name: PB7
#define RED_LED_PIN (14) // Pin name: PB14

#define GPIOC_BASE 0x48000800
#define GPIOC_MODE (*(volatile uint32_t *)(GPIOC_BASE + 0x00))
#define GPIOC_OTYPE (*(volatile uint32_t *)(GPIOB_BASE + 0x04))
#define GPIOC_PUPD (*(volatile uint32_t *)(GPIOB_BASE + 0x0c))
#define GPIOC_IDR  (*(volatile uint32_t *)(GPIOC_BASE + 0x10))
#define GPIOC_ODR  (*(volatile uint32_t *)(GPIOB_BASE + 0x14))
#define GPIOC_BSRR (*(volatile uint32_t *)(GPIOB_BASE + 0x18))
#define BUTTON_PIN (13)
#define GREEN_LED_PIN (7) // Pin name: PC7

/* SYSCFG */
#define SYSCFG_BASE (0x40010000)
#define SYSCFG_EXTICR4 (*(volatile uint32_t *)(SYSCFG_BASE + 0x14))
#define EXTICR_EXTI13_MASK (0xFFFF)

/* EXTI */
#define EXTI_BASE (0x40010400)
#define EXTI_IMR    (*(volatile uint32_t *)(EXTI_BASE + 0x00))
#define EXTI_EMR    (*(volatile uint32_t *)(EXTI_BASE + 0x04))
#define EXTI_RTSR   (*(volatile uint32_t *)(EXTI_BASE + 0x08))
#define EXTI_FTSR   (*(volatile uint32_t *)(EXTI_BASE + 0x0c))
#define EXTI_SWIER  (*(volatile uint32_t *)(EXTI_BASE + 0x10))
#define EXTI_PR     (*(volatile uint32_t *)(EXTI_BASE + 0x14))


/* POWER CONTROL REGISTER */
#define POW_BASE (0x40007000)
#define POW_CR1          (*(volatile uint32_t *)(POW_BASE + 0x00))
#define POW_CR3          (*(volatile uint32_t *)(POW_BASE + 0x08))
#define POW_SCR         (*(volatile uint32_t *)(POW_BASE + 0x18))

#define POW_CR1_LPMS  (1 << 0)
#define POW_CR1_VOS   (1 << 9)
#define POW_CR1_DBPEN (1 << 8)
#define POW_SCR_CSBF  (1 << 8)
#define POW_SCR_CWUF1 (1 << 0)
#define POW_SR_WUF    (1 << 0)
#define POW_CR3_EWUP  (1 << 4)


#if (CPU_FREQ == 120000000)
#   define PLLM 0
#   define PLLN 30
#   define PLLR 0
#   define HPRE RCC_PRESCALER_DIV_NONE
#   define PPRE1 RCC_PRESCALER_DIV_4
#   define PPRE2 RCC_PRESCALER_DIV_2
#   define FLASH_WAITSTATES 5
#elif (CPU_FREQ == 100000000)
#   define PLLM 0
#   define PLLN 25
#   define PLLR 0
#   define POWER_SAVE 1
#   define HPRE RCC_PRESCALER_DIV_NONE
#   define PPRE1 RCC_PRESCALER_DIV_2
#   define PPRE2 RCC_PRESCALER_DIV_NONE
#   define FLASH_WAITSTATES 4
#elif (CPU_FREQ == 80000000)
#   define PLLM 0
#   define PLLN 40
#   define PLLR 1
#   define HPRE RCC_PRESCALER_DIV_NONE
#   define PPRE1 RCC_PRESCALER_DIV_2
#   define PPRE2 RCC_PRESCALER_DIV_NONE
#   define FLASH_WAITSTATES 3
#elif (CPU_FREQ == 48000000)
#   define PLLM 0
#   define PLLN 24
#   define PLLR 1
#   define POWER_SAVE 1
#   define HPRE RCC_PRESCALER_DIV_NONE
#   define PPRE1 RCC_PRESCALER_DIV_4 
#   define PPRE2 RCC_PRESCALER_DIV_2
#   define FLASH_WAITSTATES 2
#else 
# error "Please select a valid CPU_FREQ in system.h"
#endif

/* SCB for sleep configuration */
#define SCB_SCR (*(volatile uint32_t *)(0xE000ED10))
#define SCB_SCR_SEVONPEND	(1 << 4)
#define SCB_SCR_SLEEPDEEP	(1 << 2)
#define SCB_SCR_SLEEPONEXIT     (1 << 1)

/* Assembly helpers */
#define DMB() __asm__ volatile ("dmb")
#define WFI() __asm__ volatile ("wfi")
#define WFE() __asm__ volatile ("wfe")
#define SEV() __asm__ volatile ("sev")

/* Master clock setting */
void clock_pll_on(int powersave);
void clock_pll_off(void);


/* NVIC */
/* NVIC ISER Base register (Cortex-M) */
#define NVIC_EXTI15_10_IRQN     (40)
#define NVIC_TIM2_IRQN          (28)
#define NVIC_ISER_BASE (0xE000E100)
#define NVIC_ICER_BASE (0xE000E180)
#define NVIC_ICPR_BASE (0xE000E280)
#define NVIC_IPRI_BASE (0xE000E400)

static inline void nvic_irq_enable(uint8_t n)
{
    int i = n / 32;
    volatile uint32_t *nvic_iser = ((volatile uint32_t *)(NVIC_ISER_BASE + 4 * i));
    *nvic_iser |= (1 << (n % 32));
}

static inline void nvic_irq_disable(uint8_t n)
{
    int i = n / 32;
    volatile uint32_t *nvic_icer = ((volatile uint32_t *)(NVIC_ICER_BASE + 4 * i));
    *nvic_icer |= (1 << (n % 32));
}

static inline void nvic_irq_setprio(uint8_t n, uint8_t prio)
{
    volatile uint8_t *nvic_ipri = ((volatile uint8_t *)(NVIC_IPRI_BASE + n));
    *nvic_ipri = prio;
}


static inline void nvic_irq_clear(uint8_t n)
{
    int i = n / 32;
    volatile uint8_t *nvic_icpr = ((volatile uint8_t *)(NVIC_ICPR_BASE + 4 * i));
    *nvic_icpr = (1 << (n % 32));
}
#endif
