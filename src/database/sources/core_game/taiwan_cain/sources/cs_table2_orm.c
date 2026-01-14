#include "cs_table2_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ==================== CRUD 操作 ==================== */

int CsTable2_Add(DBConnectionManager* manager, const CsTable2* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "REPLACE INTO cs_table2 (account_id, charac_id) VALUES ('%s', '%s')",
        record->account_id, record->charac_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CsTable2_Get(DBConnectionManager* manager, const char* account_id, const char* charac_id, CsTable2* record) {
    if (!manager || !account_id || !charac_id || !record) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT account_id, charac_id FROM cs_table2 WHERE account_id = '%s' AND charac_id = '%s'",
        account_id, charac_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[2];
    int ret = -1;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(record->account_id, values[0], sizeof(record->account_id) - 1);
        record->account_id[sizeof(record->account_id) - 1] = '\0';
        strncpy(record->charac_id, values[1], sizeof(record->charac_id) - 1);
        record->charac_id[sizeof(record->charac_id) - 1] = '\0';
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int CsTable2_Delete(DBConnectionManager* manager, const char* account_id, const char* charac_id) {
    if (!manager || !account_id || !charac_id) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "DELETE FROM cs_table2 WHERE account_id = '%s' AND charac_id = '%s'",
        account_id, charac_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CsTable2_Exists(DBConnectionManager* manager, const char* account_id, const char* charac_id) {
    if (!manager || !account_id || !charac_id) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM cs_table2 WHERE account_id = '%s' AND charac_id = '%s'",
        account_id, charac_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int exists = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 专项操作 ==================== */

int CsTable2_GetByAccountId(DBConnectionManager* manager, const char* account_id, CsTable2* records, int max_count) {
    if (!manager || !account_id || !records || max_count <= 0) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT account_id, charac_id FROM cs_table2 WHERE account_id = '%s' LIMIT %d",
        account_id, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[2];
    int count = 0;

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_count) {
        strncpy(records[count].account_id, values[0], sizeof(records[count].account_id) - 1);
        records[count].account_id[sizeof(records[count].account_id) - 1] = '\0';
        strncpy(records[count].charac_id, values[1], sizeof(records[count].charac_id) - 1);
        records[count].charac_id[sizeof(records[count].charac_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CsTable2_GetByCharacId(DBConnectionManager* manager, const char* charac_id, CsTable2* records, int max_count) {
    if (!manager || !charac_id || !records || max_count <= 0) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT account_id, charac_id FROM cs_table2 WHERE charac_id = '%s' LIMIT %d",
        charac_id, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[2];
    int count = 0;

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_count) {
        strncpy(records[count].account_id, values[0], sizeof(records[count].account_id) - 1);
        records[count].account_id[sizeof(records[count].account_id) - 1] = '\0';
        strncpy(records[count].charac_id, values[1], sizeof(records[count].charac_id) - 1);
        records[count].charac_id[sizeof(records[count].charac_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CsTable2_DeleteByAccountId(DBConnectionManager* manager, const char* account_id) {
    if (!manager || !account_id) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "DELETE FROM cs_table2 WHERE account_id = '%s'", account_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int deleted = result.row_count;
    DBQueryResult_Free(&result);
    return deleted;
}

int CsTable2_DeleteByCharacId(DBConnectionManager* manager, const char* charac_id) {
    if (!manager || !charac_id) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "DELETE FROM cs_table2 WHERE charac_id = '%s'", charac_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int deleted = result.row_count;
    DBQueryResult_Free(&result);
    return deleted;
}

/* ==================== 查询操作 ==================== */

int CsTable2_CountAll(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[128];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM cs_table2");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int CsTable2_CountByAccountId(DBConnectionManager* manager, const char* account_id) {
    if (!manager || !account_id) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM cs_table2 WHERE account_id = '%s'", account_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int CsTable2_CountByCharacId(DBConnectionManager* manager, const char* charac_id) {
    if (!manager || !charac_id) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM cs_table2 WHERE charac_id = '%s'", charac_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int CsTable2_GetAll(DBConnectionManager* manager, int offset, int limit, CsTable2* records) {
    if (!manager || !records || offset < 0 || limit <= 0) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT account_id, charac_id FROM cs_table2 LIMIT %d OFFSET %d",
        limit, offset);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[2];
    int count = 0;

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < limit) {
        strncpy(records[count].account_id, values[0], sizeof(records[count].account_id) - 1);
        records[count].account_id[sizeof(records[count].account_id) - 1] = '\0';
        strncpy(records[count].charac_id, values[1], sizeof(records[count].charac_id) - 1);
        records[count].charac_id[sizeof(records[count].charac_id) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 工具函数 ==================== */

void CsTable2_PrintInfo(const CsTable2* record) {
    if (!record) {
        printf("  [CsTable2] NULL record\n");
        return;
    }

    printf("  ----------------------------------------\n");
    printf("  账户ID: %s\n", record->account_id);
    printf("  角色ID: %s\n", record->charac_id);
    printf("  ----------------------------------------\n");
}
