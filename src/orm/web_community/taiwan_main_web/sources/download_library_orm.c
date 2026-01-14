#include "download_library_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DownloadLibrary_Add(DBConnectionManager* manager, const DownloadLibrary* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO download_library (category, m_id, m_user_id, open, main, title, create_day, view, download, img_link) "
        "VALUES (%d, %d, '%s', %d, %d, '%s', %d, %d, %d, '%s')",
        record->category, record->m_id, record->m_user_id, record->open, record->main, record->title, record->create_day, record->view, record->download, record->img_link);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DownloadLibrary_Get(DBConnectionManager* manager, int no, DownloadLibrary* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, category, m_id, m_user_id, open, main, title, create_day, view, download, img_link FROM download_library WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DownloadLibrary));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->category = row[1] ? atoi(row[1]) : 0;
    record->m_id = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->m_user_id, row[3], sizeof(record->m_user_id) - 1);
    record->open = row[4] ? atoi(row[4]) : 0;
    record->main = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->title, row[6], sizeof(record->title) - 1);
    record->create_day = row[7] ? atoi(row[7]) : 0;
    record->view = row[8] ? atoi(row[8]) : 0;
    record->download = row[9] ? atoi(row[9]) : 0;
    if (row[10]) strncpy(record->img_link, row[10], sizeof(record->img_link) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DownloadLibrary_GetAll(DBConnectionManager* manager, DownloadLibrary* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, category, m_id, m_user_id, open, main, title, create_day, view, download, img_link FROM download_library");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DownloadLibrary));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].category = row[1] ? atoi(row[1]) : 0;
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].m_user_id, row[3], sizeof(records[count].m_user_id) - 1);
        records[count].open = row[4] ? atoi(row[4]) : 0;
        records[count].main = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].title, row[6], sizeof(records[count].title) - 1);
        records[count].create_day = row[7] ? atoi(row[7]) : 0;
        records[count].view = row[8] ? atoi(row[8]) : 0;
        records[count].download = row[9] ? atoi(row[9]) : 0;
        if (row[10]) strncpy(records[count].img_link, row[10], sizeof(records[count].img_link) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
