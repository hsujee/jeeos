/*
 * @Author: Jee Hsu
 * @Description: Fork/Wait/Exit 系统调用实现
 *
 * 实现进程的创建与回收：
 * - sys_fork: 创建子进程
 * - sys_exit: 进程退出
 * - sys_wait: 等待子进程退出
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

/*===========================================================================
 * 进程退出
 *===========================================================================*/

/**
 * @brief 进程退出
 * @param exit_code 退出码
 */
void sys_exit(sint_t exit_code) {
    thread_t *current = get_current_thread();
    thread_t *init_td = get_init_thread();
    
    /* 设置退出码和状态 */
    current->td_exit_code = exit_code;
    current->td_status = TDSTUS_ZOMB;
    
    /* 收养子进程给 init */
    if (!list_is_empty_careful(&current->td_children) && init_td != NULL) {
        /* 使用 list_for_each_head_dell 遍历并转移子进程 */
        list_h_t *pos;
        while (!list_is_empty_careful(&current->td_children)) {
            pos = current->td_children.next;
            thread_t *child = list_entry(pos, thread_t, td_sibling);
            list_del(&child->td_sibling);
            child->td_parent = init_td;
            list_add(&child->td_sibling, &init_td->td_children);
        }
    }
    
    /* 从调度队列移除（设置状态为僵尸已等同于移除） */
    /* sched_remove_thread(current); */
    
    /* TODO: 通知父进程（信号或唤醒 wait） */
    
    /* 切换到其他进程 */
    schedul();
    
    /* 不应该到达这里 */
    for (;;) {
        die(1000);
        schedul();
    }
}

/*===========================================================================
 * 等待子进程
 *===========================================================================*/

/**
 * @brief 等待任意子进程退出
 * @param status 用于存储退出状态的指针（可为 NULL）
 * @return 退出的子进程 PID，无子进程返回 -1
 */
sint_t sys_wait(sint_t *status) {
    thread_t *current = get_current_thread();
    
    /* 没有子进程 */
    if (list_is_empty_careful(&current->td_children)) {
        return -1;
    }
    
    /* 查找已退出的子进程 */
    for (;;) {
        list_h_t *pos;
        list_for_each(pos, &current->td_children) {
            thread_t *child = list_entry(pos, thread_t, td_sibling);
            
            if (child->td_status == TDSTUS_ZOMB) {
                /* 找到僵尸子进程 */
                sint_t pid = (sint_t)child->td_pid;
                
                if (status != NULL) {
                    *status = child->td_exit_code;
                }
                
                /* 从子进程列表移除 */
                list_del(&child->td_sibling);
                child->td_parent = NULL;
                
                /* TODO: 释放子进程资源 */
                /* delete_thread(child); */
                
                return pid;
            }
        }
        
        /* 没有僵尸子进程，等待 */
        die(10);
        schedul();
        
        /* 再次检查是否还有子进程 */
        if (list_is_empty_careful(&current->td_children)) {
            return -1;
        }
    }
}

/**
 * @brief 等待指定子进程退出
 * @param pid 要等待的子进程 PID
 * @param status 用于存储退出状态的指针（可为 NULL）
 * @return 成功返回 PID，失败返回 -1
 */
sint_t sys_waitpid(uint_t pid, sint_t *status) {
    thread_t *current = get_current_thread();
    
    /* 没有子进程 */
    if (list_is_empty_careful(&current->td_children)) {
        return -1;
    }
    
    /* 查找指定 PID 的子进程 */
    for (;;) {
        list_h_t *pos;
        thread_t *target = NULL;
        
        list_for_each(pos, &current->td_children) {
            thread_t *child = list_entry(pos, thread_t, td_sibling);
            if (child->td_pid == pid) {
                target = child;
                break;
            }
        }
        
        /* 没有找到指定 PID 的子进程 */
        if (target == NULL) {
            return -1;
        }
        
        if (target->td_status == TDSTUS_ZOMB) {
            /* 子进程已退出 */
            if (status != NULL) {
                *status = target->td_exit_code;
            }
            
            /* 从子进程列表移除 */
            list_del(&target->td_sibling);
            target->td_parent = NULL;
            
            /* TODO: 释放子进程资源 */
            
            return (sint_t)pid;
        }
        
        /* 子进程还在运行，等待 */
        die(10);
        schedul();
    }
}

/*===========================================================================
 * Fork 系统调用（用户态接口）
 *===========================================================================*/

/**
 * @brief Fork 系统调用
 * @return 父进程返回子进程 PID，子进程返回 0，失败返回 -1
 * 
 * 注意：当前实现为简化版本，需要指定子进程入口
 * 完整版本的 fork 需要复制上下文让子进程从 fork 调用点返回
 */
/* 此函数需要在汇编层配合实现完整的上下文复制 */
/* 暂时使用 fork_thread() 函数代替 */

