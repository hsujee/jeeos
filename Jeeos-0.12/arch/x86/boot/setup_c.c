/*
 * @Author: Jee Hsu
 * @Description: 系统设置C语言部分 - 二级引导器入口
 *
 * 由setup.asm调用，是二级引导器的C语言入口：
 * 1. 初始化VGA光标和屏幕
 * 2. 调用init_bootparam()完成所有硬件初始化
 * 3. 返回后跳转到内核入口(0x2000000)
 *
 * 主要工作委托给bootparam_c.c中的init_bootparam()：
 * - CPU检测
 * - 内存收集
 * - 页表建立
 * - 图形模式设置
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "setup_c.h"

extern idtr_t IDT_PTR;

// 二级引导器的 C 入口函数，在二级引导器中需要做如下工作
// 1. 检查 CPU 是否支持 64 位的工作模式
// 2. 收集内存布局信息，看看是不是合乎我们操作系统的最低运行要求
// 3. 设置操作系统需要的 MMU 页表、设置显卡模式、释放中文字体文件
void setup_entry() {
    init_curs();
    close_curs();
    clear_screen(VGADP_DFVL);

    init_bootparam();
    return;
}

void kerror(char_t* kestr) {
    printk("INITKLDR DIE ERROR:%s\n",kestr);
    for(;;);
    return;
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
void die(u32_t dt) {
    u32_t dttt=dt,dtt=dt;
    if(dt==0) {
        for(;;);
    }
    for(u32_t i=0;i<dt;i++) {
        for(u32_t j=0;j<dtt;j++) {
            for(u32_t k=0;k<dttt;k++) {
                ;
            }
        }
    }
    return;
}
#pragma GCC pop_options
