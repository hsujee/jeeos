/*
 * @Author: Jee Hsu
 * @Description: I/O节点管理 - I/O请求对象
 *
 * 管理系统中的I/O请求对象，用于设备驱动和文件系统
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _OBJNODE_H
#define _OBJNODE_H

/**
 * @brief 初始化对象节点结构
 * @param initp 对象节点指针
 */
void objnode_t_init(objnode_t* initp);

/**
 * @brief 分配新的对象节点
 * @return 对象节点指针，失败返回NULL
 */
objnode_t* new_objnode();

/**
 * @brief 删除对象节点
 * @param onodep 要删除的对象节点指针
 * @return TRUE成功，FALSE失败
 */
bool_t del_objnode(objnode_t* onodep);

#endif // _OBJNODE_H
