#include "tcg_card_ref_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int TcgCardRef_Add(DBConnectionManager* manager, const TcgCardRef* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO tcg_card_ref (act_info, type, card_name, card_lev, card_type, card_rare, card_num) "
        "VALUES ('%s', '%s', '%s', %d, '%s', %d, %d)",
        record->act_info, record->type, record->card_name, record->card_lev, record->card_type, record->card_rare, record->card_num);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TcgCardRef_GetAll(DBConnectionManager* manager, TcgCardRef* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT act_info, type, card_name, card_lev, card_type, card_rare, card_num FROM tcg_card_ref");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(TcgCardRef));
        if (row[0]) strncpy(records[count].act_info, row[0], sizeof(records[count].act_info) - 1);
        if (row[1]) strncpy(records[count].type, row[1], sizeof(records[count].type) - 1);
        if (row[2]) strncpy(records[count].card_name, row[2], sizeof(records[count].card_name) - 1);
        records[count].card_lev = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].card_type, row[4], sizeof(records[count].card_type) - 1);
        records[count].card_rare = row[5] ? atoi(row[5]) : 0;
        records[count].card_num = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
