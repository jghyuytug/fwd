#include "member_play_info_del_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* Add a new deleted member play info record */
int MemberPlayInfoDel_Add(DBConnectionManager* manager, const MemberPlayInfoDel* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_play_info_del "
        "(sdate, occ_date, m_id, play_time, play_count, trade_cnt, exp, used_fatigue, "
        "ip, last_play_time, pcbang_flag, end_ip, ting_count, mac_addr, server_id) "
        "VALUES ('%s', '%s', %d, %u, %u, %d, %u, %u, '%s', %u, %u, '%s', %u, '%s', %u)",
        info->sdate, info->occ_date, info->m_id, info->play_time, info->play_count,
        info->trade_cnt, info->exp, info->used_fatigue, info->ip,
        info->last_play_time, info->pcbang_flag, info->end_ip,
        info->ting_count, info->mac_addr, info->server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get deleted member play info by 3-field composite primary key */
int MemberPlayInfoDel_Get(DBConnectionManager* manager, const char* sdate,
                           const char* occ_date, int m_id, MemberPlayInfoDel* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[15];

    if (!manager || !sdate || !occ_date || !info) return -1;

    snprintf(query, sizeof(query),
        "SELECT sdate, occ_date, m_id, play_time, play_count, trade_cnt, exp, used_fatigue, "
        "ip, last_play_time, pcbang_flag, end_ip, ting_count, mac_addr, server_id "
        "FROM member_play_info_del WHERE sdate = '%s' AND occ_date = '%s' AND m_id = %d",
        sdate, occ_date, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(info, 0, sizeof(MemberPlayInfoDel));
    if (row[0]) strncpy(info->sdate, row[0], sizeof(info->sdate) - 1);
    if (row[1]) strncpy(info->occ_date, row[1], sizeof(info->occ_date) - 1);
    info->m_id = row[2] ? atoi(row[2]) : 0;
    info->play_time = row[3] ? (unsigned int)atol(row[3]) : 0;
    info->play_count = row[4] ? (unsigned int)atol(row[4]) : 0;
    info->trade_cnt = row[5] ? atoi(row[5]) : 0;
    info->exp = row[6] ? (unsigned int)atol(row[6]) : 0;
    info->used_fatigue = row[7] ? (unsigned short)atoi(row[7]) : 0;
    if (row[8]) strncpy(info->ip, row[8], sizeof(info->ip) - 1);
    info->last_play_time = row[9] ? (unsigned int)atol(row[9]) : 0;
    info->pcbang_flag = row[10] ? (unsigned char)atoi(row[10]) : 0;
    if (row[11]) strncpy(info->end_ip, row[11], sizeof(info->end_ip) - 1);
    info->ting_count = row[12] ? (unsigned short)atoi(row[12]) : 0;
    if (row[13]) strncpy(info->mac_addr, row[13], sizeof(info->mac_addr) - 1);
    info->server_id = row[14] ? (unsigned char)atoi(row[14]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

/* Update deleted member play info */
int MemberPlayInfoDel_Update(DBConnectionManager* manager, const MemberPlayInfoDel* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_play_info_del SET "
        "play_time = %u, play_count = %u, trade_cnt = %d, exp = %u, "
        "used_fatigue = %u, ip = '%s', last_play_time = %u, pcbang_flag = %u, "
        "end_ip = '%s', ting_count = %u, mac_addr = '%s', server_id = %u "
        "WHERE sdate = '%s' AND occ_date = '%s' AND m_id = %d",
        info->play_time, info->play_count, info->trade_cnt, info->exp,
        info->used_fatigue, info->ip, info->last_play_time, info->pcbang_flag,
        info->end_ip, info->ting_count, info->mac_addr, info->server_id,
        info->sdate, info->occ_date, info->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete by 3-field composite primary key */
int MemberPlayInfoDel_Delete(DBConnectionManager* manager, const char* sdate,
                              const char* occ_date, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !sdate || !occ_date) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_play_info_del WHERE sdate = '%s' AND occ_date = '%s' AND m_id = %d",
        sdate, occ_date, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if deleted member play info exists */
int MemberPlayInfoDel_Exists(DBConnectionManager* manager, const char* sdate,
                              const char* occ_date, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !sdate || !occ_date) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_play_info_del "
        "WHERE sdate = '%s' AND occ_date = '%s' AND m_id = %d",
        sdate, occ_date, m_id);

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

/* Get all deleted play info records for a member */
int MemberPlayInfoDel_GetByMemberId(DBConnectionManager* manager, int m_id,
                                     MemberPlayInfoDel* infos, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[15];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT sdate, occ_date, m_id, play_time, play_count, trade_cnt, exp, used_fatigue, "
        "ip, last_play_time, pcbang_flag, end_ip, ting_count, mac_addr, server_id "
        "FROM member_play_info_del WHERE m_id = %d ORDER BY sdate DESC, occ_date DESC",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(MemberPlayInfoDel));
        if (row[0]) strncpy(infos[count].sdate, row[0], sizeof(infos[count].sdate) - 1);
        if (row[1]) strncpy(infos[count].occ_date, row[1], sizeof(infos[count].occ_date) - 1);
        infos[count].m_id = row[2] ? atoi(row[2]) : 0;
        infos[count].play_time = row[3] ? (unsigned int)atol(row[3]) : 0;
        infos[count].play_count = row[4] ? (unsigned int)atol(row[4]) : 0;
        infos[count].trade_cnt = row[5] ? atoi(row[5]) : 0;
        infos[count].exp = row[6] ? (unsigned int)atol(row[6]) : 0;
        infos[count].used_fatigue = row[7] ? (unsigned short)atoi(row[7]) : 0;
        if (row[8]) strncpy(infos[count].ip, row[8], sizeof(infos[count].ip) - 1);
        infos[count].last_play_time = row[9] ? (unsigned int)atol(row[9]) : 0;
        infos[count].pcbang_flag = row[10] ? (unsigned char)atoi(row[10]) : 0;
        if (row[11]) strncpy(infos[count].end_ip, row[11], sizeof(infos[count].end_ip) - 1);
        infos[count].ting_count = row[12] ? (unsigned short)atoi(row[12]) : 0;
        if (row[13]) strncpy(infos[count].mac_addr, row[13], sizeof(infos[count].mac_addr) - 1);
        infos[count].server_id = row[14] ? (unsigned char)atoi(row[14]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get deleted records by delete date */
int MemberPlayInfoDel_GetByDeleteDate(DBConnectionManager* manager, const char* sdate,
                                       MemberPlayInfoDel* infos, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[15];
    int count = 0;

    if (!manager || !sdate || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT sdate, occ_date, m_id, play_time, play_count, trade_cnt, exp, used_fatigue, "
        "ip, last_play_time, pcbang_flag, end_ip, ting_count, mac_addr, server_id "
        "FROM member_play_info_del WHERE sdate = '%s' ORDER BY occ_date DESC, m_id",
        sdate);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(MemberPlayInfoDel));
        if (row[0]) strncpy(infos[count].sdate, row[0], sizeof(infos[count].sdate) - 1);
        if (row[1]) strncpy(infos[count].occ_date, row[1], sizeof(infos[count].occ_date) - 1);
        infos[count].m_id = row[2] ? atoi(row[2]) : 0;
        infos[count].play_time = row[3] ? (unsigned int)atol(row[3]) : 0;
        infos[count].play_count = row[4] ? (unsigned int)atol(row[4]) : 0;
        infos[count].trade_cnt = row[5] ? atoi(row[5]) : 0;
        infos[count].exp = row[6] ? (unsigned int)atol(row[6]) : 0;
        infos[count].used_fatigue = row[7] ? (unsigned short)atoi(row[7]) : 0;
        if (row[8]) strncpy(infos[count].ip, row[8], sizeof(infos[count].ip) - 1);
        infos[count].last_play_time = row[9] ? (unsigned int)atol(row[9]) : 0;
        infos[count].pcbang_flag = row[10] ? (unsigned char)atoi(row[10]) : 0;
        if (row[11]) strncpy(infos[count].end_ip, row[11], sizeof(infos[count].end_ip) - 1);
        infos[count].ting_count = row[12] ? (unsigned short)atoi(row[12]) : 0;
        if (row[13]) strncpy(infos[count].mac_addr, row[13], sizeof(infos[count].mac_addr) - 1);
        infos[count].server_id = row[14] ? (unsigned char)atoi(row[14]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get deleted records within date range */
int MemberPlayInfoDel_GetByDateRange(DBConnectionManager* manager, const char* start_sdate,
                                      const char* end_sdate, MemberPlayInfoDel* infos,
                                      int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[15];
    int count = 0;

    if (!manager || !start_sdate || !end_sdate || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT sdate, occ_date, m_id, play_time, play_count, trade_cnt, exp, used_fatigue, "
        "ip, last_play_time, pcbang_flag, end_ip, ting_count, mac_addr, server_id "
        "FROM member_play_info_del WHERE sdate >= '%s' AND sdate <= '%s' "
        "ORDER BY sdate DESC, occ_date DESC",
        start_sdate, end_sdate);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(MemberPlayInfoDel));
        if (row[0]) strncpy(infos[count].sdate, row[0], sizeof(infos[count].sdate) - 1);
        if (row[1]) strncpy(infos[count].occ_date, row[1], sizeof(infos[count].occ_date) - 1);
        infos[count].m_id = row[2] ? atoi(row[2]) : 0;
        infos[count].play_time = row[3] ? (unsigned int)atol(row[3]) : 0;
        infos[count].play_count = row[4] ? (unsigned int)atol(row[4]) : 0;
        infos[count].trade_cnt = row[5] ? atoi(row[5]) : 0;
        infos[count].exp = row[6] ? (unsigned int)atol(row[6]) : 0;
        infos[count].used_fatigue = row[7] ? (unsigned short)atoi(row[7]) : 0;
        if (row[8]) strncpy(infos[count].ip, row[8], sizeof(infos[count].ip) - 1);
        infos[count].last_play_time = row[9] ? (unsigned int)atol(row[9]) : 0;
        infos[count].pcbang_flag = row[10] ? (unsigned char)atoi(row[10]) : 0;
        if (row[11]) strncpy(infos[count].end_ip, row[11], sizeof(infos[count].end_ip) - 1);
        infos[count].ting_count = row[12] ? (unsigned short)atoi(row[12]) : 0;
        if (row[13]) strncpy(infos[count].mac_addr, row[13], sizeof(infos[count].mac_addr) - 1);
        infos[count].server_id = row[14] ? (unsigned char)atoi(row[14]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Print deleted member play info details */
void MemberPlayInfoDel_PrintInfo(const MemberPlayInfoDel* info) {
    if (!info) return;

    printf("=== Member Play Info Del ===\n");
    printf("Delete Date: %s\n", info->sdate);
    printf("Occurrence Date: %s\n", info->occ_date);
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
    printf("============================\n");
}
