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


extern volatile int sleep;

void button_setup(void)
{
    uint32_t reg;
    AHB2_CLOCK_ER |= GPIOC_AHB2_CLOCK_ER;
    APB2_CLOCK_ER |= SYSCFG_APB2_CLOCK_ER;

    GPIOC_MODE &= ~ (0x03 << (BUTTON_PIN * 2));
    SYSCFG_EXTICR4 &= ~EXTICR_EXTI13_MASK;
    SYSCFG_EXTICR4 |= (2 << 4);
    
    nvic_irq_enable(NVIC_EXTI15_10_IRQN);

    /* Interrupt lines: disabled */
    EXTI_IMR &= ~0x7FFFFF;

    /* Event: enabled on button pin */
    reg = EXTI_EMR & ~0x7FFFFF;
    EXTI_EMR = reg | (1 << BUTTON_PIN);

    /* Rising trigger selection */
    reg =  EXTI_RTSR & ~0x7FFFFF;
    EXTI_RTSR = reg | (1 << BUTTON_PIN);
    EXTI_FTSR &= ~0x7FFFFF;
}

void isr_exti15_10(void)
{
    nvic_irq_clear(NVIC_EXTI15_10_IRQN);
    nvic_irq_disable(NVIC_EXTI15_10_IRQN);
    EXTI_PR |= (1 << BUTTON_PIN);
}

