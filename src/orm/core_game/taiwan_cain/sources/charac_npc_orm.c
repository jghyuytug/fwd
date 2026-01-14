#include "charac_npc_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 16384
#define MAX_FIELDS 16

/*******************************************************************************
 * 辅助函数
 ******************************************************************************/

static void bin_to_hex(const unsigned char* bin, int bin_len, char* hex) {
    int i;
    for (i = 0; i < bin_len; i++) {
        sprintf(hex + (i * 2), "%02x", bin[i]);
    }
    hex[bin_len * 2] = '\0';
}

static int hex_to_bin(const char* hex, unsigned char* bin, int max_bin_len) {
    int hex_len = strlen(hex);
    int bin_len = hex_len / 2;
    int i;

    if (bin_len > max_bin_len) {
        bin_len = max_bin_len;
    }

    for (i = 0; i < bin_len; i++) {
        sscanf(hex + (i * 2), "%2hhx", &bin[i]);
    }

    return bin_len;
}

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int CharacNpc_Add(DBConnectionManager* manager, const CharacNpc* record) {
    char query[MAX_QUERY_LEN];
    char hex_npc_data[MAX_NPC_DATA_LEN * 2 + 1];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    bin_to_hex(record->npc_data, record->npc_data_len, hex_npc_data);

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_npc (charac_no, npc_cnt, npc_data) "
        "VALUES (%u, %u, UNHEX('%s'))",
        record->charac_no, record->npc_cnt, hex_npc_data);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacNpc_Get(DBConnectionManager* manager, unsigned int charac_no,
                  CharacNpc* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, npc_cnt, HEX(npc_data) "
        "FROM charac_npc WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);
        record->npc_cnt = (unsigned char)atoi(values[1]);

        if (values[2]) {
            record->npc_data_len = hex_to_bin(values[2], record->npc_data, MAX_NPC_DATA_LEN);
        } else {
            record->npc_data_len = 0;
        }

        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int CharacNpc_Update(DBConnectionManager* manager, const CharacNpc* record) {
    char query[MAX_QUERY_LEN];
    char hex_npc_data[MAX_NPC_DATA_LEN * 2 + 1];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    bin_to_hex(record->npc_data, record->npc_data_len, hex_npc_data);

    snprintf(query, sizeof(query),
        "UPDATE charac_npc SET npc_cnt = %u, npc_data = UNHEX('%s') "
        "WHERE charac_no = %u",
        record->npc_cnt, hex_npc_data, record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacNpc_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_npc WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacNpc_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int exists = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM charac_npc WHERE charac_no = %u LIMIT 1",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = 1;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/*******************************************************************************
 * 业务操作实现
 ******************************************************************************/

int CharacNpc_UpdateNpcData(DBConnectionManager* manager, unsigned int charac_no,
                            const unsigned char* npc_data, int npc_data_len) {
    char query[MAX_QUERY_LEN];
    char hex_npc_data[MAX_NPC_DATA_LEN * 2 + 1];
    DBQueryResult result;

    if (!manager || !npc_data || npc_data_len <= 0) {
        return -1;
    }

    bin_to_hex(npc_data, npc_data_len, hex_npc_data);

    snprintf(query, sizeof(query),
        "UPDATE charac_npc SET npc_data = UNHEX('%s') WHERE charac_no = %u",
        hex_npc_data, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacNpc_UpdateNpcCnt(DBConnectionManager* manager, unsigned int charac_no,
                          unsigned char npc_cnt) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_npc SET npc_cnt = %u WHERE charac_no = %u",
        npc_cnt, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacNpc_IncrementNpcCnt(DBConnectionManager* manager, unsigned int charac_no,
                             unsigned int increment) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_npc SET npc_cnt = npc_cnt + %u WHERE charac_no = %u",
        increment, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void CharacNpc_PrintInfo(const CharacNpc* record) {
    int i;

    if (!record) {
        printf("CharacNpc record is NULL\n");
        return;
    }

    printf("=== CharacNpc Record ===\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  npc_cnt: %u\n", record->npc_cnt);
    printf("  npc_data length: %d bytes\n", record->npc_data_len);

    if (record->npc_data_len > 0) {
        printf("  npc_data (hex, first 64 bytes): ");
        for (i = 0; i < record->npc_data_len && i < 64; i++) {
            printf("%02x", record->npc_data[i]);
        }
        printf("\n");
    }

    printf("===========================\n");
}
