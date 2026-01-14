#include "charac_option_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 200000

/* ==================== 辅助函数 ==================== */

static char* bin_to_hex(const unsigned char* data, unsigned int len) {
    char* hex;
    unsigned int i;

    if (!data || len == 0) {
        return NULL;
    }

    hex = (char*)malloc(len * 2 + 1);
    if (!hex) {
        return NULL;
    }

    for (i = 0; i < len; i++) {
        sprintf(hex + i * 2, "%02X", data[i]);
    }
    hex[len * 2] = '\0';

    return hex;
}

static unsigned char* hex_to_bin(const char* hex, unsigned int* out_len) {
    unsigned char* bin;
    unsigned int hex_len, bin_len, i;

    if (!hex || !out_len) {
        return NULL;
    }

    hex_len = strlen(hex);
    if (hex_len % 2 != 0) {
        return NULL;
    }

    bin_len = hex_len / 2;
    bin = (unsigned char*)malloc(bin_len);
    if (!bin) {
        return NULL;
    }

    for (i = 0; i < bin_len; i++) {
        sscanf(hex + i * 2, "%2hhX", &bin[i]);
    }

    *out_len = bin_len;
    return bin;
}

/* ==================== CRUD 操作实现 ==================== */

int CharacOption_Add(DBConnectionManager* manager, const CharacOption* record) {
    char* query;
    char* options_value;
    char* best_clear_time_value;
    DBQueryResult result;
    char* options_hex = NULL;
    char* best_clear_time_hex = NULL;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    query = (char*)malloc(MAX_QUERY_LEN);
    options_value = (char*)malloc(MAX_QUERY_LEN / 3);
    best_clear_time_value = (char*)malloc(MAX_QUERY_LEN / 3);

    if (!query || !options_value || !best_clear_time_value) {
        free(query);
        free(options_value);
        free(best_clear_time_value);
        return -1;
    }

    options_hex = bin_to_hex(record->options, record->options_len);
    best_clear_time_hex = bin_to_hex(record->best_clear_time, record->best_clear_time_len);

    if (options_hex) {
        snprintf(options_value, MAX_QUERY_LEN / 3, "0x%s", options_hex);
    } else {
        strcpy(options_value, "NULL");
    }

    if (best_clear_time_hex) {
        snprintf(best_clear_time_value, MAX_QUERY_LEN / 3, "0x%s", best_clear_time_hex);
    } else {
        strcpy(best_clear_time_value, "NULL");
    }

    snprintf(query, MAX_QUERY_LEN,
        "REPLACE INTO charac_option "
        "(charac_no, options, best_clear_time, blue_marble_enter_count, charac_inform_notice) "
        "VALUES (%d, %s, %s, %u, '%s')",
        record->charac_no,
        options_value, best_clear_time_value,
        (unsigned int)record->blue_marble_enter_count,
        record->charac_inform_notice);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) >= 0) {
        DBQueryResult_Free(&result);
        ret = 0;
    }

    free(query);
    free(options_value);
    free(best_clear_time_value);
    free(options_hex);
    free(best_clear_time_hex);

    return ret;
}

int CharacOption_Get(DBConnectionManager* manager, int charac_no, CharacOption* record) {
    char query[4096];
    DBQueryResult result;
    char* values[16];

    if (!manager || !record) {
        return -1;
    }

    memset(record, 0, sizeof(CharacOption));

    snprintf(query, sizeof(query),
        "SELECT charac_no, HEX(options), HEX(best_clear_time), blue_marble_enter_count, charac_inform_notice "
        "FROM charac_option WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = atoi(values[0]);

        /* 处理 options BLOB (从HEX字符串转换) */
        if (values[1] && strlen(values[1]) > 0) {
            record->options = hex_to_bin(values[1], &record->options_len);
        }

        /* 处理 best_clear_time BLOB (从HEX字符串转换) */
        if (values[2] && strlen(values[2]) > 0) {
            record->best_clear_time = hex_to_bin(values[2], &record->best_clear_time_len);
        }

        record->blue_marble_enter_count = (unsigned char)atoi(values[3]);
        if (values[4]) {
            strncpy(record->charac_inform_notice, values[4], MAX_NOTICE_LEN - 1);
        }

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacOption_Update(DBConnectionManager* manager, const CharacOption* record) {
    char* query;
    char* options_value;
    char* best_clear_time_value;
    DBQueryResult result;
    char* options_hex = NULL;
    char* best_clear_time_hex = NULL;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    query = (char*)malloc(MAX_QUERY_LEN);
    options_value = (char*)malloc(MAX_QUERY_LEN / 3);
    best_clear_time_value = (char*)malloc(MAX_QUERY_LEN / 3);

    if (!query || !options_value || !best_clear_time_value) {
        free(query);
        free(options_value);
        free(best_clear_time_value);
        return -1;
    }

    options_hex = bin_to_hex(record->options, record->options_len);
    best_clear_time_hex = bin_to_hex(record->best_clear_time, record->best_clear_time_len);

    if (options_hex) {
        snprintf(options_value, MAX_QUERY_LEN / 3, "0x%s", options_hex);
    } else {
        strcpy(options_value, "NULL");
    }

    if (best_clear_time_hex) {
        snprintf(best_clear_time_value, MAX_QUERY_LEN / 3, "0x%s", best_clear_time_hex);
    } else {
        strcpy(best_clear_time_value, "NULL");
    }

    snprintf(query, MAX_QUERY_LEN,
        "UPDATE charac_option SET "
        "options = %s, best_clear_time = %s, blue_marble_enter_count = %u, charac_inform_notice = '%s' "
        "WHERE charac_no = %d",
        options_value, best_clear_time_value,
        (unsigned int)record->blue_marble_enter_count,
        record->charac_inform_notice,
        record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) >= 0) {
        DBQueryResult_Free(&result);
        ret = 0;
    }

    free(query);
    free(options_value);
    free(best_clear_time_value);
    free(options_hex);
    free(best_clear_time_hex);

    return ret;
}

int CharacOption_Delete(DBConnectionManager* manager, int charac_no) {
    char query[1024];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_option WHERE charac_no = %d", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacOption_Exists(DBConnectionManager* manager, int charac_no) {
    char query[1024];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_option WHERE charac_no = %d", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return (count > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

/* ==================== 专项操作实现 ==================== */

int CharacOption_UpdateOptions(DBConnectionManager* manager, int charac_no,
                                const unsigned char* options, unsigned int options_len) {
    char* query;
    char* options_value;
    DBQueryResult result;
    char* options_hex = NULL;
    int ret = -1;

    if (!manager) {
        return -1;
    }

    query = (char*)malloc(MAX_QUERY_LEN);
    options_value = (char*)malloc(MAX_QUERY_LEN / 2);

    if (!query || !options_value) {
        free(query);
        free(options_value);
        return -1;
    }

    options_hex = bin_to_hex(options, options_len);

    if (options_hex) {
        snprintf(options_value, MAX_QUERY_LEN / 2, "0x%s", options_hex);
    } else {
        strcpy(options_value, "NULL");
    }

    snprintf(query, MAX_QUERY_LEN,
        "UPDATE charac_option SET options = %s WHERE charac_no = %d",
        options_value, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) >= 0) {
        DBQueryResult_Free(&result);
        ret = 0;
    }

    free(query);
    free(options_value);
    free(options_hex);

    return ret;
}

int CharacOption_UpdateBestClearTime(DBConnectionManager* manager, int charac_no,
                                      const unsigned char* best_clear_time, unsigned int time_len) {
    char* query;
    char* time_value;
    DBQueryResult result;
    char* time_hex = NULL;
    int ret = -1;

    if (!manager) {
        return -1;
    }

    query = (char*)malloc(MAX_QUERY_LEN);
    time_value = (char*)malloc(MAX_QUERY_LEN / 2);

    if (!query || !time_value) {
        free(query);
        free(time_value);
        return -1;
    }

    time_hex = bin_to_hex(best_clear_time, time_len);

    if (time_hex) {
        snprintf(time_value, MAX_QUERY_LEN / 2, "0x%s", time_hex);
    } else {
        strcpy(time_value, "NULL");
    }

    snprintf(query, MAX_QUERY_LEN,
        "UPDATE charac_option SET best_clear_time = %s WHERE charac_no = %d",
        time_value, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) >= 0) {
        DBQueryResult_Free(&result);
        ret = 0;
    }

    free(query);
    free(time_value);
    free(time_hex);

    return ret;
}

int CharacOption_UpdateBlueMarbleEnterCount(DBConnectionManager* manager, int charac_no,
                                              unsigned char enter_count) {
    char query[1024];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_option SET blue_marble_enter_count = %u WHERE charac_no = %d",
        (unsigned int)enter_count, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacOption_IncrementBlueMarbleEnterCount(DBConnectionManager* manager, int charac_no, int increment) {
    char query[1024];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_option SET blue_marble_enter_count = blue_marble_enter_count + %d WHERE charac_no = %d",
        increment, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacOption_UpdateInformNotice(DBConnectionManager* manager, int charac_no,
                                     const char* notice) {
    char query[2048];
    DBQueryResult result;

    if (!manager || !notice) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_option SET charac_inform_notice = '%s' WHERE charac_no = %d",
        notice, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作实现 ==================== */

int CharacOption_GetTotalCount(DBConnectionManager* manager) {
    char query[512];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_option");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return count;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacOption_GetByBlueMarbleCountRange(DBConnectionManager* manager,
                                             unsigned char min_count, unsigned char max_count,
                                             int* charac_nos, int max_result) {
    char query[1024];
    DBQueryResult result;
    char* values[16];
    int count = 0;

    if (!manager || !charac_nos || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no FROM charac_option WHERE blue_marble_enter_count BETWEEN %u AND %u LIMIT %d",
        (unsigned int)min_count, (unsigned int)max_count, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        charac_nos[count++] = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

long long CharacOption_GetTotalBlueMarbleEnterCount(DBConnectionManager* manager) {
    char query[512];
    DBQueryResult result;
    char* values[1];
    long long total = 0;

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT SUM(blue_marble_enter_count) FROM charac_option");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        total = atoll(values[0]);
    }

    DBQueryResult_Free(&result);
    return total;
}

/* ==================== 工具函数实现 ==================== */

void CharacOption_PrintInfo(const CharacOption* record) {
    if (!record) {
        printf("CharacOption: NULL\n");
        return;
    }

    printf("CharacOption {\n");
    printf("  charac_no: %d\n", record->charac_no);
    printf("  options_len: %u bytes\n", record->options_len);
    printf("  best_clear_time_len: %u bytes\n", record->best_clear_time_len);
    printf("  blue_marble_enter_count: %u\n", (unsigned int)record->blue_marble_enter_count);
    printf("  charac_inform_notice: %s\n", record->charac_inform_notice);
    printf("}\n");
}

void CharacOption_Free(CharacOption* record) {
    if (!record) {
        return;
    }

    if (record->options) {
        free(record->options);
        record->options = NULL;
        record->options_len = 0;
    }

    if (record->best_clear_time) {
        free(record->best_clear_time);
        record->best_clear_time = NULL;
        record->best_clear_time_len = 0;
    }
}
