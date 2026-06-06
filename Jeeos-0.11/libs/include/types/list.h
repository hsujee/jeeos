/*
 * @Author: Jee Hsu
 * @Description: 双向链表实现 - Linux风格链表
 *
 * 提供侵入式双向链表操作，节点嵌入到用户结构中
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _LIST_H
#define _LIST_H

/**
 * @brief 初始化链表头
 * @param list 链表头指针
 */
KLINE void list_init(list_h_t *list) {
	list->prev = list;
	list->next = list;
	return;
}

/**
 * @brief 删除两节点之间的节点(内部函数)
 * @param prev 前节点
 * @param next 后节点
 */
KLINE void __list_del(list_h_t *prev, list_h_t *next) {
	next->prev = prev;
	prev->next = next;
	return;
}

/**
 * @brief 在两节点之间插入新节点(内部函数)
 * @param new 新节点
 * @param prev 前节点
 * @param next 后节点
 */
KLINE void __list_add(list_h_t *new, list_h_t *prev, list_h_t *next) {
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
	return;
}

/**
 * @brief 在头部添加节点
 * @param new 新节点
 * @param head 链表头
 */
KLINE void list_add(list_h_t *new, list_h_t *head) {
	__list_add(new, head, head->next);
	return;
}

/**
 * @brief 在尾部添加节点
 * @param new 新节点
 * @param head 链表头
 */
KLINE void list_add_tail(list_h_t *new, list_h_t *head) {
	__list_add(new, head->prev, head);
	return;
}

/**
 * @brief 删除节点(内部函数)
 * @param entry 要删除的节点
 */
KLINE void __list_del_entry(list_h_t *entry) {
	__list_del(entry->prev, entry->next);
	return;
}

/**
 * @brief 删除并重新初始化节点
 * @param entry 要删除的节点
 */
KLINE void list_del(list_h_t *entry) {
	__list_del(entry->prev, entry->next);
	list_init(entry);
	return;
}

/**
 * @brief 移动节点到头部
 * @param list 要移动的节点
 * @param head 链表头
 */
KLINE void list_move(list_h_t *list, list_h_t *head) {
	list_del(list);
	list_add(list, head);
	return;
}

/**
 * @brief 移动节点到尾部
 * @param list 要移动的节点
 * @param head 链表头
 */
KLINE void list_move_tail(list_h_t *list, list_h_t *head) {
	list_del(list);
	list_add_tail(list, head);
	return;
}

/**
 * @brief 检查链表是否为空
 * @param head 链表头
 * @return TRUE为空，FALSE不为空
 */
KLINE bool_t list_is_empty(const list_h_t *head) {
	if (head->next == head) {
		return TRUE;
	}
	return FALSE;
}

/**
 * @brief 检查是否为第一个节点
 * @param list 节点
 * @param head 链表头
 * @return TRUE是第一个，FALSE不是
 */
KLINE bool_t list_is_first(const list_h_t* list, const list_h_t* head) {
	if(list->prev == head) {
		return TRUE;
	}
	return FALSE;
}

/**
 * @brief 检查是否为最后一个节点
 * @param list 节点
 * @param head 链表头
 * @return TRUE是最后一个，FALSE不是
 */
KLINE bool_t list_is_last(const list_h_t* list, const list_h_t* head) {
	if(list->next == head) {
		return TRUE;
	}
	return FALSE;
}

/**
 * @brief 仔细检查链表是否为空
 * @param head 链表头
 * @return TRUE为空，FALSE不为空
 */
KLINE bool_t list_is_empty_careful(const list_h_t *head) {
	list_h_t *next = head->next;
	if (next == head && next == head->prev) {
		return TRUE;
	}
	return FALSE;
}

/** @brief 遍历链表 */
#define list_for_each(pos, head) for (pos = (head)->next; pos != (head); pos = pos->next)

/** @brief 遍历链表(可删除当前节点) */
#define list_for_each_head_dell(pos, head) for (pos = (head)->next; pos != (head); pos = (head)->next)

/**
 * @brief 通过链表节点获取包含结构
 * @param ptr 链表节点指针
 * @param type 包含结构类型
 * @param member 链表节点在结构中的成员名
 */
#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

/** @brief 获取链表第一个对象 */
#define list_first_oneobj(head, o_type, o_member) list_entry((head)->next, o_type, o_member)

/** @brief 获取下一个对象 */
#define list_next_entry(pos, type, member) \
	list_entry((pos)->member.next, type, member)

/** @brief 获取前一个对象 */
#define list_prev_entry(pos, type, member) \
	list_entry((pos)->member.prev, type, member)

#endif // _LIST_H
