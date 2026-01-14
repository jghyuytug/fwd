#include "guild_member_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GuildMember_Add(DBConnectionManager* manager, const GuildMember* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_member (guild_id, m_id, server_id, charac_no, charac_name, memo, grade, job, grow_type, lev, age, born_year, sex, apply_time, member_time, member_flag, bbs_cnt, last_visit_time, secede_type, secede_time, member_point, member_point_prev, last_play_time, nick_name) "
        "VALUES (%d, %d, %d, %d, '%s', '%s', %d, %d, %d, %d, %d, '%s', '%s', '%s', '%s', %d, %d, '%s', %d, '%s', %d, %d, '%s', '%s')",
        record->guild_id, record->m_id, record->server_id, record->charac_no, record->charac_name, record->memo, record->grade, record->job, record->grow_type, record->lev, record->age, record->born_year, record->sex, record->apply_time, record->member_time, record->member_flag, record->bbs_cnt, record->last_visit_time, record->secede_type, record->secede_time, record->member_point, record->member_point_prev, record->last_play_time, record->nick_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildMember_Get(DBConnectionManager* manager, int guild_id, int charac_no, GuildMember* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[24];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, m_id, server_id, charac_no, charac_name, memo, grade, job, grow_type, lev, age, born_year, sex, apply_time, member_time, member_flag, bbs_cnt, last_visit_time, secede_type, secede_time, member_point, member_point_prev, last_play_time, nick_name FROM guild_member WHERE guild_id = %d AND charac_no = %d",
        guild_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GuildMember));
    record->guild_id = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? atoi(row[2]) : 0;
    record->charac_no = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->charac_name, row[4], sizeof(record->charac_name) - 1);
    if (row[5]) strncpy(record->memo, row[5], sizeof(record->memo) - 1);
    record->grade = row[6] ? atoi(row[6]) : 0;
    record->job = row[7] ? atoi(row[7]) : 0;
    record->grow_type = row[8] ? atoi(row[8]) : 0;
    record->lev = row[9] ? atoi(row[9]) : 0;
    record->age = row[10] ? atoi(row[10]) : 0;
    if (row[11]) strncpy(record->born_year, row[11], sizeof(record->born_year) - 1);
    if (row[12]) strncpy(record->sex, row[12], sizeof(record->sex) - 1);
    if (row[13]) strncpy(record->apply_time, row[13], sizeof(record->apply_time) - 1);
    if (row[14]) strncpy(record->member_time, row[14], sizeof(record->member_time) - 1);
    record->member_flag = row[15] ? atoi(row[15]) : 0;
    record->bbs_cnt = row[16] ? atoi(row[16]) : 0;
    if (row[17]) strncpy(record->last_visit_time, row[17], sizeof(record->last_visit_time) - 1);
    record->secede_type = row[18] ? atoi(row[18]) : 0;
    if (row[19]) strncpy(record->secede_time, row[19], sizeof(record->secede_time) - 1);
    record->member_point = row[20] ? atoi(row[20]) : 0;
    record->member_point_prev = row[21] ? atoi(row[21]) : 0;
    if (row[22]) strncpy(record->last_play_time, row[22], sizeof(record->last_play_time) - 1);
    if (row[23]) strncpy(record->nick_name, row[23], sizeof(record->nick_name) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GuildMember_GetAll(DBConnectionManager* manager, GuildMember* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[24];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT guild_id, m_id, server_id, charac_no, charac_name, memo, grade, job, grow_type, lev, age, born_year, sex, apply_time, member_time, member_flag, bbs_cnt, last_visit_time, secede_type, secede_time, member_point, member_point_prev, last_play_time, nick_name FROM guild_member");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GuildMember));
        records[count].guild_id = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        records[count].charac_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].charac_name, row[4], sizeof(records[count].charac_name) - 1);
        if (row[5]) strncpy(records[count].memo, row[5], sizeof(records[count].memo) - 1);
        records[count].grade = row[6] ? atoi(row[6]) : 0;
        records[count].job = row[7] ? atoi(row[7]) : 0;
        records[count].grow_type = row[8] ? atoi(row[8]) : 0;
        records[count].lev = row[9] ? atoi(row[9]) : 0;
        records[count].age = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].born_year, row[11], sizeof(records[count].born_year) - 1);
        if (row[12]) strncpy(records[count].sex, row[12], sizeof(records[count].sex) - 1);
        if (row[13]) strncpy(records[count].apply_time, row[13], sizeof(records[count].apply_time) - 1);
        if (row[14]) strncpy(records[count].member_time, row[14], sizeof(records[count].member_time) - 1);
        records[count].member_flag = row[15] ? atoi(row[15]) : 0;
        records[count].bbs_cnt = row[16] ? atoi(row[16]) : 0;
        if (row[17]) strncpy(records[count].last_visit_time, row[17], sizeof(records[count].last_visit_time) - 1);
        records[count].secede_type = row[18] ? atoi(row[18]) : 0;
        if (row[19]) strncpy(records[count].secede_time, row[19], sizeof(records[count].secede_time) - 1);
        records[count].member_point = row[20] ? atoi(row[20]) : 0;
        records[count].member_point_prev = row[21] ? atoi(row[21]) : 0;
        if (row[22]) strncpy(records[count].last_play_time, row[22], sizeof(records[count].last_play_time) - 1);
        if (row[23]) strncpy(records[count].nick_name, row[23], sizeof(records[count].nick_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
