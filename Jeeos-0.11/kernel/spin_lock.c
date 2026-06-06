/*
 * @Author: Jee Hsu
 * @Description: 自旋锁包装层 - 调用架构层实现
 *
 * 自旋锁是内核中最基本的同步原语，用于保护短期临界区。
 * 实际实现位于 arch/x86/kernel/cpu_ops.c 中。
 *
 * 主要函数：
 * - spinlock_init()     : 初始化自旋锁
 * - spinlock_lock()     : 获取锁（忙等待）
 * - spinlock_unlock()   : 释放锁
 * - spinlock_cli()      : 关中断并获取锁
 * - spinunlock_sti()    : 释放锁并恢复中断
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

/*
 * 自旋锁的架构相关实现在 arch/x86/kernel/cpu_ops.c 中
 * 本文件保留用于未来可能的内核层扩展
 */
