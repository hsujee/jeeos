/*
 * @Author: Jee Hsu
 * @Description: VBE图形模式初始化
 *
 * 通过BIOS VBE(VESA BIOS Extensions)设置图形显示：
 * - 获取VBE模式列表(getvbemode)
 * - 设置目标分辨率(setvbemode)
 * - 显示启动Logo(bmp_print)
 *
 * 支持的显示模式：
 * - 分辨率: 1024x768或更高
 * - 颜色深度: 24bpp/32bpp
 * - 帧缓冲: 线性帧缓冲模式
 *
 * 像素格式：
 * - 32bpp: BGRA (Blue-Green-Red-Alpha)
 * - 24bpp: BGR (Blue-Green-Red)
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#include "graph.h"

void write_pixcolor(abootparam_t* mabp,u32_t x,u32_t y,pixl_t pix) {
    u8_t* p24bas;
    if(mabp->mb_ghparm.gh_onepixbits==24) {
        u32_t p24adr=(x+(y*mabp->mb_ghparm.gh_x))*3;
        p24bas=(u8_t*)(p24adr+mabp->mb_ghparm.gh_framphyadr);
        p24bas[0]=(u8_t)(pix);
        p24bas[1]=(u8_t)(pix>>8);
        p24bas[2]=(u8_t)(pix>>16);
        return;
    }
    u32_t* phybas=(u32_t*)mabp->mb_ghparm.gh_framphyadr;
    phybas[x+(y*mabp->mb_ghparm.gh_x)]=pix;

    return;
}

void bmp_print(void* bmfile,abootparam_t* mabp) {
    if(NULL==bmfile) {
        return;
    }
    pixl_t pix=0;
    bmdbgr_t* bpixp=NULL;
    bmfhead_t* bmphdp=(bmfhead_t*)bmfile;
    bitminfo_t* binfp= (bitminfo_t*)(bmphdp+1);
    u32_t img=(u32_t)bmfile+bmphdp->bf_off;
    bpixp=(bmdbgr_t*)img;
    int l=0;
    int k=0;
    int ilinebc = (((binfp->bi_w*24) + 31) >> 5) << 2;
    for(int y=639;y>=129;y--,l++) {
        k=0;
        for(int x=322;x<662;x++) {
            pix=BGRA(bpixp[k].bmd_r,bpixp[k].bmd_g,bpixp[k].bmd_b);
            write_pixcolor(mabp,x,y,pix);
            k++;
        }
        bpixp=(bmdbgr_t*)(((int)bpixp)+ilinebc);
    } 
    return;
}

// 显示logo图片
void logo(abootparam_t* mabp) {
    u32_t retadr=0,sz=0;
    // 从映像文件中获取logo.bmp文件
    get_file_rpadrandsz("logo.bmp",mabp,&retadr,&sz);
    if(0==retadr) {
        kerror("logo getfilerpadrsz err");
    }
    // 显示logo文件中的图像数据
    bmp_print((void*)retadr,mabp);

    return;
}

void init_graph(abootparam_t* mabp) {
    // 初始化图形数据结构
    graph_t_init(&mabp->mb_ghparm);
    // 初始化VBE显示参数
    init_bgadevice(mabp);
    if(mabp->mb_ghparm.gh_mode!=BGAMODE) {
        // 获取VBE模式，通过BIOS中断
        get_vbemode(mabp);
        // 获取一个具体VBE模式的信息，通过BIOS中断
        get_vbemodeinfo(mabp);
        // 设置VBE模式，通过BIOS中断
        set_vbemodeinfo();
    }
    init_kinitfvram(mabp);
    logo(mabp);
    return;
}

void graph_t_init(graph_t* initp) {
    memset(initp,0,sizeof(graph_t));
    return;
}

void init_kinitfvram(abootparam_t* mabp) {
    mabp->mb_fvrmphyadr=KINITFRVM_PHYADR;
    mabp->mb_fvrmsz=KINITFRVM_SZ;
    memset((void*)KINITFRVM_PHYADR,0,KINITFRVM_SZ);

    return;
}

u32_t vfartolineadr(u32_t vfar) {
    u32_t tmps=0,sec=0,off=0;
    off=vfar&0xffff;
    tmps=(vfar>>16)&0xffff;
    sec=tmps<<4;
    return (sec+off);
}

void get_vbemode(abootparam_t* mabp) {
    realadr_call_entry(RLINTNR(2),0,0);
    vbeinfo_t* vbeinfoptr=(vbeinfo_t*)VBEINFO_ADR;
    u16_t* mnm;

    if(vbeinfoptr->vbesignature[0]!='V'||
            vbeinfoptr->vbesignature[1]!='E'||
            vbeinfoptr->vbesignature[2]!='S'||
            vbeinfoptr->vbesignature[3]!='A') {
        kerror("vbe is not VESA");
    }
    printk("vbe vbever:%x\n",vbeinfoptr->vbeversion);
    if(vbeinfoptr->vbeversion<0x0200) {
        kerror("vbe version not vbe3");
    }

    if(vbeinfoptr->videomodeptr>0xffff) {
        mnm=(u16_t*)vfartolineadr(vbeinfoptr->videomodeptr);//
    } else {
        mnm=(u16_t*)(vbeinfoptr->videomodeptr);
    }

    int bm=0;
    for(int i=0;mnm[i]!=0xffff;i++) {
        if(mnm[i]==0x118) {
            bm=1;
        }
        if(i>0x1000) {
            break;
        }
    }

    if(bm==0) {
        kerror("getvbemode not 118");
    }
    mabp->mb_ghparm.gh_mode=VBEMODE;
    mabp->mb_ghparm.gh_vbemodenr=0x118;
    mabp->mb_ghparm.gh_vifphyadr=VBEINFO_ADR;
    m2mcopy(vbeinfoptr,&mabp->mb_ghparm.gh_vbeinfo,sizeof(vbeinfo_t));
    return;
}


void bga_write_reg(u16_t index, u16_t data) {
    out_u16(VBE_DISPI_IOPORT_INDEX, index);
    out_u16(VBE_DISPI_IOPORT_DATA, data);
    return;
}

u16_t bga_read_reg(u16_t index) {
    out_u16(VBE_DISPI_IOPORT_INDEX, index);
    return in_u16(VBE_DISPI_IOPORT_DATA);
}

u32_t get_bgadevice() {
    u16_t bgaid=bga_read_reg(VBE_DISPI_INDEX_ID);
    if(BGA_DEV_ID0<=bgaid&&BGA_DEV_ID5>=bgaid) {
        bga_write_reg(VBE_DISPI_INDEX_ID,bgaid);
        if(bga_read_reg(VBE_DISPI_INDEX_ID)!=bgaid) {
            return 0;
        }
        return (u32_t)bgaid;
    }
    return 0;
}

u32_t chk_bgamaxver() {
    bga_write_reg(VBE_DISPI_INDEX_ID,BGA_DEV_ID5);
    if(bga_read_reg(VBE_DISPI_INDEX_ID)==BGA_DEV_ID5) {
        return (u32_t)BGA_DEV_ID5;
    }
    bga_write_reg(VBE_DISPI_INDEX_ID,BGA_DEV_ID4);
    if(bga_read_reg(VBE_DISPI_INDEX_ID)==BGA_DEV_ID4) {
        return (u32_t)BGA_DEV_ID4;
    }
    bga_write_reg(VBE_DISPI_INDEX_ID,BGA_DEV_ID3);
    if(bga_read_reg(VBE_DISPI_INDEX_ID)==BGA_DEV_ID3) {
        return (u32_t)BGA_DEV_ID3;
    }
    bga_write_reg(VBE_DISPI_INDEX_ID,BGA_DEV_ID2);
    if(bga_read_reg(VBE_DISPI_INDEX_ID)==BGA_DEV_ID2) {
        return (u32_t)BGA_DEV_ID2;
    }
    bga_write_reg(VBE_DISPI_INDEX_ID,BGA_DEV_ID1);
    if(bga_read_reg(VBE_DISPI_INDEX_ID)==BGA_DEV_ID1) {
        return (u32_t)BGA_DEV_ID1;
    }
    bga_write_reg(VBE_DISPI_INDEX_ID,BGA_DEV_ID0);
    if(bga_read_reg(VBE_DISPI_INDEX_ID)==BGA_DEV_ID0) {
        return (u32_t)BGA_DEV_ID0;
    }
    return 0;
}

// 扫描 PCI 总线查找 VGA 兼容设备，获取其帧缓冲地址
static u32_t pci_find_vga_framebuffer() {
    for (u8_t bus = 0; bus < 2; bus++) {
        for (u8_t device = 0; device < 32; device++) {
            for (u8_t func = 0; func < 8; func++) {
                u32_t vendor_device = pci_read_config(bus, device, func, 0x00);
                u16_t vendor_id = (u16_t)(vendor_device & 0xFFFF);
                if (vendor_id == 0xFFFF) continue;
                
                u32_t class_rev = pci_read_config(bus, device, func, 0x08);
                u8_t class_code = (u8_t)((class_rev >> 24) & 0xFF);
                
                if (class_code == 0x03) {  // Display controller
                    u32_t bar0 = pci_read_config(bus, device, func, 0x10);
                    if ((bar0 & 0x01) == 0) {  // Memory BAR
                        u32_t framebuffer_addr = bar0 & 0xFFFFFFF0;
                        if (framebuffer_addr >= 0x10000000) {
                            return framebuffer_addr;
                        }
                    }
                }
            }
        }
    }
    return 0xE0000000;  // 默认地址
}

// 使用了 VBE 的 118h 模式，该模式下屏幕分辨率为 1024x768
// 显存大小是 16.8MB。显存开始地址通过 PCI 配置获取
// 即把屏幕分成 768 行，每行 1024 个像素点，但每个像素点占用显存的 32 位数据（4 字节，红、绿、蓝、透明各占 8 位）。
// 我们只要往对应的显存地址写入相应的像素数据，屏幕对应的位置就能显示了。
void init_bgadevice(abootparam_t* mabp) {
    u32_t retdevid=get_bgadevice();
    if(0==retdevid) {
        return;
    }
    retdevid=chk_bgamaxver();
    if(0==retdevid) {
        return;
    }
    bga_write_reg(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write_reg(VBE_DISPI_INDEX_XRES, 1024);
    bga_write_reg(VBE_DISPI_INDEX_YRES, 768);
    bga_write_reg(VBE_DISPI_INDEX_BPP, 0x20);
    bga_write_reg(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED|(VBE_DISPI_LFB_ENABLED)); 
    mabp->mb_ghparm.gh_mode=BGAMODE;
    mabp->mb_ghparm.gh_vbemodenr=retdevid;
    mabp->mb_ghparm.gh_x=1024;
    mabp->mb_ghparm.gh_y=768;
    // 通过 PCI 扫描获取正确的帧缓冲地址
    mabp->mb_ghparm.gh_framphyadr=pci_find_vga_framebuffer();
    mabp->mb_ghparm.gh_onepixbits=0x20;
    mabp->mb_ghparm.gh_bank=4;
    mabp->mb_ghparm.gh_curdipbnk=0;
    mabp->mb_ghparm.gh_nextbnk=0;
    mabp->mb_ghparm.gh_banksz=(mabp->mb_ghparm.gh_x*mabp->mb_ghparm.gh_x*4);
    //test_bga();
    return;
}

void test_bga() {
    int* p=(int*)(0xe0000000);
    int* p2=(int*)(0xe0000000+(1024*768*4));
    int* p3=(int*)(0xe0000000+(1024*768*4)*2);

    for(int i=0;i<(1024*768);i++) {
        p2[i]=0x00ff00ff;
    }
    for(int i=0;i<(1024*768);i++) {
        p[i]=0x0000ff00;
    }
    for(int i=0;i<(1024*768);i++) {
        p3[i]=0x00ff0000;
    }
    for(;;) {
        bga_write_reg(VBE_DISPI_INDEX_X_OFFSET,0);
        bga_write_reg(VBE_DISPI_INDEX_Y_OFFSET,0);
        bga_write_reg(VBE_DISPI_INDEX_VIRT_WIDTH,1024);
        bga_write_reg(VBE_DISPI_INDEX_VIRT_HEIGHT,768);
        die(0x400);
        bga_write_reg(VBE_DISPI_INDEX_X_OFFSET,0);
        bga_write_reg(VBE_DISPI_INDEX_Y_OFFSET,768);
        bga_write_reg(VBE_DISPI_INDEX_VIRT_WIDTH,1024);
        bga_write_reg(VBE_DISPI_INDEX_VIRT_HEIGHT,768*2);
        die(0x400);
        bga_write_reg(VBE_DISPI_INDEX_X_OFFSET,0);
        bga_write_reg(VBE_DISPI_INDEX_Y_OFFSET,768*2);
        bga_write_reg(VBE_DISPI_INDEX_VIRT_WIDTH,1024);
        bga_write_reg(VBE_DISPI_INDEX_VIRT_HEIGHT,768*3);
        die(0x400);
    }

    for(;;);
    return;
}

void get_vbemodeinfo(abootparam_t* mabp) {
    realadr_call_entry(RLINTNR(3),0,0);
    vbeominfo_t* vomif=(vbeominfo_t*)VBEMINFO_ADR;
    u32_t x=vomif->XResolution,y=vomif->YResolution;
    u32_t* phybass=(u32_t*)(vomif->PhysBasePtr);
    if(vomif->BitsPerPixel<24) {
        kerror("vomif->BitsPerPixel!=32");
    }
    if(x!=1024||y!=768) {
        kerror("xy not");
    }
    if((u32_t)phybass<0x100000) {
        kerror("phybass not");
    }
    mabp->mb_ghparm.gh_x=vomif->XResolution;
    mabp->mb_ghparm.gh_y=vomif->YResolution;
    mabp->mb_ghparm.gh_framphyadr=vomif->PhysBasePtr;
    mabp->mb_ghparm.gh_onepixbits=vomif->BitsPerPixel;
    mabp->mb_ghparm.gh_vmifphyadr=VBEMINFO_ADR;
    m2mcopy(vomif,&mabp->mb_ghparm.gh_vminfo,sizeof(vbeominfo_t));

    return;
}

void set_vbemodeinfo() {
    realadr_call_entry(RLINTNR(4),0,0);
    return;
}

u32_t utf8_to_unicode(utf8_t* utfp,int* retuib){
    u8_t uhd=utfp->utf_b1,ubyt=0;
    u32_t ucode=0,tmpuc=0;
    if(0x80>uhd) { //0xbf&&uhd<=0xbf
        ucode=utfp->utf_b1&0x7f;
        *retuib=1;
        return ucode;
    }
    if(0xc0<=uhd&&uhd<=0xdf) { //0xdf
        ubyt=utfp->utf_b1&0x1f;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b2&0x3f;
        ucode=(tmpuc<<6)|ubyt;
        *retuib=2;
        return ucode;
    }
    if(0xe0<=uhd&&uhd<=0xef) { //0xef
        ubyt=utfp->utf_b1&0x0f;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b2&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b3&0x3f;
        ucode=(tmpuc<<6)|ubyt;
        *retuib=3;
        return ucode;
    }
    if(0xf0<=uhd&&uhd<=0xf7) { //0xf7
        ubyt=utfp->utf_b1&0x7;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b2&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b3&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b4&0x3f;
        ucode=(tmpuc<<6)|ubyt;
        *retuib=4;
        return ucode;
    }
    if(0xf8<=uhd&&uhd<=0xfb) { //0xfb
        ubyt=utfp->utf_b1&0x3;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b2&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b3&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b4&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b5&0x3f;
        ucode=(tmpuc<<6)|ubyt;
        *retuib=5;
        return ucode;
    }
    if(0xfc<=uhd&&uhd<=0xfd) { //0xfd
        ubyt=utfp->utf_b1&0x1;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b2&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b3&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b4&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b5&0x3f;
        tmpuc<<=6;
        tmpuc|=ubyt;
        ubyt=utfp->utf_b6&0x3f;
        ucode=(tmpuc<<6)|ubyt;
        *retuib=6;
        return ucode;
    }
    *retuib=0;
    return 0;
}
