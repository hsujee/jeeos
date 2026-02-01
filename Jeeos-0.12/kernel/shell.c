/*
 * @Author: Jee Hsu
 * @Description: Jeeos Shell 命令行解释器
 *
 * 实现基本的命令行交互：
 * - 从键盘读取输入
 * - 解析并执行内置命令
 * - 支持基本编辑(退格)
 *
 * 内置命令：
 * - help: 显示帮助信息
 * - clear: 清屏
 * - time: 显示系统时间
 * - mem: 显示内存信息
 * - ps: 显示进程列表
 * - echo: 回显字符串
 * - reboot: 重启系统
 * - version: 显示版本信息
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

/*===========================================================================
 * 内联字符串函数
 *===========================================================================*/

PRIVATE int shell_strcmp(const char *s1, const char *s2) {
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

PRIVATE char *shell_strchr(const char *s, int c) {
    char ch = (char)c;
    while (*s) {
        if (*s == ch) return (char *)s;
        s++;
    }
    return NULL;
}

/* 简单的 strtok 实现 */
PRIVATE char *shell_strtok_last = NULL;

PRIVATE char *shell_strtok(char *str, const char *delim) {
    char *start;
    
    if (str != NULL) {
        shell_strtok_last = str;
    }
    
    if (shell_strtok_last == NULL) {
        return NULL;
    }
    
    /* 跳过前导分隔符 */
    while (*shell_strtok_last && shell_strchr(delim, *shell_strtok_last)) {
        shell_strtok_last++;
    }
    
    if (*shell_strtok_last == '\0') {
        shell_strtok_last = NULL;
        return NULL;
    }
    
    start = shell_strtok_last;
    
    /* 找到下一个分隔符 */
    while (*shell_strtok_last && !shell_strchr(delim, *shell_strtok_last)) {
        shell_strtok_last++;
    }
    
    if (*shell_strtok_last) {
        *shell_strtok_last++ = '\0';
    } else {
        shell_strtok_last = NULL;
    }
    
    return start;
}

/*===========================================================================
 * Shell 状态
 *===========================================================================*/

PRIVATE char shell_cmdline[SHELL_CMDLINE_MAX];
PRIVATE uint_t shell_cmdpos = 0;

/* 键盘设备缓存 */
PRIVATE device_t *kb_device = NULL;

/*===========================================================================
 * 输入输出函数
 *===========================================================================*/

/**
 * @brief 从键盘读取一个字符
 */
PRIVATE int shell_getchar(void) {
    if (kb_device == NULL) {
        return -1;
    }
    
    objnode_t node;
    objnode_t_init(&node);
    node.on_objadr = kb_device;
    node.on_opercode = IOIF_CODE_READ;
    
    char c = 0;
    node.on_buf = &c;
    node.on_len = 1;
    node.on_bufsz = 1;
    
    if (dev_io(&node) == DFCOKSTUS && node.on_len > 0) {
        return (int)(unsigned char)c;
    }
    
    return -1;
}

/**
 * @brief 输出一个字符
 */
PRIVATE void shell_putchar(char c) {
    char buf[2] = {c, '\0'};
    printk("%s", buf);
}

/**
 * @brief 输出字符串
 */
PRIVATE void shell_puts(const char *s) {
    printk("%s", s);
}

/**
 * @brief 格式化输出
 */
PRIVATE void shell_printf(const char *fmt, ...) {
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    vsprintfk(buf, fmt, ap);
    va_end(ap);
    printk("%s", buf);
}

/*===========================================================================
 * 内置命令实现
 *===========================================================================*/

/**
 * @brief help 命令 - 显示帮助
 */
PRIVATE int cmd_help(int argc, char *argv[]) {
    shell_puts("\n");
    shell_puts("Jeeos Shell - Built-in Commands\n");
    shell_puts("================================\n");
    shell_puts("  help      - Show this help message\n");
    shell_puts("  clear     - Clear the screen\n");
    shell_puts("  time      - Show current system time\n");
    shell_puts("  uptime    - Show system uptime\n");
    shell_puts("  mem       - Show memory information\n");
    shell_puts("  ps        - Show process list\n");
    shell_puts("  echo      - Echo arguments\n");
    shell_puts("  version   - Show system version\n");
    shell_puts("  reboot    - Reboot the system\n");
    shell_puts("\n");
    return 0;
}

/**
 * @brief clear 命令 - 清屏 (恢复背景图片)
 */
PRIVATE int cmd_clear(int argc, char *argv[]) {
    /* 重置字符显示位置 */
    set_charsdxwflush(0, 0);
    /* 恢复背景图片 */
    arch_background();
    return 0;
}

/**
 * @brief time 命令 - 显示时间
 */
PRIVATE int cmd_time(int argc, char *argv[]) {
    shell_puts("\n");
    /* kt_year 存储的是 2 位年份，需要加上 2000 */
    uint_t year = 2000 + osktime.kt_year;
    uint_t mon = osktime.kt_mon;
    uint_t day = osktime.kt_day;
    uint_t hour = osktime.kt_hour;
    uint_t min = osktime.kt_min;
    uint_t sec = osktime.kt_sec;
    
    /* 手动格式化，因为 vsprintfk 不支持 %02d */
    shell_printf("System Time: %d-", year);
    if (mon < 10) shell_putchar('0');
    shell_printf("%d-", mon);
    if (day < 10) shell_putchar('0');
    shell_printf("%d ", day);
    if (hour < 10) shell_putchar('0');
    shell_printf("%d:", hour);
    if (min < 10) shell_putchar('0');
    shell_printf("%d:", min);
    if (sec < 10) shell_putchar('0');
    shell_printf("%d\n", sec);
    return 0;
}

/**
 * @brief uptime 命令 - 显示运行时间
 */
PRIVATE int cmd_uptime(int argc, char *argv[]) {
    u64_t ticks = get_systick();
    u64_t seconds = ticks / 100;  /* 假设 100Hz */
    u64_t minutes = seconds / 60;
    u64_t hours = minutes / 60;
    
    shell_puts("\n");
    shell_printf("System Uptime: %d hours, %d minutes, %d seconds\n",
           (uint_t)hours,
           (uint_t)(minutes % 60),
           (uint_t)(seconds % 60));
    shell_printf("Total Ticks: %d\n", (uint_t)ticks);
    return 0;
}

/**
 * @brief mem 命令 - 显示内存信息
 */
PRIVATE int cmd_mem(int argc, char *argv[]) {
    shell_puts("\n");
    shell_puts("Memory Information\n");
    shell_puts("==================\n");
    
    /* 从 memmgrob 获取真实内存信息 */
    uint_t total_pages = memmgrob.mo_maxpages;
    uint_t free_pages = memmgrob.mo_freepages;
    uint_t alloc_pages = memmgrob.mo_alocpages;
    uint_t total_mb = (total_pages * 4) / 1024;  /* 每页 4KB */
    uint_t free_mb = (free_pages * 4) / 1024;
    uint_t used_mb = (alloc_pages * 4) / 1024;
    
    shell_printf("Total Memory:  %d MB (%d pages)\n", total_mb, total_pages);
    shell_printf("Free Memory:   %d MB (%d pages)\n", free_mb, free_pages);
    shell_printf("Used Memory:   %d MB (%d pages)\n", used_mb, alloc_pages);
    shell_printf("Page Size:     %d bytes\n", PAGE_SIZE);
    shell_puts("\n");
    shell_puts("Memory Pools:\n");
    shell_printf("  Page pools:   %d\n", oskmempool.mp_pgmplnr);
    shell_printf("  Object pools: %d\n", oskmempool.mp_obmplnr);
    shell_puts("\n");
    return 0;
}

/**
 * @brief ps 命令 - 显示进程列表
 */
PRIVATE int cmd_ps(int argc, char *argv[]) {
    shell_puts("\n");
    shell_puts("Process List\n");
    shell_puts("============\n");
    shell_puts("  PID              STATUS  PRI  NAME\n");
    shell_puts("  ---------------  ------  ---  --------\n");
    
    uint_t cpuid = arch_retn_cpuid();
    schdata_t *schdap = &osschedcls.scls_schda[cpuid];
    uint_t count = 0;
    
    /* 显示当前运行的进程 */
    if (schdap->sda_currtd != NULL) {
        thread_t *td = schdap->sda_currtd;
        shell_printf("  %x  RUN     %d    ", (uint_t)td, td->td_priority);
        shell_puts(td->td_name[0] ? td->td_name : "unknown");
        shell_puts("\n");
        count++;
    }
    
    /* 遍历所有优先级队列中的就绪线程 */
    for (uint_t pity = 0; pity < PRITY_MAX; pity++) {
        thrdlst_t *thdlst = &schdap->sda_thdlst[pity];
        if (thdlst->tdl_nr == 0) continue;
        
        list_h_t *pos;
        list_for_each(pos, &thdlst->tdl_lsth) {
            thread_t *td = list_entry(pos, thread_t, td_list);
            /* 跳过当前运行的线程(已显示) */
            if (td == schdap->sda_currtd) continue;
            
            shell_printf("  %x  READY   %d    ", (uint_t)td, td->td_priority);
            shell_puts(td->td_name[0] ? td->td_name : "unknown");
            shell_puts("\n");
            count++;
        }
    }
    
    /* 显示空转进程 */
    if (schdap->sda_cpuidle != NULL) {
        thread_t *td = schdap->sda_cpuidle;
        shell_printf("  %x  IDLE    %d    ", (uint_t)td, td->td_priority);
        shell_puts(td->td_name[0] ? td->td_name : "idle");
        shell_puts("\n");
        count++;
    }
    
    shell_printf("\nTotal: %d (displayed: %d)\n", osschedcls.scls_threadnr, count);
    
    return 0;
}

/**
 * @brief echo 命令 - 回显
 */
PRIVATE int cmd_echo(int argc, char *argv[]) {
    shell_puts("\n");
    for (int i = 1; i < argc; i++) {
        shell_puts(argv[i]);
        if (i < argc - 1) {
            shell_puts(" ");
        }
    }
    shell_puts("\n");
    return 0;
}

/**
 * @brief version 命令 - 显示版本
 */
PRIVATE int cmd_version(int argc, char *argv[]) {
    shell_puts("\n");
    shell_puts("Jeeos 0.12 (Jee Hsu)\n");
    shell_puts("A simple educational operating system\n");
    shell_puts("Copyright (c) 2025 Jee Hsu\n");
    shell_puts("\n");
    return 0;
}

/**
 * @brief reboot 命令 - 重启系统
 */
PRIVATE int cmd_reboot(int argc, char *argv[]) {
    shell_puts("\nRebooting system...\n");
    shell_puts("(In QEMU: use Machine -> Reset to reboot)\n\n");
    
    CLI();
    
    /* QEMU ACPI 关机: 写入 PM1a 控制寄存器 */
    out_u16(0x604, 0x2000);  /* SLP_TYP=5 (S5关机), SLP_EN=1 */
    out_u16(0xB004, 0x2000); /* 备用: QEMU Q35 */
    
    /* 如果 ACPI 关机失败，停机 */
    for (;;) {
        HALT();
    }
    
    return 0;
}

/*===========================================================================
 * 命令表
 *===========================================================================*/

typedef struct {
    const char *name;
    int (*func)(int argc, char *argv[]);
    const char *desc;
} shell_cmd_t;

PRIVATE shell_cmd_t shell_commands[] = {
    { "help",     cmd_help,     "Show help" },
    { "clear",    cmd_clear,    "Clear screen" },
    { "time",     cmd_time,     "Show time" },
    { "uptime",   cmd_uptime,   "Show uptime" },
    { "mem",      cmd_mem,      "Show memory" },
    { "ps",       cmd_ps,       "Show processes" },
    { "echo",     cmd_echo,     "Echo text" },
    { "version",  cmd_version,  "Show version" },
    { "reboot",   cmd_reboot,   "Reboot system" },
    { NULL,       NULL,         NULL }
};

/*===========================================================================
 * 命令解析和执行
 *===========================================================================*/

/**
 * @brief 执行命令
 */
int shell_execute(char *cmdline) {
    char *argv[SHELL_ARGS_MAX];
    int argc = 0;
    
    /* 跳过前导空格 */
    while (*cmdline == ' ' || *cmdline == '\t') {
        cmdline++;
    }
    
    if (*cmdline == '\0') {
        return 0;
    }
    
    /* 分割命令行 */
    char *token = shell_strtok(cmdline, " \t");
    while (token != NULL && argc < SHELL_ARGS_MAX - 1) {
        argv[argc++] = token;
        token = shell_strtok(NULL, " \t");
    }
    argv[argc] = NULL;
    
    if (argc == 0) {
        return 0;
    }
    
    /* 查找并执行命令 */
    for (int i = 0; shell_commands[i].name != NULL; i++) {
        if (shell_strcmp(argv[0], shell_commands[i].name) == 0) {
            return shell_commands[i].func(argc, argv);
        }
    }
    
    /* 未知命令 */
    shell_puts("\nUnknown command: ");
    shell_puts(argv[0]);
    shell_puts("\nType 'help' for available commands.\n");
    
    return -1;
}

/*===========================================================================
 * Shell 主循环
 *===========================================================================*/

/**
 * @brief Shell 主函数
 */
void shell_main(void) {
    printk("\n");
    shell_puts("========================================\n");
    shell_puts("  Jeeos Shell v1.0\n");
    shell_puts("  Type 'help' for available commands\n");
    shell_puts("========================================\n");
    
    /* 查找键盘设备 */
    devid_t kbid;
    kbid.dev_mtype = KEYBOARD_DEVICE;
    kbid.dev_stype = 0;
    kbid.dev_nr = 0;
    kb_device = find_device(&kbid, DIDFIL_IDN);
    
    if (kb_device == NULL) {
        shell_puts("[Shell] Warning: Keyboard not found!\n");
    } else {
        shell_puts("[Shell] Keyboard device found.\n");
    }
    shell_puts("\n");
    
    /* 初始化命令行缓冲区 */
    shell_cmdpos = 0;
    arch_memset(shell_cmdline, 0, SHELL_CMDLINE_MAX);
    
    /* 显示提示符 */
    shell_puts(SHELL_PROMPT);
    
    for (;;) {
        int c = shell_getchar();
        
        if (c < 0) {
            die(50);
            schedul();
            continue;
        }
        
        switch (c) {
        case '\n':
        case '\r':
            shell_puts("\n");
            shell_cmdline[shell_cmdpos] = '\0';
            
            if (shell_cmdpos > 0) {
                shell_execute(shell_cmdline);
            }
            
            shell_cmdpos = 0;
            arch_memset(shell_cmdline, 0, SHELL_CMDLINE_MAX);
            shell_puts(SHELL_PROMPT);
            break;
            
        case '\b':
        case 0x7F:
            if (shell_cmdpos > 0) {
                shell_cmdpos--;
                shell_cmdline[shell_cmdpos] = '\0';
                shell_puts("\b \b");
            }
            break;
            
        default:
            if (c >= 32 && c < 127 && shell_cmdpos < SHELL_CMDLINE_MAX - 1) {
                shell_cmdline[shell_cmdpos++] = (char)c;
                shell_putchar((char)c);
            }
            break;
        }
    }
}

/**
 * @brief 初始化并启动 Shell
 */
void init_shell(void) {
    thread_t *shell_td = new_thread(
        (void *)shell_main,
        KERNTHREAD_FLG,
        PRILG_SYS,
        PRITY_MIN,
        DAFT_TDUSRSTKSZ,
        DAFT_TDKRLSTKSZ,
        "shell"
    );
    
    if (shell_td != NULL) {
        printk("  - [Shell] Initialized\n");
    } else {
        printk("  - [Shell] Init FAILED!\n");
    }
}
