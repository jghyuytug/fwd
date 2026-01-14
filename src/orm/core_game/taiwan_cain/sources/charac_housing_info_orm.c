#include "charac_housing_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192
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

int CharacHousingInfo_Add(DBConnectionManager* manager, const CharacHousingInfo* record) {
    char query[MAX_QUERY_LEN];
    char hex_decoration[MAX_DECORATION_INVEN_LEN * 2 + 1];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    bin_to_hex(record->decoration_inven, record->decoration_inven_len, hex_decoration);

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_housing_info (charac_no, installed, decoration_inven, version) "
        "VALUES (%u, %u, UNHEX('%s'), %u)",
        record->charac_no, record->installed, hex_decoration, record->version);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingInfo_Get(DBConnectionManager* manager, unsigned int charac_no,
                          CharacHousingInfo* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, installed, HEX(decoration_inven), version "
        "FROM charac_housing_info WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);
        record->installed = (unsigned short)atoi(values[1]);

        if (values[2]) {
            record->decoration_inven_len = hex_to_bin(values[2], record->decoration_inven,
                                                      MAX_DECORATION_INVEN_LEN);
        } else {
            record->decoration_inven_len = 0;
        }

        record->version = (unsigned short)atoi(values[3]);

        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int CharacHousingInfo_Update(DBConnectionManager* manager, const CharacHousingInfo* record) {
    char query[MAX_QUERY_LEN];
    char hex_decoration[MAX_DECORATION_INVEN_LEN * 2 + 1];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    bin_to_hex(record->decoration_inven, record->decoration_inven_len, hex_decoration);

    snprintf(query, sizeof(query),
        "UPDATE charac_housing_info SET installed = %u, decoration_inven = UNHEX('%s'), "
        "version = %u WHERE charac_no = %u",
        record->installed, hex_decoration, record->version, record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingInfo_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_housing_info WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingInfo_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int exists = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM charac_housing_info WHERE charac_no = %u LIMIT 1",
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

int CharacHousingInfo_UpdateDecorationInven(DBConnectionManager* manager,
                                            unsigned int charac_no,
                                            const unsigned char* decoration_inven,
                                            int decoration_inven_len) {
    char query[MAX_QUERY_LEN];
    char hex_decoration[MAX_DECORATION_INVEN_LEN * 2 + 1];
    DBQueryResult result;

    if (!manager || !decoration_inven || decoration_inven_len <= 0) {
        return -1;
    }

    bin_to_hex(decoration_inven, decoration_inven_len, hex_decoration);

    snprintf(query, sizeof(query),
        "UPDATE charac_housing_info SET decoration_inven = UNHEX('%s') WHERE charac_no = %u",
        hex_decoration, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingInfo_UpdateInstalled(DBConnectionManager* manager,
                                      unsigned int charac_no,
                                      unsigned short installed) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_housing_info SET installed = %u WHERE charac_no = %u",
        installed, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingInfo_UpdateVersion(DBConnectionManager* manager,
                                    unsigned int charac_no,
                                    unsigned short version) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_housing_info SET version = %u WHERE charac_no = %u",
        version, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacHousingInfo_IncrementInstalled(DBConnectionManager* manager,
                                         unsigned int charac_no,
                                         unsigned int increment) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_housing_info SET installed = installed + %u WHERE charac_no = %u",
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

void CharacHousingInfo_PrintInfo(const CharacHousingInfo* record) {
    int i;

    if (!record) {
        printf("CharacHousingInfo record is NULL\n");
        return;
    }

    printf("=== CharacHousingInfo Record ===\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  installed: %u\n", record->installed);
    printf("  decoration_inven length: %d bytes\n", record->decoration_inven_len);

    if (record->decoration_inven_len > 0) {
        printf("  decoration_inven (hex, first 64 bytes): ");
        for (i = 0; i < record->decoration_inven_len && i < 64; i++) {
            printf("%02x", record->decoration_inven[i]);
        }
        printf("\n");
    }

    printf("  version: %u\n", record->version);
    printf("==================================\n");
}
