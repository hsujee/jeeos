/*
 * @Author: Jee Hsu
 * @Description: 系统服务 - sysstr
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SYS_STR_H
#define _SYS_STR_H
int imrand();
int sysrand(int min,int max);
sint_t sys_strcmp(char_t* str_s,char_t* str_d);
sint_t sys_strlen(char* str_s);
sint_t sys_strcpy(char_t* str_s,char_t* str_d);
sint_t sys_memcopy(void* src, void* dest,uint_t count);
sint_t sys_memset(void* s,u8_t c,uint_t count);
#endif
