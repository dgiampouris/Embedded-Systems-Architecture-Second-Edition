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
#include <string.h>
#include "system.h"
#include "uart.h"

#define USART2 (0x40004400)

#define USART2_CR1      (*(volatile uint32_t *)(USART2))
#define USART2_CR2      (*(volatile uint32_t *)(USART2 + 0x04))
#define USART2_BRR      (*(volatile uint32_t *)(USART2 + 0x0C))
#define USART2_SR       (*(volatile uint32_t *)(USART2 + 0x1C))
#define USART2_RDR      (*(volatile uint32_t *)(USART2 + 0x24))
#define USART2_TDR      (*(volatile uint32_t *)(USART2 + 0x28))

#define USART2_CR1_USART_ENABLE    (1 << 0)
#define USART2_CR1_SYMBOL_LEN     (0 << 28)
#define USART2_CR1_FIFO_EN        (1 << 29)
#define USART2_CR1_PARITY_ENABLED (1 << 10)
#define USART2_CR1_PARITY_ODD     (1 << 9)
#define USART2_CR1_TXEIE          (1 << 7)
#define USART2_CR1_RXNEIE         (1 << 5)
#define USART2_CR1_TX_ENABLE      (1 << 3)
#define USART2_CR1_RX_ENABLE      (1 << 2)
#define USART2_CR2_STOPBITS       (0 << 12)
#define USART2_SR_TX_EMPTY        (1 << 7)
#define USART2_SR_RX_NOTEMPTY     (1 << 5)



#define APB1_CLOCK_ER           (*(volatile uint32_t *)(0x40021058))
#define USART2_APB1_CLOCK_ER_VAL 	(1 << 17)

#define AHB2_CLOCK_ER (*(volatile uint32_t *)(0x4002104C))
#define GPIOD_AHB2_CLOCK_ER (1 << 3)
#define GPIOD_BASE 0x48000c00
#define GPIOD_MODE  (*(volatile uint32_t *)(GPIOD_BASE + 0x00))
#define GPIOD_AFL   (*(volatile uint32_t *)(GPIOD_BASE + 0x20))
#define GPIOD_AFH   (*(volatile uint32_t *)(GPIOD_BASE + 0x24))
#define GPIO_MODE_AF (7)
#define USART2_PIN_AF 7
#define USART2_RX_PIN 6
#define USART2_TX_PIN 5

static void usart2_tx_interrupt_onoff(int enable)
{
    if (enable)
        USART2_CR1 |= USART2_CR1_TXEIE;
    else
        USART2_CR1 &= ~USART2_CR1_TXEIE;
}

static void usart2_rx_interrupt_onoff(int enable)
{
    if (enable)
        USART2_CR1 |= USART2_CR1_RXNEIE;
    else
        USART2_CR1 &= ~USART2_CR1_RXNEIE;
}

static void usart2_pins_setup(void)
{
    uint32_t reg;
    AHB2_CLOCK_ER |= GPIOD_AHB2_CLOCK_ER;
    /* Set mode = AF */
    reg = GPIOD_MODE & ~ (0x03 << (USART2_RX_PIN * 2));
    GPIOD_MODE = reg | (2 << (USART2_RX_PIN * 2));
    reg = GPIOD_MODE & ~ (0x03 << (USART2_TX_PIN * 2));
    GPIOD_MODE = reg | (2 << (USART2_TX_PIN * 2));

    /* Alternate function: use low pins (6 and 5) */
    reg = GPIOD_AFL & ~(0xf << (USART2_TX_PIN * 4));
    GPIOD_AFL = reg | (USART2_PIN_AF << (USART2_TX_PIN * 4));
    reg = GPIOD_AFL & ~(0xf << (USART2_RX_PIN  * 4));
    GPIOD_AFL = reg | (USART2_PIN_AF << (USART2_RX_PIN * 4));
}

int usart2_setup(uint32_t bitrate, uint8_t data, char parity, uint8_t stop)
{
    uint32_t reg;
    /* Enable pins and configure for AF7 */
    usart2_pins_setup();
    /* Turn on the device */
    APB1_CLOCK_ER |= USART2_APB1_CLOCK_ER_VAL;

    /* Configure for TX + RX */
    USART2_CR1 |= (USART2_CR1_TX_ENABLE | USART2_CR1_RX_ENABLE);

    /* Configure clock */
    USART2_BRR =  CPU_FREQ / bitrate;

    /* Configure data bits */
    if (data == 8)
        USART2_CR1 &= ~USART2_CR1_SYMBOL_LEN;
    else
        USART2_CR1 |= USART2_CR1_SYMBOL_LEN;

    /* Default: No parity */
    USART2_CR1 &= ~(USART2_CR1_PARITY_ENABLED | USART2_CR1_PARITY_ODD);

    /* Configure parity */
    switch (parity) {
        case 'O':
            USART2_CR1 |= USART2_CR1_PARITY_ODD;
            /* fall through to enable parity */
        case 'E':
            USART2_CR1 |= USART2_CR1_PARITY_ENABLED;
            break;
    }
    /* Set stop bits */
    reg = USART2_CR2 & ~USART2_CR2_STOPBITS;
    if (stop > 1)
        USART2_CR2 = reg & (2 << 12);
    else
        USART2_CR2 = reg;

    /* Enable interrupts in NVIC */
    nvic_irq_enable(NVIC_USART2_IRQN);
    nvic_irq_setprio(NVIC_USART2_IRQN, 0);

    /* Enable RX interrupt */
    usart2_rx_interrupt_onoff(1);

    /* Turn on usart */
    USART2_CR1 |= USART2_CR1_USART_ENABLE;

    return 0;
}

static char buf_rx[64];
static int received_bytes = 0;

static char buf_tx[64];
static int tx_pending_bytes = 0, tx_transmitted_bytes = 0;

void isr_usart2(void)
{
    volatile uint32_t reg;
    reg = USART2_SR;
    if (reg & USART2_SR_RX_NOTEMPTY) {
        if (received_bytes >= 64)
            reg = USART2_RDR;
        else
            buf_rx[received_bytes++] = (char)(USART2_RDR & 0xFF);
    }

    if ((reg & USART2_SR_TX_EMPTY) &&
            (tx_pending_bytes > tx_transmitted_bytes)) {
        USART2_TDR = buf_tx[tx_transmitted_bytes++];
        if (tx_transmitted_bytes == tx_pending_bytes) {
            usart2_tx_interrupt_onoff(0);
            tx_transmitted_bytes = tx_pending_bytes = 0;
        }
    }
}

void usart2_write(const char *text)
{
    const char *p = text;
    volatile uint32_t reg;
    while(*p) {
        reg = USART2_SR;
        if (reg & USART2_SR_TX_EMPTY)
        {
            USART2_TDR = *p;
        } else {
            if (tx_pending_bytes >= 63)
                break;
            buf_tx[tx_pending_bytes++] = *p;
            usart2_tx_interrupt_onoff(1);
        }
        p++;
    }
}

int usart2_read(char *buf, int len)
{
    int ret = 0;
    if (received_bytes < len)
        len = received_bytes;
    if (received_bytes > 0) {
        int i;
        for (i = 0; i < len; i++)
            buf[i] = buf_rx[i];
        ret = len;
        received_bytes = 0;
    }
    return ret;
}
