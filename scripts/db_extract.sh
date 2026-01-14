#!/bin/bash
# DNF数据库结构提取脚本
# 生成时间: 2025-11-11
# 目的: 提取27个数据库的完整schema

MYSQL_PASS="88888888"
OUTPUT_DIR="/tmp/db_export_$(date +%Y%m%d_%H%M%S)"

echo "=========================================="
echo "DNF 数据库结构提取工具"
echo "=========================================="
echo ""

# 创建输出目录
mkdir -p "$OUTPUT_DIR"
cd "$OUTPUT_DIR"

echo "[1/6] 检查MySQL连接..."
mysql -u root -p${MYSQL_PASS} -e "SELECT VERSION();" > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "✓ MySQL连接成功"
    mysql -u root -p${MYSQL_PASS} -e "SELECT VERSION();" > mysql_version.txt
else
    echo "✗ MySQL连接失败，请检查密码"
    exit 1
fi

echo ""
echo "[2/6] 获取数据库列表..."
mysql -u root -p${MYSQL_PASS} -e "SHOW DATABASES;" > database_list.txt
cat database_list.txt
echo "✓ 数据库列表已保存"

echo ""
echo "[3/6] 提取所有数据库结构（无数据）..."
mysqldump -u root -p${MYSQL_PASS} \
    --no-data \
    --all-databases \
    --skip-lock-tables \
    --skip-add-drop-table \
    > all_databases_schema.sql 2>&1

if [ -s all_databases_schema.sql ]; then
    echo "✓ 全部数据库schema已导出: $(du -h all_databases_schema.sql | cut -f1)"
else
    echo "⚠ schema文件为空，尝试单独导出"
fi

echo ""
echo "[4/6] 提取27个DNF数据库的详细结构..."

# DNF核心数据库列表
DNF_DATABASES=(
    "d_taiwan"
    "d_gmaster"
    "taiwan_billing"
    "taiwan_cain"
    "taiwan_cain_2nd"
    "taiwan_login"
    "taiwan_pvp"
    "taiwan_guild"
    "taiwan_cosmo"
    "taiwan_event"
    "taiwan_practice"
    "taiwan_test"
    "d_channelinfo"
    "d_gameinfo"
    "taiwan_admin"
    "taiwan_web"
    "taiwan_mall"
    "taiwan_community"
    "taiwan_quest"
    "taiwan_dungeon"
    "taiwan_item"
    "taiwan_skill"
    "taiwan_auction"
    "taiwan_mail"
    "taiwan_trade"
    "taiwan_pvp_rank"
    "taiwan_achievement"
)

mkdir -p individual_schemas

for db in "${DNF_DATABASES[@]}"; do
    # 检查数据库是否存在
    EXISTS=$(mysql -u root -p${MYSQL_PASS} -e "SHOW DATABASES LIKE '$db';" 2>/dev/null | grep -c "$db")

    if [ "$EXISTS" -eq 1 ]; then
        echo "  导出: $db"

        # 导出schema
        mysqldump -u root -p${MYSQL_PASS} \
            --no-data \
            --skip-lock-tables \
            --databases "$db" \
            > "individual_schemas/${db}_schema.sql" 2>&1

        # 导出表统计
        mysql -u root -p${MYSQL_PASS} -e "
            SELECT
                TABLE_NAME as '表名',
                TABLE_ROWS as '行数',
                ROUND(DATA_LENGTH/1024/1024, 2) as '数据大小(MB)',
                ROUND(INDEX_LENGTH/1024/1024, 2) as '索引大小(MB)'
            FROM information_schema.TABLES
            WHERE TABLE_SCHEMA='$db'
            ORDER BY DATA_LENGTH DESC;
        " > "individual_schemas/${db}_stats.txt"
    else
        echo "  ⚠ 数据库不存在: $db"
    fi
done

echo "✓ 单独数据库schema已导出到 individual_schemas/"

echo ""
echo "[5/6] 提取核心表的详细字段定义..."

# 提取核心数据库的表和字段信息
mysql -u root -p${MYSQL_PASS} -e "
SELECT
    TABLE_SCHEMA as '数据库',
    TABLE_NAME as '表名',
    COLUMN_NAME as '字段名',
    COLUMN_TYPE as '类型',
    IS_NULLABLE as '允许NULL',
    COLUMN_DEFAULT as '默认值',
    COLUMN_KEY as '键',
    EXTRA as '额外',
    COLUMN_COMMENT as '注释'
FROM information_schema.COLUMNS
WHERE TABLE_SCHEMA IN ('d_taiwan', 'taiwan_cain', 'taiwan_billing', 'taiwan_login', 'taiwan_guild')
ORDER BY TABLE_SCHEMA, TABLE_NAME, ORDINAL_POSITION;
" > core_tables_detailed_fields.txt

echo "✓ 核心表字段信息已导出"

echo ""
echo "[6/6] 生成统计报告..."

# 生成摘要报告
cat > extraction_report.txt <<EOF
================================================================================
DNF 数据库结构提取报告
================================================================================
提取时间: $(date)
服务器: $(hostname)
MySQL版本: $(mysql -u root -p${MYSQL_PASS} -e "SELECT VERSION();" -s -N)

--------------------------------------------------------------------------------
1. 数据库统计
--------------------------------------------------------------------------------
EOF

mysql -u root -p${MYSQL_PASS} -e "
SELECT
    SCHEMA_NAME as '数据库名',
    COUNT(*) as '表数量'
FROM information_schema.TABLES
WHERE SCHEMA_NAME LIKE 'taiwan_%' OR SCHEMA_NAME LIKE 'd_%'
GROUP BY SCHEMA_NAME
ORDER BY SCHEMA_NAME;
" >> extraction_report.txt

cat >> extraction_report.txt <<EOF

--------------------------------------------------------------------------------
2. 总体统计
--------------------------------------------------------------------------------
EOF

mysql -u root -p${MYSQL_PASS} -e "
SELECT
    COUNT(DISTINCT TABLE_SCHEMA) as '数据库总数',
    COUNT(*) as '表总数',
    SUM(TABLE_ROWS) as '总行数',
    ROUND(SUM(DATA_LENGTH)/1024/1024/1024, 2) as '数据总大小(GB)',
    ROUND(SUM(INDEX_LENGTH)/1024/1024/1024, 2) as '索引总大小(GB)'
FROM information_schema.TABLES
WHERE TABLE_SCHEMA LIKE 'taiwan_%' OR TABLE_SCHEMA LIKE 'd_%';
" >> extraction_report.txt

cat >> extraction_report.txt <<EOF

--------------------------------------------------------------------------------
3. 导出文件列表
--------------------------------------------------------------------------------
EOF

ls -lh >> extraction_report.txt

echo "✓ 统计报告已生成: extraction_report.txt"

echo ""
echo "=========================================="
echo "✓ 数据提取完成！"
echo "=========================================="
echo ""
echo "输出目录: $OUTPUT_DIR"
echo ""
echo "导出文件："
echo "  - all_databases_schema.sql      (全部数据库结构)"
echo "  - individual_schemas/*.sql      (单独数据库结构)"
echo "  - individual_schemas/*_stats.txt (表统计信息)"
echo "  - core_tables_detailed_fields.txt (核心表字段详细)"
echo "  - extraction_report.txt         (统计报告)"
echo "  - database_list.txt             (数据库列表)"
echo ""
echo "打包命令："
echo "  tar -czf db_export.tar.gz *"
echo ""
