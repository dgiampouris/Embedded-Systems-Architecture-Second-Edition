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
#include <stdlib.h>
#include "system.h"
#include "button.h"


static void (*button_callback)(void) = NULL;

void button_setup(void (*callback)(void))
{
    AHB2_CLOCK_ER |= GPIOC_AHB2_CLOCK_ER;
    APB2_CLOCK_ER |= SYSCFG_APB2_CLOCK_ER;

    GPIOC_MODE &= ~ (0x03 << (BUTTON_PIN * 2));
    SYSCFG_EXTICR4 &= ~EXTICR_EXTI13_MASK;
    SYSCFG_EXTICR4 |= (2 << 4);
    
    button_callback = callback;
}

void button_start_read(void)
{
    EXTI_IMR |= (1 << BUTTON_PIN);
    EXTI_EMR |= (1 << BUTTON_PIN);
    EXTI_RTSR |= (1 << BUTTON_PIN);
    nvic_irq_enable(NVIC_EXTI15_10_IRQN);
}

void isr_exti15_10(void)
{
    nvic_irq_disable(NVIC_EXTI15_10_IRQN);
    EXTI_PR |= (1 << BUTTON_PIN);
    if (button_callback)
        button_callback();
}

