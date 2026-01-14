#!/bin/bash
# 批量测试键映射发现

PVF="$1"
TOOL="./auto_map_keys"

if [ -z "$PVF" ]; then
    echo "用法: $0 <pvf_file>"
    exit 1
fi

echo "========================================"
echo "批量键映射测试"
echo "========================================"
echo ""

# 测试已知的映射关系
echo "=== 测试已知映射 ==="
echo ""

echo "[1/10] 测试 [rarity] = 5 (史诗)"
$TOOL "$PVF" "[rarity]" 5 200 | grep "0x0000066C" | head -1
echo ""

echo "[2/10] 测试 [grade] = 70"
$TOOL "$PVF" "[grade]" 70 200 | grep "0x00004817" | head -1
echo ""

echo "[3/10] 测试 [minimum level] = 60"
$TOOL "$PVF" "[minimum level]" 60 200 | grep "0x0000481C" | head -1
echo ""

echo "[4/10] 测试 [physical attack] = 30"
$TOOL "$PVF" "[physical attack]" 30 200 | grep "0x00001265" | head -1
echo ""

echo "[5/10] 测试 [magical attack] = 30"
$TOOL "$PVF" "[magical attack]" 30 200 | grep "0x00001D0C" | head -1
echo ""

echo "[6/10] 测试 [anti evil] = 50"
$TOOL "$PVF" "[anti evil]" 50 200 | grep "0x0000487A" | head -1
echo ""

echo "[7/10] 测试 [physical critical hit] = 1"
$TOOL "$PVF" "[physical critical hit]" 1 200 | grep "0x00004961" | head -1
echo ""

echo "[8/10] 测试 [MP regen speed] = 10"
$TOOL "$PVF" "[MP regen speed]" 10 200 | grep "0x00001D11" | head -1
echo ""

echo "[9/10] 测试 [move speed] = 20"
$TOOL "$PVF" "[move speed]" 20 200 | grep "0x00004922" | head -1
echo ""

echo "[10/10] 测试 [cool time] = 10000"
$TOOL "$PVF" "[cool time]" 10000 200 | grep "0x0000482C" | head -1
echo ""

echo "========================================"
echo "测试完成"
echo "========================================"
