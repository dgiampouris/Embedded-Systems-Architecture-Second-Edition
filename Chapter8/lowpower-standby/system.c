/*
 *
 * Embedded System Architecture - Second Edition
 *
 * Copyright (c) 2024 Dimitrios Giampouris
 * Copyright (c) 2018-2022 Packt
 *
 * Author: Daniele Lacamera <root@danielinux.net>
 * Modified: Dimitrios Giampouris <d_g@dgiab.org>
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

extern uint32_t cpu_freq;

static void flash_set_waitstates(int waitstates)
{
    FLASH_ACR |=  waitstates | FLASH_ACR_ENABLE_DATA_CACHE | FLASH_ACR_ENABLE_INST_CACHE;
}

void clock_pll_off(void)
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

    /* Turn off PLL */
    RCC_CR &= ~RCC_CR_PLLON;
    DMB();
}


void clock_pll_on(int powersave)
{
    uint32_t reg32;
    uint32_t plln, pllm, pllq, pllp, pllr, hpre, ppre1, ppre2, flash_waitstates;
    
    /* Enable Power controller */
    APB1_CLOCK_ER |= PWR_APB1_CLOCK_ER_VAL;

    /* Select clock parameters */
    if (powersave) { /* 48 MHz */
        cpu_freq = 48000000;
        pllm = 0;
        plln = 24;
        pllr = 1;
        hpre = RCC_PRESCALER_DIV_NONE;
        ppre1 = RCC_PRESCALER_DIV_4; 
        ppre2 = RCC_PRESCALER_DIV_2;
        flash_waitstates = 2;
    } else { /* 120 MHz */
        cpu_freq = 120000000;
        pllm = 0;
        plln = 30;
        pllr = 0;
        hpre = RCC_PRESCALER_DIV_NONE;
        ppre1 = RCC_PRESCALER_DIV_4; 
        ppre2 = RCC_PRESCALER_DIV_2;
        flash_waitstates = 5;
    }

    flash_set_waitstates(flash_waitstates);

    /* Enable internal high-speed oscillator. */
    RCC_CR |= RCC_CR_HSION;
    DMB();
    while ((RCC_CR & RCC_CR_HSIRDY) == 0) {};

    /* Select HSI as SYSCLK source. */
    reg32 = RCC_CFGR;
    reg32 &= ~((1 << 1) | (1 << 0));
    RCC_CFGR = (reg32 | RCC_CFGR_SW_HSI);
    DMB();

    /* Enable low speed external oscillator.  */
    APB1_CLOCK_ER |= PWR_APB1_CLOCK_ER_VAL;
    DMB();
    POW_CR1 |= POW_CR1_DBPEN;
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
    RCC_CFGR = (reg32 | (hpre << 4));
    DMB();
    reg32 = RCC_CFGR;
    reg32 &= ~(0x1C00);
    RCC_CFGR = (reg32 | (ppre1 << 8));
    DMB();
    reg32 = RCC_CFGR;
    reg32 &= ~(0x07 << 11);
    RCC_CFGR = (reg32 | (ppre2 << 11));
    DMB();

    /* Set PLL config */
    reg32 = RCC_PLLCFGR;
    reg32 &= ~(PLL_FULL_MASK);
    RCC_PLLCFGR = reg32 | RCC_PLLCFGR_PLLSRC | (pllm << 4) | 
        (plln << 8) | (pllr << 25); 
    DMB();

    /* Enable power-save mode if selected */
    if (powersave) { 
        POW_CR1 |= (POW_CR1_VOS);
    }

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

