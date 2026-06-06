/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - 内存管理(mmap/munmap)
 *
 * 实现内存映射相关系统调用：
 * - mmap: 映射文件或匿名内存
 * - munmap: 解除内存映射
 * - mprotect: 修改内存保护属性
 *
 * 函数接口：
 * - syscall_mmap(): 系统调用表入口
 * - sys_mmap(): 内存映射
 * - sys_munmap(): 解除映射
 *
 * 当前状态: 桩实现(待完善)
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "sys_deps.h"

// 系统调用表入口 - 桩实现
sysstus_t syscall_mmap(uint_t inr, stkparame_t *stkparv) {
    (void)inr;
    (void)stkparv;
    return SYSSTUSERR;
}

// 内存映射接口 - 桩实现
void *sys_mmap(void *addr, size_t len, uint_t prot, uint_t flags) {
    (void)addr;
    (void)len;
    (void)prot;
    (void)flags;
    return NULL;
}

// 内存取消映射接口 - 桩实现  
sysstus_t sys_munmap(void *addr, size_t len) {
    (void)addr;
    (void)len;
    return SYSSTUSERR;
}
