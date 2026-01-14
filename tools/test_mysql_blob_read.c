#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>

int main() {
    MYSQL *conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, "192.168.200.131", "game", "uu5!^%jg",
                           "test_taiwan_cain", 3306, NULL, 0)) {
        fprintf(stderr, "连接失败: %s\n", mysql_error(conn));
        return 1;
    }

    printf("成功连接到test_taiwan_cain数据库\n\n");

    // 测试1：直接查询 LENGTH(cargo_data)
    printf("测试1: 查询 BLOB 数据长度\n");
    printf("================================================================================\n");
    if (mysql_query(conn, "SELECT m_id, LENGTH(cargo_data) FROM account_cargo")) {
        fprintf(stderr, "查询失败: %s\n", mysql_error(conn));
    } else {
        MYSQL_RES *result = mysql_store_result(conn);
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            printf("m_id=%s, LENGTH=%s\n", row[0], row[1] ? row[1] : "NULL");
        }
        mysql_free_result(result);
    }
    printf("\n");

    // 测试2：查询 HEX(cargo_data) 的长度
    printf("测试2: 查询 HEX(cargo_data) 的长度\n");
    printf("================================================================================\n");
    if (mysql_query(conn, "SELECT m_id, LENGTH(HEX(cargo_data)) FROM account_cargo")) {
        fprintf(stderr, "查询失败: %s\n", mysql_error(conn));
    } else {
        MYSQL_RES *result = mysql_store_result(conn);
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            printf("m_id=%s, HEX_LENGTH=%s\n", row[0], row[1] ? row[1] : "NULL");
        }
        mysql_free_result(result);
    }
    printf("\n");

    // 测试3：直接查询 HEX(cargo_data)，检查是否被截断
    printf("测试3: 查询 HEX(cargo_data) 数据（m_id=1）\n");
    printf("================================================================================\n");
    if (mysql_query(conn, "SELECT m_id, HEX(cargo_data) FROM account_cargo WHERE m_id=1")) {
        fprintf(stderr, "查询失败: %s\n", mysql_error(conn));
    } else {
        MYSQL_RES *result = mysql_store_result(conn);
        MYSQL_ROW row = mysql_fetch_row(result);

        if (row) {
            unsigned long *lengths = mysql_fetch_lengths(result);
            printf("m_id=%s\n", row[0]);
            printf("HEX(cargo_data) 字段长度 (mysql_fetch_lengths): %lu\n", lengths[1]);
            printf("HEX(cargo_data) 字段长度 (strlen): %lu\n", row[1] ? strlen(row[1]) : 0);

            if (row[1]) {
                printf("HEX(cargo_data) 前40字符: %.40s\n", row[1]);
                printf("HEX(cargo_data) 数据是否为空: %s\n", strlen(row[1]) == 0 ? "是" : "否");
            } else {
                printf("HEX(cargo_data) 为 NULL\n");
            }
        }
        mysql_free_result(result);
    }
    printf("\n");

    // 测试4：使用 max_allowed_packet 设置
    printf("测试4: 检查 max_allowed_packet 设置\n");
    printf("================================================================================\n");
    if (mysql_query(conn, "SHOW VARIABLES LIKE 'max_allowed_packet'")) {
        fprintf(stderr, "查询失败: %s\n", mysql_error(conn));
    } else {
        MYSQL_RES *result = mysql_store_result(conn);
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            printf("%s = %s\n", row[0], row[1]);
        }
        mysql_free_result(result);
    }

    mysql_close(conn);
    return 0;
}
