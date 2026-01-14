#include "guild_grade_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildGradeLog_Add(DBConnectionManager* manager, const GuildGradeLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_grade_log (guild_id, m_id, server_id, charac_no, charac_name, occ_time, grade_prev, grade_next, reason, admin_no, admin_name) "
        "VALUES (%d, %d, %d, %d, '%s', '%s', %d, %d, '%s', %d, '%s')",
        record->guild_id, record->m_id, record->server_id, record->charac_no, record->charac_name, record->occ_time, record->grade_prev, record->grade_next, record->reason, record->admin_no, record->admin_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildGradeLog_Get(DBConnectionManager* manager, int id, GuildGradeLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, guild_id, m_id, server_id, charac_no, charac_name, occ_time, grade_prev, grade_next, reason, admin_no, admin_name FROM guild_grade_log WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildGradeLog));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->guild_id = row[1] ? atoi(row[1]) : 0;
    record->m_id = row[2] ? atoi(row[2]) : 0;
    record->server_id = row[3] ? atoi(row[3]) : 0;
    record->charac_no = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->charac_name, row[5], sizeof(record->charac_name) - 1);
    if (row[6]) strncpy(record->occ_time, row[6], sizeof(record->occ_time) - 1);
    record->grade_prev = row[7] ? atoi(row[7]) : 0;
    record->grade_next = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->reason, row[9], sizeof(record->reason) - 1);
    record->admin_no = row[10] ? atoi(row[10]) : 0;
    if (row[11]) strncpy(record->admin_name, row[11], sizeof(record->admin_name) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GuildGradeLog_GetAll(DBConnectionManager* manager, GuildGradeLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, guild_id, m_id, server_id, charac_no, charac_name, occ_time, grade_prev, grade_next, reason, admin_no, admin_name FROM guild_grade_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildGradeLog));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].guild_id = row[1] ? atoi(row[1]) : 0;
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        records[count].server_id = row[3] ? atoi(row[3]) : 0;
        records[count].charac_no = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].charac_name, row[5], sizeof(records[count].charac_name) - 1);
        if (row[6]) strncpy(records[count].occ_time, row[6], sizeof(records[count].occ_time) - 1);
        records[count].grade_prev = row[7] ? atoi(row[7]) : 0;
        records[count].grade_next = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].reason, row[9], sizeof(records[count].reason) - 1);
        records[count].admin_no = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].admin_name, row[11], sizeof(records[count].admin_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
