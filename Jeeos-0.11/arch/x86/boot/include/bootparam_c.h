/*
 * @Author: Jee Hsu
 * @Description: 引导参数处理
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef BOOT_INIT_H
#define BOOT_INIT_H

#include "type.h"
#include "boot_types.h"
#include "setup_c.h"
#include "cpumem.h"
#include "fs.h"
#include "graph.h"

void init_bootparam();
void init_abootparam_t(abootparam_t* initp);
int adrzone_is_ok(u64_t sadr,u64_t slen,u64_t kadr,u64_t klen);
int chkadr_is_ok(abootparam_t* mabp,u64_t chkadr,u64_t cksz);
#endif // BOOT_INIT_H
