/*
 * @Author: Jee Hsu
 * @Description: 基础类型定义 - 系统基本类型
 *
 * 定义系统使用的基本数据类型、宏和错误码
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef TYPEDEF_H
#define TYPEDEF_H

/*===========================================================================
 * 基本整数类型
 *===========================================================================*/

typedef unsigned char       u8_t;       /**< 无符号8位整数 */
typedef unsigned short      u16_t;      /**< 无符号16位整数 */
typedef unsigned int        u32_t;      /**< 无符号32位整数 */
typedef unsigned long       u64_t;      /**< 无符号64位整数 */
typedef char                s8_t;       /**< 有符号8位整数 */
typedef short               s16_t;      /**< 有符号16位整数 */
typedef int                 s32_t;      /**< 有符号32位整数 */
typedef long                s64_t;      /**< 有符号64位整数 */
typedef long                sint_t;     /**< 有符号机器字长整数 */
typedef unsigned long       uint_t;     /**< 无符号机器字长整数 */

/*===========================================================================
 * 系统类型定义
 *===========================================================================*/

typedef u64_t               cpuflg_t;   /**< CPU标志类型 */
typedef unsigned long       adr_t;      /**< 地址类型 */
typedef sint_t              bool_t;     /**< 布尔类型 */
typedef u32_t               dev_t;      /**< 设备ID类型 */
typedef const char*         str_t;      /**< 字符串类型 */
typedef char                char_t;     /**< 字符类型 */
typedef u32_t               reg_t;      /**< 寄存器类型 */

/*===========================================================================
 * 状态和返回值类型
 *===========================================================================*/

typedef sint_t              drvstus_t;  /**< 驱动状态类型 */
typedef sint_t              sysstus_t;  /**< 系统调用状态类型 */
typedef u64_t               mmstus_t;   /**< 内存状态类型 */
typedef sint_t              hand_t;     /**< 句柄类型 */
typedef void*               buf_t;      /**< 缓冲区类型 */

/* size_t: 避免与标准库冲突 */
#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
typedef unsigned long       size_t;     /**< 大小类型 */
#endif

/*===========================================================================
 * 函数指针类型
 *===========================================================================*/

/** @brief 通用中断处理函数 (无参数) */
typedef void (*inthandler_t)(void);

/** @brief 中断处理函数 (带中断号) */
typedef drvstus_t (*irq_handler_t)(uint_t int_nr);

/** @brief 异常处理函数 (带栈帧) */
typedef drvstus_t (*fault_handler_t)(uint_t int_nr, void* sframe);

/** @brief 设备中断回调函数 */
typedef drvstus_t (*int_handler_t)(uint_t ift_nr, void* device, void* sframe);

/*===========================================================================
 * 常用宏定义
 *===========================================================================*/

/* 函数修饰符 */
#define KLINE               static inline   /**< 内联函数标记 */
#define PUBLIC                              /**< 公共函数标记 */
#define PRIVATE             static          /**< 私有函数标记 */
#define EXTERN              extern          /**< 外部变量标记 */
#define KEXTERN             extern          /**< 内核全局变量标记 (配合 KRL_DEFGLOB_VARIABLE 使用) */

/* 布尔值和空指针 (带保护) */
#ifndef NULL
#define NULL                0               /**< 空指针/空值 */
#endif
#ifndef TRUE
#define TRUE                1               /**< 真值 */
#endif
#ifndef FALSE
#define FALSE               0               /**< 假值 */
#endif

/* 状态码 */
#define DFCERRSTUS          (-1)            /**< 默认错误状态 */
#define DFCOKSTUS           (0)             /**< 默认成功状态 */
#define NO_HAND             (-1)            /**< 无效句柄 */

/* 对齐宏 */
#define ALIGN(x, a)         (((x) + (a) - 1) & ~((a) - 1))

/*===========================================================================
 * 段属性宏
 *===========================================================================*/

#define LKHEAD_T            __attribute__((section(".head.text")))  /**< 头部代码段 */
#define LKHEAD_D            __attribute__((section(".head.data")))  /**< 头部数据段 */
#define LKINIT                                                      /**< 初始化函数标记 */

/*===========================================================================
 * 错误码定义 (POSIX兼容)
 *===========================================================================*/

#define EOK                 0       /**< 成功 */
#define EPERM               1       /**< 操作不允许 */
#define ENOENT              2       /**< 文件或目录不存在 */
#define ESRCH               3       /**< 进程不存在 */
#define EINTR               4       /**< 系统调用被中断 */
#define EIO                 5       /**< I/O错误 */
#define ENXIO               6       /**< 设备或地址不存在 */
#define E2BIG               7       /**< 参数列表过长 */
#define ENOEXEC             8       /**< 可执行格式错误 */
#define EBADF               9       /**< 文件描述符错误 */
#define ECHILD              10      /**< 无子进程 */
#define EAGAIN              11      /**< 请重试 */
#define ENOMEM              12      /**< 内存不足 */
#define EACCES              13      /**< 权限拒绝 */
#define EFAULT              14      /**< 地址错误 */
#define ENOTBLK             15      /**< 需要块设备 */
#define EBUSY               16      /**< 设备或资源忙 */
#define EEXIST              17      /**< 文件已存在 */
#define EXDEV               18      /**< 跨设备链接 */
#define ENODEV              19      /**< 设备不存在 */
#define ENOTDIR             20      /**< 不是目录 */
#define EISDIR              21      /**< 是目录 */
#define EINVAL              22      /**< 参数无效 */
#define ENFILE              23      /**< 文件表溢出 */
#define EMFILE              24      /**< 打开文件过多 */
#define ENOTTY              25      /**< 不是终端 */
#define ETXTBSY             26      /**< 文本文件忙 */
#define EFBIG               27      /**< 文件过大 */
#define ENOSPC              28      /**< 磁盘空间不足 */
#define ESPIPE              29      /**< 非法seek */
#define EROFS               30      /**< 只读文件系统 */
#define EMLINK              31      /**< 链接过多 */
#define EPIPE               32      /**< 管道断开 */
#define EDOM                33      /**< 数学参数超出域 */
#define ERANGE              34      /**< 数学结果不可表示 */
#define EALLOC              35      /**< 分配错误 */
#define ENOOBJ              36      /**< 对象不存在 */
#define EGOON               37      /**< 继续 */
#define ECPLT               38      /**< 完成 */
#define EPARAM              39      /**< 参数错误 */

#endif /* TYPEDEF_H */
