#include "member_login_backup_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int MemberLoginBackup_Add(DBConnectionManager* manager, const MemberLoginBackup* backup) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !backup) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_login_backup "
        "(m_id, login_time, expire_time, last_play_time, total_account_fail, "
        "account_fail, report_cnt, reliable_flag, trade_gold_daily, last_gift_time, "
        "gift_cnt, login_ip, security_flag, power_side, dungeon_gain_gold, "
        "school_id, rating, cleanpad_point, tutorial_skipable) "
        "VALUES (%d, %u, %u, %u, %u, %d, %d, %d, %u, %u, %u, '%s', %d, %d, %u, %d, %f, %u, '%c')",
        backup->m_id, backup->login_time, backup->expire_time, backup->last_play_time,
        backup->total_account_fail, backup->account_fail, backup->report_cnt,
        backup->reliable_flag, backup->trade_gold_daily, backup->last_gift_time,
        backup->gift_cnt, backup->login_ip, backup->security_flag, backup->power_side,
        backup->dungeon_gain_gold, backup->school_id, backup->rating,
        backup->cleanpad_point, backup->tutorial_skipable);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberLoginBackup_GetByMemberId(DBConnectionManager* manager, int m_id,
                                     MemberLoginBackup* backups, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[19];
    int count = 0;

    if (!manager || !backups || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, login_time, expire_time, last_play_time, total_account_fail, "
        "account_fail, report_cnt, reliable_flag, trade_gold_daily, last_gift_time, "
        "gift_cnt, login_ip, security_flag, power_side, dungeon_gain_gold, "
        "school_id, rating, cleanpad_point, tutorial_skipable "
        "FROM member_login_backup WHERE m_id = %d ORDER BY login_time DESC", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&backups[count], 0, sizeof(MemberLoginBackup));

        backups[count].m_id = row[0] ? atoi(row[0]) : 0;
        backups[count].login_time = row[1] ? (unsigned int)strtoul(row[1], NULL, 10) : 0;
        backups[count].expire_time = row[2] ? (unsigned int)strtoul(row[2], NULL, 10) : 0;
        backups[count].last_play_time = row[3] ? (unsigned int)strtoul(row[3], NULL, 10) : 0;
        backups[count].total_account_fail = row[4] ? (unsigned int)strtoul(row[4], NULL, 10) : 0;
        backups[count].account_fail = row[5] ? (char)atoi(row[5]) : 0;
        backups[count].report_cnt = row[6] ? atoi(row[6]) : 0;
        backups[count].reliable_flag = row[7] ? (char)atoi(row[7]) : 0;
        backups[count].trade_gold_daily = row[8] ? (unsigned int)strtoul(row[8], NULL, 10) : 0;
        backups[count].last_gift_time = row[9] ? (unsigned int)strtoul(row[9], NULL, 10) : 0;
        backups[count].gift_cnt = row[10] ? (unsigned short)atoi(row[10]) : 0;
        if (row[11]) strncpy(backups[count].login_ip, row[11], sizeof(backups[count].login_ip) - 1);
        backups[count].security_flag = row[12] ? (char)atoi(row[12]) : 0;
        backups[count].power_side = row[13] ? (char)atoi(row[13]) : 0;
        backups[count].dungeon_gain_gold = row[14] ? (unsigned int)strtoul(row[14], NULL, 10) : 0;
        backups[count].school_id = row[15] ? atoi(row[15]) : 0;
        backups[count].rating = row[16] ? (float)atof(row[16]) : 0.0f;
        backups[count].cleanpad_point = row[17] ? (unsigned int)strtoul(row[17], NULL, 10) : 0;
        backups[count].tutorial_skipable = row[18] ? row[18][0] : '0';

        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberLoginBackup_DeleteByMemberId(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_login_backup WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberLoginBackup_DeleteAll(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM member_login_backup");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberLoginBackup_GetByIp(DBConnectionManager* manager, const char* login_ip,
                               MemberLoginBackup* backups, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[19];
    int count = 0;

    if (!manager || !login_ip || !backups || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, login_time, expire_time, last_play_time, total_account_fail, "
        "account_fail, report_cnt, reliable_flag, trade_gold_daily, last_gift_time, "
        "gift_cnt, login_ip, security_flag, power_side, dungeon_gain_gold, "
        "school_id, rating, cleanpad_point, tutorial_skipable "
        "FROM member_login_backup WHERE login_ip = '%s' ORDER BY login_time DESC LIMIT %d",
        login_ip, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&backups[count], 0, sizeof(MemberLoginBackup));

        backups[count].m_id = row[0] ? atoi(row[0]) : 0;
        backups[count].login_time = row[1] ? (unsigned int)strtoul(row[1], NULL, 10) : 0;
        backups[count].expire_time = row[2] ? (unsigned int)strtoul(row[2], NULL, 10) : 0;
        backups[count].last_play_time = row[3] ? (unsigned int)strtoul(row[3], NULL, 10) : 0;
        backups[count].total_account_fail = row[4] ? (unsigned int)strtoul(row[4], NULL, 10) : 0;
        backups[count].account_fail = row[5] ? (char)atoi(row[5]) : 0;
        backups[count].report_cnt = row[6] ? atoi(row[6]) : 0;
        backups[count].reliable_flag = row[7] ? (char)atoi(row[7]) : 0;
        backups[count].trade_gold_daily = row[8] ? (unsigned int)strtoul(row[8], NULL, 10) : 0;
        backups[count].last_gift_time = row[9] ? (unsigned int)strtoul(row[9], NULL, 10) : 0;
        backups[count].gift_cnt = row[10] ? (unsigned short)atoi(row[10]) : 0;
        if (row[11]) strncpy(backups[count].login_ip, row[11], sizeof(backups[count].login_ip) - 1);
        backups[count].security_flag = row[12] ? (char)atoi(row[12]) : 0;
        backups[count].power_side = row[13] ? (char)atoi(row[13]) : 0;
        backups[count].dungeon_gain_gold = row[14] ? (unsigned int)strtoul(row[14], NULL, 10) : 0;
        backups[count].school_id = row[15] ? atoi(row[15]) : 0;
        backups[count].rating = row[16] ? (float)atof(row[16]) : 0.0f;
        backups[count].cleanpad_point = row[17] ? (unsigned int)strtoul(row[17], NULL, 10) : 0;
        backups[count].tutorial_skipable = row[18] ? row[18][0] : '0';

        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberLoginBackup_GetByDateRange(DBConnectionManager* manager,
                                       unsigned int start_time, unsigned int end_time,
                                       MemberLoginBackup* backups, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[19];
    int count = 0;

    if (!manager || !backups || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, login_time, expire_time, last_play_time, total_account_fail, "
        "account_fail, report_cnt, reliable_flag, trade_gold_daily, last_gift_time, "
        "gift_cnt, login_ip, security_flag, power_side, dungeon_gain_gold, "
        "school_id, rating, cleanpad_point, tutorial_skipable "
        "FROM member_login_backup WHERE login_time >= %u AND login_time <= %u "
        "ORDER BY login_time DESC LIMIT %d", start_time, end_time, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&backups[count], 0, sizeof(MemberLoginBackup));

        backups[count].m_id = row[0] ? atoi(row[0]) : 0;
        backups[count].login_time = row[1] ? (unsigned int)strtoul(row[1], NULL, 10) : 0;
        backups[count].expire_time = row[2] ? (unsigned int)strtoul(row[2], NULL, 10) : 0;
        backups[count].last_play_time = row[3] ? (unsigned int)strtoul(row[3], NULL, 10) : 0;
        backups[count].total_account_fail = row[4] ? (unsigned int)strtoul(row[4], NULL, 10) : 0;
        backups[count].account_fail = row[5] ? (char)atoi(row[5]) : 0;
        backups[count].report_cnt = row[6] ? atoi(row[6]) : 0;
        backups[count].reliable_flag = row[7] ? (char)atoi(row[7]) : 0;
        backups[count].trade_gold_daily = row[8] ? (unsigned int)strtoul(row[8], NULL, 10) : 0;
        backups[count].last_gift_time = row[9] ? (unsigned int)strtoul(row[9], NULL, 10) : 0;
        backups[count].gift_cnt = row[10] ? (unsigned short)atoi(row[10]) : 0;
        if (row[11]) strncpy(backups[count].login_ip, row[11], sizeof(backups[count].login_ip) - 1);
        backups[count].security_flag = row[12] ? (char)atoi(row[12]) : 0;
        backups[count].power_side = row[13] ? (char)atoi(row[13]) : 0;
        backups[count].dungeon_gain_gold = row[14] ? (unsigned int)strtoul(row[14], NULL, 10) : 0;
        backups[count].school_id = row[15] ? atoi(row[15]) : 0;
        backups[count].rating = row[16] ? (float)atof(row[16]) : 0.0f;
        backups[count].cleanpad_point = row[17] ? (unsigned int)strtoul(row[17], NULL, 10) : 0;
        backups[count].tutorial_skipable = row[18] ? row[18][0] : '0';

        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberLoginBackup_CountByMemberId(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_login_backup WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        count = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return count;
}

void MemberLoginBackup_PrintInfo(const MemberLoginBackup* backup) {
    if (!backup) {
        printf("MemberLoginBackup: NULL\n");
        return;
    }

    printf("MemberLoginBackup:\n");
    printf("  m_id: %d\n", backup->m_id);
    printf("  login_time: %u\n", backup->login_time);
    printf("  expire_time: %u\n", backup->expire_time);
    printf("  last_play_time: %u\n", backup->last_play_time);
    printf("  total_account_fail: %u\n", backup->total_account_fail);
    printf("  account_fail: %d\n", backup->account_fail);
    printf("  report_cnt: %d\n", backup->report_cnt);
    printf("  reliable_flag: %d\n", backup->reliable_flag);
    printf("  trade_gold_daily: %u\n", backup->trade_gold_daily);
    printf("  last_gift_time: %u\n", backup->last_gift_time);
    printf("  gift_cnt: %u\n", backup->gift_cnt);
    printf("  login_ip: %s\n", backup->login_ip);
    printf("  security_flag: %d\n", backup->security_flag);
    printf("  power_side: %d\n", backup->power_side);
    printf("  dungeon_gain_gold: %u\n", backup->dungeon_gain_gold);
    printf("  school_id: %d\n", backup->school_id);
    printf("  rating: %.2f\n", backup->rating);
    printf("  cleanpad_point: %u\n", backup->cleanpad_point);
    printf("  tutorial_skipable: %c\n", backup->tutorial_skipable);
}
