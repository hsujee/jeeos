/*
 * @Author: Jee Hsu
 * @Description: 内核打印函数 - 格式化输出
 *
 * 提供内核态的格式化输出功能，支持：
 * - %d 十进制整数
 * - %x 十六进制整数
 * - %s 字符串
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_PRINTK_H
#define ARCH_X86_PRINTK_H

/**
 * @brief 内核格式化打印函数
 * @param fmt 格式字符串
 * @param ... 可变参数
 */
void printk(const char* fmt, ...);

/**
 * @brief 字符串拷贝
 * @param buf 目标缓冲区
 * @param str_s 源字符串
 * @return 目标缓冲区末尾指针
 */
char_t* strcopyk(char_t* buf, char_t* str_s);

/**
 * @brief 格式化字符串到缓冲区
 * @param buf 目标缓冲区
 * @param fmt 格式字符串
 * @param args 可变参数列表
 */
void vsprintfk(char* buf, const char* fmt, va_list args);

/**
 * @brief 整数转字符串
 * @param str 目标缓冲区
 * @param n 整数值
 * @param base 进制(10或16)
 * @return 目标缓冲区末尾指针
 */
char_t* numberk(char_t* str, uint_t n, sint_t base);

/**
 * @brief 输出字符到串口
 * @param buf 字符缓冲区
 */
void char_write_uart(char_t* buf);

#endif // ARCH_X86_PRINTK_H
