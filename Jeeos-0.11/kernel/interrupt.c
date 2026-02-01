/*
 * @Author: Jee Hsu
 * @Description: 中断服务注册管理
 *
 * 提供设备驱动注册中断处理函数的接口：
 * - add_irqhandle(): 添加中断处理函数
 * - del_irqhandle(): 删除中断处理函数
 *
 * 中断处理流程：
 * 1. 硬件触发中断
 * 2. CPU跳转到IDT中的入口
 * 3. 保存上下文，调用arch_hwint_allocator
 * 4. 遍历int_server_t链表调用处理函数
 * 5. 发送EOI，恢复上下文
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

int_server_t *add_irqhandle(void *device, int_handler_t handle, uint_t phyiline) {
    if (device == NULL || handle == NULL) {
        return NULL;
    }
    //根据设备中断线返回对应中断异常描述符
    int_desc_t *intp = arch_retn_intfltdsc(phyiline);
    if (intp == NULL) {
        return NULL;
    }
    int_server_t *serdscp = (int_server_t *)new_mem(sizeof(int_server_t));
    if (serdscp == NULL) {
        return NULL;
    }
    //初始化int_server_t结构体实例变量，并把设备指针和回调函数放入其中
    int_server_init(serdscp, 0, intp, device, handle);
    //把int_server_t结构体实例变量挂载到中断异常描述符结构中
    if (arch_add_ihandle(intp, serdscp) == FALSE) {
        if (delete_mem((adr_t)serdscp, sizeof(int_server_t)) == FALSE) {
            arch_sysdie("add_irqhandle ERR");
        }
        return NULL;
    }

    return serdscp;
}

drvstus_t enable_intline(uint_t ifdnr) {
    return arch_enable_intline(ifdnr);
}

drvstus_t disable_intline(uint_t ifdnr) {
    return arch_disable_intline(ifdnr);
}
