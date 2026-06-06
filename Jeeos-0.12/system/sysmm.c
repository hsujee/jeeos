/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - 内存管理(mmap/munmap)
 *
 * 实现内存管理系统调用：
 * - mmap: 内存映射/分配
 * - munmap: 内存解映射/释放
 *
 * 函数接口：
 * - syscall_mmap(): 系统调用表入口
 * - sys_mmap(): 内存映射
 * - sys_munmap(): 内存解映射
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "sys_deps.h"

/**
 * 系统调用表mmap入口
 * 从栈参数中提取参数并调用核心函数
 */
sysstus_t syscall_mmap(uint_t inr, stkparame_t *stkparv) {
    if (stkparv == NULL) {
        return SYSSTUSERR;
    }
    
    if (inr == INR_MM_ALLOC) {
        /* 内存分配 */
        /* 提取参数: parmv1=addr, parmv2=len, parmv3=prot, parmv4=flags */
        void *addr = (void *)stkparv->parmv1;
        size_t len = (size_t)stkparv->parmv2;
        uint_t prot = (uint_t)stkparv->parmv3;
        uint_t flags = (uint_t)stkparv->parmv4;
        
        /* 调用内存映射函数 */
        void *retaddr = sys_mmap(addr, len, prot, flags);
        
        /* 将结果存入parmv1返回给用户态 */
        stkparv->parmv1 = (u64_t)retaddr;
        
        return (retaddr != NULL) ? SYSSTUSOK : SYSSTUSERR;
        
    } else if (inr == INR_MM_FREE) {
        /* 内存释放 */
        /* 提取参数: parmv1=addr, parmv2=len */
        void *addr = (void *)stkparv->parmv1;
        size_t len = (size_t)stkparv->parmv2;
        
        /* 调用内存解映射函数 */
        sysstus_t ret = sys_munmap(addr, len);
        
        /* 将结果存入parmv1返回给用户态 */
        stkparv->parmv1 = (u64_t)ret;
        
        return ret;
    }
    
    return SYSSTUSERR;
}

/**
 * 内存映射
 * @param addr 建议映射地址(可为NULL)
 * @param len 映射长度
 * @param prot 保护标志
 * @param flags 映射标志
 * @return 映射地址，失败返回NULL
 */
void *sys_mmap(void *addr, size_t len, uint_t prot, uint_t flags) {
    (void)addr;
    (void)prot;
    (void)flags;
    
    if (len == 0) {
        return NULL;
    }
    
    /* 对齐到页大小 */
    size_t aligned_len = ALIGN(len, PAGE_SIZE);
    
    /* 分配内存 */
    adr_t memaddr = new_mem(aligned_len);
    if (memaddr == (adr_t)NULL) {
        return NULL;
    }
    
    /* 清零 */
    memset((void *)memaddr, 0, aligned_len);
    
    return (void *)memaddr;
}

/**
 * 取消内存映射
 * @param addr 映射地址
 * @param len 映射长度
 * @return SYSSTUSOK成功，SYSSTUSERR失败
 */
sysstus_t sys_munmap(void *addr, size_t len) {
    if (addr == NULL || len == 0) {
        return SYSSTUSERR;
    }
    
    /* 对齐到页大小 */
    size_t aligned_len = ALIGN(len, PAGE_SIZE);
    
    /* 释放内存 */
    if (delete_mem((adr_t)addr, aligned_len) == FALSE) {
        return SYSSTUSERR;
    }
    
    return SYSSTUSOK;
}
