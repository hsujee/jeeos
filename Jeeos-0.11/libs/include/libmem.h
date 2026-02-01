/*
 * @Author: Jee Hsu
 * @Description: 用户态库 - 内存管理接口声明
 *
 * 提供用户态内存分配接口：
 * - mallocblk: 分配内存块
 * - mfreeblk: 释放内存块
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _LIBMEM_H
#define _LIBMEM_H

#include "libtypes.h"

/* 内存分配操作 */
void*     mallocblk(size_t blksz);
sysstus_t mfreeblk(void* fradr, size_t blksz);

#endif /* _LIBMEM_H */

