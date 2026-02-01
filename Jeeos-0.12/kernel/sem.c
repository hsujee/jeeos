/*
 * @Author: Jee Hsu
 * @Description: 信号量(Semaphore)同步原语
 *
 * 实现计数信号量用于进程同步：
 * - sem_t_init(): 初始化信号量
 * - sem_down(): P操作(等待/减少计数)
 * - sem_up(): V操作(唤醒/增加计数)
 *
 * 使用场景：
 * - 资源计数(如缓冲区槽位)
 * - 互斥访问(二值信号量)
 * - 生产者-消费者同步
 *
 * 实现原理：
 * - 计数>0时P操作直接返回
 * - 计数<=0时P操作阻塞当前进程
 * - V操作唤醒一个等待进程
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

void sem_t_init(sem_t* initp) {
    spinlock_init(&initp->sem_lock);
    initp->sem_flags=0;
    initp->sem_count=0;
    waitlist_t_init(&initp->sem_waitlst);
    return;
}

void sem_set(sem_t* setsem,uint_t flg,sint_t conut) {
    cpuflg_t cpufg;
    spinlock_cli(&setsem->sem_lock,&cpufg);
    setsem->sem_flags=flg;
    setsem->sem_count=conut;
    spinunlock_sti(&setsem->sem_lock,&cpufg);
    return;
}    

void sem_down(sem_t* sem) {
    cpuflg_t cpufg;
start_step:    
    spinlock_cli(&sem->sem_lock,&cpufg);
    if(sem->sem_count<1) {
        waitlist_wait(&sem->sem_waitlst);
        spinunlock_sti(&sem->sem_lock,&cpufg);
        schedul();
        goto start_step; 
    }
    sem->sem_count--;
    spinunlock_sti(&sem->sem_lock,&cpufg);
    return;
}

void sem_up(sem_t* sem) {
    cpuflg_t cpufg;

    spinlock_cli(&sem->sem_lock,&cpufg);
    sem->sem_count++;
    if(sem->sem_count<1) {
        spinunlock_sti(&sem->sem_lock,&cpufg);
        arch_sysdie("sem up err");
    }
    waitlist_allup(&sem->sem_waitlst);
    spinunlock_sti(&sem->sem_lock,&cpufg);
    sched_set_schedflags();
    return;
}