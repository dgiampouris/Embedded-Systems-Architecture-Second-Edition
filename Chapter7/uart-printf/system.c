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
#include <stdint.h>
#include "system.h"

/*** FLASH ***/
#define FLASH_BASE (0x40022000)
#define FLASH_ACR  (*(volatile uint32_t *)(FLASH_BASE + 0x00))
#define FLASH_ACR_ENABLE_DATA_CACHE (1 << 10)
#define FLASH_ACR_ENABLE_INST_CACHE (1 << 9)

/*** RCC ***/

#define RCC_BASE (0x40021000)
#define RCC_CR       (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_PLLCFGR  (*(volatile uint32_t *)(RCC_BASE + 0x0c))
#define RCC_CFGR     (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_APB1ENR1 (*(volatile uint32_t *)(RCC_BASE + 0x58))
#define RCC_BDCR     (*(volatile uint32_t *)(RCC_BASE + 0x90))

#define RCC_CR_PLLRDY               (1 << 25)
#define RCC_CR_PLLON                (1 << 24)
#define RCC_CR_HSIRDY               (1 << 10)
#define RCC_CR_HSION                (1 << 8)
#define RCC_CR_MSIRDY               (1 << 1)
#define RCC_CR_MSION                (1 << 0)
#define RCC_CR_MSIPLLEN             (1 << 2)
#define RCC_CR_MSIRGSEL             (1 << 3)
#define RCC_CR_MSIRANGE             (6 << 4)


#define RCC_BDCR_LSERDY               (1 << 1)
#define RCC_BDCR_LSEON                1

#define RCC_CFGR_SW_HSI             0x1
#define RCC_CFGR_SW_HSE             0x2
#define RCC_CFGR_SW_PLL             0x3


#define RCC_PLLCFGR_PLLSRC 1    // MSI clock as PLL clock entry

#define RCC_PRESCALER_DIV_NONE 0
#define RCC_PRESCALER_DIV_2    4
#define RCC_PRESCALER_DIV_4    5



#define PLLM 0
#define PLLN 40
#define PLLPEN 0
#define PLLP 0
#define PLLPDIV 0
#define PLLQEN 1
#define PLLQ 2
#define PLLREN 1
#define PLLR 0

/* PWR */
#define PWR_BASE (0x40007000)
#define PWR_CR1      (*(volatile uint32_t *)(PWR_BASE + 0x00))

#define PWR_CR1_DBPEN (1 << 8)
#define RCC_APB1ENR1_PWREN (1 << 28)

void flash_set_waitstates(void)
{
    FLASH_ACR |= 5 | FLASH_ACR_ENABLE_DATA_CACHE | FLASH_ACR_ENABLE_INST_CACHE;
}


void clock_config(void)
{
    uint32_t reg32;
    /* Enable internal high-speed oscillator. */
    RCC_CR |= RCC_CR_HSION;
    DMB();
    while ((RCC_CR & RCC_CR_HSIRDY) == 0) {};

    /* Select HSI as SYSCLK source. */

    reg32 = RCC_CFGR;
    reg32 &= ~((1 << 1) | (1 << 0));
    RCC_CFGR = (reg32 | RCC_CFGR_SW_HSI);
    DMB();

    /* Enable low speed external oscillator. */
    RCC_APB1ENR1 |= RCC_APB1ENR1_PWREN;
    DMB();
    PWR_CR1 |= PWR_CR1_DBPEN;
    DMB();
    RCC_BDCR |= RCC_BDCR_LSEON;
    DMB();
    while ((RCC_BDCR & RCC_BDCR_LSERDY) == 0) {};

    /* Enable additional internal high-speed oscillator 8MHz. */
    RCC_CR |= RCC_CR_MSION;
    DMB();
    while ((RCC_CR & RCC_CR_MSIRDY) == 0) {};
    // add MSI options
    RCC_CR |= RCC_CR_MSIPLLEN | RCC_CR_MSIRGSEL | RCC_CR_MSIRANGE;
    DMB();

    /*
     * Set prescalers for AHB, ADC, ABP1, ABP2.
     */
    reg32 = RCC_CFGR;
    reg32 &= ~(0xF0);
    RCC_CFGR = (reg32 | (RCC_PRESCALER_DIV_NONE << 4));
    DMB();
    reg32 = RCC_CFGR;
    reg32 &= ~(0x1C00);
    RCC_CFGR = (reg32 | (RCC_PRESCALER_DIV_NONE << 8));
    DMB();
    reg32 = RCC_CFGR;
    reg32 &= ~(0x07 << 11);
    RCC_CFGR = (reg32 | (RCC_PRESCALER_DIV_NONE << 11));
    DMB();

    /* Set PLL config */
    reg32 = RCC_PLLCFGR;
    reg32 &= ~(PLL_FULL_MASK);
    RCC_PLLCFGR = reg32 | RCC_PLLCFGR_PLLSRC | (PLLM << 4) | 
        (PLLN << 8) | (PLLPEN << 16)  | (PLLP << 17) | 
        (PLLQEN << 20) | (PLLQ << 21) | (PLLREN << 24) |
        (PLLR << 25) | (PLLPDIV << 27); 
    DMB();
    /* Enable PLL oscillator and wait for it to stabilize. */
    RCC_CR |= RCC_CR_PLLON;
    DMB();
    while ((RCC_CR & RCC_CR_PLLRDY) == 0) {};

    /* Select PLL as SYSCLK source. */
    reg32 = RCC_CFGR;
    reg32 &= ~((1 << 1) | (1 << 0));
    RCC_CFGR = (reg32 | RCC_CFGR_SW_PLL);
    DMB();

    /* Wait for PLL clock to be selected. */
    while ((RCC_CFGR & ((1 << 1) | (1 << 0))) != RCC_CFGR_SW_PLL) {};

    /* Disable internal high-speed oscillator. */
    RCC_CR &= ~RCC_CR_HSION;
}

