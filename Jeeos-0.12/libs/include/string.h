/*
 * @Author: Jee Hsu
 * @Description: 标准字符串和内存操作函数
 *
 * 提供 C 标准库兼容的字符串和内存操作：
 * - 内存操作: memcpy, memset, memmove, memcmp
 * - 字符串操作: strlen, strcpy, strncpy, strcmp, strncmp
 * - 字符串拼接: strcat, strncat
 * - 字符串查找: strchr, strrchr, strstr
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _STRING_H
#define _STRING_H

#include "types/typedef.h"

/*===========================================================================
 * 内存操作函数
 *===========================================================================*/

/**
 * @brief 内存复制
 * @param dest 目标地址
 * @param src 源地址
 * @param n 复制字节数
 * @return 目标地址
 * @note 源和目标内存区域不能重叠
 */
void *memcpy(void *dest, const void *src, size_t n);

/**
 * @brief 内存设置
 * @param s 目标地址
 * @param c 填充字节值
 * @param n 填充字节数
 * @return 目标地址
 */
void *memset(void *s, int c, size_t n);

/**
 * @brief 内存移动 (支持重叠区域)
 * @param dest 目标地址
 * @param src 源地址
 * @param n 移动字节数
 * @return 目标地址
 */
void *memmove(void *dest, const void *src, size_t n);

/**
 * @brief 内存比较
 * @param s1 内存区域1
 * @param s2 内存区域2
 * @param n 比较字节数
 * @return 0相等，<0 s1<s2，>0 s1>s2
 */
int memcmp(const void *s1, const void *s2, size_t n);

/**
 * @brief 在内存中查找字节
 * @param s 内存区域
 * @param c 要查找的字节
 * @param n 搜索范围
 * @return 找到返回位置指针，否则返回NULL
 */
void *memchr(const void *s, int c, size_t n);

/*===========================================================================
 * 字符串长度和复制
 *===========================================================================*/

/**
 * @brief 计算字符串长度
 * @param s 字符串指针
 * @return 字符串长度（不含终止符）
 */
size_t strlen(const char *s);

/**
 * @brief 计算有限长度字符串长度
 * @param s 字符串指针
 * @param maxlen 最大检查长度
 * @return 字符串长度或maxlen（取较小值）
 */
size_t strnlen(const char *s, size_t maxlen);

/**
 * @brief 复制字符串
 * @param dest 目标缓冲区
 * @param src 源字符串
 * @return 目标地址
 * @warning 不检查缓冲区溢出，建议使用strncpy
 */
char *strcpy(char *dest, const char *src);

/**
 * @brief 复制有限长度字符串
 * @param dest 目标缓冲区
 * @param src 源字符串
 * @param n 最大复制字符数
 * @return 目标地址
 */
char *strncpy(char *dest, const char *src, size_t n);

/*===========================================================================
 * 字符串比较
 *===========================================================================*/

/**
 * @brief 比较字符串
 * @param s1 字符串1
 * @param s2 字符串2
 * @return 0相等，<0 s1<s2，>0 s1>s2
 */
int strcmp(const char *s1, const char *s2);

/**
 * @brief 比较有限长度字符串
 * @param s1 字符串1
 * @param s2 字符串2
 * @param n 最大比较字符数
 * @return 0相等，<0 s1<s2，>0 s1>s2
 */
int strncmp(const char *s1, const char *s2, size_t n);

/*===========================================================================
 * 字符串拼接
 *===========================================================================*/

/**
 * @brief 拼接字符串
 * @param dest 目标字符串
 * @param src 源字符串
 * @return 目标地址
 * @warning 不检查缓冲区溢出，建议使用strncat
 */
char *strcat(char *dest, const char *src);

/**
 * @brief 拼接有限长度字符串
 * @param dest 目标字符串
 * @param src 源字符串
 * @param n 最大追加字符数
 * @return 目标地址
 */
char *strncat(char *dest, const char *src, size_t n);

/*===========================================================================
 * 字符串查找
 *===========================================================================*/

/**
 * @brief 在字符串中查找字符（正向）
 * @param s 字符串
 * @param c 要查找的字符
 * @return 找到返回位置指针，否则返回NULL
 */
char *strchr(const char *s, int c);

/**
 * @brief 在字符串中查找字符（反向）
 * @param s 字符串
 * @param c 要查找的字符
 * @return 找到返回最后出现位置，否则返回NULL
 */
char *strrchr(const char *s, int c);

/**
 * @brief 在字符串中查找子串
 * @param haystack 被搜索的字符串
 * @param needle 要查找的子串
 * @return 找到返回位置指针，否则返回NULL
 */
char *strstr(const char *haystack, const char *needle);

/*===========================================================================
 * 字符串分割和转换
 *===========================================================================*/

/**
 * @brief 分割字符串
 * @param str 要分割的字符串(首次调用)或NULL(后续调用)
 * @param delim 分隔符字符串
 * @return 下一个token，或NULL表示结束
 */
char *strtok(char *str, const char *delim);

/**
 * @brief 判断字符是否为空白字符
 * @param c 要判断的字符
 * @return 非0为空白字符，0不是
 */
int isspace(int c);

/**
 * @brief 判断字符是否为数字
 * @param c 要判断的字符
 * @return 非0为数字，0不是
 */
int isdigit(int c);

/**
 * @brief 字符串转整数
 * @param str 字符串
 * @return 转换后的整数值
 */
int atoi(const char *str);

#endif /* _STRING_H */

