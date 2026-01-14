#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Enhancement System Data Analysis
从生产数据库提取真实的强化/增幅参数

数据来源: taiwan_cain_log.log_item_upgrade 表

字段说明:
- upgrade: 强化等级 (0到最大等级)
- amplify_item_cnt: 增幅物品数量
- rarity: 稀有度 (0-5: Common, Uncommon, Rare, Epic, Unique, Legendary)
- level: 物品等级
- item_cnt: 物品数量
"""

import pymysql
import sys
from collections import defaultdict, Counter

# 数据库连接配置
DB_CONFIG = {
    'host': '192.168.200.131',  # 生产服务器IP
    'port': 3306,
    'user': 'root',
    'password': '88888888',  # 从config/database.cfg提取
    'database': 'taiwan_cain_log',
    'charset': 'utf8'  # 旧版MySQL使用utf8
}

# 稀有度名称
RARITY_NAMES = {
    0: 'Common',
    1: 'Uncommon',
    2: 'Rare',
    3: 'Epic',
    4: 'Unique',
    5: 'Legendary'
}

def connect_database():
    """连接到生产数据库"""
    try:
        conn = pymysql.connect(**DB_CONFIG)
        print(f"[OK] 成功连接到数据库: {DB_CONFIG['host']}:{DB_CONFIG['database']}\n")
        return conn
    except Exception as e:
        print(f"[ERROR] 数据库连接失败: {e}")
        print("\n请配置正确的数据库连接信息:")
        print("  - 编辑 DB_CONFIG['user']")
        print("  - 编辑 DB_CONFIG['password']")
        print("  - 确认服务器IP: 192.168.200.131\n")
        return None

def analyze_upgrade_levels(conn):
    """
    分析强化等级分布

    查询所有log_item_upgrade记录，统计：
    1. 最大强化等级
    2. 按稀有度的最大强化等级
    3. 强化等级分布（直方图）
    """
    print("=" * 80)
    print("📊 强化等级分析 (Strengthen Level Analysis)")
    print("=" * 80)

    cursor = conn.cursor()

    # 查询1: 全局最大强化等级
    query = """
        SELECT MAX(upgrade) as max_level,
               MIN(upgrade) as min_level,
               AVG(upgrade) as avg_level,
               COUNT(*) as total_records
        FROM log_item_upgrade
        WHERE upgrade > 0
    """

    cursor.execute(query)
    result = cursor.fetchone()

    max_level, min_level, avg_level, total_records = result

    print(f"\n全局统计:")
    print(f"  - 最大强化等级: +{max_level} ⭐")
    print(f"  - 最小强化等级: +{min_level}")
    print(f"  - 平均强化等级: +{avg_level:.2f}")
    print(f"  - 总记录数: {total_records:,}\n")

    # 查询2: 按稀有度的最大强化等级
    query_by_rarity = """
        SELECT rarity,
               MAX(upgrade) as max_level,
               COUNT(*) as count
        FROM log_item_upgrade
        WHERE upgrade > 0
        GROUP BY rarity
        ORDER BY rarity
    """

    cursor.execute(query_by_rarity)
    results = cursor.fetchall()

    print("按稀有度的最大强化等级:")
    print(f"  {'稀有度':<15} {'最大等级':<10} {'记录数':<15}")
    print("  " + "-" * 40)

    max_by_rarity = {}
    for rarity, max_lvl, count in results:
        rarity_name = RARITY_NAMES.get(rarity, f'Unknown({rarity})')
        print(f"  {rarity_name:<15} +{max_lvl:<9} {count:>10,}")
        max_by_rarity[rarity] = max_lvl

    # 查询3: 强化等级分布（直方图）
    query_distribution = """
        SELECT upgrade, COUNT(*) as count
        FROM log_item_upgrade
        WHERE upgrade > 0
        GROUP BY upgrade
        ORDER BY upgrade
    """

    cursor.execute(query_distribution)
    results = cursor.fetchall()

    print(f"\n强化等级分布（直方图）:")
    print(f"  {'等级':<8} {'数量':<15} {'百分比':<10} {'图表'}")
    print("  " + "-" * 60)

    for level, count in results:
        percentage = (count / total_records) * 100
        bar_length = int(percentage / 2)  # 每2%一个字符
        bar = '█' * bar_length
        print(f"  +{level:<7} {count:>10,}   {percentage:>6.2f}%   {bar}")

    cursor.close()

    return {
        'max_level': max_level,
        'max_by_rarity': max_by_rarity,
        'avg_level': avg_level
    }

def analyze_amplify_data(conn):
    """
    分析增幅数据

    查询增幅物品数量分布
    """
    print("\n\n" + "=" * 80)
    print("📊 增幅系统分析 (Amplify System Analysis)")
    print("=" * 80)

    cursor = conn.cursor()

    # 查询: 增幅物品数量统计
    query = """
        SELECT amplify_item_cnt,
               COUNT(*) as count,
               AVG(upgrade) as avg_upgrade_level
        FROM log_item_upgrade
        WHERE amplify_item_cnt > 0
        GROUP BY amplify_item_cnt
        ORDER BY amplify_item_cnt
    """

    cursor.execute(query)
    results = cursor.fetchall()

    if not results:
        print("\n⚠️  数据库中没有增幅记录\n")
        cursor.close()
        return {}

    print(f"\n增幅物品数量分布:")
    print(f"  {'增幅数量':<12} {'记录数':<15} {'平均强化等级':<15}")
    print("  " + "-" * 45)

    max_amplify = 0
    for amplify_cnt, count, avg_upgrade in results:
        print(f"  {amplify_cnt:<12} {count:>10,}   +{avg_upgrade:.2f}")
        if amplify_cnt > max_amplify:
            max_amplify = amplify_cnt

    print(f"\n  - 最大增幅数量: {max_amplify} ⭐")

    # 查询: 增幅与强化的关系
    query_correlation = """
        SELECT upgrade,
               COUNT(CASE WHEN amplify_item_cnt > 0 THEN 1 END) as amplified_count,
               COUNT(*) as total_count
        FROM log_item_upgrade
        WHERE upgrade > 0
        GROUP BY upgrade
        ORDER BY upgrade
    """

    cursor.execute(query_correlation)
    results = cursor.fetchall()

    print(f"\n增幅与强化等级的关系:")
    print(f"  {'强化等级':<12} {'增幅数量':<15} {'总数':<15} {'增幅率'}")
    print("  " + "-" * 60)

    for upgrade_lvl, amplified, total in results:
        amplify_rate = (amplified / total) * 100 if total > 0 else 0
        print(f"  +{upgrade_lvl:<11} {amplified:>10,}   {total:>10,}   {amplify_rate:>6.2f}%")

    cursor.close()

    return {
        'max_amplify': max_amplify
    }

def analyze_item_level_distribution(conn):
    """
    分析物品等级分布

    了解哪些等级的装备最常被强化
    """
    print("\n\n" + "=" * 80)
    print("📊 物品等级分析 (Item Level Analysis)")
    print("=" * 80)

    cursor = conn.cursor()

    # 查询: 物品等级分布（仅显示前20个）
    query = """
        SELECT level,
               COUNT(*) as count,
               AVG(upgrade) as avg_upgrade,
               MAX(upgrade) as max_upgrade
        FROM log_item_upgrade
        WHERE level > 0
        GROUP BY level
        ORDER BY count DESC
        LIMIT 20
    """

    cursor.execute(query)
    results = cursor.fetchall()

    print(f"\n被强化最多的物品等级（Top 20）:")
    print(f"  {'物品等级':<12} {'强化次数':<15} {'平均强化':<12} {'最大强化'}")
    print("  " + "-" * 60)

    for item_level, count, avg_upgrade, max_upgrade in results:
        print(f"  Lv.{item_level:<9} {count:>10,}   +{avg_upgrade:>6.2f}     +{max_upgrade}")

    cursor.close()

def analyze_top_upgraded_items(conn):
    """
    分析被强化到最高等级的物品
    """
    print("\n\n" + "=" * 80)
    print("📊 最高强化物品分析 (Top Upgraded Items)")
    print("=" * 80)

    cursor = conn.cursor()

    # 查询: 强化到最高等级的物品ID
    query = """
        SELECT item_id,
               MAX(upgrade) as max_upgrade,
               COUNT(*) as count,
               MAX(rarity) as rarity,
               MAX(level) as item_level
        FROM log_item_upgrade
        WHERE upgrade >= 10
        GROUP BY item_id
        ORDER BY max_upgrade DESC, count DESC
        LIMIT 20
    """

    cursor.execute(query)
    results = cursor.fetchall()

    print(f"\n强化到+10及以上的物品（Top 20）:")
    print(f"  {'物品ID':<12} {'最高强化':<12} {'强化次数':<12} {'稀有度':<12} {'物品等级'}")
    print("  " + "-" * 70)

    for item_id, max_upgrade, count, rarity, item_level in results:
        rarity_name = RARITY_NAMES.get(rarity, f'Unknown({rarity})')
        print(f"  {item_id:<12} +{max_upgrade:<11} {count:>10,}   {rarity_name:<12} Lv.{item_level}")

    cursor.close()

def generate_recommendations(upgrade_stats, amplify_stats):
    """
    基于真实数据生成Phase 6.7参数建议
    """
    print("\n\n" + "=" * 80)
    print("🎯 Phase 6.7 Enhancement System - 参数建议（基于真实数据）")
    print("=" * 80)

    max_level = upgrade_stats.get('max_level', 12)
    max_by_rarity = upgrade_stats.get('max_by_rarity', {})
    max_amplify = amplify_stats.get('max_amplify', 4)

    print(f"\n### 1. 最大强化等级")
    print(f"   **建议**: **+{max_level}** ✅ (从真实数据提取)")

    print(f"\n### 2. 按稀有度的最大强化等级")
    for rarity in sorted(max_by_rarity.keys()):
        rarity_name = RARITY_NAMES.get(rarity, f'Unknown({rarity})')
        max_lvl = max_by_rarity[rarity]
        print(f"   - {rarity_name}: +{max_lvl}")

    print(f"\n### 3. 强化成功率配置")
    print(f"   **注意**: 成功率在代码中是动态计算的 (GetItemUpgradeSuccessRate)")
    print(f"   建议使用常见游戏平衡配置:")
    print(f"   - +1 to +3: 100%")
    print(f"   - +4 to +6: 80%")
    print(f"   - +7 to +9: 60%")
    print(f"   - +10 to +{max_level}: 30-40%")

    print(f"\n### 4. 增幅系统")
    print(f"   - 最大增幅属性数量: **{max_amplify}** ✅ (从真实数据提取)")
    print(f"   - 增幅等级范围: 建议 +0 到 +10")

    print(f"\n### 5. 其他参数")
    print(f"   - 失败惩罚: 等级降低1级")
    print(f"   - 保护道具: 防止等级降低")
    print(f"   - 大成功概率: 10%（升级+2）")
    print(f"   - 附魔卡片: 一次一张，移除时有概率返还")

    print(f"\n" + "=" * 80)

def main():
    """主函数"""
    print("=" * 80)
    print("Enhancement System Data Analyzer")
    print("从生产数据库提取真实Enhancement参数")
    print("=" * 80)
    print(f"\n数据库: {DB_CONFIG['host']} / {DB_CONFIG['database']}\n")

    # 连接数据库
    conn = connect_database()
    if not conn:
        return 1

    try:
        # 1. 分析强化等级
        upgrade_stats = analyze_upgrade_levels(conn)

        # 2. 分析增幅数据
        amplify_stats = analyze_amplify_data(conn)

        # 3. 分析物品等级分布
        analyze_item_level_distribution(conn)

        # 4. 分析最高强化物品
        analyze_top_upgraded_items(conn)

        # 5. 生成参数建议
        generate_recommendations(upgrade_stats, amplify_stats)

        print(f"\n✅ 数据分析完成！")

    except Exception as e:
        print(f"\n❌ 分析过程出错: {e}")
        import traceback
        traceback.print_exc()
        return 1

    finally:
        conn.close()
        print(f"\n数据库连接已关闭\n")

    return 0

if __name__ == '__main__':
    sys.exit(main())
