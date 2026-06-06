/*
 * @Author: Jee Hsu
 * @Description: buildfile模块头文件
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _BUILDFILE_H
#define _BUILDFILE_H
#define BUILD_CORE_OBJS buildtool.o imgcore.o imgmgrhead.o param.o file.o\
    imgundo.o memdisk.o mem.o limgerror.o
#define BUILD_IMG_LINK BUILD_CORE_OBJS
#define BUILD_IMG_EBIN buildtool

#endif // _BUILDFILE_H
