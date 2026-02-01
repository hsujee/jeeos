/*
 * @Author: Jee Hsu
 * @Description: Init 进程 - 所有用户进程的祖先
 *
 * Init 进程是系统中第一个用户进程（PID=1）：
 * - 负责启动 shell 等系统服务
 * - 收养孤儿进程
 * - 系统关机时最后退出
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "global_types.h"
#include "global_heads.h"

/* Init 进程指针 */
PRIVATE thread_t *init_thread = NULL;

/* 键盘设备 */
PRIVATE device_t *init_kb_device = NULL;

/**
 * @brief 获取 init 进程
 */
thread_t *get_init_thread(void) {
    return init_thread;
}

/**
 * @brief 初始化键盘设备
 */
PRIVATE void init_keyboard(void) {
    devid_t kbid;
    kbid.dev_mtype = KEYBOARD_DEVICE;
    kbid.dev_stype = 0;
    kbid.dev_nr = 0;
    init_kb_device = find_device(&kbid, DIDFIL_IDN);
    
    if (init_kb_device != NULL) {
        /* 打开键盘设备 */
        objnode_t node;
        objnode_t_init(&node);
        node.on_objadr = init_kb_device;
        node.on_opercode = IOIF_CODE_OPEN;
        dev_io(&node);
    }
}

/**
 * @brief 等待用户按任意键
 */
PRIVATE void wait_for_keypress(void) {
    if (init_kb_device == NULL) {
        /* 没有键盘设备，延时后继续 */
        die(3000);
        schedul();
        return;
    }
    
    objnode_t node;
    char c = 0;
    
    /* 等待按键 */
    while (c == 0) {
        objnode_t_init(&node);
        node.on_objadr = init_kb_device;
        node.on_opercode = IOIF_CODE_READ;
        node.on_buf = &c;
        node.on_bufsz = 1;
        node.on_len = 1;
        
        dev_io(&node);
        
        if (c == 0) {
            die(50);
            schedul();
        }
    }
}

/**
 * @brief Init 进程主函数
 * 
 * 作为 PID=1 的第一个用户进程运行
 */
void init_main(void) {
    thread_t *self = get_current_thread();
    
    printk("  - [Init] Process started (PID=%d)\n", self->td_pid);
    
    /* 初始化键盘 */
    init_keyboard();
    
    /* 显示提示并等待用户按键 */
    printk("\n");
    printk("  Press any key to enter shell...\n");
    wait_for_keypress();
    
    /* 启动 shell 作为子进程 */
    extern void shell_main(void);
    thread_t *shell = fork_thread(self, (void *)shell_main, "shell");
    
    if (shell != NULL) {
        printk("  - [Init] Shell started (PID=%d)\n", shell->td_pid);
    } else {
        printk("  - [Init] Failed to start shell!\n");
    }
    
    /* Init 进程主循环 - 等待并回收子进程 */
    for (;;) {
        /* TODO: 实现 wait() 回收僵尸子进程 */
        
        /* 让出 CPU */
        die(100);
        schedul();
    }
}

/**
 * @brief 创建 init 进程
 * 
 * 由内核启动时调用，创建 PID=1 的 init 进程
 */
void init_process(void) {
    /* 创建 init 进程 */
    init_thread = new_thread(
        (void *)init_main,
        KERNTHREAD_FLG,
        PRILG_SYS,
        PRITY_MIN,
        DAFT_TDUSRSTKSZ,
        DAFT_TDKRLSTKSZ,
        "init"
    );
    
    if (init_thread != NULL) {
        /* init 进程的 PID 应该是 1 */
        printk("  - [Init] Created (PID=%d)\n", init_thread->td_pid);
    } else {
        printk("  - [Init] Creation FAILED!\n");
    }
}

