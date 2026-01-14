#include "manager_promotion_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ManagerPromotionInfo_Add(DBConnectionManager* manager, const ManagerPromotionInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO manager_promotion_info (promotion_name, occ_date, user_id, promotion_explain) "
        "VALUES ('%s', '%s', '%s', '%s')",
        record->promotion_name, record->occ_date, record->user_id, record->promotion_explain);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ManagerPromotionInfo_Get(DBConnectionManager* manager, int pro_no, ManagerPromotionInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT pro_no, promotion_name, occ_date, user_id, promotion_explain FROM manager_promotion_info WHERE pro_no = %d",
        pro_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ManagerPromotionInfo));
    record->pro_no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->promotion_name, row[1], sizeof(record->promotion_name) - 1);
    if (row[2]) strncpy(record->occ_date, row[2], sizeof(record->occ_date) - 1);
    if (row[3]) strncpy(record->user_id, row[3], sizeof(record->user_id) - 1);
    if (row[4]) strncpy(record->promotion_explain, row[4], sizeof(record->promotion_explain) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int ManagerPromotionInfo_GetAll(DBConnectionManager* manager, ManagerPromotionInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT pro_no, promotion_name, occ_date, user_id, promotion_explain FROM manager_promotion_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ManagerPromotionInfo));
        records[count].pro_no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].promotion_name, row[1], sizeof(records[count].promotion_name) - 1);
        if (row[2]) strncpy(records[count].occ_date, row[2], sizeof(records[count].occ_date) - 1);
        if (row[3]) strncpy(records[count].user_id, row[3], sizeof(records[count].user_id) - 1);
        if (row[4]) strncpy(records[count].promotion_explain, row[4], sizeof(records[count].promotion_explain) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
