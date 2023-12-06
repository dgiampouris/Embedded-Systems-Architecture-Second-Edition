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

void led_setup(void)
{
    uint32_t reg;
    AHB2_CLOCK_ER |= GPIOB_AHB2_CLOCK_ER;
    reg = GPIOB_MODE & ~(0x03 << (BLUE_LED_PIN * 2));
    GPIOB_MODE = reg | (1 << (BLUE_LED_PIN * 2));
    reg = GPIOB_MODE & ~(0x03 << (RED_LED_PIN * 2));
    GPIOB_MODE = reg |(1 << (RED_LED_PIN * 2));
    reg = GPIOC_MODE & ~(0x03 << (GREEN_LED_PIN * 2));
    GPIOC_MODE = reg | (1 << (GREEN_LED_PIN * 2));

    reg = GPIOB_PUPD & ~(0x03 <<  (BLUE_LED_PIN * 2));
    GPIOB_PUPD = reg | (0x02 << (BLUE_LED_PIN * 2));
    reg = GPIOB_PUPD & ~(0x03 <<  (RED_LED_PIN * 2));
    GPIOB_PUPD = reg | (0x02 << (RED_LED_PIN * 2));
    reg = GPIOC_PUPD & ~(0x03 <<  (GREEN_LED_PIN * 2));
    GPIOC_PUPD = reg | (0x02 << (GREEN_LED_PIN * 2));
}

void blue_led_on(void)
{
    GPIOB_BSRR |= (1 << BLUE_LED_PIN);
}

void blue_led_off(void)
{
    GPIOB_BSRR |= (1 << (BLUE_LED_PIN + 16));
}

void blue_led_toggle(void)
{
    if ((GPIOB_ODR & (1 << BLUE_LED_PIN)) == (1 << BLUE_LED_PIN))
        blue_led_off();
    else
        blue_led_on();
}

void red_led_on(void)
{
    GPIOB_BSRR |= (1 << RED_LED_PIN);
}

void red_led_off(void)
{
    GPIOB_BSRR |= (1 << (RED_LED_PIN + 16));
}

void red_led_toggle(void)
{
    if ((GPIOB_ODR & (1 << RED_LED_PIN)) == (1 << RED_LED_PIN))
        red_led_off();
    else
        red_led_on();
}

void green_led_on(void)
{
    GPIOC_BSRR |= (1 << GREEN_LED_PIN);
}

void green_led_off(void)
{
    GPIOC_BSRR |= (1 << (GREEN_LED_PIN + 16));
}

void green_led_toggle(void)
{
    if ((GPIOC_ODR & (1 << GREEN_LED_PIN)) == (1 << GREEN_LED_PIN))
        green_led_off();
    else
        green_led_on();
}
