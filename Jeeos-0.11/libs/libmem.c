/*
 * @Author: Jee Hsu
 * @Description: 用户态库 - 内存管理
 *
 * 提供用户态的内存分配函数：
 * - mallocblk(): 分配内存块
 * - mfreeblk(): 释放内存块
 *
 * 底层实现：
 * - 通过系统调用触发内核的mmap/munmap
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "libtypes.h"
#include "libsyscalls.h"

/**
 * @brief 分配内存块
 * @param blksz 需要分配的内存大小（字节）
 * @return 成功返回内存地址，失败返回NULL
 */
void* mallocblk(size_t blksz) {
    void* retadr;
    __SYSCALL_PARAM1(INR_MM_ALLOC, retadr, blksz);
    return retadr;
}

/**
 * @brief 释放内存块
 * @param fradr 要释放的内存地址
 * @param blksz 内存块大小
 * @return 成功返回SYSSTUSOK，失败返回SYSSTUSERR
 */
sysstus_t mfreeblk(void* fradr, size_t blksz) {
    sysstus_t retstus;
    __SYSCALL_PARAM2(INR_MM_FREE, retstus, fradr, blksz);
    return retstus;
}

