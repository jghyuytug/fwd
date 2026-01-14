#!/bin/bash
# 导出DNF服务器的真实数据库Schema
# 使用方法: ssh root@192.168.200.131 'bash -s' < export_real_db_schema.sh

MYSQL_USER="root"
MYSQL_PASS="88888888"
OUTPUT_DIR="/tmp/dnf_schema"

mkdir -p $OUTPUT_DIR

echo "[*] 开始导出DNF数据库Schema..."

# 导出主要数据库的Schema
echo "[1/6] 导出 d_taiwan (账号数据库)"
mysqldump -u$MYSQL_USER -p$MYSQL_PASS --no-data d_taiwan > $OUTPUT_DIR/schema_d_taiwan.sql

echo "[2/6] 导出 taiwan_cain (角色数据库)"
mysqldump -u$MYSQL_USER -p$MYSQL_PASS --no-data taiwan_cain > $OUTPUT_DIR/schema_taiwan_cain.sql

echo "[3/6] 导出 taiwan_cain_log (日志数据库)"
mysqldump -u$MYSQL_USER -p$MYSQL_PASS --no-data taiwan_cain_log > $OUTPUT_DIR/schema_taiwan_cain_log.sql

echo "[4/6] 导出 taiwan_billing (计费数据库)"
mysqldump -u$MYSQL_USER -p$MYSQL_PASS --no-data taiwan_billing > $OUTPUT_DIR/schema_taiwan_billing.sql

echo "[5/6] 导出 taiwan_login (登录数据库)"
mysqldump -u$MYSQL_USER -p$MYSQL_PASS --no-data taiwan_login > $OUTPUT_DIR/schema_taiwan_login.sql

echo "[6/6] 导出 d_guild (公会数据库)"
mysqldump -u$MYSQL_USER -p$MYSQL_PASS --no-data d_guild > $OUTPUT_DIR/schema_d_guild.sql 2>/dev/null

echo ""
echo "[+] Schema导出完成！"
echo "[+] 文件保存在: $OUTPUT_DIR/"
ls -lh $OUTPUT_DIR/

echo ""
echo "[*] 正在打包..."
cd /tmp
tar -czf dnf_schema.tar.gz dnf_schema/
echo "[+] 打包完成: /tmp/dnf_schema.tar.gz"
ls -lh /tmp/dnf_schema.tar.gz
