/*
 * @Author: Jee Hsu
 * @Description: 统一错误处理框架实现
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "config.h"
#include "btypes.h"
#include "arch_types.h"
#include "kernel_types.h"
#include "systypes.h"
#include "arch_heads.h"
#include "kernel_heads.h"
#include "kernel_error.h"

/**
 * @brief 错误级别字符串
 */
PRIVATE const char *err_level_str[] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

/**
 * @brief 内核错误处理函数
 */
void kernel_error(err_level_t level, const char *module, const char *msg) {
    /* 打印错误信息 */
    if (level >= ERR_LEVEL_WARN) {
        printk("\n[%s] %s: %s\n", err_level_str[level], module, msg);
    }
    
    /* 致命错误，停止系统 */
    if (level == ERR_LEVEL_FATAL) {
        printk("\n");
        printk("========================================\n");
        printk("  KERNEL PANIC - System Halted\n");
        printk("========================================\n");
        printk("Module: %s\n", module);
        printk("Message: %s\n", msg);
        printk("========================================\n");
        printk("\n");
        
        /* 停止系统 */
        die(0xFF);
    }
}

