#include "guide_view_cnt_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuideViewCnt_Add(DBConnectionManager* manager, const GuideViewCnt* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guide_view_cnt (bbs_cd, bbs_no, occ_time, click_cnt, day_cnt) "
        "VALUES (%d, %d, %d, %d, %d)",
        record->bbs_cd, record->bbs_no, record->occ_time, record->click_cnt, record->day_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuideViewCnt_Get(DBConnectionManager* manager, int no, GuideViewCnt* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, bbs_cd, bbs_no, occ_time, click_cnt, day_cnt FROM guide_view_cnt WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuideViewCnt));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->bbs_cd = row[1] ? atoi(row[1]) : 0;
    record->bbs_no = row[2] ? atoi(row[2]) : 0;
    record->occ_time = row[3] ? atoi(row[3]) : 0;
    record->click_cnt = row[4] ? atoi(row[4]) : 0;
    record->day_cnt = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GuideViewCnt_GetAll(DBConnectionManager* manager, GuideViewCnt* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, bbs_cd, bbs_no, occ_time, click_cnt, day_cnt FROM guide_view_cnt");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuideViewCnt));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].bbs_cd = row[1] ? atoi(row[1]) : 0;
        records[count].bbs_no = row[2] ? atoi(row[2]) : 0;
        records[count].occ_time = row[3] ? atoi(row[3]) : 0;
        records[count].click_cnt = row[4] ? atoi(row[4]) : 0;
        records[count].day_cnt = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
