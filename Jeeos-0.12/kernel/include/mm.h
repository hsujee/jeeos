/*
 * @Author: Jee Hsu
 * @Description: 内存管理模块 - 内存分配与释放
 *
 * 提供内核级内存管理接口，封装底层页分配和SLAB分配器
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _MM_H
#define _MM_H

/**
 * @brief 初始化内存管理子系统
 */
void init_mm();

/**
 * @brief 分配指定大小的内存
 * @param mmsize 请求的内存大小(字节)
 * @return 成功返回内存地址，失败返回NULL
 */
adr_t new_mem(size_t mmsize);

/**
 * @brief 释放已分配的内存
 * @param fradr 要释放的内存起始地址
 * @param frsz 要释放的内存大小
 * @return TRUE成功，FALSE失败
 */
bool_t delete_mem(adr_t fradr, size_t frsz);

#endif // _MM_H
