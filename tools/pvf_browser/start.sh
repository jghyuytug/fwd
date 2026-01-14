#!/bin/bash
# DNF装备浏览器启动脚本 (Linux/macOS)

echo "========================================"
echo "DNF装备浏览器"
echo "========================================"
echo ""

# 检查后端是否已编译
if [ ! -f "backend/pvf_api" ]; then
    echo "[错误] 找不到backend/pvf_api"
    echo "请先编译后端:"
    echo "  cd backend"
    echo "  make"
    echo ""
    exit 1
fi

# 检查Python
if ! command -v python3 &> /dev/null; then
    echo "[错误] 找不到Python 3"
    echo "请先安装Python 3.6+"
    echo ""
    exit 1
fi

echo "[启动] 正在启动Web服务器..."
echo ""

# 启动服务器
python3 server.py
