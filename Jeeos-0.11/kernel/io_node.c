/*
 * @Author: Jee Hsu
 * @Description: I/O请求节点管理
 *
 * 管理设备I/O操作的请求节点：
 * - objnode_t: I/O请求节点(包含操作类型、数据、状态)
 * - 支持同步和异步I/O操作
 * - 使用信号量实现完成通知
 *
 * I/O操作流程：
 * 1. 创建objnode_t并设置参数
 * 2. 提交到设备驱动
 * 3. 等待完成信号量(同步)或回调(异步)
 * 4. 获取操作结果
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

void objnode_t_init(objnode_t *initp) {
    spinlock_init(&initp->on_lock);
    list_init(&initp->on_list);
    sem_t_init(&initp->on_complesem);
    initp->on_flags = 0;
    initp->on_status = 0;
    initp->on_opercode = -1;
    initp->on_objtype = 0;
    initp->on_objadr = NULL;
    initp->on_acsflags = 0;
    initp->on_acsstus = 0;
    initp->on_currops = 0;
    initp->on_len = 0;
    initp->on_ioctrd = 0;
    initp->on_buf = NULL;
    initp->on_bufcurops = 0;
    initp->on_bufsz = 0;
    initp->on_count = 0;
    initp->on_safedsc = NULL;
    initp->on_fname = NULL;
    initp->on_finode = NULL;
    initp->on_extp = NULL;

    return;
}

objnode_t *new_objnode() {
    //建立objnode_t结构
    objnode_t *ondp = (objnode_t *)new_mem((size_t)sizeof(objnode_t));
    if (ondp == NULL) {
        return NULL;
    }
    //初始化objnode_t结构
    objnode_t_init(ondp);
    return ondp;
}

bool_t del_objnode(objnode_t *onodep) {
    //删除objnode_t结构
    if (delete_mem((adr_t)onodep, (size_t)sizeof(objnode_t)) == FALSE) {
        arch_sysdie("del_objnode err");
        return FALSE;
    }
    return TRUE;
}
