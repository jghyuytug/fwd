#include "single_rank_avg_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/* ========== CRUD 操作 ========== */

int SingleRankAvg_Add(DBConnectionManager* manager, const SingleRankAvg* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO single_rank_avg (dungeon_index, level, job, clear_count, average) "
        "VALUES (%hd, %hd, %hd, %lld, %d)",
        record->dungeon_index, record->level, record->job,
        record->clear_count, record->average);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SingleRankAvg_Get(DBConnectionManager* manager, short dungeon_index, short level,
                      short job, SingleRankAvg* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT dungeon_index, level, job, clear_count, average "
        "FROM single_rank_avg "
        "WHERE dungeon_index=%hd AND level=%hd AND job=%hd",
        dungeon_index, level, job);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->dungeon_index = (short)atoi(values[0]);
        record->level = (short)atoi(values[1]);
        record->job = (short)atoi(values[2]);
        record->clear_count = atoll(values[3]);
        record->average = atoi(values[4]);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    fprintf(stderr, "SingleRankAvg_Get: Record not found (dungeon=%hd, level=%hd, job=%hd)\n",
            dungeon_index, level, job);
    return -1;
}

int SingleRankAvg_Update(DBConnectionManager* manager, const SingleRankAvg* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE single_rank_avg SET clear_count=%lld, average=%d "
        "WHERE dungeon_index=%hd AND level=%hd AND job=%hd",
        record->clear_count, record->average,
        record->dungeon_index, record->level, record->job);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SingleRankAvg_Delete(DBConnectionManager* manager, short dungeon_index, short level, short job) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM single_rank_avg WHERE dungeon_index=%hd AND level=%hd AND job=%hd",
        dungeon_index, level, job);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SingleRankAvg_Exists(DBConnectionManager* manager, short dungeon_index, short level, short job) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM single_rank_avg "
        "WHERE dungeon_index=%hd AND level=%hd AND job=%hd",
        dungeon_index, level, job);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count > 0 ? 1 : 0;
}

/* ========== 专项操作 ========== */

int SingleRankAvg_IncrementClearCount(DBConnectionManager* manager, short dungeon_index,
                                      short level, short job, long long count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE single_rank_avg SET clear_count = clear_count + %lld "
        "WHERE dungeon_index=%hd AND level=%hd AND job=%hd",
        count, dungeon_index, level, job);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SingleRankAvg_UpdateAverage(DBConnectionManager* manager, short dungeon_index,
                                short level, short job, int average) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE single_rank_avg SET average=%d "
        "WHERE dungeon_index=%hd AND level=%hd AND job=%hd",
        average, dungeon_index, level, job);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ========== 查询操作 ========== */

int SingleRankAvg_CountByDungeon(DBConnectionManager* manager, short dungeon_index) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM single_rank_avg WHERE dungeon_index=%hd",
        dungeon_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int SingleRankAvg_CountByDungeonAndLevel(DBConnectionManager* manager, short dungeon_index, short level) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM single_rank_avg WHERE dungeon_index=%hd AND level=%hd",
        dungeon_index, level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

long long SingleRankAvg_GetTotalClearCountByJob(DBConnectionManager* manager, short job) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    long long total = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT SUM(clear_count) FROM single_rank_avg WHERE job=%hd",
        job);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0] != NULL) {
        total = atoll(values[0]);
    }

    DBQueryResult_Free(&result);
    return total;
}

int SingleRankAvg_DeleteByDungeon(DBConnectionManager* manager, short dungeon_index) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM single_rank_avg WHERE dungeon_index=%hd",
        dungeon_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SingleRankAvg_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM single_rank_avg");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ========== 工具函数 ========== */

void SingleRankAvg_PrintInfo(const SingleRankAvg* record) {
    if (!record) {
        printf("SingleRankAvg: NULL\n");
        return;
    }

    printf("========================================\n");
    printf("SingleRankAvg Information\n");
    printf("========================================\n");
    printf("Dungeon Index : %hd\n", record->dungeon_index);
    printf("Level         : %hd\n", record->level);
    printf("Job           : %hd\n", record->job);
    printf("Clear Count   : %lld\n", record->clear_count);
    printf("Average       : %d\n", record->average);
    printf("========================================\n");
}
