/*
 * @Author: Jee Hsu
 * @Description: imgcore模块头文件
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _IMGCORE_H
#define _IMGCORE_H
void init_imgcore();
void exit_imgcore();
void img_core();
void img_boot_mode_run();
void img_krnl_mode_run();
void img_ldsk_mode_run();
void img_undo_mode_run();
void img_core_in_mode_run(uint_t mode);
sint_t img_write_mlosrddsc();
sint_t img_write_ldrheadfile();
int img_write_imginitblk();
void img_config_subzn(fzone_t* fznp,uint_t fstartpos,uint_t fcurrepos,uint_t fendpos);
void img_config_fzone();
void kankan();
void img_write_bldrzn();
void img_write_mftlzn();
void img_write_filezn();
void img_set_mlosrddsccurrpos(binfhead_t* bfhp);
void img_upd_mlosrddsccurrpos(binfhead_t* bfhp);
void img_set_ldrfilecurrpos(binfhead_t* bfhp);
void img_upd_ldrfilecurrpos(binfhead_t* bfhp);
void img_set_infilecurrpos(binfhead_t* bfhp);
uint_t img_ret_infilecurrpos();
void img_upd_infilecurrpos(binfhead_t* bfhp);
void img_set_fheadcurrpos(binfhead_t* bfhp);
void img_upd_fheadcurrpos(binfhead_t* bfhp);
int img_rw_bldr_file(binfhead_t* ibfhp,binfhead_t* obfhp);
int img_rw_one_file(binfhead_t* ibfhp,binfhead_t* obfhp);
sint_t run_rw_func_onldfile(char* pathname);
sint_t run_rw_func_oninfile(char* pathname);
void del_outimg_file();
void new_outimg_file();
int alloc_new_inputfile(char* pathname,binfhead_t* inpbfhp);
uint_t computer_sum(void* buf,uint_t sz);
int free_new_inputfile(binfhead_t* inpbfhp);
void read_imgfile_to_buf(binfhead_t* bfhdp);
void copy_infbuf_to_oufbuf(binfhead_t* ibfhdp,binfhead_t* obfhdp);
void write_imgfile_fr_buf(binfhead_t* bfhdp);
int open_newoutimgfile(char* pathname);
#endif
