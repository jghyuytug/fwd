#include "dnf_mobile_board_comments_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfMobileBoardComments_Add(DBConnectionManager* manager, const DnfMobileBoardComments* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_mobile_board_comments (dmb_idx, writer_name, comments, occ_time) "
        "VALUES (%d, '%s', '%s', %d)",
        record->dmb_idx, record->writer_name, record->comments, record->occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMobileBoardComments_Get(DBConnectionManager* manager, int no, DnfMobileBoardComments* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, dmb_idx, writer_name, comments, occ_time FROM dnf_mobile_board_comments WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfMobileBoardComments));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->dmb_idx = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->writer_name, row[2], sizeof(record->writer_name) - 1);
    if (row[3]) strncpy(record->comments, row[3], sizeof(record->comments) - 1);
    record->occ_time = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMobileBoardComments_GetAll(DBConnectionManager* manager, DnfMobileBoardComments* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, dmb_idx, writer_name, comments, occ_time FROM dnf_mobile_board_comments");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfMobileBoardComments));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].dmb_idx = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].writer_name, row[2], sizeof(records[count].writer_name) - 1);
        if (row[3]) strncpy(records[count].comments, row[3], sizeof(records[count].comments) - 1);
        records[count].occ_time = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
