#include "charac_titlebook_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 400000

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

int CharacTitlebook_Add(DBConnectionManager* manager, const CharacTitlebook* record) {
    char* query;
    char* specific_value;
    char* general_value;
    char* despair_value;
    char* event_value;
    DBQueryResult result;
    char* specific_hex = NULL;
    char* general_hex = NULL;
    char* despair_hex = NULL;
    char* event_hex = NULL;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    query = (char*)malloc(MAX_QUERY_LEN);
    specific_value = (char*)malloc(MAX_QUERY_LEN / 5);
    general_value = (char*)malloc(MAX_QUERY_LEN / 5);
    despair_value = (char*)malloc(MAX_QUERY_LEN / 5);
    event_value = (char*)malloc(MAX_QUERY_LEN / 5);

    if (!query || !specific_value || !general_value || !despair_value || !event_value) {
        free(query);
        free(specific_value);
        free(general_value);
        free(despair_value);
        free(event_value);
        return -1;
    }

    specific_hex = bin_to_hex(record->specific_section, record->specific_section_len);
    general_hex = bin_to_hex(record->general_section, record->general_section_len);
    despair_hex = bin_to_hex(record->despair, record->despair_len);
    event_hex = bin_to_hex(record->event, record->event_len);

    if (specific_hex) {
        snprintf(specific_value, MAX_QUERY_LEN / 5, "0x%s", specific_hex);
    } else {
        strcpy(specific_value, "''");
    }

    if (general_hex) {
        snprintf(general_value, MAX_QUERY_LEN / 5, "0x%s", general_hex);
    } else {
        strcpy(general_value, "''");
    }

    if (despair_hex) {
        snprintf(despair_value, MAX_QUERY_LEN / 5, "0x%s", despair_hex);
    } else {
        strcpy(despair_value, "''");
    }

    if (event_hex) {
        snprintf(event_value, MAX_QUERY_LEN / 5, "0x%s", event_hex);
    } else {
        strcpy(event_value, "''");
    }

    snprintf(query, MAX_QUERY_LEN,
        "REPLACE INTO charac_titlebook "
        "(charac_no, specific_section, general_section, despair, event) "
        "VALUES (%u, %s, %s, %s, %s)",
        record->charac_no,
        specific_value, general_value, despair_value, event_value);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) >= 0) {
        DBQueryResult_Free(&result);
        ret = 0;
    }

    free(query);
    free(specific_value);
    free(general_value);
    free(despair_value);
    free(event_value);
    free(specific_hex);
    free(general_hex);
    free(despair_hex);
    free(event_hex);

    return ret;
}

int CharacTitlebook_Get(DBConnectionManager* manager, unsigned int charac_no, CharacTitlebook* record) {
    char query[4096];
    DBQueryResult result;
    char* values[16];

    if (!manager || !record) {
        return -1;
    }

    memset(record, 0, sizeof(CharacTitlebook));

    snprintf(query, sizeof(query),
        "SELECT charac_no, HEX(specific_section), HEX(general_section), HEX(despair), HEX(event) "
        "FROM charac_titlebook WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);

        /* 处理 specific_section BLOB */
        if (values[1] && strlen(values[1]) > 0) {
            record->specific_section = hex_to_bin(values[1], &record->specific_section_len);
        }

        /* 处理 general_section BLOB */
        if (values[2] && strlen(values[2]) > 0) {
            record->general_section = hex_to_bin(values[2], &record->general_section_len);
        }

        /* 处理 despair BLOB */
        if (values[3] && strlen(values[3]) > 0) {
            record->despair = hex_to_bin(values[3], &record->despair_len);
        }

        /* 处理 event BLOB */
        if (values[4] && strlen(values[4]) > 0) {
            record->event = hex_to_bin(values[4], &record->event_len);
        }

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacTitlebook_Update(DBConnectionManager* manager, const CharacTitlebook* record) {
    char* query;
    char* specific_value;
    char* general_value;
    char* despair_value;
    char* event_value;
    DBQueryResult result;
    char* specific_hex = NULL;
    char* general_hex = NULL;
    char* despair_hex = NULL;
    char* event_hex = NULL;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    query = (char*)malloc(MAX_QUERY_LEN);
    specific_value = (char*)malloc(MAX_QUERY_LEN / 5);
    general_value = (char*)malloc(MAX_QUERY_LEN / 5);
    despair_value = (char*)malloc(MAX_QUERY_LEN / 5);
    event_value = (char*)malloc(MAX_QUERY_LEN / 5);

    if (!query || !specific_value || !general_value || !despair_value || !event_value) {
        free(query);
        free(specific_value);
        free(general_value);
        free(despair_value);
        free(event_value);
        return -1;
    }

    specific_hex = bin_to_hex(record->specific_section, record->specific_section_len);
    general_hex = bin_to_hex(record->general_section, record->general_section_len);
    despair_hex = bin_to_hex(record->despair, record->despair_len);
    event_hex = bin_to_hex(record->event, record->event_len);

    if (specific_hex) {
        snprintf(specific_value, MAX_QUERY_LEN / 5, "0x%s", specific_hex);
    } else {
        strcpy(specific_value, "''");
    }

    if (general_hex) {
        snprintf(general_value, MAX_QUERY_LEN / 5, "0x%s", general_hex);
    } else {
        strcpy(general_value, "''");
    }

    if (despair_hex) {
        snprintf(despair_value, MAX_QUERY_LEN / 5, "0x%s", despair_hex);
    } else {
        strcpy(despair_value, "''");
    }

    if (event_hex) {
        snprintf(event_value, MAX_QUERY_LEN / 5, "0x%s", event_hex);
    } else {
        strcpy(event_value, "''");
    }

    snprintf(query, MAX_QUERY_LEN,
        "UPDATE charac_titlebook SET "
        "specific_section = %s, general_section = %s, despair = %s, event = %s "
        "WHERE charac_no = %u",
        specific_value, general_value, despair_value, event_value,
        record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) >= 0) {
        DBQueryResult_Free(&result);
        ret = 0;
    }

    free(query);
    free(specific_value);
    free(general_value);
    free(despair_value);
    free(event_value);
    free(specific_hex);
    free(general_hex);
    free(despair_hex);
    free(event_hex);

    return ret;
}

int CharacTitlebook_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[1024];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_titlebook WHERE charac_no = %u", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTitlebook_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[1024];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_titlebook WHERE charac_no = %u", charac_no);

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

int CharacTitlebook_UpdateSpecificSection(DBConnectionManager* manager, unsigned int charac_no,
                                           const unsigned char* data, unsigned int data_len) {
    char* query;
    char* value;
    DBQueryResult result;
    char* hex = NULL;
    int ret = -1;

    if (!manager) {
        return -1;
    }

    query = (char*)malloc(MAX_QUERY_LEN);
    value = (char*)malloc(MAX_QUERY_LEN / 2);

    if (!query || !value) {
        free(query);
        free(value);
        return -1;
    }

    hex = bin_to_hex(data, data_len);

    if (hex) {
        snprintf(value, MAX_QUERY_LEN / 2, "0x%s", hex);
    } else {
        strcpy(value, "''");
    }

    snprintf(query, MAX_QUERY_LEN,
        "UPDATE charac_titlebook SET specific_section = %s WHERE charac_no = %u",
        value, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) >= 0) {
        DBQueryResult_Free(&result);
        ret = 0;
    }

    free(query);
    free(value);
    free(hex);

    return ret;
}

int CharacTitlebook_UpdateGeneralSection(DBConnectionManager* manager, unsigned int charac_no,
                                          const unsigned char* data, unsigned int data_len) {
    char* query;
    char* value;
    DBQueryResult result;
    char* hex = NULL;
    int ret = -1;

    if (!manager) {
        return -1;
    }

    query = (char*)malloc(MAX_QUERY_LEN);
    value = (char*)malloc(MAX_QUERY_LEN / 2);

    if (!query || !value) {
        free(query);
        free(value);
        return -1;
    }

    hex = bin_to_hex(data, data_len);

    if (hex) {
        snprintf(value, MAX_QUERY_LEN / 2, "0x%s", hex);
    } else {
        strcpy(value, "''");
    }

    snprintf(query, MAX_QUERY_LEN,
        "UPDATE charac_titlebook SET general_section = %s WHERE charac_no = %u",
        value, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) >= 0) {
        DBQueryResult_Free(&result);
        ret = 0;
    }

    free(query);
    free(value);
    free(hex);

    return ret;
}

int CharacTitlebook_UpdateDespair(DBConnectionManager* manager, unsigned int charac_no,
                                   const unsigned char* data, unsigned int data_len) {
    char* query;
    char* value;
    DBQueryResult result;
    char* hex = NULL;
    int ret = -1;

    if (!manager) {
        return -1;
    }

    query = (char*)malloc(MAX_QUERY_LEN);
    value = (char*)malloc(MAX_QUERY_LEN / 2);

    if (!query || !value) {
        free(query);
        free(value);
        return -1;
    }

    hex = bin_to_hex(data, data_len);

    if (hex) {
        snprintf(value, MAX_QUERY_LEN / 2, "0x%s", hex);
    } else {
        strcpy(value, "''");
    }

    snprintf(query, MAX_QUERY_LEN,
        "UPDATE charac_titlebook SET despair = %s WHERE charac_no = %u",
        value, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) >= 0) {
        DBQueryResult_Free(&result);
        ret = 0;
    }

    free(query);
    free(value);
    free(hex);

    return ret;
}

int CharacTitlebook_UpdateEvent(DBConnectionManager* manager, unsigned int charac_no,
                                 const unsigned char* data, unsigned int data_len) {
    char* query;
    char* value;
    DBQueryResult result;
    char* hex = NULL;
    int ret = -1;

    if (!manager) {
        return -1;
    }

    query = (char*)malloc(MAX_QUERY_LEN);
    value = (char*)malloc(MAX_QUERY_LEN / 2);

    if (!query || !value) {
        free(query);
        free(value);
        return -1;
    }

    hex = bin_to_hex(data, data_len);

    if (hex) {
        snprintf(value, MAX_QUERY_LEN / 2, "0x%s", hex);
    } else {
        strcpy(value, "''");
    }

    snprintf(query, MAX_QUERY_LEN,
        "UPDATE charac_titlebook SET event = %s WHERE charac_no = %u",
        value, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) >= 0) {
        DBQueryResult_Free(&result);
        ret = 0;
    }

    free(query);
    free(value);
    free(hex);

    return ret;
}

/* ==================== 查询操作实现 ==================== */

int CharacTitlebook_GetTotalCount(DBConnectionManager* manager) {
    char query[512];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_titlebook");

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

/* ==================== 工具函数实现 ==================== */

void CharacTitlebook_PrintInfo(const CharacTitlebook* record) {
    if (!record) {
        printf("CharacTitlebook: NULL\n");
        return;
    }

    printf("CharacTitlebook {\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  specific_section_len: %u bytes\n", record->specific_section_len);
    printf("  general_section_len: %u bytes\n", record->general_section_len);
    printf("  despair_len: %u bytes\n", record->despair_len);
    printf("  event_len: %u bytes\n", record->event_len);
    printf("}\n");
}

void CharacTitlebook_Free(CharacTitlebook* record) {
    if (!record) {
        return;
    }

    if (record->specific_section) {
        free(record->specific_section);
        record->specific_section = NULL;
        record->specific_section_len = 0;
    }

    if (record->general_section) {
        free(record->general_section);
        record->general_section = NULL;
        record->general_section_len = 0;
    }

    if (record->despair) {
        free(record->despair);
        record->despair = NULL;
        record->despair_len = 0;
    }

    if (record->event) {
        free(record->event);
        record->event = NULL;
        record->event_len = 0;
    }
}
