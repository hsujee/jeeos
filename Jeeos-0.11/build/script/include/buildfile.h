/*
 * @Author: Jee Hsu
 * @Description: 构建配置头文件
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef BUILDFILE_H
#define BUILDFILE_H
#include "config.h"
#ifdef CFG_X86_PLATFORM

#define BUILD_ARCH_OBJS entry.o startup.o arch_init.o arch_global.o\
                        platform.o video.o cpu_ops.o printk.o arch_mm.o\
                        arch_trap.o trap.o mm_init.o i8259.o idt.o\
                        page_desc.o slab_alloc.o page_alloc.o mmu.o mem_zone.o

#define BUILD_KRNL_OBJS main.o global.o sem.o wait_list.o\
                        thread.o sched.o mm.o mm_pages.o time.o cpu_idle.o\
                        interrupt.o vmem.o device.o io_node.o

#define BUILD_SYST_OBJS syshandler.o sysopen.o sysclose.o sysread.o\
                        syswrite.o syslseek.o sysioctrl.o sysmm.o\
                        systhread.o

#define BUILD_LINK_OBJS BUILD_ARCH_OBJS\
                        BUILD_KRNL_OBJS\
                        BUILD_SYST_OBJS

#define LINKR_IPUT_FILE BUILD_LINK_OBJS
#define LINKR_OPUT_FILE jeeos.elf
#define KERNL_ELFF_FILE LINKR_OPUT_FILE
#define KERNL_BINF_FILE jeeos.bin

#endif

#endif // BUILDFILE_H
