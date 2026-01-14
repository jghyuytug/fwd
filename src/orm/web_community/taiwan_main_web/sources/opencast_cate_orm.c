#include "opencast_cate_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int OpencastCate_Add(DBConnectionManager* manager, const OpencastCate* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO opencast_cate (cate_name, reg_date, open_date) "
        "VALUES ('%s', '%s', '%s')",
        record->cate_name, record->reg_date, record->open_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int OpencastCate_Get(DBConnectionManager* manager, int cate_no, OpencastCate* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT cate_no, cate_name, reg_date, open_date FROM opencast_cate WHERE cate_no = %d",
        cate_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(OpencastCate));
    record->cate_no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->cate_name, row[1], sizeof(record->cate_name) - 1);
    if (row[2]) strncpy(record->reg_date, row[2], sizeof(record->reg_date) - 1);
    if (row[3]) strncpy(record->open_date, row[3], sizeof(record->open_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int OpencastCate_GetAll(DBConnectionManager* manager, OpencastCate* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT cate_no, cate_name, reg_date, open_date FROM opencast_cate");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(OpencastCate));
        records[count].cate_no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].cate_name, row[1], sizeof(records[count].cate_name) - 1);
        if (row[2]) strncpy(records[count].reg_date, row[2], sizeof(records[count].reg_date) - 1);
        if (row[3]) strncpy(records[count].open_date, row[3], sizeof(records[count].open_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
