/*
 * @Author: Jee Hsu
 * @Description: 引导扇区C语言部分 - 二级引导器加载
 *
 * 由bootsect.asm调用，负责：
 * 1. 初始化VGA光标和屏幕
 * 2. 从jeeos.eki映像中加载bootparam.bin到0x1000
 * 3. 从jeeos.eki映像中加载setup.bin到0x8000
 *
 * 文件加载流程：
 * 1. 解析jeeos.eki中的文件头(fhdsc_t)
 * 2. 使用m2mcopy复制文件到目标地址
 * 3. 跳转到bootparam.bin继续引导
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "bootsect_c.h"

// GRUB头的入口函数
// 将二级引导器文件copy到内存的指定位置
// 初始化硬件信息
void head_entry() {
    init_curs();
    close_curs();
    clear_screen(VGADP_DFVL);

    write_bootparam();
    write_setup();

    return;
}

// 将bootparam.bin文件写到内存中
void write_bootparam() {
    fhdsc_t* fhdscstart = find_file("bootparam.bin");
    if (fhdscstart == NULL) {
        error("not file bootparam.bin");
    }
    // 调用 m2mcopy 函数把bootparam.bin复制到内存空间(0x1000)
    m2mcopy((void*)((u32_t)(fhdscstart->fhd_intsfsoff) + LDRFILEADR),
        (void*)REALDRV_PHYADR, (sint_t)fhdscstart->fhd_frealsz);
    return;
}

// 将setup.bin文件写到特定的内存中
void write_setup() {
    fhdsc_t* fhdscstart = find_file("setup.bin");
    if (fhdscstart == NULL) {
        error("not file setup.bin");
    }
    // 调用 m2mcopy 函数把setup.bin复制到内存空间(0x200000)
    m2mcopy((void*)((u32_t)(fhdscstart->fhd_intsfsoff) + LDRFILEADR),
        (void*)ILDRKRL_PHYADR, (sint_t)fhdscstart->fhd_frealsz);

    return;
}

//在映像文件中查找指定文件
fhdsc_t* find_file(char_t* fname) {
    mlosrddsc_t* mrddadrs = MRDDSC_ADR;
    if (mrddadrs->mdc_endgic != MDC_ENDGIC ||
        mrddadrs->mdc_rv != MDC_RVGIC ||
        mrddadrs->mdc_fhdnr < 2 ||
        mrddadrs->mdc_filnr < 2) {
        error("no mrddsc");
    }

    s64_t rethn = -1;
    // 扫描映像文件中的文件头描述符这样就可以得到文件在映像文件中的位置和大小了
    fhdsc_t* fhdscstart = (fhdsc_t*)((u32_t)(mrddadrs->mdc_fhdbk_s) + LDRFILEADR);

    // 找到fname文件，获取对应文件头描述符的地址作为返回值
    for (u64_t i = 0; i < mrddadrs->mdc_fhdnr; i++) {
        if (strcmpl(fname, fhdscstart[i].fhd_name) == 0) {
            rethn = (s64_t)i;
            goto ok_l;
        }
    }
    rethn = -1;
ok_l:
    if (rethn < 0) {
        error("not find file");
    }
    return &fhdscstart[rethn];
}

int strcmpl(const char* a, const char* b) {
    while (*b && *a && (*b == *a)) {
        b++;
        a++;
    }

    return *b - *a;
}

void error(char_t* estr) {
    printk("INITLDR DIE ERROR:%s\n", estr);
    for (;;)
        ;
    return;
}