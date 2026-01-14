#include "check_manager_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CheckManager_Add(DBConnectionManager* manager, const CheckManager* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO check_manager (check_start_time, check_end_time, type, checkup_content, dnf_title_1, dnf_link_1, dnf_source_1, dnf_title_2, dnf_link_2, dnf_source_2, dnf_title_3, dnf_link_3, dnf_source_3, dnf_title_4, dnf_link_4, dnf_source_4, dnf_title_5, dnf_link_5, dnf_source_5, dnf_title_6, dnf_link_6, dnf_source_6) "
        "VALUES ('%s', '%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
        record->check_start_time, record->check_end_time, record->type, record->checkup_content, record->dnf_title_1, record->dnf_link_1, record->dnf_source_1, record->dnf_title_2, record->dnf_link_2, record->dnf_source_2, record->dnf_title_3, record->dnf_link_3, record->dnf_source_3, record->dnf_title_4, record->dnf_link_4, record->dnf_source_4, record->dnf_title_5, record->dnf_link_5, record->dnf_source_5, record->dnf_title_6, record->dnf_link_6, record->dnf_source_6);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CheckManager_GetAll(DBConnectionManager* manager, CheckManager* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[22];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT check_start_time, check_end_time, type, checkup_content, dnf_title_1, dnf_link_1, dnf_source_1, dnf_title_2, dnf_link_2, dnf_source_2, dnf_title_3, dnf_link_3, dnf_source_3, dnf_title_4, dnf_link_4, dnf_source_4, dnf_title_5, dnf_link_5, dnf_source_5, dnf_title_6, dnf_link_6, dnf_source_6 FROM check_manager");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CheckManager));
        if (row[0]) strncpy(records[count].check_start_time, row[0], sizeof(records[count].check_start_time) - 1);
        if (row[1]) strncpy(records[count].check_end_time, row[1], sizeof(records[count].check_end_time) - 1);
        records[count].type = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].checkup_content, row[3], sizeof(records[count].checkup_content) - 1);
        if (row[4]) strncpy(records[count].dnf_title_1, row[4], sizeof(records[count].dnf_title_1) - 1);
        if (row[5]) strncpy(records[count].dnf_link_1, row[5], sizeof(records[count].dnf_link_1) - 1);
        if (row[6]) strncpy(records[count].dnf_source_1, row[6], sizeof(records[count].dnf_source_1) - 1);
        if (row[7]) strncpy(records[count].dnf_title_2, row[7], sizeof(records[count].dnf_title_2) - 1);
        if (row[8]) strncpy(records[count].dnf_link_2, row[8], sizeof(records[count].dnf_link_2) - 1);
        if (row[9]) strncpy(records[count].dnf_source_2, row[9], sizeof(records[count].dnf_source_2) - 1);
        if (row[10]) strncpy(records[count].dnf_title_3, row[10], sizeof(records[count].dnf_title_3) - 1);
        if (row[11]) strncpy(records[count].dnf_link_3, row[11], sizeof(records[count].dnf_link_3) - 1);
        if (row[12]) strncpy(records[count].dnf_source_3, row[12], sizeof(records[count].dnf_source_3) - 1);
        if (row[13]) strncpy(records[count].dnf_title_4, row[13], sizeof(records[count].dnf_title_4) - 1);
        if (row[14]) strncpy(records[count].dnf_link_4, row[14], sizeof(records[count].dnf_link_4) - 1);
        if (row[15]) strncpy(records[count].dnf_source_4, row[15], sizeof(records[count].dnf_source_4) - 1);
        if (row[16]) strncpy(records[count].dnf_title_5, row[16], sizeof(records[count].dnf_title_5) - 1);
        if (row[17]) strncpy(records[count].dnf_link_5, row[17], sizeof(records[count].dnf_link_5) - 1);
        if (row[18]) strncpy(records[count].dnf_source_5, row[18], sizeof(records[count].dnf_source_5) - 1);
        if (row[19]) strncpy(records[count].dnf_title_6, row[19], sizeof(records[count].dnf_title_6) - 1);
        if (row[20]) strncpy(records[count].dnf_link_6, row[20], sizeof(records[count].dnf_link_6) - 1);
        if (row[21]) strncpy(records[count].dnf_source_6, row[21], sizeof(records[count].dnf_source_6) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
