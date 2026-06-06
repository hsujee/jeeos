/*
 * @Author: Jee Hsu
 * @Description: 文件系统模块 - 映像文件管理
 *
 * 负责从内核映像文件(jeeos.eki)中查找和加载各种文件：
 * - 内核二进制文件(jeeos.bin)
 * - 字体文件(font.fnt)
 * - 图片文件(logo.bmp等)
 *
 * 映像文件结构:
 *   mlosrddsc_t: 映像头描述符
 *   fhdsc_t[]:   文件头数组
 *   file_data:   实际文件数据
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "fs.h"

/**
 * 文件系统入口(占位)
 */
void fs_entry() {
    return;
}

/**
 * 字符串比较函数
 * @return 0表示相等，非0表示不等
 */
int strcmpl(const char *a, const char *b) {
    while (*b && *a && (*b == *a)) {
        b++;
        a++;
    }

    return *b - *a;
}

/**
 * 从映像文件中获取指定文件的描述符
 * @param fname 要查找的文件名
 * @param mabp 启动参数结构
 * @return 文件头描述符指针，失败则调用kerror
 */
fhdsc_t *get_fileinfo(char_t *fname, abootparam_t *mabp) {
    mlosrddsc_t *mrddadrs = (mlosrddsc_t *)((u32_t)(mabp->mb_imgpadr + MLOSDSC_OFF));
    if (mrddadrs->mdc_endgic != MDC_ENDGIC ||
        mrddadrs->mdc_rv != MDC_RVGIC ||
        mrddadrs->mdc_fhdnr < 2 ||
        mrddadrs->mdc_filnr < 2) {
        kerror("no mrddsc");
    }

    s64_t rethn = -1;
    fhdsc_t *fhdscstart = (fhdsc_t *)((u32_t)(mrddadrs->mdc_fhdbk_s) + ((u32_t)(mabp->mb_imgpadr)));

    for (u64_t i = 0; i < mrddadrs->mdc_fhdnr; i++) {
        if (strcmpl(fname, fhdscstart[i].fhd_name) == 0) {
            rethn = (s64_t)i;
            goto ok_l;
        }
    }
    rethn = -1;
ok_l:
    if (rethn < 0) {
        kerror("not find file");
    }
    return &fhdscstart[rethn];
}

/**
 * 移动内核映像以腾出内存空间
 * 
 * 当要分配的内存区域与映像文件冲突时，将映像移动到新位置
 * 
 * @param mabp 启动参数结构
 * @param cpyadr 需要使用的内存地址
 * @param cpysz 需要使用的内存大小
 * @return 0=地址无效, 1=移动成功, 2=无冲突无需移动, -1=无可用内存
 */
int move_krlimg(abootparam_t *mabp, u64_t cpyadr, u64_t cpysz) {
    /* 检查地址范围是否有效 */
    if (0xffffffff <= (cpyadr + cpysz) || 1 > cpysz) {
        return 0;
    }
    /* 计算映像移动的目标地址(4K对齐) */
    void *toadr = (void *)((u32_t)(P4K_ALIGN(cpyadr + cpysz)));
    sint_t tosz = (sint_t)mabp->mb_imgsz;
    
    /* 检查是否与映像区域冲突 */
    if (0 != adrzone_is_ok(mabp->mb_imgpadr, mabp->mb_imgsz, cpyadr, cpysz)) {
        /* 检查目标地址是否在可用内存范围内 */
        if (NULL == chk_memsize((e820map_t *)((u32_t)(mabp->mb_e820padr)),
            (u32_t)mabp->mb_e820nr, (u64_t)((u32_t)toadr), (u64_t)tosz)) {
            return -1;
        }
        /* 将映像移动到新位置并更新地址 */
        m2mcopy((void *)((u32_t)mabp->mb_imgpadr), toadr, tosz);
        mabp->mb_imgpadr = (u64_t)((u32_t)toadr);
        return 1;
    }
    return 2;
}

//拷贝放置内核文件
void init_kernel_img(abootparam_t *mabp) {
    // 在映像中查找相应的文件，复制到对应的地址，并返回文件的大小，这里是查找jeeos.bin文件
    u64_t sz = r_file_to_padr(mabp, IMGKRNL_PHYADR, "jeeos.bin");
    if (0 == sz) {
        kerror("r_file_to_padr err");
    }
    // 放置完成后更新机器信息结构中的数据
    mabp->mb_krlimgpadr = IMGKRNL_PHYADR;
    mabp->mb_krlsz = sz;
    // mabp->mb_nextwtpadr始终要保持指向下一段空闲内存的首地址
    mabp->mb_nextwtpadr = P4K_ALIGN(mabp->mb_krlimgpadr + mabp->mb_krlsz);
    mabp->mb_kalldendpadr = mabp->mb_krlimgpadr + mabp->mb_krlsz;
    return;
}

//拷贝放置字库文件
void init_defutfont(abootparam_t *mabp) {
    u64_t sz = 0;
    // 获取下一段空闲内存空间的首地址
    u32_t dfadr = (u32_t)mabp->mb_nextwtpadr;
    // 在映像中查找相应的文件，并复制到对应的地址，并返回文件的大小，这里是查找font.fnt文件
    sz = r_file_to_padr(mabp, dfadr, "font.fnt");
    if (0 == sz) {
        kerror("r_file_to_padr err");
    }
    // 放置完成后更新机器信息结构中的数据
    mabp->mb_bfontpadr = (u64_t)(dfadr);
    mabp->mb_bfontsz = sz;
    // 更新机器信息结构中下一段空闲内存的首地址
    mabp->mb_nextwtpadr = P4K_ALIGN((u32_t)(dfadr) + sz);
    mabp->mb_kalldendpadr = mabp->mb_bfontpadr + mabp->mb_bfontsz;
    return;
}

void get_file_rpadrandsz(char_t *fname, abootparam_t *mabp, u32_t *retadr, u32_t *retsz) {
    u64_t padr = 0, fsz = 0;
    if (NULL == fname || NULL == mabp) {
        *retadr = 0;
        return;
    }
    fhdsc_t *fhdsc = get_fileinfo(fname, mabp);
    if (fhdsc == NULL) {
        *retadr = 0;
        return;
    }
    padr = fhdsc->fhd_intsfsoff + mabp->mb_imgpadr;
    if (padr > 0xffffffff) {
        *retadr = 0;
        return;
    }
    fsz = (u32_t)fhdsc->fhd_frealsz;
    if (fsz > 0xffffffff) {
        *retadr = 0;
        return;
    }
    *retadr = (u32_t)padr;
    *retsz = (u32_t)fsz;
    return;
}

u64_t get_filesz(char_t *filenm, abootparam_t *mabp) {
    if (filenm == NULL || mabp == NULL) {
        return 0;
    }
    fhdsc_t *fhdscstart = get_fileinfo(filenm, mabp);
    if (fhdscstart == NULL) {
        return 0;
    }
    return fhdscstart->fhd_frealsz;
}

u64_t get_wt_imgfilesz(abootparam_t *mabp) {
    u64_t retsz = LDRFILEADR;
    mlosrddsc_t *mrddadrs = MRDDSC_ADR;
    if (mrddadrs->mdc_endgic != MDC_ENDGIC ||
        mrddadrs->mdc_rv != MDC_RVGIC ||
        mrddadrs->mdc_fhdnr < 2 ||
        mrddadrs->mdc_filnr < 2) {
        return 0;
    }
    if (mrddadrs->mdc_filbk_e < 0x4000) {
        return 0;
    }
    retsz += mrddadrs->mdc_filbk_e;
    retsz -= LDRFILEADR;
    mabp->mb_imgpadr = LDRFILEADR;
    mabp->mb_imgsz = retsz;
    return retsz;
}

u64_t r_file_to_padr(abootparam_t *mabp, u32_t f2adr, char_t *fnm) {
    if (NULL == f2adr || NULL == fnm || NULL == mabp) {
        return 0;
    }
    u32_t fpadr = 0, sz = 0;
    get_file_rpadrandsz(fnm, mabp, &fpadr, &sz);
    if (0 == fpadr || 0 == sz) {
        return 0;
    }
    if (NULL == chk_memsize((e820map_t *)((u32_t)mabp->mb_e820padr), (u32_t)(mabp->mb_e820nr), f2adr, sz)) {
        return 0;
    }
    if (0 != chkadr_is_ok(mabp, f2adr, sz)) {
        return 0;
    }
    m2mcopy((void *)fpadr, (void *)f2adr, (sint_t)sz);
    return sz;
}

u64_t ret_imgfilesz() {
    u64_t retsz = LDRFILEADR;
    mlosrddsc_t *mrddadrs = MRDDSC_ADR;
    if (mrddadrs->mdc_endgic != MDC_ENDGIC ||
        mrddadrs->mdc_rv != MDC_RVGIC ||
        mrddadrs->mdc_fhdnr < 2 ||
        mrddadrs->mdc_filnr < 2) {
        kerror("no mrddsc");
    }
    if (mrddadrs->mdc_filbk_e < 0x4000) {
        kerror("imgfile error");
    }
    retsz += mrddadrs->mdc_filbk_e;
    retsz -= LDRFILEADR;
    return retsz;
}
