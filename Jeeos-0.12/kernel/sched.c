/*
 * @Author: Jee Hsu
 * @Description: 进程调度器 - 多任务调度核心
 *
 * 实现抢占式多任务调度：
 * - 基于优先级的调度算法
 * - 时间片轮转
 * - 进程状态管理(运行/就绪/阻塞/僵尸)
 *
 * 核心数据结构：
 * - schedclass_t: 调度器类(管理所有CPU的调度数据)
 * - schdata_t: 单CPU调度数据(当前进程、就绪队列)
 * - thrdlst_t: 进程链表(按优先级组织)
 *
 * 调度时机：
 * - 时钟中断(时间片耗尽)
 * - 进程主动让出CPU(睡眠/等待)
 * - 高优先级进程就绪(抢占)
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

void thrdlst_t_init(thrdlst_t *initp) {
    //初始化挂载进程的链表
    list_init(&initp->tdl_lsth);
    //开始没有运行进程
    initp->tdl_curruntd = NULL;
    //开始没有进程
    initp->tdl_nr = 0;
    return;
}

void schdata_t_init(schdata_t *initp) {
    spinlock_init(&initp->sda_lock);
    //获取CPU id
    initp->sda_cpuid = arch_retn_cpuid();
    initp->sda_schedflags = NOTS_SCHED_FLGS;
    initp->sda_premptidx = 0;
    initp->sda_threadnr = 0;
    initp->sda_prityidx = 0;
    //开始没有空转进程和运行的进程
    initp->sda_cpuidle = NULL;
    initp->sda_currtd = NULL;
    //初始化schdata_t结构中的每个thrdlst_t结构
    for (uint_t ti = 0; ti < PRITY_MAX; ti++) {
        thrdlst_t_init(&initp->sda_thdlst[ti]);
    }
    return;
}

void schedclass_t_init(schedclass_t *initp) {
    spinlock_init(&initp->scls_lock);
    //CPU最大个数
    initp->scls_cpunr = CPUCORE_MAX;
    //开始没有进程
    initp->scls_threadnr = 0;
    initp->scls_threadid_inc = 0;
    //初始化osschedcls变量中的每个schdata_t
    for (uint_t si = 0; si < CPUCORE_MAX; si++) {
        schdata_t_init(&initp->scls_schda[si]);
    }
    return;
}

void init_sched() {
    //初始化osschedcls变量
    schedclass_t_init(&osschedcls);
    /* 调度器初始化完成 */
    return;
}

thread_t *sched_retn_currthread() {
    //通过cpuid获取当前cpu的调度数据结构
    uint_t cpuid = arch_retn_cpuid();
    schdata_t *schdap = &osschedcls.scls_schda[cpuid];
    //若调度数据结构中当前运行进程的指针为空，就出错死机
    if (schdap->sda_currtd == NULL) {
        arch_sysdie("schdap->sda_currtd NULL");
    }
    //返回当前运行的进程
    return schdap->sda_currtd;
}

uint_t sched_retn_schedflags() {
    uint_t cpuid = arch_retn_cpuid();
    schdata_t *schdap = &osschedcls.scls_schda[cpuid];
    return schdap->sda_schedflags;
}

void sched_wait(waitlist_t *wlst) {
    cpuflg_t cufg, tcufg;
    uint_t cpuid = arch_retn_cpuid();
    schdata_t *schdap = &osschedcls.scls_schda[cpuid];
    //获取当前正在运行的进程
    thread_t *tdp = sched_retn_currthread();
    uint_t pity = tdp->td_priority;

    if (pity >= PRITY_MAX || wlst == NULL) {
        goto err_step;
    }
    if (schdap->sda_thdlst[pity].tdl_nr < 1) {
        goto err_step;
    }

    spinlock_cli(&schdap->sda_lock, &cufg);

    spinlock_cli(&tdp->td_lock, &tcufg);
    //设置进程状态为等待状态
    tdp->td_status = TDSTUS_WAIT;
    //脱链
    list_del(&tdp->td_list);
    spinunlock_sti(&tdp->td_lock, &tcufg);

    if (schdap->sda_thdlst[pity].tdl_curruntd == tdp) {
        schdap->sda_thdlst[pity].tdl_curruntd = NULL;
    }
    schdap->sda_thdlst[pity].tdl_nr--;

    spinunlock_sti(&schdap->sda_lock, &cufg);
    //将进程加入等待结构中
    waitlist_add_thread(wlst, tdp);

    return;

err_step:
    arch_sysdie("sched_wait err");
    return;
}

void sched_up(waitlist_t *wlst) {
    cpuflg_t cufg, tcufg;
    uint_t cpuid = arch_retn_cpuid();
    schdata_t *schdap = &osschedcls.scls_schda[cpuid];
    thread_t *tdp;
    uint_t pity;
    if (wlst == NULL) {
        goto err_step;
    }
    //取出等待数据结构第一个进程并从等待数据结构中删除
    tdp = waitlist_del_thread(wlst);
    if (tdp == NULL) {
        goto err_step;
    }
    //获取进程的优先级
    pity = tdp->td_priority;
    if (pity >= PRITY_MAX) {
        goto err_step;
    }
    spinlock_cli(&schdap->sda_lock, &cufg);
    spinlock_cli(&tdp->td_lock, &tcufg);
    //设置进程的状态为运行状态
    tdp->td_status = TDSTUS_RUN;
    spinunlock_sti(&tdp->td_lock, &tcufg);
    //加入进程优先级链表
    list_add_tail(&tdp->td_list, &(schdap->sda_thdlst[pity].tdl_lsth));
    schdap->sda_thdlst[pity].tdl_nr++;
    spinunlock_sti(&schdap->sda_lock, &cufg);

    return;
err_step:
    arch_sysdie("sched_up err");
    return;
}

thread_t *sched_retn_idlethread() {
    uint_t cpuid = arch_retn_cpuid();
    //通过cpuid获取当前cpu的调度数据结构
    schdata_t *schdap = &osschedcls.scls_schda[cpuid];

    if (schdap->sda_cpuidle == NULL) {
        //若调度数据结构中空转进程的指针为空，就出错死机
        arch_sysdie("schdap->sda_cpuidle NULL");
    }
    //返回空转进程
    return schdap->sda_cpuidle;
}

void sched_set_schedflags() {
    cpuflg_t cpuflg;
    uint_t cpuid = arch_retn_cpuid();
    schdata_t *schdap = &osschedcls.scls_schda[cpuid];

    spinlock_cli(&schdap->sda_lock, &cpuflg);
    schdap->sda_schedflags = NEED_SCHED_FLGS;
    spinunlock_sti(&schdap->sda_lock, &cpuflg);
    return;
}

void sched_set_schedflags_ex(uint_t flags) {
    cpuflg_t cpuflg;
    uint_t cpuid = arch_retn_cpuid();
    schdata_t *schdap = &osschedcls.scls_schda[cpuid];

    spinlock_cli(&schdap->sda_lock, &cpuflg);
    schdap->sda_schedflags = flags;
    spinunlock_sti(&schdap->sda_lock, &cpuflg);
    return;
}

void sched_chkneed_pmptsched() {
    cpuflg_t cpuflg;
    uint_t schd = 0, cpuid = arch_retn_cpuid();
    schdata_t *schdap = &osschedcls.scls_schda[cpuid];

    spinlock_cli(&schdap->sda_lock, &cpuflg);

    if (schdap->sda_schedflags == NEED_SCHED_FLGS && schdap->sda_premptidx == PMPT_FLGS) {
        schdap->sda_schedflags = NOTS_SCHED_FLGS;
        schd = 1;
    }
    if (schdap->sda_schedflags == NEED_START_CPUILDE_SCHED_FLGS) {
        schd = 1;
    }
    spinunlock_sti(&schdap->sda_lock, &cpuflg);
    if (schd == 1) {
        schedul();
    }
    return;
}

thread_t *sched_select_thread() {
    thread_t *retthd, *tdtmp;
    cpuflg_t cufg;
    uint_t cpuid = arch_retn_cpuid();
    schdata_t *schdap = &osschedcls.scls_schda[cpuid];

    spinlock_cli(&schdap->sda_lock, &cufg);
    for (uint_t pity = 0; pity < PRITY_MAX; pity++) {
        //从最高优先级开始扫描
        if (schdap->sda_thdlst[pity].tdl_nr > 0) {
            //若当前优先级的进程链表不为空
            if (list_is_empty_careful(&(schdap->sda_thdlst[pity].tdl_lsth)) == FALSE) {
                //取出当前优先级进程链表下的第一个进程
                tdtmp = list_entry(schdap->sda_thdlst[pity].tdl_lsth.next, thread_t, td_list);
                //脱链
                list_del(&tdtmp->td_list);
                if (schdap->sda_thdlst[pity].tdl_curruntd != NULL) {
                    //将这sda_thdlst[pity].tdl_curruntd的进程挂入链表尾
                    list_add_tail(&(schdap->sda_thdlst[pity].tdl_curruntd->td_list), &schdap->sda_thdlst[pity].tdl_lsth);
                }
                //将选择的进程放入sda_thdlst[pity].tdl_curruntd中，并返回
                schdap->sda_thdlst[pity].tdl_curruntd = tdtmp;
                retthd = tdtmp;

                goto return_step;
            }
            if (schdap->sda_thdlst[pity].tdl_curruntd != NULL) {
                //若sda_thdlst[pity].tdl_curruntd不为空就直接返回它
                retthd = schdap->sda_thdlst[pity].tdl_curruntd;
                goto return_step;
            }
        }
    }
    //如果最后也没有找到进程就返回默认的空转进程
    schdap->sda_prityidx = PRITY_MIN;
    retthd = sched_retn_idlethread();

return_step:
    //解锁并返回进程
    spinunlock_sti(&schdap->sda_lock, &cufg);
    return retthd;
}

void schedul() {
    if (sched_retn_schedflags() == NEED_START_CPUILDE_SCHED_FLGS) {
        sched_set_schedflags_ex(NOTS_SCHED_FLGS);
        retnfrom_first_sched(sched_retn_idlethread());
        return;
    }
    //返回当前运行进程，并且选择下一个运行的进程
    thread_t *prev = sched_retn_currthread(),
             *next = sched_select_thread();
    //从当前进程切换到下一个进程
    save_to_new_context(next, prev);
    return;
}

void schedclass_add_thread(thread_t *thdp) {
    uint_t cpuid = arch_retn_cpuid();
    schdata_t *schdap = &osschedcls.scls_schda[cpuid];
    cpuflg_t cufg;

    spinlock_cli(&schdap->sda_lock, &cufg);
    list_add(&thdp->td_list, &schdap->sda_thdlst[thdp->td_priority].tdl_lsth);
    schdap->sda_thdlst[thdp->td_priority].tdl_nr++;
    schdap->sda_threadnr++;
    spinunlock_sti(&schdap->sda_lock, &cufg);

    spinlock_cli(&osschedcls.scls_lock, &cufg);
    osschedcls.scls_threadnr++;
    spinunlock_sti(&osschedcls.scls_lock, &cufg);

    return;
}

TNCCALL void __to_new_context(thread_t *next, thread_t *prev) {
    uint_t cpuid = arch_retn_cpuid();
    schdata_t *schdap = &osschedcls.scls_schda[cpuid];
    //设置当前运行进程为下一个运行的进程
    schdap->sda_currtd = next;
    //设置下一个运行进程的tss为当前CPU的tss
    next->td_context.ctx_nexttss = &x64tss[cpuid];
    //设置当前CPU的tss中的R0栈为下一个运行进程的内核栈
    next->td_context.ctx_nexttss->rsp0 = next->td_krlstktop;
    //装载下一个运行进程的MMU页表
    arch_mmu_load(&next->td_mmdsc->msd_mmu);
    if (next->td_status == TDSTUS_NEW) {
        //如果是新建进程第一次运行就要进行处理
        next->td_status = TDSTUS_RUN;
        retnfrom_first_sched(next);
    }

    return;
}

void save_to_new_context(thread_t *next, thread_t *prev) {
#ifdef CFG_X86_PLATFORM
    __asm__ __volatile__(
        //保存当前进程的标志寄存器
        "pushfq \n\t"
        //关中断
        "cli \n\t"
        //保存当前进程的通用寄存器
        "pushq %%rax\n\t"
        "pushq %%rbx\n\t"
        "pushq %%rcx\n\t"
        "pushq %%rdx\n\t"
        "pushq %%rbp\n\t"
        "pushq %%rsi\n\t"
        "pushq %%rdi\n\t"
        "pushq %%r8\n\t"
        "pushq %%r9\n\t"
        "pushq %%r10\n\t"
        "pushq %%r11\n\t"
        "pushq %%r12\n\t"
        "pushq %%r13\n\t"
        "pushq %%r14\n\t"
        "pushq %%r15\n\t"
        //保存CPU的RSP寄存器到当前进程的机器上下文结构中
        "movq %%rsp,%[PREV_RSP] \n\t"
        //把下一个进程的机器上下文结构中的RSP的值，写入CPU的RSP寄存器中
        //事实上这里已经切换到下一个进程了，因为切换进程的内核栈
        "movq %[NEXT_RSP],%%rsp \n\t"
        //调用__to_new_context函数切换MMU页表
        "callq __to_new_context\n\t"
        //恢复下一个进程的通用寄存器
        "popq %%r15\n\t"
        "popq %%r14\n\t"
        "popq %%r13\n\t"
        "popq %%r12\n\t"
        "popq %%r11\n\t"
        "popq %%r10\n\t"
        "popq %%r9\n\t"
        "popq %%r8\n\t"
        "popq %%rdi\n\t"
        "popq %%rsi\n\t"
        "popq %%rbp\n\t"
        "popq %%rdx\n\t"
        "popq %%rcx\n\t"
        "popq %%rbx\n\t"
        "popq %%rax\n\t"
        //恢复下一个进程的标志寄存器
        "popfq \n\t"
        //输出当前进程的内核栈地址
        : [ PREV_RSP ] "=m"(prev->td_context.ctx_nextrsp)
        //读取下一个进程的内核栈地址
        : [ NEXT_RSP ] "m"(next->td_context.ctx_nextrsp), "D"(next), "S"(prev)
        : "memory");
#endif
    return;
}

void retnfrom_first_sched(thread_t *thrdp) {
#ifdef CFG_X86_PLATFORM
    __asm__ __volatile__(
        //设置CPU的RSP寄存器为该进程机器上下文结构中的RSP
        "movq %[NEXT_RSP],%%rsp\n\t"
        //恢复进程保存在内核栈中的段寄存器
        "popq %%r14\n\t"
        "movw %%r14w,%%gs\n\t"
        "popq %%r14\n\t"
        "movw %%r14w,%%fs\n\t"
        "popq %%r14\n\t"
        "movw %%r14w,%%es\n\t"
        "popq %%r14\n\t"
        "movw %%r14w,%%ds\n\t"
        //恢复进程保存在内核栈中的通用寄存器
        "popq %%r15\n\t"
        "popq %%r14\n\t"
        "popq %%r13\n\t"
        "popq %%r12\n\t"
        "popq %%r11\n\t"
        "popq %%r10\n\t"
        "popq %%r9\n\t"
        "popq %%r8\n\t"
        "popq %%rdi\n\t"
        "popq %%rsi\n\t"
        "popq %%rbp\n\t"
        "popq %%rdx\n\t"
        "popq %%rcx\n\t"
        "popq %%rbx\n\t"
        "popq %%rax\n\t"
        //恢复进程保存在内核栈中的RIP、CS、RFLAGS，（有可能需要恢复进程应用程序的RSP、SS）寄存器
        "iretq\n\t"
        :
        : [ NEXT_RSP ] "m"(thrdp->td_context.ctx_nextrsp)
        : "memory");
#endif
}
