#include "tcg_relay_config_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int TcgRelayConfig_Add(DBConnectionManager* manager, const TcgRelayConfig* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO tcg_relay_config (occ_date, event_seq, event_name, prize_count, item_nick, item_name, item_quantity, prize_date, prize_user_m_id, prize_user_server_id, prize_user_charac_no, item_id) "
        "VALUES (%d, %d, '%s', %d, '%s', '%s', %d, %d, %d, %d, %d, %d)",
        record->occ_date, record->event_seq, record->event_name, record->prize_count, record->item_nick, record->item_name, record->item_quantity, record->prize_date, record->prize_user_m_id, record->prize_user_server_id, record->prize_user_charac_no, record->item_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TcgRelayConfig_Get(DBConnectionManager* manager, int seq, TcgRelayConfig* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT seq, occ_date, event_seq, event_name, prize_count, item_nick, item_name, item_quantity, prize_date, prize_user_m_id, prize_user_server_id, prize_user_charac_no, item_id FROM tcg_relay_config WHERE seq = %d",
        seq);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(TcgRelayConfig));
    record->seq = row[0] ? atoi(row[0]) : 0;
    record->occ_date = row[1] ? atoi(row[1]) : 0;
    record->event_seq = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->event_name, row[3], sizeof(record->event_name) - 1);
    record->prize_count = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->item_nick, row[5], sizeof(record->item_nick) - 1);
    if (row[6]) strncpy(record->item_name, row[6], sizeof(record->item_name) - 1);
    record->item_quantity = row[7] ? atoi(row[7]) : 0;
    record->prize_date = row[8] ? atoi(row[8]) : 0;
    record->prize_user_m_id = row[9] ? atoi(row[9]) : 0;
    record->prize_user_server_id = row[10] ? atoi(row[10]) : 0;
    record->prize_user_charac_no = row[11] ? atoi(row[11]) : 0;
    record->item_id = row[12] ? atoi(row[12]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int TcgRelayConfig_GetAll(DBConnectionManager* manager, TcgRelayConfig* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT seq, occ_date, event_seq, event_name, prize_count, item_nick, item_name, item_quantity, prize_date, prize_user_m_id, prize_user_server_id, prize_user_charac_no, item_id FROM tcg_relay_config");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(TcgRelayConfig));
        records[count].seq = row[0] ? atoi(row[0]) : 0;
        records[count].occ_date = row[1] ? atoi(row[1]) : 0;
        records[count].event_seq = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].event_name, row[3], sizeof(records[count].event_name) - 1);
        records[count].prize_count = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].item_nick, row[5], sizeof(records[count].item_nick) - 1);
        if (row[6]) strncpy(records[count].item_name, row[6], sizeof(records[count].item_name) - 1);
        records[count].item_quantity = row[7] ? atoi(row[7]) : 0;
        records[count].prize_date = row[8] ? atoi(row[8]) : 0;
        records[count].prize_user_m_id = row[9] ? atoi(row[9]) : 0;
        records[count].prize_user_server_id = row[10] ? atoi(row[10]) : 0;
        records[count].prize_user_charac_no = row[11] ? atoi(row[11]) : 0;
        records[count].item_id = row[12] ? atoi(row[12]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
