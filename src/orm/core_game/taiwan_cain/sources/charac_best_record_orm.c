#include "charac_best_record_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int CharacBestRecord_Add(DBConnectionManager* manager, const CharacBestRecord* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_best_record (charac_no, dungeon_no, difficulty, "
        "style, technic, attacked, rank) "
        "VALUES (%u, %d, %d, %d, %d, %d, %d)",
        record->charac_no, record->dungeon_no, record->difficulty,
        record->style, record->technic, record->attacked, record->rank);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacBestRecord_Get(DBConnectionManager* manager, unsigned int charac_no,
                         short dungeon_no, short difficulty,
                         CharacBestRecord* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, dungeon_no, difficulty, style, technic, attacked, rank "
        "FROM charac_best_record WHERE charac_no = %u AND dungeon_no = %d AND difficulty = %d",
        charac_no, dungeon_no, difficulty);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);
        record->dungeon_no = (short)atoi(values[1]);
        record->difficulty = (short)atoi(values[2]);
        record->style = atoi(values[3]);
        record->technic = atoi(values[4]);
        record->attacked = atoi(values[5]);
        record->rank = atoi(values[6]);

        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int CharacBestRecord_Update(DBConnectionManager* manager, const CharacBestRecord* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_best_record SET style = %d, technic = %d, attacked = %d, rank = %d "
        "WHERE charac_no = %u AND dungeon_no = %d AND difficulty = %d",
        record->style, record->technic, record->attacked, record->rank,
        record->charac_no, record->dungeon_no, record->difficulty);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacBestRecord_Delete(DBConnectionManager* manager, unsigned int charac_no,
                            short dungeon_no, short difficulty) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_best_record WHERE charac_no = %u AND dungeon_no = %d AND difficulty = %d",
        charac_no, dungeon_no, difficulty);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacBestRecord_Exists(DBConnectionManager* manager, unsigned int charac_no,
                            short dungeon_no, short difficulty) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int exists = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM charac_best_record WHERE charac_no = %u AND dungeon_no = %d "
        "AND difficulty = %d LIMIT 1",
        charac_no, dungeon_no, difficulty);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = 1;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int CharacBestRecord_DeleteByCharac(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_best_record WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/*******************************************************************************
 * 业务操作实现
 ******************************************************************************/

int CharacBestRecord_UpdateScores(DBConnectionManager* manager, unsigned int charac_no,
                                  short dungeon_no, short difficulty,
                                  int style, int technic, int attacked) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_best_record SET style = %d, technic = %d, attacked = %d "
        "WHERE charac_no = %u AND dungeon_no = %d AND difficulty = %d",
        style, technic, attacked, charac_no, dungeon_no, difficulty);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacBestRecord_UpdateRank(DBConnectionManager* manager, unsigned int charac_no,
                                short dungeon_no, short difficulty, int rank) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_best_record SET rank = %d "
        "WHERE charac_no = %u AND dungeon_no = %d AND difficulty = %d",
        rank, charac_no, dungeon_no, difficulty);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacBestRecord_GetByDungeon(DBConnectionManager* manager, unsigned int charac_no,
                                  short dungeon_no, CharacBestRecord* records,
                                  int max_count) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, dungeon_no, difficulty, style, technic, attacked, rank "
        "FROM charac_best_record WHERE charac_no = %u AND dungeon_no = %d "
        "ORDER BY difficulty",
        charac_no, dungeon_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        records[count].dungeon_no = (short)atoi(values[1]);
        records[count].difficulty = (short)atoi(values[2]);
        records[count].style = atoi(values[3]);
        records[count].technic = atoi(values[4]);
        records[count].attacked = atoi(values[5]);
        records[count].rank = atoi(values[6]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void CharacBestRecord_PrintInfo(const CharacBestRecord* record) {
    if (!record) {
        printf("CharacBestRecord record is NULL\n");
        return;
    }

    printf("=== CharacBestRecord Record ===\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  dungeon_no: %d\n", record->dungeon_no);
    printf("  difficulty: %d\n", record->difficulty);
    printf("  style: %d\n", record->style);
    printf("  technic: %d\n", record->technic);
    printf("  attacked: %d\n", record->attacked);
    printf("  rank: %d\n", record->rank);
    printf("=================================\n");
}
