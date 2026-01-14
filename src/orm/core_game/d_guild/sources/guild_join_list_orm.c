#include "guild_join_list_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildJoinList_Add(DBConnectionManager* manager, const GuildJoinList* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_join_list (guild_id, charac_no, server_group, m_id, born_year, memo, occ_time) "
        "VALUES (%d, %d, %d, %d, '%s', '%s', '%s')",
        record->guild_id, record->charac_no, record->server_group, record->m_id, record->born_year, record->memo, record->occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildJoinList_Get(DBConnectionManager* manager, int guild_id, int charac_no, GuildJoinList* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, charac_no, server_group, m_id, born_year, memo, occ_time FROM guild_join_list WHERE guild_id = %d AND charac_no = %d",
        guild_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildJoinList));
    record->guild_id = row[0] ? atoi(row[0]) : 0;
    record->charac_no = row[1] ? atoi(row[1]) : 0;
    record->server_group = row[2] ? atoi(row[2]) : 0;
    record->m_id = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->born_year, row[4], sizeof(record->born_year) - 1);
    if (row[5]) strncpy(record->memo, row[5], sizeof(record->memo) - 1);
    if (row[6]) strncpy(record->occ_time, row[6], sizeof(record->occ_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GuildJoinList_GetAll(DBConnectionManager* manager, GuildJoinList* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, charac_no, server_group, m_id, born_year, memo, occ_time FROM guild_join_list");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildJoinList));
        records[count].guild_id = row[0] ? atoi(row[0]) : 0;
        records[count].charac_no = row[1] ? atoi(row[1]) : 0;
        records[count].server_group = row[2] ? atoi(row[2]) : 0;
        records[count].m_id = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].born_year, row[4], sizeof(records[count].born_year) - 1);
        if (row[5]) strncpy(records[count].memo, row[5], sizeof(records[count].memo) - 1);
        if (row[6]) strncpy(records[count].occ_time, row[6], sizeof(records[count].occ_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
