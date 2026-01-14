#include "../../include/database/headers/event_levelup_support_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 2048
#define MAX_FIELDS 32

/* ==================== CRUD 操作 ==================== */

int EventLevelupSupport_Add(DBConnectionManager* manager, const EventLevelupSupport* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "REPLACE INTO event_levelup_support (charac_no, level, state) VALUES (%d, %d, %d)",
        record->charac_no, record->level, record->state);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventLevelupSupport_Get(DBConnectionManager* manager, int charac_no, int level,
                            EventLevelupSupport* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int ret = -1;

    memset(&result, 0, sizeof(DBQueryResult));
    memset(record, 0, sizeof(EventLevelupSupport));

    snprintf(query, sizeof(query),
        "SELECT charac_no, level, state FROM event_levelup_support WHERE charac_no = %d AND level = %d",
        charac_no, level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = atoi(values[0]);
        record->level = atoi(values[1]);
        record->state = values[2] ? atoi(values[2]) : 0;
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int EventLevelupSupport_Update(DBConnectionManager* manager, const EventLevelupSupport* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE event_levelup_support SET state = %d WHERE charac_no = %d AND level = %d",
        record->state, record->charac_no, record->level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventLevelupSupport_Delete(DBConnectionManager* manager, int charac_no, int level) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM event_levelup_support WHERE charac_no = %d AND level = %d",
        charac_no, level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventLevelupSupport_Exists(DBConnectionManager* manager, int charac_no, int level) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int exists = 0;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_levelup_support WHERE charac_no = %d AND level = %d",
        charac_no, level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        exists = atoi(values[0]) > 0 ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 专项操作 ==================== */

int EventLevelupSupport_UpdateState(DBConnectionManager* manager, int charac_no, int level, int state) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE event_levelup_support SET state = %d WHERE charac_no = %d AND level = %d",
        state, charac_no, level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventLevelupSupport_DeleteByCharac(DBConnectionManager* manager, int charac_no) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM event_levelup_support WHERE charac_no = %d", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作 ==================== */

int EventLevelupSupport_GetByCharac(DBConnectionManager* manager, int charac_no,
                                    EventLevelupSupport* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT charac_no, level, state FROM event_levelup_support WHERE charac_no = %d ORDER BY level",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = atoi(values[0]);
        records[count].level = atoi(values[1]);
        records[count].state = values[2] ? atoi(values[2]) : 0;
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int EventLevelupSupport_GetByLevel(DBConnectionManager* manager, int level,
                                   EventLevelupSupport* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT charac_no, level, state FROM event_levelup_support WHERE level = %d ORDER BY charac_no",
        level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = atoi(values[0]);
        records[count].level = atoi(values[1]);
        records[count].state = values[2] ? atoi(values[2]) : 0;
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int EventLevelupSupport_GetByState(DBConnectionManager* manager, int state,
                                   EventLevelupSupport* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT charac_no, level, state FROM event_levelup_support WHERE state = %d ORDER BY charac_no, level",
        state);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = atoi(values[0]);
        records[count].level = atoi(values[1]);
        records[count].state = values[2] ? atoi(values[2]) : 0;
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int EventLevelupSupport_CountByCharac(DBConnectionManager* manager, int charac_no) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_levelup_support WHERE charac_no = %d", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 工具函数 ==================== */

void EventLevelupSupport_PrintInfo(const EventLevelupSupport* record) {
    if (!record) {
        return;
    }

    printf("========================================\n");
    printf("角色编号: %d\n", record->charac_no);
    printf("等级: %d\n", record->level);
    printf("状态: %d\n", record->state);
    printf("========================================\n");
}
