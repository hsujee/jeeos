/*
 * @Author: Jee Hsu
 * @Description: 引导扇区程序
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef BOOT_HEAD_H
#define BOOT_HEAD_H

#include "type.h"
#include "boot_types.h"
#include "io.h"
#include "vgastr.h"


void head_entry();
void write_bootparam();
void write_setup();
fhdsc_t* find_file(char_t* fname);
int strcmpl(const char *a,const char *b);
void error(char_t* estr);
#endif // BOOT_HEAD_H
