#!/bin/bash
# DNF服务器配置文件提取脚本
# 生成时间: 2025-11-11

OUTPUT_DIR="/tmp/config_export_$(date +%Y%m%d_%H%M%S)"

echo "=========================================="
echo "DNF 配置文件提取工具"
echo "=========================================="
echo ""

mkdir -p "$OUTPUT_DIR"
cd "$OUTPUT_DIR"

echo "[1/5] 检查服务运行状态..."
ps aux | grep -E 'df_|mysql' | grep -v grep > running_processes.txt
echo "✓ 进程列表已保存"
cat running_processes.txt

echo ""
echo "[2/5] 检查监听端口..."
netstat -tlnp 2>/dev/null | grep -E 'df_|mysql' > listening_ports.txt
if [ ! -s listening_ports.txt ]; then
    ss -tlnp | grep -E 'df_|mysql' > listening_ports.txt
fi
echo "✓ 端口列表已保存"
cat listening_ports.txt

echo ""
echo "[3/5] 提取配置文件..."
mkdir -p configs

# MySQL配置
if [ -f /etc/my.cnf ]; then
    cp /etc/my.cnf configs/
    echo "✓ /etc/my.cnf"
fi

if [ -f /etc/mysql/my.cnf ]; then
    cp /etc/mysql/my.cnf configs/mysql_my.cnf
    echo "✓ /etc/mysql/my.cnf"
fi

# DNF服务配置
if [ -d /home/neople ]; then
    echo "✓ 找到 /home/neople 目录"

    # 查找所有.cfg文件
    find /home/neople -name "*.cfg" -type f > cfg_file_list.txt

    # 复制配置文件（保持目录结构）
    while IFS= read -r cfg_file; do
        # 提取相对路径
        rel_path=${cfg_file#/home/neople/}
        target_dir=$(dirname "configs/neople/$rel_path")

        mkdir -p "$target_dir"
        cp "$cfg_file" "configs/neople/$rel_path"
        echo "  复制: $cfg_file"
    done < cfg_file_list.txt

    echo "✓ DNF配置文件已复制"
else
    echo "⚠ /home/neople 目录不存在"
fi

# 启动脚本
if [ -f /root/run ]; then
    cp /root/run configs/
    echo "✓ /root/run (启动脚本)"
fi

echo ""
echo "[4/5] 提取目录结构..."
if [ -d /home/neople ]; then
    tree -L 3 /home/neople > neople_directory_tree.txt 2>/dev/null
    if [ ! -s neople_directory_tree.txt ]; then
        # 如果没有tree命令，使用find
        find /home/neople -maxdepth 3 -type d | sort > neople_directory_tree.txt
    fi
    echo "✓ 目录树已保存"
fi

echo ""
echo "[5/5] 生成服务器信息报告..."
cat > server_info.txt <<EOF
================================================================================
DNF 服务器配置信息报告
================================================================================
提取时间: $(date)
主机名: $(hostname)
系统: $(cat /etc/redhat-release 2>/dev/null || cat /etc/os-release 2>/dev/null | head -1)
内核: $(uname -r)
架构: $(uname -m)

--------------------------------------------------------------------------------
1. 网络配置
--------------------------------------------------------------------------------
IP地址:
$(ip addr show 2>/dev/null || ifconfig 2>/dev/null)

--------------------------------------------------------------------------------
2. 服务端口
--------------------------------------------------------------------------------
$(cat listening_ports.txt)

--------------------------------------------------------------------------------
3. 运行进程
--------------------------------------------------------------------------------
$(cat running_processes.txt)

--------------------------------------------------------------------------------
4. 配置文件列表
--------------------------------------------------------------------------------
$(find configs -type f | sort)

--------------------------------------------------------------------------------
5. MySQL状态
--------------------------------------------------------------------------------
$(mysqladmin -u root -p88888888 status 2>/dev/null)

EOF

echo "✓ 服务器信息报告已生成"

echo ""
echo "=========================================="
echo "✓ 配置提取完成！"
echo "=========================================="
echo ""
echo "输出目录: $OUTPUT_DIR"
echo ""
echo "导出文件："
echo "  - configs/              (所有配置文件)"
echo "  - running_processes.txt (运行进程)"
echo "  - listening_ports.txt   (监听端口)"
echo "  - server_info.txt       (服务器信息)"
echo ""
echo "打包命令："
echo "  tar -czf config_export.tar.gz *"
echo ""
