#include "dnf_schl_cd_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfSchlCd_Add(DBConnectionManager* manager, const DnfSchlCd* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_schl_cd (schl_cd, schl_name, file_name, param, description) "
        "VALUES ('%s', '%s', '%s', '%s', '%s')",
        record->schl_cd, record->schl_name, record->file_name, record->param, record->description);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfSchlCd_Get(DBConnectionManager* manager, const char* schl_cd, DnfSchlCd* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT schl_cd, schl_name, file_name, param, description FROM dnf_schl_cd WHERE schl_cd = '%s'",
        schl_cd);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfSchlCd));
    if (row[0]) strncpy(record->schl_cd, row[0], sizeof(record->schl_cd) - 1);
    if (row[1]) strncpy(record->schl_name, row[1], sizeof(record->schl_name) - 1);
    if (row[2]) strncpy(record->file_name, row[2], sizeof(record->file_name) - 1);
    if (row[3]) strncpy(record->param, row[3], sizeof(record->param) - 1);
    if (row[4]) strncpy(record->description, row[4], sizeof(record->description) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfSchlCd_GetAll(DBConnectionManager* manager, DnfSchlCd* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT schl_cd, schl_name, file_name, param, description FROM dnf_schl_cd");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfSchlCd));
        if (row[0]) strncpy(records[count].schl_cd, row[0], sizeof(records[count].schl_cd) - 1);
        if (row[1]) strncpy(records[count].schl_name, row[1], sizeof(records[count].schl_name) - 1);
        if (row[2]) strncpy(records[count].file_name, row[2], sizeof(records[count].file_name) - 1);
        if (row[3]) strncpy(records[count].param, row[3], sizeof(records[count].param) - 1);
        if (row[4]) strncpy(records[count].description, row[4], sizeof(records[count].description) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
