/*
 * @Author: Jee Hsu
 * @Description: 串口调试输出实现
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-LICENSE-Identifier: MIT
 */
#include "config.h"
#include "btypes.h"
#include "arch_types.h"
#include "arch_heads.h"
#include "debug_uart.h"

/**
 * @brief 串口输出单个字符
 */
void debug_uart_putchar(char c) {
    /* 等待发送保持寄存器空 */
    while ((in_u8(DEBUG_UART_PORT + UART_LSR) & UART_LSR_THRE) == 0);
    /* 发送字符 */
    out_u8(DEBUG_UART_PORT + UART_THR, (u8_t)c);
}

/**
 * @brief 串口输出字符串
 */
void debug_uart_puts(const char *s) {
    if (s == NULL) return;
    while (*s) {
        debug_uart_putchar(*s++);
    }
}

/**
 * @brief 简单的数字转字符串（用于调试）
 */
PRIVATE void debug_itoa(uint_t val, char *buf, uint_t base) {
    char *p = buf;
    char *p1, *p2;
    uint_t tmp;
    
    if (val == 0) {
        *p++ = '0';
        *p = '\0';
        return;
    }
    
    while (val) {
        tmp = val % base;
        if (tmp < 10) {
            *p++ = (char)('0' + tmp);
        } else {
            *p++ = (char)('a' + tmp - 10);
        }
        val /= base;
    }
    *p = '\0';
    
    /* 反转字符串 */
    p1 = buf;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp_char = *p1;
        *p1 = *p2;
        *p2 = tmp_char;
        p1++;
        p2--;
    }
}

/**
 * @brief 串口输出格式化字符串（简化版）
 */
void debug_uart_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    char num_buf[32];
    const char *p = fmt;
    
    while (*p) {
        if (*p == '%' && *(p + 1)) {
            p++;
            switch (*p) {
            case 'd': {
                int val = va_arg(args, int);
                debug_itoa((uint_t)val, num_buf, 10);
                debug_uart_puts(num_buf);
                break;
            }
            case 'x':
            case 'X': {
                uint_t val = va_arg(args, uint_t);
                if (val != 0) {
                    debug_uart_puts("0x");
                }
                debug_itoa(val, num_buf, 16);
                debug_uart_puts(num_buf);
                break;
            }
            case 's': {
                const char *s = va_arg(args, const char*);
                debug_uart_puts(s);
                break;
            }
            case 'c': {
                char c = (char)va_arg(args, int);
                debug_uart_putchar(c);
                break;
            }
            default:
                debug_uart_putchar('%');
                debug_uart_putchar(*p);
                break;
            }
        } else {
            debug_uart_putchar(*p);
        }
        p++;
    }
    
    va_end(args);
}

