#include "gen_event_detail_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GenEventDetail_Add(DBConnectionManager* manager, const GenEventDetail* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO gen_event_detail (gno, type_flag1, type_flag2, type_flag3, type_flag4, type_flag5, type_flag6, content1, file_location1, content2, file_location2, content3, file_location3, content4, file_location4, content5, file_location5, content6, file_location6) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
        record->gno, record->type_flag1, record->type_flag2, record->type_flag3, record->type_flag4, record->type_flag5, record->type_flag6, record->content1, record->file_location1, record->content2, record->file_location2, record->content3, record->file_location3, record->content4, record->file_location4, record->content5, record->file_location5, record->content6, record->file_location6);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GenEventDetail_Get(DBConnectionManager* manager, int gno, GenEventDetail* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[19];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, type_flag1, type_flag2, type_flag3, type_flag4, type_flag5, type_flag6, content1, file_location1, content2, file_location2, content3, file_location3, content4, file_location4, content5, file_location5, content6, file_location6 FROM gen_event_detail WHERE gno = %d",
        gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GenEventDetail));
    record->gno = row[0] ? atoi(row[0]) : 0;
    record->type_flag1 = row[1] ? atoi(row[1]) : 0;
    record->type_flag2 = row[2] ? atoi(row[2]) : 0;
    record->type_flag3 = row[3] ? atoi(row[3]) : 0;
    record->type_flag4 = row[4] ? atoi(row[4]) : 0;
    record->type_flag5 = row[5] ? atoi(row[5]) : 0;
    record->type_flag6 = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->content1, row[7], sizeof(record->content1) - 1);
    if (row[8]) strncpy(record->file_location1, row[8], sizeof(record->file_location1) - 1);
    if (row[9]) strncpy(record->content2, row[9], sizeof(record->content2) - 1);
    if (row[10]) strncpy(record->file_location2, row[10], sizeof(record->file_location2) - 1);
    if (row[11]) strncpy(record->content3, row[11], sizeof(record->content3) - 1);
    if (row[12]) strncpy(record->file_location3, row[12], sizeof(record->file_location3) - 1);
    if (row[13]) strncpy(record->content4, row[13], sizeof(record->content4) - 1);
    if (row[14]) strncpy(record->file_location4, row[14], sizeof(record->file_location4) - 1);
    if (row[15]) strncpy(record->content5, row[15], sizeof(record->content5) - 1);
    if (row[16]) strncpy(record->file_location5, row[16], sizeof(record->file_location5) - 1);
    if (row[17]) strncpy(record->content6, row[17], sizeof(record->content6) - 1);
    if (row[18]) strncpy(record->file_location6, row[18], sizeof(record->file_location6) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GenEventDetail_GetAll(DBConnectionManager* manager, GenEventDetail* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[19];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, type_flag1, type_flag2, type_flag3, type_flag4, type_flag5, type_flag6, content1, file_location1, content2, file_location2, content3, file_location3, content4, file_location4, content5, file_location5, content6, file_location6 FROM gen_event_detail");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GenEventDetail));
        records[count].gno = row[0] ? atoi(row[0]) : 0;
        records[count].type_flag1 = row[1] ? atoi(row[1]) : 0;
        records[count].type_flag2 = row[2] ? atoi(row[2]) : 0;
        records[count].type_flag3 = row[3] ? atoi(row[3]) : 0;
        records[count].type_flag4 = row[4] ? atoi(row[4]) : 0;
        records[count].type_flag5 = row[5] ? atoi(row[5]) : 0;
        records[count].type_flag6 = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].content1, row[7], sizeof(records[count].content1) - 1);
        if (row[8]) strncpy(records[count].file_location1, row[8], sizeof(records[count].file_location1) - 1);
        if (row[9]) strncpy(records[count].content2, row[9], sizeof(records[count].content2) - 1);
        if (row[10]) strncpy(records[count].file_location2, row[10], sizeof(records[count].file_location2) - 1);
        if (row[11]) strncpy(records[count].content3, row[11], sizeof(records[count].content3) - 1);
        if (row[12]) strncpy(records[count].file_location3, row[12], sizeof(records[count].file_location3) - 1);
        if (row[13]) strncpy(records[count].content4, row[13], sizeof(records[count].content4) - 1);
        if (row[14]) strncpy(records[count].file_location4, row[14], sizeof(records[count].file_location4) - 1);
        if (row[15]) strncpy(records[count].content5, row[15], sizeof(records[count].content5) - 1);
        if (row[16]) strncpy(records[count].file_location5, row[16], sizeof(records[count].file_location5) - 1);
        if (row[17]) strncpy(records[count].content6, row[17], sizeof(records[count].content6) - 1);
        if (row[18]) strncpy(records[count].file_location6, row[18], sizeof(records[count].file_location6) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
