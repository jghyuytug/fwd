#include "member_punish_info_history_2012_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberPunishInfoHistory2012_Add(DBConnectionManager* manager,
                                     const MemberPunishInfoHistory2012* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    /* Build INSERT query - no need to specify 'no' (auto_increment) */
    snprintf(query, sizeof(query),
        "INSERT INTO member_punish_info_history_2012 "
        "(m_id, punish_type, occ_time, punish_value, apply_flag, "
        "start_time, end_time, admin_id, reason, is_kicked, first_ssn, second_ssn) "
        "VALUES (%d, %d, %s, %d, %d, %s, %s, %s, %s, %s, %s, %s)",
        record->m_id,
        record->punish_type,
        record->occ_time[0] ? (snprintf(query + strlen(query), sizeof(query) - strlen(query), "'%s'", record->occ_time), "") : "NOW()",
        record->punish_value,
        record->apply_flag,
        record->start_time[0] ? (snprintf(query + strlen(query), sizeof(query) - strlen(query), "'%s'", record->start_time), "") : "NOW()",
        record->end_time[0] ? (snprintf(query + strlen(query), sizeof(query) - strlen(query), "'%s'", record->end_time), "") : "NOW()",
        record->admin_id[0] ? (snprintf(query + strlen(query), sizeof(query) - strlen(query), "'%s'", record->admin_id), "") : "NULL",
        record->reason[0] ? (snprintf(query + strlen(query), sizeof(query) - strlen(query), "'%s'", record->reason), "") : "NULL",
        record->is_kicked >= 0 ? (snprintf(query + strlen(query), sizeof(query) - strlen(query), "%d", record->is_kicked), "") : "NULL",
        record->first_ssn[0] ? (snprintf(query + strlen(query), sizeof(query) - strlen(query), "'%s'", record->first_ssn), "") : "NULL",
        record->second_ssn[0] ? (snprintf(query + strlen(query), sizeof(query) - strlen(query), "'%s'", record->second_ssn), "") : "NULL");

    /* Simpler approach - build complete query */
    memset(query, 0, sizeof(query));
    int offset = snprintf(query, sizeof(query),
        "INSERT INTO member_punish_info_history_2012 "
        "(m_id, punish_type, occ_time, punish_value, apply_flag, start_time, end_time");

    if (record->admin_id[0]) offset += snprintf(query + offset, sizeof(query) - offset, ", admin_id");
    if (record->reason[0]) offset += snprintf(query + offset, sizeof(query) - offset, ", reason");
    if (record->is_kicked >= 0) offset += snprintf(query + offset, sizeof(query) - offset, ", is_kicked");
    if (record->first_ssn[0]) offset += snprintf(query + offset, sizeof(query) - offset, ", first_ssn");
    if (record->second_ssn[0]) offset += snprintf(query + offset, sizeof(query) - offset, ", second_ssn");

    offset += snprintf(query + offset, sizeof(query) - offset, ") VALUES (%d, %d, ",
        record->m_id, record->punish_type);

    if (record->occ_time[0]) {
        offset += snprintf(query + offset, sizeof(query) - offset, "'%s'", record->occ_time);
    } else {
        offset += snprintf(query + offset, sizeof(query) - offset, "NOW()");
    }

    offset += snprintf(query + offset, sizeof(query) - offset, ", %d, %d, ",
        record->punish_value, record->apply_flag);

    if (record->start_time[0]) {
        offset += snprintf(query + offset, sizeof(query) - offset, "'%s'", record->start_time);
    } else {
        offset += snprintf(query + offset, sizeof(query) - offset, "NOW()");
    }

    offset += snprintf(query + offset, sizeof(query) - offset, ", ");

    if (record->end_time[0]) {
        offset += snprintf(query + offset, sizeof(query) - offset, "'%s'", record->end_time);
    } else {
        offset += snprintf(query + offset, sizeof(query) - offset, "NOW()");
    }

    if (record->admin_id[0]) {
        offset += snprintf(query + offset, sizeof(query) - offset, ", '%s'", record->admin_id);
    }
    if (record->reason[0]) {
        offset += snprintf(query + offset, sizeof(query) - offset, ", '%s'", record->reason);
    }
    if (record->is_kicked >= 0) {
        offset += snprintf(query + offset, sizeof(query) - offset, ", %d", record->is_kicked);
    }
    if (record->first_ssn[0]) {
        offset += snprintf(query + offset, sizeof(query) - offset, ", '%s'", record->first_ssn);
    }
    if (record->second_ssn[0]) {
        offset += snprintf(query + offset, sizeof(query) - offset, ", '%s'", record->second_ssn);
    }

    snprintf(query + offset, sizeof(query) - offset, ")");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoHistory2012_Get(DBConnectionManager* manager,
                                     int no,
                                     MemberPunishInfoHistory2012* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, punish_type, occ_time, punish_value, apply_flag, "
        "start_time, end_time, admin_id, reason, is_kicked, first_ssn, second_ssn "
        "FROM member_punish_info_history_2012 WHERE no = %d", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberPunishInfoHistory2012));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->punish_type = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->occ_time, row[3], sizeof(record->occ_time) - 1);
    record->punish_value = row[4] ? atoi(row[4]) : 0;
    record->apply_flag = row[5] ? (signed char)atoi(row[5]) : 0;
    if (row[6]) strncpy(record->start_time, row[6], sizeof(record->start_time) - 1);
    if (row[7]) strncpy(record->end_time, row[7], sizeof(record->end_time) - 1);
    if (row[8]) strncpy(record->admin_id, row[8], sizeof(record->admin_id) - 1);
    if (row[9]) strncpy(record->reason, row[9], sizeof(record->reason) - 1);
    record->is_kicked = row[10] ? (signed char)atoi(row[10]) : -1;
    if (row[11]) strncpy(record->first_ssn, row[11], sizeof(record->first_ssn) - 1);
    if (row[12]) strncpy(record->second_ssn, row[12], sizeof(record->second_ssn) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoHistory2012_Update(DBConnectionManager* manager,
                                        const MemberPunishInfoHistory2012* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    int offset = 0;

    if (!manager || !record) return -1;

    offset = snprintf(query, sizeof(query),
        "UPDATE member_punish_info_history_2012 SET "
        "m_id = %d, punish_type = %d, occ_time = '%s', punish_value = %d, "
        "apply_flag = %d, start_time = '%s', end_time = '%s'",
        record->m_id, record->punish_type, record->occ_time, record->punish_value,
        record->apply_flag, record->start_time, record->end_time);

    if (record->admin_id[0]) {
        offset += snprintf(query + offset, sizeof(query) - offset, ", admin_id = '%s'", record->admin_id);
    } else {
        offset += snprintf(query + offset, sizeof(query) - offset, ", admin_id = NULL");
    }

    if (record->reason[0]) {
        offset += snprintf(query + offset, sizeof(query) - offset, ", reason = '%s'", record->reason);
    } else {
        offset += snprintf(query + offset, sizeof(query) - offset, ", reason = NULL");
    }

    if (record->is_kicked >= 0) {
        offset += snprintf(query + offset, sizeof(query) - offset, ", is_kicked = %d", record->is_kicked);
    } else {
        offset += snprintf(query + offset, sizeof(query) - offset, ", is_kicked = NULL");
    }

    if (record->first_ssn[0]) {
        offset += snprintf(query + offset, sizeof(query) - offset, ", first_ssn = '%s'", record->first_ssn);
    } else {
        offset += snprintf(query + offset, sizeof(query) - offset, ", first_ssn = NULL");
    }

    if (record->second_ssn[0]) {
        offset += snprintf(query + offset, sizeof(query) - offset, ", second_ssn = '%s'", record->second_ssn);
    } else {
        offset += snprintf(query + offset, sizeof(query) - offset, ", second_ssn = NULL");
    }

    snprintf(query + offset, sizeof(query) - offset, " WHERE no = %d", record->no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoHistory2012_Delete(DBConnectionManager* manager, int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_punish_info_history_2012 WHERE no = %d", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoHistory2012_GetByMId(DBConnectionManager* manager,
                                          int m_id,
                                          MemberPunishInfoHistory2012* records,
                                          int max_count,
                                          int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, punish_type, occ_time, punish_value, apply_flag, "
        "start_time, end_time, admin_id, reason, is_kicked, first_ssn, second_ssn "
        "FROM member_punish_info_history_2012 WHERE m_id = %d ORDER BY no DESC", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberPunishInfoHistory2012));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].punish_type = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].occ_time, row[3], sizeof(records[count].occ_time) - 1);
        records[count].punish_value = row[4] ? atoi(row[4]) : 0;
        records[count].apply_flag = row[5] ? (signed char)atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].start_time, row[6], sizeof(records[count].start_time) - 1);
        if (row[7]) strncpy(records[count].end_time, row[7], sizeof(records[count].end_time) - 1);
        if (row[8]) strncpy(records[count].admin_id, row[8], sizeof(records[count].admin_id) - 1);
        if (row[9]) strncpy(records[count].reason, row[9], sizeof(records[count].reason) - 1);
        records[count].is_kicked = row[10] ? (signed char)atoi(row[10]) : -1;
        if (row[11]) strncpy(records[count].first_ssn, row[11], sizeof(records[count].first_ssn) - 1);
        if (row[12]) strncpy(records[count].second_ssn, row[12], sizeof(records[count].second_ssn) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoHistory2012_GetByTimeRange(DBConnectionManager* manager,
                                                 const char* start_time,
                                                 const char* end_time,
                                                 MemberPunishInfoHistory2012* records,
                                                 int max_count,
                                                 int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !start_time || !end_time || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, punish_type, occ_time, punish_value, apply_flag, "
        "start_time, end_time, admin_id, reason, is_kicked, first_ssn, second_ssn "
        "FROM member_punish_info_history_2012 "
        "WHERE occ_time >= '%s' AND occ_time <= '%s' ORDER BY occ_time DESC",
        start_time, end_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberPunishInfoHistory2012));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].punish_type = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].occ_time, row[3], sizeof(records[count].occ_time) - 1);
        records[count].punish_value = row[4] ? atoi(row[4]) : 0;
        records[count].apply_flag = row[5] ? (signed char)atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].start_time, row[6], sizeof(records[count].start_time) - 1);
        if (row[7]) strncpy(records[count].end_time, row[7], sizeof(records[count].end_time) - 1);
        if (row[8]) strncpy(records[count].admin_id, row[8], sizeof(records[count].admin_id) - 1);
        if (row[9]) strncpy(records[count].reason, row[9], sizeof(records[count].reason) - 1);
        records[count].is_kicked = row[10] ? (signed char)atoi(row[10]) : -1;
        if (row[11]) strncpy(records[count].first_ssn, row[11], sizeof(records[count].first_ssn) - 1);
        if (row[12]) strncpy(records[count].second_ssn, row[12], sizeof(records[count].second_ssn) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoHistory2012_GetByPunishType(DBConnectionManager* manager,
                                                  int punish_type,
                                                  MemberPunishInfoHistory2012* records,
                                                  int max_count,
                                                  int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, punish_type, occ_time, punish_value, apply_flag, "
        "start_time, end_time, admin_id, reason, is_kicked, first_ssn, second_ssn "
        "FROM member_punish_info_history_2012 WHERE punish_type = %d ORDER BY no DESC",
        punish_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberPunishInfoHistory2012));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].punish_type = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].occ_time, row[3], sizeof(records[count].occ_time) - 1);
        records[count].punish_value = row[4] ? atoi(row[4]) : 0;
        records[count].apply_flag = row[5] ? (signed char)atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].start_time, row[6], sizeof(records[count].start_time) - 1);
        if (row[7]) strncpy(records[count].end_time, row[7], sizeof(records[count].end_time) - 1);
        if (row[8]) strncpy(records[count].admin_id, row[8], sizeof(records[count].admin_id) - 1);
        if (row[9]) strncpy(records[count].reason, row[9], sizeof(records[count].reason) - 1);
        records[count].is_kicked = row[10] ? (signed char)atoi(row[10]) : -1;
        if (row[11]) strncpy(records[count].first_ssn, row[11], sizeof(records[count].first_ssn) - 1);
        if (row[12]) strncpy(records[count].second_ssn, row[12], sizeof(records[count].second_ssn) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoHistory2012_CountByMId(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count = 0;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_punish_info_history_2012 WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        count = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberPunishInfoHistory2012_UpdateApplyFlag(DBConnectionManager* manager,
                                                  int no,
                                                  signed char apply_flag) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_punish_info_history_2012 SET apply_flag = %d WHERE no = %d",
        apply_flag, no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

void MemberPunishInfoHistory2012_PrintInfo(const MemberPunishInfoHistory2012* record) {
    if (!record) {
        printf("NULL record\n");
        return;
    }

    printf("Member Punish Info History 2012:\n");
    printf("  No: %d\n", record->no);
    printf("  Member ID: %d\n", record->m_id);
    printf("  Punish Type: %d\n", record->punish_type);
    printf("  Occurrence Time: %s\n", record->occ_time);
    printf("  Punish Value: %d\n", record->punish_value);
    printf("  Apply Flag: %d\n", record->apply_flag);
    printf("  Start Time: %s\n", record->start_time);
    printf("  End Time: %s\n", record->end_time);
    printf("  Admin ID: %s\n", record->admin_id[0] ? record->admin_id : "(null)");
    printf("  Reason: %s\n", record->reason[0] ? record->reason : "(null)");
    printf("  Is Kicked: %d\n", record->is_kicked);
    printf("  First SSN: %s\n", record->first_ssn[0] ? record->first_ssn : "(null)");
    printf("  Second SSN: %s\n", record->second_ssn[0] ? record->second_ssn : "(null)");
}
