#include "charac_titlebook_mod_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CharacTitlebookMod_Add(DBConnectionManager* manager, const CharacTitlebookMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO charac_titlebook_mod (server_info, charac_no, MNG_user_id, work_time, work_type, memo) "
        "VALUES (%d, %d, '%s', '%s', %d, '%s')",
        record->server_info, record->charac_no, record->MNG_user_id, record->work_time, record->work_type, record->memo);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTitlebookMod_Get(DBConnectionManager* manager, int id, CharacTitlebookMod* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_info, charac_no, MNG_user_id, work_time, work_type, memo FROM charac_titlebook_mod WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CharacTitlebookMod));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->server_info = row[1] ? atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->MNG_user_id, row[3], sizeof(record->MNG_user_id) - 1);
    if (row[4]) strncpy(record->work_time, row[4], sizeof(record->work_time) - 1);
    record->work_type = row[5] ? row[5][0] : '\0';
    if (row[6]) strncpy(record->memo, row[6], sizeof(record->memo) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTitlebookMod_GetAll(DBConnectionManager* manager, CharacTitlebookMod* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, server_info, charac_no, MNG_user_id, work_time, work_type, memo FROM charac_titlebook_mod");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CharacTitlebookMod));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].server_info = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].MNG_user_id, row[3], sizeof(records[count].MNG_user_id) - 1);
        if (row[4]) strncpy(records[count].work_time, row[4], sizeof(records[count].work_time) - 1);
        records[count].work_type = row[5] ? row[5][0] : '\0';
        if (row[6]) strncpy(records[count].memo, row[6], sizeof(records[count].memo) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
