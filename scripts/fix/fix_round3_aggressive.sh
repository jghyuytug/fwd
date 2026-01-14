#!/bin/bash
# 第三轮修复: 激进注释策略
# 日期: 2025-10-29
# 目标: 修复102,944个错误

set -e

DECL_FILE="src_split/df_game_r_decl.h"

echo "[*] 第三轮激进修复开始..."
echo "[*] 目标文件: $DECL_FILE"
echo "[*] 当前错误数: 102,944"

# 备份
BACKUP_FILE="${DECL_FILE}.backup_round3_$(date +%Y%m%d_%H%M%S)"
cp "$DECL_FILE" "$BACKUP_FILE"
echo "[*] 已备份到: $BACKUP_FILE"

# 1. 扩展单引号修复 (通用模式)
echo "[*] 修复1: 清除所有 'to' 单引号模式..."
sed -i "s/ to'/ to /g" "$DECL_FILE"

# 2. 注释 keyed to 声明
echo "[*] 修复2: 注释所有 'keyed to' 声明..."
sed -i "s/^\(.*keyed to.*\)/\/\/ \1/" "$DECL_FILE"

# 3. 注释模板参数错误的 std::_Vector_base
echo "[*] 修复3: 注释单参数 std::_Vector_base..."
sed -i "s/^\(.*std::_Vector_base<[^,>]*>\)/\/\/ \1/" "$DECL_FILE"

# 4. 注释问题严重的 std::_Destroy 特化
echo "[*] 修复4: 注释 std::_Destroy 特化声明..."
sed -i "s/^\(.*void std::_Destroy<.*\)/\/\/ \1/" "$DECL_FILE"

# 5. 注释 global constructor/destructor
echo "[*] 修复5: 注释全局构造/析构函数..."
sed -i "s/^\(.*global constructor.*\)/\/\/ \1/" "$DECL_FILE"
sed -i "s/^\(.*global destructor.*\)/\/\/ \1/" "$DECL_FILE"

# 统计修复结果
echo ""
echo "[*] 修复完成!"
echo ""
echo "验证结果:"
KEYED_TO=$(grep -c "keyed to" "$DECL_FILE" || echo "0")
echo "  keyed to 剩余: $KEYED_TO"

SINGLE_PARAM_VECTOR=$(grep -c "std::_Vector_base<[^,>]*>" "$DECL_FILE" || echo "0")
echo "  单参数Vector剩余: $SINGLE_PARAM_VECTOR"

DESTROY_SPEC=$(grep -c "void std::_Destroy<" "$DECL_FILE" || echo "0")
echo "  _Destroy特化剩余: $DESTROY_SPEC"

echo ""
echo "备份文件: $BACKUP_FILE"
echo ""
echo "预期效果:"
echo "  修复前: 102,944 错误"
echo "  修复后: ~40,000 错误 (-61%)"
echo ""
echo "下一步: cd src_split && make clean && make -j12 all 2>&1 | tee ../compile_round4_\$(date +%Y%m%d_%H%M%S).log"
