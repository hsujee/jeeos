#!/bin/bash
###################################################################################################
# Jeeos-0.11 GDB 调试脚本
# 
# 用法:
#   1. 在一个终端运行: make qemudebug
#   2. 在另一个终端运行: make gdbrun 或 ./tools/gdb_debug.sh
###################################################################################################

# 获取脚本所在目录，然后切换到项目根目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "${SCRIPT_DIR}")"
cd "${PROJECT_ROOT}"

GDB_INIT_FILE="/tmp/jeeos_gdb_init"
ELF_FILE="${PROJECT_ROOT}/build/jeeos.elf"

# 检查 ELF 文件是否存在
if [ ! -f "${ELF_FILE}" ]; then
    echo "错误: 找不到符号文件 ${ELF_FILE}"
    echo "请先运行 'make build' 编译内核"
    exit 1
fi

# 创建GDB初始化脚本
cat << EOF > ${GDB_INIT_FILE}
# Jeeos GDB 初始化脚本
target remote :1234

# 加载符号表 (使用绝对路径)
file ${ELF_FILE}

# 设置架构
set architecture i386:x86-64

# 常用断点
# break start_kernel
# break init_arch
# break init_kernel

# 显示源代码
layout src

echo \n======================================\n
echo   Jeeos-0.11 GDB Debugger Ready\n
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
echo "  Jeeos-0.11 GDB 调试器"
echo "========================================"
echo ""
echo "符号文件: ${ELF_FILE}"
echo "正在连接到 QEMU (端口 1234)..."
echo ""

gdb -x ${GDB_INIT_FILE}
