/*
 * @Author: Jee Hsu
 * @Description: 内存管理
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_MM_INIT_T_H
#define ARCH_X86_MM_INIT_T_H

typedef struct s_MEMMGROB {
	list_h_t mo_list;
	spinlock_t mo_lock;           //保护自身自旋锁
	uint_t mo_status;               //状态
	uint_t mo_flags;              //标志
	sem_t mo_sem;
	u64_t mo_memsz;               //内存大小
	u64_t mo_maxpages;            //内存最大页面数
	u64_t mo_freepages;           //内存最大空闲页面数
	u64_t mo_alocpages;           //内存最大分配页面数
	u64_t mo_resvpages;           //内存保留页面数
	u64_t mo_horizline;           //内存分配水位线
	phymmarge_t* mo_pmagestat;    //内存空间布局结构指针
	u64_t mo_pmagenr;
	msadsc_t* mo_msadscstat;      //内存页面结构指针
	u64_t mo_msanr;
	mmarea_t* mo_mareastat;       //内存区结构指针
	u64_t mo_mareanr;
	kmsobmgrhed_t mo_kmsobmgr;    //管理kmsob_t结构的数据结构
	void* mo_privp;
	void* mo_extp;
} memmgrob_t;

#endif // ARCH_X86_MM_INIT_T_H