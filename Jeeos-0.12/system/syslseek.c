/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - 文件定位(lseek)
 *
 * 实现POSIX lseek()系统调用：
 * - 验证文件句柄
 * - 更新文件偏移位置
 *
 * 函数接口：
 * - syscall_lseek(): 系统调用表入口
 * - sys_lseek(): 用户接口
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "sys_deps.h"

/**
 * 系统调用表lseek入口
 * 从栈参数中提取参数并调用核心函数
 */
sysstus_t syscall_lseek(uint_t inr, stkparame_t *stkparv) {
    (void)inr;
    
    if (stkparv == NULL) {
        return SYSSTUSERR;
    }
    
    /* 提取参数: parmv1=fhand, parmv2=pos */
    hand_t fhand = (hand_t)stkparv->parmv1;
    uint_t pos = (uint_t)stkparv->parmv2;
    
    /* 调用核心定位函数 */
    sysstus_t ret = sys_lseek(fhand, pos);
    
    /* 将结果存入parmv1返回给用户态 */
    stkparv->parmv1 = (u64_t)ret;
    
    return ret;
}

/**
 * 文件定位
 * @param fhand 文件句柄
 * @param pos 目标位置
 * @return SYSSTUSOK成功，SYSSTUSERR失败
 */
sysstus_t sys_lseek(hand_t fhand, uint_t pos) {
    if (fhand == NO_HAND) {
        return SYSSTUSERR;
    }
    
    /* 获取当前线程 */
    thread_t *curtdp = sched_retn_currthread();
    if (curtdp == NULL) {
        return SYSSTUSERR;
    }
    
    /* 获取对象节点 */
    objnode_t *ondep = thread_get_objnode(curtdp, fhand);
    if (ondep == NULL) {
        return SYSSTUSERR;
    }
    
    /* 根据对象类型处理 */
    if (ondep->on_objtype == OBJN_TY_DEV) {
        /* 设备定位 */
        device_t *devp = (device_t *)ondep->on_objadr;
        if (devp == NULL) {
            return SYSSTUSERR;
        }
        
        /* 设置定位操作码和参数 */
        ondep->on_opercode = IOIF_CODE_LSEEK;
        ondep->on_currops = pos;
        
        /* 调用设备I/O */
        drvstus_t drvret = dev_io(ondep);
        if (drvret != DFCOKSTUS) {
            return SYSSTUSERR;
        }
    } else if (ondep->on_objtype == OBJN_TY_FIL) {
        /* 文件定位 - 直接更新偏移量 */
        ondep->on_currops = pos;
    } else {
        return SYSSTUSERR;
    }
    
    return SYSSTUSOK;
}
