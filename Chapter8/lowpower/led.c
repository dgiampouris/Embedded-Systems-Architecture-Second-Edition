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

void led_setup(void)
{
    uint32_t reg;
    AHB2_CLOCK_ER |= GPIOB_AHB2_CLOCK_ER;
    reg = GPIOB_MODE & ~ (0x03 << (LED_PIN * 2));
    GPIOB_MODE = reg | (1 << (LED_PIN * 2));

    reg = GPIOB_PUPD & ~(0x03 <<  (LED_PIN * 2));
    GPIOB_PUPD = reg | (0x02 << (LED_PIN * 2));
}

void led_on(void)
{
    GPIOB_BSRR |= (1 << LED_PIN);
}

void led_off(void)
{
    GPIOB_BSRR |= (1 << (LED_PIN + 16));
}

void led_toggle(void)
{
    if ((GPIOB_ODR & (1 << LED_PIN)) == (1 << LED_PIN))
        led_off();
    else
        led_on();
}
