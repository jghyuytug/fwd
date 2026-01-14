#include "charac_action_point_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192
#define MAX_FIELDS 16
#define MAX_HEX_LEN 131072  /* 64KB blob = 128KB hex string */

/* ==================== CRUD 操作 ==================== */

int CharacActionPoint_Add(DBConnectionManager* manager, const CharacActionPoint* record) {
    char query[MAX_QUERY_LEN];
    char* hex_clear_state = NULL;
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    /* 为BLOB字段转换为十六进制字符串 */
    if (record->ap_clear_state && record->ap_clear_state_len > 0) {
        hex_clear_state = (char*)malloc(record->ap_clear_state_len * 2 + 1);
        if (!hex_clear_state) {
            return -1;
        }

        int i;
        for (i = 0; i < record->ap_clear_state_len; i++) {
            sprintf(hex_clear_state + (i * 2), "%02x", record->ap_clear_state[i]);
        }
        hex_clear_state[record->ap_clear_state_len * 2] = '\0';
    }

    if (hex_clear_state) {
        snprintf(query, sizeof(query),
            "REPLACE INTO charac_action_point "
            "(occ_date, charac_no, ap_sum, is_reward_medal, "
            "is_reward_item_1, is_reward_item_2, is_reward_item_3, is_reward_item_4, ap_clear_state) "
            "VALUES ('%s', %u, %u, %u, %u, %u, %u, %u, UNHEX('%s'))",
            record->occ_date, record->charac_no, record->ap_sum, record->is_reward_medal,
            record->is_reward_item_1, record->is_reward_item_2, record->is_reward_item_3,
            record->is_reward_item_4, hex_clear_state);
    } else {
        snprintf(query, sizeof(query),
            "REPLACE INTO charac_action_point "
            "(occ_date, charac_no, ap_sum, is_reward_medal, "
            "is_reward_item_1, is_reward_item_2, is_reward_item_3, is_reward_item_4, ap_clear_state) "
            "VALUES ('%s', %u, %u, %u, %u, %u, %u, %u, '')",
            record->occ_date, record->charac_no, record->ap_sum, record->is_reward_medal,
            record->is_reward_item_1, record->is_reward_item_2, record->is_reward_item_3,
            record->is_reward_item_4);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        if (hex_clear_state) free(hex_clear_state);
        return -1;
    }

    DBQueryResult_Free(&result);
    if (hex_clear_state) free(hex_clear_state);
    return 0;
}

int CharacActionPoint_Get(DBConnectionManager* manager, unsigned int charac_no,
                          const char* occ_date, CharacActionPoint* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager || !occ_date || !record) {
        return -1;
    }

    memset(record, 0, sizeof(CharacActionPoint));

    snprintf(query, sizeof(query),
        "SELECT occ_date, charac_no, ap_sum, is_reward_medal, is_reward_item_1, "
        "is_reward_item_2, is_reward_item_3, is_reward_item_4, HEX(ap_clear_state) "
        "FROM charac_action_point WHERE charac_no = %u AND occ_date = '%s'",
        charac_no, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(record->occ_date, values[0], sizeof(record->occ_date) - 1);
        record->occ_date[sizeof(record->occ_date) - 1] = '\0';
        record->charac_no = (unsigned int)atoi(values[1]);
        record->ap_sum = (unsigned int)atoi(values[2]);
        record->is_reward_medal = (unsigned char)atoi(values[3]);
        record->is_reward_item_1 = (unsigned char)atoi(values[4]);
        record->is_reward_item_2 = (unsigned char)atoi(values[5]);
        record->is_reward_item_3 = (unsigned char)atoi(values[6]);
        record->is_reward_item_4 = (unsigned char)atoi(values[7]);

        /* 处理BLOB字段 - HEX格式转换为二进制 */
        if (values[8] && strlen(values[8]) > 0) {
            int hex_len = strlen(values[8]);
            int bin_len = hex_len / 2;

            record->ap_clear_state = (unsigned char*)malloc(bin_len);
            if (record->ap_clear_state) {
                int i;
                for (i = 0; i < bin_len; i++) {
                    sscanf(values[8] + (i * 2), "%2hhx", &record->ap_clear_state[i]);
                }
                record->ap_clear_state_len = bin_len;
            }
        }

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacActionPoint_Update(DBConnectionManager* manager, const CharacActionPoint* record) {
    char query[MAX_QUERY_LEN];
    char* hex_clear_state = NULL;
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    /* 为BLOB字段转换为十六进制字符串 */
    if (record->ap_clear_state && record->ap_clear_state_len > 0) {
        hex_clear_state = (char*)malloc(record->ap_clear_state_len * 2 + 1);
        if (!hex_clear_state) {
            return -1;
        }

        int i;
        for (i = 0; i < record->ap_clear_state_len; i++) {
            sprintf(hex_clear_state + (i * 2), "%02x", record->ap_clear_state[i]);
        }
        hex_clear_state[record->ap_clear_state_len * 2] = '\0';
    }

    if (hex_clear_state) {
        snprintf(query, sizeof(query),
            "UPDATE charac_action_point SET "
            "ap_sum = %u, is_reward_medal = %u, "
            "is_reward_item_1 = %u, is_reward_item_2 = %u, "
            "is_reward_item_3 = %u, is_reward_item_4 = %u, "
            "ap_clear_state = UNHEX('%s') "
            "WHERE charac_no = %u AND occ_date = '%s'",
            record->ap_sum, record->is_reward_medal,
            record->is_reward_item_1, record->is_reward_item_2,
            record->is_reward_item_3, record->is_reward_item_4,
            hex_clear_state, record->charac_no, record->occ_date);
    } else {
        snprintf(query, sizeof(query),
            "UPDATE charac_action_point SET "
            "ap_sum = %u, is_reward_medal = %u, "
            "is_reward_item_1 = %u, is_reward_item_2 = %u, "
            "is_reward_item_3 = %u, is_reward_item_4 = %u, "
            "ap_clear_state = '' "
            "WHERE charac_no = %u AND occ_date = '%s'",
            record->ap_sum, record->is_reward_medal,
            record->is_reward_item_1, record->is_reward_item_2,
            record->is_reward_item_3, record->is_reward_item_4,
            record->charac_no, record->occ_date);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        if (hex_clear_state) free(hex_clear_state);
        return -1;
    }

    DBQueryResult_Free(&result);
    if (hex_clear_state) free(hex_clear_state);
    return 0;
}

int CharacActionPoint_Delete(DBConnectionManager* manager, unsigned int charac_no,
                             const char* occ_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_date) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_action_point WHERE charac_no = %u AND occ_date = '%s'",
        charac_no, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacActionPoint_Exists(DBConnectionManager* manager, unsigned int charac_no,
                             const char* occ_date) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager || !occ_date) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_action_point WHERE charac_no = %u AND occ_date = '%s'",
        charac_no, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return count > 0;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 专项操作 ==================== */

int CharacActionPoint_IncrementAPSum(DBConnectionManager* manager, unsigned int charac_no,
                                     const char* occ_date, unsigned int ap_value) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_date) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_action_point SET ap_sum = ap_sum + %u "
        "WHERE charac_no = %u AND occ_date = '%s'",
        ap_value, charac_no, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacActionPoint_ClaimRewardMedal(DBConnectionManager* manager, unsigned int charac_no,
                                       const char* occ_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_date) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_action_point SET is_reward_medal = 1 "
        "WHERE charac_no = %u AND occ_date = '%s'",
        charac_no, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacActionPoint_ClaimRewardItem(DBConnectionManager* manager, unsigned int charac_no,
                                      const char* occ_date, int item_slot) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_date || item_slot < 1 || item_slot > 4) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_action_point SET is_reward_item_%d = 1 "
        "WHERE charac_no = %u AND occ_date = '%s'",
        item_slot, charac_no, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacActionPoint_UpdateAPClearState(DBConnectionManager* manager, unsigned int charac_no,
                                         const char* occ_date,
                                         const unsigned char* clear_state, int state_len) {
    char query[MAX_QUERY_LEN];
    char* hex_clear_state = NULL;
    DBQueryResult result;

    if (!manager || !occ_date || !clear_state || state_len <= 0) {
        return -1;
    }

    /* 为BLOB字段转换为十六进制字符串 */
    hex_clear_state = (char*)malloc(state_len * 2 + 1);
    if (!hex_clear_state) {
        return -1;
    }

    int i;
    for (i = 0; i < state_len; i++) {
        sprintf(hex_clear_state + (i * 2), "%02x", clear_state[i]);
    }
    hex_clear_state[state_len * 2] = '\0';

    snprintf(query, sizeof(query),
        "UPDATE charac_action_point SET ap_clear_state = UNHEX('%s') "
        "WHERE charac_no = %u AND occ_date = '%s'",
        hex_clear_state, charac_no, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        free(hex_clear_state);
        return -1;
    }

    DBQueryResult_Free(&result);
    free(hex_clear_state);
    return 0;
}

/* ==================== 查询操作 ==================== */

int CharacActionPoint_GetByDate(DBConnectionManager* manager, const char* occ_date,
                                CharacActionPoint* records, int max_result) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !occ_date || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT occ_date, charac_no, ap_sum, is_reward_medal, is_reward_item_1, "
        "is_reward_item_2, is_reward_item_3, is_reward_item_4, HEX(ap_clear_state) "
        "FROM charac_action_point WHERE occ_date = '%s' ORDER BY ap_sum DESC LIMIT %d",
        occ_date, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_result && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(records[count].occ_date, values[0], sizeof(records[count].occ_date) - 1);
        records[count].occ_date[sizeof(records[count].occ_date) - 1] = '\0';
        records[count].charac_no = (unsigned int)atoi(values[1]);
        records[count].ap_sum = (unsigned int)atoi(values[2]);
        records[count].is_reward_medal = (unsigned char)atoi(values[3]);
        records[count].is_reward_item_1 = (unsigned char)atoi(values[4]);
        records[count].is_reward_item_2 = (unsigned char)atoi(values[5]);
        records[count].is_reward_item_3 = (unsigned char)atoi(values[6]);
        records[count].is_reward_item_4 = (unsigned char)atoi(values[7]);

        /* 处理BLOB字段 */
        records[count].ap_clear_state = NULL;
        records[count].ap_clear_state_len = 0;
        if (values[8] && strlen(values[8]) > 0) {
            int hex_len = strlen(values[8]);
            int bin_len = hex_len / 2;

            records[count].ap_clear_state = (unsigned char*)malloc(bin_len);
            if (records[count].ap_clear_state) {
                int i;
                for (i = 0; i < bin_len; i++) {
                    sscanf(values[8] + (i * 2), "%2hhx", &records[count].ap_clear_state[i]);
                }
                records[count].ap_clear_state_len = bin_len;
            }
        }
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacActionPoint_GetByCharacNo(DBConnectionManager* manager, unsigned int charac_no,
                                    CharacActionPoint* records, int max_result) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT occ_date, charac_no, ap_sum, is_reward_medal, is_reward_item_1, "
        "is_reward_item_2, is_reward_item_3, is_reward_item_4, HEX(ap_clear_state) "
        "FROM charac_action_point WHERE charac_no = %u ORDER BY occ_date DESC LIMIT %d",
        charac_no, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_result && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(records[count].occ_date, values[0], sizeof(records[count].occ_date) - 1);
        records[count].occ_date[sizeof(records[count].occ_date) - 1] = '\0';
        records[count].charac_no = (unsigned int)atoi(values[1]);
        records[count].ap_sum = (unsigned int)atoi(values[2]);
        records[count].is_reward_medal = (unsigned char)atoi(values[3]);
        records[count].is_reward_item_1 = (unsigned char)atoi(values[4]);
        records[count].is_reward_item_2 = (unsigned char)atoi(values[5]);
        records[count].is_reward_item_3 = (unsigned char)atoi(values[6]);
        records[count].is_reward_item_4 = (unsigned char)atoi(values[7]);

        /* 处理BLOB字段 */
        records[count].ap_clear_state = NULL;
        records[count].ap_clear_state_len = 0;
        if (values[8] && strlen(values[8]) > 0) {
            int hex_len = strlen(values[8]);
            int bin_len = hex_len / 2;

            records[count].ap_clear_state = (unsigned char*)malloc(bin_len);
            if (records[count].ap_clear_state) {
                int i;
                for (i = 0; i < bin_len; i++) {
                    sscanf(values[8] + (i * 2), "%2hhx", &records[count].ap_clear_state[i]);
                }
                records[count].ap_clear_state_len = bin_len;
            }
        }
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacActionPoint_CheckRewardStatus(DBConnectionManager* manager, unsigned int charac_no,
                                        const char* occ_date,
                                        int* medal_claimed, int* items_claimed) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager || !occ_date || !medal_claimed || !items_claimed) {
        return -1;
    }

    *medal_claimed = 0;
    *items_claimed = 0;

    snprintf(query, sizeof(query),
        "SELECT is_reward_medal, is_reward_item_1, is_reward_item_2, "
        "is_reward_item_3, is_reward_item_4 "
        "FROM charac_action_point WHERE charac_no = %u AND occ_date = '%s'",
        charac_no, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        *medal_claimed = atoi(values[0]);
        *items_claimed = atoi(values[1]) + atoi(values[2]) +
                         atoi(values[3]) + atoi(values[4]);

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacActionPoint_GetByMinAPSum(DBConnectionManager* manager, const char* occ_date,
                                    unsigned int min_ap,
                                    CharacActionPoint* records, int max_result) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !occ_date || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT occ_date, charac_no, ap_sum, is_reward_medal, is_reward_item_1, "
        "is_reward_item_2, is_reward_item_3, is_reward_item_4, HEX(ap_clear_state) "
        "FROM charac_action_point WHERE occ_date = '%s' AND ap_sum >= %u "
        "ORDER BY ap_sum DESC LIMIT %d",
        occ_date, min_ap, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_result && DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(records[count].occ_date, values[0], sizeof(records[count].occ_date) - 1);
        records[count].occ_date[sizeof(records[count].occ_date) - 1] = '\0';
        records[count].charac_no = (unsigned int)atoi(values[1]);
        records[count].ap_sum = (unsigned int)atoi(values[2]);
        records[count].is_reward_medal = (unsigned char)atoi(values[3]);
        records[count].is_reward_item_1 = (unsigned char)atoi(values[4]);
        records[count].is_reward_item_2 = (unsigned char)atoi(values[5]);
        records[count].is_reward_item_3 = (unsigned char)atoi(values[6]);
        records[count].is_reward_item_4 = (unsigned char)atoi(values[7]);

        /* 处理BLOB字段 */
        records[count].ap_clear_state = NULL;
        records[count].ap_clear_state_len = 0;
        if (values[8] && strlen(values[8]) > 0) {
            int hex_len = strlen(values[8]);
            int bin_len = hex_len / 2;

            records[count].ap_clear_state = (unsigned char*)malloc(bin_len);
            if (records[count].ap_clear_state) {
                int i;
                for (i = 0; i < bin_len; i++) {
                    sscanf(values[8] + (i * 2), "%2hhx", &records[count].ap_clear_state[i]);
                }
                records[count].ap_clear_state_len = bin_len;
            }
        }
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 工具函数 ==================== */

void CharacActionPoint_PrintInfo(const CharacActionPoint* record) {
    if (!record) {
        printf("CharacActionPoint: NULL\n");
        return;
    }

    printf("\nCharacActionPoint {\n");
    printf("  occ_date: %s\n", record->occ_date);
    printf("  charac_no: %u\n", record->charac_no);
    printf("  ap_sum: %u\n", record->ap_sum);
    printf("  is_reward_medal: %u\n", record->is_reward_medal);
    printf("  is_reward_item_1: %u\n", record->is_reward_item_1);
    printf("  is_reward_item_2: %u\n", record->is_reward_item_2);
    printf("  is_reward_item_3: %u\n", record->is_reward_item_3);
    printf("  is_reward_item_4: %u\n", record->is_reward_item_4);
    printf("  ap_clear_state_len: %d bytes\n", record->ap_clear_state_len);

    if (record->ap_clear_state && record->ap_clear_state_len > 0) {
        int i;
        int display_len = record->ap_clear_state_len < 32 ? record->ap_clear_state_len : 32;
        printf("  ap_clear_state (hex, first %d bytes): ", display_len);
        for (i = 0; i < display_len; i++) {
            printf("%02x", record->ap_clear_state[i]);
        }
        if (record->ap_clear_state_len > 32) {
            printf("... (%d more bytes)", record->ap_clear_state_len - 32);
        }
        printf("\n");
    }

    printf("}\n");
}

void CharacActionPoint_Free(CharacActionPoint* record) {
    if (record && record->ap_clear_state) {
        free(record->ap_clear_state);
        record->ap_clear_state = NULL;
        record->ap_clear_state_len = 0;
    }
}
