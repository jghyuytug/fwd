#include <stdio.h>
#include <mysql/mysql.h>

int main() {
    MYSQL *conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, "192.168.200.131", "game", "uu5!^%jg",
                           "test_taiwan_cain", 3306, NULL, 0)) {
        fprintf(stderr, "连接失败: %s\n", mysql_error(conn));
        return 1;
    }

    printf("成功连接到test_taiwan_cain数据库\n\n");

    if (mysql_query(conn, "SHOW COLUMNS FROM charac_info")) {
        fprintf(stderr, "查询失败: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    MYSQL_ROW row;

    printf("charac_info 表结构:\n");
    printf("================================================================================\n");
    printf("%-25s %-20s %-8s %-8s %-10s %-20s\n",
           "字段名", "类型", "NULL", "键", "默认值", "额外");
    printf("================================================================================\n");

    while ((row = mysql_fetch_row(result))) {
        printf("%-25s %-20s %-8s %-8s %-10s %-20s\n",
               row[0], row[1], row[2], row[3],
               row[4] ? row[4] : "NULL", row[5] ? row[5] : "");
    }

    mysql_free_result(result);

    printf("\n样本数据 (前3条):\n");
    printf("================================================================================\n");
    if (mysql_query(conn, "SELECT * FROM charac_info LIMIT 3")) {
        fprintf(stderr, "查询失败: %s\n", mysql_error(conn));
    } else {
        int i;
        int num_fields;
        MYSQL_FIELD *fields;

        result = mysql_store_result(conn);
        num_fields = mysql_num_fields(result);

        fields = mysql_fetch_fields(result);
        for (i = 0; i < num_fields; i++) {
            printf("%-20s ", fields[i].name);
        }
        printf("\n");
        printf("================================================================================\n");

        while ((row = mysql_fetch_row(result))) {
            for (i = 0; i < num_fields; i++) {
                printf("%-20s ", row[i] ? row[i] : "NULL");
            }
            printf("\n");
        }
        mysql_free_result(result);
    }

    mysql_close(conn);
    return 0;
}
