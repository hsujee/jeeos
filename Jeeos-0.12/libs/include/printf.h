/*
 * @Author: Jee Hsu
 * @Description: 格式化打印函数
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef PRINTF_H
#define PRINTF_H
typedef __builtin_va_list va_list;
#define va_start(ap,np) __builtin_va_start(ap,np)
#define va_end(ap) __builtin_va_end(ap)
#define va_arg(ap,ty) __builtin_va_arg(ap,ty)

int printf(const char* fmt,...);
char* strcopy(char* buf,char* str_s);
void vsprintf(char* buf,const char* fmt,va_list args);
char* number(char* str,uint_t n, sint_t base);

#endif // PRINTF_H
