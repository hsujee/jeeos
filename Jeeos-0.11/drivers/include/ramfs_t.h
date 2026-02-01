/*
 * @Author: Jee Hsu
 * @Description: RAM文件系统类型定义
 *
 * 定义RFS文件系统的数据结构：
 * - rfsdevext_t: 设备扩展(文件系统元数据)
 * - rfssublk_t: 超级块
 * - rfsdir_t: 目录项
 * - filblks_t: 文件块索引
 * - fimgrhd_t: 文件管理头
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef DRVRFS_T_H
#define DRVRFS_T_H
#define DR_NM_MAX (128-(sizeof(uint_t)*3))
#define FBLKS_MAX 32
#define RDR_NUL_TYPE 0
#define RDR_DIR_TYPE 1
#define RDR_FIL_TYPE 2
#define RDR_DEL_TYPE 5

#define FMD_NUL_TYPE 0
#define FMD_DIR_TYPE 1
#define FMD_FIL_TYPE 2
#define FMD_DEL_TYPE 5  //文件管理头也需要表明它管理的是目录文件还是普通文件
#define FSMM_BLK 0x400000
#define FSYS_ALCBLKSZ 0x1000

/**
 * @brief RFS设备扩展结构
 * 
 * 保存文件系统使用的内存块信息
 */
typedef struct s_RFSDEVEXT {
    spinlock_t rde_lock;    /**< 保护设备扩展的自旋锁 */
    list_h_t rde_list;      /**< 链表节点 */
    uint_t rde_flags;       /**< 标志 */
    uint_t rde_status;        /**< 状态 */
    void* rde_mstart;       /**< 存储介质内存块起始地址 */
    size_t rde_msize;       /**< 内存块大小 */
    void* rde_ext;          /**< 扩展数据 */
} rfsdevext_t;

/**
 * @brief RFS目录项结构
 * 
 * 描述目录中的一个条目(文件或子目录)
 */
typedef struct s_RFSDIR {
    uint_t rdr_status;        /**< 目录项状态 */
    uint_t rdr_type;        /**< 类型: 空/目录/文件/已删除 */
    uint_t rdr_blknr;       /**< 文件管理头所在块号 */
    char_t rdr_name[DR_NM_MAX]; /**< 名称(最大116字节) */
} rfsdir_t;

/**
 * @brief 文件块索引结构
 * 
 * 描述文件占用的一段连续逻辑块
 */
typedef struct s_FILBLKS {
    uint_t fb_blkstart;     /**< 起始逻辑块号 */
    uint_t fb_blknr;        /**< 连续块数量 */
} filblks_t;

/**
 * @brief RFS超级块结构
 * 
 * 文件系统元数据，位于存储介质开头
 */
typedef struct s_RFSSUBLK {
    spinlock_t rsb_lock;    /**< 保护超级块的自旋锁 */
    uint_t rsb_mgic;        /**< 文件系统魔数 */
    uint_t rsb_vec;         /**< 文件系统版本 */
    uint_t rsb_flags;       /**< 标志 */
    uint_t rsb_status;        /**< 状态 */
    size_t rsb_sz;          /**< 超级块结构大小 */
    size_t rsb_sblksz;      /**< 超级块占用大小 */
    size_t rsb_dblksz;      /**< 逻辑块大小(4KB) */
    uint_t rsb_bmpbks;      /**< 位图起始块号 */
    uint_t rsb_bmpbknr;     /**< 位图占用块数 */
    uint_t rsb_fsysallblk;  /**< 文件系统总块数 */
    rfsdir_t rsb_rootdir;   /**< 根目录项 */
} rfssublk_t;

/**
 * @brief 文件管理头结构
 * 
 * 描述一个文件的元数据和数据块位置
 */
typedef struct s_fimgrhd {
    uint_t fmd_status;        /**< 文件状态 */
    uint_t fmd_type;        /**< 文件类型: 目录/普通/空/已删除 */
    uint_t fmd_flags;       /**< 文件标志 */
    uint_t fmd_sfblk;       /**< 本结构所在块号 */
    uint_t fmd_acss;        /**< 访问权限 */
    uint_t fmd_newtime;     /**< 创建时间(秒) */
    uint_t fmd_acstime;     /**< 访问时间(秒) */
    uint_t fmd_fileallbk;   /**< 文件总块数 */
    uint_t fmd_filesz;      /**< 文件大小(字节) */
    uint_t fmd_fileifstbkoff;   /**< 首块内数据偏移 */
    uint_t fmd_fileiendbkoff;   /**< 尾块内数据偏移 */
    uint_t fmd_curfwritebk;     /**< 当前写入块号 */
    uint_t fmd_curfinwbkoff;    /**< 当前写入块内偏移 */
    filblks_t fmd_fleblk[FBLKS_MAX]; /**< 数据块索引数组(32项) */
    uint_t fmd_linkpblk;    /**< 上一个管理头块号 */
    uint_t fmd_linknblk;    /**< 下一个管理头块号 */
} fimgrhd_t;

#endif
