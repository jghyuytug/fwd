#include "member_key_option_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 16384

static char* bin_to_hex(const unsigned char* data, size_t len) {
    char* hex = (char*)malloc(len * 2 + 1);
    if (!hex) return NULL;

    for (size_t i = 0; i < len; i++) {
        sprintf(hex + i * 2, "%02X", data[i]);
    }
    hex[len * 2] = '\0';
    return hex;
}

static unsigned char* hex_to_bin(const char* hex, size_t* out_len) {
    size_t hex_len = strlen(hex);
    if (hex_len % 2 != 0) return NULL;

    size_t bin_len = hex_len / 2;
    unsigned char* bin = (unsigned char*)malloc(bin_len);
    if (!bin) return NULL;

    for (size_t i = 0; i < bin_len; i++) {
        unsigned int byte;
        if (sscanf(hex + i * 2, "%2X", &byte) != 1) {
            free(bin);
            return NULL;
        }
        bin[i] = (unsigned char)byte;
    }

    *out_len = bin_len;
    return bin;
}

int MemberKeyOption_Add(DBConnectionManager* manager, const MemberKeyOption* option) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* hex_data;

    if (!manager || !option || !option->key_option) return -1;

    hex_data = bin_to_hex(option->key_option, option->key_option_len);
    if (!hex_data) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_key_option (m_id, key_type, key_option) "
        "VALUES (%llu, %u, UNHEX('%s'))",
        option->m_id, option->key_type, hex_data);

    free(hex_data);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberKeyOption_Get(DBConnectionManager* manager, unsigned long long m_id,
                        unsigned char key_type, MemberKeyOption* option) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !option) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, key_type, HEX(key_option) FROM member_key_option "
        "WHERE m_id = %llu AND key_type = %u",
        m_id, key_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(option, 0, sizeof(MemberKeyOption));
    option->m_id = row[0] ? strtoull(row[0], NULL, 10) : 0;
    option->key_type = row[1] ? (unsigned char)atoi(row[1]) : 0;

    if (row[2] && row[2][0]) {
        option->key_option = hex_to_bin(row[2], &option->key_option_len);
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberKeyOption_Update(DBConnectionManager* manager, const MemberKeyOption* option) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* hex_data;

    if (!manager || !option || !option->key_option) return -1;

    hex_data = bin_to_hex(option->key_option, option->key_option_len);
    if (!hex_data) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_key_option SET key_option = UNHEX('%s') "
        "WHERE m_id = %llu AND key_type = %u",
        hex_data, option->m_id, option->key_type);

    free(hex_data);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberKeyOption_Delete(DBConnectionManager* manager, unsigned long long m_id,
                           unsigned char key_type) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_key_option WHERE m_id = %llu AND key_type = %u",
        m_id, key_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberKeyOption_Exists(DBConnectionManager* manager, unsigned long long m_id,
                           unsigned char key_type) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_key_option WHERE m_id = %llu AND key_type = %u",
        m_id, key_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int MemberKeyOption_GetByMemberId(DBConnectionManager* manager, unsigned long long m_id,
                                   MemberKeyOption* options, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !options || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, key_type, HEX(key_option) FROM member_key_option "
        "WHERE m_id = %llu ORDER BY key_type", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&options[count], 0, sizeof(MemberKeyOption));
        options[count].m_id = row[0] ? strtoull(row[0], NULL, 10) : 0;
        options[count].key_type = row[1] ? (unsigned char)atoi(row[1]) : 0;

        if (row[2] && row[2][0]) {
            options[count].key_option = hex_to_bin(row[2], &options[count].key_option_len);
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberKeyOption_DeleteByMemberId(DBConnectionManager* manager, unsigned long long m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_key_option WHERE m_id = %llu", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void MemberKeyOption_Free(MemberKeyOption* option) {
    if (option && option->key_option) {
        free(option->key_option);
        option->key_option = NULL;
        option->key_option_len = 0;
    }
}

void MemberKeyOption_FreeArray(MemberKeyOption* options, int count) {
    if (!options) return;
    for (int i = 0; i < count; i++) {
        MemberKeyOption_Free(&options[i]);
    }
}

void MemberKeyOption_PrintInfo(const MemberKeyOption* option) {
    if (!option) {
        printf("MemberKeyOption: NULL\n");
        return;
    }

    printf("MemberKeyOption:\n");
    printf("  m_id: %llu\n", option->m_id);
    printf("  key_type: %u\n", option->key_type);
    printf("  key_option_len: %zu bytes\n", option->key_option_len);

    if (option->key_option && option->key_option_len > 0) {
        printf("  key_option (hex): ");
        for (size_t i = 0; i < (option->key_option_len < 32 ? option->key_option_len : 32); i++) {
            printf("%02X ", option->key_option[i]);
        }
        if (option->key_option_len > 32) printf("...");
        printf("\n");
    } else {
        printf("  key_option: NULL\n");
    }
}
