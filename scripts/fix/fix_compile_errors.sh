#!/bin/bash
# 编译错误自动修复脚本
# 生成时间: 2025-10-29 16:28:51

set -e

echo "🔧 开始修复编译错误..."

# 备份原文件
echo "📦 备份 df_game_r_decl.h..."
cp src_split/df_game_r_decl.h src_split/df_game_r_decl.h.backup_$(date +%Y%m%d_%H%M%S)

# 1. 恢复STL内部函数声明
echo "🔄 恢复STL内部函数声明..."
sed -i 's|^// \(.*std::_.*\)|\1|' src_split/df_game_r_decl.h
sed -i 's|^// \(.*std::basic_.*\)|\1|' src_split/df_game_r_decl.h

# 2. 恢复模板特化声明
echo "🔄 恢复模板特化声明..."
sed -i 's|^// \(template<>.*\)|\1|' src_split/df_game_r_decl.h

# 3. 恢复Boost库声明
echo "🔄 恢复Boost库声明..."
sed -i 's|^// \(.*boost::.*\)|\1|' src_split/df_game_r_decl.h

# 4. 统计恢复结果
echo "📊 统计修复结果..."
echo "STL声明: $(grep -c 'std::_' src_split/df_game_r_decl.h || true)"
echo "模板特化: $(grep -c 'template<>' src_split/df_game_r_decl.h || true)"
echo "Boost声明: $(grep -c 'boost::' src_split/df_game_r_decl.h || true)"

echo "✅ 修复完成！"
echo "请运行: cd src_split && make clean && make -j12 all"
