/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - 线程管理
 *
 * 实现线程管理系统调用：
 * - 线程创建
 * - 线程退出
 * - 线程等待
 * - 休眠
 *
 * 函数接口：
 * - syscall_thread(): 系统调用表入口
 * - sys_thread_create(): 创建线程
 * - sys_thread_exit(): 退出线程
 * - sys_thread_wait(): 等待线程
 * - sys_sleep(): 休眠
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "sys_deps.h"

/**
 * 系统调用表thread入口
 * 根据不同的系统调用号分发到具体处理函数
 */
sysstus_t syscall_thread(uint_t inr, stkparame_t *stkparv) {
    if (stkparv == NULL) {
        return SYSSTUSERR;
    }
    
    sysstus_t ret = SYSSTUSERR;
    
    switch (inr) {
        case INR_TD_EXEL: {
            /* 创建并执行线程 */
            void *entry = (void *)stkparv->parmv1;
            uint_t flags = (uint_t)stkparv->parmv2;
            void *arg = (void *)stkparv->parmv3;
            
            hand_t thhand = sys_thread_create(entry, flags, arg);
            stkparv->parmv1 = (u64_t)thhand;
            ret = (thhand != NO_HAND) ? SYSSTUSOK : SYSSTUSERR;
            break;
        }
        
        case INR_TD_EXIT: {
            /* 退出线程 */
            sysstus_t exitstus = (sysstus_t)stkparv->parmv1;
            sys_thread_exit(exitstus);
            /* 不会返回 */
            ret = SYSSTUSOK;
            break;
        }
        
        case INR_TD_HAND: {
            /* 等待线程 */
            hand_t thhand = (hand_t)stkparv->parmv1;
            ret = sys_thread_wait(thhand);
            stkparv->parmv1 = (u64_t)ret;
            break;
        }
        
        case INR_TD_RSTATS:
        case INR_TD_SSTATS: {
            /* 线程状态操作 - 当前未实现 */
            ret = SYSSTUSERR;
            break;
        }
        
        default:
            ret = SYSSTUSERR;
            break;
    }
    
    return ret;
}

/**
 * 创建线程
 * @param entry 线程入口函数
 * @param flags 创建标志
 * @param arg 线程参数(当前未使用)
 * @return 线程句柄，失败返回NO_HAND
 */
hand_t sys_thread_create(void *entry, uint_t flags, void *arg) {
    (void)arg;
    
    if (entry == NULL) {
        return NO_HAND;
    }
    
    /* 创建新线程 */
    thread_t *newtdp = new_thread(
        entry,                      /* 入口地址 */
        flags,                      /* 标志 */
        0,                          /* 特权级(用户态) */
        PRITY_MIN,                  /* 优先级 */
        DAFT_TDUSRSTKSZ,            /* 用户栈大小 */
        DAFT_TDKRLSTKSZ,            /* 内核栈大小 */
        "user_thread"               /* 线程名称 */
    );
    
    if (newtdp == NULL) {
        return NO_HAND;
    }
    
    /* 将线程添加到调度队列 */
    schedclass_add_thread(newtdp);
    
    /* 返回线程ID作为句柄 */
    return (hand_t)thread_get_id(newtdp);
}

/**
 * 退出线程
 * @param stus 退出状态
 */
void sys_thread_exit(sysstus_t stus) {
    (void)stus;
    
    /* 获取当前线程 */
    thread_t *curtdp = sched_retn_currthread();
    if (curtdp == NULL) {
        return;
    }
    
    /* 设置线程状态为僵尸(已结束) */
    curtdp->td_status = TDSTUS_ZOMB;
    
    /* 触发调度 */
    sched_set_schedflags();
    schedul();
    
    /* 不会返回 */
    return;
}

/**
 * 等待线程
 * @param thhand 线程句柄
 * @return SYSSTUSOK成功，SYSSTUSERR失败
 */
sysstus_t sys_thread_wait(hand_t thhand) {
    (void)thhand;
    
    /* TODO: 实现线程等待 */
    /* 需要等待队列支持 */
    
    return SYSSTUSERR;
}

/**
 * 休眠
 * @param ms 休眠毫秒数
 * @return SYSSTUSOK成功，SYSSTUSERR失败
 */
sysstus_t sys_sleep(uint_t ms) {
    (void)ms;
    
    /* TODO: 实现休眠 */
    /* 需要时间子系统支持 */
    
    return SYSSTUSERR;
}
