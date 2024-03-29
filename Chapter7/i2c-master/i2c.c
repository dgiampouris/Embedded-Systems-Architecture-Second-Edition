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
#include "i2c.h"

#define I2C1 (0x40005400)
#define I2C1_CR1        (*(volatile uint32_t *)(I2C1))
#define I2C1_CR2        (*(volatile uint32_t *)(I2C1 + 0x04))
#define I2C1_OAR1       (*(volatile uint32_t *)(I2C1 + 0x08))
#define I2C1_OAR2       (*(volatile uint32_t *)(I2C1 + 0x0c))
#define I2C1_ISR        (*(volatile uint32_t *)(I2C1 + 0x18))
#define I2C1_RXDR       (*(volatile uint32_t *)(I2C1 + 0x24))
#define I2C1_TXDR       (*(volatile uint32_t *)(I2C1 + 0x28))

#define I2C_CR1_ENABLE              (1 << 0)
#define I2C_CR2_START		    (1 << 13)
#define I2C_CR2_STOP		    (1 << 14)
#define I2C_CR2_NACK		    (1 << 15)
#define I2C_CR2_FREQ_MASK           (0x3ff)

#define I2C_ISR_RX_NOTEMPTY  	    (1 << 2)
#define I2C_ISR_TX_EMPTY	    (1 << 0)

#define I2C_ISR_BUSY                (1 << 15)

#define APB1_CLOCK_RST (*(volatile uint32_t *)(0x40021038))
#define APB1_CLOCK_ER (*(volatile uint32_t *)(0x40021058))
#define I2C1_APB1_CLOCK_ER_VAL 	(1 << 21)

#define GPIOB_BASE 0x48000400
#define GPIOB_MODE  (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIOB_AFL   (*(volatile uint32_t *)(GPIOB_BASE + 0x20))
#define GPIOB_AFH   (*(volatile uint32_t *)(GPIOB_BASE + 0x24))

#define AHB2_CLOCK_ER           (*(volatile uint32_t *)(0x4002104C))
#define AHB2_CLOCK_RST          (*(volatile uint32_t *)(0x4002102C))
#define GPIOB_AHB2_CLOCK_ER (1 << 1)

#define I2C1_PIN_AF 4
#define I2C1_SDA 9
#define I2C1_SCL 8

static void i2c1_pins_setup(void)
{
    uint32_t reg;
    AHB2_CLOCK_ER |= GPIOB_AHB2_CLOCK_ER;
    /* Set mode = AF */
    reg = GPIOB_MODE & ~ (0x03 << (I2C1_SCL * 2));
    GPIOB_MODE = reg | (2 << (I2C1_SCL * 2));
    reg = GPIOB_MODE & ~ (0x03 << (I2C1_SDA * 2));
    GPIOB_MODE = reg | (2 << (I2C1_SDA * 2));

    /* Alternate function: */
    reg =  GPIOB_AFH & ~(0xf << ((I2C1_SDA - 8) * 4));
    GPIOB_AFH = reg | (I2C1_PIN_AF << ((I2C1_SDA - 8) * 4));
}

static void i2c1_reset(void)
{
    APB1_CLOCK_RST |= I2C1_APB1_CLOCK_ER_VAL;
    APB1_CLOCK_RST &= ~I2C1_APB1_CLOCK_ER_VAL;
}

static void i2c1_send_start(void)
{
    I2C1_CR2 |= I2C_CR2_START;
}

static void i2c1_send_stop(void)
{
    I2C1_CR2 |= I2C_CR2_STOP;
}

void i2c1_test_sequence(void)
{
    volatile uint32_t isr;
    const uint8_t address = 0x42;
    I2C1_CR1 &= ~I2C_CR1_ENABLE;
    I2C1_CR2 &= ~I2C_CR2_STOP;
    I2C1_CR2 &= ~I2C_CR2_NACK;
    I2C1_CR1 |= I2C_CR1_ENABLE;

    /* Wait if the bus is busy */
    do {
        isr = I2C1_ISR;
    } while ((isr & I2C_ISR_BUSY) != 0);;
    
    /* Send a start condition */
    i2c1_send_start;

    /* Send address + R/W = 0 */
    I2C1_TXDR = (address << 1);
    do {
        isr = I2C1_ISR;
    } while ((isr & I2C_ISR_BUSY) != I2C_ISR_BUSY);;
    I2C1_TXDR = (0x00);
    do {
        isr = I2C1_ISR;
    } while ((isr & I2C_ISR_TX_EMPTY) != 0);;
    I2C1_TXDR = (0x01);
    do {
        isr = I2C1_ISR;
    } while ((isr & I2C_ISR_TX_EMPTY) != 0);;

    i2c1_send_stop();
}

void i2c1_setup(void)
{
    uint32_t reg;
    i2c1_pins_setup();
    APB1_CLOCK_ER |= I2C1_APB1_CLOCK_ER_VAL;
    I2C1_CR1 &= ~I2C_CR1_ENABLE;
    i2c1_reset();

    I2C1_CR1 |= I2C_CR1_ENABLE;
}
