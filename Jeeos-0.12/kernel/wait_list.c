/*
 * @Author: Jee Hsu
 * @Description: 等待队列(Wait Queue)管理
 *
 * 提供进程等待和唤醒机制：
 * - waitlist_wait(): 当前进程加入等待队列并睡眠
 * - waitlist_up(): 唤醒等待队列中的进程
 *
 * 使用场景：
 * - I/O等待(等待数据就绪)
 * - 事件等待(等待条件满足)
 * - 信号量内部实现
 *
 * 核心数据结构：
 * - waitlist_t: 等待队列头
 * - wl_list: 等待进程链表
 * - wl_tdnr: 等待进程数量
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

void waitlist_t_init(waitlist_t *initp) {
    spinlock_init(&initp->wl_lock);
    initp->wl_tdnr = 0;
    list_init(&initp->wl_list);
    return;
}

void waitlist_wait(waitlist_t *wlst) {
    sched_wait(wlst);
    return;
}

void waitlist_up(waitlist_t *wlst) {
    if (list_is_empty_careful(&wlst->wl_list) == TRUE) {
        return;
    }
    sched_up(wlst);

    return;
}

void waitlist_allup(waitlist_t *wlst) {
    while (list_is_empty_careful(&wlst->wl_list) == FALSE) {
        sched_up(wlst);
    }
    return;
}

void waitlist_add_thread(waitlist_t *wlst, thread_t *tdp) {
    cpuflg_t cufg;
    spinlock_cli(&wlst->wl_lock, &cufg);

    list_add(&tdp->td_list, &wlst->wl_list);
    wlst->wl_tdnr++;
    spinunlock_sti(&wlst->wl_lock, &cufg);
    return;
}

thread_t *waitlist_del_thread(waitlist_t *wlst) {
    thread_t *tdp = NULL;
    cpuflg_t cufg;
    list_h_t *list;
    spinlock_cli(&wlst->wl_lock, &cufg);
    list_for_each(list, &wlst->wl_list) {
        tdp = list_entry(list, thread_t, td_list);
        if (tdp == NULL || wlst->wl_tdnr < 1) {
            tdp = NULL;
            goto return_step;
        }
        list_del(&tdp->td_list);
        wlst->wl_tdnr--;
        tdp = tdp;
        goto return_step;
    }
    tdp = NULL;
return_step:
    spinunlock_sti(&wlst->wl_lock, &cufg);
    return tdp;
}