/*
 * @Author: Jee Hsu
 * @Description: 系统时钟驱动(8254 PIT)
 *
 * 管理系统时钟中断和计时：
 * - 初始化8254可编程间隔定时器
 * - 处理时钟中断(IRQ0)
 * - 维护系统滴答计数(systick)
 * - 触发进程调度
 *
 * 时钟频率：
 * - 默认100Hz(10ms间隔)
 * - 每次中断systick++
 *
 * 驱动接口：
 * - systick_open/close: 打开/关闭
 * - systick_read: 读取当前systick
 * - systick_handle: 中断处理函数
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "drv_deps.h"

void init_8254() {
    out_u8_p(PTIPROTM, TIMEMODE);
    out_u8_p(PTIPROT1, HZLL);
    out_u8_p(PTIPROT1, HZHH);
    return;
}

void systick_set_driver(driver_t *drvp) {
    //配置驱动程序功能派发函数
    drvp->drv_dipfun[IOIF_CODE_OPEN] = systick_open;
    drvp->drv_dipfun[IOIF_CODE_CLOSE] = systick_close;
    drvp->drv_dipfun[IOIF_CODE_READ] = systick_read;
    drvp->drv_dipfun[IOIF_CODE_WRITE] = systick_write;
    drvp->drv_dipfun[IOIF_CODE_LSEEK] = systick_lseek;
    drvp->drv_dipfun[IOIF_CODE_IOCTRL] = systick_ioctrl;
    drvp->drv_dipfun[IOIF_CODE_DEV_START] = systick_dev_start;
    drvp->drv_dipfun[IOIF_CODE_DEV_STOP] = systick_dev_stop;
    drvp->drv_dipfun[IOIF_CODE_SET_POWERSTUS] = systick_set_powerstus;
    drvp->drv_dipfun[IOIF_CODE_ENUM_DEV] = systick_enum_dev;
    drvp->drv_dipfun[IOIF_CODE_FLUSH] = systick_flush;
    drvp->drv_dipfun[IOIF_CODE_SHUTDOWN] = systick_shutdown;
    //设置驱动程序名称
    drvp->drv_name = "systick0drv";
    return;
}


drvstus_t systick_entry(driver_t *drvp, uint_t val, void *p) {
    if (drvp == NULL) {
        return DFCERRSTUS;
    }
    
    /* 注册带中断的设备（不会自动启用中断） */
    dev_reg_info_t info = {
        .dev_name = "systick0",
        .dev_mtype = SYSTICK_DEVICE,
        .dev_stype = 0,
        .dev_nr = 0,
        .dev_flags = DEVFLG_SHARE
    };
    
    device_t *devp = register_device_with_irq(
        drvp, &info, systick_set_driver,
        systick_handle, 0x20
    );
    
    if (devp == NULL) {
        return DFCERRSTUS;
    }
    
    /* 初始化物理设备 */
    init_8254();
    
    /* 硬件初始化完成后，启用定时器中断 */
    if (enable_intline(0x20) == DFCERRSTUS) {
        return DFCERRSTUS;
    }
    
    return DFCOKSTUS;
}

drvstus_t systick_exit(driver_t *drvp, uint_t val, void *p) {
    return DFCERRSTUS;
}

//系统定时驱动程序中断处理函数
drvstus_t systick_handle(uint_t ift_nr, void *devp, void *sframe) {
    (void)ift_nr;
    (void)devp;
    (void)sframe;
    
    //增加系统滴答计数
    inc_systick();
    
    //更新当前进程的tick (内部会检查时间片并设置调度标志)
    thread_inc_tick(sched_retn_currthread());
    
    //更新系统时间
    update_times_from_jeeos();
    
    return DFCOKSTUS;
}

drvstus_t systick_open(device_t *devp, void *iopack) {
    return device_default_open(devp, iopack);
}

drvstus_t systick_close(device_t *devp, void *iopack) {
    return device_default_close(devp, iopack);
}

//读设备数据函数 - 返回系统滴答计数
drvstus_t systick_read(device_t *devp, void *iopack) {
    objnode_t *obp = (objnode_t *)iopack;
    
    if (obp->on_buf == NULL || obp->on_bufsz < sizeof(u64_t)) {
        return DFCERRSTUS;
    }
    
    //返回系统启动后的滴答计数
    *(u64_t *)obp->on_buf = get_systick();
    obp->on_len = sizeof(u64_t);
    
    return DFCOKSTUS;
}

//写设备数据函数
drvstus_t systick_write(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

//调整读写设备数据位置函数
drvstus_t systick_lseek(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

//控制设备函数
drvstus_t systick_ioctrl(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

//开启设备函数
drvstus_t systick_dev_start(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

//停止设备函数
drvstus_t systick_dev_stop(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

//设置设备电源函数
drvstus_t systick_set_powerstus(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

//枚举设备函数
drvstus_t systick_enum_dev(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

//刷新设备缓存函数
drvstus_t systick_flush(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}

//设备关机函数
drvstus_t systick_shutdown(device_t *devp, void *iopack) {
    return DFCERRSTUS;
}
