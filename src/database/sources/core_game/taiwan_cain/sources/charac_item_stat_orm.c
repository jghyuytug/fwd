/**
 * DNF游戏服务器 - CharacItemStat表ORM实现
 */

#define _GNU_SOURCE
#include "charac_item_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

/* ========================================================================
 * 内部辅助函数
 * ======================================================================== */

/**
 * 转义BLOB数据用于SQL查询（HEX格式）
 */
static char* EscapeBlobData(const unsigned char* data, int len) {
    if (!data || len <= 0) {
        return strdup("NULL");
    }

    /* 为HEX格式分配内存 (每个字节变成2个字符，加上0x前缀) */
    char* hex = (char*)malloc(len * 2 + 3);
    if (!hex) {
        return NULL;
    }

    hex[0] = '0';
    hex[1] = 'x';

    {
        int i;
        for (i = 0; i < len; i++) {
            sprintf(&hex[2 + i * 2], "%02X", data[i]);
        }
    }

    hex[len * 2 + 2] = '\0';
    return hex;
}

/**
 * 将HEX字符串解码为二进制数据
 */
static unsigned char* DecodeHexString(const char* hex_str, int* out_len) {
    int hex_len, i;
    unsigned char* data;

    if (!hex_str || strlen(hex_str) == 0) {
        *out_len = 0;
        return NULL;
    }

    hex_len = strlen(hex_str);
    *out_len = hex_len / 2;

    data = (unsigned char*)malloc(*out_len);
    if (!data) {
        *out_len = 0;
        return NULL;
    }

    for (i = 0; i < *out_len; i++) {
        char byte_str[3] = {hex_str[i * 2], hex_str[i * 2 + 1], '\0'};
        data[i] = (unsigned char)strtol(byte_str, NULL, 16);
    }

    return data;
}

/* ========================================================================
 * CRUD操作实现
 * ======================================================================== */

int CharacItemStat_Create(DBConnectionManager* manager, CharacItemStat* item_stat) {
    char* query = NULL;
    char* hex_cooltime = NULL;
    char* hex_effect = NULL;
    char* hex_check = NULL;
    int ret = -1;
    size_t query_len;
    DBQueryResult result;

    /* 转义BLOB数据 */
    hex_cooltime = EscapeBlobData(item_stat->cooltime_item, item_stat->cooltime_item_len);
    hex_effect = EscapeBlobData(item_stat->effect_item, item_stat->effect_item_len);
    hex_check = EscapeBlobData(item_stat->check_flag, item_stat->check_flag_len);

    if (!hex_cooltime || !hex_effect || !hex_check) {
        printf("[CharacItemStat ORM] Error: Failed to escape blob data\n");
        goto cleanup;
    }

    /* 构建INSERT查询 */
    query_len = item_stat->cooltime_item_len * 2 +
                item_stat->effect_item_len * 2 +
                item_stat->check_flag_len * 2 + 1024;
    query = (char*)malloc(query_len);
    if (!query) {
        goto cleanup;
    }

    snprintf(query, query_len,
             "INSERT INTO charac_item_stat (charac_no, cooltime_item, effect_item, check_flag) "
             "VALUES (%d, %s, %s, %s)",
             item_stat->charac_no, hex_cooltime, hex_effect, hex_check);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret > 0) {
        printf("[CharacItemStat ORM] Created item stat for charac_no=%d\n", item_stat->charac_no);
        ret = 0;  /* Success */
    } else {
        ret = -1;  /* Failure */
    }

    DBQueryResult_Free(&result);

cleanup:
    if (query) free(query);
    if (hex_cooltime) free(hex_cooltime);
    if (hex_effect) free(hex_effect);
    if (hex_check) free(hex_check);
    return ret;
}

int CharacItemStat_GetByCharacNo(DBConnectionManager* manager, int charac_no, CharacItemStat* item_stat) {
    char query[512];
    char* values[4];
    DBQueryResult result;
    int ret;

    snprintf(query, sizeof(query),
             "SELECT charac_no, HEX(cooltime_item), HEX(effect_item), HEX(check_flag) "
             "FROM charac_item_stat WHERE charac_no = %d",
             charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret <= 0) {  /* No rows or error */
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 获取行数据 */
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 填充基本信息 */
    item_stat->charac_no = atoi(values[0]);

    /* 处理BLOB数据 (HEX字符串解码为二进制) */
    if (values[1] && strlen(values[1]) > 0) {
        item_stat->cooltime_item = DecodeHexString(values[1], &item_stat->cooltime_item_len);
    } else {
        item_stat->cooltime_item = NULL;
        item_stat->cooltime_item_len = 0;
    }

    if (values[2] && strlen(values[2]) > 0) {
        item_stat->effect_item = DecodeHexString(values[2], &item_stat->effect_item_len);
    } else {
        item_stat->effect_item = NULL;
        item_stat->effect_item_len = 0;
    }

    if (values[3] && strlen(values[3]) > 0) {
        item_stat->check_flag = DecodeHexString(values[3], &item_stat->check_flag_len);
    } else {
        item_stat->check_flag = NULL;
        item_stat->check_flag_len = 0;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacItemStat_Update(DBConnectionManager* manager, CharacItemStat* item_stat) {
    char* query = NULL;
    char* hex_cooltime = NULL;
    char* hex_effect = NULL;
    char* hex_check = NULL;
    int ret = -1;
    size_t query_len;
    DBQueryResult result;

    /* 转义BLOB数据 */
    hex_cooltime = EscapeBlobData(item_stat->cooltime_item, item_stat->cooltime_item_len);
    hex_effect = EscapeBlobData(item_stat->effect_item, item_stat->effect_item_len);
    hex_check = EscapeBlobData(item_stat->check_flag, item_stat->check_flag_len);

    if (!hex_cooltime || !hex_effect || !hex_check) {
        printf("[CharacItemStat ORM] Error: Failed to escape blob data\n");
        goto cleanup;
    }

    /* 构建UPDATE查询 */
    query_len = item_stat->cooltime_item_len * 2 +
                item_stat->effect_item_len * 2 +
                item_stat->check_flag_len * 2 + 1024;
    query = (char*)malloc(query_len);
    if (!query) {
        goto cleanup;
    }

    snprintf(query, query_len,
             "UPDATE charac_item_stat SET cooltime_item=%s, effect_item=%s, check_flag=%s "
             "WHERE charac_no=%d",
             hex_cooltime, hex_effect, hex_check, item_stat->charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    /* Query execution error */
    if (ret < 0) {
        DBQueryResult_Free(&result);
        ret = -1;
        goto cleanup;
    }

    /* Update succeeded (even if 0 rows affected - idempotent) */
    if (ret > 0) {
        printf("[CharacItemStat ORM] Updated item stat for charac_no=%d\n", item_stat->charac_no);
    }

    DBQueryResult_Free(&result);
    ret = 0;  /* Success even if 0 rows affected */

cleanup:
    if (query) free(query);
    if (hex_cooltime) free(hex_cooltime);
    if (hex_effect) free(hex_effect);
    if (hex_check) free(hex_check);
    return ret;
}

int CharacItemStat_Delete(DBConnectionManager* manager, int charac_no) {
    char query[256];
    DBQueryResult result;
    int ret;

    snprintf(query, sizeof(query),
             "DELETE FROM charac_item_stat WHERE charac_no=%d", charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret >= 0) {
        printf("[CharacItemStat ORM] Deleted item stat for charac_no=%d\n", charac_no);
        ret = 0;  /* Success */
    } else {
        ret = -1;  /* Failure */
    }

    DBQueryResult_Free(&result);
    return ret;
}

/* ========================================================================
 * 辅助查询函数实现
 * ======================================================================== */

bool CharacItemStat_Exists(DBConnectionManager* manager, int charac_no) {
    char query[256];
    char* values[1];
    int count;
    DBQueryResult result;
    int ret;

    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM charac_item_stat WHERE charac_no=%d", charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    count = atoi(values[0]);
    DBQueryResult_Free(&result);

    return (count > 0);
}

int CharacItemStat_GetBlobSize(DBConnectionManager* manager, int charac_no, const char* field_name) {
    char query[512];
    char* values[1];
    int size;
    DBQueryResult result;
    int ret;

    snprintf(query, sizeof(query),
             "SELECT LENGTH(%s) FROM charac_item_stat WHERE charac_no=%d",
             field_name, charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    size = atoi(values[0]);
    DBQueryResult_Free(&result);

    return size;
}

int CharacItemStat_GetCount(DBConnectionManager* manager) {
    char query[128];
    char* values[1];
    int count;
    DBQueryResult result;
    int ret;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM charac_item_stat");

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    count = atoi(values[0]);
    DBQueryResult_Free(&result);

    return count;
}

int CharacItemStat_GetAllCharacNos(DBConnectionManager* manager, int* charac_nos, int max_count) {
    char query[256];
    char* values[1];
    int count, i;
    DBQueryResult result;
    int ret;

    snprintf(query, sizeof(query),
             "SELECT charac_no FROM charac_item_stat ORDER BY charac_no LIMIT %d", max_count);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    count = (result.row_count < max_count) ? result.row_count : max_count;

    for (i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            charac_nos[i] = atoi(values[0]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ========================================================================
 * 工具函数实现
 * ======================================================================== */

void CharacItemStat_Init(CharacItemStat* item_stat) {
    memset(item_stat, 0, sizeof(CharacItemStat));
    item_stat->charac_no = 0;
    item_stat->cooltime_item = NULL;
    item_stat->cooltime_item_len = 0;
    item_stat->effect_item = NULL;
    item_stat->effect_item_len = 0;
    item_stat->check_flag = NULL;
    item_stat->check_flag_len = 0;
}

void CharacItemStat_Free(CharacItemStat* item_stat) {
    if (item_stat->cooltime_item) {
        free(item_stat->cooltime_item);
        item_stat->cooltime_item = NULL;
    }
    if (item_stat->effect_item) {
        free(item_stat->effect_item);
        item_stat->effect_item = NULL;
    }
    if (item_stat->check_flag) {
        free(item_stat->check_flag);
        item_stat->check_flag = NULL;
    }
    item_stat->cooltime_item_len = 0;
    item_stat->effect_item_len = 0;
    item_stat->check_flag_len = 0;
}

void CharacItemStat_Print(const CharacItemStat* item_stat) {
    int i, display_len;

    printf("================================================================================\n");
    printf("CharacItemStat Information:\n");
    printf("================================================================================\n");
    printf("  角色编号:      %d\n", item_stat->charac_no);
    printf("  冷却数据大小:  %d bytes\n", item_stat->cooltime_item_len);
    printf("  效果数据大小:  %d bytes\n", item_stat->effect_item_len);
    printf("  标记数据大小:  %d bytes\n", item_stat->check_flag_len);

    /* 显示冷却数据前32字节的十六进制 */
    if (item_stat->cooltime_item && item_stat->cooltime_item_len > 0) {
        display_len = (item_stat->cooltime_item_len > 32) ? 32 : item_stat->cooltime_item_len;
        printf("  冷却数据预览:  ");
        for (i = 0; i < display_len; i++) {
            printf("%02X ", item_stat->cooltime_item[i]);
            if ((i + 1) % 16 == 0 && i < display_len - 1) {
                printf("\n                 ");
            }
        }
        if (item_stat->cooltime_item_len > 32) {
            printf("... (%d more bytes)", item_stat->cooltime_item_len - 32);
        }
        printf("\n");
    } else {
        printf("  冷却数据预览:  (空)\n");
    }

    /* 显示效果数据前32字节的十六进制 */
    if (item_stat->effect_item && item_stat->effect_item_len > 0) {
        display_len = (item_stat->effect_item_len > 32) ? 32 : item_stat->effect_item_len;
        printf("  效果数据预览:  ");
        for (i = 0; i < display_len; i++) {
            printf("%02X ", item_stat->effect_item[i]);
            if ((i + 1) % 16 == 0 && i < display_len - 1) {
                printf("\n                 ");
            }
        }
        if (item_stat->effect_item_len > 32) {
            printf("... (%d more bytes)", item_stat->effect_item_len - 32);
        }
        printf("\n");
    } else {
        printf("  效果数据预览:  (空)\n");
    }

    /* 显示标记数据前32字节的十六进制 */
    if (item_stat->check_flag && item_stat->check_flag_len > 0) {
        display_len = (item_stat->check_flag_len > 32) ? 32 : item_stat->check_flag_len;
        printf("  标记数据预览:  ");
        for (i = 0; i < display_len; i++) {
            printf("%02X ", item_stat->check_flag[i]);
            if ((i + 1) % 16 == 0 && i < display_len - 1) {
                printf("\n                 ");
            }
        }
        if (item_stat->check_flag_len > 32) {
            printf("... (%d more bytes)", item_stat->check_flag_len - 32);
        }
        printf("\n");
    } else {
        printf("  标记数据预览:  (空)\n");
    }

    printf("================================================================================\n");
}

int CharacItemStat_AllocateBlob(CharacItemStat* item_stat, int field, int size) {
    if (size <= 0) {
        return -1;
    }

    switch (field) {
    case 1:  /* cooltime_item */
        if (item_stat->cooltime_item) {
            free(item_stat->cooltime_item);
        }
        item_stat->cooltime_item = (unsigned char*)malloc(size);
        if (!item_stat->cooltime_item) {
            return -1;
        }
        memset(item_stat->cooltime_item, 0, size);
        item_stat->cooltime_item_len = size;
        break;

    case 2:  /* effect_item */
        if (item_stat->effect_item) {
            free(item_stat->effect_item);
        }
        item_stat->effect_item = (unsigned char*)malloc(size);
        if (!item_stat->effect_item) {
            return -1;
        }
        memset(item_stat->effect_item, 0, size);
        item_stat->effect_item_len = size;
        break;

    case 3:  /* check_flag */
        if (item_stat->check_flag) {
            free(item_stat->check_flag);
        }
        item_stat->check_flag = (unsigned char*)malloc(size);
        if (!item_stat->check_flag) {
            return -1;
        }
        memset(item_stat->check_flag, 0, size);
        item_stat->check_flag_len = size;
        break;

    default:
        return -1;
    }

    return 0;
}

int CharacItemStat_Copy(CharacItemStat* dest, const CharacItemStat* src) {
    /* 释放目标的旧数据 */
    CharacItemStat_Free(dest);

    /* 复制基本字段 */
    dest->charac_no = src->charac_no;

    /* 复制cooltime_item */
    if (src->cooltime_item && src->cooltime_item_len > 0) {
        dest->cooltime_item = (unsigned char*)malloc(src->cooltime_item_len);
        if (!dest->cooltime_item) {
            dest->cooltime_item_len = 0;
            return -1;
        }
        memcpy(dest->cooltime_item, src->cooltime_item, src->cooltime_item_len);
        dest->cooltime_item_len = src->cooltime_item_len;
    }

    /* 复制effect_item */
    if (src->effect_item && src->effect_item_len > 0) {
        dest->effect_item = (unsigned char*)malloc(src->effect_item_len);
        if (!dest->effect_item) {
            dest->effect_item_len = 0;
            return -1;
        }
        memcpy(dest->effect_item, src->effect_item, src->effect_item_len);
        dest->effect_item_len = src->effect_item_len;
    }

    /* 复制check_flag */
    if (src->check_flag && src->check_flag_len > 0) {
        dest->check_flag = (unsigned char*)malloc(src->check_flag_len);
        if (!dest->check_flag) {
            dest->check_flag_len = 0;
            return -1;
        }
        memcpy(dest->check_flag, src->check_flag, src->check_flag_len);
        dest->check_flag_len = src->check_flag_len;
    }

    return 0;
}
