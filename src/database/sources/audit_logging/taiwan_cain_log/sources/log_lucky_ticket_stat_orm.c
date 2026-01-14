#include "log_lucky_ticket_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogLuckyTicketStat_Add(DBConnectionManager* manager, const LogLuckyTicketStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_lucky_ticket_stat (occ_time, m_id, charac_no, type_flag, ticket_flag, item_idx, upgrade, succ_flag) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d)",
        record->occ_time, record->m_id, record->charac_no, record->type_flag, record->ticket_flag, record->item_idx, record->upgrade, record->succ_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogLuckyTicketStat_Get(DBConnectionManager* manager, int idx, LogLuckyTicketStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT idx, occ_time, m_id, charac_no, type_flag, ticket_flag, item_idx, upgrade, succ_flag FROM log_lucky_ticket_stat WHERE idx = %d",
        idx);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogLuckyTicketStat));
    record->idx = row[0] ? atoi(row[0]) : 0;
    record->occ_time = row[1] ? atoi(row[1]) : 0;
    record->m_id = row[2] ? atoi(row[2]) : 0;
    record->charac_no = row[3] ? atoi(row[3]) : 0;
    record->type_flag = row[4] ? atoi(row[4]) : 0;
    record->ticket_flag = row[5] ? atoi(row[5]) : 0;
    record->item_idx = row[6] ? atoi(row[6]) : 0;
    record->upgrade = row[7] ? atoi(row[7]) : 0;
    record->succ_flag = row[8] ? atoi(row[8]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogLuckyTicketStat_GetAll(DBConnectionManager* manager, LogLuckyTicketStat* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT idx, occ_time, m_id, charac_no, type_flag, ticket_flag, item_idx, upgrade, succ_flag FROM log_lucky_ticket_stat");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogLuckyTicketStat));
        records[count].idx = row[0] ? atoi(row[0]) : 0;
        records[count].occ_time = row[1] ? atoi(row[1]) : 0;
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        records[count].charac_no = row[3] ? atoi(row[3]) : 0;
        records[count].type_flag = row[4] ? atoi(row[4]) : 0;
        records[count].ticket_flag = row[5] ? atoi(row[5]) : 0;
        records[count].item_idx = row[6] ? atoi(row[6]) : 0;
        records[count].upgrade = row[7] ? atoi(row[7]) : 0;
        records[count].succ_flag = row[8] ? atoi(row[8]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
