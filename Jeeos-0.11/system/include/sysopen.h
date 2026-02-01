/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - open打开操作
 *
 * 提供文件/设备打开的系统调用接口（桩实现）
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _SYS_OPEN_H
#define _SYS_OPEN_H

/**
 * @brief 系统调用表open入口
 * @param inr 系统调用号
 * @param stkparv 栈参数
 * @return 系统调用状态码
 */
sysstus_t syscall_open(uint_t inr, stkparame_t* stkparv);

/**
 * @brief 打开文件/设备
 * @param file 文件路径/设备名
 * @param flgs 打开标志
 * @param stus 打开状态
 * @return 文件句柄，失败返回NO_HAND
 */
hand_t sys_open(void *file, uint_t flags, uint_t stus);

/**
 * @brief 打开文件/设备核心函数
 * @param file 文件路径/设备名
 * @param flgs 打开标志
 * @param stus 打开状态
 * @return 文件句柄，失败返回NO_HAND
 */
hand_t sys_core_open(void *file, uint_t flags, uint_t stus);

/**
 * @brief 设备打开操作
 * @param ondep 对象节点指针
 * @return 系统调用状态码
 */
sysstus_t sys_open_device(objnode_t* ondep);

#endif // _SYS_OPEN_H
