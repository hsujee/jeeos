#!/bin/bash
###################################################################################################
# Jeeos-0.12 虚拟硬盘创建脚本
# 
# 功能: 创建带GRUB引导的虚拟硬盘镜像
# 用法: ./mk_hdisk.sh
###################################################################################################

set -e

# 配置变量
H_DISK="../hdisk"
H_IMAGE_PATH=../build/vbox/hd.img
GRUB_CFG_PATH=${H_DISK}/boot/grub/grub.cfg
DISK_SIZE=204800  # 100MB (512 * 204800)

echo "========================================"
echo "  Jeeos-0.12 虚拟硬盘创建工具"
echo "========================================"

# 如果虚拟硬盘已存在，跳过创建
if [ -f "${H_IMAGE_PATH}" ]; then
    echo "[INFO] 虚拟硬盘已存在: ${H_IMAGE_PATH}"
    echo "[INFO] 跳过创建步骤"
    mkdir -p ${H_DISK}
    echo "========================================"
    exit 0
fi

# 清理旧的挂载点
if mountpoint -q ${H_DISK} 2>/dev/null; then
    echo "[CLEAN] 卸载旧挂载点..."
    sudo umount ${H_DISK}
fi

# 创建目录
mkdir -p ../build/vbox
rm -rf ${H_DISK}
mkdir -p ${H_DISK}

# 获取可用的loop设备
LOOP_PATH=$(sudo losetup -f)
echo "[INFO] 可用loop设备: ${LOOP_PATH}"

# 创建空白磁盘镜像
echo "[DISK] 创建 100MB 虚拟硬盘..."
dd bs=512 if=/dev/zero of=${H_IMAGE_PATH} count=${DISK_SIZE} 2>/dev/null

# 设置loop设备
sudo losetup ${LOOP_PATH} ${H_IMAGE_PATH}
echo "[DISK] 挂载为块设备: ${LOOP_PATH}"

# 格式化为EXT4
echo "[DISK] 格式化为 EXT4..."
sudo mkfs.ext4 -q ${LOOP_PATH}

# 挂载硬盘 (使用已创建的loop设备)
sudo mount ${LOOP_PATH} ${H_DISK}
echo "[DISK] 挂载到 ${H_DISK}"

# 创建boot目录
sudo mkdir -p ${H_DISK}/boot

# 安装GRUB
echo "[GRUB] 安装 GRUB 引导程序..."
sudo grub-install --boot-directory=${H_DISK}/boot --force --allow-floppy --target=i386-pc ${LOOP_PATH} 2>/dev/null

# 创建GRUB配置
echo "[GRUB] 写入 GRUB 配置..."
sudo tee ${GRUB_CFG_PATH} > /dev/null << "EOF"
menuentry 'Jeeos-0.12' {
    insmod part_msdos
    insmod ext2
    set root='hd0'
    multiboot /boot/jeeos.eki
    boot
}
set timeout_style=menu
set timeout=5
EOF

sudo chmod 644 ${GRUB_CFG_PATH}

# 清理：先卸载再释放loop设备
sudo umount ${H_DISK}
sudo losetup -d ${LOOP_PATH}

echo "[DONE] 虚拟硬盘创建完成: ${H_IMAGE_PATH}"
echo "========================================"
