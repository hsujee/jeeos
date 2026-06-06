/*
 * @Author: Jee Hsu
 * @Description: 链表和树类型定义
 *
 * 提供通用的数据结构类型：
 * - list_h_t: 双向链表节点
 * - tree_t: 红黑树节点
 *
 * 链表使用：将list_h_t嵌入到其他结构体中
 * 通过list_entry宏从节点获取包含它的结构体指针
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef LIST_T_H
#define LIST_T_H

/**
 * @brief 双向链表节点结构
 * 
 * 内嵌到其他结构体中实现链表功能
 * 通过 list_entry 宏可从节点获取包含它的结构体指针
 */
typedef struct s_LIST_H {
    struct s_LIST_H *prev;  /**< 前驱节点指针 */
    struct s_LIST_H *next;  /**< 后继节点指针 */
} list_h_t;

/**
 * @brief 红黑树节点结构
 * 
 * 用于实现平衡二叉搜索树，保证O(log n)的查找/插入/删除
 */
typedef struct s_TREE {
    u16_t tr_type;              /**< 节点类型 */
    u16_t tr_color;             /**< 节点颜色(红/黑) */
    u64_t tr_hight;             /**< 子树高度 */
    struct s_TREE* tr_left;     /**< 左子节点 */
    struct s_TREE* tr_right;    /**< 右子节点 */
    struct s_TREE* tr_paret;    /**< 父节点 */
    struct s_TREE* tr_subs;     /**< 子节点链表 */
} tree_t;

#endif // LIST_T_H
