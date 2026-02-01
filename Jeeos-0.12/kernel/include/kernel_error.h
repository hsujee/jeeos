/*
 * @Author: Jee Hsu
 * @Description: 统一错误处理框架
 *
 * 提供内核统一的错误处理接口，替代分散的错误处理方式
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _KERNEL_ERROR_H
#define _KERNEL_ERROR_H

/**
 * @brief 错误级别定义
 */
typedef enum {
    ERR_LEVEL_DEBUG = 0,    /* 调试信息 */
    ERR_LEVEL_INFO,         /* 一般信息 */
    ERR_LEVEL_WARN,         /* 警告 */
    ERR_LEVEL_ERROR,        /* 错误 */
    ERR_LEVEL_FATAL         /* 致命错误，导致系统停止 */
} err_level_t;

/**
 * @brief 内核错误处理函数
 * @param level 错误级别
 * @param module 模块名称（通常使用__FILE__）
 * @param msg 错误消息
 */
void kernel_error(err_level_t level, const char *module, const char *msg);

/*===========================================================================
 * 便捷宏定义
 *===========================================================================*/

/**
 * @brief 致命错误，系统将停止运行
 */
#define KERNEL_PANIC(msg) kernel_error(ERR_LEVEL_FATAL, __FILE__, msg)

/**
 * @brief 错误，但系统可以继续运行
 */
#define KERNEL_ERROR(msg) kernel_error(ERR_LEVEL_ERROR, __FILE__, msg)

/**
 * @brief 警告信息
 */
#define KERNEL_WARN(msg) kernel_error(ERR_LEVEL_WARN, __FILE__, msg)

/**
 * @brief 调试信息
 */
#define KERNEL_DEBUG(msg) kernel_error(ERR_LEVEL_DEBUG, __FILE__, msg)

/*===========================================================================
 * 错误检查辅助宏
 *===========================================================================*/

/**
 * @brief 检查指针，如果为NULL则返回NULL
 */
#define CHECK_PTR_RET_NULL(ptr, msg) \
    do { \
        if ((ptr) == NULL) { \
            KERNEL_ERROR(msg); \
            return NULL; \
        } \
    } while(0)

/**
 * @brief 检查条件，如果为真则返回错误状态
 */
#define CHECK_COND_RET_ERR(cond, msg) \
    do { \
        if (cond) { \
            KERNEL_ERROR(msg); \
            return DFCERRSTUS; \
        } \
    } while(0)

/**
 * @brief 检查条件，如果为真则返回FALSE
 */
#define CHECK_COND_RET_FALSE(cond, msg) \
    do { \
        if (cond) { \
            KERNEL_ERROR(msg); \
            return FALSE; \
        } \
    } while(0)

/**
 * @brief 检查条件，如果为真则触发panic
 */
#define CHECK_COND_PANIC(cond, msg) \
    do { \
        if (cond) { \
            KERNEL_PANIC(msg); \
        } \
    } while(0)

#endif // _KERNEL_ERROR_H

