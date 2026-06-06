/*
 * @Author: Jee Hsu
 * @Description: CPU空闲进程和测试进程
 *
 * 提供系统空闲时的处理和测试进程：
 * - cpu_idle: CPU空闲进程(每CPU一个)
 * - thread_a/b_main: 测试进程A/B
 * - init_cpuidle(): 创建空闲进程
 *
 * 空闲进程功能：
 * - 当没有就绪进程时运行
 * - 可执行节能操作(HLT指令)
 * - 优先级最低，永不退出
 *
 * 调度器初始化完成后：
 * 1. 创建空闲进程
 * 2. 创建测试进程(可选)
 * 3. 开始调度循环
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

/* 系统运行统计 */
volatile uint_t sys_idle_loops = 0;

#if 0  /* 测试线程 A/B - 0.12 版本已禁用 */
/*===========================================================================
 * 显示配置
 *===========================================================================*/

/* 状态打印间隔（循环次数）- 值越大打印越少 */
#define PRINT_INTERVAL          200

/* 最大打印次数（之后停止打印，保留启动信息可见） */
#define MAX_STATUS_PRINTS       5

/* 旋转指示字符 */
static const char spinner_chars[] = "|/-\\";
static uint_t spinner_idx = 0;
static uint_t print_count = 0;

volatile uint_t sys_thread_a_runs = 0;  /* 进程A运行次数 */
volatile uint_t sys_thread_b_runs = 0;  /* 进程B运行次数 */

//进程A主函数 - 负责打印系统状态
void thread_a_main() {
    printk("  - [Thread A] Running\n");
    for (;;) {
        sys_thread_a_runs++;
        
        /* 每 PRINT_INTERVAL 次打印一次系统状态，最多打印 MAX_STATUS_PRINTS 次 */
        if ((sys_thread_a_runs % PRINT_INTERVAL) == 0 && print_count < MAX_STATUS_PRINTS) {
            char spinner = spinner_chars[spinner_idx++ % 4];
            /* 时间取模确保在有效范围内 */
            uint_t h = osktime.kt_hour % 24;
            uint_t m = osktime.kt_min % 60;
            uint_t s = osktime.kt_sec % 60;
            printk("[%c] %d:%d:%d | Thread A sched count:%d Thread B sched count:%d\n",
                   spinner, h, m, s,
                   sys_thread_a_runs, sys_thread_b_runs);
            print_count++;
            
            /* 最后一次打印后提示 */
            if (print_count == MAX_STATUS_PRINTS) {
                printk("[*] The printing has stopped, but the system continues to run.\n");
            }
        }
        
        die(200);
        schedul();
    }
    return;
}

//进程B主函数
void thread_b_main() {
    printk("  - [Thread B] Running\n");
    for (;;) {
        sys_thread_b_runs++;
        die(150);
        schedul();
    }
    return;
}

void init_ab_thread() {
    //建立进程A
    new_thread((void*)thread_a_main, KERNTHREAD_FLG, 
                PRILG_SYS, PRITY_MIN, DAFT_TDUSRSTKSZ, DAFT_TDKRLSTKSZ, "thread_a");
    //建立进程B
    new_thread((void*)thread_b_main, KERNTHREAD_FLG, 
                PRILG_SYS, PRITY_MIN, DAFT_TDUSRSTKSZ, DAFT_TDKRLSTKSZ, "thread_b");
    return;
}
#endif /* 测试线程 A/B */

void init_cpuidle() {
    //建立空转进程
    new_cpuidle();
    //创建 init 进程（PID=1），init 会 fork 启动 shell
    init_process();
    /* Idle 进程初始化完成 */
    //启动空转进程运行 (此函数不返回)
    cpuidle_start();

    return;
}

void cpuidle_start() {
    uint_t cpuid = arch_retn_cpuid();
    schdata_t *schdap = &osschedcls.scls_schda[cpuid];
    //取得空转进程
    thread_t *tdp = schdap->sda_cpuidle;
    //设置空转进程的tss和R0特权级的栈
    tdp->td_context.ctx_nexttss = &x64tss[cpuid];
    tdp->td_context.ctx_nexttss->rsp0 = tdp->td_krlstktop;
    //设置空转进程的状态为运行状态
    tdp->td_status = TDSTUS_RUN;
    //启动进程运行
    retnfrom_first_sched(tdp);

    return;
}

thread_t *new_cpuidle_thread() {
    thread_t *ret_td = NULL;
    bool_t acs = FALSE;
    adr_t krlstkadr = NULL;
    uint_t cpuid = arch_retn_cpuid();
    schdata_t *schdap = &osschedcls.scls_schda[cpuid];
    //分配进程的内核栈
    krlstkadr = new_mem(DAFT_TDKRLSTKSZ);
    if (krlstkadr == NULL) {
        return NULL;
    }
    //分配thread_t结构体变量
    ret_td = new_thread_dsc();
    if (ret_td == NULL) {
        acs = delete_mem(krlstkadr, DAFT_TDKRLSTKSZ);
        if (acs == FALSE) {
            return NULL;
        }
        return NULL;
    }
    //设置进程名称和权限
    thread_set_name(ret_td, "idle");
    ret_td->td_privilege = PRILG_SYS;
    ret_td->td_priority = PRITY_MIN;
    //设置进程的内核栈顶和内核栈开始地址
    ret_td->td_krlstktop = krlstkadr + (adr_t)(DAFT_TDKRLSTKSZ - 1);
    ret_td->td_krlstkstart = krlstkadr;
    //初始化进程的内核栈
    thread_kernstack_init(ret_td, (void *)cpuidle_main, KMOD_EFLAGS);
    //设置调度系统数据结构的空转进程和当前进程为ret_td
    schdap->sda_cpuidle = ret_td;
    schdap->sda_currtd = ret_td;

    return ret_td;
}

//新建空转进程
void new_cpuidle() {
    //建立空转进程
    thread_t *thp = new_cpuidle_thread();
    if (thp == NULL) {
        //失败则主动死机
        arch_sysdie("newcpuilde err");
    }
    printk("  - Idle process created at %x\n", (uint_t)thp);
    return;
}

void cpuidle_main() {
    /* 浅绿色显示运行信息 */
    pixl_t oldpx = set_deffontpx(BGRA(0x90, 0xee, 0x90));
    printk("[Jeeos-0.12] System Running!\n");
    set_deffontpx(oldpx);

    for (;;) {
        sys_idle_loops++;
        /* idle 进程只在没有其他进程时运行 */
        schedul();
    }
    return;
}
