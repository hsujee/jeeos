/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - 文件关闭(close)
 *
 * 实现POSIX close()系统调用：
 * - 验证文件句柄
 * - 刷新缓冲区
 * - 释放文件句柄
 * - 减少引用计数
 *
 * 函数接口：
 * - syscall_close(): 系统调用表入口
 * - sys_close(): 用户接口
 * - sys_core_close(): 核心实现
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "sys_deps.h"

/**
 * 系统调用表close入口
 * 从栈参数中提取参数并调用核心函数
 */
sysstus_t syscall_close(uint_t inr, stkparame_t *stkparv) {
    (void)inr;
    
    if (stkparv == NULL) {
        return SYSSTUSERR;
    }
    
    /* 提取参数: parmv1=fhand */
    hand_t fhand = (hand_t)stkparv->parmv1;
    
    /* 调用核心关闭函数 */
    sysstus_t ret = sys_core_close(fhand);
    
    /* 将结果存入parmv1返回给用户态 */
    stkparv->parmv1 = (u64_t)ret;
    
    return ret;
}

/**
 * 系统关闭接口
 */
sysstus_t sys_close(hand_t fhand) {
    return sys_core_close(fhand);
}

/**
 * 核心关闭实现
 * @param fhand 文件/设备句柄
 * @return SYSSTUSOK成功，SYSSTUSERR失败
 */
sysstus_t sys_core_close(hand_t fhand) {
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
        /* 设备关闭 */
        if (sys_close_device(ondep) != SYSSTUSOK) {
            return SYSSTUSERR;
        }
    } else if (ondep->on_objtype == OBJN_TY_FIL) {
        /* 文件关闭 */
        /* TODO: 文件系统支持 */
    }
    
    /* 从线程中删除对象节点 */
    if (thread_del_objnode(curtdp, fhand) == NO_HAND) {
        return SYSSTUSERR;
    }
    
    /* 释放对象节点 */
    del_objnode(ondep);
    
    return SYSSTUSOK;
}

/**
 * 设备关闭操作
 * 调用驱动的关闭函数
 */
sysstus_t sys_close_device(objnode_t *ondep) {
    if (ondep == NULL) {
        return SYSSTUSERR;
    }
    
    device_t *devp = (device_t *)ondep->on_objadr;
    if (devp == NULL) {
        return SYSSTUSERR;
    }
    
    /* 设置关闭操作码 */
    ondep->on_opercode = IOIF_CODE_CLOSE;
    
    /* 调用设备I/O */
    drvstus_t drvret = dev_io(ondep);
    if (drvret != DFCOKSTUS) {
        return SYSSTUSERR;
    }
    
    /* 减少设备引用计数 */
    dev_dec_devcount(devp);
    
    return SYSSTUSOK;
}
