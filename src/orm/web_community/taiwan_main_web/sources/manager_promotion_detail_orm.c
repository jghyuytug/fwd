#include "manager_promotion_detail_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ManagerPromotionDetail_Add(DBConnectionManager* manager, const ManagerPromotionDetail* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO manager_promotion_detail (pro_no, pro_type, mag_id) "
        "VALUES (%d, %d, %d)",
        record->pro_no, record->pro_type, record->mag_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ManagerPromotionDetail_GetAll(DBConnectionManager* manager, ManagerPromotionDetail* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT pro_no, pro_type, mag_id FROM manager_promotion_detail");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ManagerPromotionDetail));
        records[count].pro_no = row[0] ? atoi(row[0]) : 0;
        records[count].pro_type = row[1] ? atoi(row[1]) : 0;
        records[count].mag_id = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
