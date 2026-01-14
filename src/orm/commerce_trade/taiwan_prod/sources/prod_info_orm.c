#include "prod_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ProdInfo_Add(DBConnectionManager* manager, const ProdInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO prod_info (prod_part_id, ipg_no, prod_step, it_no, prod_name, master_type, sub_type, job, class, set_flag, sell_type, prod_stat, prod_gold, prod_winpoint, prod_price, prod_cnt, prod_supply, public_flag, prod_limit_flag, prod_period, prod_icon, bonus_weight, bonus_rate, bonus_flag, prod_explain, regist_user_id, regist_time, mod_user_id, mod_time) "
        "VALUES (%d, %d, %d, %d, '%s', %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, '%s', '%s', '%s', '%s', '%s')",
        record->prod_part_id, record->ipg_no, record->prod_step, record->it_no, record->prod_name, record->master_type, record->sub_type, record->job, record->class, record->set_flag, record->sell_type, record->prod_stat, record->prod_gold, record->prod_winpoint, record->prod_price, record->prod_cnt, record->prod_supply, record->public_flag, record->prod_limit_flag, record->prod_period, record->prod_icon, record->bonus_weight, record->bonus_rate, record->bonus_flag, record->prod_explain, record->regist_user_id, record->regist_time, record->mod_user_id, record->mod_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ProdInfo_Get(DBConnectionManager* manager, int ipg_no, ProdInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[29];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT prod_part_id, ipg_no, prod_step, it_no, prod_name, master_type, sub_type, job, class, set_flag, sell_type, prod_stat, prod_gold, prod_winpoint, prod_price, prod_cnt, prod_supply, public_flag, prod_limit_flag, prod_period, prod_icon, bonus_weight, bonus_rate, bonus_flag, prod_explain, regist_user_id, regist_time, mod_user_id, mod_time FROM prod_info WHERE ipg_no = %d",
        ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ProdInfo));
    record->prod_part_id = row[0] ? atoi(row[0]) : 0;
    record->ipg_no = row[1] ? atoi(row[1]) : 0;
    record->prod_step = row[2] ? atoi(row[2]) : 0;
    record->it_no = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->prod_name, row[4], sizeof(record->prod_name) - 1);
    record->master_type = row[5] ? atoi(row[5]) : 0;
    record->sub_type = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->job, row[7], sizeof(record->job) - 1);
    record->class = row[8] ? atoi(row[8]) : 0;
    record->set_flag = row[9] ? atoi(row[9]) : 0;
    record->sell_type = row[10] ? atoi(row[10]) : 0;
    record->prod_stat = row[11] ? atoi(row[11]) : 0;
    record->prod_gold = row[12] ? atoi(row[12]) : 0;
    record->prod_winpoint = row[13] ? atoi(row[13]) : 0;
    record->prod_price = row[14] ? atoi(row[14]) : 0;
    record->prod_cnt = row[15] ? atoi(row[15]) : 0;
    record->prod_supply = row[16] ? atoi(row[16]) : 0;
    record->public_flag = row[17] ? atoi(row[17]) : 0;
    record->prod_limit_flag = row[18] ? atoi(row[18]) : 0;
    record->prod_period = row[19] ? atoi(row[19]) : 0;
    if (row[20]) strncpy(record->prod_icon, row[20], sizeof(record->prod_icon) - 1);
    record->bonus_weight = row[21] ? atoi(row[21]) : 0;
    record->bonus_rate = row[22] ? atoi(row[22]) : 0;
    record->bonus_flag = row[23] ? atoi(row[23]) : 0;
    if (row[24]) strncpy(record->prod_explain, row[24], sizeof(record->prod_explain) - 1);
    if (row[25]) strncpy(record->regist_user_id, row[25], sizeof(record->regist_user_id) - 1);
    if (row[26]) strncpy(record->regist_time, row[26], sizeof(record->regist_time) - 1);
    if (row[27]) strncpy(record->mod_user_id, row[27], sizeof(record->mod_user_id) - 1);
    if (row[28]) strncpy(record->mod_time, row[28], sizeof(record->mod_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int ProdInfo_GetAll(DBConnectionManager* manager, ProdInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[29];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT prod_part_id, ipg_no, prod_step, it_no, prod_name, master_type, sub_type, job, class, set_flag, sell_type, prod_stat, prod_gold, prod_winpoint, prod_price, prod_cnt, prod_supply, public_flag, prod_limit_flag, prod_period, prod_icon, bonus_weight, bonus_rate, bonus_flag, prod_explain, regist_user_id, regist_time, mod_user_id, mod_time FROM prod_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ProdInfo));
        records[count].prod_part_id = row[0] ? atoi(row[0]) : 0;
        records[count].ipg_no = row[1] ? atoi(row[1]) : 0;
        records[count].prod_step = row[2] ? atoi(row[2]) : 0;
        records[count].it_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].prod_name, row[4], sizeof(records[count].prod_name) - 1);
        records[count].master_type = row[5] ? atoi(row[5]) : 0;
        records[count].sub_type = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].job, row[7], sizeof(records[count].job) - 1);
        records[count].class = row[8] ? atoi(row[8]) : 0;
        records[count].set_flag = row[9] ? atoi(row[9]) : 0;
        records[count].sell_type = row[10] ? atoi(row[10]) : 0;
        records[count].prod_stat = row[11] ? atoi(row[11]) : 0;
        records[count].prod_gold = row[12] ? atoi(row[12]) : 0;
        records[count].prod_winpoint = row[13] ? atoi(row[13]) : 0;
        records[count].prod_price = row[14] ? atoi(row[14]) : 0;
        records[count].prod_cnt = row[15] ? atoi(row[15]) : 0;
        records[count].prod_supply = row[16] ? atoi(row[16]) : 0;
        records[count].public_flag = row[17] ? atoi(row[17]) : 0;
        records[count].prod_limit_flag = row[18] ? atoi(row[18]) : 0;
        records[count].prod_period = row[19] ? atoi(row[19]) : 0;
        if (row[20]) strncpy(records[count].prod_icon, row[20], sizeof(records[count].prod_icon) - 1);
        records[count].bonus_weight = row[21] ? atoi(row[21]) : 0;
        records[count].bonus_rate = row[22] ? atoi(row[22]) : 0;
        records[count].bonus_flag = row[23] ? atoi(row[23]) : 0;
        if (row[24]) strncpy(records[count].prod_explain, row[24], sizeof(records[count].prod_explain) - 1);
        if (row[25]) strncpy(records[count].regist_user_id, row[25], sizeof(records[count].regist_user_id) - 1);
        if (row[26]) strncpy(records[count].regist_time, row[26], sizeof(records[count].regist_time) - 1);
        if (row[27]) strncpy(records[count].mod_user_id, row[27], sizeof(records[count].mod_user_id) - 1);
        if (row[28]) strncpy(records[count].mod_time, row[28], sizeof(records[count].mod_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
