#include "log_gameserver_load_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogGameserverLoadStat_Add(DBConnectionManager* manager, const LogGameserverLoadStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_gameserver_load_stat (occ_time, server_id, channel_no, dbq_cnt, logdbq_cnt, netq_cnt, packetq_cnt) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d)",
        record->occ_time, record->server_id, record->channel_no, record->dbq_cnt, record->logdbq_cnt, record->netq_cnt, record->packetq_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogGameserverLoadStat_Get(DBConnectionManager* manager, const char* occ_time, signed char server_id, int channel_no, LogGameserverLoadStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, server_id, channel_no, dbq_cnt, logdbq_cnt, netq_cnt, packetq_cnt FROM log_gameserver_load_stat WHERE occ_time = '%s' AND server_id = %d AND channel_no = %d",
        occ_time, server_id, channel_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogGameserverLoadStat));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->server_id = row[1] ? atoi(row[1]) : 0;
    record->channel_no = row[2] ? atoi(row[2]) : 0;
    record->dbq_cnt = row[3] ? atoi(row[3]) : 0;
    record->logdbq_cnt = row[4] ? atoi(row[4]) : 0;
    record->netq_cnt = row[5] ? atoi(row[5]) : 0;
    record->packetq_cnt = row[6] ? atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogGameserverLoadStat_GetAll(DBConnectionManager* manager, LogGameserverLoadStat* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, server_id, channel_no, dbq_cnt, logdbq_cnt, netq_cnt, packetq_cnt FROM log_gameserver_load_stat");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogGameserverLoadStat));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].server_id = row[1] ? atoi(row[1]) : 0;
        records[count].channel_no = row[2] ? atoi(row[2]) : 0;
        records[count].dbq_cnt = row[3] ? atoi(row[3]) : 0;
        records[count].logdbq_cnt = row[4] ? atoi(row[4]) : 0;
        records[count].netq_cnt = row[5] ? atoi(row[5]) : 0;
        records[count].packetq_cnt = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
