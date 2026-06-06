/*
 * @Author: Jee Hsu
 * @Description: 图形显示模块
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _GRAPH_H
#define _GRAPH_H

#include "type.h"
#include "boot_types.h"
#include "setup_c.h"
#include "io.h"
#include "fs.h"
#include "param.h"

void init_graph(abootparam_t* mabp);
void graph_init(graph_t* initp);
u32_t vfartolineadr(u32_t vfar);
void init_kinitfvram(abootparam_t* mabp);
void get_vbemode(abootparam_t* mabp);
void get_vbemodeinfo(abootparam_t* mabp);
void set_vbemodeinfo();
void bga_write_reg(u16_t index, u16_t data);
u16_t bga_read_reg(u16_t index);
u32_t get_bgadevice();
u32_t chk_bgamaxver();
void init_bgadevice(abootparam_t* mabp);
u32_t utf8_to_unicode(utf8_t* utfp,int* retuib);

#endif
