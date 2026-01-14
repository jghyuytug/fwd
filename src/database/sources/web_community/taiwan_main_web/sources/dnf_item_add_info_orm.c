#include "dnf_item_add_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfItemAddInfo_Add(DBConnectionManager* manager, const DnfItemAddInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_item_add_info (it_no, dungeon, shop, pot, mix_item_no, link_it_no, ir_info, item_pic, add_item_memo, quest_name, view_flag, recomm_avg, recomm_cnt, recomm_score) "
        "VALUES (%d, '%s', '%s', %d, %d, '%s', '%s', '%s', '%s', '%s', %d, %f, %d, %d)",
        record->it_no, record->dungeon, record->shop, record->pot, record->mix_item_no, record->link_it_no, record->ir_info, record->item_pic, record->add_item_memo, record->quest_name, record->view_flag, record->recomm_avg, record->recomm_cnt, record->recomm_score);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfItemAddInfo_Get(DBConnectionManager* manager, int it_no, DnfItemAddInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_no, dungeon, shop, pot, mix_item_no, link_it_no, ir_info, item_pic, add_item_memo, quest_name, view_flag, recomm_avg, recomm_cnt, recomm_score FROM dnf_item_add_info WHERE it_no = %d",
        it_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfItemAddInfo));
    record->it_no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->dungeon, row[1], sizeof(record->dungeon) - 1);
    if (row[2]) strncpy(record->shop, row[2], sizeof(record->shop) - 1);
    record->pot = row[3] ? atoi(row[3]) : 0;
    record->mix_item_no = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->link_it_no, row[5], sizeof(record->link_it_no) - 1);
    if (row[6]) strncpy(record->ir_info, row[6], sizeof(record->ir_info) - 1);
    if (row[7]) strncpy(record->item_pic, row[7], sizeof(record->item_pic) - 1);
    if (row[8]) strncpy(record->add_item_memo, row[8], sizeof(record->add_item_memo) - 1);
    if (row[9]) strncpy(record->quest_name, row[9], sizeof(record->quest_name) - 1);
    record->view_flag = row[10] ? atoi(row[10]) : 0;
    record->recomm_avg = row[11] ? atof(row[11]) : 0;
    record->recomm_cnt = row[12] ? atoi(row[12]) : 0;
    record->recomm_score = row[13] ? atoi(row[13]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfItemAddInfo_GetAll(DBConnectionManager* manager, DnfItemAddInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_no, dungeon, shop, pot, mix_item_no, link_it_no, ir_info, item_pic, add_item_memo, quest_name, view_flag, recomm_avg, recomm_cnt, recomm_score FROM dnf_item_add_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfItemAddInfo));
        records[count].it_no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].dungeon, row[1], sizeof(records[count].dungeon) - 1);
        if (row[2]) strncpy(records[count].shop, row[2], sizeof(records[count].shop) - 1);
        records[count].pot = row[3] ? atoi(row[3]) : 0;
        records[count].mix_item_no = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].link_it_no, row[5], sizeof(records[count].link_it_no) - 1);
        if (row[6]) strncpy(records[count].ir_info, row[6], sizeof(records[count].ir_info) - 1);
        if (row[7]) strncpy(records[count].item_pic, row[7], sizeof(records[count].item_pic) - 1);
        if (row[8]) strncpy(records[count].add_item_memo, row[8], sizeof(records[count].add_item_memo) - 1);
        if (row[9]) strncpy(records[count].quest_name, row[9], sizeof(records[count].quest_name) - 1);
        records[count].view_flag = row[10] ? atoi(row[10]) : 0;
        records[count].recomm_avg = row[11] ? atof(row[11]) : 0;
        records[count].recomm_cnt = row[12] ? atoi(row[12]) : 0;
        records[count].recomm_score = row[13] ? atoi(row[13]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
