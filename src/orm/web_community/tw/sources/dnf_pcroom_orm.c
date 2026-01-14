#include "dnf_pcroom_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfPcroom_Add(DBConnectionManager* manager, const DnfPcroom* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_pcroom (district, firm_name, telephone, address, leader, start_ip, end_ip) "
        "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s')",
        record->district, record->firm_name, record->telephone, record->address, record->leader, record->start_ip, record->end_ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfPcroom_Get(DBConnectionManager* manager, int ip_no, DnfPcroom* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT ip_no, district, firm_name, telephone, address, leader, start_ip, end_ip FROM dnf_pcroom WHERE ip_no = %d",
        ip_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfPcroom));
    record->ip_no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->district, row[1], sizeof(record->district) - 1);
    if (row[2]) strncpy(record->firm_name, row[2], sizeof(record->firm_name) - 1);
    if (row[3]) strncpy(record->telephone, row[3], sizeof(record->telephone) - 1);
    if (row[4]) strncpy(record->address, row[4], sizeof(record->address) - 1);
    if (row[5]) strncpy(record->leader, row[5], sizeof(record->leader) - 1);
    if (row[6]) strncpy(record->start_ip, row[6], sizeof(record->start_ip) - 1);
    if (row[7]) strncpy(record->end_ip, row[7], sizeof(record->end_ip) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfPcroom_GetAll(DBConnectionManager* manager, DnfPcroom* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT ip_no, district, firm_name, telephone, address, leader, start_ip, end_ip FROM dnf_pcroom");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfPcroom));
        records[count].ip_no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].district, row[1], sizeof(records[count].district) - 1);
        if (row[2]) strncpy(records[count].firm_name, row[2], sizeof(records[count].firm_name) - 1);
        if (row[3]) strncpy(records[count].telephone, row[3], sizeof(records[count].telephone) - 1);
        if (row[4]) strncpy(records[count].address, row[4], sizeof(records[count].address) - 1);
        if (row[5]) strncpy(records[count].leader, row[5], sizeof(records[count].leader) - 1);
        if (row[6]) strncpy(records[count].start_ip, row[6], sizeof(records[count].start_ip) - 1);
        if (row[7]) strncpy(records[count].end_ip, row[7], sizeof(records[count].end_ip) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
