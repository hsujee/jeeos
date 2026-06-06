/*
 * @Author: Jee Hsu
 * @Description: CPU操作函数 - 自旋锁、内存操作、中断控制
 *
 * 提供CPU级别的原子操作和底层功能，包括：
 * - 自旋锁的初始化、加锁、解锁
 * - 中断标志的保存和恢复
 * - 内存拷贝和设置
 * - 系统错误处理
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_CPU_OPS_H
#define ARCH_X86_CPU_OPS_H

/**
 * @brief 写回并使缓存无效
 * 
 * 执行WBINVD指令，将缓存写回内存并使缓存无效
 */
void arch_wbinvd();

/**
 * @brief 使缓存无效
 * 
 * 执行INVD指令，使缓存无效（不写回）
 */
void arch_invd();

/**
 * @brief 恢复中断标志并开中断
 * @param cpuflg 保存的CPU标志指针
 */
void arch_sti_cpuflag(cpuflg_t* cpuflg);

/**
 * @brief 保存中断标志并关中断
 * @param cpuflg 用于保存CPU标志的指针
 */
void arch_cli_cpuflag(cpuflg_t* cpuflg);

/**
 * @brief 从保存的标志恢复中断状态
 * @param cpuflg 保存的CPU标志指针
 */
void arch_cpuflag_sti(cpuflg_t* cpuflg);

/**
 * @brief 保存当前中断标志
 * @param cpuflg 用于保存CPU标志的指针
 */
void arch_cpuflag_cli(cpuflg_t* cpuflg);

/**
 * @brief 初始化自旋锁
 * @param lock 自旋锁指针
 */
void arch_spinlock_init(spinlock_t *lock);

/**
 * @brief 获取自旋锁
 * @param lock 自旋锁指针
 */
void arch_spinlock_lock(spinlock_t *lock);

/**
 * @brief 释放自旋锁
 * @param lock 自旋锁指针
 */
void arch_spinlock_unlock(spinlock_t *lock);

/**
 * @brief 保存标志、关中断并获取自旋锁
 * @param lock 自旋锁指针
 * @param cpuflg 用于保存CPU标志的指针
 */
void arch_spinlock_saveflg_cli(spinlock_t *lock, cpuflg_t *cpuflg);

/**
 * @brief 获取自旋锁（简化接口）
 * @param lock 自旋锁指针
 */
void spinlock(spinlock_t * lock);

/**
 * @brief 释放自旋锁（简化接口）
 * @param lock 自旋锁指针
 */
void spinunlock(spinlock_t * lock);

/**
 * @brief 释放自旋锁并恢复中断标志
 * @param lock 自旋锁指针
 * @param cpuflg 保存的CPU标志指针
 */
void arch_spinunlock_restflg_sti(spinlock_t *lock, cpuflg_t *cpuflg);

/**
 * @brief 初始化自旋锁（通用接口）
 * @param lock 自旋锁指针
 */
void spinlock_init(spinlock_t *lock);

/**
 * @brief 获取自旋锁（通用接口）
 * @param lock 自旋锁指针
 */
void spinlock_lock(spinlock_t *lock);

/**
 * @brief 释放自旋锁（通用接口）
 * @param lock 自旋锁指针
 */
void spinlock_unlock(spinlock_t *lock);

/**
 * @brief 关中断并获取自旋锁
 * @param lock 自旋锁指针
 * @param cpuflg 用于保存CPU标志的指针
 */
void spinlock_cli(spinlock_t *lock, cpuflg_t *cpuflg);

/**
 * @brief 释放自旋锁并开中断
 * @param lock 自旋锁指针
 * @param cpuflg 保存的CPU标志指针
 */
void spinunlock_sti(spinlock_t *lock, cpuflg_t *cpuflg);

/**
 * @brief 内存填充
 * @param setp 目标内存地址
 * @param setval 填充值
 * @param n 填充字节数
 */
void arch_memset(void *setp, u8_t setval, size_t n);

/**
 * @brief 内存拷贝
 * @param src 源地址
 * @param dst 目标地址
 * @param n 拷贝字节数
 */
void arch_memcpy(void *src, void *dst, size_t n);

/**
 * @brief 系统致命错误处理
 * @param errmsg 错误信息字符串
 */
void arch_sysdie(char_t *errmsg);

/**
 * @brief 系统错误处理
 * @param errmsg 错误信息字符串
 */
void system_error(char_t *errmsg);

/**
 * @brief 获取当前CPU ID
 * @return 当前CPU的ID号
 */
uint_t arch_retn_cpuid();

#endif // ARCH_X86_CPU_OPS_H
