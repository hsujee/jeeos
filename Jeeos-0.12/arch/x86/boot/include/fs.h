/*
 * @Author: Jee Hsu
 * @Description: 文件系统模块
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _FS_H
#define _FS_H

#include "type.h"
#include "boot_types.h"
#include "io.h"
#include "bootparam_c.h"
#include "param.h"

int strcmpl(const char *a,const char *b);
int move_krlimg(abootparam_t* mabp,u64_t cpyadr,u64_t cpysz);
void init_kernel_img(abootparam_t* mabp);
void init_defutfont(abootparam_t *mabp);
fhdsc_t* get_fileinfo(char_t* fname,abootparam_t* mabp);
void get_file_rpadrandsz(char_t *fname, abootparam_t* mabp,u32_t* retadr,u32_t* retsz);
u64_t get_filesz(char_t* filenm,abootparam_t* mabp);
u64_t get_wt_imgfilesz(abootparam_t* mabp);
u64_t r_file_to_padr(abootparam_t* mabp,u32_t f2adr,char_t* fnm);
void set_rwhdhdpack(rwhdpach_t* setp,u8_t sn,u16_t off,u16_t seg,u32_t lbal,u32_t lbah);
void* read_sector(u64_t sector);
void ret_mbr();
int is_part(dpt_t* isp);
u64_t ret_imgfilesz();
#endif // FS_H
