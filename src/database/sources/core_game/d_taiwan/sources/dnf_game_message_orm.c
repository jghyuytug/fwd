#include "dnf_game_message_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 2048

/* CRUD 操作实现 */

int DnfGameMessage_Add(DBConnectionManager* manager, const DnfGameMessage* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    /* 有主键，使用 REPLACE INTO */
    if (record->no > 0) {
        snprintf(query, sizeof(query),
            "REPLACE INTO dnf_game_message (no, message, display_type, start_h, end_h, occ_date) "
            "VALUES (%u, '%s', %u, %u, %u, '%s')",
            record->no, record->message, record->display_type,
            record->start_h, record->end_h, record->occ_date);
    } else {
        /* 自增主键，使用 INSERT */
        snprintf(query, sizeof(query),
            "INSERT INTO dnf_game_message (message, display_type, start_h, end_h, occ_date) "
            "VALUES ('%s', %u, %u, %u, '%s')",
            record->message, record->display_type,
            record->start_h, record->end_h, record->occ_date);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameMessage_Get(DBConnectionManager* manager, unsigned int no, DnfGameMessage* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT no, message, display_type, start_h, end_h, occ_date "
        "FROM dnf_game_message WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfGameMessage));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->message, row[1], sizeof(record->message) - 1);
    record->display_type = row[2] ? atoi(row[2]) : 1;
    record->start_h = row[3] ? atoi(row[3]) : 0;
    record->end_h = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->occ_date, row[5], sizeof(record->occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameMessage_Update(DBConnectionManager* manager, const DnfGameMessage* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_game_message SET message='%s', display_type=%u, start_h=%u, "
        "end_h=%u, occ_date='%s' WHERE no=%u",
        record->message, record->display_type, record->start_h,
        record->end_h, record->occ_date, record->no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameMessage_Delete(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_game_message WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameMessage_Exists(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_game_message WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, row) > 0 && row[0]) {
        exists = atoi(row[0]) > 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* 专项操作实现 */

int DnfGameMessage_UpdateMessage(DBConnectionManager* manager, unsigned int no, const char* message) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !message) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_game_message SET message='%s' WHERE no=%u",
        message, no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameMessage_UpdateDisplayType(DBConnectionManager* manager, unsigned int no,
                                      unsigned char display_type) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_game_message SET display_type=%u WHERE no=%u",
        display_type, no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameMessage_UpdateTimeRange(DBConnectionManager* manager, unsigned int no,
                                   unsigned char start_h, unsigned char end_h) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_game_message SET start_h=%u, end_h=%u WHERE no=%u",
        start_h, end_h, no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameMessage_SetMessageSchedule(DBConnectionManager* manager, unsigned int no,
                                      const char* message, unsigned char display_type,
                                      unsigned char start_h, unsigned char end_h,
                                      const char* occ_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !message || !occ_date) {
        return -1;
    }

    /* 幂等设置消息计划，使用 REPLACE INTO */
    if (no > 0) {
        snprintf(query, sizeof(query),
            "REPLACE INTO dnf_game_message (no, message, display_type, start_h, end_h, occ_date) "
            "VALUES (%u, '%s', %u, %u, %u, '%s')",
            no, message, display_type, start_h, end_h, occ_date);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO dnf_game_message (message, display_type, start_h, end_h, occ_date) "
            "VALUES ('%s', %u, %u, %u, '%s')",
            message, display_type, start_h, end_h, occ_date);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* 查询操作实现 */

int DnfGameMessage_GetByDisplayType(DBConnectionManager* manager, unsigned char display_type,
                                     DnfGameMessage** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT no, message, display_type, start_h, end_h, occ_date "
        "FROM dnf_game_message WHERE display_type = %u ORDER BY occ_date DESC, no DESC",
        display_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    /* 先分配较大空间，边读边计数 */
    int max_rows = 1000;
    *records = (DnfGameMessage*)malloc(sizeof(DnfGameMessage) * max_rows);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *count = 0;
    while (DBQueryResult_FetchRow(&result, row) > 0 && *count < max_rows) {
        memset(&(*records)[*count], 0, sizeof(DnfGameMessage));
        (*records)[*count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy((*records)[*count].message, row[1], sizeof((*records)[*count].message) - 1);
        (*records)[*count].display_type = row[2] ? atoi(row[2]) : 1;
        (*records)[*count].start_h = row[3] ? atoi(row[3]) : 0;
        (*records)[*count].end_h = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy((*records)[*count].occ_date, row[5], sizeof((*records)[*count].occ_date) - 1);
        (*count)++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameMessage_GetByDate(DBConnectionManager* manager, const char* occ_date,
                             DnfGameMessage** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !occ_date || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT no, message, display_type, start_h, end_h, occ_date "
        "FROM dnf_game_message WHERE occ_date = '%s' ORDER BY no",
        occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    /* 先分配较大空间，边读边计数 */
    int max_rows = 1000;
    *records = (DnfGameMessage*)malloc(sizeof(DnfGameMessage) * max_rows);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *count = 0;
    while (DBQueryResult_FetchRow(&result, row) > 0 && *count < max_rows) {
        memset(&(*records)[*count], 0, sizeof(DnfGameMessage));
        (*records)[*count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy((*records)[*count].message, row[1], sizeof((*records)[*count].message) - 1);
        (*records)[*count].display_type = row[2] ? atoi(row[2]) : 1;
        (*records)[*count].start_h = row[3] ? atoi(row[3]) : 0;
        (*records)[*count].end_h = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy((*records)[*count].occ_date, row[5], sizeof((*records)[*count].occ_date) - 1);
        (*count)++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameMessage_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                   const char* end_date, DnfGameMessage** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !start_date || !end_date || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT no, message, display_type, start_h, end_h, occ_date "
        "FROM dnf_game_message WHERE occ_date BETWEEN '%s' AND '%s' "
        "ORDER BY occ_date DESC, no DESC",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    /* 先分配较大空间，边读边计数 */
    int max_rows = 1000;
    *records = (DnfGameMessage*)malloc(sizeof(DnfGameMessage) * max_rows);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *count = 0;
    while (DBQueryResult_FetchRow(&result, row) > 0 && *count < max_rows) {
        memset(&(*records)[*count], 0, sizeof(DnfGameMessage));
        (*records)[*count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy((*records)[*count].message, row[1], sizeof((*records)[*count].message) - 1);
        (*records)[*count].display_type = row[2] ? atoi(row[2]) : 1;
        (*records)[*count].start_h = row[3] ? atoi(row[3]) : 0;
        (*records)[*count].end_h = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy((*records)[*count].occ_date, row[5], sizeof((*records)[*count].occ_date) - 1);
        (*count)++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameMessage_GetActiveMessages(DBConnectionManager* manager, unsigned char display_type,
                                      const char* current_date, unsigned char current_hour,
                                      DnfGameMessage** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !current_date || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    /* 查询当前日期、显示类型匹配、并且在时间范围内的消息 */
    snprintf(query, sizeof(query),
        "SELECT no, message, display_type, start_h, end_h, occ_date "
        "FROM dnf_game_message "
        "WHERE display_type = %u AND occ_date = '%s' "
        "AND start_h <= %u AND end_h >= %u "
        "ORDER BY no",
        display_type, current_date, current_hour, current_hour);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    /* 先分配较大空间，边读边计数 */
    int max_rows = 1000;
    *records = (DnfGameMessage*)malloc(sizeof(DnfGameMessage) * max_rows);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *count = 0;
    while (DBQueryResult_FetchRow(&result, row) > 0 && *count < max_rows) {
        memset(&(*records)[*count], 0, sizeof(DnfGameMessage));
        (*records)[*count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy((*records)[*count].message, row[1], sizeof((*records)[*count].message) - 1);
        (*records)[*count].display_type = row[2] ? atoi(row[2]) : 1;
        (*records)[*count].start_h = row[3] ? atoi(row[3]) : 0;
        (*records)[*count].end_h = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy((*records)[*count].occ_date, row[5], sizeof((*records)[*count].occ_date) - 1);
        (*count)++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameMessage_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count = 0;

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM dnf_game_message");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, row) > 0 && row[0]) {
        count = atoi(row[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/* 批量操作实现 */

int DnfGameMessage_DeleteByDate(DBConnectionManager* manager, const char* occ_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_date) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_game_message WHERE occ_date = '%s'", occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameMessage_Clear(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "DELETE FROM dnf_game_message");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* 工具函数实现 */

void DnfGameMessage_PrintInfo(const DnfGameMessage* record) {
    if (!record) {
        return;
    }

    printf("========================================\n");
    printf("DnfGameMessage 信息:\n");
    printf("  消息编号: %u\n", record->no);
    printf("  消息内容: %s\n", record->message);
    printf("  显示类型: %u\n", record->display_type);
    printf("  时间范围: %02u:00 - %02u:00\n", record->start_h, record->end_h);
    printf("  发生日期: %s\n", record->occ_date);
    printf("========================================\n");
}
