/*
 * @Author: Jee Hsu
 * @Description: 引导参数C语言部分 - 硬件信息收集
 *
 * 二级引导器的核心模块,负责:
 * 1. 检查CPU是否支持64位长模式
 * 2. 收集E820内存映射信息
 * 3. 设置内核初始栈
 * 4. 加载内核映像(jeeos.bin)
 * 5. 加载字体文件(font.fnt)
 * 6. 建立初始页表(恒等映射+高半核映射)
 * 7. 设置VBE图形模式
 *
 * 所有收集的信息保存在abootparam_t结构体中
 * 位于物理地址1MB(MBSPADR)处,供内核使用
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "bootparam_c.h"

// 初始化启动参数，主要工作如下
// 检查 CPU 模式、收集内存信息、设置内核栈，设置内核字体、建立内核MMU页表数据
void init_bootparam() {
    // 创建并初始化启动参数实例
    abootparam_t *mabp = MBSPADR;
    init_abootparam_t(mabp);
    // 检查CPU
    init_chkcpu(mabp);
    // 获取内存布局
    init_mem(mabp);
    if (0 == get_wt_imgfilesz(mabp)) {
        kerror("imgfilesz 0");
    }
    // 初始化内核栈
    init_stack(mabp);
    // 放置内核文件
    init_kernel_img(mabp);
    // 放置字库文件
    init_defutfont(mabp);
    // 放置e820map的内存映射
    init_meme820(mabp);
    // 建立MMU页表
    init_mmpages(mabp);
    // 设置图形模式
    init_graph(mabp);
    return;
}

// 在 1MB 内存地址处初始化了一个机器信息结构 abootparam_t
void init_abootparam_t(abootparam_t *initp) {
    memset(initp, 0, sizeof(abootparam_t));
    initp->mb_migc = MBS_MIGC;
    return;
}

int adrzone_is_ok(u64_t sadr, u64_t slen, u64_t kadr, u64_t klen) {
    if (kadr >= sadr && kadr <= (sadr + slen)) {
        return -1;
    }
    /*if(kadr<=sadr&&((kadr+klen)>=(sadr+slen)))
    {
        return -2;
    }*/
    if (kadr <= sadr && ((kadr + klen) >= sadr)) {
        return -2;
    }

    return 0;
}

int chkadr_is_ok(abootparam_t *mabp, u64_t chkadr, u64_t cksz) {
    //u64_t len=chkadr+cksz;
    if (adrzone_is_ok((mabp->mb_krlinitstack - mabp->mb_krlitstacksz),
            mabp->mb_krlitstacksz, chkadr, cksz) != 0) {
        return -1;
    }

    if (adrzone_is_ok(mabp->mb_imgpadr, mabp->mb_imgsz, chkadr, cksz) != 0) {
        return -2;
    }

    if (adrzone_is_ok(mabp->mb_krlimgpadr, mabp->mb_krlsz, chkadr, cksz) != 0) {
        return -3;
    }

    if (adrzone_is_ok(mabp->mb_bfontpadr, mabp->mb_bfontsz, chkadr, cksz) != 0) {
        return -4;
    }

    if (adrzone_is_ok(mabp->mb_e820padr, mabp->mb_e820sz, chkadr, cksz) != 0) {
        return -5;
    }

    if (adrzone_is_ok(mabp->mb_memznpadr, mabp->mb_memznsz, chkadr, cksz) != 0) {
        return -6;
    }

    if (adrzone_is_ok(mabp->mb_memmappadr, mabp->mb_memmapsz, chkadr, cksz) != 0) {
        return -7;
    }

    return 0;
}

void testm2m() {
    u8_t *s = (u8_t *)0x400000;
    u8_t *d = (u8_t *)(0x400000 - 1);

    for (u8_t i = 0; i < 10; i++) {
        s[i] = i;
    }

    for (u8_t j = 0; j < 10; j++) {
        printk("s[%d]:%d\n", j, s[j]);
    }

    m2mcopy(s, d, 10);

    for (u8_t k = 0; k < 10; k++) {
        printk("d[%d]:%d\n", k, d[k]);
    }

    die(0);
    return;
}

void disp_mabp(abootparam_t *mabp) {
    printk("mabp->mb_imgpadr:%x\n", mabp->mb_imgpadr);
    printk("mabp->mb_imgsz:%x\n", mabp->mb_imgsz);
    printk("mabp->mb_krlimgpadr:%x\n", mabp->mb_krlimgpadr);
    printk("mabp->mb_krlimgsz:%x\n", mabp->mb_krlsz);
    printk("mabp->mb_e820padr:%x\n", mabp->mb_e820padr);
    printk("mabp->mb_e820nr:%x\n", mabp->mb_e820nr);
    printk("mabp->mb_e820sz:%x\n", mabp->mb_e820sz);
    printk("mabp->mb_nextwtpadr:%x\n", mabp->mb_nextwtpadr);
    printk("mabp->mb_kalldendpadr:%x\n", mabp->mb_kalldendpadr);
    printk("mabp->mb_pml4padr:%x\n", mabp->mb_pml4padr);
    printk("mabp->mb_subpageslen:%x\n", mabp->mb_subpageslen);
    printk("mabp->mb_kpmapphymemsz:%x\n", mabp->mb_kpmapphymemsz);
    printk("mabp->mb_cpumode:%x\n", mabp->mb_cpumode);
    printk("mabp->mb_memsz:%x\n", mabp->mb_memsz);
    printk("mabp->mb_krlinitstack:%x\n", mabp->mb_krlinitstack);
    printk("mabp->mb_krlitstacksz:%x\n", mabp->mb_krlitstacksz);
    printk("mabp->mb_bfontpadr:%x\n", mabp->mb_bfontpadr);
    printk("mabp->mb_bfontsz:%x\n", mabp->mb_bfontsz);
    die(0);
    return;
}