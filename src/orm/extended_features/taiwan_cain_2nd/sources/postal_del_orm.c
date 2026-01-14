#include "postal_del_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int PostalDel_Add(DBConnectionManager* manager, const PostalDel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO postal_del (sdate, postal_id, occ_time, send_charac_no, send_charac_name, receive_charac_no, item_id, add_info, endurance, upgrade, amplify_option, amplify_value, gold, receive_time, delete_flag, avata_flag, unlimit_flag, seal_flag, creature_flag, postal, letter_id, extend_info, ipg_db_id, ipg_transaction_id, ipg_nexon_id, auction_id, random_option, seperate_upgrade, type, item_guid) "
        "VALUES ('%s', %d, '%s', %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, '%s', %d, %d, '%s')",
        record->sdate, record->postal_id, record->occ_time, record->send_charac_no, record->send_charac_name, record->receive_charac_no, record->item_id, record->add_info, record->endurance, record->upgrade, record->amplify_option, record->amplify_value, record->gold, record->receive_time, record->delete_flag, record->avata_flag, record->unlimit_flag, record->seal_flag, record->creature_flag, record->postal, record->letter_id, record->extend_info, record->ipg_db_id, record->ipg_transaction_id, record->ipg_nexon_id, record->auction_id, record->random_option, record->seperate_upgrade, record->type, record->item_guid);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PostalDel_Get(DBConnectionManager* manager, const char* sdate, int postal_id, PostalDel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[30];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT sdate, postal_id, occ_time, send_charac_no, send_charac_name, receive_charac_no, item_id, add_info, endurance, upgrade, amplify_option, amplify_value, gold, receive_time, delete_flag, avata_flag, unlimit_flag, seal_flag, creature_flag, postal, letter_id, extend_info, ipg_db_id, ipg_transaction_id, ipg_nexon_id, auction_id, random_option, seperate_upgrade, type, item_guid FROM postal_del WHERE sdate = '%s' AND postal_id = %d",
        sdate, postal_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(PostalDel));
    if (row[0]) strncpy(record->sdate, row[0], sizeof(record->sdate) - 1);
    record->postal_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->occ_time, row[2], sizeof(record->occ_time) - 1);
    record->send_charac_no = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->send_charac_name, row[4], sizeof(record->send_charac_name) - 1);
    record->receive_charac_no = row[5] ? atoi(row[5]) : 0;
    record->item_id = row[6] ? atoi(row[6]) : 0;
    record->add_info = row[7] ? atoi(row[7]) : 0;
    record->endurance = row[8] ? atoi(row[8]) : 0;
    record->upgrade = row[9] ? atoi(row[9]) : 0;
    record->amplify_option = row[10] ? atoi(row[10]) : 0;
    record->amplify_value = row[11] ? atoi(row[11]) : 0;
    record->gold = row[12] ? atoi(row[12]) : 0;
    if (row[13]) strncpy(record->receive_time, row[13], sizeof(record->receive_time) - 1);
    record->delete_flag = row[14] ? atoi(row[14]) : 0;
    record->avata_flag = row[15] ? atoi(row[15]) : 0;
    record->unlimit_flag = row[16] ? atoi(row[16]) : 0;
    record->seal_flag = row[17] ? atoi(row[17]) : 0;
    record->creature_flag = row[18] ? atoi(row[18]) : 0;
    record->postal = row[19] ? atoi(row[19]) : 0;
    record->letter_id = row[20] ? atoi(row[20]) : 0;
    record->extend_info = row[21] ? atoi(row[21]) : 0;
    record->ipg_db_id = row[22] ? atoi(row[22]) : 0;
    record->ipg_transaction_id = row[23] ? atoi(row[23]) : 0;
    if (row[24]) strncpy(record->ipg_nexon_id, row[24], sizeof(record->ipg_nexon_id) - 1);
    record->auction_id = row[25] ? atoll(row[25]) : 0;
    if (row[26]) memcpy(record->random_option, row[26], sizeof(record->random_option));
    record->seperate_upgrade = row[27] ? atoi(row[27]) : 0;
    record->type = row[28] ? atoi(row[28]) : 0;
    if (row[29]) memcpy(record->item_guid, row[29], sizeof(record->item_guid));

    DBQueryResult_Free(&result);
    return 0;
}

int PostalDel_GetAll(DBConnectionManager* manager, PostalDel* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[30];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT sdate, postal_id, occ_time, send_charac_no, send_charac_name, receive_charac_no, item_id, add_info, endurance, upgrade, amplify_option, amplify_value, gold, receive_time, delete_flag, avata_flag, unlimit_flag, seal_flag, creature_flag, postal, letter_id, extend_info, ipg_db_id, ipg_transaction_id, ipg_nexon_id, auction_id, random_option, seperate_upgrade, type, item_guid FROM postal_del");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(PostalDel));
        if (row[0]) strncpy(records[count].sdate, row[0], sizeof(records[count].sdate) - 1);
        records[count].postal_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].occ_time, row[2], sizeof(records[count].occ_time) - 1);
        records[count].send_charac_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].send_charac_name, row[4], sizeof(records[count].send_charac_name) - 1);
        records[count].receive_charac_no = row[5] ? atoi(row[5]) : 0;
        records[count].item_id = row[6] ? atoi(row[6]) : 0;
        records[count].add_info = row[7] ? atoi(row[7]) : 0;
        records[count].endurance = row[8] ? atoi(row[8]) : 0;
        records[count].upgrade = row[9] ? atoi(row[9]) : 0;
        records[count].amplify_option = row[10] ? atoi(row[10]) : 0;
        records[count].amplify_value = row[11] ? atoi(row[11]) : 0;
        records[count].gold = row[12] ? atoi(row[12]) : 0;
        if (row[13]) strncpy(records[count].receive_time, row[13], sizeof(records[count].receive_time) - 1);
        records[count].delete_flag = row[14] ? atoi(row[14]) : 0;
        records[count].avata_flag = row[15] ? atoi(row[15]) : 0;
        records[count].unlimit_flag = row[16] ? atoi(row[16]) : 0;
        records[count].seal_flag = row[17] ? atoi(row[17]) : 0;
        records[count].creature_flag = row[18] ? atoi(row[18]) : 0;
        records[count].postal = row[19] ? atoi(row[19]) : 0;
        records[count].letter_id = row[20] ? atoi(row[20]) : 0;
        records[count].extend_info = row[21] ? atoi(row[21]) : 0;
        records[count].ipg_db_id = row[22] ? atoi(row[22]) : 0;
        records[count].ipg_transaction_id = row[23] ? atoi(row[23]) : 0;
        if (row[24]) strncpy(records[count].ipg_nexon_id, row[24], sizeof(records[count].ipg_nexon_id) - 1);
        records[count].auction_id = row[25] ? atoll(row[25]) : 0;
        if (row[26]) memcpy(records[count].random_option, row[26], sizeof(records[count].random_option));
        records[count].seperate_upgrade = row[27] ? atoi(row[27]) : 0;
        records[count].type = row[28] ? atoi(row[28]) : 0;
        if (row[29]) memcpy(records[count].item_guid, row[29], sizeof(records[count].item_guid));
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
