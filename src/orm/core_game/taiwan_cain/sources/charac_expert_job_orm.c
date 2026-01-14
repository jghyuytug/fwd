#include "charac_expert_job_orm.h"
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

int CharacExpertJob_Add(DBConnectionManager* manager, const CharacExpertJob* record) {
    char query[MAX_QUERY_LEN];
    char hex_recipe[MAX_RECIPE_LEN * 2 + 1];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    bin_to_hex(record->recipe, record->recipe_len, hex_recipe);

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_expert_job (charac_no, expert_job_giveup_cnt, "
        "expert_job_info, expert_job_info_ex, recipe) "
        "VALUES (%u, %u, %d, %d, UNHEX('%s'))",
        record->charac_no, record->expert_job_giveup_cnt,
        record->expert_job_info, record->expert_job_info_ex, hex_recipe);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacExpertJob_Get(DBConnectionManager* manager, unsigned int charac_no,
                        CharacExpertJob* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, expert_job_giveup_cnt, expert_job_info, "
        "expert_job_info_ex, HEX(recipe) "
        "FROM charac_expert_job WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);
        record->expert_job_giveup_cnt = (unsigned char)atoi(values[1]);
        record->expert_job_info = atoi(values[2]);
        record->expert_job_info_ex = atoi(values[3]);

        if (values[4]) {
            record->recipe_len = hex_to_bin(values[4], record->recipe, MAX_RECIPE_LEN);
        } else {
            record->recipe_len = 0;
        }

        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int CharacExpertJob_Update(DBConnectionManager* manager, unsigned int charac_no,
                           int expert_job_info, int expert_job_info_ex) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_expert_job SET expert_job_info = %d, expert_job_info_ex = %d "
        "WHERE charac_no = %u",
        expert_job_info, expert_job_info_ex, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacExpertJob_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_expert_job WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacExpertJob_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int exists = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM charac_expert_job WHERE charac_no = %u LIMIT 1",
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

int CharacExpertJob_UpdateRecipe(DBConnectionManager* manager, unsigned int charac_no,
                                 const unsigned char* recipe, int recipe_len) {
    char query[MAX_QUERY_LEN];
    char hex_recipe[MAX_RECIPE_LEN * 2 + 1];
    DBQueryResult result;

    if (!manager || !recipe || recipe_len <= 0) {
        return -1;
    }

    bin_to_hex(recipe, recipe_len, hex_recipe);

    snprintf(query, sizeof(query),
        "UPDATE charac_expert_job SET recipe = UNHEX('%s') WHERE charac_no = %u",
        hex_recipe, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacExpertJob_IncrementGiveupCnt(DBConnectionManager* manager,
                                       unsigned int charac_no,
                                       unsigned int increment) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_expert_job SET expert_job_giveup_cnt = expert_job_giveup_cnt + %u "
        "WHERE charac_no = %u",
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

void CharacExpertJob_PrintInfo(const CharacExpertJob* record) {
    int i;

    if (!record) {
        printf("CharacExpertJob record is NULL\n");
        return;
    }

    printf("=== CharacExpertJob Record ===\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  expert_job_giveup_cnt: %u\n", record->expert_job_giveup_cnt);
    printf("  expert_job_info: %d\n", record->expert_job_info);
    printf("  expert_job_info_ex: %d\n", record->expert_job_info_ex);
    printf("  recipe length: %d bytes\n", record->recipe_len);

    if (record->recipe_len > 0) {
        printf("  recipe (hex, first 64 bytes): ");
        for (i = 0; i < record->recipe_len && i < 64; i++) {
            printf("%02x", record->recipe[i]);
        }
        printf("\n");
    }

    printf("==============================\n");
}
