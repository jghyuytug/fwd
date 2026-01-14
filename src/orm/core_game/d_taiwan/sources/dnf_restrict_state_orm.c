#include "dnf_restrict_state_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* CRUD 操作实现 */

int DnfRestrictState_Add(DBConnectionManager* manager, const DnfRestrictState* state) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !state) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO dnf_restrict_state (server_group, category, restrict_code, "
        "restrict_value, mod_date, reg_date) VALUES (%d, %d, %d, '%c', '%s', '%s')",
        state->server_group, state->category, state->restrict_code, state->restrict_value,
        state->mod_date, state->reg_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictState_Get(DBConnectionManager* manager, int server_group, int category,
                          int restrict_code, DnfRestrictState* state) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !state) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT server_group, category, restrict_code, restrict_value, mod_date, reg_date "
        "FROM dnf_restrict_state WHERE server_group = %d AND category = %d AND restrict_code = %d",
        server_group, category, restrict_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(state, 0, sizeof(DnfRestrictState));
    state->server_group = row[0] ? atoi(row[0]) : 0;
    state->category = row[1] ? atoi(row[1]) : 0;
    state->restrict_code = row[2] ? atoi(row[2]) : 0;
    state->restrict_value = row[3] ? row[3][0] : '\0';
    if (row[4]) strncpy(state->mod_date, row[4], sizeof(state->mod_date) - 1);
    if (row[5]) strncpy(state->reg_date, row[5], sizeof(state->reg_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictState_Update(DBConnectionManager* manager, const DnfRestrictState* state) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !state) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_restrict_state SET restrict_value = '%c', mod_date = '%s', reg_date = '%s' "
        "WHERE server_group = %d AND category = %d AND restrict_code = %d",
        state->restrict_value, state->mod_date, state->reg_date,
        state->server_group, state->category, state->restrict_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictState_Delete(DBConnectionManager* manager, int server_group, int category,
                             int restrict_code) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_restrict_state WHERE server_group = %d AND category = %d AND restrict_code = %d",
        server_group, category, restrict_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictState_Exists(DBConnectionManager* manager, int server_group, int category,
                             int restrict_code) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_restrict_state WHERE server_group = %d AND category = %d AND restrict_code = %d",
        server_group, category, restrict_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count > 0 ? 1 : 0;
}

/* 业务操作实现 */

int DnfRestrictState_GetByServerGroup(DBConnectionManager* manager, int server_group,
                                       DnfRestrictState* states, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !states || !actual_count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT server_group, category, restrict_code, restrict_value, mod_date, reg_date "
        "FROM dnf_restrict_state WHERE server_group = %d ORDER BY category, restrict_code",
        server_group);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&states[count], 0, sizeof(DnfRestrictState));
        states[count].server_group = row[0] ? atoi(row[0]) : 0;
        states[count].category = row[1] ? atoi(row[1]) : 0;
        states[count].restrict_code = row[2] ? atoi(row[2]) : 0;
        states[count].restrict_value = row[3] ? row[3][0] : '\0';
        if (row[4]) strncpy(states[count].mod_date, row[4], sizeof(states[count].mod_date) - 1);
        if (row[5]) strncpy(states[count].reg_date, row[5], sizeof(states[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictState_GetByCategory(DBConnectionManager* manager, int category,
                                    DnfRestrictState* states, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !states || !actual_count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT server_group, category, restrict_code, restrict_value, mod_date, reg_date "
        "FROM dnf_restrict_state WHERE category = %d ORDER BY server_group, restrict_code",
        category);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&states[count], 0, sizeof(DnfRestrictState));
        states[count].server_group = row[0] ? atoi(row[0]) : 0;
        states[count].category = row[1] ? atoi(row[1]) : 0;
        states[count].restrict_code = row[2] ? atoi(row[2]) : 0;
        states[count].restrict_value = row[3] ? row[3][0] : '\0';
        if (row[4]) strncpy(states[count].mod_date, row[4], sizeof(states[count].mod_date) - 1);
        if (row[5]) strncpy(states[count].reg_date, row[5], sizeof(states[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictState_GetByServerGroupAndCategory(DBConnectionManager* manager, int server_group,
                                                  int category, DnfRestrictState* states,
                                                  int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !states || !actual_count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT server_group, category, restrict_code, restrict_value, mod_date, reg_date "
        "FROM dnf_restrict_state WHERE server_group = %d AND category = %d ORDER BY restrict_code",
        server_group, category);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&states[count], 0, sizeof(DnfRestrictState));
        states[count].server_group = row[0] ? atoi(row[0]) : 0;
        states[count].category = row[1] ? atoi(row[1]) : 0;
        states[count].restrict_code = row[2] ? atoi(row[2]) : 0;
        states[count].restrict_value = row[3] ? row[3][0] : '\0';
        if (row[4]) strncpy(states[count].mod_date, row[4], sizeof(states[count].mod_date) - 1);
        if (row[5]) strncpy(states[count].reg_date, row[5], sizeof(states[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictState_SetRestrictValue(DBConnectionManager* manager, int server_group,
                                       int category, int restrict_code, char restrict_value) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_restrict_state SET restrict_value = '%c', mod_date = NOW() "
        "WHERE server_group = %d AND category = %d AND restrict_code = %d",
        restrict_value, server_group, category, restrict_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictState_DeleteByServerGroup(DBConnectionManager* manager, int server_group) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_restrict_state WHERE server_group = %d", server_group);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictState_CountByServerGroup(DBConnectionManager* manager, int server_group) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_restrict_state WHERE server_group = %d", server_group);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count;
}

/* 工具函数实现 */

void DnfRestrictState_PrintInfo(const DnfRestrictState* state) {
    if (!state) {
        return;
    }

    printf("=== DNF限制状态信息 ===\n");
    printf("服务器组: %d\n", state->server_group);
    printf("分类: %d\n", state->category);
    printf("限制代码: %d\n", state->restrict_code);
    printf("限制值: %c\n", state->restrict_value);
    printf("修改日期: %s\n", state->mod_date);
    printf("注册日期: %s\n", state->reg_date);
    printf("======================\n");
}
