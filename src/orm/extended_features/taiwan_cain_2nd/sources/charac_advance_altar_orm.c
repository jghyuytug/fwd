#include "charac_advance_altar_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CharacAdvanceAltar_Add(DBConnectionManager* manager, const CharacAdvanceAltar* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO charac_advance_altar (charac_no, ridable_id, ticket_free, ticket_cera, star_game, star_cera, star_usable, survival_best, star_reset_count, is_unlock_stage_effect) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->charac_no, record->ridable_id, record->ticket_free, record->ticket_cera, record->star_game, record->star_cera, record->star_usable, record->survival_best, record->star_reset_count, record->is_unlock_stage_effect);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacAdvanceAltar_Get(DBConnectionManager* manager, int charac_no, int ridable_id, CharacAdvanceAltar* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, ridable_id, ticket_free, ticket_cera, star_game, star_cera, star_usable, survival_best, star_reset_count, is_unlock_stage_effect FROM charac_advance_altar WHERE charac_no = %d AND ridable_id = %d",
        charac_no, ridable_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CharacAdvanceAltar));
    record->charac_no = row[0] ? atoi(row[0]) : 0;
    record->ridable_id = row[1] ? atoi(row[1]) : 0;
    record->ticket_free = row[2] ? atoi(row[2]) : 0;
    record->ticket_cera = row[3] ? atoi(row[3]) : 0;
    record->star_game = row[4] ? atoi(row[4]) : 0;
    record->star_cera = row[5] ? atoi(row[5]) : 0;
    record->star_usable = row[6] ? atoi(row[6]) : 0;
    record->survival_best = row[7] ? atoi(row[7]) : 0;
    record->star_reset_count = row[8] ? atoi(row[8]) : 0;
    record->is_unlock_stage_effect = row[9] ? atoi(row[9]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int CharacAdvanceAltar_GetAll(DBConnectionManager* manager, CharacAdvanceAltar* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, ridable_id, ticket_free, ticket_cera, star_game, star_cera, star_usable, survival_best, star_reset_count, is_unlock_stage_effect FROM charac_advance_altar");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CharacAdvanceAltar));
        records[count].charac_no = row[0] ? atoi(row[0]) : 0;
        records[count].ridable_id = row[1] ? atoi(row[1]) : 0;
        records[count].ticket_free = row[2] ? atoi(row[2]) : 0;
        records[count].ticket_cera = row[3] ? atoi(row[3]) : 0;
        records[count].star_game = row[4] ? atoi(row[4]) : 0;
        records[count].star_cera = row[5] ? atoi(row[5]) : 0;
        records[count].star_usable = row[6] ? atoi(row[6]) : 0;
        records[count].survival_best = row[7] ? atoi(row[7]) : 0;
        records[count].star_reset_count = row[8] ? atoi(row[8]) : 0;
        records[count].is_unlock_stage_effect = row[9] ? atoi(row[9]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
