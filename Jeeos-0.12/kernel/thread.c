/*
 * @Author: Jee Hsu
 * @Description: 线程管理 - 进程/线程创建与控制
 *
 * 管理进程和线程的生命周期：
 * - 进程创建(new_thread)
 * - 进程销毁(kill_thread)
 * - 上下文切换(context_t)
 * - 内核栈管理(micrstk_t)
 *
 * 核心数据结构：
 * - thread_t: 线程控制块(TCB)
 * - context_t: CPU上下文(寄存器状态)
 * - micrstk_t: 微内核栈
 *
 * 进程状态：
 * - TDSTUS_RUN: 运行中
 * - TDSTUS_SLEEP: 睡眠
 * - TDSTUS_WAIT: 等待资源
 * - TDSTUS_ZOMBIE: 僵尸(等待回收)
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

/*===========================================================================
 * PID 分配器
 *===========================================================================*/

PRIVATE uint_t next_pid = 1;  /* PID 从 1 开始，0 保留 */

/**
 * @brief 分配新的 PID
 */
uint_t alloc_pid(void) {
    return next_pid++;
}

/**
 * @brief 获取当前最大 PID
 */
uint_t get_max_pid(void) {
    return next_pid - 1;
}

/*===========================================================================
 * 初始化函数
 *===========================================================================*/

void micrstk_t_init(micrstk_t *initp) {
    for (uint_t i = 0; i < MICRSTK_MAX; i++) {
        initp->msk_val[i] = 0;
    }
    return;
}

//初始化context_t结构
void context_t_init(context_t *initp) {
    initp->ctx_nextrip = 0;
    initp->ctx_nextrsp = 0;
    //指向当前CPU的tss
    initp->ctx_nexttss = &x64tss[arch_retn_cpuid()];
    return;
}

//返回进程id其实就thread_t结构的地址
uint_t thread_get_id(thread_t *tdp) {
    return (uint_t)tdp;
}

//初始化thread_t结构
void thread_t_init(thread_t *initp) {
    spinlock_init(&initp->td_lock);
    list_init(&initp->td_list);
    initp->td_name[0] = '\0';  // 初始化名称为空
    initp->td_flags = TDFLAG_FREE;
    initp->td_status = TDSTUS_NEW;    //进程状态为新建
    initp->td_cpuid = arch_retn_cpuid();
    initp->td_id = thread_get_id(initp);
    initp->td_pid = 0;                // PID 稍后分配
    initp->td_tick = 0;
    initp->td_privilege = PRILG_USR; //普通进程权限
    initp->td_priority = PRITY_MIN;  //最高优先级
    initp->td_runmode = 0;
    initp->td_krlstktop = NULL;
    initp->td_krlstkstart = NULL;
    initp->td_usrstktop = NULL;
    initp->td_usrstkstart = NULL;
    initp->td_mmdsc = &initmmadrsdsc; //指向默认的地址空间结构
    initp->td_resdsc = NULL;
    initp->td_privtep = NULL;
    initp->td_extdatap = NULL;

    context_t_init(&initp->td_context);
    //初始化td_handtbl数组
    for (uint_t hand = 0; hand < TD_HAND_MAX; hand++) {
        initp->td_handtbl[hand] = NULL;
    }
    
    // 初始化进程关系
    initp->td_parent = NULL;
    list_init(&initp->td_children);
    list_init(&initp->td_sibling);
    initp->td_exit_code = 0;
    
    return;
}

//创建thread_t结构
thread_t *new_thread_dsc() {
    //分配thread_t结构大小的内存空间
    thread_t *rettdp = (thread_t *)(new_mem((size_t)(sizeof(thread_t))));
    if (rettdp == NULL) {
        return NULL;
    }
    //初始化刚刚分配的thread_t结构
    thread_t_init(rettdp);
    return rettdp;
}

void thread_inc_tick(thread_t *thdp) {
    cpuflg_t cpuflg;
    spinlock_cli(&thdp->td_lock, &cpuflg);
    thdp->td_tick++;
    if (thdp->td_tick > TDRUN_TICK) {
        thdp->td_tick = 0;
        sched_set_schedflags();
    }
    spinunlock_sti(&thdp->td_lock, &cpuflg);
    return;
}

hand_t thread_get_nullhand(thread_t *thdp) {
    cpuflg_t cpuflg;
    hand_t rethd = NO_HAND;
    spinlock_cli(&thdp->td_lock, &cpuflg);
    for (uint_t hand = 0; hand < TD_HAND_MAX; hand++) {
        if (thdp->td_handtbl[hand] == NULL) {
            rethd = (hand_t)hand;
            goto retn_step;
        }
    }
    rethd = NO_HAND;
retn_step:
    spinunlock_sti(&thdp->td_lock, &cpuflg);
    return rethd;
}

hand_t thread_add_objnode(thread_t *thdp, objnode_t *ondp) {
    cpuflg_t cpuflg;
    hand_t rethd = NO_HAND;
    spinlock_cli(&thdp->td_lock, &cpuflg);
    for (uint_t hand = 0; hand < TD_HAND_MAX; hand++) {
        if (thdp->td_handtbl[hand] == NULL) {
            rethd = (hand_t)hand;
            goto next_step;
        }
    }
    rethd = NO_HAND;
    goto retn_step;

next_step:
    thdp->td_handtbl[rethd] = ondp;
retn_step:
    spinunlock_sti(&thdp->td_lock, &cpuflg);
    return rethd;
}

hand_t thread_del_objnode(thread_t *thdp, hand_t hand) {
    if ((hand >= TD_HAND_MAX) || (hand <= NO_HAND)) {
        return NO_HAND;
    }

    cpuflg_t cpuflg;
    hand_t rethd = NO_HAND;
    spinlock_cli(&thdp->td_lock, &cpuflg);

    if (thdp->td_handtbl[hand] == NULL) {
        rethd = NO_HAND;
        goto retn_step;
    }
    thdp->td_handtbl[hand] = NULL;
    rethd = hand;
retn_step:
    spinunlock_sti(&thdp->td_lock, &cpuflg);
    return rethd;
}

objnode_t *thread_get_objnode(thread_t *thdp, hand_t hand) {
    if ((hand >= TD_HAND_MAX) || (hand <= NO_HAND)) {
        return NULL;
    }

    cpuflg_t cpuflg;
    objnode_t *retondp = NULL;
    spinlock_cli(&thdp->td_lock, &cpuflg);

    if (thdp->td_handtbl[hand] == NULL) {
        retondp = NULL;
        goto retn_step;
    }
    retondp = thdp->td_handtbl[hand];
retn_step:
    spinunlock_sti(&thdp->td_lock, &cpuflg);
    return retondp;
}

void thread_kernstack_init(thread_t *thdp, void *runadr, uint_t cpuflags) {
    //处理栈顶16字节对齐
    thdp->td_krlstktop &= (~0xf);
    thdp->td_usrstktop &= (~0xf);
    //内核栈顶减去intstkregs_t结构的大小
    intstkregs_t *arp = (intstkregs_t *)(thdp->td_krlstktop - sizeof(intstkregs_t));
    //把intstkregs_t结构的空间初始化为0
    arch_memset((void*)arp, 0, sizeof(intstkregs_t));
    //rip寄存器的值设为程序运行首地址
    arp->r_rip_old = (uint_t)runadr;
    //cs寄存器的值设为内核代码段选择子
    arp->r_cs_old = K_CS_IDX;
    arp->r_rflags = cpuflags;
    //返回进程的内核栈
    arp->r_rsp_old = thdp->td_krlstktop;
    arp->r_ss_old = 0;
    //其它段寄存器的值设为内核数据段选择子
    arp->r_ds = K_DS_IDX;
    arp->r_es = K_DS_IDX;
    arp->r_fs = K_DS_IDX;
    arp->r_gs = K_DS_IDX;
    //设置进程下一次运行的地址为runadr
    thdp->td_context.ctx_nextrip = (uint_t)runadr;
    //设置进程下一次运行的栈地址为arp
    thdp->td_context.ctx_nextrsp = (uint_t)arp;

    return;
}

void thread_userstack_init(thread_t *thdp, void *runadr, uint_t cpuflags) {
    //处理栈顶16字节对齐
    thdp->td_krlstktop &= (~0xf);
    thdp->td_usrstktop &= (~0xf);
    //内核栈顶减去intstkregs_t结构的大小
    intstkregs_t *arp = (intstkregs_t *)(thdp->td_krlstktop - sizeof(intstkregs_t));
    //把intstkregs_t结构的空间初始化为0
    arch_memset((void*)arp, 0, sizeof(intstkregs_t));
    //rip寄存器的值设为程序运行首地址
    arp->r_rip_old = (uint_t)runadr;
    //cs寄存器的值设为应用程序代码段选择子
    arp->r_cs_old = U_CS_IDX;
    arp->r_rflags = cpuflags;
    //返回进程应用程序空间的栈
    arp->r_rsp_old = thdp->td_usrstktop;
    //其它段寄存器的值设为应用程序数据段选择子
    arp->r_ss_old = U_DS_IDX;
   
    arp->r_ds = U_DS_IDX;
    arp->r_es = U_DS_IDX;
    arp->r_fs = U_DS_IDX;
    arp->r_gs = U_DS_IDX;
    //设置进程下一次运行的地址为runadr
    thdp->td_context.ctx_nextrip = (uint_t)runadr;
    //设置进程下一次运行的栈地址为arp
    thdp->td_context.ctx_nextrsp = (uint_t)arp;

    return;
}

thread_t *new_user_thread_core(void *filerun, uint_t flg, uint_t prilg, uint_t prity, size_t usrstksz, size_t krlstksz) {
    thread_t *ret_td = NULL;
    bool_t acs = FALSE;
    adr_t usrstkadr = NULL, krlstkadr = NULL;
    //分配应用程序栈空间
    usrstkadr = new_mem(usrstksz);
    if (usrstkadr == NULL) {
        return NULL;
    }
    //分配内核栈空间
    krlstkadr = new_mem(krlstksz);
    if (krlstkadr == NULL) {
        if (delete_mem(usrstkadr, usrstksz) == FALSE) {
            return NULL;
        }
        return NULL;
    }
    //建立thread_t结构体的实例变量
    ret_td = new_thread_dsc();
    //创建失败必须要释放之前的栈空间
    if (ret_td == NULL) {
        acs = delete_mem(usrstkadr, usrstksz);
        acs = delete_mem(krlstkadr, krlstksz);
        if (acs == FALSE) {
            return NULL;
        }
        return NULL;
    }
    //设置进程权限
    ret_td->td_privilege = prilg;
    //设置进程优先级
    ret_td->td_priority = prity;
    //设置进程的内核栈顶和内核栈开始地址
    ret_td->td_krlstktop = krlstkadr + (adr_t)(krlstksz - 1);
    ret_td->td_krlstkstart = krlstkadr;
    //设置进程的应用程序栈顶和内核应用程序栈开始地址
    ret_td->td_usrstktop = usrstkadr + (adr_t)(usrstksz - 1);
    ret_td->td_usrstkstart = usrstkadr;
    //初始化返回进程应用程序空间的内核栈
    thread_userstack_init(ret_td, filerun, UMOD_EFLAGS);
    //加入调度器系统
    schedclass_add_thread(ret_td);
    return ret_td;
}

// 内核进程就是用进程的方式去运行一段内核代码，那么这段代码就可以随时暂停或者继续运行，又或者和其它代码段并发运行
// 只是这种进程永远不会回到进程应用程序地址空间中去，只会在内核地址空间中运行。
thread_t *new_kern_thread_core(void *filerun, uint_t flg, uint_t prilg, uint_t prity, size_t usrstksz, size_t krlstksz) {
    thread_t *ret_td = NULL;
    bool_t acs = FALSE;
    adr_t krlstkadr = NULL;
    //分配内核栈空间
    krlstkadr = new_mem(krlstksz);
    if (krlstkadr == NULL) {
        return NULL;
    }
    //建立thread_t结构体的实例变量
    ret_td = new_thread_dsc();
    if (ret_td == NULL) {
        //创建失败必须要释放之前的栈空间
        acs = delete_mem(krlstkadr, krlstksz);
        if (acs == FALSE) {
            return NULL;
        }
        return NULL;
    }
    //设置进程权限
    ret_td->td_privilege = prilg;
    //设置进程优先级
    ret_td->td_priority = prity;
    //设置进程的内核栈顶和内核栈开始地址
    ret_td->td_krlstktop = krlstkadr + (adr_t)(krlstksz - 1);
    ret_td->td_krlstkstart = krlstkadr;
    //初始化进程的内核栈
    thread_kernstack_init(ret_td, filerun, KMOD_EFLAGS);
    //加入进程调度系统
    schedclass_add_thread(ret_td);
    //返回进程指针
    return ret_td;
}

// 设置线程名称
void thread_set_name(thread_t *tdp, const char *name) {
    if (tdp == NULL || name == NULL) return;
    uint_t i;
    for (i = 0; i < TD_NAME_MAX - 1 && name[i] != '\0'; i++) {
        tdp->td_name[i] = name[i];
    }
    tdp->td_name[i] = '\0';
}

// filerun 应用程序启动运行的地址
// flg 创建标志
// prilg 进程权限和
// prity 进程优先级
// usrstksz 进程的应用程序栈大小
// krlstksz 内核栈大小
// name 线程名称
thread_t *new_thread(void *filerun, uint_t flg, uint_t prilg, uint_t prity, 
                     size_t usrstksz, size_t krlstksz, const char *name) {
    size_t tustksz = usrstksz, tkstksz = krlstksz;
    thread_t *ret_td = NULL;
    
    //对参数进行检查，不合乎要求就返回NULL表示创建失败
    if (filerun == NULL || usrstksz > DAFT_TDUSRSTKSZ || krlstksz > DAFT_TDKRLSTKSZ) {
        return NULL;
    }

    if ((prilg != PRILG_USR && prilg != PRILG_SYS) || (prity >= PRITY_MAX)) {
        return NULL;
    }
    //进程应用程序栈大小检查，大于默认大小则使用默认大小(8个页面)
    if (usrstksz < DAFT_TDUSRSTKSZ) {
        tustksz = DAFT_TDUSRSTKSZ;
    }
    //进程内核栈大小检查，大于默认大小则使用默认大小(8个页面)
    if (krlstksz < DAFT_TDKRLSTKSZ) {
        tkstksz = DAFT_TDKRLSTKSZ;
    }

    if (KERNTHREAD_FLG == flg) {
        //是否建立内核进程
        ret_td = new_kern_thread_core(filerun, flg, prilg, prity, tustksz, tkstksz);
    } else if (USERTHREAD_FLG == flg) {
        //是否建立普通进程
        ret_td = new_user_thread_core(filerun, flg, prilg, prity, tustksz, tkstksz);
    }
    
    if (ret_td != NULL) {
        // 分配 PID
        ret_td->td_pid = alloc_pid();
        // 设置线程名称
        if (name != NULL) {
            thread_set_name(ret_td, name);
        }
    }
    
    return ret_td;
}

/*===========================================================================
 * Fork 实现
 *===========================================================================*/

/**
 * @brief 添加子进程到父进程的子进程列表
 */
void thread_add_child(thread_t *parent, thread_t *child) {
    if (parent == NULL || child == NULL) return;
    
    child->td_parent = parent;
    list_add(&child->td_sibling, &parent->td_children);
}

/**
 * @brief 从父进程的子进程列表中移除
 */
void thread_remove_child(thread_t *child) {
    if (child == NULL || child->td_parent == NULL) return;
    
    list_del(&child->td_sibling);
    child->td_parent = NULL;
}

/**
 * @brief Fork 当前进程
 * @param parent 父进程
 * @param child_entry 子进程入口函数（如果为 NULL 则从 fork 返回点继续）
 * @param child_name 子进程名称
 * @return 子进程指针，失败返回 NULL
 */
thread_t *fork_thread(thread_t *parent, void *child_entry, const char *child_name) {
    if (parent == NULL) return NULL;
    
    thread_t *child = NULL;
    adr_t krlstkadr = NULL;
    
    // 分配子进程的内核栈
    krlstkadr = new_mem(DAFT_TDKRLSTKSZ);
    if (krlstkadr == NULL) {
        return NULL;
    }
    
    // 创建子进程 thread_t 结构
    child = new_thread_dsc();
    if (child == NULL) {
        delete_mem(krlstkadr, DAFT_TDKRLSTKSZ);
        return NULL;
    }
    
    // 分配 PID
    child->td_pid = alloc_pid();
    
    // 复制父进程属性
    child->td_privilege = parent->td_privilege;
    child->td_priority = parent->td_priority;
    child->td_runmode = parent->td_runmode;
    child->td_mmdsc = parent->td_mmdsc;  // 共享地址空间（简化版）
    
    // 设置子进程的内核栈
    child->td_krlstktop = krlstkadr + (adr_t)(DAFT_TDKRLSTKSZ - 1);
    child->td_krlstkstart = krlstkadr;
    
    // 设置子进程名称
    if (child_name != NULL) {
        thread_set_name(child, child_name);
    } else {
        thread_set_name(child, parent->td_name);
    }
    
    // 建立父子关系
    thread_add_child(parent, child);
    
    // 初始化子进程的内核栈并设置入口点
    if (child_entry != NULL) {
        thread_kernstack_init(child, child_entry, KMOD_EFLAGS);
    }
    
    // 将子进程加入调度队列
    schedclass_add_thread(child);
    
    return child;
}
