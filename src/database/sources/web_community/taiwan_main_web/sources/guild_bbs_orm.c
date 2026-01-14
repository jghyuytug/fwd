#include "guild_bbs_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildBbs_Add(DBConnectionManager* manager, const GuildBbs* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_bbs (bd_id, empyn, mgno, open, main, reg_date, mod_date, hits, body_type, m_id, reg_id, subject) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, '%s', '%s')",
        record->bd_id, record->empyn, record->mgno, record->open, record->main, record->reg_date, record->mod_date, record->hits, record->body_type, record->m_id, record->reg_id, record->subject);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildBbs_Get(DBConnectionManager* manager, int gno, GuildBbs* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, bd_id, empyn, mgno, open, main, reg_date, mod_date, hits, body_type, m_id, reg_id, subject FROM guild_bbs WHERE gno = %d",
        gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildBbs));
    record->gno = row[0] ? atoi(row[0]) : 0;
    record->bd_id = row[1] ? atoi(row[1]) : 0;
    record->empyn = row[2] ? atoi(row[2]) : 0;
    record->mgno = row[3] ? atoi(row[3]) : 0;
    record->open = row[4] ? atoi(row[4]) : 0;
    record->main = row[5] ? atoi(row[5]) : 0;
    record->reg_date = row[6] ? atoi(row[6]) : 0;
    record->mod_date = row[7] ? atoi(row[7]) : 0;
    record->hits = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->body_type, row[9], sizeof(record->body_type) - 1);
    record->m_id = row[10] ? atoi(row[10]) : 0;
    if (row[11]) strncpy(record->reg_id, row[11], sizeof(record->reg_id) - 1);
    if (row[12]) strncpy(record->subject, row[12], sizeof(record->subject) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GuildBbs_GetAll(DBConnectionManager* manager, GuildBbs* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, bd_id, empyn, mgno, open, main, reg_date, mod_date, hits, body_type, m_id, reg_id, subject FROM guild_bbs");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildBbs));
        records[count].gno = row[0] ? atoi(row[0]) : 0;
        records[count].bd_id = row[1] ? atoi(row[1]) : 0;
        records[count].empyn = row[2] ? atoi(row[2]) : 0;
        records[count].mgno = row[3] ? atoi(row[3]) : 0;
        records[count].open = row[4] ? atoi(row[4]) : 0;
        records[count].main = row[5] ? atoi(row[5]) : 0;
        records[count].reg_date = row[6] ? atoi(row[6]) : 0;
        records[count].mod_date = row[7] ? atoi(row[7]) : 0;
        records[count].hits = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].body_type, row[9], sizeof(records[count].body_type) - 1);
        records[count].m_id = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].reg_id, row[11], sizeof(records[count].reg_id) - 1);
        if (row[12]) strncpy(records[count].subject, row[12], sizeof(records[count].subject) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
