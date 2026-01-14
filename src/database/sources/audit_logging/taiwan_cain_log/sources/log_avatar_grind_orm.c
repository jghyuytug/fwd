#include "log_avatar_grind_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogAvatarGrind_Add(DBConnectionManager* manager, const LogAvatarGrind* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_avatar_grind (cur_date, grade, body_part, avatar_emblem_grind, avatar_bindcube_grind, avatar_rechargestone_grind) "
        "VALUES ('%s', %d, %d, %d, %d, %d)",
        record->cur_date, record->grade, record->body_part, record->avatar_emblem_grind, record->avatar_bindcube_grind, record->avatar_rechargestone_grind);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogAvatarGrind_Get(DBConnectionManager* manager, const char* cur_date, char grade, char body_part, LogAvatarGrind* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT cur_date, grade, body_part, avatar_emblem_grind, avatar_bindcube_grind, avatar_rechargestone_grind FROM log_avatar_grind WHERE cur_date = '%s' AND grade = %d AND body_part = %d",
        cur_date, grade, body_part);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogAvatarGrind));
    if (row[0]) strncpy(record->cur_date, row[0], sizeof(record->cur_date) - 1);
    record->grade = row[1] ? row[1][0] : '\0';
    record->body_part = row[2] ? row[2][0] : '\0';
    record->avatar_emblem_grind = row[3] ? atoi(row[3]) : 0;
    record->avatar_bindcube_grind = row[4] ? atoi(row[4]) : 0;
    record->avatar_rechargestone_grind = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogAvatarGrind_GetAll(DBConnectionManager* manager, LogAvatarGrind* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT cur_date, grade, body_part, avatar_emblem_grind, avatar_bindcube_grind, avatar_rechargestone_grind FROM log_avatar_grind");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogAvatarGrind));
        if (row[0]) strncpy(records[count].cur_date, row[0], sizeof(records[count].cur_date) - 1);
        records[count].grade = row[1] ? row[1][0] : '\0';
        records[count].body_part = row[2] ? row[2][0] : '\0';
        records[count].avatar_emblem_grind = row[3] ? atoi(row[3]) : 0;
        records[count].avatar_bindcube_grind = row[4] ? atoi(row[4]) : 0;
        records[count].avatar_rechargestone_grind = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
