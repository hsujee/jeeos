/*
 * @Author: Jee Hsu
 * @Description: imgundo模块
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "imgheads.h"

imgundo_t mundostc;
udoutf_t mudooutfile;
void init_imgundo() {
    imgundo_t_init(&mundostc);
    udoutf_t_init(&mudooutfile);
    return;
}

void exit_imgundo() {
    return;
}

void imgundo_t_init(imgundo_t *initp) {
    initp->iu_fd = -1;
    initp->iu_fsz = 0;
    initp->iu_fname = NULL;
    initp->iu_buf = NULL;
    initp->iu_bufsz = BFH_BUF_SZ;
    initp->iu_mdscbuf = NULL;
    initp->iu_mdscbufsz = BFH_BUF_SZ;
    initp->iu_mdscinfilepos_s = 0x1000;
    initp->iu_fhdnr = 0;
    initp->iu_fhdbuf = NULL;
    initp->iu_fhdbufsz = BFH_BUF_SZ;
    initp->iu_fhdrwnr = 0;
    initp->iu_fhdrwstus = 0;
    initp->iu_fhdinfilepos_s = 0;
    initp->iu_fhdinfilepos_c = 0;
    initp->iu_fhdinfilepos_e = 0;
    return;
}

void udoutf_t_init(udoutf_t *initp) {
    initp->uf_fd = -1;
    initp->uf_fsz = 0;
    initp->uf_fname = NULL;
    initp->uf_buf = NULL;
    initp->uf_bufsz = BFH_BUF_SZ;
    initp->uf_onerwsz = 0;
    initp->uf_currrwsz = 0;
    initp->uf_iudoscp = NULL;
    initp->uf_curfhdscp = NULL;
    return;
}

void img_undomain() {
    open_undofile();
    allocbuf_onimundo();
    read_mlosrddsc();
    img_udfidinfo_omlosrddsc();
    img_run_undoimg();
    close_undofile();
    return;
}

void img_udfidinfo_omlosrddsc() {
    mlosrddsc_t *mdcp = (mlosrddsc_t *)mundostc.iu_mdscbuf;
    mundostc.iu_fhdnr = mdcp->mdc_fhdnr;
    mundostc.iu_fhdinfilepos_s = mdcp->mdc_fhdbk_s;
    mundostc.iu_fhdinfilepos_c = mdcp->mdc_fhdbk_s;
    mundostc.iu_fhdinfilepos_e = mdcp->mdc_fhdbk_e;
    return;
}

void img_run_undoimg() {
    fhdsc_t *fhdp = img_undo_retnextfhdsc();
    while (fhdp != NULL) {
        //
        if (-1 == img_undoimg_onfhdsc(fhdp, &mudooutfile)) {
            img_error("undofile err");
        }
        fhdp = img_undo_retnextfhdsc();
    }
    return;
}

sint_t img_undoimg_onfhdsc(fhdsc_t *fhdp, udoutf_t *oufp) {
    if (fhdp == NULL || oufp == NULL) {
        img_error("no fhdp oufp");
        return -1;
    }
    alloc_undofile(fhdp, oufp);

    off_t ufs = fhdp->fhd_intsfsoff; 
    if (img_lseekfile(oufp->uf_iudoscp->iu_fd, ufs, SEEK_SET) == -1) {
        img_error("set leek");
        return -1;
    }
 
    for (;;) {
        if (oufp->uf_currrwsz >= oufp->uf_fsz) {
            break;
        }

        img_memclr(oufp->uf_buf, 0, oufp->uf_bufsz);

        ssize_t sz = img_readfile(oufp->uf_iudoscp->iu_fd, oufp->uf_buf, oufp->uf_onerwsz);
        if (sz == -1) {
            img_error("read fhdsc");
            return -1;
        }
        oufp->uf_currrwsz += sz;
        size_t wsz = sz;
        
        if (oufp->uf_currrwsz > oufp->uf_fsz) {
            wsz = oufp->uf_bufsz - (oufp->uf_currrwsz - oufp->uf_fsz);
        }
        
        if (img_writefile(oufp->uf_fd, oufp->uf_buf, wsz) == -1) {
            img_error("write file err");
        }
    }

    free_undofile(oufp);
    return 1;
}

fhdsc_t *img_undo_retnextfhdsc() {
    if (mundostc.iu_fhdrwnr < mundostc.iu_fhdnr) {
        if (undo_read_nextfhdsc(&mundostc) == -1) {
            img_error("read nextfhdsc");
            return NULL;
        }
        return (fhdsc_t *)mundostc.iu_fhdbuf;
    }
    return NULL;
}

sint_t undo_read_nextfhdsc(imgundo_t *iudp) {
    off_t off = img_lseekfile(iudp->iu_fd, iudp->iu_fhdinfilepos_c, SEEK_SET);
    if (off == -1) {
        img_error("set leek");
        return -1;
    }
    img_memclr(iudp->iu_fhdbuf, 0, iudp->iu_fhdbufsz);
    ssize_t sz = img_readfile(iudp->iu_fd, iudp->iu_fhdbuf, sizeof(fhdsc_t));
    if (sz < (ssize_t)(sizeof(fhdsc_t))) {
        img_error("read fhdsc");
        return -1;
    }
    off = img_lseekfile(iudp->iu_fd, 0, SEEK_CUR);
    if (off == -1) {
        img_error("set leek");
        return -1;
    }
    iudp->iu_fhdinfilepos_c = off;
    iudp->iu_fhdrwnr++;
    return (sint_t)sz;
}

void alloc_undofile(fhdsc_t *fhdscp, udoutf_t *oufp) {
    if (fhdscp == NULL || oufp == NULL) {
        img_error("no fhdp oufp");
        return;
    }
    void *buf = img_mem(oufp->uf_bufsz, NULL, MFLG_ALLOC);
    if (buf == NULL) {
        img_error("img_mem");
        return;
    }
    int fd = img_newfile(fhdscp->fhd_name, O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (fd == -1) {
        if (img_mem(0, buf, MFLG_FREE) == NULL) {
            img_error("img_mem");
            return;
        }
        img_error("new file");
        return;
    }
    oufp->uf_fd = fd;
    oufp->uf_fname = fhdscp->fhd_name;
    oufp->uf_fsz = fhdscp->fhd_frealsz;
    oufp->uf_curfhdscp = fhdscp;
    oufp->uf_iudoscp = &mundostc;

    oufp->uf_buf = buf;
    oufp->uf_onerwsz = oufp->uf_bufsz;
    return;
}

void free_undofile(udoutf_t *oufp) {
    if (img_closefile(oufp->uf_fd) == -1) {
        img_error("close file");
        return;
    }
    if (img_mem(0, oufp->uf_buf, MFLG_FREE) == NULL) {
        img_error("img_mem");
        return;
    }
    udoutf_t_init(oufp);
    return;
}

int sum_mlorddsc(mlosrddsc_t *mrddcp) {
    //JEEOSMDSK
    char *strmgic = (char *)mrddcp;
    u64_t sum = 0;

    if (strmgic[0] == 'J' && strmgic[1] == 'E' && strmgic[2] == 'E' &&
        strmgic[3] == 'O' && strmgic[4] == 'S' && strmgic[5] == 'M' &&
        strmgic[6] == 'D' && strmgic[7] == 'S' && strmgic[8] == 'K') {
        if (mrddcp->mdc_endgic != MDC_ENDGIC || mrddcp->mdc_rv != MDC_RVGIC) {
            return -1;
        }
        sum = mrddcp->mdc_sfsum;
        mrddcp->mdc_sfsum = 0;

        if (sum != computer_sum((void *)mrddcp, sizeof(mlosrddsc_t))) {
            img_error("sum err");
            return -1;
        }
        mrddcp->mdc_sfsum = sum;
        return 1;
    }
    return -1;
}

void read_mlosrddsc() {
    if (mundostc.iu_fd == -1 || mundostc.iu_fsz < 0x1000 ||
        mundostc.iu_mdscinfilepos_s != 0x1000) {
        img_error("no undofile");
        return;
    }
    if (mundostc.iu_buf == NULL || mundostc.iu_mdscbuf == NULL
        || mundostc.iu_bufsz < BFH_BUF_SZ || mundostc.iu_mdscbufsz < BFH_BUF_SZ) {
        img_error("no undofile");
        return;
    }
    if (img_lseekfile(mundostc.iu_fd, 0x1000, SEEK_SET) != 0x1000) {
        img_error("lseek file");
        return;
    }
    if (img_readfile(mundostc.iu_fd, mundostc.iu_buf, mundostc.iu_bufsz) == -1) {
        img_error("read file");
        return;
    }

    img_memcpy(mundostc.iu_mdscbuf, mundostc.iu_buf, mundostc.iu_mdscbufsz);
    if (sum_mlorddsc((mlosrddsc_t *)mundostc.iu_mdscbuf) == -1) {
        img_error("sum file");
        return;
    }

    return;
}

void allocbuf_onimundo() {
    void *buf = img_mem(mundostc.iu_bufsz, NULL, MFLG_ALLOC);
    if (buf == NULL) {
        img_error("img_mem");
        return;
    }
    mundostc.iu_buf = buf;
    img_memclr(mundostc.iu_buf, 0, mundostc.iu_bufsz);
    buf = img_mem(mundostc.iu_mdscbufsz, NULL, MFLG_ALLOC);
    if (buf == NULL) {
        img_error("img_mem");
        return;
    }
    mundostc.iu_mdscbuf = buf;
    img_memclr(mundostc.iu_mdscbuf, 0, mundostc.iu_mdscbufsz);
    buf = img_mem(mundostc.iu_fhdbufsz, NULL, MFLG_ALLOC);
    if (buf == NULL) {
        img_error("img_mem");
        return;
    }
    mundostc.iu_fhdbuf = buf;
    img_memclr(mundostc.iu_fhdbuf, 0, mundostc.iu_fhdbufsz);
    return;
}

void open_undofile() {
    char *pna = img_retnext_ipathname();
    if (pna == NULL) {
        img_error("on file");
        return;
    }
    int fd = img_openfile(pna, O_RDWR);
    if (fd == -1) {
        img_error("openfile");
        return;
    }
    mundostc.iu_fsz = img_retszfile(pna);
    mundostc.iu_fname = pna;
    mundostc.iu_fd = fd;
    return;
}

void close_undofile() {
    if (img_closefile(mundostc.iu_fd) == -1) {
        img_error("openfile");
        return;
    }
    return;
}
