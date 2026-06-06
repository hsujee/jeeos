/*
 * @Author: Jee Hsu
 * @Description: 设备管理模块
 *
 * 负责设备的注册、查找和管理：
 * - 设备表(devtable_t)管理所有已注册设备
 * - 设备描述符(device_t)描述单个设备
 * - 驱动描述符(driver_t)描述设备驱动
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

/* osdevtable 已在 global.c 中通过 KRL_DEFGLOB_VARIABLE 宏定义 */

// 设备类型链表初始化
void devtlst_t_init(devtlst_t *initp, uint_t dtype) {
    initp->dtl_type = dtype;
    initp->dtl_nr = 0;
    list_init(&initp->dtl_list);
    return;
}

// 设备表初始化
void devtable_t_init(devtable_t *initp) {
    list_init(&initp->devt_list);
    spinlock_init(&initp->devt_lock);
    list_init(&initp->devt_devlist);
    list_init(&initp->devt_drvlist);
    initp->devt_devnr = 0;
    initp->devt_drvnr = 0;
    for (uint_t i = 0; i < DEVICE_MAX; i++) {
        devtlst_t_init(&initp->devt_devclsl[i], i);
    }
    return;
}

// 设备ID初始化
void devid_t_init(devid_t *initp, uint_t mty, uint_t sty, uint_t nr) {
    initp->dev_mtype = mty;
    initp->dev_stype = sty;
    initp->dev_nr = nr;
    return;
}

// 设备结构初始化
void device_t_init(device_t *initp) {
    list_init(&initp->dev_list);
    list_init(&initp->dev_indrvlst);
    list_init(&initp->dev_intbllst);
    spinlock_init(&initp->dev_lock);
    initp->dev_count = 0;
    sem_t_init(&initp->dev_sem);
    initp->dev_status = 0;
    initp->dev_flags = 0;
    devid_t_init(&initp->dev_id, 0, 0, 0);
    initp->dev_intlnenr = 0;
    list_init(&initp->dev_intserlst);
    list_init(&initp->dev_rqlist);
    initp->dev_rqlnr = 0;
    sem_t_init(&initp->dev_waitints);
    initp->dev_drv = NULL;
    initp->dev_attrb = NULL;
    initp->dev_privdata = NULL;
    initp->dev_userdata = NULL;
    initp->dev_extdata = NULL;
    initp->dev_name = NULL;
    return;
}

// 驱动程序结构初始化
void driver_t_init(driver_t *initp) {
    spinlock_init(&initp->drv_lock);
    list_init(&initp->drv_list);
    initp->drv_status = 0;
    initp->drv_flags = 0;
    initp->drv_id = 0;
    initp->drv_count = 0;
    sem_t_init(&initp->drv_sem);
    initp->drv_safedsc = NULL;
    initp->drv_attrb = NULL;
    initp->drv_privdata = NULL;
    for (uint_t i = 0; i < IOIF_CODE_MAX; i++) {
        initp->drv_dipfun[i] = NULL;
    }
    list_init(&initp->drv_alldevlist);
    initp->drv_entry = NULL;
    initp->drv_exit = NULL;
    initp->drv_userdata = NULL;
    initp->drv_extdata = NULL;
    initp->drv_name = NULL;
    return;
}

// 设备子系统初始化
void init_device() {
    devtable_t_init(&osdevtable);
    return;
}

// 驱动程序入口运行
drvstus_t run_driverentry(drventyexit_t drventry) {
    if (drventry == NULL) {
        return DFCERRSTUS;
    }
    
    /* 分配驱动描述符 */
    driver_t *drvp = new_driver_dsc();
    if (drvp == NULL) {
        return DFCERRSTUS;
    }
    
    /* 设置驱动入口函数 */
    drvp->drv_entry = drventry;
    
    /* 调用驱动入口函数 */
    drvstus_t ret = drventry(drvp, 0, NULL);
    if (ret != DFCOKSTUS) {
        del_driver_dsc(drvp);
        return ret;
    }
    
    /* 将驱动添加到系统 */
    ret = driver_add_system(drvp);
    if (ret != DFCOKSTUS) {
        del_driver_dsc(drvp);
        return ret;
    }
    
    return DFCOKSTUS;
}

/* 外部驱动入口函数声明 */
extern drvstus_t uart_entry(driver_t *drvp, uint_t val, void *p);
extern drvstus_t systick_entry(driver_t *drvp, uint_t val, void *p);
extern drvstus_t rfs_entry(driver_t *drvp, uint_t val, void *p);
extern drvstus_t nullzero_entry(driver_t *drvp, uint_t val, void *p);

/* 驱动入口表 */
typedef struct {
    drventyexit_t entry;
    const char *name;
} driver_entry_t;

static driver_entry_t driver_table[] = {
    { uart_entry,     "UART" },       /* 串口驱动 */
    { systick_entry,  "SYSTICK" },    /* 系统定时器驱动 */
    { rfs_entry,      "RFS" },        /* RAM文件系统驱动 */
    { nullzero_entry, "NULL/ZERO" },  /* 虚拟设备驱动 */
    { NULL,           NULL }          /* 结束标记 */
};

// 驱动程序初始化 - 自动加载所有驱动
void init_driver() {
    uint_t loaded = 0, failed = 0;
    
    for (uint_t i = 0; driver_table[i].entry != NULL; i++) {
        printk("  - Loading %s driver...", driver_table[i].name);
        if (run_driverentry(driver_table[i].entry) == DFCOKSTUS) {
            printk(" OK\n");
            loaded++;
        } else {
            printk(" FAILED!\n");
            failed++;
        }
    }
    
    printk("[DRIVER] %d drivers loaded, %d failed.\n", loaded, failed);
    return;
}

// 删除驱动描述符
drvstus_t del_driver_dsc(driver_t *drvp) {
    if (drvp == NULL) {
        return DFCERRSTUS;
    }
    return kmsob_delete((void *)drvp, sizeof(driver_t)) ? DFCOKSTUS : DFCERRSTUS;
}

// 创建新驱动描述符
driver_t *new_driver_dsc() {
    driver_t *drvp = (driver_t *)kmsob_new(sizeof(driver_t));
    if (drvp == NULL) {
        return NULL;
    }
    driver_t_init(drvp);
    return drvp;
}

// 删除设备描述符
drvstus_t del_device_dsc(device_t *devp) {
    if (devp == NULL) {
        return DFCERRSTUS;
    }
    return kmsob_delete((void *)devp, sizeof(device_t)) ? DFCOKSTUS : DFCERRSTUS;
}

// 创建新设备描述符
device_t *new_device_dsc() {
    device_t *devp = (device_t *)kmsob_new(sizeof(device_t));
    if (devp == NULL) {
        return NULL;
    }
    device_t_init(devp);
    return devp;
}

// 默认驱动函数 - 桩实现
drvstus_t drv_defalt_func(device_t *devp, void *iopack) {
    (void)devp;
    (void)iopack;
    return DFCERRSTUS;
}

// 比较设备ID
bool_t cmp_devid(devid_t *sdidp, devid_t *cdidp) {
    if (sdidp->dev_mtype == cdidp->dev_mtype &&
        sdidp->dev_stype == cdidp->dev_stype &&
        sdidp->dev_nr == cdidp->dev_nr) {
        return TRUE;
    }
    return FALSE;
}

// 添加驱动到系统
drvstus_t driver_add_system(driver_t *drvp) {
    if (drvp == NULL) {
        return DFCERRSTUS;
    }
    cpuflg_t cpuflg;
    arch_spinlock_saveflg_cli(&osdevtable.devt_lock, &cpuflg);
    list_add(&drvp->drv_list, &osdevtable.devt_drvlist);
    osdevtable.devt_drvnr++;
    arch_spinunlock_restflg_sti(&osdevtable.devt_lock, &cpuflg);
    return DFCOKSTUS;
}

// 设备添加到驱动
drvstus_t dev_add_driver(device_t *devp, driver_t *drvp) {
    if (devp == NULL || drvp == NULL) {
        return DFCERRSTUS;
    }
    cpuflg_t cpuflg;
    arch_spinlock_saveflg_cli(&drvp->drv_lock, &cpuflg);
    list_add(&devp->dev_indrvlst, &drvp->drv_alldevlist);
    devp->dev_drv = drvp;
    drvp->drv_count++;
    arch_spinunlock_restflg_sti(&drvp->drv_lock, &cpuflg);
    return DFCOKSTUS;
}

// 新建设备 - 注册到设备表
drvstus_t new_device(device_t *devp) {
    if (devp == NULL) {
        return DFCERRSTUS;
    }
    cpuflg_t cpuflg;
    arch_spinlock_saveflg_cli(&osdevtable.devt_lock, &cpuflg);
    
    /* 添加到全局设备链表 */
    list_add(&devp->dev_list, &osdevtable.devt_devlist);
    osdevtable.devt_devnr++;
    
    /* 添加到对应类型的链表 */
    uint_t dtype = devp->dev_id.dev_mtype;
    if (dtype < DEVICE_MAX) {
        list_add(&devp->dev_intbllst, &osdevtable.devt_devclsl[dtype].dtl_list);
        osdevtable.devt_devclsl[dtype].dtl_nr++;
    }
    
    arch_spinunlock_restflg_sti(&osdevtable.devt_lock, &cpuflg);
    return DFCOKSTUS;
}

// 设备引用计数增加
drvstus_t dev_inc_devcount(device_t *devp) {
    if (devp == NULL) {
        return DFCERRSTUS;
    }
    cpuflg_t cpuflg;
    arch_spinlock_saveflg_cli(&devp->dev_lock, &cpuflg);
    devp->dev_count++;
    arch_spinunlock_restflg_sti(&devp->dev_lock, &cpuflg);
    return DFCOKSTUS;
}

// 设备引用计数减少
drvstus_t dev_dec_devcount(device_t *devp) {
    if (devp == NULL) {
        return DFCERRSTUS;
    }
    cpuflg_t cpuflg;
    arch_spinlock_saveflg_cli(&devp->dev_lock, &cpuflg);
    if (devp->dev_count > 0) {
        devp->dev_count--;
    }
    arch_spinunlock_restflg_sti(&devp->dev_lock, &cpuflg);
    return DFCOKSTUS;
}

// 添加设备请求
drvstus_t dev_add_request(device_t *devp, objnode_t *request) {
    if (devp == NULL || request == NULL) {
        return DFCERRSTUS;
    }
    cpuflg_t cpuflg;
    arch_spinlock_saveflg_cli(&devp->dev_lock, &cpuflg);
    list_add(&request->on_list, &devp->dev_rqlist);
    devp->dev_rqlnr++;
    arch_spinunlock_restflg_sti(&devp->dev_lock, &cpuflg);
    return DFCOKSTUS;
}

// 完成设备请求
drvstus_t dev_complete_request(device_t *devp, objnode_t *request) {
    if (devp == NULL || request == NULL) {
        return DFCERRSTUS;
    }
    cpuflg_t cpuflg;
    arch_spinlock_saveflg_cli(&devp->dev_lock, &cpuflg);
    list_del(&request->on_list);
    if (devp->dev_rqlnr > 0) {
        devp->dev_rqlnr--;
    }
    arch_spinunlock_restflg_sti(&devp->dev_lock, &cpuflg);
    return DFCOKSTUS;
}

// 返回设备请求
drvstus_t dev_retn_request(device_t *devp, uint_t iocode, objnode_t **retreq) {
    if (devp == NULL || retreq == NULL) {
        return DFCERRSTUS;
    }
    
    cpuflg_t cpuflg;
    arch_spinlock_saveflg_cli(&devp->dev_lock, &cpuflg);
    
    *retreq = NULL;
    list_h_t *pos;
    list_for_each(pos, &devp->dev_rqlist) {
        objnode_t *req = list_entry(pos, objnode_t, on_list);
        if (req->on_opercode == iocode) {
            *retreq = req;
            break;
        }
    }
    
    arch_spinunlock_restflg_sti(&devp->dev_lock, &cpuflg);
    return (*retreq != NULL) ? DFCOKSTUS : DFCERRSTUS;
}

// 等待设备请求
drvstus_t dev_wait_request(device_t *devp, objnode_t *request) {
    if (devp == NULL || request == NULL) {
        return DFCERRSTUS;
    }
    /* 简化版：直接返回，不实际等待 */
    return DFCOKSTUS;
}

// 返回请求参数
drvstus_t dev_retn_rqueparm(void *request, buf_t *retbuf, uint_t *retcops,
                               uint_t *retlen, uint_t *retioclde,
                               uint_t *retbufcops, size_t *retbufsz) {
    if (request == NULL) {
        return DFCERRSTUS;
    }
    
    objnode_t *ondep = (objnode_t *)request;
    
    if (retbuf != NULL) {
        *retbuf = ondep->on_buf;
    }
    if (retcops != NULL) {
        *retcops = ondep->on_currops;
    }
    if (retlen != NULL) {
        *retlen = ondep->on_len;
    }
    if (retioclde != NULL) {
        *retioclde = ondep->on_opercode;
    }
    if (retbufcops != NULL) {
        *retbufcops = ondep->on_bufcurops;
    }
    if (retbufsz != NULL) {
        *retbufsz = ondep->on_bufsz;
    }
    
    return DFCOKSTUS;
}

// 根据ID/名称查找设备
device_t *find_device(void *dfname, uint_t flags) {
    if (dfname == NULL) {
        return NULL;
    }
    
    cpuflg_t cpuflg;
    device_t *devp = NULL;
    list_h_t *pos;
    
    arch_spinlock_saveflg_cli(&osdevtable.devt_lock, &cpuflg);
    
    if (flags == DIDFIL_IDN) {
        /* 按设备ID查找 */
        devid_t *findid = (devid_t *)dfname;
        uint_t dtype = findid->dev_mtype;
        
        if (dtype < DEVICE_MAX) {
            /* 在对应类型链表中查找 */
            list_for_each(pos, &osdevtable.devt_devclsl[dtype].dtl_list) {
                device_t *tmp = list_entry(pos, device_t, dev_intbllst);
                if (cmp_devid(&tmp->dev_id, findid)) {
                    devp = tmp;
                    break;
                }
            }
        }
    } else if (flags == DIDFIL_FLN) {
        /* 按名称查找 - dfname 指向 devid_t 结构 */
        devid_t *findid = (devid_t *)dfname;
        uint_t dtype = findid->dev_mtype;
        
        if (dtype < DEVICE_MAX) {
            list_for_each(pos, &osdevtable.devt_devclsl[dtype].dtl_list) {
                device_t *tmp = list_entry(pos, device_t, dev_intbllst);
                if (cmp_devid(&tmp->dev_id, findid)) {
                    devp = tmp;
                    break;
                }
            }
        }
    }
    
    arch_spinunlock_restflg_sti(&osdevtable.devt_lock, &cpuflg);
    return devp;
}

// 新建设备中断处理
drvstus_t new_devhandle(device_t *devp, int_handler_t handle, uint_t phyiline) {
    if (devp == NULL || handle == NULL) {
        return DFCERRSTUS;
    }
    
    /* 获取中断描述符 */
    int_desc_t *intdesc = arch_retn_intfltdsc(phyiline);
    if (intdesc == NULL) {
        return DFCERRSTUS;
    }
    
    /* 分配并初始化中断服务结构 */
    int_server_t *intser = (int_server_t *)kmsob_new(sizeof(int_server_t));
    if (intser == NULL) {
        return DFCERRSTUS;
    }
    
    int_server_init(intser, 0, intdesc, devp, handle);
    
    /* 添加到中断描述符的服务链表 */
    if (!arch_add_ihandle(intdesc, intser)) {
        kmsob_delete(intser, sizeof(int_server_t));
        return DFCERRSTUS;
    }
    
    /* 添加到设备的中断服务链表 */
    cpuflg_t cpuflg;
    arch_spinlock_saveflg_cli(&devp->dev_lock, &cpuflg);
    list_add(&intser->s_indevlst, &devp->dev_intserlst);
    devp->dev_intlnenr++;
    arch_spinunlock_restflg_sti(&devp->dev_lock, &cpuflg);
    
    return DFCOKSTUS;
}

// 设备IO操作
drvstus_t dev_io(objnode_t *nodep) {
    if (nodep == NULL) {
        return DFCERRSTUS;
    }
    
    /* 获取设备和驱动 */
    device_t *devp = (device_t *)nodep->on_objadr;
    if (devp == NULL || devp->dev_drv == NULL) {
        return DFCERRSTUS;
    }
    
    driver_t *drvp = devp->dev_drv;
    
    /* 获取操作码 */
    uint_t iocode = nodep->on_opercode;
    if (iocode >= IOIF_CODE_MAX) {
        return DFCERRSTUS;
    }
    
    /* 调用对应的驱动函数 */
    drivcallfun_t func = drvp->drv_dipfun[iocode];
    if (func == NULL) {
        return DFCERRSTUS;
    }
    
    return func(devp, (void *)nodep);
}

// 调用驱动程序 (直接调用，不通过 objnode)
drvstus_t dev_call_driver(device_t *devp, uint_t iocode, uint_t val1,
                             uint_t val2, void *p1, void *p2) {
    (void)val1;
    (void)val2;
    (void)p1;
    (void)p2;
    
    if (devp == NULL || devp->dev_drv == NULL) {
        return DFCERRSTUS;
    }
    
    driver_t *drvp = devp->dev_drv;
    
    if (iocode >= IOIF_CODE_MAX) {
        return DFCERRSTUS;
    }
    
    drivcallfun_t func = drvp->drv_dipfun[iocode];
    if (func == NULL) {
        return DFCERRSTUS;
    }
    
    /* 创建临时 objnode 用于传参 */
    objnode_t tmpnode;
    objnode_t_init(&tmpnode);
    tmpnode.on_objadr = devp;
    tmpnode.on_opercode = iocode;
    
    return func(devp, (void *)&tmpnode);
}

// 返回驱动ID - 桩实现
void retn_driverid(driver_t *dverp) {
    (void)dverp;
    return;
}
