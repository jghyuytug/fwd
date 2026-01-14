#include "log_upgrade_pot_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogUpgradePot_Add(DBConnectionManager* manager, const LogUpgradePot* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_upgrade_pot (occ_date, id_2670203, id_2670204, id_2670205, id_2670206, id_2670241, id_2670242, id_2670243, id_2670244, id_2670245, id_2670246, id_2670247, id_2670248, id_2670249, id_2670250, id_2670251, id_2670252, id_2670253) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->occ_date, record->id_2670203, record->id_2670204, record->id_2670205, record->id_2670206, record->id_2670241, record->id_2670242, record->id_2670243, record->id_2670244, record->id_2670245, record->id_2670246, record->id_2670247, record->id_2670248, record->id_2670249, record->id_2670250, record->id_2670251, record->id_2670252, record->id_2670253);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogUpgradePot_Get(DBConnectionManager* manager, const char* occ_date, LogUpgradePot* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[18];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, id_2670203, id_2670204, id_2670205, id_2670206, id_2670241, id_2670242, id_2670243, id_2670244, id_2670245, id_2670246, id_2670247, id_2670248, id_2670249, id_2670250, id_2670251, id_2670252, id_2670253 FROM log_upgrade_pot WHERE occ_date = '%s'",
        occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogUpgradePot));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->id_2670203 = row[1] ? atoi(row[1]) : 0;
    record->id_2670204 = row[2] ? atoi(row[2]) : 0;
    record->id_2670205 = row[3] ? atoi(row[3]) : 0;
    record->id_2670206 = row[4] ? atoi(row[4]) : 0;
    record->id_2670241 = row[5] ? atoi(row[5]) : 0;
    record->id_2670242 = row[6] ? atoi(row[6]) : 0;
    record->id_2670243 = row[7] ? atoi(row[7]) : 0;
    record->id_2670244 = row[8] ? atoi(row[8]) : 0;
    record->id_2670245 = row[9] ? atoi(row[9]) : 0;
    record->id_2670246 = row[10] ? atoi(row[10]) : 0;
    record->id_2670247 = row[11] ? atoi(row[11]) : 0;
    record->id_2670248 = row[12] ? atoi(row[12]) : 0;
    record->id_2670249 = row[13] ? atoi(row[13]) : 0;
    record->id_2670250 = row[14] ? atoi(row[14]) : 0;
    record->id_2670251 = row[15] ? atoi(row[15]) : 0;
    record->id_2670252 = row[16] ? atoi(row[16]) : 0;
    record->id_2670253 = row[17] ? atoi(row[17]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogUpgradePot_GetAll(DBConnectionManager* manager, LogUpgradePot* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[18];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, id_2670203, id_2670204, id_2670205, id_2670206, id_2670241, id_2670242, id_2670243, id_2670244, id_2670245, id_2670246, id_2670247, id_2670248, id_2670249, id_2670250, id_2670251, id_2670252, id_2670253 FROM log_upgrade_pot");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogUpgradePot));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].id_2670203 = row[1] ? atoi(row[1]) : 0;
        records[count].id_2670204 = row[2] ? atoi(row[2]) : 0;
        records[count].id_2670205 = row[3] ? atoi(row[3]) : 0;
        records[count].id_2670206 = row[4] ? atoi(row[4]) : 0;
        records[count].id_2670241 = row[5] ? atoi(row[5]) : 0;
        records[count].id_2670242 = row[6] ? atoi(row[6]) : 0;
        records[count].id_2670243 = row[7] ? atoi(row[7]) : 0;
        records[count].id_2670244 = row[8] ? atoi(row[8]) : 0;
        records[count].id_2670245 = row[9] ? atoi(row[9]) : 0;
        records[count].id_2670246 = row[10] ? atoi(row[10]) : 0;
        records[count].id_2670247 = row[11] ? atoi(row[11]) : 0;
        records[count].id_2670248 = row[12] ? atoi(row[12]) : 0;
        records[count].id_2670249 = row[13] ? atoi(row[13]) : 0;
        records[count].id_2670250 = row[14] ? atoi(row[14]) : 0;
        records[count].id_2670251 = row[15] ? atoi(row[15]) : 0;
        records[count].id_2670252 = row[16] ? atoi(row[16]) : 0;
        records[count].id_2670253 = row[17] ? atoi(row[17]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
