/*
 * @Author: Jee Hsu
 * @Description: 中断和异常处理模块
 *
 * 负责x86-64架构的中断/异常管理：
 * - 中断描述符初始化和管理
 * - 中断处理函数注册和调用
 * - 异常(fault)处理分发
 * - 硬件中断(IRQ)处理分发
 *
 * 中断向量分配：
 * - 0-31: CPU异常(除零、缺页等)
 * - 32-47: 硬件中断(IRQ0-15，由8259 PIC管理)
 * - 48+: 软件中断/系统调用
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

// 初始化256个中断描述符结构实例
void int_desc_init(int_desc_t *initp, u32_t flg, u32_t sts, uint_t prity, uint_t irq) {
    arch_spinlock_init(&initp->i_lock);
    initp->i_flags = flg;
    initp->i_status = sts;
    initp->i_prity = prity;
    initp->i_irqnr = irq;
    initp->i_deep = 0;
    initp->i_indx = 0;
    list_init(&initp->i_serlist);
    initp->i_sernr = 0;
    list_init(&initp->i_serthrdlst);
    initp->i_serthrdnr = 0;
    initp->i_onethread = NULL;
    initp->i_rbtreeroot = NULL;
    list_init(&initp->i_serfisrlst);
    initp->i_serfisrnr = 0;
    initp->i_msgmpool = NULL;
    initp->i_privp = NULL;
    initp->i_extp = NULL;
    return;
}

void init_int_desc() {
    for (uint_t i = 0; i < IDTMAX; i++) {
        int_desc_init(&machintflt[i], 0, 0, i, i);
    }
    return;
}

PUBLIC void init_archint() {
    printk("  - Setting up GDT...\n");
    init_descriptor();
    
    printk("  - Setting up IDT...\n");
    init_idt_descriptor();
    
    printk("  - Initializing interrupt descriptors...\n");
    init_int_desc();
    
    printk("  - Initializing 8259 PIC...\n");
    init_i8259();
    
    /* 注意：定时器中断(IRQ0)将在定时器驱动加载时启用
     * 这样可以确保中断处理函数已经注册后再启用中断 */
    
    return;
}

PUBLIC int_desc_t *arch_retn_intfltdsc(uint_t irqnr) {
    if (irqnr > IDTMAX) {
        return NULL;
    }
    return &machintflt[irqnr];
}

void int_server_init(int_server_t *initp, u32_t flg, int_desc_t *intfltp, void *device, int_handler_t handle) {

    list_init(&initp->s_list);
    list_init(&initp->s_indevlst);
    initp->s_flags = flg;
    initp->s_intfltp = intfltp;
    initp->s_indx = 0;
    initp->s_device = device;
    initp->s_handle = handle;
    return;
}

bool_t arch_add_ihandle(int_desc_t *intdscp, int_server_t *serdscp) {
    if (intdscp == NULL || serdscp == NULL) {
        return FALSE;
    }
    cpuflg_t cpuflg;
    arch_spinlock_saveflg_cli(&intdscp->i_lock, &cpuflg);
    list_add(&serdscp->s_list, &intdscp->i_serlist);
    intdscp->i_sernr++;
    arch_spinunlock_restflg_sti(&intdscp->i_lock, &cpuflg);
    return TRUE;
}

drvstus_t arch_enable_intline(uint_t ifdnr) {
    /* x86中断向量: 0x20-0x2F 对应 IRQ0-IRQ15 */
    if (0x20 > ifdnr || 0x2F < ifdnr) {
        return DFCERRSTUS;
    }
    /* 将中断向量号转换为 IRQ 号: 向量0x20 = IRQ0, 向量0x21 = IRQ1, ... */
    i8259_enabled_line((u32_t)(ifdnr - 0x20));
    return DFCOKSTUS;
}

drvstus_t arch_disable_intline(uint_t ifdnr) {
    if (20 > ifdnr || 36 < ifdnr) {
        return DFCERRSTUS;
    }
    /* 将中断向量号转换为 IRQ 号 */
    i8259_disable_line((u32_t)(ifdnr - 20));
    return DFCOKSTUS;
}

drvstus_t arch_intflt_default(uint_t ift_nr, void *sframe) {
    if (ift_nr == 0xffffffff || sframe == NULL) {
        return DFCERRSTUS;
    }
    return DFCOKSTUS;
}

void arch_run_int_handler(uint_t ifdnr, void *sframe) {
    int_server_t *isdscp;
    list_h_t *lst;
    //根据中断号获取中断异常描述符地址
    int_desc_t *ifdscp = arch_retn_intfltdsc(ifdnr);
    if (ifdscp == NULL) {
        KERNEL_PANIC("arch_run_intfdsc: Invalid interrupt descriptor");
        return;
    }

    //遍历i_serlist链表
    list_for_each(lst, &ifdscp->i_serlist) {
        //获取i_serlist链表上对象即int_server_t结构
        isdscp = list_entry(lst, int_server_t, s_list);
        //调用中断处理回调函数
        isdscp->s_handle(ifdnr, isdscp->s_device, sframe);
    }

    return;
}

void arch_do_hwint(uint_t intnumb, void *krnlsframp) {
    int_desc_t *ifdscp = NULL;
    cpuflg_t cpuflg;
    if (intnumb > IDTMAX || krnlsframp == NULL) {
        KERNEL_PANIC("arch_do_hwint: Invalid interrupt number or frame");
        return;
    }
    //根据中断号获取中断异常描述符地址
    ifdscp = arch_retn_intfltdsc(intnumb);
    if (ifdscp == NULL) {
        KERNEL_PANIC("arch_do_hwint: NULL interrupt descriptor");
        return;
    }
    //对断异常描述符加锁并中断
    arch_spinlock_saveflg_cli(&ifdscp->i_lock, &cpuflg);
    ifdscp->i_indx++;
    ifdscp->i_deep++;
    //运行中断处理的回调函数
    arch_run_int_handler(intnumb, krnlsframp);
    ifdscp->i_deep--;
    //解锁并恢复中断状态
    arch_spinunlock_restflg_sti(&ifdscp->i_lock, &cpuflg);
    return;
}

// 系统调用处理函数声明
extern sysstus_t syshandle(uint_t inr, void *sframe);

/**
 * 系统调用分发器
 * 由 int 0xFF 触发，将系统调用请求分发到系统服务层
 * @param inr 系统调用号 (存储在 rax 中)
 * @param krnlsframp 内核栈帧指针 (包含用户态参数)
 * @return 系统调用返回值
 */
sysstus_t arch_syscall_allocator(uint_t inr, void* krnlsframp) {
    if (krnlsframp == NULL) {
        return SYSSTUSERR;
    }
    
    /* 调用系统服务层处理 */
    return syshandle(inr, krnlsframp);
}

void arch_fault_allocator(uint_t faultnumb, void *krnlsframp) {
    adr_t fairvadrs;
    //打印异常号
    printk("faultnumb is :%d\n", faultnumb);
    //如果异常号等于14则是内存缺页异常
    if (faultnumb == 14) {
        //获取缺页的地址
        fairvadrs = (adr_t)read_cr2();
        //打印缺页地址，这地址保存在CPU的CR2寄存器中
        printk("Exception address:%x, access denied\n", fairvadrs);
        //简化版不处理缺页，直接报错
        KERNEL_PANIC("Page fault handler not implemented");
        return;
    }
    //死机，不让这个函数返回了
    die(0);
    return;
}

// 调度检查函数声明
extern void sched_chkneed_pmptsched(void);

// 中断分发器
void arch_hwint_allocator(uint_t intnumb, void *krnlsframp) {
    i8259_send_eoi();
    arch_do_hwint(intnumb, krnlsframp);
    // 检查是否需要调度
    sched_chkneed_pmptsched();
    return;
}
