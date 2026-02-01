/*
 * @Author: Jee Hsu
 * @Description: 原子操作 - x86原子整数操作
 *
 * 提供原子整数操作和引用计数，使用x86 LOCK前缀实现
 * 包含32位和64位原子操作、内存屏障、CAS操作
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _ATOMIC_H
#define _ATOMIC_H

/*===========================================================================
 * 内存屏障 (Memory Barriers)
 *===========================================================================*/

/**
 * @brief 全内存屏障 - 保证屏障前后的读写操作不会乱序
 */
#define mb()    __asm__ __volatile__("mfence" ::: "memory")

/**
 * @brief 读内存屏障 - 保证屏障前后的读操作不会乱序
 */
#define rmb()   __asm__ __volatile__("lfence" ::: "memory")

/**
 * @brief 写内存屏障 - 保证屏障前后的写操作不会乱序
 */
#define wmb()   __asm__ __volatile__("sfence" ::: "memory")

/**
 * @brief SMP全内存屏障
 */
#define smp_mb()    mb()

/**
 * @brief SMP读内存屏障
 */
#define smp_rmb()   rmb()

/**
 * @brief SMP写内存屏障
 */
#define smp_wmb()   wmb()

/**
 * @brief 编译器屏障 - 阻止编译器重排序
 */
#define barrier()   __asm__ __volatile__("" ::: "memory")

/*===========================================================================
 * 32位原子操作
 *===========================================================================*/

/**
 * @brief 原子读取
 * @param v 原子变量指针
 * @return 当前值
 */
KLINE s32_t atomic_read(const atomic_t *v) {
    return (*(volatile u32_t *)&(v)->a_count);
}

/**
 * @brief 原子设置
 * @param v 原子变量指针
 * @param i 要设置的值
 */
KLINE void atomic_set(atomic_t *v, int i) {
    v->a_count = i;
}

/**
 * @brief 原子加法
 * @param i 加数
 * @param v 原子变量指针
 */
KLINE void atomic_add(int i, atomic_t *v) {
    __asm__ __volatile__("lock;"
                            "addl %1,%0"
                            : "+m"(v->a_count)
                            : "ir"(i));
}

/**
 * @brief 原子减法
 * @param i 减数
 * @param v 原子变量指针
 */
KLINE void atomic_sub(int i, atomic_t *v) {
    __asm__ __volatile__("lock;"
                            "subl %1,%0"
                            : "+m"(v->a_count)
                            : "ir"(i));
}

/**
 * @brief 原子减法并测试是否为零
 * @param i 减数
 * @param v 原子变量指针
 * @return 结果为零返回非零值
 */
KLINE int atomic_sub_and_test(int i, atomic_t *v) {
    unsigned char c;
    __asm__ __volatile__("lock;"
                            "subl %2,%0; sete %1"
                            : "+m"(v->a_count), "=qm"(c)
                            : "ir"(i)
                            : "memory");
    return c;
}

/**
 * @brief 原子自增
 * @param v 原子变量指针
 */
KLINE void atomic_inc(atomic_t *v) {
    __asm__ __volatile__("lock;"
                            "incl %0"
                            : "+m"(v->a_count));
}

/**
 * @brief 原子自减
 * @param v 原子变量指针
 */
KLINE void atomic_dec(atomic_t *v) {
    __asm__ __volatile__("lock;"
                            "decl %0"
                            : "+m"(v->a_count));
}

/**
 * @brief 原子自减并测试是否为零
 * @param v 原子变量指针
 * @return 结果为零返回非零值
 */
KLINE int atomic_dec_and_test(atomic_t *v) {
    unsigned char c;
    __asm__ __volatile__("lock;"
                            "decl %0; sete %1"
                            : "+m"(v->a_count), "=qm"(c)
                            :
                            : "memory");
    return c != 0;
}

/**
 * @brief 原子自增并测试是否为零
 * @param v 原子变量指针
 * @return 结果为零返回非零值
 */
KLINE int atomic_inc_and_test(atomic_t *v) {
    unsigned char c;
    __asm__ __volatile__("lock;"
                            "incl %0; sete %1"
                            : "+m"(v->a_count), "=qm"(c)
                            :
                            : "memory");
    return c != 0;
}

/**
 * @brief 原子交换 - 将新值写入并返回旧值
 * @param v 原子变量指针
 * @param new_val 新值
 * @return 旧值
 */
KLINE s32_t atomic_xchg(atomic_t *v, s32_t new_val) {
    s32_t old;
    __asm__ __volatile__("xchgl %0, %1"
                         : "=r"(old), "+m"(v->a_count)
                         : "0"(new_val)
                         : "memory");
    return old;
}

/**
 * @brief 原子比较交换 (CAS) - 如果当前值等于old则替换为new
 * @param v 原子变量指针
 * @param old_val 期望的旧值
 * @param new_val 要设置的新值
 * @return 操作前的值（可与old_val比较判断是否成功）
 */
KLINE s32_t atomic_cmpxchg(atomic_t *v, s32_t old_val, s32_t new_val) {
    s32_t ret;
    __asm__ __volatile__("lock; cmpxchgl %2, %1"
                         : "=a"(ret), "+m"(v->a_count)
                         : "r"(new_val), "0"(old_val)
                         : "memory");
    return ret;
}

/**
 * @brief 原子加法并返回结果
 * @param i 加数
 * @param v 原子变量指针
 * @return 加法后的结果
 */
KLINE s32_t atomic_add_return(s32_t i, atomic_t *v) {
    s32_t ret = i;
    __asm__ __volatile__("lock; xaddl %0, %1"
                         : "+r"(ret), "+m"(v->a_count)
                         :
                         : "memory");
    return ret + i;
}

/**
 * @brief 原子减法并返回结果
 * @param i 减数
 * @param v 原子变量指针
 * @return 减法后的结果
 */
KLINE s32_t atomic_sub_return(s32_t i, atomic_t *v) {
    return atomic_add_return(-i, v);
}

/*===========================================================================
 * 64位原子操作
 *===========================================================================*/

/**
 * @brief 64位原子读取
 * @param v 原子变量指针
 * @return 当前值
 */
KLINE s64_t atomic64_read(const atomic64_t *v) {
    return (*(volatile s64_t *)&(v)->a_count);
}

/**
 * @brief 64位原子设置
 * @param v 原子变量指针
 * @param i 要设置的值
 */
KLINE void atomic64_set(atomic64_t *v, s64_t i) {
    v->a_count = i;
}

/**
 * @brief 64位原子加法
 * @param i 加数
 * @param v 原子变量指针
 */
KLINE void atomic64_add(s64_t i, atomic64_t *v) {
    __asm__ __volatile__("lock; addq %1, %0"
                         : "+m"(v->a_count)
                         : "ir"(i));
}

/**
 * @brief 64位原子减法
 * @param i 减数
 * @param v 原子变量指针
 */
KLINE void atomic64_sub(s64_t i, atomic64_t *v) {
    __asm__ __volatile__("lock; subq %1, %0"
                         : "+m"(v->a_count)
                         : "ir"(i));
}

/**
 * @brief 64位原子自增
 * @param v 原子变量指针
 */
KLINE void atomic64_inc(atomic64_t *v) {
    __asm__ __volatile__("lock; incq %0"
                         : "+m"(v->a_count));
}

/**
 * @brief 64位原子自减
 * @param v 原子变量指针
 */
KLINE void atomic64_dec(atomic64_t *v) {
    __asm__ __volatile__("lock; decq %0"
                         : "+m"(v->a_count));
}

/**
 * @brief 64位原子交换
 * @param v 原子变量指针
 * @param new_val 新值
 * @return 旧值
 */
KLINE s64_t atomic64_xchg(atomic64_t *v, s64_t new_val) {
    s64_t old;
    __asm__ __volatile__("xchgq %0, %1"
                         : "=r"(old), "+m"(v->a_count)
                         : "0"(new_val)
                         : "memory");
    return old;
}

/**
 * @brief 64位原子比较交换 (CAS)
 * @param v 原子变量指针
 * @param old_val 期望的旧值
 * @param new_val 要设置的新值
 * @return 操作前的值
 */
KLINE s64_t atomic64_cmpxchg(atomic64_t *v, s64_t old_val, s64_t new_val) {
    s64_t ret;
    __asm__ __volatile__("lock; cmpxchgq %2, %1"
                         : "=a"(ret), "+m"(v->a_count)
                         : "r"(new_val), "0"(old_val)
                         : "memory");
    return ret;
}

/*===========================================================================
 * 引用计数操作
 *===========================================================================*/

/**
 * @brief 初始化引用计数
 * @param initp 引用计数指针
 */
KLINE void refcount_init(refcount_t *initp) {
    atomic_set(&initp->ref_count, 0);
    return;
}

/**
 * @brief 增加引用计数
 * @param refc 引用计数指针
 */
KLINE void refcount_inc(refcount_t *refc) {
    atomic_inc(&refc->ref_count);
    return;
}

/**
 * @brief 减少引用计数
 * @param refc 引用计数指针
 */
KLINE void refcount_dec(refcount_t *refc) {
    atomic_dec(&refc->ref_count);
}

/**
 * @brief 读取引用计数
 * @param refc 引用计数指针
 * @return 当前引用计数值
 */
KLINE s32_t refcount_read(refcount_t *refc) {
    return atomic_read(&refc->ref_count);
}

#endif // _ATOMIC_H
