/*
 * @Author: Jee Hsu
 * @Description: 系统调用分发入口
 *
 * 接收来自用户态的系统调用请求并分发：
 * - 验证系统调用号
 * - 提取参数
 * - 调用对应的处理函数
 * - 返回结果
 *
 * 系统调用流程：
 * 用户态 -> int 0xFF -> arch_syscall_allocator -> syshandle -> 具体处理
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "sys_deps.h"

/* 系统调用处理函数声明 */
extern sysstus_t syscall_mmap(uint_t inr, stkparame_t *stkparv);
extern sysstus_t syscall_thread(uint_t inr, stkparame_t *stkparv);
extern sysstus_t syscall_open(uint_t inr, stkparame_t *stkparv);
extern sysstus_t syscall_close(uint_t inr, stkparame_t *stkparv);
extern sysstus_t syscall_read(uint_t inr, stkparame_t *stkparv);
extern sysstus_t syscall_write(uint_t inr, stkparame_t *stkparv);
extern sysstus_t syscall_ioctrl(uint_t inr, stkparame_t *stkparv);
extern sysstus_t syscall_lseek(uint_t inr, stkparame_t *stkparv);
extern sysstus_t syscall_time(uint_t inr, stkparame_t *stkparv);

/* 无效系统调用处理 */
static sysstus_t syscall_invalid(uint_t inr, stkparame_t *stkparv) {
    (void)inr;
    (void)stkparv;
    return SYSSTUSERR;
}

/* 时间系统调用 */
sysstus_t syscall_time(uint_t inr, stkparame_t *stkparv) {
    (void)inr;
    (void)stkparv;
    // TODO: 实现时间获取
    return SYSSTUSOK;
}

/* 系统调用表 */
static syscall_t syscall_table[INR_MAX] = {
    [0] = syscall_invalid,              /* 0: 保留 */
    [INR_MM_ALLOC] = syscall_mmap,      /* 1: 内存分配 */
    [INR_MM_FREE] = syscall_mmap,       /* 2: 内存释放 */
    [INR_TD_EXEL] = syscall_thread,     /* 3: 线程执行 */
    [INR_TD_EXIT] = syscall_thread,     /* 4: 线程退出 */
    [INR_TD_HAND] = syscall_thread,     /* 5: 线程句柄 */
    [INR_TD_RSTATS] = syscall_thread,   /* 6: 线程状态读取 */
    [INR_TD_SSTATS] = syscall_thread,   /* 7: 线程状态设置 */
    [INR_FS_OPEN] = syscall_open,       /* 8: 文件打开 */
    [INR_FS_CLOSE] = syscall_close,     /* 9: 文件关闭 */
    [INR_FS_READ] = syscall_read,       /* a: 文件读取 */
    [INR_FS_WRITE] = syscall_write,     /* b: 文件写入 */
    [INR_FS_IOCTRL] = syscall_ioctrl,   /* c: 设备控制 */
    [INR_FS_LSEEK] = syscall_lseek,     /* d: 文件定位 */
    [INR_TIME] = syscall_time,          /* e: 时间获取 */
};

/**
 * 系统调用处理函数
 * @param inr 系统调用号
 * @param sframe 栈帧指针(stkparame_t*)
 * @return 系统调用状态码
 */
sysstus_t syshandle(uint_t inr, void *sframe) {
    stkparame_t *stkparv = (stkparame_t *)sframe;
    
    /* 验证系统调用号 */
    if (inr >= INR_MAX) {
        return SYSSTUSERR;
    }
    
    /* 验证系统调用表项 */
    syscall_t handler = syscall_table[inr];
    if (handler == NULL) {
        return SYSSTUSERR;
    }
    
    /* 调用对应的系统调用处理函数 */
    return handler(inr, stkparv);
}
