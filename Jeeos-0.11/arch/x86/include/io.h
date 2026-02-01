/*
 * @Author: Jee Hsu
 * @Description: I/O操作函数 - 端口访问和底层操作
 *
 * 提供x86平台的I/O操作：
 * - 端口输入/输出 (8/16/32位)
 * - MSR寄存器读写
 * - 内存操作
 * - 中断标志操作
 * - CR寄存器访问
 * - 时间戳和性能计数器
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef ARCH_X86_IO_H
#define ARCH_X86_IO_H

/*===========================================================================
 * 8259 PIC常量定义
 *===========================================================================*/

#define ICW1 0x11           /**< ICW1: 边沿触发、级联、需要ICW4 */
#define ZICW2 0x20          /**< 主8259 ICW2: 中断向量起始 0x20 */
#define SICW2 0x28          /**< 从8259 ICW2: 中断向量起始 0x28 */
#define ZICW3 0x04          /**< 主8259 ICW3: IR2连接从8259 */
#define SICW3 0x02          /**< 从8259 ICW3: 连接到主8259的IR2 */
#define ICW4 0x01           /**< ICW4: 8086模式 */

#define ZIOPT 0x20          /**< 主8259命令端口 */
#define ZIOPT1 0x21         /**< 主8259数据端口 */
#define SIOPT 0xA0          /**< 从8259命令端口 */
#define SIOPT1 0xA1         /**< 从8259数据端口 */

#define _INTM_CTL 0x20      /**< 主8259 I/O端口 */
#define _INTM_CTLMASK 0x21  /**< 主8259屏蔽端口 */
#define _INTS_CTL 0xA0      /**< 从8259 I/O端口 */
#define _INTS_CTLMASK 0xA1  /**< 从8259屏蔽端口 */
#define _EOI 0x20           /**< EOI命令 */

/*===========================================================================
 * 8254定时器常量定义
 *===========================================================================*/

#define PTIPROT1 0x40       /**< 计数器0端口 */
#define PTIPROT2 0x41       /**< 计数器1端口 */
#define PTIPROT3 0x42       /**< 计数器2端口 */
#define PTIPROTM 0x43       /**< 模式控制端口 */

#define TIMEMODE 0x34       /**< 定时器模式: 计数器0, 先低后高, 方波 */
#define TIMEJISU 1194000UL  /**< 定时器输入频率 */
#define HZ 1000UL           /**< 系统时钟频率 */
#define HZLL ((TIMEJISU / HZ) & 0xff)        /**< 计数值低字节 */
#define HZHH (((TIMEJISU / HZ) >> 8) & 0xff) /**< 计数值高字节 */

/*===========================================================================
 * 函数属性宏定义
 *===========================================================================*/

#define NORETURN __attribute__((noreturn))
#define SYSRCALL __attribute__((regparm(3)))
#define HINTCALL __attribute__((regparm(2)))
#define FAUTCALL __attribute__((regparm(2)))
#define REGCALL __attribute__((regparm(3)))

/*===========================================================================
 * 中断标志操作宏
 *===========================================================================*/

/** @brief 保存标志寄存器 */
#define __SAVE_FLAGS(x) __asm__ __volatile__("pushfl ; popl %0" \
                                             : "=g"(x)          \
                                             : /* no input */)

/** @brief 恢复标志寄存器 */
#define __RESTORE_FLAGS(x) __asm__ __volatile__("pushl %0 ; popfl" \
                                                : /* no output */  \
                                                : "g"(x)           \
                                                : "memory")

/** @brief 关中断 */
#define CLI() __asm__ __volatile__("cli" ::: "memory")

/** @brief 开中断 */
#define STI() __asm__ __volatile__("sti" ::: "memory")

/** @brief CPU停机 */
#define HALT() __asm__ __volatile__("hlt" ::: "memory")

/** @brief 开中断并停机 */
#define STI_HALT() __asm__ __volatile__("sti; hlt" ::: "memory")

/** @brief 关中断并停机 */
#define CLI_HALT() __asm__ __volatile__("cli; hlt" ::: "memory")

/*===========================================================================
 * I/O端口操作函数
 *===========================================================================*/

/**
 * @brief 写一个字节到端口
 * @param port 端口号
 * @param val 要写入的值
 */
KLINE void out_u8(const u16_t port, const u8_t val) {
    __asm__ __volatile__("outb  %1, %0\n" : : "dN"(port), "a"(val));
}

/**
 * @brief 写一个字节到端口(带延迟)
 * @param port 端口号
 * @param val 要写入的值
 */
KLINE void out_u8_p(const u16_t port, const u8_t val) {
    __asm__ __volatile__("outb  %1, %0\n\t"
                         "nop \n\t"
                         "nop \n\t" : : "dN"(port), "a"(val));
}

/**
 * @brief 从端口读取一个字节
 * @param port 端口号
 * @return 读取的值
 */
KLINE u8_t in_u8(const u16_t port) {
    u8_t tmp;
    __asm__ __volatile__("inb %1, %0\n" : "=a"(tmp) : "dN"(port));
    return tmp;
}

/**
 * @brief 写一个字(16位)到端口
 * @param port 端口号
 * @param val 要写入的值
 */
KLINE void out_u16(const u16_t port, const u16_t val) {
    __asm__ __volatile__("outw  %1, %0\n" : : "dN"(port), "a"(val));
}

/**
 * @brief 从端口读取一个字(16位)
 * @param port 端口号
 * @return 读取的值
 */
KLINE u16_t in_u16(const u16_t port) {
    u16_t tmp;
    __asm__ __volatile__("inw %1, %0\n" : "=a"(tmp) : "dN"(port));
    return tmp;
}

/**
 * @brief 写一个双字(32位)到端口
 * @param port 端口号
 * @param val 要写入的值
 */
KLINE void out_u32(const u16_t port, const u32_t val) {
    __asm__ __volatile__("outl  %1, %0\n" : : "dN"(port), "a"(val));
}

/**
 * @brief 从端口读取一个双字(32位)
 * @param port 端口号
 * @return 读取的值
 */
KLINE u32_t in_u32(const u16_t port) {
    u32_t tmp;
    __asm__ __volatile__("inl %1, %0\n" : "=a"(tmp) : "dN"(port));
    return tmp;
}

/*===========================================================================
 * MSR寄存器操作函数
 *===========================================================================*/

/**
 * @brief 读取MSR寄存器
 * @param reg MSR寄存器号
 * @return 64位MSR值
 */
KLINE u64_t read_msr(const u32_t reg) {
    u32_t eax, edx;
    __asm__ __volatile__("rdmsr" : "=a"(eax), "=d"(edx) : "c"(reg));
    return (((u64_t)edx) << 32) | (u64_t)eax;
}

/**
 * @brief 写入MSR寄存器
 * @param reg MSR寄存器号
 * @param val 要写入的64位值
 */
KLINE void write_msr(const u32_t reg, const u64_t val) {
    __asm__ __volatile__("wrmsr" : : "a"((u32_t)val), "d"((u32_t)(val >> 32)), "c"(reg));
}

/*===========================================================================
 * 内存操作函数
 *===========================================================================*/

/**
 * @brief 内存拷贝
 * @param src 源地址
 * @param dest 目标地址
 * @param count 拷贝字节数
 */
KLINE void memcopy(void *src, void *dest, uint_t count) {
    u8_t *ss = src, *sd = dest;
    for (uint_t i = 0; i < count; i++) {
        sd[i] = ss[i];
    }
    return;
}

/**
 * @brief 安全内存拷贝(处理重叠)
 * @param sadr 源地址
 * @param dadr 目标地址
 * @param len 拷贝长度
 * @return 实际拷贝字节数
 */
KLINE sint_t m2mcopy(void *sadr, void *dadr, sint_t len) {
    if (NULL == sadr || NULL == dadr || 1 > len) {
        return 0;
    }
    u8_t *s = (u8_t *)sadr, *d = (u8_t *)dadr;
    if (s < d) {
        for (sint_t i = (len - 1); i >= 0; i--) {
            d[i] = s[i];
        }
        return len;
    }
    if (s > d) {
        for (sint_t j = 0; j < len; j++) {
            d[j] = s[j];
        }
        return len;
    }
    if (s == d) {
        return len;
    }
    return 0;
}

/**
 * @brief 内存填充
 * @param s 目标地址
 * @param c 填充值
 * @param count 填充字节数
 */
KLINE void memset(void *s, u8_t c, uint_t count) {
    u8_t *st = s;
    for (uint_t i = 0; i < count; i++) {
        st[i] = c;
    }
    return;
}

/*===========================================================================
 * 中断标志操作函数
 *===========================================================================*/

/**
 * @brief 保存标志并关中断
 * @param flags 保存标志的指针
 */
KLINE void save_flags_cli(cpuflg_t *flags) {
    __asm__ __volatile__(
        "pushfq \t\n"
        "cli    \t\n"
        "popq %0 \t\n"
        : "=m"(*flags) : : "memory");
    return;
}

/**
 * @brief 恢复标志并开中断
 * @param flagsres 保存的标志指针
 */
KLINE void restore_flags_sti(cpuflg_t *flagsres) {
    __asm__ __volatile__(
        "pushq %0 \t\n"
        "popfq \t\n"
        : : "m"(*flagsres) : "memory");
    return;
}

/**
 * @brief 恢复标志并关中断
 * @param flags 保存的标志指针
 */
KLINE void restore_flags_cli(cpuflg_t *flags) {
    __asm__ __volatile__(
        "pushq %0 \t\n"
        "popfq \t\n"
        : "=m"(*flags) : : "memory");
    return;
}

/**
 * @brief 保存标志并开中断
 * @param flags 保存标志的指针
 */
KLINE void save_flags_sti(cpuflg_t *flags) {
    __asm__ __volatile__(
        "pushfq \t\n"
        "popq %0\t\n"
        "sti\t\n"
        : : "m"(*flags) : "memory");
    return;
}

/*===========================================================================
 * 性能计数器和时间戳函数
 *===========================================================================*/

/**
 * @brief 读取性能计数器
 * @param ctrsel 计数器选择
 * @return 计数器值
 */
KLINE u64_t x86_rdpmc(const int ctrsel) {
    u32_t eax, edx;
    __asm__ __volatile__("rdpmc" : "=a"(eax), "=d"(edx) : "c"(ctrsel));
    return (((u64_t)edx) << 32) | (u64_t)eax;
}

/**
 * @brief 读取时间戳计数器
 * @return TSC值
 */
KLINE u64_t x86_rdtsc(void) {
    u32_t leax, ledx;
    __asm__ __volatile__("rdtsc" : "=a"(leax), "=d"(ledx));
    return (((u64_t)ledx) << 32) | (u64_t)leax;
}

/*===========================================================================
 * 位操作函数
 *===========================================================================*/

/**
 * @brief 搜索64位值最高有效位
 * @param val 要搜索的值
 * @return 最高有效位位置(1-64)，0表示未找到
 */
KLINE sint_t search_64rlbits(u64_t val) {
    sint_t retbitnr = -1;
    __asm__ __volatile__("bsrq %1,%q0 \t\n" : "+r"(retbitnr) : "rm"(val));
    return retbitnr + 1;
}

/**
 * @brief 搜索32位值最高有效位
 * @param val 要搜索的值
 * @return 最高有效位位置(1-32)，0表示未找到
 */
KLINE sint_t search_32rlbits(u32_t val) {
    sint_t retbitnr = -1;
    __asm__ __volatile__("bsrl %1,%0 \t\n" : "+r"(retbitnr) : "rm"(val));
    return retbitnr + 1;
}

/*===========================================================================
 * 寄存器访问函数
 *===========================================================================*/

/**
 * @brief 读取ESP寄存器
 * @return ESP值
 */
KLINE u32_t read_kesp() {
    u32_t esp;
    __asm__ __volatile__("movl %%esp,%0" : "=g"(esp) : : "memory");
    return esp;
}

/**
 * @brief 读取RSP寄存器
 * @return RSP值
 */
KLINE u64_t read_rsp() {
    u64_t rsp;
    __asm__ __volatile__("movq %%rsp,%0" : "=g"(rsp) : : "memory");
    return rsp;
}

/**
 * @brief 设置CR3寄存器
 * @param pl4adr 页表基地址
 */
KLINE void set_cr3(u64_t pl4adr) {
    __asm__ __volatile__("movq %0,%%cr3 \n\t" : : "r"(pl4adr) : "memory");
    return;
}

/**
 * @brief 读取CR2寄存器
 * @return CR2值(页错误地址)
 */
KLINE uint_t read_cr2() {
    uint_t regtmp = 0;
    __asm__ __volatile__("movq %%cr2,%0\n\t" : "=r"(regtmp) : : "memory");
    return regtmp;
}

/**
 * @brief 读取CR3寄存器
 * @return CR3值(页表基地址)
 */
KLINE uint_t read_cr3() {
    uint_t regtmp = 0;
    __asm__ __volatile__("movq %%cr3,%0\n\t" : "=r"(regtmp) : : "memory");
    return regtmp;
}

/**
 * @brief 写入CR3寄存器
 * @param r_val 要写入的值
 */
KLINE void write_cr3(uint_t r_val) {
    __asm__ __volatile__("movq %0,%%cr3 \n\t" : : "r"(r_val) : "memory");
    return;
}

#endif // ARCH_X86_IO_H
