#include "charac_members_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 2048
#define MAX_FIELDS 16

/* ==================== CRUD 操作实现 ==================== */

int CharacMembers_Add(DBConnectionManager* manager, const CharacMembers* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_members "
        "(charac_no, master_no, exp, create_time, delete_time) "
        "VALUES (%d, %d, %d, '%s', '%s')",
        record->charac_no, record->master_no, record->exp,
        record->create_time, record->delete_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacMembers_Get(DBConnectionManager* manager, int charac_no, CharacMembers* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];

    if (!manager || !record) {
        return -1;
    }

    memset(record, 0, sizeof(CharacMembers));

    snprintf(query, sizeof(query),
        "SELECT charac_no, master_no, exp, create_time, delete_time "
        "FROM charac_members WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = atoi(values[0]);
        record->master_no = atoi(values[1]);
        record->exp = atoi(values[2]);
        strncpy(record->create_time, values[3], sizeof(record->create_time) - 1);
        strncpy(record->delete_time, values[4], sizeof(record->delete_time) - 1);

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacMembers_Update(DBConnectionManager* manager, const CharacMembers* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_members SET "
        "master_no = %d, exp = %d, create_time = '%s', delete_time = '%s' "
        "WHERE charac_no = %d",
        record->master_no, record->exp,
        record->create_time, record->delete_time,
        record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacMembers_Delete(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_members WHERE charac_no = %d", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacMembers_Exists(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_members WHERE charac_no = %d", charac_no);

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

int CharacMembers_UpdateMasterNo(DBConnectionManager* manager, int charac_no, int master_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_members SET master_no = %d WHERE charac_no = %d",
        master_no, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacMembers_AddExp(DBConnectionManager* manager, int charac_no, int exp) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_members SET exp = exp + %d WHERE charac_no = %d",
        exp, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacMembers_UpdateExp(DBConnectionManager* manager, int charac_no, int exp) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_members SET exp = %d WHERE charac_no = %d",
        exp, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacMembers_UpdateDeleteTime(DBConnectionManager* manager, int charac_no, const char* delete_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !delete_time) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_members SET delete_time = '%s' WHERE charac_no = %d",
        delete_time, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作实现 ==================== */

int CharacMembers_GetByMasterNo(DBConnectionManager* manager, int master_no,
                                 int* charac_nos, int max_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];
    int count = 0;

    if (!manager || !charac_nos || max_count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no FROM charac_members WHERE master_no = %d LIMIT %d",
        master_no, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_count) {
        charac_nos[count++] = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacMembers_GetCountByMasterNo(DBConnectionManager* manager, int master_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_members WHERE master_no = %d", master_no);

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

long long CharacMembers_GetTotalExp(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    long long total = 0;

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT SUM(exp) FROM charac_members");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        total = atoll(values[0]);
    }

    DBQueryResult_Free(&result);
    return total;
}

int CharacMembers_GetActiveMembersCount(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_members WHERE delete_time = '0000-00-00 00:00:00'");

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

void CharacMembers_PrintInfo(const CharacMembers* record) {
    if (!record) {
        printf("CharacMembers: NULL\n");
        return;
    }

    printf("CharacMembers {\n");
    printf("  charac_no: %d\n", record->charac_no);
    printf("  master_no: %d\n", record->master_no);
    printf("  exp: %d\n", record->exp);
    printf("  create_time: %s\n", record->create_time);
    printf("  delete_time: %s\n", record->delete_time);
    printf("}\n");
}
