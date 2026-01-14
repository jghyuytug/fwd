#include "opencast_subscribe_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int OpencastSubscribe_Add(DBConnectionManager* manager, const OpencastSubscribe* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO opencast_subscribe (m_id, cc_no, occ_date) "
        "VALUES (%d, %d, '%s')",
        record->m_id, record->cc_no, record->occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int OpencastSubscribe_Get(DBConnectionManager* manager, int no, OpencastSubscribe* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, cc_no, occ_date FROM opencast_subscribe WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(OpencastSubscribe));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->cc_no = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->occ_date, row[3], sizeof(record->occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int OpencastSubscribe_GetAll(DBConnectionManager* manager, OpencastSubscribe* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, cc_no, occ_date FROM opencast_subscribe");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(OpencastSubscribe));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].cc_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].occ_date, row[3], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
