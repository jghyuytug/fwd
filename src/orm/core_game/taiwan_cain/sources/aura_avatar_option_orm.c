#include "aura_avatar_option_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int AuraAvatarOption_Add(DBConnectionManager* manager, const AuraAvatarOption* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "REPLACE INTO aura_avatar_option (charac_no, option_type, value_1) VALUES (%d, %d, %d)",
        record->charac_no, record->option_type, record->value_1);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AuraAvatarOption_Get(DBConnectionManager* manager, int charac_no, char option_type, AuraAvatarOption* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(AuraAvatarOption));

    snprintf(query, sizeof(query),
        "SELECT charac_no, option_type, value_1 FROM aura_avatar_option WHERE charac_no = %d AND option_type = %d",
        charac_no, option_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = atoi(values[0]);
        record->option_type = (char)atoi(values[1]);
        record->value_1 = atoi(values[2]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int AuraAvatarOption_Update(DBConnectionManager* manager, const AuraAvatarOption* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE aura_avatar_option SET value_1 = %d WHERE charac_no = %d AND option_type = %d",
        record->value_1, record->charac_no, record->option_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AuraAvatarOption_Delete(DBConnectionManager* manager, int charac_no, char option_type) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM aura_avatar_option WHERE charac_no = %d AND option_type = %d",
        charac_no, option_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AuraAvatarOption_Exists(DBConnectionManager* manager, int charac_no, char option_type) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM aura_avatar_option WHERE charac_no = %d AND option_type = %d",
        charac_no, option_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return (count > 0) ? 1 : 0;
}

/*******************************************************************************
 * 查询操作实现
 ******************************************************************************/

int AuraAvatarOption_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM aura_avatar_option");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int AuraAvatarOption_GetAll(DBConnectionManager* manager, AuraAvatarOption* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT charac_no, option_type, value_1 FROM aura_avatar_option ORDER BY charac_no, option_type LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = atoi(values[0]);
        records[count].option_type = (char)atoi(values[1]);
        records[count].value_1 = atoi(values[2]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int AuraAvatarOption_GetByCharacter(DBConnectionManager* manager, int charac_no, AuraAvatarOption* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT charac_no, option_type, value_1 FROM aura_avatar_option WHERE charac_no = %d ORDER BY option_type LIMIT %d",
        charac_no, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = atoi(values[0]);
        records[count].option_type = (char)atoi(values[1]);
        records[count].value_1 = atoi(values[2]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int AuraAvatarOption_DeleteByCharacter(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int deleted_count = 0;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    /* First get count */
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM aura_avatar_option WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        deleted_count = atoi(values[0]);
    }
    DBQueryResult_Free(&result);

    /* Then delete */
    memset(&result, 0, sizeof(DBQueryResult));
    snprintf(query, sizeof(query),
        "DELETE FROM aura_avatar_option WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return deleted_count;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void AuraAvatarOption_PrintInfo(const AuraAvatarOption* record) {
    if (!record) {
        printf("AuraAvatarOption: NULL\n");
        return;
    }

    printf("=== Aura Avatar Option ===\n");
    printf("  charac_no: %d\n", record->charac_no);
    printf("  option_type: %d\n", record->option_type);
    printf("  value_1: %d\n", record->value_1);
    printf("==========================\n");
}
