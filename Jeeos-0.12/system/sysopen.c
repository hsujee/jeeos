/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - 文件打开(open)
 *
 * 实现POSIX open()系统调用：
 * - 解析文件路径
 * - 查找/创建文件
 * - 分配文件句柄
 * - 返回句柄给用户态
 *
 * 函数接口：
 * - syscall_open(): 系统调用表入口
 * - sys_open(): 用户接口
 * - sys_core_open(): 核心实现
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "sys_deps.h"

/**
 * 系统调用表open入口
 * 从栈参数中提取参数并调用核心函数
 */
sysstus_t syscall_open(uint_t inr, stkparame_t *stkparv) {
    (void)inr;
    
    if (stkparv == NULL) {
        return SYSSTUSERR;
    }
    
    /* 提取参数: parmv1=file, parmv2=flags, parmv3=stus */
    void *file = (void *)stkparv->parmv1;
    uint_t flags = (uint_t)stkparv->parmv2;
    uint_t stus = (uint_t)stkparv->parmv3;
    
    /* 调用核心打开函数 */
    hand_t hand = sys_core_open(file, flags, stus);
    
    /* 将结果存入parmv1返回给用户态 */
    stkparv->parmv1 = (u64_t)hand;
    
    return (hand != NO_HAND) ? SYSSTUSOK : SYSSTUSERR;
}

/**
 * 系统打开接口
 */
hand_t sys_open(void *file, uint_t flags, uint_t stus) {
    return sys_core_open(file, flags, stus);
}

/**
 * 核心打开实现
 * @param file 文件/设备名称
 * @param flags 打开标志 (FILE_TY_DEV, RW_FLG等)
 * @param stus 打开状态
 * @return 文件句柄，失败返回NO_HAND
 */
hand_t sys_core_open(void *file, uint_t flags, uint_t stus) {
    hand_t rethand = NO_HAND;
    
    if (file == NULL) {
        return NO_HAND;
    }
    
    /* 获取当前线程 */
    thread_t *curtdp = sched_retn_currthread();
    if (curtdp == NULL) {
        return NO_HAND;
    }
    
    /* 分配对象节点 */
    objnode_t *ondep = new_objnode();
    if (ondep == NULL) {
        return NO_HAND;
    }
    
    /* 判断文件类型 */
    uint_t filetype = flags & FILE_TY_MASK;
    
    if (filetype == FILE_TY_DEV) {
        /* 设备打开 */
        /* 查找设备 */
        device_t *devp = find_device(file, DIDFIL_FLN);
        if (devp == NULL) {
            del_objnode(ondep);
            return NO_HAND;
        }
        
        /* 设置对象节点 */
        ondep->on_objtype = OBJN_TY_DEV;
        ondep->on_objadr = devp;
        ondep->on_acsflags = (flags >> RWO_FLG_BITS) & RWO_FLG_MASK;
        ondep->on_acsstus = stus;
        ondep->on_fname = file;
        
        /* 调用设备打开 */
        if (sys_open_device(ondep) != SYSSTUSOK) {
            del_objnode(ondep);
            return NO_HAND;
        }
    } else {
        /* 文件打开 (FILE_TY_FILE) */
        ondep->on_objtype = OBJN_TY_FIL;
        ondep->on_acsflags = (flags >> RWO_FLG_BITS) & RWO_FLG_MASK;
        ondep->on_acsstus = stus;
        ondep->on_fname = file;
        
        /* TODO: 文件系统支持 */
        /* 目前仅支持设备打开 */
        del_objnode(ondep);
        return NO_HAND;
    }
    
    /* 将对象节点添加到当前线程并返回句柄 */
    rethand = thread_add_objnode(curtdp, ondep);
    if (rethand == NO_HAND) {
        del_objnode(ondep);
        return NO_HAND;
    }
    
    ondep->on_count = 1;
    
    return rethand;
}

/**
 * 设备打开操作
 * 调用驱动的打开函数
 */
sysstus_t sys_open_device(objnode_t *ondep) {
    if (ondep == NULL) {
        return SYSSTUSERR;
    }
    
    device_t *devp = (device_t *)ondep->on_objadr;
    if (devp == NULL) {
        return SYSSTUSERR;
    }
    
    /* 设置打开操作码 */
    ondep->on_opercode = IOIF_CODE_OPEN;
    
    /* 调用设备I/O */
    drvstus_t drvret = dev_io(ondep);
    if (drvret != DFCOKSTUS) {
        return SYSSTUSERR;
    }
    
    /* 增加设备引用计数 */
    dev_inc_devcount(devp);
    
    return SYSSTUSOK;
}
