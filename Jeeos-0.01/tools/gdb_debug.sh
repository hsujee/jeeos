#!/bin/bash
###################################################################################################
# Jeeos-0.01 GDB 调试脚本
# 
# 用法:
#   1. 在一个终端运行: make qemudebug
#   2. 在另一个终端运行: ./tools/gdb_debug.sh
###################################################################################################

GDB_INIT_FILE="/tmp/jeeos_gdb_init"

# 创建GDB初始化脚本
cat << 'EOF' > ${GDB_INIT_FILE}
# Jeeos GDB 初始化脚本
target remote :1234

# 加载符号表
file build/JeeOS.elf

# 设置架构
set architecture i386

# 常用断点
# break main
# break _start

# 显示源代码
layout src

echo \n======================================\n
echo   Jeeos-0.01 GDB Debugger Ready\n
echo   常用命令:\n
echo   - b <func>  : 设置断点\n
echo   - c         : 继续执行\n
echo   - n         : 单步跳过\n
echo   - s         : 单步进入\n
echo   - bt        : 显示调用栈\n
echo   - p <var>   : 打印变量\n
echo   - info reg  : 显示寄存器\n
echo ======================================\n
EOF

echo "========================================"
echo "  Jeeos-0.01 GDB 调试器"
echo "========================================"
echo ""
echo "正在连接到 QEMU (端口 1234)..."
echo ""

gdb -x ${GDB_INIT_FILE}
