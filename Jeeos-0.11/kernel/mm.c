/*
 * @Author: Jee Hsu
 * @Description: 内存管理入口 - 内核级内存分配
 *
 * 提供内核和用户态的内存分配接口：
 * - init_mm(): 初始化内存子系统
 * - new_mem(): 分配内存
 * - del_mem(): 释放内存
 *
 * 初始化流程：
 * 1. init_mmpages() - 初始化SLAB分配器
 * 2. init_kvirmemadrs() - 初始化内核虚拟地址空间
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

void init_mm() {
    init_mmpages();
    // 初始化内核虚拟地址空间
    init_kvirmemadrs();
    return;
}

adr_t new_mem(size_t mmsize) {
    if (mmsize == MALCSZ_MIN || mmsize > MALCSZ_MAX) {
        return NULL;
    }
    return kmempool_new(mmsize);
}

bool_t delete_mem(adr_t fradr, size_t frsz) {
    if (fradr == NULL || frsz == MALCSZ_MIN || frsz > MALCSZ_MAX) {
        return FALSE;
    }
    return kmempool_delete(fradr, frsz);
}
