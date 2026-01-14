#include "knowcafe_gift_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowcafeGiftEntry_Add(DBConnectionManager* manager, const KnowcafeGiftEntry* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowcafe_gift_entry (m_id, occ_date, server_id, charac_no, item_no, item_cnt, item_check, total_point, occ_type) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->m_id, record->occ_date, record->server_id, record->charac_no, record->item_no, record->item_cnt, record->item_check, record->total_point, record->occ_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafeGiftEntry_Get(DBConnectionManager* manager, int no, KnowcafeGiftEntry* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, occ_date, server_id, charac_no, item_no, item_cnt, item_check, total_point, occ_type FROM knowcafe_gift_entry WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(KnowcafeGiftEntry));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->occ_date = row[2] ? atoi(row[2]) : 0;
    record->server_id = row[3] ? atoi(row[3]) : 0;
    record->charac_no = row[4] ? atoi(row[4]) : 0;
    record->item_no = row[5] ? atoi(row[5]) : 0;
    record->item_cnt = row[6] ? atoi(row[6]) : 0;
    record->item_check = row[7] ? atoi(row[7]) : 0;
    record->total_point = row[8] ? atoi(row[8]) : 0;
    record->occ_type = row[9] ? atoi(row[9]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafeGiftEntry_GetAll(DBConnectionManager* manager, KnowcafeGiftEntry* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, occ_date, server_id, charac_no, item_no, item_cnt, item_check, total_point, occ_type FROM knowcafe_gift_entry");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowcafeGiftEntry));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].occ_date = row[2] ? atoi(row[2]) : 0;
        records[count].server_id = row[3] ? atoi(row[3]) : 0;
        records[count].charac_no = row[4] ? atoi(row[4]) : 0;
        records[count].item_no = row[5] ? atoi(row[5]) : 0;
        records[count].item_cnt = row[6] ? atoi(row[6]) : 0;
        records[count].item_check = row[7] ? atoi(row[7]) : 0;
        records[count].total_point = row[8] ? atoi(row[8]) : 0;
        records[count].occ_type = row[9] ? atoi(row[9]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
