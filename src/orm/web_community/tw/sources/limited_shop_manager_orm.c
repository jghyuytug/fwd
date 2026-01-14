#include "limited_shop_manager_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LimitedShopManager_Add(DBConnectionManager* manager, const LimitedShopManager* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO limited_shop_manager (occ_time, server_id, ipg_no, item_no, item_cnt, cera_price, gold_price, avatar_period_type, total_cnt, sell_cnt, restrict_no, start_time, end_time, real_end_time, npc_idx, cond_charac_job, cond_lev_begin, cond_lev_end, cond_acc_create_time_begin, cond_acc_create_time_end, cond_cha_create_time_begin, cond_cha_create_time_end, status_flag, title, range_section, reason_etc, reason_stop, pos_flag) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, '%s', '%s', '%s')",
        record->occ_time, record->server_id, record->ipg_no, record->item_no, record->item_cnt, record->cera_price, record->gold_price, record->avatar_period_type, record->total_cnt, record->sell_cnt, record->restrict_no, record->start_time, record->end_time, record->real_end_time, record->npc_idx, record->cond_charac_job, record->cond_lev_begin, record->cond_lev_end, record->cond_acc_create_time_begin, record->cond_acc_create_time_end, record->cond_cha_create_time_begin, record->cond_cha_create_time_end, record->status_flag, record->title, record->range_section, record->reason_etc, record->reason_stop, record->pos_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitedShopManager_Get(DBConnectionManager* manager, int no, signed char server_id, int ipg_no, LimitedShopManager* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[29];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, occ_time, server_id, ipg_no, item_no, item_cnt, cera_price, gold_price, avatar_period_type, total_cnt, sell_cnt, restrict_no, start_time, end_time, real_end_time, npc_idx, cond_charac_job, cond_lev_begin, cond_lev_end, cond_acc_create_time_begin, cond_acc_create_time_end, cond_cha_create_time_begin, cond_cha_create_time_end, status_flag, title, range_section, reason_etc, reason_stop, pos_flag FROM limited_shop_manager WHERE no = %d AND server_id = %d AND ipg_no = %d",
        no, server_id, ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LimitedShopManager));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->occ_time = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? atoi(row[2]) : 0;
    record->ipg_no = row[3] ? atoi(row[3]) : 0;
    record->item_no = row[4] ? atoi(row[4]) : 0;
    record->item_cnt = row[5] ? atoi(row[5]) : 0;
    record->cera_price = row[6] ? atoi(row[6]) : 0;
    record->gold_price = row[7] ? atoi(row[7]) : 0;
    record->avatar_period_type = row[8] ? atoi(row[8]) : 0;
    record->total_cnt = row[9] ? atoi(row[9]) : 0;
    record->sell_cnt = row[10] ? atoi(row[10]) : 0;
    record->restrict_no = row[11] ? atoi(row[11]) : 0;
    record->start_time = row[12] ? atoi(row[12]) : 0;
    record->end_time = row[13] ? atoi(row[13]) : 0;
    record->real_end_time = row[14] ? atoi(row[14]) : 0;
    record->npc_idx = row[15] ? atoi(row[15]) : 0;
    record->cond_charac_job = row[16] ? atoi(row[16]) : 0;
    record->cond_lev_begin = row[17] ? atoi(row[17]) : 0;
    record->cond_lev_end = row[18] ? atoi(row[18]) : 0;
    record->cond_acc_create_time_begin = row[19] ? atoi(row[19]) : 0;
    record->cond_acc_create_time_end = row[20] ? atoi(row[20]) : 0;
    record->cond_cha_create_time_begin = row[21] ? atoi(row[21]) : 0;
    record->cond_cha_create_time_end = row[22] ? atoi(row[22]) : 0;
    record->status_flag = row[23] ? atoi(row[23]) : 0;
    if (row[24]) strncpy(record->title, row[24], sizeof(record->title) - 1);
    record->range_section = row[25] ? atoi(row[25]) : 0;
    if (row[26]) strncpy(record->reason_etc, row[26], sizeof(record->reason_etc) - 1);
    if (row[27]) strncpy(record->reason_stop, row[27], sizeof(record->reason_stop) - 1);
    if (row[28]) strncpy(record->pos_flag, row[28], sizeof(record->pos_flag) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int LimitedShopManager_GetAll(DBConnectionManager* manager, LimitedShopManager* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[29];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, occ_time, server_id, ipg_no, item_no, item_cnt, cera_price, gold_price, avatar_period_type, total_cnt, sell_cnt, restrict_no, start_time, end_time, real_end_time, npc_idx, cond_charac_job, cond_lev_begin, cond_lev_end, cond_acc_create_time_begin, cond_acc_create_time_end, cond_cha_create_time_begin, cond_cha_create_time_end, status_flag, title, range_section, reason_etc, reason_stop, pos_flag FROM limited_shop_manager");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LimitedShopManager));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].occ_time = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        records[count].ipg_no = row[3] ? atoi(row[3]) : 0;
        records[count].item_no = row[4] ? atoi(row[4]) : 0;
        records[count].item_cnt = row[5] ? atoi(row[5]) : 0;
        records[count].cera_price = row[6] ? atoi(row[6]) : 0;
        records[count].gold_price = row[7] ? atoi(row[7]) : 0;
        records[count].avatar_period_type = row[8] ? atoi(row[8]) : 0;
        records[count].total_cnt = row[9] ? atoi(row[9]) : 0;
        records[count].sell_cnt = row[10] ? atoi(row[10]) : 0;
        records[count].restrict_no = row[11] ? atoi(row[11]) : 0;
        records[count].start_time = row[12] ? atoi(row[12]) : 0;
        records[count].end_time = row[13] ? atoi(row[13]) : 0;
        records[count].real_end_time = row[14] ? atoi(row[14]) : 0;
        records[count].npc_idx = row[15] ? atoi(row[15]) : 0;
        records[count].cond_charac_job = row[16] ? atoi(row[16]) : 0;
        records[count].cond_lev_begin = row[17] ? atoi(row[17]) : 0;
        records[count].cond_lev_end = row[18] ? atoi(row[18]) : 0;
        records[count].cond_acc_create_time_begin = row[19] ? atoi(row[19]) : 0;
        records[count].cond_acc_create_time_end = row[20] ? atoi(row[20]) : 0;
        records[count].cond_cha_create_time_begin = row[21] ? atoi(row[21]) : 0;
        records[count].cond_cha_create_time_end = row[22] ? atoi(row[22]) : 0;
        records[count].status_flag = row[23] ? atoi(row[23]) : 0;
        if (row[24]) strncpy(records[count].title, row[24], sizeof(records[count].title) - 1);
        records[count].range_section = row[25] ? atoi(row[25]) : 0;
        if (row[26]) strncpy(records[count].reason_etc, row[26], sizeof(records[count].reason_etc) - 1);
        if (row[27]) strncpy(records[count].reason_stop, row[27], sizeof(records[count].reason_stop) - 1);
        if (row[28]) strncpy(records[count].pos_flag, row[28], sizeof(records[count].pos_flag) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
