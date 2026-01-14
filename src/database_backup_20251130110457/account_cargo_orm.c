/**
 * DNF游戏服务器 - AccountCargo表ORM实现
 */

#define _GNU_SOURCE
#include "account_cargo_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mysql/mysql.h>

// ========================================================================
// 内部辅助函数
// ========================================================================

/**
 * 获取当前时间字符串 (YYYY-MM-DD HH:MM:SS)
 */
static void GetCurrentTimeString(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    snprintf(buffer, size, "%04d-%02d-%02d %02d:%02d:%02d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
}

/**
 * 转义BLOB数据用于SQL查询
 * 注意: MySQL的BLOB数据需要使用HEX格式或使用mysql_real_escape_string
 * 这里使用简化的方法，实际生产环境应使用预编译语句
 */
static char* EscapeBlobData(const unsigned char* data, int len) {
    // 为HEX格式分配内存 (每个字节变成2个字符，加上0x前缀)
    char* hex = (char*)malloc(len * 2 + 3);
    if (!hex) {
        return NULL;
    }

    hex[0] = '0';
    hex[1] = 'x';

    for (int i = 0; i < len; i++) {
        sprintf(&hex[2 + i * 2], "%02X", data[i]);
    }

    hex[len * 2 + 2] = '\0';
    return hex;
}

/**
 * 将HEX字符串解码为二进制数据
 */
static unsigned char* DecodeHexString(const char* hex_str, int* out_len) {
    if (!hex_str || strlen(hex_str) == 0) {
        *out_len = 0;
        return NULL;
    }

    int hex_len = strlen(hex_str);
    *out_len = hex_len / 2;

    unsigned char* data = (unsigned char*)malloc(*out_len);
    if (!data) {
        *out_len = 0;
        return NULL;
    }

    for (int i = 0; i < *out_len; i++) {
        char byte_str[3] = {hex_str[i * 2], hex_str[i * 2 + 1], '\0'};
        data[i] = (unsigned char)strtol(byte_str, NULL, 16);
    }

    return data;
}

// ========================================================================
// CRUD操作实现
// ========================================================================

int AccountCargo_Create(DBConnectionManager* manager, AccountCargo* cargo) {
    char* query = NULL;
    char* hex_data = NULL;
    char update_time[20];
    int ret = -1;

    // 获取当前时间
    GetCurrentTimeString(update_time, sizeof(update_time));
    strncpy(cargo->update_time, update_time, sizeof(cargo->update_time) - 1);

    // 转义BLOB数据
    if (cargo->cargo_data && cargo->cargo_data_len > 0) {
        hex_data = EscapeBlobData(cargo->cargo_data, cargo->cargo_data_len);
        if (!hex_data) {
            printf("[AccountCargo ORM] Error: Failed to escape blob data\n");
            return -1;
        }
    } else {
        hex_data = strdup("NULL");
    }

    // 构建INSERT查询
    query = (char*)malloc(cargo->cargo_data_len * 2 + 512);
    if (!query) {
        free(hex_data);
        return -1;
    }

    snprintf(query, cargo->cargo_data_len * 2 + 512,
             "INSERT INTO account_cargo (m_id, cargo_data, update_time) "
             "VALUES (%d, %s, '%s')",
             cargo->m_id, hex_data, cargo->update_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret > 0) {
        printf("[AccountCargo ORM] Created cargo for account m_id=%d, size=%d bytes\n",
               cargo->m_id, cargo->cargo_data_len);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    free(query);
    free(hex_data);
    return ret;
}

int AccountCargo_GetByID(DBConnectionManager* manager, int m_id, AccountCargo* cargo) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT m_id, HEX(cargo_data), update_time "
             "FROM account_cargo WHERE m_id = %d",
             m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    // 获取行数据
    char* values[3];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // 填充基本信息
    cargo->m_id = atoi(values[0]);

    // 处理BLOB数据 (HEX字符串解码为二进制)
    const char* hex_str = values[1];
    if (hex_str && strlen(hex_str) > 0) {
        cargo->cargo_data = DecodeHexString(hex_str, &cargo->cargo_data_len);
    } else {
        cargo->cargo_data = NULL;
        cargo->cargo_data_len = 0;
    }

    strncpy(cargo->update_time, values[2], sizeof(cargo->update_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int AccountCargo_Update(DBConnectionManager* manager, AccountCargo* cargo) {
    char* query = NULL;
    char* hex_data = NULL;
    char update_time[20];
    int ret = -1;

    // 更新时间
    GetCurrentTimeString(update_time, sizeof(update_time));
    strncpy(cargo->update_time, update_time, sizeof(cargo->update_time) - 1);

    // 转义BLOB数据
    if (cargo->cargo_data && cargo->cargo_data_len > 0) {
        hex_data = EscapeBlobData(cargo->cargo_data, cargo->cargo_data_len);
        if (!hex_data) {
            printf("[AccountCargo ORM] Error: Failed to escape blob data\n");
            return -1;
        }
    } else {
        hex_data = strdup("NULL");
    }

    // 构建UPDATE查询
    query = (char*)malloc(cargo->cargo_data_len * 2 + 512);
    if (!query) {
        free(hex_data);
        return -1;
    }

    snprintf(query, cargo->cargo_data_len * 2 + 512,
             "UPDATE account_cargo SET cargo_data=%s, update_time='%s' "
             "WHERE m_id=%d",
             hex_data, cargo->update_time, cargo->m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret > 0) {
        printf("[AccountCargo ORM] Updated cargo for m_id=%d, size=%d bytes\n",
               cargo->m_id, cargo->cargo_data_len);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    free(query);
    free(hex_data);
    return ret;
}

int AccountCargo_Delete(DBConnectionManager* manager, int m_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM account_cargo WHERE m_id=%d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret >= 0) {  // DELETE can affect 0 rows if record doesn't exist
        printf("[AccountCargo ORM] Deleted cargo for m_id=%d\n", m_id);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

// ========================================================================
// 辅助查询函数实现
// ========================================================================

bool AccountCargo_Exists(DBConnectionManager* manager, int m_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM account_cargo WHERE m_id=%d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return false;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);

    return (count > 0);
}

int AccountCargo_GetDataSize(DBConnectionManager* manager, int m_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT LENGTH(cargo_data) FROM account_cargo WHERE m_id=%d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int size = atoi(values[0]);
    DBQueryResult_Free(&result);

    return size;
}

int AccountCargo_GetAllAccountIDs(DBConnectionManager* manager, int* m_ids, int max_count) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT m_id FROM account_cargo ORDER BY m_id LIMIT %d", max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = (result.row_count < max_count) ? result.row_count : max_count;

    char* values[1];
    for (int i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            m_ids[i] = atoi(values[0]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

int AccountCargo_GetCount(DBConnectionManager* manager) {
    char query[128];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM account_cargo");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);

    return count;
}

int AccountCargo_CleanupOldData(DBConnectionManager* manager, int days) {
    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM account_cargo "
             "WHERE update_time < DATE_SUB(NOW(), INTERVAL %d DAY)",
             days);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret == 0) {
        // MySQL的affected_rows可以获取删除数量，这里简化处理
        printf("[AccountCargo ORM] Cleaned up cargo data older than %d days\n", days);
        DBQueryResult_Free(&result);
        return 0; // 返回0表示成功，实际应返回删除数量
    }

    DBQueryResult_Free(&result);
    return -1;
}

// ========================================================================
// 工具函数实现
// ========================================================================

void AccountCargo_Init(AccountCargo* cargo) {
    memset(cargo, 0, sizeof(AccountCargo));
    cargo->m_id = 0;
    cargo->cargo_data = NULL;
    cargo->cargo_data_len = 0;
    strcpy(cargo->update_time, "0000-00-00 00:00:00");
}

void AccountCargo_Free(AccountCargo* cargo) {
    if (cargo->cargo_data) {
        free(cargo->cargo_data);
        cargo->cargo_data = NULL;
    }
    cargo->cargo_data_len = 0;
}

void AccountCargo_Print(const AccountCargo* cargo) {
    printf("================================================================================\n");
    printf("AccountCargo Information:\n");
    printf("================================================================================\n");
    printf("  账号ID:        %d\n", cargo->m_id);
    printf("  数据大小:      %d bytes\n", cargo->cargo_data_len);
    printf("  更新时间:      %s\n", cargo->update_time);

    // 显示前32字节的十六进制数据（如果有）
    if (cargo->cargo_data && cargo->cargo_data_len > 0) {
        int display_len = (cargo->cargo_data_len > 32) ? 32 : cargo->cargo_data_len;
        printf("  数据预览:      ");
        for (int i = 0; i < display_len; i++) {
            printf("%02X ", cargo->cargo_data[i]);
            if ((i + 1) % 16 == 0 && i < display_len - 1) {
                printf("\n                 ");
            }
        }
        if (cargo->cargo_data_len > 32) {
            printf("... (%d more bytes)", cargo->cargo_data_len - 32);
        }
        printf("\n");
    } else {
        printf("  数据预览:      (空)\n");
    }

    printf("================================================================================\n");
}

int AccountCargo_AllocateData(AccountCargo* cargo, int size) {
    if (size <= 0) {
        return -1;
    }

    // 释放旧数据
    AccountCargo_Free(cargo);

    // 分配新数据
    cargo->cargo_data = (unsigned char*)malloc(size);
    if (!cargo->cargo_data) {
        return -1;
    }

    memset(cargo->cargo_data, 0, size);
    cargo->cargo_data_len = size;

    return 0;
}

int AccountCargo_Copy(AccountCargo* dest, const AccountCargo* src) {
    // 释放目标的旧数据
    AccountCargo_Free(dest);

    // 复制基本字段
    dest->m_id = src->m_id;
    strncpy(dest->update_time, src->update_time, sizeof(dest->update_time) - 1);

    // 复制BLOB数据
    if (src->cargo_data && src->cargo_data_len > 0) {
        dest->cargo_data = (unsigned char*)malloc(src->cargo_data_len);
        if (!dest->cargo_data) {
            dest->cargo_data_len = 0;
            return -1;
        }

        memcpy(dest->cargo_data, src->cargo_data, src->cargo_data_len);
        dest->cargo_data_len = src->cargo_data_len;
    } else {
        dest->cargo_data = NULL;
        dest->cargo_data_len = 0;
    }

    return 0;
}
