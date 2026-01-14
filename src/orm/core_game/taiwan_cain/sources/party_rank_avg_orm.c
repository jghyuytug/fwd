#include "party_rank_avg_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int PartyRankAvg_Add(DBConnectionManager* manager, const PartyRankAvg* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "REPLACE INTO party_rank_avg (dungeon_index, party_level, clear_count, average) VALUES (%d, %d, %lld, %d)",
        record->dungeon_index, record->party_level, record->clear_count, record->average);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PartyRankAvg_Get(DBConnectionManager* manager, short dungeon_index, short party_level, PartyRankAvg* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(PartyRankAvg));

    snprintf(query, sizeof(query),
        "SELECT dungeon_index, party_level, clear_count, average FROM party_rank_avg WHERE dungeon_index = %d AND party_level = %d",
        dungeon_index, party_level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->dungeon_index = (short)atoi(values[0]);
        record->party_level = (short)atoi(values[1]);
        record->clear_count = atoll(values[2]);
        record->average = atoi(values[3]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int PartyRankAvg_Update(DBConnectionManager* manager, const PartyRankAvg* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE party_rank_avg SET clear_count = %lld, average = %d WHERE dungeon_index = %d AND party_level = %d",
        record->clear_count, record->average, record->dungeon_index, record->party_level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PartyRankAvg_Delete(DBConnectionManager* manager, short dungeon_index, short party_level) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM party_rank_avg WHERE dungeon_index = %d AND party_level = %d",
        dungeon_index, party_level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PartyRankAvg_Exists(DBConnectionManager* manager, short dungeon_index, short party_level) {
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
        "SELECT COUNT(*) FROM party_rank_avg WHERE dungeon_index = %d AND party_level = %d",
        dungeon_index, party_level);

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

int PartyRankAvg_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM party_rank_avg");

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

int PartyRankAvg_GetAll(DBConnectionManager* manager, PartyRankAvg* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT dungeon_index, party_level, clear_count, average FROM party_rank_avg ORDER BY dungeon_index, party_level LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].dungeon_index = (short)atoi(values[0]);
        records[count].party_level = (short)atoi(values[1]);
        records[count].clear_count = atoll(values[2]);
        records[count].average = atoi(values[3]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int PartyRankAvg_GetByDungeon(DBConnectionManager* manager, short dungeon_index, PartyRankAvg* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT dungeon_index, party_level, clear_count, average FROM party_rank_avg WHERE dungeon_index = %d ORDER BY party_level LIMIT %d",
        dungeon_index, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].dungeon_index = (short)atoi(values[0]);
        records[count].party_level = (short)atoi(values[1]);
        records[count].clear_count = atoll(values[2]);
        records[count].average = atoi(values[3]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int PartyRankAvg_CountByDungeon(DBConnectionManager* manager, short dungeon_index) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM party_rank_avg WHERE dungeon_index = %d",
        dungeon_index);

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

/*******************************************************************************
 * 批量操作实现
 ******************************************************************************/

int PartyRankAvg_BatchDelete(DBConnectionManager* manager, const PartyRankAvg* records, int count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    int i;

    if (!manager || !records || count <= 0) {
        return -1;
    }

    for (i = 0; i < count; i++) {
        memset(&result, 0, sizeof(DBQueryResult));

        snprintf(query, sizeof(query),
            "DELETE FROM party_rank_avg WHERE dungeon_index = %d AND party_level = %d",
            records[i].dungeon_index, records[i].party_level);

        if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
            DBQueryResult_Free(&result);
            return -1;
        }

        DBQueryResult_Free(&result);
    }

    return 0;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void PartyRankAvg_PrintInfo(const PartyRankAvg* record) {
    if (!record) {
        printf("PartyRankAvg: NULL\n");
        return;
    }

    printf("=== Party Rank Avg ===\n");
    printf("  dungeon_index: %d\n", record->dungeon_index);
    printf("  party_level: %d\n", record->party_level);
    printf("  clear_count: %lld\n", record->clear_count);
    printf("  average: %d\n", record->average);
    printf("======================\n");
}
