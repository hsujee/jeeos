/*
 * @Author: Jee Hsu
 * @Description: 物理页描述符(msadsc_t)管理
 *
 * 每个物理页(4KB)对应一个页描述符msadsc_t：
 * - md_list: 链表节点(挂载到空闲/已分配链表)
 * - md_lock: 自旋锁保护
 * - md_indxflags: 页面索引和标志
 * - md_phyadrs: 物理地址和状态标志
 *
 * 页面标志(paf_*)：
 * - paf_alloc: 是否已分配
 * - paf_shared: 是否共享
 * - paf_dirty: 是否脏页
 * - paf_lock: 是否锁定
 *
 * 页面状态(mf_mocty)：
 * - MF_MOCTY_FREE: 空闲
 * - MF_MOCTY_KRNL: 内核使用
 * - MF_MOCTY_PROC: 进程使用
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

void msadsc_init(msadsc_t *initp) {
	list_init(&initp->md_list);
	spinlock_init(&initp->md_lock);
	initp->md_indxflags.mf_olkty = MF_OLKTY_INIT;
	initp->md_indxflags.mf_lstty = MF_LSTTY_LIST;
	initp->md_indxflags.mf_mocty = MF_MOCTY_FREE;
	initp->md_indxflags.mf_marty = MF_MARTY_INIT;
	initp->md_indxflags.mf_uindx = MF_UINDX_INIT;
	initp->md_phyadrs.paf_alloc = PAF_NO_ALLOC;
	initp->md_phyadrs.paf_shared = PAF_NO_SHARED;
	initp->md_phyadrs.paf_swap = PAF_NO_SWAP;
	initp->md_phyadrs.paf_cache = PAF_NO_CACHE;
	initp->md_phyadrs.paf_kmap = PAF_NO_KMAP;
	initp->md_phyadrs.paf_lock = PAF_NO_LOCK;
	initp->md_phyadrs.paf_dirty = PAF_NO_DIRTY;
	initp->md_phyadrs.paf_busy = PAF_NO_BUSY;
	initp->md_phyadrs.paf_rv2 = PAF_RV2_VAL;
	initp->md_phyadrs.paf_padrs = PAF_INIT_PADRS;
	initp->md_odlink = NULL;
	return;
}

void disp_one_msadsc(msadsc_t *mp) {
	printk("msadsc_t.md_f:_ux[%x],_my[%x],md_phyadrs:_alc[%x],_shd[%x],_swp[%x],_che[%x],_kmp[%x],_lck[%x],_dty[%x],_bsy[%x],_padrs[%x]\n",
		   (uint_t)mp->md_indxflags.mf_uindx, (uint_t)mp->md_indxflags.mf_mocty, (uint_t)mp->md_phyadrs.paf_alloc, (uint_t)mp->md_phyadrs.paf_shared, (uint_t)mp->md_phyadrs.paf_swap, (uint_t)mp->md_phyadrs.paf_cache, (uint_t)mp->md_phyadrs.paf_kmap, (uint_t)mp->md_phyadrs.paf_lock,
		   (uint_t)mp->md_phyadrs.paf_dirty, (uint_t)mp->md_phyadrs.paf_busy, (uint_t)(mp->md_phyadrs.paf_padrs << 12));
	return;
}

bool_t ret_msadsc_vadrandsz(abootparam_t *mabp, msadsc_t **retmasvp, u64_t *retmasnr) {
	if (NULL == mabp || NULL == retmasvp || NULL == retmasnr) {
		return FALSE;
	}
	if (mabp->mb_e820exnr < 1 || NULL == mabp->mb_e820expadr || (mabp->mb_e820exnr * sizeof(phymmarge_t)) != mabp->mb_e820exsz) {
		*retmasvp = NULL;
		*retmasnr = 0;
		return FALSE;
	}
	phymmarge_t *pmagep = (phymmarge_t *)phyadr_to_viradr((adr_t)mabp->mb_e820expadr);
	u64_t usrmemsz = 0, msadnr = 0;
	for (u64_t i = 0; i < mabp->mb_e820exnr; i++) {
		if (PMR_T_OSAPUSERRAM == pmagep[i].pmr_type) {
			usrmemsz += pmagep[i].pmr_size;
			msadnr += (pmagep[i].pmr_size >> 12);
		}
	}
	if (0 == usrmemsz || (usrmemsz >> 12) < 1 || msadnr < 1) {
		*retmasvp = NULL;
		*retmasnr = 0;
		return FALSE;
	}
	//msadnr=usrmemsz>>12;
	if (0 != initchkadr_is_ok(mabp, mabp->mb_nextwtpadr, (msadnr * sizeof(msadsc_t)))) {
		system_error("ret_msadsc_vadrandsz initchkadr_is_ok err\n");
	}

	*retmasvp = (msadsc_t *)phyadr_to_viradr((adr_t)mabp->mb_nextwtpadr);
	*retmasnr = msadnr;
	return TRUE;
}

void write_one_msadsc(msadsc_t *msap, u64_t phyadr) {
	//对msadsc_t结构做基本的初始化，比如链表、锁、标志位
	msadsc_init(msap);
	//这是把一个64位的变量地址转换成phyadrflgs_t*类型方便取得其中的地址位段
	phyadrflgs_t *tmp = (phyadrflgs_t *)(&phyadr);
	//把页的物理地址写入到msadsc_t结构中
	msap->md_phyadrs.paf_padrs = tmp->paf_padrs;
	return;
}

u64_t init_msadsc_core(abootparam_t *mabp, msadsc_t *msavstart, u64_t msanr) {
	//获取phymmarge_t结构数组开始地址
	phymmarge_t *pmagep = (phymmarge_t *)phyadr_to_viradr((adr_t)mabp->mb_e820expadr);
	u64_t mdindx = 0;
	//扫描phymmarge_t结构数组
	for (u64_t i = 0; i < mabp->mb_e820exnr; i++) {
		//判断phymmarge_t结构的类型是不是可用内存
		if (PMR_T_OSAPUSERRAM == pmagep[i].pmr_type) {
			//遍历phymmarge_t结构的地址区间
			for (u64_t start = pmagep[i].pmr_start; start < pmagep[i].pmr_end; start += 4096) {
				//每次加上4KB-1比较是否小于等于phymmarge_t结构的结束地址
				if ((start + 4096 - 1) <= pmagep[i].pmr_end) {
					//与当前地址为参数写入第mdindx个msadsc结构
					write_one_msadsc(&msavstart[mdindx], start);
					mdindx++;
				}
			}
		}
	}

	return mdindx;
}

void init_msadsc() {
	u64_t coremdnr = 0, msadscnr = 0;
	msadsc_t *msadscvp = NULL;
	abootparam_t *mabp = &kabootparam;
	//计算msadsc_t结构数组的开始地址和数组元素个数
	if (ret_msadsc_vadrandsz(mabp, &msadscvp, &msadscnr) == FALSE) {
		system_error("init_msadsc ret_msadsc_vadrandsz err\n");
	}
	//开始真正初始化msadsc_t结构数组
	coremdnr = init_msadsc_core(mabp, msadscvp, msadscnr);
	if (coremdnr != msadscnr) {
		system_error("init_msadsc init_msadsc_core err\n");
	}
	//将msadsc_t结构数组的开始的物理地址写入kabootparam结构中
	mabp->mb_memmappadr = viradr_to_phyadr((adr_t)msadscvp);
	//将msadsc_t结构数组的元素个数写入kabootparam结构中
	mabp->mb_memmapnr = coremdnr;
	//将msadsc_t结构数组的大小写入kabootparam结构中
	mabp->mb_memmapsz = coremdnr * sizeof(msadsc_t);
	//计算下一个空闲内存的开始地址
	mabp->mb_nextwtpadr = PAGE_ALIGN(mabp->mb_memmappadr + mabp->mb_memmapsz);
	return;
}

void disp_phymsadsc() {
	u64_t coremdnr = 0;
	msadsc_t *msadscvp = NULL;
	abootparam_t *mabp = &kabootparam;

	msadscvp = (msadsc_t *)phyadr_to_viradr((adr_t)mabp->mb_memmappadr);
	coremdnr = mabp->mb_memmapnr;

	for (int i = 0; i < 10; ++i) {
		disp_one_msadsc(&msadscvp[i]);
	}

	for (u64_t i = coremdnr / 2; i < coremdnr / 2 + 10; ++i) {
		disp_one_msadsc(&msadscvp[i]);
	}

	for (u64_t i = coremdnr - 10; i < coremdnr; ++i) {
		disp_one_msadsc(&msadscvp[i]);
	}
	return;
}

//搜索一段内存地址空间所对应的msadsc_t结构
u64_t search_segment_occupymsadsc(msadsc_t *msastart, u64_t msanr, u64_t ocpystat, u64_t ocpyend) {
	u64_t mphyadr = 0, fsmsnr = 0;
	msadsc_t *fstatmp = NULL;
	for (u64_t mnr = 0; mnr < msanr; mnr++) {
		if ((msastart[mnr].md_phyadrs.paf_padrs << PSHRSIZE) == ocpystat) {
			//找出开始地址对应的第一个msadsc_t结构，就跳转到step1
			fstatmp = &msastart[mnr];
			goto step1;
		}
	}
step1:
	fsmsnr = 0;
	if (NULL == fstatmp) {
		return 0;
	}
	for (u64_t tmpadr = ocpystat; tmpadr < ocpyend; tmpadr += PAGESIZE, fsmsnr++) {
		//从开始地址对应的第一个msadsc_t结构开始设置，直到结束地址对应的最后一个masdsc_t结构
		mphyadr = fstatmp[fsmsnr].md_phyadrs.paf_padrs << PSHRSIZE;
		if (mphyadr != tmpadr) {
			return 0;
		}
		if (MF_MOCTY_FREE != fstatmp[fsmsnr].md_indxflags.mf_mocty ||
			0 != fstatmp[fsmsnr].md_indxflags.mf_uindx ||
			PAF_NO_ALLOC != fstatmp[fsmsnr].md_phyadrs.paf_alloc) {
			return 0;
		}
		//设置msadsc_t结构为已经分配，已经分配给内核
		fstatmp[fsmsnr].md_indxflags.mf_mocty = MF_MOCTY_KRNL;
		fstatmp[fsmsnr].md_indxflags.mf_uindx++;
		fstatmp[fsmsnr].md_phyadrs.paf_alloc = PAF_ALLOC;
	}
	//进行一些数据的正确性检查
	u64_t ocpysz = ocpyend - ocpystat;
	if ((ocpysz & 0xfff) != 0) {
		if (((ocpysz >> PSHRSIZE) + 1) != fsmsnr) {
			return 0;
		}
		return fsmsnr;
	}
	if ((ocpysz >> PSHRSIZE) != fsmsnr) {
		return 0;
	}
	return fsmsnr;
}

void test_schkrloccuymm(abootparam_t *mabp, u64_t ocpystat, u64_t sz) {
	msadsc_t *msadstat = (msadsc_t *)phyadr_to_viradr((adr_t)mabp->mb_memmappadr);
	u64_t msanr = mabp->mb_memmapnr;
	u64_t chkmnr = 0;
	u64_t chkadr = ocpystat;
	if ((sz & 0xfff) != 0) {
		chkmnr = (sz >> PSHRSIZE) + 1;
	} else {
		chkmnr = sz >> PSHRSIZE;
	}
	msadsc_t *fstatmp = NULL;
	for (u64_t mnr = 0; mnr < msanr; mnr++) {
		if ((msadstat[mnr].md_phyadrs.paf_padrs << PSHRSIZE) == ocpystat) {
			fstatmp = &msadstat[mnr];
			goto step1;
		}
	}
step1:
	if (fstatmp == NULL) {
		system_error("fstatmp NULL\n");
	}

	for (u64_t i = 0; i < chkmnr; i++, chkadr += PAGESIZE) {
		if (chkadr != fstatmp[i].md_phyadrs.paf_padrs << PSHRSIZE) {
			system_error("chkadr != err\n");
		}
		if (PAF_ALLOC != fstatmp[i].md_phyadrs.paf_alloc) {
			system_error("PAF_ALLOC err\n");
		}
		if (1 != fstatmp[i].md_indxflags.mf_uindx) {
			system_error("mf_uindx err\n");
		}
		if (MF_MOCTY_KRNL != fstatmp[i].md_indxflags.mf_mocty) {
			system_error("mf_olkty err\n");
		}
	}
	if (chkadr != (ocpystat + (chkmnr * PAGESIZE))) {
		system_error("test_schkrloccuymm err\n");
	}
	return;
}

bool_t search_krloccupymsadsc_core(abootparam_t *mabp) {
	u64_t retschmnr = 0;
	msadsc_t *msadstat = (msadsc_t *)phyadr_to_viradr((adr_t)mabp->mb_memmappadr);
	u64_t msanr = mabp->mb_memmapnr;
	//搜索BIOS中断表占用的内存页所对应msadsc_t结构
	retschmnr = search_segment_occupymsadsc(msadstat, msanr, 0, 0x1000);
	if (0 == retschmnr) {
		return FALSE;
	}
	//搜索内核栈占用的内存页所对应msadsc_t结构
	retschmnr = search_segment_occupymsadsc(msadstat, msanr, mabp->mb_krlinitstack & (~(0xfffUL)), mabp->mb_krlinitstack);
	if (0 == retschmnr) {
		return FALSE;
	}
	//搜索内核占用的内存页所对应msadsc_t结构
	retschmnr = search_segment_occupymsadsc(msadstat, msanr, mabp->mb_krlimgpadr, mabp->mb_nextwtpadr);
	if (0 == retschmnr) {
		return FALSE;
	}
	//搜索内核映像文件占用的内存页所对应msadsc_t结构
	retschmnr = search_segment_occupymsadsc(msadstat, msanr, mabp->mb_imgpadr, mabp->mb_imgpadr + mabp->mb_imgsz);
	if (0 == retschmnr) {
		return FALSE;
	}
	return TRUE;
}

//初始化搜索内核占用的内存页面
void init_search_krloccupymm(abootparam_t *mabp) {
	//实际初始化搜索内核占用的内存页面
	if (search_krloccupymsadsc_core(mabp) == FALSE) {
		system_error("search_krloccupymsadsc_core fail\n");
	}
	return;
}
