/*
 * @Author: Jee Hsu
 * @Description: 系统设置程序
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef BOOT_ENTRY_H
#define BOOT_ENTRY_H

#include "type.h"
#include "boot_types.h"
#include "bootparam_c.h"
#include "vgastr.h"
#include "io.h"

void setup_entry();

void kerror(char_t* kestr);
void die(u32_t dt);
#endif // BOOT_ENTRY_H
