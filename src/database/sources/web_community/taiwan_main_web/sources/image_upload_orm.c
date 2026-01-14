#include "image_upload_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ImageUpload_Add(DBConnectionManager* manager, const ImageUpload* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO image_upload (date_start, date_end, img_url) "
        "VALUES ('%s', '%s', '%s')",
        record->date_start, record->date_end, record->img_url);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ImageUpload_Get(DBConnectionManager* manager, int no, ImageUpload* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, date_start, date_end, img_url FROM image_upload WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ImageUpload));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->date_start, row[1], sizeof(record->date_start) - 1);
    if (row[2]) strncpy(record->date_end, row[2], sizeof(record->date_end) - 1);
    if (row[3]) strncpy(record->img_url, row[3], sizeof(record->img_url) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int ImageUpload_GetAll(DBConnectionManager* manager, ImageUpload* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, date_start, date_end, img_url FROM image_upload");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ImageUpload));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].date_start, row[1], sizeof(records[count].date_start) - 1);
        if (row[2]) strncpy(records[count].date_end, row[2], sizeof(records[count].date_end) - 1);
        if (row[3]) strncpy(records[count].img_url, row[3], sizeof(records[count].img_url) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
