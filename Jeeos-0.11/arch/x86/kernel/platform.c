/*
 * @Author: Jee Hsu
 * @Description: 平台初始化模块 - 硬件抽象和引导参数处理
 *
 * 负责x86平台的底层初始化：
 * - 虚拟/物理地址转换
 * - 引导参数复制和管理
 * - 内核映像重定位
 * - 早期串口调试输出
 *
 * 内存布局(高半核映射)：
 * - 物理地址0开始的内存映射到0xffff800000000000
 * - 内核运行在虚拟地址空间的高半部分
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

// 早期串口调试输出（在视频初始化之前使用）
void early_serial_putc(char c) {
    // 等待串口就绪
    while ((in_u8(0x3f8 + 5) & 0x20) == 0);
    out_u8(0x3f8, (u8_t)c);
}

void early_serial_puts(const char* s) {
    while (*s) {
        if (*s == '\n') early_serial_putc('\r');
        early_serial_putc(*s++);
    }
}

adr_t viradr_to_phyadr(adr_t kviradr) {
    if (kviradr < KRNL_MAP_VIRTADDRESS_START || kviradr > KRNL_MAP_VIRTADDRESS_END) {
        system_error("virtadr_to_phyadr err\n");
        return KRNL_ADDR_ERROR;
    }
    return kviradr - KRNL_MAP_VIRTADDRESS_START;
}

adr_t phyadr_to_viradr(adr_t kphyadr) {
    if (kphyadr >= KRNL_MAP_PHYADDRESS_END) {
        system_error("phyadr_to_viradr err\n");
        return KRNL_ADDR_ERROR;
    }
    return kphyadr + KRNL_MAP_VIRTADDRESS_START;
}

void init_abootparam_t(abootparam_t* initp) {
    memset(initp, 0, sizeof(abootparam_t));
    return;
}

// 将二级引导器建立的机器信息结构，复制到全局变量 kabootparam 中
void init_abootparam() {
    abootparam_t* kmabp = &kabootparam;
    abootparam_t* smabp = MBSPADR; // 物理地址1MB处
    init_abootparam_t(kmabp);
    // 复制，要把地址转换成虚拟地址
    memcopy((void*)phyadr_to_viradr((adr_t)smabp), (void*)kmabp, sizeof(abootparam_t));
    return;
}

// 平台初始化函数
void init_archplalt() {
    early_serial_puts("[EARLY] init_archplalt start\n");
    // 复制机器信息结构
    init_abootparam();
    early_serial_puts("[EARLY] init_abootparam done\n");
    // 打印视频参数调试信息
    {
        u32_t mode = kabootparam.mb_ghparm.gh_mode;
        u32_t x = kabootparam.mb_ghparm.gh_x;
        u32_t y = kabootparam.mb_ghparm.gh_y;
        u64_t fb = kabootparam.mb_ghparm.gh_framphyadr;
        early_serial_puts("[VIDEO] mode=");
        // VBEMODE=1, GPUMODE=2, BGAMODE=3
        if (mode == 0) early_serial_puts("0(none)");
        else if (mode == 1) early_serial_puts("1(VBE)");
        else if (mode == 2) early_serial_puts("2(GPU)");
        else if (mode == 3) early_serial_puts("3(BGA)");
        else {
            early_serial_puts("unknown(");
            early_serial_putc('0' + (mode / 10));
            early_serial_putc('0' + (mode % 10));
            early_serial_puts(")");
        }
        early_serial_puts(" res=");
        // 简单显示分辨率
        if (x == 1024 && y == 768) early_serial_puts("1024x768");
        else if (x == 800 && y == 600) early_serial_puts("800x600");
        else early_serial_puts("other");
        early_serial_puts(" fb=0x");
        // 打印帧缓冲地址的十六进制值
        {
            char hex[17];
            u64_t val = fb;
            for (int i = 15; i >= 0; i--) {
                int digit = val & 0xF;
                hex[i] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
                val >>= 4;
            }
            hex[16] = '\0';
            // 跳过前导零
            char* p = hex;
            while (*p == '0' && p[1] != '\0') p++;
            early_serial_puts(p);
        }
        early_serial_puts("\n");
    }
    /* 初始化图形显示驱动 */
    init_archvideo();
    
    /* 移动内核镜像到指定地址 */
    move_img2maxpadr(&kabootparam);
    
    return;
}

int strcmpl(const char* a, const char* b) {
    while (*b && *a && (*b == *a)) {
        b++;
        a++;
    }

    return *b - *a;
}

fhdsc_t* get_fileinfo(char_t* fname, abootparam_t* mabp) {
    mlosrddsc_t* mrddadrs = (mlosrddsc_t*)phyadr_to_viradr((adr_t)(mabp->mb_imgpadr + MLOSDSC_OFF));
    if (mrddadrs->mdc_endgic != MDC_ENDGIC ||
        mrddadrs->mdc_rv != MDC_RVGIC ||
        mrddadrs->mdc_fhdnr < 2 ||
        mrddadrs->mdc_filnr < 2) {
        system_error("no mrddsc");
    }
    s64_t rethn = -1;
    fhdsc_t* fhdscstart = (fhdsc_t*)((uint_t)((mrddadrs->mdc_fhdbk_s) + (phyadr_to_viradr((adr_t)mabp->mb_imgpadr))));

    for (u64_t i = 0; i < mrddadrs->mdc_fhdnr; i++) {
        if (strcmpl(fname, fhdscstart[i].fhd_name) == 0) {
            rethn = (s64_t)i;
            goto ok_l;
        }
    }
    rethn = -1;
ok_l:
    if (rethn < 0) {
        system_error("not find file");
    }
    return &fhdscstart[rethn];
}

void get_file_rvadrandsz(char_t* fname, abootparam_t* mabp, u64_t* retadr, u64_t* retsz) {
    u64_t padr = 0, fsz = 0;
    if (NULL == fname || NULL == mabp) {
        *retadr = 0;
        return;
    }
    fhdsc_t* fhdsc = get_fileinfo(fname, mabp);
    if (fhdsc == NULL) {
        *retadr = 0;
        return;
    }
    padr = fhdsc->fhd_intsfsoff + phyadr_to_viradr((adr_t)mabp->mb_imgpadr);
    fsz = fhdsc->fhd_frealsz;

    *retadr = padr;
    *retsz = fsz;
    return;
}

e820map_t* get_maxmappadr_e820map(abootparam_t* mabp, u64_t mappadr) {
    if (NULL == mabp) {
        return NULL;
    }
    u64_t enr = mabp->mb_e820nr;
    e820map_t* emp = (e820map_t*)phyadr_to_viradr((adr_t)mabp->mb_e820padr);
    e820map_t* retemp = NULL;
    u64_t maxadr = emp[0].saddr;
    for (u64_t i = 0; i < enr; i++) {
        if (emp[i].type == RAM_USABLE) {
            if (emp[i].saddr > maxadr && (mappadr > (emp[i].saddr + emp[i].lsize))) {
                maxadr = emp[i].saddr;
                retemp = &emp[i];
            }
        }
    }
    return retemp;
}

e820map_t* ret_kmaxmpadrcmpsz_e820map(abootparam_t* mabp, u64_t mappadr, u64_t cpsz) {
    if (NULL == mabp) {
        return NULL;
    }
    u64_t enr = mabp->mb_e820nr;
    e820map_t* emp = (e820map_t*)phyadr_to_viradr((adr_t)mabp->mb_e820padr);
    e820map_t* retemp = NULL;
    u64_t maxadr = emp[0].saddr;
    for (u64_t i = 0; i < enr; i++) {
        if (emp[i].type == RAM_USABLE) {
            if (emp[i].saddr >= maxadr && (mappadr > (emp[i].saddr + emp[i].lsize)) &&
                (emp[i].lsize >= cpsz)) {
                maxadr = emp[i].saddr;
                retemp = &emp[i];
            }
        }
    }
    if ((mappadr > (retemp->saddr + retemp->lsize)) && (retemp->lsize >= cpsz)) {
        return retemp;
    }
    return NULL;
}

void move_img2maxpadr(abootparam_t* mabp) {
    u64_t kmapadrend = mabp->mb_kpmapphymemsz;
    e820map_t* emp = ret_kmaxmpadrcmpsz_e820map(mabp, kmapadrend, mabp->mb_imgsz);
    if (NULL == emp) {
        system_error("move_img2maxpadr1 emp not ok");
    }

    u64_t imgtoadr = (emp->saddr + (emp->lsize - mabp->mb_imgsz));
    imgtoadr &= ~(0xfffUL);
    if (initchkadr_is_ok(mabp, imgtoadr, mabp->mb_imgsz) != 0) {
        system_error("initchkadr_is_ok not ok\n");
    }
    void* sadr = (void*)phyadr_to_viradr((adr_t)mabp->mb_imgpadr);
    void* dadr = (void*)phyadr_to_viradr((adr_t)imgtoadr);
    if (m2mcopy(sadr, dadr, (sint_t)(mabp->mb_imgsz)) != ((sint_t)(mabp->mb_imgsz))) {
        system_error("move_img2maxpadr1 m2mcopy not ok");
    }
    mabp->mb_imgpadr = imgtoadr;
    return;
}

//函数:move_img2maxpadr中检查相应的地址有问题
int adrzone_is_ok(u64_t sadr, u64_t slen, u64_t kadr, u64_t klen) {
    if (kadr >= sadr && kadr <= (sadr + slen)) {
        return -1;
    }
    if (kadr <= sadr && ((kadr + klen) >= sadr)) {
        return -2;
    }

    return 0;
}

int initchkadr_is_ok(abootparam_t* mabp, u64_t chkadr, u64_t cksz) {
    //u64_t len=chkadr+cksz;
    if (adrzone_is_ok((mabp->mb_krlinitstack - mabp->mb_krlitstacksz), mabp->mb_krlitstacksz, chkadr, cksz) != 0) {
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
    if (adrzone_is_ok(mabp->mb_e820expadr, mabp->mb_e820exsz, chkadr, cksz) != 0) {
        return -8;
    }
    if ((chkadr + cksz) >= mabp->mb_kpmapphymemsz) {
        return -9;
    }
    return 0;
}

#pragma GCC push_options
#pragma GCC optimize("O0")
void die(u32_t dt) {
    u32_t dttt = dt, dtt = dt;
    if (dt == 0) {
        for (;;)
            ;
    }

    for (u32_t i = 0; i < dt; i++) {
        for (u32_t j = 0; j < dtt; j++) {
            for (u32_t k = 0; k < dttt; k++) {
                ;
            }
        }
    }

    return;
}

#pragma GCC pop_options