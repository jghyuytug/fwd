#include "charac_dimension_inout_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 2048

/* ==================== CRUD 操作实现 ==================== */

int CharacDimensionInout_Add(DBConnectionManager* manager, const CharacDimensionInout* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_dimension_inout "
        "(charac_no, dungeon1, dungeon2, dungeon3, dungeon4, dungeon5, "
        "dungeon6, dungeon7, dungeon8, dungeon9, dungeon10) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->charac_no,
        record->dungeon_status[0], record->dungeon_status[1], record->dungeon_status[2],
        record->dungeon_status[3], record->dungeon_status[4], record->dungeon_status[5],
        record->dungeon_status[6], record->dungeon_status[7], record->dungeon_status[8],
        record->dungeon_status[9]);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDimensionInout_Get(DBConnectionManager* manager, int charac_no, CharacDimensionInout* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[11];

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, dungeon1, dungeon2, dungeon3, dungeon4, dungeon5, "
        "dungeon6, dungeon7, dungeon8, dungeon9, dungeon10 "
        "FROM charac_dimension_inout WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = atoi(values[0]);
        record->dungeon_status[0] = (char)atoi(values[1]);
        record->dungeon_status[1] = (char)atoi(values[2]);
        record->dungeon_status[2] = (char)atoi(values[3]);
        record->dungeon_status[3] = (char)atoi(values[4]);
        record->dungeon_status[4] = (char)atoi(values[5]);
        record->dungeon_status[5] = (char)atoi(values[6]);
        record->dungeon_status[6] = (char)atoi(values[7]);
        record->dungeon_status[7] = (char)atoi(values[8]);
        record->dungeon_status[8] = (char)atoi(values[9]);
        record->dungeon_status[9] = (char)atoi(values[10]);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacDimensionInout_Update(DBConnectionManager* manager, const CharacDimensionInout* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_dimension_inout SET "
        "dungeon1 = %d, dungeon2 = %d, dungeon3 = %d, dungeon4 = %d, dungeon5 = %d, "
        "dungeon6 = %d, dungeon7 = %d, dungeon8 = %d, dungeon9 = %d, dungeon10 = %d "
        "WHERE charac_no = %d",
        record->dungeon_status[0], record->dungeon_status[1], record->dungeon_status[2],
        record->dungeon_status[3], record->dungeon_status[4], record->dungeon_status[5],
        record->dungeon_status[6], record->dungeon_status[7], record->dungeon_status[8],
        record->dungeon_status[9], record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDimensionInout_Delete(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_dimension_inout WHERE charac_no = %d", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDimensionInout_Exists(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_dimension_inout WHERE charac_no = %d", charac_no);

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

/* ==================== 副本状态操作实现 ==================== */

int CharacDimensionInout_GetDungeonStatus(DBConnectionManager* manager, int charac_no, int dungeon_index) {
    CharacDimensionInout record;

    if (!manager || dungeon_index < 1 || dungeon_index > DIMENSION_DUNGEON_COUNT) {
        return -1;
    }

    if (CharacDimensionInout_Get(manager, charac_no, &record) < 0) {
        return -1;
    }

    return (int)record.dungeon_status[dungeon_index - 1];
}

int CharacDimensionInout_UpdateDungeonStatus(DBConnectionManager* manager, int charac_no,
                                          int dungeon_index, char status) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    const char* field_names[] = {
        "dungeon1", "dungeon2", "dungeon3", "dungeon4", "dungeon5",
        "dungeon6", "dungeon7", "dungeon8", "dungeon9", "dungeon10"
    };

    if (!manager || dungeon_index < 1 || dungeon_index > DIMENSION_DUNGEON_COUNT) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_dimension_inout SET %s = %d WHERE charac_no = %d",
        field_names[dungeon_index - 1], (int)status, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDimensionInout_GetUnlockedDungeons(DBConnectionManager* manager, int charac_no,
                                          int* unlocked_dungeons, int max_count) {
    CharacDimensionInout record;
    int i, count = 0;

    if (!manager || !unlocked_dungeons || max_count <= 0) {
        return -1;
    }

    if (CharacDimensionInout_Get(manager, charac_no, &record) < 0) {
        return -1;
    }

    for (i = 0; i < DIMENSION_DUNGEON_COUNT && count < max_count; i++) {
        if (record.dungeon_status[i] > 0) {
            unlocked_dungeons[count++] = i + 1;  /* 副本索引从1开始 */
        }
    }

    return count;
}

int CharacDimensionInout_UnlockDungeon(DBConnectionManager* manager, int charac_no, int dungeon_index) {
    return CharacDimensionInout_UpdateDungeonStatus(manager, charac_no, dungeon_index, 1);
}

int CharacDimensionInout_SetAllDungeonStatus(DBConnectionManager* manager, int charac_no, char status) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_dimension_inout SET "
        "dungeon1 = %d, dungeon2 = %d, dungeon3 = %d, dungeon4 = %d, dungeon5 = %d, "
        "dungeon6 = %d, dungeon7 = %d, dungeon8 = %d, dungeon9 = %d, dungeon10 = %d "
        "WHERE charac_no = %d",
        (int)status, (int)status, (int)status, (int)status, (int)status,
        (int)status, (int)status, (int)status, (int)status, (int)status,
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 统计操作实现 ==================== */

int CharacDimensionInout_CountUnlockedDungeons(DBConnectionManager* manager, int charac_no) {
    CharacDimensionInout record;
    int i, count = 0;

    if (!manager) {
        return -1;
    }

    if (CharacDimensionInout_Get(manager, charac_no, &record) < 0) {
        return -1;
    }

    for (i = 0; i < DIMENSION_DUNGEON_COUNT; i++) {
        if (record.dungeon_status[i] > 0) {
            count++;
        }
    }

    return count;
}

int CharacDimensionInout_CountDungeonsByStatus(DBConnectionManager* manager, int charac_no, char status) {
    CharacDimensionInout record;
    int i, count = 0;

    if (!manager) {
        return -1;
    }

    if (CharacDimensionInout_Get(manager, charac_no, &record) < 0) {
        return -1;
    }

    for (i = 0; i < DIMENSION_DUNGEON_COUNT; i++) {
        if (record.dungeon_status[i] == status) {
            count++;
        }
    }

    return count;
}

int CharacDimensionInout_IsAllUnlocked(DBConnectionManager* manager, int charac_no) {
    CharacDimensionInout record;
    int i;

    if (!manager) {
        return -1;
    }

    if (CharacDimensionInout_Get(manager, charac_no, &record) < 0) {
        return -1;
    }

    for (i = 0; i < DIMENSION_DUNGEON_COUNT; i++) {
        if (record.dungeon_status[i] == 0) {
            return 0;  /* 有未解锁的副本 */
        }
    }

    return 1;  /* 全部解锁 */
}

/* ==================== 工具函数实现 ==================== */

void CharacDimensionInout_PrintInfo(const CharacDimensionInout* record) {
    int i;

    if (!record) {
        printf("CharacDimensionInout: NULL\n");
        return;
    }

    printf("CharacDimensionInout {\n");
    printf("  charac_no: %d\n", record->charac_no);
    printf("  副本状态:\n");
    for (i = 0; i < DIMENSION_DUNGEON_COUNT; i++) {
        printf("    dungeon%d: %d (%s)\n", i + 1, record->dungeon_status[i],
               record->dungeon_status[i] > 0 ? "已解锁" : "未解锁");
    }
    printf("}\n");
}
