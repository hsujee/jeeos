/*
 * @Author: Jee Hsu
 * @Description: VGA文本模式控制台驱动
 *
 * 提供基于VGA文本模式(0xB8000)的屏幕输出功能，
 * 包括清屏、光标控制、字符串输出等基本操作。
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <stdint.h>

/**
 * @brief 清除屏幕内容
 * 
 * 将整个VGA文本缓冲区填充为空格字符，
 * 并将光标位置重置到屏幕左上角(0,0)。
 */
void clear_screen(void);

/**
 * @brief 更新硬件光标位置
 * 
 * 将软件维护的光标坐标同步到VGA硬件光标，
 * 通过0x3D4/0x3D5端口写入光标位置寄存器。
 */
void update_cursor(void);

/**
 * @brief 输出字符串到屏幕
 * 
 * 将字符串逐字符写入VGA显存，自动处理换行符，
 * 当屏幕写满时自动滚屏。
 * 
 * @param string 要输出的以'\0'结尾的字符串
 */
void _strwrite(const char* string);

/**
 * @brief 格式化输出函数
 * 
 * 目前仅支持纯字符串输出，后续可扩展格式化功能。
 * 
 * @param fmt 格式字符串
 */
void printf(const char* fmt);

#endif /* _CONSOLE_H */
