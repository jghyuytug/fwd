#include "member_punish_info_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int MemberPunishInfoHistory_Add(DBConnectionManager* manager, const MemberPunishInfoHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_punish_info_history "
        "(m_id, punish_type, occ_time, punish_value, apply_flag, start_time, end_time, "
        "admin_id, reason, is_kicked, first_ssn, second_ssn) "
        "VALUES (%d, %d, '%s', %d, %u, '%s', '%s', %s%s%s, %s%s%s, %u, %s%s%s, %s%s%s)",
        record->m_id, record->punish_type, record->occ_time, record->punish_value,
        record->apply_flag, record->start_time, record->end_time,
        record->admin_id[0] ? "'" : "NULL", record->admin_id[0] ? record->admin_id : "", record->admin_id[0] ? "'" : "",
        record->reason[0] ? "'" : "NULL", record->reason[0] ? record->reason : "", record->reason[0] ? "'" : "",
        record->is_kicked,
        record->first_ssn[0] ? "'" : "NULL", record->first_ssn[0] ? record->first_ssn : "", record->first_ssn[0] ? "'" : "",
        record->second_ssn[0] ? "'" : "NULL", record->second_ssn[0] ? record->second_ssn : "", record->second_ssn[0] ? "'" : "");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoHistory_Get(DBConnectionManager* manager, unsigned int no, MemberPunishInfoHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, punish_type, occ_time, punish_value, apply_flag, start_time, "
        "end_time, admin_id, reason, is_kicked, first_ssn, second_ssn "
        "FROM member_punish_info_history WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberPunishInfoHistory));
    record->no = row[0] ? (unsigned int)atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->punish_type = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->occ_time, row[3], sizeof(record->occ_time) - 1);
    record->punish_value = row[4] ? atoi(row[4]) : 0;
    record->apply_flag = row[5] ? (unsigned char)atoi(row[5]) : 0;
    if (row[6]) strncpy(record->start_time, row[6], sizeof(record->start_time) - 1);
    if (row[7]) strncpy(record->end_time, row[7], sizeof(record->end_time) - 1);
    if (row[8]) strncpy(record->admin_id, row[8], sizeof(record->admin_id) - 1);
    if (row[9]) strncpy(record->reason, row[9], sizeof(record->reason) - 1);
    record->is_kicked = row[10] ? (unsigned char)atoi(row[10]) : 0;
    if (row[11]) strncpy(record->first_ssn, row[11], sizeof(record->first_ssn) - 1);
    if (row[12]) strncpy(record->second_ssn, row[12], sizeof(record->second_ssn) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoHistory_Update(DBConnectionManager* manager, const MemberPunishInfoHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_punish_info_history SET "
        "m_id = %d, punish_type = %d, occ_time = '%s', punish_value = %d, "
        "apply_flag = %u, start_time = '%s', end_time = '%s', "
        "admin_id = %s%s%s, reason = %s%s%s, is_kicked = %u, "
        "first_ssn = %s%s%s, second_ssn = %s%s%s "
        "WHERE no = %u",
        record->m_id, record->punish_type, record->occ_time, record->punish_value,
        record->apply_flag, record->start_time, record->end_time,
        record->admin_id[0] ? "'" : "NULL", record->admin_id[0] ? record->admin_id : "", record->admin_id[0] ? "'" : "",
        record->reason[0] ? "'" : "NULL", record->reason[0] ? record->reason : "", record->reason[0] ? "'" : "",
        record->is_kicked,
        record->first_ssn[0] ? "'" : "NULL", record->first_ssn[0] ? record->first_ssn : "", record->first_ssn[0] ? "'" : "",
        record->second_ssn[0] ? "'" : "NULL", record->second_ssn[0] ? record->second_ssn : "", record->second_ssn[0] ? "'" : "",
        record->no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoHistory_Delete(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_punish_info_history WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoHistory_Exists(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_punish_info_history WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int MemberPunishInfoHistory_GetByMemberId(DBConnectionManager* manager, int m_id,
                                           MemberPunishInfoHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, punish_type, occ_time, punish_value, apply_flag, start_time, "
        "end_time, admin_id, reason, is_kicked, first_ssn, second_ssn "
        "FROM member_punish_info_history WHERE m_id = %d ORDER BY occ_time DESC", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberPunishInfoHistory));
        records[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].punish_type = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].occ_time, row[3], sizeof(records[count].occ_time) - 1);
        records[count].punish_value = row[4] ? atoi(row[4]) : 0;
        records[count].apply_flag = row[5] ? (unsigned char)atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].start_time, row[6], sizeof(records[count].start_time) - 1);
        if (row[7]) strncpy(records[count].end_time, row[7], sizeof(records[count].end_time) - 1);
        if (row[8]) strncpy(records[count].admin_id, row[8], sizeof(records[count].admin_id) - 1);
        if (row[9]) strncpy(records[count].reason, row[9], sizeof(records[count].reason) - 1);
        records[count].is_kicked = row[10] ? (unsigned char)atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].first_ssn, row[11], sizeof(records[count].first_ssn) - 1);
        if (row[12]) strncpy(records[count].second_ssn, row[12], sizeof(records[count].second_ssn) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoHistory_GetByPunishType(DBConnectionManager* manager, int punish_type,
                                             MemberPunishInfoHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, punish_type, occ_time, punish_value, apply_flag, start_time, "
        "end_time, admin_id, reason, is_kicked, first_ssn, second_ssn "
        "FROM member_punish_info_history WHERE punish_type = %d ORDER BY occ_time DESC", punish_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberPunishInfoHistory));
        records[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].punish_type = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].occ_time, row[3], sizeof(records[count].occ_time) - 1);
        records[count].punish_value = row[4] ? atoi(row[4]) : 0;
        records[count].apply_flag = row[5] ? (unsigned char)atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].start_time, row[6], sizeof(records[count].start_time) - 1);
        if (row[7]) strncpy(records[count].end_time, row[7], sizeof(records[count].end_time) - 1);
        if (row[8]) strncpy(records[count].admin_id, row[8], sizeof(records[count].admin_id) - 1);
        if (row[9]) strncpy(records[count].reason, row[9], sizeof(records[count].reason) - 1);
        records[count].is_kicked = row[10] ? (unsigned char)atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].first_ssn, row[11], sizeof(records[count].first_ssn) - 1);
        if (row[12]) strncpy(records[count].second_ssn, row[12], sizeof(records[count].second_ssn) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoHistory_GetByAdmin(DBConnectionManager* manager, const char* admin_id,
                                        MemberPunishInfoHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !admin_id || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, punish_type, occ_time, punish_value, apply_flag, start_time, "
        "end_time, admin_id, reason, is_kicked, first_ssn, second_ssn "
        "FROM member_punish_info_history WHERE admin_id = '%s' ORDER BY occ_time DESC", admin_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberPunishInfoHistory));
        records[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].punish_type = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].occ_time, row[3], sizeof(records[count].occ_time) - 1);
        records[count].punish_value = row[4] ? atoi(row[4]) : 0;
        records[count].apply_flag = row[5] ? (unsigned char)atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].start_time, row[6], sizeof(records[count].start_time) - 1);
        if (row[7]) strncpy(records[count].end_time, row[7], sizeof(records[count].end_time) - 1);
        if (row[8]) strncpy(records[count].admin_id, row[8], sizeof(records[count].admin_id) - 1);
        if (row[9]) strncpy(records[count].reason, row[9], sizeof(records[count].reason) - 1);
        records[count].is_kicked = row[10] ? (unsigned char)atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].first_ssn, row[11], sizeof(records[count].first_ssn) - 1);
        if (row[12]) strncpy(records[count].second_ssn, row[12], sizeof(records[count].second_ssn) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoHistory_GetByTimeRange(DBConnectionManager* manager, const char* start_time, const char* end_time,
                                            MemberPunishInfoHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !start_time || !end_time || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, punish_type, occ_time, punish_value, apply_flag, start_time, "
        "end_time, admin_id, reason, is_kicked, first_ssn, second_ssn "
        "FROM member_punish_info_history "
        "WHERE occ_time >= '%s' AND occ_time <= '%s' ORDER BY occ_time DESC",
        start_time, end_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberPunishInfoHistory));
        records[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].punish_type = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].occ_time, row[3], sizeof(records[count].occ_time) - 1);
        records[count].punish_value = row[4] ? atoi(row[4]) : 0;
        records[count].apply_flag = row[5] ? (unsigned char)atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].start_time, row[6], sizeof(records[count].start_time) - 1);
        if (row[7]) strncpy(records[count].end_time, row[7], sizeof(records[count].end_time) - 1);
        if (row[8]) strncpy(records[count].admin_id, row[8], sizeof(records[count].admin_id) - 1);
        if (row[9]) strncpy(records[count].reason, row[9], sizeof(records[count].reason) - 1);
        records[count].is_kicked = row[10] ? (unsigned char)atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].first_ssn, row[11], sizeof(records[count].first_ssn) - 1);
        if (row[12]) strncpy(records[count].second_ssn, row[12], sizeof(records[count].second_ssn) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoHistory_DeleteByMemberId(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_punish_info_history WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void MemberPunishInfoHistory_PrintInfo(const MemberPunishInfoHistory* record) {
    if (!record) {
        printf("MemberPunishInfoHistory: NULL\n");
        return;
    }

    printf("=== Member Punish Info History ===\n");
    printf("No:           %u\n", record->no);
    printf("M_ID:         %d\n", record->m_id);
    printf("Punish Type:  %d\n", record->punish_type);
    printf("Occ Time:     %s\n", record->occ_time);
    printf("Punish Value: %d\n", record->punish_value);
    printf("Apply Flag:   %u\n", record->apply_flag);
    printf("Start Time:   %s\n", record->start_time);
    printf("End Time:     %s\n", record->end_time);
    printf("Admin ID:     %s\n", record->admin_id[0] ? record->admin_id : "(null)");
    printf("Reason:       %s\n", record->reason[0] ? record->reason : "(null)");
    printf("Is Kicked:    %u\n", record->is_kicked);
    printf("First SSN:    %s\n", record->first_ssn[0] ? record->first_ssn : "(null)");
    printf("Second SSN:   %s\n", record->second_ssn[0] ? record->second_ssn : "(null)");
    printf("===================================\n");
}
