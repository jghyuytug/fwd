#include "prod_sale_entry_124_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ProdSaleEntry124_Add(DBConnectionManager* manager, const ProdSaleEntry124* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO prod_sale_entry_124 (agency_no, m_id, charac_no, job, grow_type, level, ipg_no, price, buy_type, gift_server_info, gift_get_charac_no, sale_time, charge_price, free_price, count_no, each_price, price_type, seal_flag, slot_part_type, slot_part_code, buy_method_type, prod_part_id, bonus_ipg_no, user_id, server_id, prize_flag, new_flag, result_code, result_message, user_ip) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, '%s', '%s', '%s')",
        record->agency_no, record->m_id, record->charac_no, record->job, record->grow_type, record->level, record->ipg_no, record->price, record->buy_type, record->gift_server_info, record->gift_get_charac_no, record->sale_time, record->charge_price, record->free_price, record->count_no, record->each_price, record->price_type, record->seal_flag, record->slot_part_type, record->slot_part_code, record->buy_method_type, record->prod_part_id, record->bonus_ipg_no, record->user_id, record->server_id, record->prize_flag, record->new_flag, record->result_code, record->result_message, record->user_ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ProdSaleEntry124_Get(DBConnectionManager* manager, const char* agency_no, ProdSaleEntry124* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[30];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT agency_no, m_id, charac_no, job, grow_type, level, ipg_no, price, buy_type, gift_server_info, gift_get_charac_no, sale_time, charge_price, free_price, count_no, each_price, price_type, seal_flag, slot_part_type, slot_part_code, buy_method_type, prod_part_id, bonus_ipg_no, user_id, server_id, prize_flag, new_flag, result_code, result_message, user_ip FROM prod_sale_entry_124 WHERE agency_no = '%s'",
        agency_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ProdSaleEntry124));
    if (row[0]) strncpy(record->agency_no, row[0], sizeof(record->agency_no) - 1);
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    record->job = row[3] ? atoi(row[3]) : 0;
    record->grow_type = row[4] ? atoi(row[4]) : 0;
    record->level = row[5] ? atoi(row[5]) : 0;
    record->ipg_no = row[6] ? atoi(row[6]) : 0;
    record->price = row[7] ? atoi(row[7]) : 0;
    record->buy_type = row[8] ? atoi(row[8]) : 0;
    record->gift_server_info = row[9] ? atoi(row[9]) : 0;
    record->gift_get_charac_no = row[10] ? atoi(row[10]) : 0;
    if (row[11]) strncpy(record->sale_time, row[11], sizeof(record->sale_time) - 1);
    record->charge_price = row[12] ? atoi(row[12]) : 0;
    record->free_price = row[13] ? atoi(row[13]) : 0;
    record->count_no = row[14] ? atoi(row[14]) : 0;
    record->each_price = row[15] ? atoi(row[15]) : 0;
    record->price_type = row[16] ? atoi(row[16]) : 0;
    record->seal_flag = row[17] ? atoi(row[17]) : 0;
    record->slot_part_type = row[18] ? atoi(row[18]) : 0;
    record->slot_part_code = row[19] ? atoi(row[19]) : 0;
    record->buy_method_type = row[20] ? atoi(row[20]) : 0;
    record->prod_part_id = row[21] ? atoi(row[21]) : 0;
    record->bonus_ipg_no = row[22] ? atoi(row[22]) : 0;
    if (row[23]) strncpy(record->user_id, row[23], sizeof(record->user_id) - 1);
    record->server_id = row[24] ? atoi(row[24]) : 0;
    record->prize_flag = row[25] ? atoi(row[25]) : 0;
    record->new_flag = row[26] ? atoi(row[26]) : 0;
    if (row[27]) strncpy(record->result_code, row[27], sizeof(record->result_code) - 1);
    if (row[28]) strncpy(record->result_message, row[28], sizeof(record->result_message) - 1);
    if (row[29]) strncpy(record->user_ip, row[29], sizeof(record->user_ip) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int ProdSaleEntry124_GetAll(DBConnectionManager* manager, ProdSaleEntry124* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[30];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT agency_no, m_id, charac_no, job, grow_type, level, ipg_no, price, buy_type, gift_server_info, gift_get_charac_no, sale_time, charge_price, free_price, count_no, each_price, price_type, seal_flag, slot_part_type, slot_part_code, buy_method_type, prod_part_id, bonus_ipg_no, user_id, server_id, prize_flag, new_flag, result_code, result_message, user_ip FROM prod_sale_entry_124");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ProdSaleEntry124));
        if (row[0]) strncpy(records[count].agency_no, row[0], sizeof(records[count].agency_no) - 1);
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        records[count].job = row[3] ? atoi(row[3]) : 0;
        records[count].grow_type = row[4] ? atoi(row[4]) : 0;
        records[count].level = row[5] ? atoi(row[5]) : 0;
        records[count].ipg_no = row[6] ? atoi(row[6]) : 0;
        records[count].price = row[7] ? atoi(row[7]) : 0;
        records[count].buy_type = row[8] ? atoi(row[8]) : 0;
        records[count].gift_server_info = row[9] ? atoi(row[9]) : 0;
        records[count].gift_get_charac_no = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].sale_time, row[11], sizeof(records[count].sale_time) - 1);
        records[count].charge_price = row[12] ? atoi(row[12]) : 0;
        records[count].free_price = row[13] ? atoi(row[13]) : 0;
        records[count].count_no = row[14] ? atoi(row[14]) : 0;
        records[count].each_price = row[15] ? atoi(row[15]) : 0;
        records[count].price_type = row[16] ? atoi(row[16]) : 0;
        records[count].seal_flag = row[17] ? atoi(row[17]) : 0;
        records[count].slot_part_type = row[18] ? atoi(row[18]) : 0;
        records[count].slot_part_code = row[19] ? atoi(row[19]) : 0;
        records[count].buy_method_type = row[20] ? atoi(row[20]) : 0;
        records[count].prod_part_id = row[21] ? atoi(row[21]) : 0;
        records[count].bonus_ipg_no = row[22] ? atoi(row[22]) : 0;
        if (row[23]) strncpy(records[count].user_id, row[23], sizeof(records[count].user_id) - 1);
        records[count].server_id = row[24] ? atoi(row[24]) : 0;
        records[count].prize_flag = row[25] ? atoi(row[25]) : 0;
        records[count].new_flag = row[26] ? atoi(row[26]) : 0;
        if (row[27]) strncpy(records[count].result_code, row[27], sizeof(records[count].result_code) - 1);
        if (row[28]) strncpy(records[count].result_message, row[28], sizeof(records[count].result_message) - 1);
        if (row[29]) strncpy(records[count].user_ip, row[29], sizeof(records[count].user_ip) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
