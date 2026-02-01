/*
 * @Author: Jee Hsu
 * @Description: 标准字符串和内存操作函数实现
 *
 * 实现 C 标准库兼容的字符串和内存操作
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "string.h"

/*===========================================================================
 * 内存操作函数
 *===========================================================================*/

void *memcpy(void *dest, const void *src, size_t n) {
    char *d = (char *)dest;
    const char *s = (const char *)src;
    
    /* 尝试使用字对齐复制提高效率 */
    if (n >= sizeof(u64_t) && 
        ((u64_t)d & 7) == 0 && 
        ((u64_t)s & 7) == 0) {
        u64_t *d64 = (u64_t *)d;
        const u64_t *s64 = (const u64_t *)s;
        while (n >= sizeof(u64_t)) {
            *d64++ = *s64++;
            n -= sizeof(u64_t);
        }
        d = (char *)d64;
        s = (const char *)s64;
    }
    
    /* 复制剩余字节 */
    while (n--) {
        *d++ = *s++;
    }
    
    return dest;
}

void *memset(void *s, int c, size_t n) {
    unsigned char *p = (unsigned char *)s;
    unsigned char val = (unsigned char)c;
    
    /* 尝试使用字对齐填充提高效率 */
    if (n >= sizeof(u64_t) && ((u64_t)p & 7) == 0) {
        u64_t val64 = val;
        val64 |= val64 << 8;
        val64 |= val64 << 16;
        val64 |= val64 << 32;
        
        u64_t *p64 = (u64_t *)p;
        while (n >= sizeof(u64_t)) {
            *p64++ = val64;
            n -= sizeof(u64_t);
        }
        p = (unsigned char *)p64;
    }
    
    /* 填充剩余字节 */
    while (n--) {
        *p++ = val;
    }
    
    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    char *d = (char *)dest;
    const char *s = (const char *)src;
    
    if (d == s || n == 0) {
        return dest;
    }
    
    /* 如果目标在源之后且有重叠，从后向前复制 */
    if (d > s && d < s + n) {
        d += n;
        s += n;
        while (n--) {
            *--d = *--s;
        }
    } else {
        /* 否则从前向后复制 */
        while (n--) {
            *d++ = *s++;
        }
    }
    
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;
    
    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    
    return 0;
}

void *memchr(const void *s, int c, size_t n) {
    const unsigned char *p = (const unsigned char *)s;
    unsigned char val = (unsigned char)c;
    
    while (n--) {
        if (*p == val) {
            return (void *)p;
        }
        p++;
    }
    
    return NULL;
}

/*===========================================================================
 * 字符串长度和复制
 *===========================================================================*/

size_t strlen(const char *s) {
    const char *p = s;
    while (*p) {
        p++;
    }
    return p - s;
}

size_t strnlen(const char *s, size_t maxlen) {
    const char *p = s;
    while (maxlen-- && *p) {
        p++;
    }
    return p - s;
}

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++))
        ;
    return dest;
}

char *strncpy(char *dest, const char *src, size_t n) {
    char *d = dest;
    
    /* 复制字符串 */
    while (n && (*d = *src)) {
        d++;
        src++;
        n--;
    }
    
    /* 用0填充剩余空间 */
    while (n--) {
        *d++ = '\0';
    }
    
    return dest;
}

/*===========================================================================
 * 字符串比较
 *===========================================================================*/

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    if (n == 0) {
        return 0;
    }
    
    while (--n && *s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    
    return (unsigned char)*s1 - (unsigned char)*s2;
}

/*===========================================================================
 * 字符串拼接
 *===========================================================================*/

char *strcat(char *dest, const char *src) {
    char *d = dest;
    
    /* 找到目标字符串的结尾 */
    while (*d) {
        d++;
    }
    
    /* 追加源字符串 */
    while ((*d++ = *src++))
        ;
    
    return dest;
}

char *strncat(char *dest, const char *src, size_t n) {
    char *d = dest;
    
    /* 找到目标字符串的结尾 */
    while (*d) {
        d++;
    }
    
    /* 追加最多n个字符 */
    while (n-- && *src) {
        *d++ = *src++;
    }
    
    *d = '\0';
    return dest;
}

/*===========================================================================
 * 字符串查找
 *===========================================================================*/

char *strchr(const char *s, int c) {
    char ch = (char)c;
    
    while (*s) {
        if (*s == ch) {
            return (char *)s;
        }
        s++;
    }
    
    /* 也检查终止符 */
    if (ch == '\0') {
        return (char *)s;
    }
    
    return NULL;
}

char *strrchr(const char *s, int c) {
    const char *last = NULL;
    char ch = (char)c;
    
    while (*s) {
        if (*s == ch) {
            last = s;
        }
        s++;
    }
    
    /* 也检查终止符 */
    if (ch == '\0') {
        return (char *)s;
    }
    
    return (char *)last;
}

char *strstr(const char *haystack, const char *needle) {
    size_t needle_len;
    
    if (*needle == '\0') {
        return (char *)haystack;
    }
    
    needle_len = strlen(needle);
    
    while (*haystack) {
        if (*haystack == *needle) {
            if (strncmp(haystack, needle, needle_len) == 0) {
                return (char *)haystack;
            }
        }
        haystack++;
    }
    
    return NULL;
}

