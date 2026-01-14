#include "member_play_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* Add a new member play info record */
int MemberPlayInfo_Add(DBConnectionManager* manager, const MemberPlayInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_play_info "
        "(occ_date, m_id, play_time, play_count, trade_cnt, exp, used_fatigue, "
        "ip, last_play_time, pcbang_flag, end_ip, ting_count, mac_addr, server_id) "
        "VALUES ('%s', %d, %u, %u, %d, %u, %u, '%s', %u, %u, '%s', %u, '%s', %u)",
        info->occ_date, info->m_id, info->play_time, info->play_count,
        info->trade_cnt, info->exp, info->used_fatigue, info->ip,
        info->last_play_time, info->pcbang_flag, info->end_ip,
        info->ting_count, info->mac_addr, info->server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get member play info by composite primary key */
int MemberPlayInfo_Get(DBConnectionManager* manager, const char* occ_date, int m_id,
                        MemberPlayInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];

    if (!manager || !occ_date || !info) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, play_time, play_count, trade_cnt, exp, used_fatigue, "
        "ip, last_play_time, pcbang_flag, end_ip, ting_count, mac_addr, server_id "
        "FROM member_play_info WHERE occ_date = '%s' AND m_id = %d",
        occ_date, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(info, 0, sizeof(MemberPlayInfo));
    if (row[0]) strncpy(info->occ_date, row[0], sizeof(info->occ_date) - 1);
    info->m_id = row[1] ? atoi(row[1]) : 0;
    info->play_time = row[2] ? (unsigned int)atol(row[2]) : 0;
    info->play_count = row[3] ? (unsigned int)atol(row[3]) : 0;
    info->trade_cnt = row[4] ? atoi(row[4]) : 0;
    info->exp = row[5] ? (unsigned int)atol(row[5]) : 0;
    info->used_fatigue = row[6] ? (unsigned short)atoi(row[6]) : 0;
    if (row[7]) strncpy(info->ip, row[7], sizeof(info->ip) - 1);
    info->last_play_time = row[8] ? (unsigned int)atol(row[8]) : 0;
    info->pcbang_flag = row[9] ? (unsigned char)atoi(row[9]) : 0;
    if (row[10]) strncpy(info->end_ip, row[10], sizeof(info->end_ip) - 1);
    info->ting_count = row[11] ? (unsigned short)atoi(row[11]) : 0;
    if (row[12]) strncpy(info->mac_addr, row[12], sizeof(info->mac_addr) - 1);
    info->server_id = row[13] ? (unsigned char)atoi(row[13]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

/* Update member play info */
int MemberPlayInfo_Update(DBConnectionManager* manager, const MemberPlayInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_play_info SET "
        "play_time = %u, play_count = %u, trade_cnt = %d, exp = %u, "
        "used_fatigue = %u, ip = '%s', last_play_time = %u, pcbang_flag = %u, "
        "end_ip = '%s', ting_count = %u, mac_addr = '%s', server_id = %u "
        "WHERE occ_date = '%s' AND m_id = %d",
        info->play_time, info->play_count, info->trade_cnt, info->exp,
        info->used_fatigue, info->ip, info->last_play_time, info->pcbang_flag,
        info->end_ip, info->ting_count, info->mac_addr, info->server_id,
        info->occ_date, info->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete member play info by composite primary key */
int MemberPlayInfo_Delete(DBConnectionManager* manager, const char* occ_date, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_date) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_play_info WHERE occ_date = '%s' AND m_id = %d",
        occ_date, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if member play info exists */
int MemberPlayInfo_Exists(DBConnectionManager* manager, const char* occ_date, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !occ_date) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_play_info WHERE occ_date = '%s' AND m_id = %d",
        occ_date, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count > 0;
}

/* Get all play info records for a member */
int MemberPlayInfo_GetByMemberId(DBConnectionManager* manager, int m_id,
                                  MemberPlayInfo* infos, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, play_time, play_count, trade_cnt, exp, used_fatigue, "
        "ip, last_play_time, pcbang_flag, end_ip, ting_count, mac_addr, server_id "
        "FROM member_play_info WHERE m_id = %d ORDER BY occ_date DESC",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(MemberPlayInfo));
        if (row[0]) strncpy(infos[count].occ_date, row[0], sizeof(infos[count].occ_date) - 1);
        infos[count].m_id = row[1] ? atoi(row[1]) : 0;
        infos[count].play_time = row[2] ? (unsigned int)atol(row[2]) : 0;
        infos[count].play_count = row[3] ? (unsigned int)atol(row[3]) : 0;
        infos[count].trade_cnt = row[4] ? atoi(row[4]) : 0;
        infos[count].exp = row[5] ? (unsigned int)atol(row[5]) : 0;
        infos[count].used_fatigue = row[6] ? (unsigned short)atoi(row[6]) : 0;
        if (row[7]) strncpy(infos[count].ip, row[7], sizeof(infos[count].ip) - 1);
        infos[count].last_play_time = row[8] ? (unsigned int)atol(row[8]) : 0;
        infos[count].pcbang_flag = row[9] ? (unsigned char)atoi(row[9]) : 0;
        if (row[10]) strncpy(infos[count].end_ip, row[10], sizeof(infos[count].end_ip) - 1);
        infos[count].ting_count = row[11] ? (unsigned short)atoi(row[11]) : 0;
        if (row[12]) strncpy(infos[count].mac_addr, row[12], sizeof(infos[count].mac_addr) - 1);
        infos[count].server_id = row[13] ? (unsigned char)atoi(row[13]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get play info records within date range */
int MemberPlayInfo_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                   const char* end_date, MemberPlayInfo* infos,
                                   int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];
    int count = 0;

    if (!manager || !start_date || !end_date || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, play_time, play_count, trade_cnt, exp, used_fatigue, "
        "ip, last_play_time, pcbang_flag, end_ip, ting_count, mac_addr, server_id "
        "FROM member_play_info WHERE occ_date >= '%s' AND occ_date <= '%s' "
        "ORDER BY occ_date DESC, m_id",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(MemberPlayInfo));
        if (row[0]) strncpy(infos[count].occ_date, row[0], sizeof(infos[count].occ_date) - 1);
        infos[count].m_id = row[1] ? atoi(row[1]) : 0;
        infos[count].play_time = row[2] ? (unsigned int)atol(row[2]) : 0;
        infos[count].play_count = row[3] ? (unsigned int)atol(row[3]) : 0;
        infos[count].trade_cnt = row[4] ? atoi(row[4]) : 0;
        infos[count].exp = row[5] ? (unsigned int)atol(row[5]) : 0;
        infos[count].used_fatigue = row[6] ? (unsigned short)atoi(row[6]) : 0;
        if (row[7]) strncpy(infos[count].ip, row[7], sizeof(infos[count].ip) - 1);
        infos[count].last_play_time = row[8] ? (unsigned int)atol(row[8]) : 0;
        infos[count].pcbang_flag = row[9] ? (unsigned char)atoi(row[9]) : 0;
        if (row[10]) strncpy(infos[count].end_ip, row[10], sizeof(infos[count].end_ip) - 1);
        infos[count].ting_count = row[11] ? (unsigned short)atoi(row[11]) : 0;
        if (row[12]) strncpy(infos[count].mac_addr, row[12], sizeof(infos[count].mac_addr) - 1);
        infos[count].server_id = row[13] ? (unsigned char)atoi(row[13]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get play info records by server ID */
int MemberPlayInfo_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                                MemberPlayInfo* infos, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, play_time, play_count, trade_cnt, exp, used_fatigue, "
        "ip, last_play_time, pcbang_flag, end_ip, ting_count, mac_addr, server_id "
        "FROM member_play_info WHERE server_id = %u ORDER BY occ_date DESC",
        server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(MemberPlayInfo));
        if (row[0]) strncpy(infos[count].occ_date, row[0], sizeof(infos[count].occ_date) - 1);
        infos[count].m_id = row[1] ? atoi(row[1]) : 0;
        infos[count].play_time = row[2] ? (unsigned int)atol(row[2]) : 0;
        infos[count].play_count = row[3] ? (unsigned int)atol(row[3]) : 0;
        infos[count].trade_cnt = row[4] ? atoi(row[4]) : 0;
        infos[count].exp = row[5] ? (unsigned int)atol(row[5]) : 0;
        infos[count].used_fatigue = row[6] ? (unsigned short)atoi(row[6]) : 0;
        if (row[7]) strncpy(infos[count].ip, row[7], sizeof(infos[count].ip) - 1);
        infos[count].last_play_time = row[8] ? (unsigned int)atol(row[8]) : 0;
        infos[count].pcbang_flag = row[9] ? (unsigned char)atoi(row[9]) : 0;
        if (row[10]) strncpy(infos[count].end_ip, row[10], sizeof(infos[count].end_ip) - 1);
        infos[count].ting_count = row[11] ? (unsigned short)atoi(row[11]) : 0;
        if (row[12]) strncpy(infos[count].mac_addr, row[12], sizeof(infos[count].mac_addr) - 1);
        infos[count].server_id = row[13] ? (unsigned char)atoi(row[13]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get PC bang users for a specific date */
int MemberPlayInfo_GetPCBangUsers(DBConnectionManager* manager, const char* occ_date,
                                   MemberPlayInfo* infos, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];
    int count = 0;

    if (!manager || !occ_date || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, play_time, play_count, trade_cnt, exp, used_fatigue, "
        "ip, last_play_time, pcbang_flag, end_ip, ting_count, mac_addr, server_id "
        "FROM member_play_info WHERE occ_date = '%s' AND pcbang_flag = 1 "
        "ORDER BY m_id",
        occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(MemberPlayInfo));
        if (row[0]) strncpy(infos[count].occ_date, row[0], sizeof(infos[count].occ_date) - 1);
        infos[count].m_id = row[1] ? atoi(row[1]) : 0;
        infos[count].play_time = row[2] ? (unsigned int)atol(row[2]) : 0;
        infos[count].play_count = row[3] ? (unsigned int)atol(row[3]) : 0;
        infos[count].trade_cnt = row[4] ? atoi(row[4]) : 0;
        infos[count].exp = row[5] ? (unsigned int)atol(row[5]) : 0;
        infos[count].used_fatigue = row[6] ? (unsigned short)atoi(row[6]) : 0;
        if (row[7]) strncpy(infos[count].ip, row[7], sizeof(infos[count].ip) - 1);
        infos[count].last_play_time = row[8] ? (unsigned int)atol(row[8]) : 0;
        infos[count].pcbang_flag = row[9] ? (unsigned char)atoi(row[9]) : 0;
        if (row[10]) strncpy(infos[count].end_ip, row[10], sizeof(infos[count].end_ip) - 1);
        infos[count].ting_count = row[11] ? (unsigned short)atoi(row[11]) : 0;
        if (row[12]) strncpy(infos[count].mac_addr, row[12], sizeof(infos[count].mac_addr) - 1);
        infos[count].server_id = row[13] ? (unsigned char)atoi(row[13]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Print member play info details */
void MemberPlayInfo_PrintInfo(const MemberPlayInfo* info) {
    if (!info) return;

    printf("=== Member Play Info ===\n");
    printf("Date: %s\n", info->occ_date);
    printf("Member ID: %d\n", info->m_id);
    printf("Play Time: %u seconds\n", info->play_time);
    printf("Play Count: %u\n", info->play_count);
    printf("Trade Count: %d\n", info->trade_cnt);
    printf("Experience: %u\n", info->exp);
    printf("Used Fatigue: %u\n", info->used_fatigue);
    printf("IP: %s\n", info->ip);
    printf("Last Play Time: %u\n", info->last_play_time);
    printf("PC Bang: %s\n", info->pcbang_flag ? "Yes" : "No");
    printf("End IP: %s\n", info->end_ip);
    printf("Ting Count: %u\n", info->ting_count);
    printf("MAC Address: %s\n", info->mac_addr);
    printf("Server ID: %u\n", info->server_id);
    printf("========================\n");
}
