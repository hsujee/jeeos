/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - 设备控制(ioctl)
 *
 * 实现POSIX ioctl()系统调用：
 * - 验证文件句柄
 * - 发送控制命令到设备
 * - 返回控制结果
 *
 * 函数接口：
 * - syscall_ioctrl(): 系统调用表入口
 * - sys_ioctrl(): 用户接口
 * - sys_core_ioctrl(): 核心实现
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "sys_deps.h"

/**
 * 系统调用表ioctrl入口
 * 从栈参数中提取参数并调用核心函数
 */
sysstus_t syscall_ioctrl(uint_t inr, stkparame_t *stkparv) {
    (void)inr;
    
    if (stkparv == NULL) {
        return SYSSTUSERR;
    }
    
    /* 提取参数: parmv1=fhand, parmv2=ctrlcmd, parmv3=buf, parmv4=bufsz */
    hand_t fhand = (hand_t)stkparv->parmv1;
    uint_t ctrlcmd = (uint_t)stkparv->parmv2;
    void *buf = (void *)stkparv->parmv3;
    size_t bufsz = (size_t)stkparv->parmv4;
    
    /* 调用核心控制函数 */
    sysstus_t ret = sys_core_ioctrl(fhand, ctrlcmd, buf, bufsz);
    
    /* 将结果存入parmv1返回给用户态 */
    stkparv->parmv1 = (u64_t)ret;
    
    return ret;
}

/**
 * 系统控制接口
 */
sysstus_t sys_ioctrl(hand_t fhand, uint_t ctrlcmd, void *buf, size_t bufsz) {
    return sys_core_ioctrl(fhand, ctrlcmd, buf, bufsz);
}

/**
 * 核心控制实现
 * @param fhand 文件/设备句柄
 * @param ctrlcmd 控制命令
 * @param buf 缓冲区
 * @param bufsz 缓冲区大小
 * @return SYSSTUSOK成功，SYSSTUSERR失败
 */
sysstus_t sys_core_ioctrl(hand_t fhand, uint_t ctrlcmd, void *buf, size_t bufsz) {
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
    
    /* 设置控制参数 */
    ondep->on_ioctrd = ctrlcmd;
    ondep->on_buf = (buf_t)buf;
    ondep->on_bufsz = bufsz;
    
    /* 根据对象类型处理 */
    if (ondep->on_objtype == OBJN_TY_DEV) {
        /* 设备控制 */
        if (sys_ioctrl_device(ondep) != SYSSTUSOK) {
            return SYSSTUSERR;
        }
    } else if (ondep->on_objtype == OBJN_TY_FIL) {
        /* 文件控制 */
        /* TODO: 文件系统支持 */
        return SYSSTUSERR;
    } else {
        return SYSSTUSERR;
    }
    
    return SYSSTUSOK;
}

/**
 * 设备控制操作
 * 调用驱动的控制函数
 */
sysstus_t sys_ioctrl_device(objnode_t *ondep) {
    if (ondep == NULL) {
        return SYSSTUSERR;
    }
    
    device_t *devp = (device_t *)ondep->on_objadr;
    if (devp == NULL) {
        return SYSSTUSERR;
    }
    
    /* 设置控制操作码 */
    ondep->on_opercode = IOIF_CODE_IOCTRL;
    
    /* 调用设备I/O */
    drvstus_t drvret = dev_io(ondep);
    if (drvret != DFCOKSTUS) {
        return SYSSTUSERR;
    }
    
    return SYSSTUSOK;
}
