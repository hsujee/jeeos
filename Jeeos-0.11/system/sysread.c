/*
 * @Author: Jee Hsu
 * @Description: 系统调用 - 文件读取(read)
 *
 * 实现POSIX read()系统调用：
 * - 验证文件句柄
 * - 从文件/设备读取数据
 * - 复制到用户缓冲区
 * - 返回读取字节数
 *
 * 函数接口：
 * - syscall_read(): 系统调用表入口
 * - sys_read(): 用户接口
 * - sys_core_read(): 核心实现
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "sys_deps.h"

/**
 * 系统调用表read入口
 * 从栈参数中提取参数并调用核心函数
 */
sysstus_t syscall_read(uint_t inr, stkparame_t *stkparv) {
    (void)inr;
    
    if (stkparv == NULL) {
        return SYSSTUSERR;
    }
    
    /* 提取参数: parmv1=fhand, parmv2=buf, parmv3=len */
    hand_t fhand = (hand_t)stkparv->parmv1;
    void *buf = (void *)stkparv->parmv2;
    size_t len = (size_t)stkparv->parmv3;
    
    /* 调用核心读取函数 */
    size_t readlen = sys_core_read(fhand, buf, len);
    
    /* 将结果存入parmv1返回给用户态 */
    stkparv->parmv1 = (u64_t)readlen;
    
    return (readlen > 0) ? SYSSTUSOK : SYSSTUSERR;
}

/**
 * 系统读取接口
 */
size_t sys_read(hand_t fhand, void *buf, size_t len) {
    return sys_core_read(fhand, buf, len);
}

/**
 * 核心读取实现
 * @param fhand 文件/设备句柄
 * @param buf 目标缓冲区
 * @param len 读取长度
 * @return 实际读取的字节数，失败返回0
 */
size_t sys_core_read(hand_t fhand, void *buf, size_t len) {
    if (fhand == NO_HAND || buf == NULL || len == 0) {
        return 0;
    }
    
    /* 获取当前线程 */
    thread_t *curtdp = sched_retn_currthread();
    if (curtdp == NULL) {
        return 0;
    }
    
    /* 获取对象节点 */
    objnode_t *ondep = thread_get_objnode(curtdp, fhand);
    if (ondep == NULL) {
        return 0;
    }
    
    /* 检查读权限 (on_acsflags 存储的是移位后的值) */
    if ((ondep->on_acsflags & ACS_RO) == 0 && 
        (ondep->on_acsflags & ACS_RW) == 0) {
        return 0;
    }
    
    /* 设置读取参数 */
    ondep->on_buf = (buf_t)buf;
    ondep->on_bufsz = len;
    ondep->on_len = len;
    ondep->on_bufcurops = 0;
    
    size_t retread = 0;
    
    /* 根据对象类型处理 */
    if (ondep->on_objtype == OBJN_TY_DEV) {
        /* 设备读取 */
        if (sys_read_device(ondep) == SYSSTUSOK) {
            retread = ondep->on_bufcurops;
        }
    } else if (ondep->on_objtype == OBJN_TY_FIL) {
        /* 文件读取 */
        /* TODO: 文件系统支持 */
    }
    
    /* 更新当前位置 */
    ondep->on_currops += retread;
    
    return retread;
}

/**
 * 设备读取操作
 * 调用驱动的读取函数
 */
sysstus_t sys_read_device(objnode_t *ondep) {
    if (ondep == NULL) {
        return SYSSTUSERR;
    }
    
    device_t *devp = (device_t *)ondep->on_objadr;
    if (devp == NULL) {
        return SYSSTUSERR;
    }
    
    /* 设置读取操作码 */
    ondep->on_opercode = IOIF_CODE_READ;
    
    /* 调用设备I/O */
    drvstus_t drvret = dev_io(ondep);
    if (drvret != DFCOKSTUS) {
        return SYSSTUSERR;
    }
    
    return SYSSTUSOK;
}
