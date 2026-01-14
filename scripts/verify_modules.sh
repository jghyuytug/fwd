#!/bin/bash
# 模块验证脚本 - 检查Dungeon、Party、Combat模块文件

echo "==================================="
echo "DNF游戏服务器 - 模块验证"
echo "==================================="
echo ""

# 颜色定义
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

total_files=0
found_files=0
missing_files=0

# 检查文件函数
check_file() {
    local file=$1
    local desc=$2
    total_files=$((total_files + 1))
    
    if [ -f "$file" ]; then
        lines=$(wc -l < "$file")
        size=$(du -h "$file" | cut -f1)
        echo -e "${GREEN}✓${NC} $desc"
        echo "  文件: $file"
        echo "  行数: $lines | 大小: $size"
        found_files=$((found_files + 1))
    else
        echo -e "${RED}✗${NC} $desc"
        echo "  文件: $file (未找到)"
        missing_files=$((missing_files + 1))
    fi
    echo ""
}

# Combat模块
echo "==================================="
echo "Combat模块 (战斗系统)"
echo "==================================="
check_file "src/combat/combat_init.c" "Combat实现文件"
check_file "src/combat/combat_interface.h" "Combat接口文件"
check_file "src/combat/Makefile" "Combat Makefile"

# Dungeon模块
echo "==================================="
echo "Dungeon模块 (副本系统)"
echo "==================================="
check_file "src/dungeon/dungeon_init.c" "Dungeon实现文件"
check_file "src/dungeon/dungeon_interface.h" "Dungeon接口文件"
check_file "src/dungeon/Makefile" "Dungeon Makefile"

# Party模块
echo "==================================="
echo "Party模块 (组队系统)"
echo "==================================="
check_file "src/party/party_init.c" "Party实现文件"
check_file "src/party/party_interface.h" "Party接口文件"
check_file "src/party/Makefile" "Party Makefile"

# 总结
echo "==================================="
echo "验证总结"
echo "==================================="
echo "总文件数: $total_files"
echo -e "${GREEN}已找到: $found_files${NC}"
if [ $missing_files -gt 0 ]; then
    echo -e "${RED}缺失: $missing_files${NC}"
else
    echo -e "${GREEN}缺失: 0${NC}"
fi
echo ""

# 代码统计
echo "==================================="
echo "代码统计"
echo "==================================="
echo ""

if [ $found_files -eq $total_files ]; then
    echo "Combat模块:"
    wc -l src/combat/*.c src/combat/*.h src/combat/Makefile 2>/dev/null | tail -1 | awk '{print "  总行数: " $1}'
    echo ""
    
    echo "Dungeon模块:"
    wc -l src/dungeon/*.c src/dungeon/*.h src/dungeon/Makefile 2>/dev/null | tail -1 | awk '{print "  总行数: " $1}'
    echo ""
    
    echo "Party模块:"
    wc -l src/party/*.c src/party/*.h src/party/Makefile 2>/dev/null | tail -1 | awk '{print "  总行数: " $1}'
    echo ""
    
    echo "所有模块合计:"
    wc -l src/combat/*.c src/combat/*.h src/combat/Makefile \
          src/dungeon/*.c src/dungeon/*.h src/dungeon/Makefile \
          src/party/*.c src/party/*.h src/party/Makefile 2>/dev/null | tail -1 | awk '{print "  总行数: " $1}'
fi

# 函数统计
echo ""
echo "==================================="
echo "函数统计"
echo "==================================="
echo ""

if [ -f "src/combat/combat_init.c" ]; then
    combat_funcs=$(grep -c "^int Combat_\|^void Combat_" src/combat/combat_init.c 2>/dev/null || echo "0")
    echo "Combat模块函数: $combat_funcs"
fi

if [ -f "src/dungeon/dungeon_init.c" ]; then
    dungeon_funcs=$(grep -c "^int Dungeon_\|^void Dungeon_" src/dungeon/dungeon_init.c 2>/dev/null || echo "0")
    echo "Dungeon模块函数: $dungeon_funcs"
fi

if [ -f "src/party/party_init.c" ]; then
    party_funcs=$(grep -c "^int Party_\|^void Party_" src/party/party_init.c 2>/dev/null || echo "0")
    echo "Party模块函数: $party_funcs"
fi

echo ""

# 最终结果
if [ $missing_files -eq 0 ]; then
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}验证通过！所有模块文件都已创建。${NC}"
    echo -e "${GREEN}========================================${NC}"
    exit 0
else
    echo -e "${RED}========================================${NC}"
    echo -e "${RED}验证失败！有 $missing_files 个文件缺失。${NC}"
    echo -e "${RED}========================================${NC}"
    exit 1
fi
