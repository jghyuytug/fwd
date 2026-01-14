#include "guild_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildStat_Add(DBConnectionManager* manager, const GuildStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_stat (occ_date, lev, server_id, create_no, acc_create_no, member_no, acc_member_no, avg_lev, avg_master_lev, expire_no, new_account_no, new_member_no, acc_account_no) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %f, %f, %d, %d, %d, %d)",
        record->occ_date, record->lev, record->server_id, record->create_no, record->acc_create_no, record->member_no, record->acc_member_no, record->avg_lev, record->avg_master_lev, record->expire_no, record->new_account_no, record->new_member_no, record->acc_account_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildStat_Get(DBConnectionManager* manager, const char* occ_date, signed char lev, signed char server_id, GuildStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, lev, server_id, create_no, acc_create_no, member_no, acc_member_no, avg_lev, avg_master_lev, expire_no, new_account_no, new_member_no, acc_account_no FROM guild_stat WHERE occ_date = '%s' AND lev = %d AND server_id = %d",
        occ_date, lev, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildStat));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->lev = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? atoi(row[2]) : 0;
    record->create_no = row[3] ? atoi(row[3]) : 0;
    record->acc_create_no = row[4] ? atoi(row[4]) : 0;
    record->member_no = row[5] ? atoi(row[5]) : 0;
    record->acc_member_no = row[6] ? atoi(row[6]) : 0;
    record->avg_lev = row[7] ? atof(row[7]) : 0;
    record->avg_master_lev = row[8] ? atof(row[8]) : 0;
    record->expire_no = row[9] ? atoi(row[9]) : 0;
    record->new_account_no = row[10] ? atoi(row[10]) : 0;
    record->new_member_no = row[11] ? atoi(row[11]) : 0;
    record->acc_account_no = row[12] ? atoi(row[12]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildStat_GetAll(DBConnectionManager* manager, GuildStat* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, lev, server_id, create_no, acc_create_no, member_no, acc_member_no, avg_lev, avg_master_lev, expire_no, new_account_no, new_member_no, acc_account_no FROM guild_stat");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildStat));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].lev = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        records[count].create_no = row[3] ? atoi(row[3]) : 0;
        records[count].acc_create_no = row[4] ? atoi(row[4]) : 0;
        records[count].member_no = row[5] ? atoi(row[5]) : 0;
        records[count].acc_member_no = row[6] ? atoi(row[6]) : 0;
        records[count].avg_lev = row[7] ? atof(row[7]) : 0;
        records[count].avg_master_lev = row[8] ? atof(row[8]) : 0;
        records[count].expire_no = row[9] ? atoi(row[9]) : 0;
        records[count].new_account_no = row[10] ? atoi(row[10]) : 0;
        records[count].new_member_no = row[11] ? atoi(row[11]) : 0;
        records[count].acc_account_no = row[12] ? atoi(row[12]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
