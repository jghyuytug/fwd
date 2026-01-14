#include "new_charac_quest_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 65536
#define MAX_FIELDS 64

/* ========== 辅助函数 ========== */

/**
 * 转义二进制数据用于SQL查询
 */
static char* escape_binary(const unsigned char* data, unsigned int len) {
    char* empty_str;
    if (!data || len == 0) {
        empty_str = (char*)malloc(3);
        if (empty_str) {
            strcpy(empty_str, "''");
        }
        return empty_str;
    }

    char* escaped = (char*)malloc(len * 2 + 3);
    if (!escaped) {
        return NULL;
    }

    escaped[0] = '\'';
    unsigned int j = 1;
    for (unsigned int i = 0; i < len; i++) {
        if (data[i] == '\'' || data[i] == '\\') {
            escaped[j++] = '\\';
        }
        escaped[j++] = data[i];
    }
    escaped[j++] = '\'';
    escaped[j] = '\0';

    return escaped;
}

/* ========== CRUD 操作实现 ========== */

int NewCharacQuest_Add(DBConnectionManager* manager, const NewCharacQuest* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    char* clear_quest_escaped = escape_binary(record->clear_quest, record->clear_quest_len);
    char* quest_notify_escaped = escape_binary(record->quest_notify, record->quest_notify_len);

    if (!clear_quest_escaped || !quest_notify_escaped) {
        if (clear_quest_escaped) free(clear_quest_escaped);
        if (quest_notify_escaped) free(quest_notify_escaped);
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO new_charac_quest ("
        "charac_no, clear_quest, quest_notify, "
        "play_1, play_1_trigger, play_2, play_2_trigger, play_3, play_3_trigger, play_4, play_4_trigger, "
        "play_5, play_5_trigger, play_6, play_6_trigger, play_7, play_7_trigger, play_8, play_8_trigger, "
        "play_9, play_9_trigger, play_10, play_10_trigger, play_11, play_11_trigger, play_12, play_12_trigger, "
        "play_13, play_13_trigger, play_14, play_14_trigger, play_15, play_15_trigger, play_16, play_16_trigger, "
        "play_17, play_17_trigger, play_18, play_18_trigger, play_19, play_19_trigger, play_20, play_20_trigger, "
        "auto_clear, urgentQuestIndex"
        ") VALUES ("
        "%u, %s, %s, "
        "%u, %u, %u, %u, %u, %u, %u, %u, "
        "%u, %u, %u, %u, %u, %u, %u, %u, "
        "%u, %u, %u, %u, %u, %u, %u, %u, "
        "%u, %u, %u, %u, %u, %u, %u, %u, "
        "%u, %u, %u, %u, %u, %u, %u, %u, "
        "%u, %d"
        ")",
        record->charac_no, clear_quest_escaped, quest_notify_escaped,
        record->play_1, record->play_1_trigger, record->play_2, record->play_2_trigger,
        record->play_3, record->play_3_trigger, record->play_4, record->play_4_trigger,
        record->play_5, record->play_5_trigger, record->play_6, record->play_6_trigger,
        record->play_7, record->play_7_trigger, record->play_8, record->play_8_trigger,
        record->play_9, record->play_9_trigger, record->play_10, record->play_10_trigger,
        record->play_11, record->play_11_trigger, record->play_12, record->play_12_trigger,
        record->play_13, record->play_13_trigger, record->play_14, record->play_14_trigger,
        record->play_15, record->play_15_trigger, record->play_16, record->play_16_trigger,
        record->play_17, record->play_17_trigger, record->play_18, record->play_18_trigger,
        record->play_19, record->play_19_trigger, record->play_20, record->play_20_trigger,
        record->auto_clear, record->urgent_quest_index
    );

    free(clear_quest_escaped);
    free(quest_notify_escaped);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int NewCharacQuest_Get(DBConnectionManager* manager, unsigned int charac_no, NewCharacQuest* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, LENGTH(clear_quest), LENGTH(quest_notify), "
        "play_1, play_1_trigger, play_2, play_2_trigger, play_3, play_3_trigger, play_4, play_4_trigger, "
        "play_5, play_5_trigger, play_6, play_6_trigger, play_7, play_7_trigger, play_8, play_8_trigger, "
        "play_9, play_9_trigger, play_10, play_10_trigger, play_11, play_11_trigger, play_12, play_12_trigger, "
        "play_13, play_13_trigger, play_14, play_14_trigger, play_15, play_15_trigger, play_16, play_16_trigger, "
        "play_17, play_17_trigger, play_18, play_18_trigger, play_19, play_19_trigger, play_20, play_20_trigger, "
        "auto_clear, urgentQuestIndex "
        "FROM new_charac_quest WHERE charac_no = %u",
        charac_no
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(NewCharacQuest));

    /* 获取一行数据 */
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int col = 0;
    record->charac_no = atoi(values[col++]);

    /* Blob 字段 (只保存长度，不读取实际数据) */
    record->clear_quest_len = atoi(values[col++]);
    record->quest_notify_len = atoi(values[col++]);

    /* 清空 blob 数据（避免 Update 时使用脏数据） */
    memset(record->clear_quest, 0, sizeof(record->clear_quest));
    memset(record->quest_notify, 0, sizeof(record->quest_notify));

    /* 20个游戏模式 */
    record->play_1 = atoi(values[col++]);
    record->play_1_trigger = atoi(values[col++]);
    record->play_2 = atoi(values[col++]);
    record->play_2_trigger = atoi(values[col++]);
    record->play_3 = atoi(values[col++]);
    record->play_3_trigger = atoi(values[col++]);
    record->play_4 = atoi(values[col++]);
    record->play_4_trigger = atoi(values[col++]);
    record->play_5 = atoi(values[col++]);
    record->play_5_trigger = atoi(values[col++]);
    record->play_6 = atoi(values[col++]);
    record->play_6_trigger = atoi(values[col++]);
    record->play_7 = atoi(values[col++]);
    record->play_7_trigger = atoi(values[col++]);
    record->play_8 = atoi(values[col++]);
    record->play_8_trigger = atoi(values[col++]);
    record->play_9 = atoi(values[col++]);
    record->play_9_trigger = atoi(values[col++]);
    record->play_10 = atoi(values[col++]);
    record->play_10_trigger = atoi(values[col++]);
    record->play_11 = atoi(values[col++]);
    record->play_11_trigger = atoi(values[col++]);
    record->play_12 = atoi(values[col++]);
    record->play_12_trigger = atoi(values[col++]);
    record->play_13 = atoi(values[col++]);
    record->play_13_trigger = atoi(values[col++]);
    record->play_14 = atoi(values[col++]);
    record->play_14_trigger = atoi(values[col++]);
    record->play_15 = atoi(values[col++]);
    record->play_15_trigger = atoi(values[col++]);
    record->play_16 = atoi(values[col++]);
    record->play_16_trigger = atoi(values[col++]);
    record->play_17 = atoi(values[col++]);
    record->play_17_trigger = atoi(values[col++]);
    record->play_18 = atoi(values[col++]);
    record->play_18_trigger = atoi(values[col++]);
    record->play_19 = atoi(values[col++]);
    record->play_19_trigger = atoi(values[col++]);
    record->play_20 = atoi(values[col++]);
    record->play_20_trigger = atoi(values[col++]);

    record->auto_clear = atoi(values[col++]);
    record->urgent_quest_index = atoi(values[col++]);

    DBQueryResult_Free(&result);
    return 0;
}

int NewCharacQuest_Update(DBConnectionManager* manager, const NewCharacQuest* record) {
    return NewCharacQuest_Add(manager, record);
}

int NewCharacQuest_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM new_charac_quest WHERE charac_no = %u",
        charac_no
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int NewCharacQuest_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM new_charac_quest WHERE charac_no = %u",
        charac_no
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    int exists = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = atoi(values[0]) > 0 ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ========== 专项操作实现 ========== */

int NewCharacQuest_GetPlayProgress(DBConnectionManager* manager, unsigned int charac_no, int play_index, PlayProgress* progress) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager || !progress || play_index < 1 || play_index > MAX_PLAY_COUNT) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT play_%d, play_%d_trigger FROM new_charac_quest WHERE charac_no = %u",
        play_index, play_index, charac_no
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        progress->play_id = atoi(values[0]);
        progress->trigger = atoi(values[1]);
    }

    DBQueryResult_Free(&result);
    return 0;
}

int NewCharacQuest_UpdatePlayProgress(DBConnectionManager* manager, unsigned int charac_no, int play_index, const PlayProgress* progress) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !progress || play_index < 1 || play_index > MAX_PLAY_COUNT) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE new_charac_quest SET play_%d = %u, play_%d_trigger = %u WHERE charac_no = %u",
        play_index, progress->play_id, play_index, progress->trigger, charac_no
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int NewCharacQuest_ClearAllPlay(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE new_charac_quest SET "
        "play_1=0, play_1_trigger=0, play_2=0, play_2_trigger=0, "
        "play_3=0, play_3_trigger=0, play_4=0, play_4_trigger=0, "
        "play_5=0, play_5_trigger=0, play_6=0, play_6_trigger=0, "
        "play_7=0, play_7_trigger=0, play_8=0, play_8_trigger=0, "
        "play_9=0, play_9_trigger=0, play_10=0, play_10_trigger=0, "
        "play_11=0, play_11_trigger=0, play_12=0, play_12_trigger=0, "
        "play_13=0, play_13_trigger=0, play_14=0, play_14_trigger=0, "
        "play_15=0, play_15_trigger=0, play_16=0, play_16_trigger=0, "
        "play_17=0, play_17_trigger=0, play_18=0, play_18_trigger=0, "
        "play_19=0, play_19_trigger=0, play_20=0, play_20_trigger=0 "
        "WHERE charac_no = %u",
        charac_no
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int NewCharacQuest_UpdateUrgentQuest(DBConnectionManager* manager, unsigned int charac_no, short urgent_index) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE new_charac_quest SET urgentQuestIndex = %d WHERE charac_no = %u",
        urgent_index, charac_no
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int NewCharacQuest_SetAutoClear(DBConnectionManager* manager, unsigned int charac_no, unsigned char auto_clear) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE new_charac_quest SET auto_clear = %u WHERE charac_no = %u",
        auto_clear, charac_no
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ========== 查询操作实现 ========== */

int NewCharacQuest_CountByUrgentQuest(DBConnectionManager* manager, short urgent_index) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM new_charac_quest WHERE urgentQuestIndex = %d",
        urgent_index
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    int count = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int NewCharacQuest_CountByAutoClear(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM new_charac_quest WHERE auto_clear = 1"
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    int count = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int NewCharacQuest_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM new_charac_quest"
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    int count = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int NewCharacQuest_HasPlayProgress(DBConnectionManager* manager, unsigned int charac_no, int play_index) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager || play_index < 1 || play_index > MAX_PLAY_COUNT) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT play_%d FROM new_charac_quest WHERE charac_no = %u",
        play_index, charac_no
    );

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    int has_progress = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        has_progress = atoi(values[0]) > 0 ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return has_progress;
}

/* ========== 工具函数实现 ========== */

void NewCharacQuest_PrintInfo(const NewCharacQuest* record) {
    if (!record) {
        printf("NewCharacQuest: NULL\n");
        return;
    }

    printf("========== NewCharacQuest Info ==========\n");
    printf("角色编号: %u\n", record->charac_no);
    printf("已完成任务数据长度: %u 字节\n", record->clear_quest_len);
    printf("任务通知数据长度: %u 字节\n", record->quest_notify_len);
    printf("自动完成: %s\n", record->auto_clear ? "是" : "否");
    printf("紧急任务索引: %d\n", record->urgent_quest_index);

    printf("\n游戏模式进度:\n");
    printf("  Play 1:  ID=%u, Trigger=%u\n", record->play_1, record->play_1_trigger);
    printf("  Play 2:  ID=%u, Trigger=%u\n", record->play_2, record->play_2_trigger);
    printf("  Play 3:  ID=%u, Trigger=%u\n", record->play_3, record->play_3_trigger);
    printf("  Play 4:  ID=%u, Trigger=%u\n", record->play_4, record->play_4_trigger);
    printf("  Play 5:  ID=%u, Trigger=%u\n", record->play_5, record->play_5_trigger);
    printf("  Play 6:  ID=%u, Trigger=%u\n", record->play_6, record->play_6_trigger);
    printf("  Play 7:  ID=%u, Trigger=%u\n", record->play_7, record->play_7_trigger);
    printf("  Play 8:  ID=%u, Trigger=%u\n", record->play_8, record->play_8_trigger);
    printf("  Play 9:  ID=%u, Trigger=%u\n", record->play_9, record->play_9_trigger);
    printf("  Play 10: ID=%u, Trigger=%u\n", record->play_10, record->play_10_trigger);
    printf("  Play 11: ID=%u, Trigger=%u\n", record->play_11, record->play_11_trigger);
    printf("  Play 12: ID=%u, Trigger=%u\n", record->play_12, record->play_12_trigger);
    printf("  Play 13: ID=%u, Trigger=%u\n", record->play_13, record->play_13_trigger);
    printf("  Play 14: ID=%u, Trigger=%u\n", record->play_14, record->play_14_trigger);
    printf("  Play 15: ID=%u, Trigger=%u\n", record->play_15, record->play_15_trigger);
    printf("  Play 16: ID=%u, Trigger=%u\n", record->play_16, record->play_16_trigger);
    printf("  Play 17: ID=%u, Trigger=%u\n", record->play_17, record->play_17_trigger);
    printf("  Play 18: ID=%u, Trigger=%u\n", record->play_18, record->play_18_trigger);
    printf("  Play 19: ID=%u, Trigger=%u\n", record->play_19, record->play_19_trigger);
    printf("  Play 20: ID=%u, Trigger=%u\n", record->play_20, record->play_20_trigger);
    printf("=========================================\n");
}
