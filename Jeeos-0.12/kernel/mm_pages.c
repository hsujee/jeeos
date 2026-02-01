/*
 * @Author: Jee Hsu
 * @Description: 内存池和SLAB分配器初始化
 *
 * 初始化内核内存分配基础设施：
 * - 内存池(kmempool_t)管理
 * - SLAB对象分配器初始化
 * - 对象缓存管理
 *
 * 提供的内存分配接口：
 * - kmsob_new(): 分配SLAB对象
 * - kmsob_delete(): 释放SLAB对象
 *
 * SLAB分配器优点：
 * - 减少内存碎片
 * - 快速分配/释放
 * - 对象重用
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

void init_mmpages() {
#if ((defined CFG_X86_PLATFORM))
    kmempool_t_init(&oskmempool);
    // 初始化 SLAB 分配器
    init_kmsob();
    /* 内存池初始化完成 */
#endif
    //testpagemgr();
    return;
}

#if ((defined CFG_X86_PLATFORM))
void msahead_t_init(msahead_t *initp) {
    if (NULL == initp) {
        return;
    }
    initp->mlh_nr = 0;
    list_init(&initp->mlh_msalst);
}

void kmempool_t_init(kmempool_t *initp) {
    spinlock_init(&initp->mp_lock);
    list_init(&initp->mp_list);
    initp->mp_status = 0;
    initp->mp_flags = 0;
    spinlock_init(&initp->mp_pglock);
    spinlock_init(&initp->mp_oblock);
    initp->mp_pgmplnr = 0;
    initp->mp_obmplnr = 0;
    list_init(&initp->mp_pgmplmheadl);
    list_init(&initp->mp_obmplmheadl);
    initp->mp_pgmplmhcach = NULL;
    initp->mp_obmplmhcach = NULL;
#ifdef CFG_X86_PLATFORM
    for (uint_t i = 0; i < PHYMSA_MAX; i++) {
        msahead_t_init(&initp->mp_msalsthead[i]);
    }
#endif
    return;
}

#endif

#ifdef CFG_X86_PLATFORM
void msadsc_add_kmempool(kmempool_t *kmplp, msadsc_t *msa, uint_t relpnr) {
    if (NULL == kmplp || NULL == msa || 1 > relpnr) {
        system_error("msadsc_add_kmempool fail\n");
    }
    cpuflg_t cpuflg;
    spinlock_cli(&kmplp->mp_lock, &cpuflg);
    if ((PHYMSA_MAX - 1) <= relpnr) {
        list_add(&msa->md_list, &kmplp->mp_msalsthead[(PHYMSA_MAX - 1)].mlh_msalst);
        kmplp->mp_msalsthead[(PHYMSA_MAX - 1)].mlh_nr++;
    } else {
        list_add(&msa->md_list, &kmplp->mp_msalsthead[relpnr].mlh_msalst);
        kmplp->mp_msalsthead[relpnr].mlh_nr++;
    }
    spinunlock_sti(&kmplp->mp_lock, &cpuflg);
    return;
}

msadsc_t *msadsc_del_kmempool(kmempool_t *kmplp, uint_t relpnr, adr_t fradr) {
    if (NULL == kmplp || 1 > relpnr || NULL == fradr) {
        return NULL;
    }
    msadsc_t *tmpmsa = NULL, *retmsa = NULL;
    list_h_t *tmplst;
    cpuflg_t cpuflg;
    spinlock_cli(&kmplp->mp_lock, &cpuflg);

    if ((PHYMSA_MAX - 1) <= relpnr) {
        list_for_each(tmplst, &kmplp->mp_msalsthead[(PHYMSA_MAX - 1)].mlh_msalst) {
            tmpmsa = list_entry(tmplst, msadsc_t, md_list);
            if (msadsc_ret_vaddr(tmpmsa) == fradr) {
                list_del(&tmpmsa->md_list);
                retmsa = tmpmsa;
                kmplp->mp_msalsthead[(PHYMSA_MAX - 1)].mlh_nr--;
                goto ret_step;
            }
        }
    } else {
        list_for_each(tmplst, &kmplp->mp_msalsthead[relpnr].mlh_msalst) {
            tmpmsa = list_entry(tmplst, msadsc_t, md_list);
            if (msadsc_ret_vaddr(tmpmsa) == fradr) {
                list_del(&tmpmsa->md_list);
                retmsa = tmpmsa;
                kmplp->mp_msalsthead[relpnr].mlh_nr--;
                goto ret_step;
            }
        }
    }
    retmsa = NULL;
ret_step:
    spinunlock_sti(&kmplp->mp_lock, &cpuflg);
    return retmsa;
}
#endif
#if ((defined CFG_X86_PLATFORM) || (defined CFG_S3C2440A_PLATFORM))
adr_t kmempool_pages_core_new(size_t msize) {
#ifdef CFG_X86_PLATFORM
    kmempool_t *kmplp = &oskmempool;
    uint_t relpnr = 0;
    msadsc_t *retmsa = NULL;
    retmsa = mm_division_pages(&memmgrob, msize >> 12, &relpnr, MA_TYPE_KRNL, DMF_RELDIV);
    if (NULL == retmsa) {
        return NULL;
    }
    msadsc_add_kmempool(kmplp, retmsa, relpnr);
    return msadsc_ret_vaddr(retmsa);
#endif
}

bool_t kmempool_pages_core_delete(adr_t fradr, size_t frsz) {

#ifdef CFG_X86_PLATFORM
    kmempool_t *kmplp = &oskmempool;
    uint_t relpnr = frsz >> PAGE_SZRBIT;
    msadsc_t *retmsa = NULL;
    retmsa = msadsc_del_kmempool(kmplp, relpnr, fradr);
    if (NULL == retmsa) {
        return FALSE;
    }
    return mm_merge_pages(&memmgrob, retmsa, relpnr);
#endif
}

adr_t kmempool_objsz_core_new(size_t msize) {

#ifdef CFG_X86_PLATFORM
    return (adr_t)kmsob_new(msize);
#endif
}

bool_t kmempool_objsz_core_delete(adr_t fradr, size_t frsz) {

#ifdef CFG_X86_PLATFORM
    return kmsob_delete((void *)fradr, frsz);
#endif
}

adr_t kmempool_objsz_new(size_t msize) {
    size_t sz = OBJS_ALIGN(msize);
    if (sz > OBJSORPAGE) {
        return NULL;
    }
    return kmempool_objsz_core_new(sz);
}

bool_t kmempool_objsz_delete(adr_t fradr, size_t frsz) {
    size_t fsz = OBJS_ALIGN(frsz);
    if (fsz > OBJSORPAGE) {
        return FALSE;
    }
    return kmempool_objsz_core_delete(fradr, fsz);
}

adr_t kmempool_pages_new(size_t msize) {
    size_t sz = PAGE_ALIGN(msize);

    return kmempool_pages_core_new(sz);
}

bool_t kmempool_pages_delete(adr_t fradr, size_t frsz) {
    size_t sz = PAGE_ALIGN(frsz);

#ifdef CFG_X86_PLATFORM
    return kmempool_pages_core_delete(fradr, sz);
#endif
}

#endif

adr_t kmempool_onsize_new(size_t msize) {
    if (msize > OBJSORPAGE) {
        return kmempool_pages_new(msize);
    }
    return kmempool_objsz_new(msize);
}

bool_t kmempool_onsize_delete(adr_t fradr, size_t frsz) {
    if (frsz > OBJSORPAGE) {
        return kmempool_pages_delete(fradr, frsz);
    }
    return kmempool_objsz_delete(fradr, frsz);
}

adr_t kmempool_new(size_t msize) {
    if (msize < KMEMPALCSZ_MIN || msize > KMEMPALCSZ_MAX) {
        return NULL;
    }

    return kmempool_onsize_new(msize);
}

bool_t kmempool_delete(adr_t fradr, size_t frsz) {
    if (fradr == NULL || frsz < KMEMPALCSZ_MIN || frsz > KMEMPALCSZ_MAX) {
        return FALSE;
    }
    return kmempool_onsize_delete(fradr, frsz);
}
