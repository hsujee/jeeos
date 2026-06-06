/*
 * @Author: Jee Hsu
 * @Description: buildfile模块头文件
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef BUILDFILE_H
#define BUILDFILE_H
#include "config.h"
#ifdef CFG_X86_PLATFORM

#define BUILD_ARCH_OBJS entry.o startup.o arch_init.o arch_global.o\
                        platform.o video.o cpu_ops.o printk.o arch_mm.o\
                        arch_trap.o trap.o mm_init.o page_alloc.o mem_zone.o\
                        page_desc.o slab_alloc.o i8259.o idt.o mmu.o
#define BUILD_KRNL_OBJS main.o vmem.o global.o mm.o mm_pages.o debug_uart.o\
                        sem.o thread.o time.o wait_list.o\
                        sched.o cpu_idle.o device.o driver_helper.o kernel_error.o interrupt.o io_node.o\
                        shell.o init.o 
#define BUILD_KSYS_OBJS syshandler.o sysmm.o systhread.o sysfork.o sysioctrl.o\
                        sysopen.o sysclose.o sysread.o syswrite.o\
                        syslseek.o 
#define BUILD_MEMY_OBJS
#define BUILD_FSYS_OBJS
#define BUILD_DRIV_OBJS timer.o ramfs.o serial.o nullzero.o speaker.o rtc.o random.o keyboard.o
#define BUILD_LIBS_OBJS libmem.o libio.o libthread.o libtime.o printf.o string.o
#define BUILD_TASK_OBJS

#define BUILD_LINK_OBJS BUILD_ARCH_OBJS\
                        BUILD_KRNL_OBJS\
                        BUILD_KSYS_OBJS\
                        BUILD_MEMY_OBJS\
                        BUILD_FSYS_OBJS\
                        BUILD_DRIV_OBJS\
                        BUILD_LIBS_OBJS\
                        BUILD_TASK_OBJS
#define LINKR_IPUT_FILE BUILD_LINK_OBJS
#define LINKR_OPUT_FILE jeeos.elf
#define KERNL_ELFF_FILE LINKR_OPUT_FILE
#define KERNL_BINF_FILE jeeos.bin

#endif

#endif // BUILDFILE_H
