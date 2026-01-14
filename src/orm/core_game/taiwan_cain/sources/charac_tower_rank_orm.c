#include "charac_tower_rank_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* ==================== CRUD 操作实现 ==================== */

int CharacTowerRank_Add(DBConnectionManager* manager, const CharacTowerRank* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_tower_rank "
        "(tower_index, part_type, charac_no, member_info, rank) "
        "VALUES (%u, %d, %d, '%s', %u)",
        (unsigned int)record->tower_index,
        (int)record->part_type,
        record->charac_no,
        record->member_info,
        (unsigned int)record->rank);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerRank_Get(DBConnectionManager* manager, unsigned char tower_index, char part_type,
                         int charac_no, CharacTowerRank* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];

    if (!manager || !record) {
        return -1;
    }

    memset(record, 0, sizeof(CharacTowerRank));

    snprintf(query, sizeof(query),
        "SELECT tower_index, part_type, charac_no, member_info, rank "
        "FROM charac_tower_rank WHERE tower_index = %u AND part_type = %d AND charac_no = %d",
        (unsigned int)tower_index, (int)part_type, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->tower_index = (unsigned char)atoi(values[0]);
        record->part_type = (char)atoi(values[1]);
        record->charac_no = atoi(values[2]);
        if (values[3]) {
            strncpy(record->member_info, values[3], sizeof(record->member_info) - 1);
        }
        record->rank = (unsigned short)atoi(values[4]);

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacTowerRank_Update(DBConnectionManager* manager, const CharacTowerRank* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_rank SET "
        "member_info = '%s', rank = %u "
        "WHERE tower_index = %u AND part_type = %d AND charac_no = %d",
        record->member_info,
        (unsigned int)record->rank,
        (unsigned int)record->tower_index,
        (int)record->part_type,
        record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerRank_Delete(DBConnectionManager* manager, unsigned char tower_index, char part_type, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_tower_rank WHERE tower_index = %u AND part_type = %d AND charac_no = %d",
        (unsigned int)tower_index, (int)part_type, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerRank_Exists(DBConnectionManager* manager, unsigned char tower_index, char part_type, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_tower_rank WHERE tower_index = %u AND part_type = %d AND charac_no = %d",
        (unsigned int)tower_index, (int)part_type, charac_no);

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

/* ==================== 查询操作实现 ==================== */

int CharacTowerRank_GetByTowerAndType(DBConnectionManager* manager, unsigned char tower_index, char part_type,
                                       CharacTowerRank* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT tower_index, part_type, charac_no, member_info, rank "
        "FROM charac_tower_rank WHERE tower_index = %u AND part_type = %d ORDER BY rank ASC LIMIT %d",
        (unsigned int)tower_index, (int)part_type, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        records[count].tower_index = (unsigned char)atoi(values[0]);
        records[count].part_type = (char)atoi(values[1]);
        records[count].charac_no = atoi(values[2]);
        if (values[3]) {
            strncpy(records[count].member_info, values[3], sizeof(records[count].member_info) - 1);
        }
        records[count].rank = (unsigned short)atoi(values[4]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacTowerRank_GetTopN(DBConnectionManager* manager, unsigned char tower_index, char part_type,
                             int limit, CharacTowerRank* records) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    return CharacTowerRank_GetByTowerAndType(manager, tower_index, part_type, records, limit);
}

int CharacTowerRank_GetByCharacNo(DBConnectionManager* manager, int charac_no,
                                   CharacTowerRank* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT tower_index, part_type, charac_no, member_info, rank "
        "FROM charac_tower_rank WHERE charac_no = %d ORDER BY tower_index, part_type LIMIT %d",
        charac_no, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        records[count].tower_index = (unsigned char)atoi(values[0]);
        records[count].part_type = (char)atoi(values[1]);
        records[count].charac_no = atoi(values[2]);
        if (values[3]) {
            strncpy(records[count].member_info, values[3], sizeof(records[count].member_info) - 1);
        }
        records[count].rank = (unsigned short)atoi(values[4]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacTowerRank_GetCountByTower(DBConnectionManager* manager, unsigned char tower_index, char part_type) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_tower_rank WHERE tower_index = %u AND part_type = %d",
        (unsigned int)tower_index, (int)part_type);

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

int CharacTowerRank_GetRank(DBConnectionManager* manager, unsigned char tower_index, char part_type, int charac_no) {
    CharacTowerRank record;

    if (CharacTowerRank_Get(manager, tower_index, part_type, charac_no, &record) < 0) {
        return -1;
    }

    return (int)record.rank;
}

int CharacTowerRank_UpdateMemberInfo(DBConnectionManager* manager, unsigned char tower_index, char part_type,
                                      int charac_no, const char* member_info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !member_info) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_rank SET member_info = '%s' "
        "WHERE tower_index = %u AND part_type = %d AND charac_no = %d",
        member_info,
        (unsigned int)tower_index,
        (int)part_type,
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerRank_DeleteByTowerAndType(DBConnectionManager* manager, unsigned char tower_index, char part_type) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_tower_rank WHERE tower_index = %u AND part_type = %d",
        (unsigned int)tower_index, (int)part_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerRank_DeleteByCharacNo(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_tower_rank WHERE charac_no = %d", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 工具函数实现 ==================== */

void CharacTowerRank_PrintInfo(const CharacTowerRank* record) {
    if (!record) {
        printf("CharacTowerRank: NULL\n");
        return;
    }

    printf("CharacTowerRank {\n");
    printf("  tower_index: %u\n", (unsigned int)record->tower_index);
    printf("  part_type: %d\n", (int)record->part_type);
    printf("  charac_no: %d\n", record->charac_no);
    printf("  member_info: %s\n", record->member_info);
    printf("  rank: %u\n", (unsigned int)record->rank);
    printf("}\n");
}
