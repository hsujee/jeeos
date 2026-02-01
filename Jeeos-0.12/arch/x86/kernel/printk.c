/*
 * @Author: Jee Hsu
 * @Description: 内核格式化打印函数
 *
 * 实现内核的格式化打印功能：
 * - printk(): 格式化输出到视频和串口
 * - vsprintfk(): 格式化字符串
 * - numberk(): 数字转字符串
 *
 * 支持的格式说明符：
 * - %x: 十六进制无符号整数
 * - %d: 十进制无符号整数
 * - %s: 字符串
 *
 * 输出目标：
 * - 视频驱动(帧缓冲区)
 * - 串口COM1(0x3F8)用于调试
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

// 串口输出
static inline void serial_putchar(char c) {
    while ((in_u8(0x3F8 + 5) & 0x20) == 0);
    out_u8(0x3F8, c);
}

static void serial_puts(const char *s) {
    while (*s) {
        if (*s == '\n') serial_putchar('\r');
        serial_putchar(*s++);
    }
}

void printk(const char_t *fmt, ...) {
	char_t buf[512];
	va_list ap;
	va_start(ap, fmt);
	vsprintfk(buf, fmt, ap);
	gstr_write(&kdftgh, buf);
	serial_puts(buf);  // 同时输出到串口
	va_end(ap);
	return;
}

char_t *strcopyk(char_t *buf, char_t *str_s) {
	while (*str_s) {
		*buf = *str_s;
		buf++;
		str_s++;
	}
	return buf;
}

void vsprintfk(char_t *buf, const char_t *fmt, va_list args) {
	char_t *p = buf;
	while (*fmt) {
		if (*fmt != '%') {
			*p++ = *fmt++;
			continue;
		}
		fmt++;
		switch (*fmt) {
			case 'c':
				*p++ = (char_t)va_arg(args, int);
				fmt++;
				break;
			case 'x':
				p = numberk(p, va_arg(args, uint_t), 16);
				fmt++;
				break;
			case 'd':
				p = numberk(p, va_arg(args, uint_t), 10);
				fmt++;
				break;
			case 's':
				p = strcopyk(p, (char_t *)va_arg(args, uint_t));
				fmt++;
				break;
			default:
				break;
		}
	}
	*p = 0;
	return;
}

char_t *numberk(char_t *str, uint_t n, sint_t base) {
	register char_t *p;
	char_t strbuf[36];
	p = &strbuf[36];
	*--p = 0;
	if (n == 0) {
		*--p = '0';
	} else {
		do {
			*--p = "0123456789abcdef"[n % base];
		} while (n /= base);
	}
	while (*p != 0) {
		*str++ = *p++;
	}
	return str;
}
