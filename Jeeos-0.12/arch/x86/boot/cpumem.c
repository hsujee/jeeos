/*
 * @Author: Jee Hsu
 * @Description: CPU和内存检测模块
 *
 * 在二级引导阶段检测CPU和内存：
 * - CPU: 检查CPUID支持、64位长模式支持
 * - 内存: 通过E820 BIOS中断获取内存映射
 * - 页表: 建立初始页表(恒等映射+高半核映射)
 *
 * E820内存类型：
 * - 1: 可用内存(RAM_USABLE)
 * - 2: 保留内存(RAM_RESERV)
 * - 3: ACPI可回收
 * - 4: ACPI NVS
 *
 * 页表结构(4级页表)：
 * - PML4 -> PDPT -> PD -> PT
 * - 使用2MB大页简化映射
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "cpumem.h"

// CPUID 指令来检查 CPU 是否支持 64 位长模式
void init_chkcpu(abootparam_t *mabp) {
    if (!chk_cpuid()) {
        kerror("Your CPU is not support CPUID sys is die!");
        CLI_HALT();
    }

    if (!chk_cpu_longmode()) {
        kerror("Your CPU is not support 64bits mode sys is die!");
        CLI_HALT();
    }

    mabp->mb_cpumode = 0x40; //如果成功则设置机器信息结构的cpu模式为64位
    return;
}

// 检查 CPU 否支持 CPUID 指令
// 通过改写Eflags寄存器的第21位，观察其位的变化判断是否支持CPUID
int chk_cpuid() {
    int rets = 0;
    __asm__ __volatile__(
        "pushfl \n\t"
        "popl %%eax \n\t"
        "movl %%eax,%%ebx \n\t"
        "xorl $0x0200000,%%eax \n\t"
        "pushl %%eax \n\t"
        "popfl \n\t"
        "pushfl \n\t"
        "popl %%eax \n\t"
        "xorl %%ebx,%%eax \n\t"
        "jz 1f \n\t"
        "movl $1,%0 \n\t"
        "jmp 2f \n\t"
        "1: movl $0,%0 \n\t"
        "2: \n\t"
        : "=c"(rets)
        :
        :);
    return rets;
}

// 用 CPUID 指令检查 CPU 是否支持 64 位长模式
int chk_cpu_longmode() {
    int rets = 0;
    __asm__ __volatile__(
        "movl $0x80000000,%%eax \n\t"
        "cpuid \n\t" //把eax中放入0x80000000调用CPUID指令
        "cmpl $0x80000001,%%eax \n\t" //看eax中返回结果
        "setnb %%al \n\t" //不为0x80000001,则不支持0x80000001号功能
        "jb 1f \n\t"
        "movl $0x80000001,%%eax \n\t"
        "cpuid \n\t" //把eax中放入0x800000001调用CPUID指令，检查edx中的返回数据
        "bt $29,%%edx  \n\t" //长模式 支持位 是否为1
        "setcb %%al \n\t"
        "1: \n\t"
        "movzx %%al,%%eax \n\t"
        : "=a"(rets)
        :
        :);
    return rets;
}

void init_mem(abootparam_t *mabp) {
    // e820map_t结构指针，即首地址
    e820map_t *retemp;
    // e820map数组元素个数
    u32_t retemnr = 0;
    mabp->mb_ebdaphyadr = acpi_get_bios_ebda();
    mmap(&retemp, &retemnr);
    if (retemnr == 0) {
        kerror("no e820map\n");
    }
    if (chk_memsize(retemp, retemnr, 0x100000, 0x8000000) == NULL) {
        kerror("Your computer is low on memory, the memory cannot be less than 128MB!");
    }
    mabp->mb_e820padr = (u64_t)((u32_t)(retemp));
    mabp->mb_e820nr = (u64_t)retemnr;
    mabp->mb_e820sz = retemnr * (sizeof(e820map_t));
    mabp->mb_memsz = get_memsize(retemp, retemnr);
    init_acpi(mabp);
    return;
}

void init_meme820(abootparam_t *mabp) {
    e820map_t *semp = (e820map_t *)((u32_t)(mabp->mb_e820padr));
    u64_t senr = mabp->mb_e820nr;
    e820map_t *demp = (e820map_t *)((u32_t)(mabp->mb_nextwtpadr));
    // 申请内存
    if (1 > move_krlimg(mabp, (u64_t)((u32_t)demp), (senr * (sizeof(e820map_t))))) {
        kerror("move_krlimg err");
    }

    // 拷贝e820map_t到jeeos.eki之后的地址，并修正mabp指向新地址
    m2mcopy(semp, demp, (sint_t)(senr * (sizeof(e820map_t))));
    mabp->mb_e820padr = (u64_t)((u32_t)(demp));
    mabp->mb_e820sz = senr * (sizeof(e820map_t));
    mabp->mb_nextwtpadr = P4K_ALIGN((u32_t)(demp) + (u32_t)(senr * (sizeof(e820map_t))));
    mabp->mb_kalldendpadr = mabp->mb_e820padr + mabp->mb_e820sz;
    return;
}

// 调用 BIOS 中断，就能获取 e820map 结构数组
void mmap(e820map_t **retemp, u32_t *retemnr) {
    realadr_call_entry(RLINTNR(0), 0, 0);
    *retemnr = *((u32_t *)(E80MAP_NR));
    *retemp = (e820map_t *)(*((u32_t *)(E80MAP_ADRADR)));
    return;
}

e820map_t *chk_memsize(e820map_t *e8p, u32_t enr, u64_t sadr, u64_t size) {
    u64_t len = sadr + size;
    if (enr == 0 || e8p == NULL) {
        return NULL;
    }
    for (u32_t i = 0; i < enr; i++) {
        if (e8p[i].type == RAM_USABLE) {
            if ((sadr >= e8p[i].saddr) && (len < (e8p[i].saddr + e8p[i].lsize))) {
                return &e8p[i];
            }
        }
    }
    return NULL;
}

u64_t get_memsize(e820map_t *e8p, u32_t enr) {
    u64_t len = 0;
    if (enr == 0 || e8p == NULL) {
        return 0;
    }
    for (u32_t i = 0; i < enr; i++) {
        if (e8p[i].type == RAM_USABLE) {
            len += e8p[i].lsize;
        }
    }
    return len;
}

void init_chkmm() {

    e820map_t *map = (e820map_t *)EMAP_PTR;
    u16_t *map_nr = (u16_t *)EMAP_NR_PTR;
    u64_t mmsz = 0;

    for (int j = 0; j < (*map_nr); j++) {
        if (map->type == RAM_USABLE) {
            mmsz += map->lsize;
        }
        map++;
    }

    if (mmsz < BASE_MEM_SZ) {
        printk("Your computer is low on memory, the memory cannot be less than 64MB!");
        CLI_HALT();
    }

    if (!chk_cpuid()) {
        printk("Your CPU is not support CPUID sys is die!");
        CLI_HALT();
    }

    if (!chk_cpu_longmode()) {
        printk("Your CPU is not support 64bits mode sys is die!");
        CLI_HALT();
    }
    ldr_createpage_and_open();
    //for(;;);
    return;
}

unsigned int acpi_get_bios_ebda() {
    unsigned int address = *(unsigned short *)0x40E;
    address <<= 4;
    return address;
}

int acpi_checksum(unsigned char *ap, s32_t len) {
    int sum = 0;
    while (len--) {
        sum += *ap++;
    }
    return sum & 0xFF;
}

mrsdp_t *acpi_rsdp_isok(mrsdp_t *rdp) {
    // ACPI 1.0: rp_revn = 0, 只有20字节的基本结构
    // ACPI 2.0+: rp_revn >= 2, 有完整的36字节结构
    
    // 先验证基本结构的 checksum (前20字节)
    if (0 != acpi_checksum((unsigned char *)rdp, 20)) {
        return NULL;
    }
    
    // ACPI 2.0+ 需要额外验证扩展 checksum
    if (rdp->rp_revn >= 2 && rdp->rp_len > 0) {
        if (0 != acpi_checksum((unsigned char *)rdp, (s32_t)rdp->rp_len)) {
            return NULL;
        }
    }
    
    return rdp;
}

mrsdp_t *findacpi_rsdp_core(void *findstart, u32_t findlen) {
    if (NULL == findstart || 1024 > findlen) {
        return NULL;
    }

    u8_t *tmpdp = (u8_t *)findstart;

    mrsdp_t *retdrp = NULL;
    for (u64_t i = 0; i <= findlen; i++) {

        if (('R' == tmpdp[i]) && ('S' == tmpdp[i + 1]) && ('D' == tmpdp[i + 2]) && (' ' == tmpdp[i + 3]) &&
            ('P' == tmpdp[i + 4]) && ('T' == tmpdp[i + 5]) && ('R' == tmpdp[i + 6]) && (' ' == tmpdp[i + 7])) {
            retdrp = acpi_rsdp_isok((mrsdp_t *)(&tmpdp[i]));
            if (NULL != retdrp) {
                return retdrp;
            }
        }
    }
    return NULL;
}

PUBLIC mrsdp_t *find_acpi_rsdp() {
    void *fndp = (void *)acpi_get_bios_ebda();
    mrsdp_t *rdp = findacpi_rsdp_core(fndp, 1024);
    if (NULL != rdp) {
        return rdp;
    }
    //0E0000h和0FFFFFH
    fndp = (void *)(0xe0000);
    rdp = findacpi_rsdp_core(fndp, (0xfffff - 0xe0000));
    if (NULL != rdp) {
        return rdp;
    }
    return NULL;
}

PUBLIC void init_acpi(abootparam_t *mabp) {
    mrsdp_t *rdp = NULL;
    rdp = find_acpi_rsdp();
    if (NULL == rdp) {
        kerror("Your computer is not support ACPI!!");
    }
    m2mcopy(rdp, &mabp->mb_mrsdp, (sint_t)((sizeof(mrsdp_t))));
    if (acpi_rsdp_isok(&mabp->mb_mrsdp) == NULL) {
        kerror("Your computer is not support ACPI!!");
    }
    return;
}

//初始化内核栈
void init_stack(abootparam_t *mabp) {
    // 0x8f000 ～（0x8f000+0x1001）
    if (1 > move_krlimg(mabp, (u64_t)(0x8f000), 0x1001)) {
        kerror("iks_moveimg err");
    }
    mabp->mb_krlinitstack = IKSTACK_PHYADR; // 栈顶地址
    mabp->mb_krlitstacksz = IKSTACK_SIZE; // 栈大小是4KB
    return;
}

// 建立 MMU 页表
// 为了简化编程，使用长模式下的 2MB 分页方式
void init_mmpages(abootparam_t *mabp) {
    // 顶级页目录
    u64_t *p = (u64_t *)(KINITPAGE_PHYADR);
    // 页目录指针
    u64_t *pdpte = (u64_t *)(KINITPAGE_PHYADR + 0x1000);
    // 页目录
    u64_t *pde = (u64_t *)(KINITPAGE_PHYADR + 0x2000);
    // 物理地址从0开始
    u64_t adr = 0;
    
    if (1 > move_krlimg(mabp, (u64_t)(KINITPAGE_PHYADR), (0x1000 * 16 + 0x2000))) {
        kerror("move_krlimg err");
    }

    //将顶级页目录、页目录指针的空间清0
    for (uint_t mi = 0; mi < PGENTY_SIZE; mi++) {
        p[mi] = 0;
        pdpte[mi] = 0;
    }
    // 映射
    for (uint_t pdei = 0; pdei < 16; pdei++) {
        pdpte[pdei] = (u64_t)((u32_t)pde | KPDPTE_RW | KPDPTE_P);
        for (uint_t pdeii = 0; pdeii < PGENTY_SIZE; pdeii++) {
            // 大页KPDE_PS 2MB，可读写KPDE_RW，存在KPDE_P
            pde[pdeii] = 0 | adr | KPDE_PS | KPDE_RW | KPDE_P;
            adr += 0x200000;
        }
        pde = (u64_t *)((u32_t)pde + 0x1000);
    }
    // 让顶级页目录中第0项和第((KRNL_VIRTUAL_ADDRESS_START) >> KPML4_SHIFT) & 0x1ff项，指向同一个页目录指针页
    p[((KRNL_VIRTUAL_ADDRESS_START) >> KPML4_SHIFT) & 0x1ff] = (u64_t)((u32_t)pdpte | KPML4_RW | KPML4_P);
    p[0] = (u64_t)((u32_t)pdpte | KPML4_RW | KPML4_P);
    // 把页表首地址保存在机器信息结构中
    mabp->mb_pml4padr = (u64_t)(KINITPAGE_PHYADR);
    mabp->mb_subpageslen = (u64_t)(0x1000 * 16 + 0x2000);
    mabp->mb_kpmapphymemsz = (u64_t)(0x400000000);
    return;
}

void out_char(char *c) {
    char *str = c, *p = (char *)0xb8000;

    while (*str) {
        *p = *str;
        p += 2;
        str++;
    }

    return;
}

void init_mmpagesold(abootparam_t *mabp) {

    if (1 > move_krlimg(mabp, (u64_t)(PML4T_BADR), 0x3000)) {
        kerror("ip_moveimg err");
    }

    pt64_t *pml4p = (pt64_t *)PML4T_BADR, *pdptp = (pt64_t *)PDPTE_BADR, *pdep = (pt64_t *)PDE_BADR; //*ptep=(pt64_t*)PTE_BADR;
    for (int pi = 0; pi < PG_SIZE; pi++) {
        pml4p[pi] = 0;
        pdptp[pi] = 0;
        pdep[pi] = 0;
    }

    pml4p[0] = 0 | PDPTE_BADR | PDT_S_RW | PDT_S_PNT;
    pdptp[0] = 0 | PDE_BADR | PDT_S_RW | PDT_S_PNT;
    pml4p[256] = 0 | PDPTE_BADR | PDT_S_RW | PDT_S_PNT;

    pt64_t tmpba = 0, tmpbd = 0 | PDT_S_SIZE | PDT_S_RW | PDT_S_PNT;

    for (int di = 0; di < PG_SIZE; di++) {
        pdep[di] = tmpbd;
        tmpba += 0x200000;
        tmpbd = tmpba | PDT_S_SIZE | PDT_S_RW | PDT_S_PNT;
    }
    mabp->mb_pml4padr = (u64_t)((u32_t)pml4p);
    mabp->mb_subpageslen = 0x3000;
    mabp->mb_kpmapphymemsz = (0x200000 * 512);
    return;
}

void ldr_createpage_and_open() {
    pt64_t *pml4p = (pt64_t *)PML4T_BADR, *pdptp = (pt64_t *)PDPTE_BADR, *pdep = (pt64_t *)PDE_BADR; 
    for (int pi = 0; pi < PG_SIZE; pi++) {
        pml4p[pi] = 0;
        pdptp[pi] = 0;
        pdep[pi] = 0;
    
    }

    pml4p[0] = 0 | PDPTE_BADR | PDT_S_RW | PDT_S_PNT;
    pdptp[0] = 0 | PDE_BADR | PDT_S_RW | PDT_S_PNT;
    pml4p[256] = 0 | PDPTE_BADR | PDT_S_RW | PDT_S_PNT;

    pt64_t tmpba = 0, tmpbd = 0 | PDT_S_SIZE | PDT_S_RW | PDT_S_PNT;

    for (int di = 0; di < PG_SIZE; di++) {
        pdep[di] = tmpbd;
        tmpba += 0x200000;
        tmpbd = tmpba | PDT_S_SIZE | PDT_S_RW | PDT_S_PNT;
    }
    return;
}
