/*
 * @Author: Jee Hsu
 * @Description: CPU和内存检测
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _CHKCPMM_H
#define _CHKCPMM_H

#include "type.h"
#include "boot_types.h"
#include "setup_c.h"
#include "cpumem_t.h"
#include "vgastr.h"
#include "param.h"
#include "io.h"

void init_chkcpu(abootparam_t* mabp);
int chk_cpuid();
int chk_cpu_longmode();

void init_mem(abootparam_t* mabp);
void init_meme820(abootparam_t* mabp);
void mmap(e820map_t** retemp,u32_t* retemnr);
e820map_t* chk_memsize(e820map_t* e8p,u32_t enr,u64_t sadr,u64_t size);
u64_t get_memsize(e820map_t* e8p,u32_t enr);
void init_chkmm();
void init_stack(abootparam_t* mabp);
void out_char(char* c);
void init_mmpages(abootparam_t *mabp);
void ldr_createpage_and_open();

PUBLIC void init_acpi(abootparam_t *mabp);
unsigned int acpi_get_bios_ebda();


// 主动死机
#define CLI_HALT() __asm__ __volatile__("cli; hlt": : :"memory")

#endif // _CHKCPMM_H
