/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - 内存管理
 *
 * 提供内存映射的系统调用接口（桩实现）
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SYS_MM_H
#define _SYS_MM_H

/**
 * @brief 系统调用表mmap入口
 * @param inr 系统调用号
 * @param stkparv 栈参数
 * @return 系统调用状态码
 */
sysstus_t syscall_mmap(uint_t inr, stkparame_t* stkparv);

/**
 * @brief 内存映射
 * @param addr 映射地址
 * @param len 映射长度
 * @param prot 保护标志
 * @param flags 映射标志
 * @return 映射地址，失败返回NULL
 */
void *sys_mmap(void *addr, size_t len, uint_t prot, uint_t flags);

/**
 * @brief 取消内存映射
 * @param addr 映射地址
 * @param len 映射长度
 * @return 系统调用状态码
 */
sysstus_t sys_munmap(void *addr, size_t len);

#endif // _SYS_MM_H
