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

#define AHB2_CLOCK_ER (*(volatile uint32_t *)(0x4002104C))
#define GPIOC_AHB2_CLOCK_ER (1 << 2)

#define GPIOC_BASE 0x48000800
#define GPIOC_MODE (*(volatile uint32_t *)(GPIOC_BASE + 0x00))
#define GPIOC_IDR  (*(volatile uint32_t *)(GPIOC_BASE + 0x10))
#define BUTTON_PIN (13)

#define SYSCFG_BASE (0x40010000)
#define SYSCFG_EXTICR4 (*(volatile uint32_t *)(SYSCFG_BASE + 0x14))
#define EXTICR_EXTI13_MASK (0xFFFF)

#define EXTI_BASE (0x40010400)
#define EXTI_IMR    (*(volatile uint32_t *)(EXTI_BASE + 0x00))
#define EXTI_EMR    (*(volatile uint32_t *)(EXTI_BASE + 0x04))
#define EXTI_RTSR   (*(volatile uint32_t *)(EXTI_BASE + 0x08))
#define EXTI_FTSR   (*(volatile uint32_t *)(EXTI_BASE + 0x0c))
#define EXTI_SWIER  (*(volatile uint32_t *)(EXTI_BASE + 0x10))
#define EXTI_PR     (*(volatile uint32_t *)(EXTI_BASE + 0x14))


void button_setup(void)
{
    /* Configure as input */
    AHB2_CLOCK_ER |= GPIOC_AHB2_CLOCK_ER;

    GPIOC_MODE &= ~ (0x03 << (BUTTON_PIN * 2));
    SYSCFG_EXTICR4 &= ~EXTICR_EXTI13_MASK;
    SYSCFG_EXTICR4 |= (2 << 4);
    
    nvic_irq_enable(NVIC_EXTI15_10_IRQN);

    EXTI_IMR |= (1 << BUTTON_PIN);
    EXTI_EMR |= (1 << BUTTON_PIN);
    EXTI_RTSR |= (1 << BUTTON_PIN);
}

volatile uint32_t button_presses = 0;

void isr_exti15_10(void)
{
    EXTI_PR |= (1 << BUTTON_PIN);
    button_presses++;
}


int button_is_pressed(void)
{
    /* Read the value */
    return ((GPIOC_IDR & (1 << BUTTON_PIN)) >> BUTTON_PIN);
}

