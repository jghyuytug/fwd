#include "mage_grow_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MageGrowLog_Add(DBConnectionManager* manager, const MageGrowLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO mage_grow_log (grow_type1, grow_type2, grow_type3, grow_type4, grow_type5) "
        "VALUES (%d, %d, %d, %d, %d)",
        record->grow_type1, record->grow_type2, record->grow_type3, record->grow_type4, record->grow_type5);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MageGrowLog_GetAll(DBConnectionManager* manager, MageGrowLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT grow_type1, grow_type2, grow_type3, grow_type4, grow_type5 FROM mage_grow_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MageGrowLog));
        records[count].grow_type1 = row[0] ? atoi(row[0]) : 0;
        records[count].grow_type2 = row[1] ? atoi(row[1]) : 0;
        records[count].grow_type3 = row[2] ? atoi(row[2]) : 0;
        records[count].grow_type4 = row[3] ? atoi(row[3]) : 0;
        records[count].grow_type5 = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
