#include "guild_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int GuildStat_Add(DBConnectionManager* manager, const GuildStat* stat) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !stat) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_stat (occ_date, lev, server_id, create_no, acc_create_no, "
        "member_no, acc_member_no, avg_lev, avg_master_lev, expire_no, new_account_no, "
        "new_member_no, acc_account_no) "
        "VALUES ('%s', %u, %u, %d, %d, %d, %d, %f, %f, %d, %d, %d, %d)",
        stat->occ_date, stat->lev, stat->server_id, stat->create_no, stat->acc_create_no,
        stat->member_no, stat->acc_member_no, stat->avg_lev, stat->avg_master_lev,
        stat->expire_no, stat->new_account_no, stat->new_member_no, stat->acc_account_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildStat_Get(DBConnectionManager* manager, const char* occ_date, unsigned char lev,
                   unsigned char server_id, GuildStat* stat) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !occ_date || !stat) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, lev, server_id, create_no, acc_create_no, member_no, "
        "acc_member_no, avg_lev, avg_master_lev, expire_no, new_account_no, "
        "new_member_no, acc_account_no FROM guild_stat "
        "WHERE occ_date = '%s' AND lev = %u AND server_id = %u",
        occ_date, lev, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(stat, 0, sizeof(GuildStat));
    if (row[0]) strncpy(stat->occ_date, row[0], sizeof(stat->occ_date) - 1);
    stat->lev = row[1] ? (unsigned char)atoi(row[1]) : 0;
    stat->server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
    stat->create_no = row[3] ? atoi(row[3]) : 0;
    stat->acc_create_no = row[4] ? atoi(row[4]) : 0;
    stat->member_no = row[5] ? atoi(row[5]) : 0;
    stat->acc_member_no = row[6] ? atoi(row[6]) : 0;
    stat->avg_lev = row[7] ? (float)atof(row[7]) : 0.0f;
    stat->avg_master_lev = row[8] ? (float)atof(row[8]) : 0.0f;
    stat->expire_no = row[9] ? atoi(row[9]) : 0;
    stat->new_account_no = row[10] ? atoi(row[10]) : 0;
    stat->new_member_no = row[11] ? atoi(row[11]) : 0;
    stat->acc_account_no = row[12] ? atoi(row[12]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildStat_Update(DBConnectionManager* manager, const GuildStat* stat) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !stat) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_stat SET create_no = %d, acc_create_no = %d, member_no = %d, "
        "acc_member_no = %d, avg_lev = %f, avg_master_lev = %f, expire_no = %d, "
        "new_account_no = %d, new_member_no = %d, acc_account_no = %d "
        "WHERE occ_date = '%s' AND lev = %u AND server_id = %u",
        stat->create_no, stat->acc_create_no, stat->member_no, stat->acc_member_no,
        stat->avg_lev, stat->avg_master_lev, stat->expire_no, stat->new_account_no,
        stat->new_member_no, stat->acc_account_no, stat->occ_date, stat->lev, stat->server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildStat_Delete(DBConnectionManager* manager, const char* occ_date, unsigned char lev,
                      unsigned char server_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_date) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_stat WHERE occ_date = '%s' AND lev = %u AND server_id = %u",
        occ_date, lev, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int GuildStat_Exists(DBConnectionManager* manager, const char* occ_date, unsigned char lev,
                      unsigned char server_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager || !occ_date) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_stat WHERE occ_date = '%s' AND lev = %u AND server_id = %u",
        occ_date, lev, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int GuildStat_GetByDate(DBConnectionManager* manager, const char* occ_date,
                        GuildStat* stats, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !occ_date || !stats || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, lev, server_id, create_no, acc_create_no, member_no, "
        "acc_member_no, avg_lev, avg_master_lev, expire_no, new_account_no, "
        "new_member_no, acc_account_no FROM guild_stat "
        "WHERE occ_date = '%s' ORDER BY lev, server_id",
        occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&stats[count], 0, sizeof(GuildStat));
        if (row[0]) strncpy(stats[count].occ_date, row[0], sizeof(stats[count].occ_date) - 1);
        stats[count].lev = row[1] ? (unsigned char)atoi(row[1]) : 0;
        stats[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        stats[count].create_no = row[3] ? atoi(row[3]) : 0;
        stats[count].acc_create_no = row[4] ? atoi(row[4]) : 0;
        stats[count].member_no = row[5] ? atoi(row[5]) : 0;
        stats[count].acc_member_no = row[6] ? atoi(row[6]) : 0;
        stats[count].avg_lev = row[7] ? (float)atof(row[7]) : 0.0f;
        stats[count].avg_master_lev = row[8] ? (float)atof(row[8]) : 0.0f;
        stats[count].expire_no = row[9] ? atoi(row[9]) : 0;
        stats[count].new_account_no = row[10] ? atoi(row[10]) : 0;
        stats[count].new_member_no = row[11] ? atoi(row[11]) : 0;
        stats[count].acc_account_no = row[12] ? atoi(row[12]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int GuildStat_GetByDateAndLevel(DBConnectionManager* manager, const char* occ_date, unsigned char lev,
                                  GuildStat* stats, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !occ_date || !stats || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, lev, server_id, create_no, acc_create_no, member_no, "
        "acc_member_no, avg_lev, avg_master_lev, expire_no, new_account_no, "
        "new_member_no, acc_account_no FROM guild_stat "
        "WHERE occ_date = '%s' AND lev = %u ORDER BY server_id",
        occ_date, lev);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&stats[count], 0, sizeof(GuildStat));
        if (row[0]) strncpy(stats[count].occ_date, row[0], sizeof(stats[count].occ_date) - 1);
        stats[count].lev = row[1] ? (unsigned char)atoi(row[1]) : 0;
        stats[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        stats[count].create_no = row[3] ? atoi(row[3]) : 0;
        stats[count].acc_create_no = row[4] ? atoi(row[4]) : 0;
        stats[count].member_no = row[5] ? atoi(row[5]) : 0;
        stats[count].acc_member_no = row[6] ? atoi(row[6]) : 0;
        stats[count].avg_lev = row[7] ? (float)atof(row[7]) : 0.0f;
        stats[count].avg_master_lev = row[8] ? (float)atof(row[8]) : 0.0f;
        stats[count].expire_no = row[9] ? atoi(row[9]) : 0;
        stats[count].new_account_no = row[10] ? atoi(row[10]) : 0;
        stats[count].new_member_no = row[11] ? atoi(row[11]) : 0;
        stats[count].acc_account_no = row[12] ? atoi(row[12]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int GuildStat_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                           GuildStat* stats, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !stats || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, lev, server_id, create_no, acc_create_no, member_no, "
        "acc_member_no, avg_lev, avg_master_lev, expire_no, new_account_no, "
        "new_member_no, acc_account_no FROM guild_stat "
        "WHERE server_id = %u ORDER BY occ_date DESC, lev",
        server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&stats[count], 0, sizeof(GuildStat));
        if (row[0]) strncpy(stats[count].occ_date, row[0], sizeof(stats[count].occ_date) - 1);
        stats[count].lev = row[1] ? (unsigned char)atoi(row[1]) : 0;
        stats[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        stats[count].create_no = row[3] ? atoi(row[3]) : 0;
        stats[count].acc_create_no = row[4] ? atoi(row[4]) : 0;
        stats[count].member_no = row[5] ? atoi(row[5]) : 0;
        stats[count].acc_member_no = row[6] ? atoi(row[6]) : 0;
        stats[count].avg_lev = row[7] ? (float)atof(row[7]) : 0.0f;
        stats[count].avg_master_lev = row[8] ? (float)atof(row[8]) : 0.0f;
        stats[count].expire_no = row[9] ? atoi(row[9]) : 0;
        stats[count].new_account_no = row[10] ? atoi(row[10]) : 0;
        stats[count].new_member_no = row[11] ? atoi(row[11]) : 0;
        stats[count].acc_account_no = row[12] ? atoi(row[12]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int GuildStat_DeleteByDate(DBConnectionManager* manager, const char* occ_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_date) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_stat WHERE occ_date = '%s'", occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void GuildStat_PrintInfo(const GuildStat* stat) {
    if (!stat) {
        printf("GuildStat: NULL\n");
        return;
    }

    printf("=== Guild Statistics ===\n");
    printf("Date:          %s\n", stat->occ_date);
    printf("Level:         %u\n", stat->lev);
    printf("Server ID:     %u\n", stat->server_id);
    printf("Create No:     %d\n", stat->create_no);
    printf("Acc Create:    %d\n", stat->acc_create_no);
    printf("Member No:     %d\n", stat->member_no);
    printf("Acc Member:    %d\n", stat->acc_member_no);
    printf("Avg Level:     %.2f\n", stat->avg_lev);
    printf("Avg Master Lv: %.2f\n", stat->avg_master_lev);
    printf("Expire No:     %d\n", stat->expire_no);
    printf("New Account:   %d\n", stat->new_account_no);
    printf("New Member:    %d\n", stat->new_member_no);
    printf("Acc Account:   %d\n", stat->acc_account_no);
    printf("========================\n");
}
