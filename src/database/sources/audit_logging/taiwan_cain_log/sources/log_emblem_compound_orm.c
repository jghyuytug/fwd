#include "log_emblem_compound_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogEmblemCompound_Add(DBConnectionManager* manager, const LogEmblemCompound* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_emblem_compound (occ_date, channel_no, grade0, grade1, grade2, grade3, grade4, grade5, grade6) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d)",
        record->occ_date, record->channel_no, record->grade0, record->grade1, record->grade2, record->grade3, record->grade4, record->grade5, record->grade6);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogEmblemCompound_Get(DBConnectionManager* manager, const char* occ_date, signed char channel_no, LogEmblemCompound* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, channel_no, grade0, grade1, grade2, grade3, grade4, grade5, grade6 FROM log_emblem_compound WHERE occ_date = '%s' AND channel_no = %d",
        occ_date, channel_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogEmblemCompound));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->channel_no = row[1] ? atoi(row[1]) : 0;
    record->grade0 = row[2] ? atoi(row[2]) : 0;
    record->grade1 = row[3] ? atoi(row[3]) : 0;
    record->grade2 = row[4] ? atoi(row[4]) : 0;
    record->grade3 = row[5] ? atoi(row[5]) : 0;
    record->grade4 = row[6] ? atoi(row[6]) : 0;
    record->grade5 = row[7] ? atoi(row[7]) : 0;
    record->grade6 = row[8] ? atoi(row[8]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogEmblemCompound_GetAll(DBConnectionManager* manager, LogEmblemCompound* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, channel_no, grade0, grade1, grade2, grade3, grade4, grade5, grade6 FROM log_emblem_compound");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogEmblemCompound));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].channel_no = row[1] ? atoi(row[1]) : 0;
        records[count].grade0 = row[2] ? atoi(row[2]) : 0;
        records[count].grade1 = row[3] ? atoi(row[3]) : 0;
        records[count].grade2 = row[4] ? atoi(row[4]) : 0;
        records[count].grade3 = row[5] ? atoi(row[5]) : 0;
        records[count].grade4 = row[6] ? atoi(row[6]) : 0;
        records[count].grade5 = row[7] ? atoi(row[7]) : 0;
        records[count].grade6 = row[8] ? atoi(row[8]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
