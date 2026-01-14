#include "knowl_bbs_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowlBbs_Add(DBConnectionManager* manager, const KnowlBbs* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowl_bbs (kn_id, reg_date, mod_date, hits, body_type, reg_id, server_id, charac_no, charac_name, job, subject, status, up, down, other_flag, url_go, url_to) "
        "VALUES (%d, %d, %d, %d, '%s', %d, %d, %d, '%s', %d, '%s', %d, %d, %d, %d, '%s', '%s')",
        record->kn_id, record->reg_date, record->mod_date, record->hits, record->body_type, record->reg_id, record->server_id, record->charac_no, record->charac_name, record->job, record->subject, record->status, record->up, record->down, record->other_flag, record->url_go, record->url_to);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowlBbs_Get(DBConnectionManager* manager, int gno, KnowlBbs* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[18];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, kn_id, reg_date, mod_date, hits, body_type, reg_id, server_id, charac_no, charac_name, job, subject, status, up, down, other_flag, url_go, url_to FROM knowl_bbs WHERE gno = %d",
        gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(KnowlBbs));
    record->gno = row[0] ? atoi(row[0]) : 0;
    record->kn_id = row[1] ? atoi(row[1]) : 0;
    record->reg_date = row[2] ? atoi(row[2]) : 0;
    record->mod_date = row[3] ? atoi(row[3]) : 0;
    record->hits = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->body_type, row[5], sizeof(record->body_type) - 1);
    record->reg_id = row[6] ? atoi(row[6]) : 0;
    record->server_id = row[7] ? atoi(row[7]) : 0;
    record->charac_no = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->charac_name, row[9], sizeof(record->charac_name) - 1);
    record->job = row[10] ? atoi(row[10]) : 0;
    if (row[11]) strncpy(record->subject, row[11], sizeof(record->subject) - 1);
    record->status = row[12] ? atoi(row[12]) : 0;
    record->up = row[13] ? atoi(row[13]) : 0;
    record->down = row[14] ? atoi(row[14]) : 0;
    record->other_flag = row[15] ? atoi(row[15]) : 0;
    if (row[16]) strncpy(record->url_go, row[16], sizeof(record->url_go) - 1);
    if (row[17]) strncpy(record->url_to, row[17], sizeof(record->url_to) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int KnowlBbs_GetAll(DBConnectionManager* manager, KnowlBbs* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[18];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, kn_id, reg_date, mod_date, hits, body_type, reg_id, server_id, charac_no, charac_name, job, subject, status, up, down, other_flag, url_go, url_to FROM knowl_bbs");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowlBbs));
        records[count].gno = row[0] ? atoi(row[0]) : 0;
        records[count].kn_id = row[1] ? atoi(row[1]) : 0;
        records[count].reg_date = row[2] ? atoi(row[2]) : 0;
        records[count].mod_date = row[3] ? atoi(row[3]) : 0;
        records[count].hits = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].body_type, row[5], sizeof(records[count].body_type) - 1);
        records[count].reg_id = row[6] ? atoi(row[6]) : 0;
        records[count].server_id = row[7] ? atoi(row[7]) : 0;
        records[count].charac_no = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].charac_name, row[9], sizeof(records[count].charac_name) - 1);
        records[count].job = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].subject, row[11], sizeof(records[count].subject) - 1);
        records[count].status = row[12] ? atoi(row[12]) : 0;
        records[count].up = row[13] ? atoi(row[13]) : 0;
        records[count].down = row[14] ? atoi(row[14]) : 0;
        records[count].other_flag = row[15] ? atoi(row[15]) : 0;
        if (row[16]) strncpy(records[count].url_go, row[16], sizeof(records[count].url_go) - 1);
        if (row[17]) strncpy(records[count].url_to, row[17], sizeof(records[count].url_to) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
