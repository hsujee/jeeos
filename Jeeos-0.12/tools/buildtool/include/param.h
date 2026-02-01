/*
 * @Author: Jee Hsu
 * @Description: param模块头文件
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _PARAM_H
#define _PARAM_H
#define KRNEL_MODE 1
#define BOOT_MODE 2
#define LDSK_MODE 3
#define UNDO_MODE 4
typedef struct s_mparam {
    int mp_argc;
    char **mp_argv;
    uint_t mp_sifnr;
    uint_t mp_eifnr;
    uint_t mp_sofnr;
    uint_t mp_ifcurrnr;
    uint_t mp_ildrhnr;
    uint_t mp_imgmode;
    uint_t mp_ifnr;
} mparam_t;

void init_param();
void exit_param();
void mparam_t_init(mparam_t *initp);
void img_param(int argc, char *argv[]);
void img_param_is_inputfile();
void img_param_is_outfile();
void img_param_is_ldhfile();
void img_param_is_mode();
uint_t img_ret_allfilblk();
uint_t img_ret_allifimglen();
uint_t img_ret_allinfilesz();
char *img_retnext_ipathname();
char *img_ret_ldrhpathname();
char *img_retnext_opathname();
uint_t img_ret_infilenr();
uint_t img_ret_imgmode();
#endif
