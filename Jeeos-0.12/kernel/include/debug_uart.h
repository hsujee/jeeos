/*
 * @Author: Jee Hsu
 * @Description: 串口调试输出接口
 *
 * 用于调试输出，避免屏幕被覆盖
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _DEBUG_UART_H
#define _DEBUG_UART_H

#include "btypes.h"

/* COM1 端口地址 */
#define DEBUG_UART_PORT 0x3F8

/* 线路状态寄存器 */
#define UART_LSR 5
#define UART_LSR_THRE 0x20  /* 发送保持寄存器空 */

/* 发送保持寄存器 */
#define UART_THR 0

/**
 * @brief 串口输出单个字符
 */
void debug_uart_putchar(char c);

/**
 * @brief 串口输出字符串
 */
void debug_uart_puts(const char *s);

/**
 * @brief 串口输出格式化字符串（简化版，只支持 %d, %x, %s）
 */
void debug_uart_printf(const char *fmt, ...);

#endif // _DEBUG_UART_H

