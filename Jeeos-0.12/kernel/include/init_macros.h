/*
 * @Author: Jee Hsu
 * @Description: 通用初始化宏定义
 *
 * 提供统一的初始化宏，简化重复的初始化代码
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _INIT_MACROS_H
#define _INIT_MACROS_H

/**
 * @brief 将结构体清零
 */
#define INIT_STRUCT_ZERO(ptr) \
    arch_memset((ptr), 0, sizeof(*(ptr)))

/**
 * @brief 初始化包含自旋锁和链表的结构
 */
#define INIT_SPINLOCK_LIST(ptr, lock_field, list_field) \
    do { \
        spinlock_init(&(ptr)->lock_field); \
        list_init(&(ptr)->list_field); \
    } while(0)

/**
 * @brief 初始化设备通用字段
 */
#define INIT_DEVICE_COMMON(devp) \
    do { \
        spinlock_init(&(devp)->dev_lock); \
        list_init(&(devp)->dev_list); \
        (devp)->dev_status = 0; \
        (devp)->dev_flags = 0; \
    } while(0)

/**
 * @brief 初始化驱动通用字段
 */
#define INIT_DRIVER_COMMON(drvp) \
    do { \
        spinlock_init(&(drvp)->drv_lock); \
        list_init(&(drvp)->drv_list); \
        (drvp)->drv_status = 0; \
        (drvp)->drv_flags = 0; \
    } while(0)

/**
 * @brief 初始化包含状态和标志的结构
 */
#define INIT_STATUS_FLAGS(ptr, status_field, flags_field) \
    do { \
        (ptr)->status_field = 0; \
        (ptr)->flags_field = 0; \
    } while(0)

#endif // _INIT_MACROS_H

