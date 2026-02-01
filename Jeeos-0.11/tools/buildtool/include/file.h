/*
 * @Author: Jee Hsu
 * @Description: file模块头文件
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _FILE_H
#define _FILE_H
void init_file();
void exit_file();
int img_newfile(const char *pathname, int flags, mode_t mode);
int img_openfile(const char *pathname, int flags);
int img_closefile(int fd);
ssize_t img_readfile(int fd, void *buf, size_t count);
ssize_t img_writefile(int fd, const void *buf, size_t count);
off_t img_lseekfile(int fd, off_t offset, int whence);
uint_t img_retszfile(const char *pathname);
sint_t img_retfcurpos(int fd);
uint_t img_createfile_setval(char* file,size_t blksz,uint_t blknr,int setval);
#endif
