# Jeeos-0.12 完整操作系统

> ⚡ **新手？** 查看 [快速开始指南](./INDEX.md#-快速开始指南) 10分钟上手！

## 📖 项目简介

Jeeos-0.12 是一个完整的操作系统实现，基于 x86_64 架构，包含完整的内核、驱动程序、用户空间支持和交互式 Shell。这是 Jeeos 系列的完整版本，展示了从引导到用户交互的完整操作系统实现。

### 🎯 主要特性

- ✅ **完整的 HAL 层**
  - 二级引导程序（Bootsect + Setup + Bootparam）
  - GDT/IDT 管理
  - 8259 PIC 中断控制器（支持 IRQ0-IRQ15）
  - MMU 页表管理
  - 视频驱动（VGA/BGA）
  
- ✅ **内存管理**
  - 物理页描述符管理 (`page_desc.c`)
  - 物理页分配/释放 (`page_alloc.c`)
  - 内存区域管理 (`mem_zone.c`)
  - SLAB 对象分配器 (`slab_alloc.c`)
  - 虚拟内存管理 (`vmem.c`)
  - Buddy System 内存分配
  
- ✅ **进程管理**
  - 线程管理 (`thread.c`)
  - 调度器 (`sched.c`) - 优先级调度 + 时间片轮转
  - 等待队列 (`wait_list.c`)
  - 信号量 (`sem.c`)
  - 自旋锁 (`spin_lock.c`)
  - Fork 支持 (`sysfork.c`)
  
- ✅ **设备管理**
  - 设备管理框架 (`device.c`)
  - I/O 节点管理 (`io_node.c`)
  - 中断管理 (`interrupt.c`)
  - **统一的驱动注册框架** (`driver_helper.c`)
  - **统一的错误处理框架** (`kernel_error.c`)
  
- ✅ **设备驱动**
  - RAM文件系统驱动 (`ramfs.c`)
  - 定时器驱动 (`timer.c`) - 系统时钟
  - 串口驱动 (`serial.c`) - UART 16550
  - **PS/2 键盘驱动** (`keyboard.c`) - 支持按键输入
  - NULL/ZERO 设备 (`nullzero.c`)
  - PC 蜂鸣器驱动 (`speaker.c`)
  - RTC 实时时钟驱动 (`rtc.c`)
  - 随机数生成器驱动 (`random.c`)
  
- ✅ **系统调用**
  - 完整的系统调用接口 (`syshandler.c`)
  - open/close/read/write/lseek/ioctl
  - 内存管理系统调用
  - 线程系统调用
  - Fork 系统调用
  
- ✅ **用户空间支持**
  - Init 进程 (`init.c`) - PID=1，所有用户进程的祖先
  - **Shell 命令行解释器** (`shell.c`) - 交互式命令行
  - 用户空间库 (`libs/`)
  - printf 实现 (`printf.c`)

- ✅ **代码优化**
  - 驱动注册辅助函数 (`driver_helper.h/c`) - 统一驱动注册流程
  - 初始化宏 (`init_macros.h`) - 减少重复代码
  - 统一错误处理 (`kernel_error.h/c`) - 标准化错误报告
  - 串口调试支持 (`debug_uart.h/c`) - 调试输出

## 🚀 快速开始

```bash
# 1. 安装依赖
sudo apt update
sudo apt install -y nasm gcc gcc-multilib make binutils \
    qemu-system-x86 grub-pc-bin grub2-common debugfs

# 2. 查看帮助
make help

# 3. 编译
make build

# 4. 运行
make qemurun

# 5. 系统启动后，按任意键进入 Shell
```

## 🛠️ 构建命令

| 命令 | 说明 |
|------|------|
| `make help` | 显示帮助信息 |
| `make build` | 编译内核 |
| `make install` | 编译并安装到 release 目录 |
| `make clean` | 清理编译文件 |
| `make qemurun` | 使用 QEMU 运行 |
| `make qemudebug` | QEMU + GDB 调试模式 (端口:1234) |
| `make vboxrun` | 使用 VirtualBox 运行 |
| `make gdb` | 显示 GDB 连接命令 |
| `make gdbrun` | 启动 GDB 调试脚本 |
| `make cscope` | 生成 cscope 索引 |
| `make tags` | 生成 ctags 索引 |
| `make cg` | 生成调用图 |

## 🔧 调试方法

### GDB 调试

```bash
# 终端1: 启动调试模式
make qemudebug

# 终端2: 连接GDB
make gdbrun

# 或手动连接:
# gdb build/jeeos.elf
# (gdb) target remote :1234
# (gdb) break start_kernel
# (gdb) continue
```

### 串口调试

系统支持串口调试输出（COM1, 115200 bps），日志文件位于 `/tmp/serial.log`：

```bash
# 查看串口日志
tail -f /tmp/serial.log
```

## 📁 项目结构

```
Jeeos-0.12/
├── arch/x86/                   # x86 架构相关
│   ├── boot/                   # 引导程序
│   │   ├── bootsect.asm        # 一级引导 (汇编)
│   │   ├── bootsect_c.c        # 一级引导 (C)
│   │   ├── setup.asm           # 二级引导 (汇编)
│   │   ├── setup_c.c           # 二级引导 (C)
│   │   ├── bootparam.asm       # 引导参数 (汇编)
│   │   ├── bootparam_c.c       # 引导参数 (C)
│   │   ├── cpumem.c            # CPU/内存检测
│   │   ├── fs.c                # 引导文件系统
│   │   ├── graph.c             # 图形模式
│   │   ├── vgastr.c            # VGA 字符输出
│   │   ├── include/            # 引导头文件
│   │   └── Makefile            # 引导编译脚本
│   ├── kernel/                 # HAL 层
│   │   ├── startup.c           # 内核入口
│   │   ├── arch_init.c         # 架构初始化
│   │   ├── arch_global.c       # 架构全局数据
│   │   ├── arch_mm.c           # 架构内存管理
│   │   ├── arch_trap.c         # 陷阱处理（修复了IRQ计算）
│   │   ├── platform.c          # 平台初始化
│   │   ├── idt.c               # IDT 管理
│   │   ├── i8259.c             # 8259 PIC 控制器
│   │   ├── mmu.c               # MMU 页表管理
│   │   ├── mm_init.c           # 内存初始化
│   │   ├── page_desc.c         # 物理页描述符
│   │   ├── page_alloc.c        # 物理页分配
│   │   ├── mem_zone.c          # 内存区域
│   │   ├── slab_alloc.c        # SLAB 分配器
│   │   ├── cpu_ops.c           # CPU 操作
│   │   ├── video.c             # 视频驱动
│   │   ├── printk.c            # 内核打印
│   │   ├── entry.asm           # 入口汇编
│   │   └── trap.asm            # 陷阱汇编
│   └── include/                # HAL 头文件
├── kernel/                     # 内核核心
│   ├── main.c                  # 内核主入口
│   ├── global.c                # 全局数据
│   ├── mm.c                    # 内存管理
│   ├── mm_pages.c              # 页面管理
│   ├── vmem.c                  # 虚拟内存
│   ├── sched.c                 # 调度器
│   ├── thread.c                # 线程管理
│   ├── wait_list.c             # 等待队列
│   ├── sem.c                   # 信号量
│   ├── spin_lock.c             # 自旋锁
│   ├── time.c                  # 时间管理
│   ├── device.c                # 设备管理
│   ├── io_node.c               # I/O 节点
│   ├── interrupt.c             # 中断管理
│   ├── cpu_idle.c              # CPU 空闲进程
│   ├── init.c                  # Init 进程（PID=1）
│   ├── shell.c                 # Shell 命令行解释器
│   ├── driver_helper.c         # 驱动注册辅助函数
│   ├── kernel_error.c          # 统一错误处理
│   ├── debug_uart.c            # 串口调试支持
│   └── include/                # 内核头文件
│       ├── driver_helper.h     # 驱动注册接口
│       ├── kernel_error.h      # 错误处理接口
│       ├── init_macros.h       # 初始化宏
│       └── debug_uart.h        # 串口调试接口
├── drivers/                    # 设备驱动
│   ├── ramfs.c                 # RAM文件系统驱动
│   ├── timer.c                 # 定时器驱动
│   ├── serial.c                # 串口驱动
│   ├── keyboard.c              # PS/2 键盘驱动
│   ├── nullzero.c              # NULL/ZERO 设备
│   ├── speaker.c               # PC 扬声器驱动
│   ├── rtc.c                   # RTC 实时时钟驱动
│   ├── random.c                # 随机数生成器驱动
│   └── include/                # 驱动头文件
│       ├── drv_deps.h          # 驱动层依赖聚合
│       ├── drv_types.h         # 驱动类型定义
│       └── drv_heads.h         # 驱动接口定义
├── system/                     # 系统调用层
│   ├── syshandler.c            # 系统调用入口
│   ├── sysopen.c               # open 系统调用
│   ├── sysclose.c              # close 系统调用
│   ├── sysread.c               # read 系统调用
│   ├── syswrite.c              # write 系统调用
│   ├── syslseek.c              # lseek 系统调用
│   ├── sysioctrl.c             # ioctl 系统调用
│   ├── sysmm.c                 # 内存系统调用
│   ├── systhread.c             # 线程系统调用
│   ├── sysfork.c               # fork 系统调用
│   └── include/                # 系统调用头文件
├── libs/                       # 用户空间库
│   ├── libmem.c                # 内存 API
│   ├── libio.c                 # I/O API
│   ├── libthread.c             # 线程 API
│   ├── libtime.c               # 时间 API
│   ├── printf.c                # printf 实现
│   ├── string.c                # 字符串函数
│   ├── start.asm               # 用户程序入口
│   └── include/                # 库头文件
├── tools/                      # 工具
│   ├── buildtool/              # 构建工具
│   ├── mk_hdisk.sh             # 硬盘创建脚本
│   └── gdb_debug.sh            # GDB 调试脚本
├── script/                     # 构建脚本
│   ├── prebuild.mkf            # 预构建脚本
│   ├── qemu.mkf                # QEMU 运行脚本
│   ├── vbox.mkf                # VirtualBox 脚本
│   └── include/                # 构建配置
├── res/                        # 资源文件
│   ├── fonts/                  # 字体文件
│   └── images/                 # 图片文件
├── build/                      # 编译输出目录
├── docs/                       # 项目文档
├── config.h                    # 全局配置
└── Makefile                    # 主构建文件
```

## 🔄 构建流程

```
Makefile (主构建文件)
  │
  ├── 1. buildtool      编译构建工具 (tools/buildtool)
  │
  ├── 2. boot           编译引导程序 (arch/x86/boot)
  │       ├── bootsect.bin   一级引导
  │       ├── setup.bin      二级引导
  │       └── bootparam.bin  引导参数
  │
  ├── 3. prebuild       预处理构建脚本
  │
  ├── 4. archkernel     编译 HAL 层 (arch/x86/kernel)
  │
  ├── 5. kernel         编译内核 (kernel/)
  │
  ├── 6. system         编译系统调用层 (system/)
  │
  ├── 7. drivers        编译设备驱动 (drivers/)
  │
  ├── 8. libs           编译用户空间库 (libs/)
  │
  ├── 9. link           链接生成 jeeos.bin
  │
  └── 10. mkimg         打包生成 jeeos.eki
```

## 🔄 启动流程

```
GRUB
  └── bootsect.asm (一级引导)
        └── setup.asm (二级引导: 进入保护模式)
              └── bootparam.asm (收集硬件信息)
                    └── startup.c::start_kernel()
                          ├── init_arch()
                          │     ├── init_archplalt()  # 平台初始化
                          │     ├── init_archmm()     # 内存初始化
                          │     └── init_archint()    # 中断初始化
                          └── init_kernel()
                                ├── init_mm()         # 内存管理器
                                ├── init_ktime()      # 时间子系统
                                ├── init_sched()      # 调度器
                                ├── init_device()     # 设备子系统
                                ├── init_driver()     # 驱动程序加载
                                │     ├── timer       # 定时器驱动
                                │     ├── serial      # 串口驱动
                                │     ├── keyboard    # 键盘驱动
                                │     └── ...
                                ├── init_cpuidle()    # 空闲进程
                                └── init_main()       # Init 进程 (PID=1)
                                      └── shell_main() # Shell 进程
```

## 📊 启动日志

```
Jeeos v0.12 (Jee Hsu) Dec 25 2025 16:46:22 x86_64 | 1023MB RAM
CPU: QEMU Virtual CPU version 2.5+
[ARCH] Platform initialized.
[ARCH] Initializing memory...
[MM] 1000MB RAM, 256126 pages free.
[ARCH] Initializing interrupts...
  - Setting up GDT...
  - Setting up IDT...
  - Initializing interrupt descriptors...
  - Initializing 8259 PIC...
[ARCH] Architecture layer ready.
[Jeeos-0.12] Kernel Starting...
[KERNEL] Initializing memory manager...
[KERNEL] Initializing time subsystem...
[KERNEL] Initializing scheduler...
[KERNEL] Initializing device subsystem...
[KERNEL] Loading drivers...
  - Loading UART driver... OK
  - Loading SYSTICK driver... OK
  - Loading RFS driver... OK
  - Loading NULL/ZERO driver... OK
  - Loading SPEAKER driver... OK
  - Loading RTC driver... OK
  - Loading RANDOM driver... OK
  - Loading KEYBOARD driver... OK
[DRIVER] 8 drivers loaded, 0 failed.
[KERNEL] All subsystems ready.
[KERNEL] Creating idle process...
  - Idle process created at ffff80003f9101b0
  - [Init] Created (PID=1)
[Jeeos-0.12] System Running!
  - [Init] Process started (PID=1)

  Press any key to enter shell...

========================================
  Jeeos Shell v1.0
  Type 'help' for available commands
========================================
[Shell] Keyboard device found.

jeeos> help
```

## 🏗️ 系统架构

```
┌─────────────────────────────────────────────────────────────┐
│                        应用程序                              │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  Init (PID=1) - Kernel启动后创建的第一个进程        │   │
│  │  Shell (PID=2) - 简单版的Shell程序                  │   │
│  │  (作为独立进程运行)                                 │   │
│  └─────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────┤
│                      库层 (libs/)                           │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  libmem.c, libio.c, libthread.c, libtime.c          │   │
│  │  printf.c, string.c (libc库文件，类似glibc)         │   │
│  └─────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────┤
│                    系统调用层 (system/)                      │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  syshandler.c, sysopen.c, sysread.c, syswrite.c... │   │
│  │  sysfork.c (进程创建), syslseek.c, sysioctrl.c      │   │
│  └─────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────┤
│                      内核层 (kernel/)                       │
│  ┌───────────┐  ┌───────────┐  ┌───────────┐             │
│  │  内存管理  │  │  进程调度  │  │  设备管理  │             │
│  │   mm.c    │  │  sched.c  │  │ device.c  │             │
│  └───────────┘  └───────────┘  └───────────┘             │
│  ┌───────────────────────────────────────────────────────┐ │
│  │  driver_helper.c (统一驱动注册)                        │ │
│  │  kernel_error.c (统一错误处理)                         │ │
│  └───────────────────────────────────────────────────────┘ │
├─────────────────────────────────────────────────────────────┤
│                      驱动层 (drivers/)                      │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  timer.c, serial.c, keyboard.c, ramfs.c            │   │
│  │  nullzero.c, speaker.c, rtc.c, random.c            │   │
│  └─────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────┤
│                    HAL 层 (arch/x86/kernel/)                │
│  ┌───────────┐  ┌───────────┐  ┌───────────┐  ┌─────────┐ │
│  │ 中断管理   │  │  MMU管理   │  │  CPU操作   │  │ 平台初始│ │
│  │ idt.c     │  │  mmu.c    │  │ cpu_ops.c │  │platform │ │
│  │ i8259.c   │  │           │  │           │  │         │ │
│  └───────────┘  └───────────┘  └───────────┘  └─────────┘ │
├─────────────────────────────────────────────────────────────┤
│                     引导层 (arch/x86/boot/)                 │
│  ┌───────────────────────────────────────────────────────┐ │
│  │  bootsect.asm -> setup.asm -> bootparam.asm -> startup │ │
│  └───────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

## 🎮 Shell 命令

Jeeos Shell 支持以下内置命令：

| 命令 | 说明 |
|------|------|
| `help` | 显示帮助信息 |
| `clear` | 清屏 |
| `time` | 显示系统时间 |
| `uptime` | 显示系统运行时间 |
| `mem` | 显示内存信息 |
| `ps` | 显示进程列表 |
| `echo <text>` | 回显文本 |
| `version` | 显示版本信息 |
| `reboot` | 重启系统 |

## 📝 代码规范

### 头文件包含策略（混合模式）

项目采用混合模式的头文件包含策略，平衡简洁性与编译效率：

```
┌─────────────────────────────────────────────────────────────┐
│                    头文件包含策略                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  kernel/ & arch/  ──►  global_types.h + global_heads.h     │
│  (核心模块)             (全局聚合 - 保持简洁性)              │
│                                                             │
│  drivers/         ──►  drv_deps.h                          │
│  (驱动模块)             (不含 libs 层, 避免用户态依赖)       │
│                                                             │
│  system/          ──►  sys_deps.h                          │
│  (系统服务)             (不含 drv/libs 层, 精确依赖)         │
│                                                             │
│  libs/            ──►  libtypes.h + libheads.h             │
│  (用户库)               (完全独立, 最小依赖)                 │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**依赖头文件说明：**

| 文件 | 位置 | 用途 |
|------|------|------|
| `global_types.h` + `global_heads.h` | `libs/include/` | 核心模块全局聚合 |
| `drv_deps.h` | `drivers/include/` | 驱动层精确依赖聚合 |
| `sys_deps.h` | `system/include/` | 系统服务层精确依赖聚合 |
| `libtypes.h` + `libheads.h` | `libs/include/` | 用户库独立依赖 |

### 驱动注册框架

使用 `driver_helper.h/c` 统一驱动注册流程：

```c
// 通用设备注册
device_t* register_device_common(
    driver_t *drvp,
    const dev_reg_info_t *info,
    void (*set_driver_fn)(driver_t*)
);

// 带中断的设备注册
device_t* register_device_with_irq(
    driver_t *drvp,
    const dev_reg_info_t *info,
    void (*set_driver_fn)(driver_t*),
    int_handler_t irq_handler,
    uint_t irq_vector
);
```

### 统一错误处理

使用 `kernel_error.h/c` 统一错误报告：

```c
void kernel_error(err_level_t level, const char *module, const char *msg);

// 宏定义
#define KERNEL_PANIC(msg) kernel_error(ERR_LEVEL_FATAL, __FILE__, msg)
#define KERNEL_ERROR(msg) kernel_error(ERR_LEVEL_ERROR, __FILE__, msg)
#define KERNEL_WARN(msg) kernel_error(ERR_LEVEL_WARN, __FILE__, msg)
```

### arch_ 前缀规范

| 使用场景 | 示例 |
|---------|------|
| 硬件操作函数 | `arch_spinlock_lock()`, `arch_wbinvd()`, `arch_memset()` |
| MMU 管理函数 | `arch_mmu_transform()`, `arch_mmu_load()`, `arch_mmu_init()` |
| 架构特定接口 | `arch_add_ihandle()`, `arch_logo()`, `arch_sysdie()` |
| 中断控制函数 | `arch_sti_cpuflag()`, `arch_cli_cpuflag()` |

### 页表命名约定

| 本项目命名 | x86-64 标准术语 | 说明 |
|-----------|----------------|------|
| `tdire_t` | PML4E | 顶级目录项 (Top Directory Entry) |
| `sdire_t` | PDPTE | 次级目录项 (Second Directory Entry) |
| `idire_t` | PDE | 页目录项 (Internal Directory Entry) |
| `mdire_t` | PTE | 页表项 (Middle Directory Entry) |

## 🔗 相关版本

| 版本 | 说明 |
|------|------|
| Jeeos-0.01 | 入门级内核，仅 VGA 输出 |
| Jeeos-0.11 | 中级框架，完整 HAL + 内存管理 |
| **Jeeos-0.12** | 完整版，包含驱动、Shell 和用户空间 |

## 👤 作者

- **Author**: Jee Hsu
- **Version**: 0.12
- **License**: MIT
