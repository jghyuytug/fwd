#include "stat_game_channel_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ==================== CRUD 操作 ==================== */

int StatGameChannel_Add(DBConnectionManager* manager, const StatGameChannel* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO stat_game_channel (gc_channel, gc_up_time, gc_now) "
        "VALUES ('%s', '%s', %d)",
        record->gc_channel,
        record->gc_up_time,
        record->gc_now);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int StatGameChannel_Get(DBConnectionManager* manager, const char* gc_channel, const char* gc_up_time, StatGameChannel* record) {
    if (!manager || !gc_channel || !gc_up_time || !record) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT gc_channel, gc_up_time, gc_now "
        "FROM stat_game_channel "
        "WHERE gc_channel = '%s' AND gc_up_time = '%s'",
        gc_channel, gc_up_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[3];
    int ret = -1;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(record->gc_channel, values[0], sizeof(record->gc_channel) - 1);
        record->gc_channel[sizeof(record->gc_channel) - 1] = '\0';

        strncpy(record->gc_up_time, values[1], sizeof(record->gc_up_time) - 1);
        record->gc_up_time[sizeof(record->gc_up_time) - 1] = '\0';

        record->gc_now = (short)atoi(values[2]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int StatGameChannel_Update(DBConnectionManager* manager, const StatGameChannel* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "UPDATE stat_game_channel "
        "SET gc_now = %d "
        "WHERE gc_channel = '%s' AND gc_up_time = '%s'",
        record->gc_now,
        record->gc_channel,
        record->gc_up_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int StatGameChannel_Delete(DBConnectionManager* manager, const char* gc_channel, const char* gc_up_time) {
    if (!manager || !gc_channel || !gc_up_time) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "DELETE FROM stat_game_channel "
        "WHERE gc_channel = '%s' AND gc_up_time = '%s'",
        gc_channel, gc_up_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int StatGameChannel_Exists(DBConnectionManager* manager, const char* gc_channel, const char* gc_up_time) {
    if (!manager || !gc_channel || !gc_up_time) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM stat_game_channel "
        "WHERE gc_channel = '%s' AND gc_up_time = '%s'",
        gc_channel, gc_up_time);

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

int StatGameChannel_GetLatest(DBConnectionManager* manager, const char* gc_channel, StatGameChannel* record) {
    if (!manager || !gc_channel || !record) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT gc_channel, gc_up_time, gc_now "
        "FROM stat_game_channel "
        "WHERE gc_channel = '%s' "
        "ORDER BY gc_up_time DESC LIMIT 1",
        gc_channel);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[3];
    int ret = -1;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        strncpy(record->gc_channel, values[0], sizeof(record->gc_channel) - 1);
        record->gc_channel[sizeof(record->gc_channel) - 1] = '\0';

        strncpy(record->gc_up_time, values[1], sizeof(record->gc_up_time) - 1);
        record->gc_up_time[sizeof(record->gc_up_time) - 1] = '\0';

        record->gc_now = (short)atoi(values[2]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int StatGameChannel_DeleteByTimeRange(DBConnectionManager* manager, const char* gc_channel, const char* start_time, const char* end_time) {
    if (!manager || !gc_channel || !start_time || !end_time) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "DELETE FROM stat_game_channel "
        "WHERE gc_channel = '%s' AND gc_up_time >= '%s' AND gc_up_time <= '%s'",
        gc_channel, start_time, end_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int affected = result.row_count;
    DBQueryResult_Free(&result);
    return affected;
}

int StatGameChannel_DeleteOlderThan(DBConnectionManager* manager, int days) {
    if (!manager || days <= 0) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "DELETE FROM stat_game_channel "
        "WHERE gc_up_time < DATE_SUB(NOW(), INTERVAL %d DAY)",
        days);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int affected = result.row_count;
    DBQueryResult_Free(&result);
    return affected;
}

/* ==================== 查询操作 ==================== */

int StatGameChannel_CountByChannel(DBConnectionManager* manager, const char* gc_channel) {
    if (!manager || !gc_channel) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM stat_game_channel WHERE gc_channel = '%s'",
        gc_channel);

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

int StatGameChannel_CountAll(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM stat_game_channel");

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

int StatGameChannel_GetAverageOnline(DBConnectionManager* manager, const char* gc_channel, const char* start_time, const char* end_time) {
    if (!manager || !gc_channel || !start_time || !end_time) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT AVG(gc_now) FROM stat_game_channel "
        "WHERE gc_channel = '%s' AND gc_up_time >= '%s' AND gc_up_time <= '%s'",
        gc_channel, start_time, end_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int avg = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        if (values[0] != NULL) {
            avg = (int)atof(values[0]);
        }
    }

    DBQueryResult_Free(&result);
    return avg;
}

int StatGameChannel_GetMaxOnline(DBConnectionManager* manager, const char* gc_channel, const char* start_time, const char* end_time) {
    if (!manager || !gc_channel || !start_time || !end_time) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT MAX(gc_now) FROM stat_game_channel "
        "WHERE gc_channel = '%s' AND gc_up_time >= '%s' AND gc_up_time <= '%s'",
        gc_channel, start_time, end_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* values[1];
    int max_online = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        if (values[0] != NULL) {
            max_online = atoi(values[0]);
        }
    }

    DBQueryResult_Free(&result);
    return max_online;
}

int StatGameChannel_CountDistinctChannels(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT COUNT(DISTINCT gc_channel) FROM stat_game_channel");

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

/* ==================== 工具函数 ==================== */

void StatGameChannel_PrintInfo(const StatGameChannel* record) {
    if (!record) {
        printf("  [StatGameChannel] NULL record\n");
        return;
    }

    printf("  ----------------------------------------\n");
    printf("  频道名称: %s\n", record->gc_channel);
    printf("  更新时间: %s\n", record->gc_up_time);
    printf("  当前在线: %d 人\n", record->gc_now);
    printf("  ----------------------------------------\n");
}
