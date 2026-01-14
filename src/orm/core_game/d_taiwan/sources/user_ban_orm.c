#include "user_ban_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_QUERY_LEN 8192

/* Add new user ban record (no is auto_increment) */
int UserBan_Add(DBConnectionManager* manager, const UserBan* ban) {
    char query[MAX_QUERY_LEN];
    char escaped_detail[2048];
    char escaped_cancel[2048];
    DBQueryResult result;

    if (!manager || !ban) return -1;

    /* Escape TEXT fields to prevent SQL injection */
    snprintf(escaped_detail, sizeof(escaped_detail), "%s", ban->detail_reason);
    snprintf(escaped_cancel, sizeof(escaped_cancel), "%s", ban->cancel_reason);

    snprintf(query, sizeof(query),
        "INSERT INTO user_ban (category, m_id, ban_term, ban_reason, "
        "detail_reason, ban_date, cancel_reason, cancel_date, admin_id, "
        "status, first_ssn, second_ssn) VALUES "
        "(%u, %u, %u, %u, '%s', %u, '%s', %u, %u, %u, '%s', '%s')",
        ban->category, ban->m_id, ban->ban_term, ban->ban_reason,
        escaped_detail, ban->ban_date, escaped_cancel, ban->cancel_date,
        ban->admin_id, ban->status, ban->first_ssn, ban->second_ssn);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Get user ban by primary key */
int UserBan_Get(DBConnectionManager* manager, unsigned int no, UserBan* ban) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !ban) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, category, m_id, ban_term, ban_reason, detail_reason, "
        "ban_date, cancel_reason, cancel_date, admin_id, status, "
        "first_ssn, second_ssn FROM user_ban WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(ban, 0, sizeof(UserBan));
    ban->no = row[0] ? (unsigned int)atoi(row[0]) : 0;
    ban->category = row[1] ? (unsigned char)atoi(row[1]) : 1;
    ban->m_id = row[2] ? (unsigned int)atoi(row[2]) : 0;
    ban->ban_term = row[3] ? (unsigned short)atoi(row[3]) : 0;
    ban->ban_reason = row[4] ? (unsigned char)atoi(row[4]) : 0;
    if (row[5]) strncpy(ban->detail_reason, row[5], sizeof(ban->detail_reason) - 1);
    ban->ban_date = row[6] ? (unsigned int)atoi(row[6]) : 0;
    if (row[7]) strncpy(ban->cancel_reason, row[7], sizeof(ban->cancel_reason) - 1);
    ban->cancel_date = row[8] ? (unsigned int)atoi(row[8]) : 0;
    ban->admin_id = row[9] ? (unsigned int)atoi(row[9]) : 0;
    ban->status = row[10] ? (unsigned char)atoi(row[10]) : 0;
    if (row[11]) strncpy(ban->first_ssn, row[11], sizeof(ban->first_ssn) - 1);
    if (row[12]) strncpy(ban->second_ssn, row[12], sizeof(ban->second_ssn) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

/* Update user ban record */
int UserBan_Update(DBConnectionManager* manager, const UserBan* ban) {
    char query[MAX_QUERY_LEN];
    char escaped_detail[2048];
    char escaped_cancel[2048];
    DBQueryResult result;

    if (!manager || !ban) return -1;

    snprintf(escaped_detail, sizeof(escaped_detail), "%s", ban->detail_reason);
    snprintf(escaped_cancel, sizeof(escaped_cancel), "%s", ban->cancel_reason);

    snprintf(query, sizeof(query),
        "UPDATE user_ban SET category = %u, m_id = %u, ban_term = %u, "
        "ban_reason = %u, detail_reason = '%s', ban_date = %u, "
        "cancel_reason = '%s', cancel_date = %u, admin_id = %u, "
        "status = %u, first_ssn = '%s', second_ssn = '%s' WHERE no = %u",
        ban->category, ban->m_id, ban->ban_term, ban->ban_reason,
        escaped_detail, ban->ban_date, escaped_cancel, ban->cancel_date,
        ban->admin_id, ban->status, ban->first_ssn, ban->second_ssn, ban->no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete user ban record */
int UserBan_Delete(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM user_ban WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if user ban exists */
int UserBan_Exists(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM user_ban WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count > 0 ? 1 : 0;
}

/* Get all bans for a member ID */
int UserBan_GetByMemberId(DBConnectionManager* manager, unsigned int m_id,
                          UserBan* bans, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !bans || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, category, m_id, ban_term, ban_reason, detail_reason, "
        "ban_date, cancel_reason, cancel_date, admin_id, status, "
        "first_ssn, second_ssn FROM user_ban WHERE m_id = %u ORDER BY no DESC", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&bans[count], 0, sizeof(UserBan));
        bans[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        bans[count].category = row[1] ? (unsigned char)atoi(row[1]) : 1;
        bans[count].m_id = row[2] ? (unsigned int)atoi(row[2]) : 0;
        bans[count].ban_term = row[3] ? (unsigned short)atoi(row[3]) : 0;
        bans[count].ban_reason = row[4] ? (unsigned char)atoi(row[4]) : 0;
        if (row[5]) strncpy(bans[count].detail_reason, row[5], sizeof(bans[count].detail_reason) - 1);
        bans[count].ban_date = row[6] ? (unsigned int)atoi(row[6]) : 0;
        if (row[7]) strncpy(bans[count].cancel_reason, row[7], sizeof(bans[count].cancel_reason) - 1);
        bans[count].cancel_date = row[8] ? (unsigned int)atoi(row[8]) : 0;
        bans[count].admin_id = row[9] ? (unsigned int)atoi(row[9]) : 0;
        bans[count].status = row[10] ? (unsigned char)atoi(row[10]) : 0;
        if (row[11]) strncpy(bans[count].first_ssn, row[11], sizeof(bans[count].first_ssn) - 1);
        if (row[12]) strncpy(bans[count].second_ssn, row[12], sizeof(bans[count].second_ssn) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get bans by status */
int UserBan_GetByStatus(DBConnectionManager* manager, unsigned char status,
                        UserBan* bans, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !bans || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, category, m_id, ban_term, ban_reason, detail_reason, "
        "ban_date, cancel_reason, cancel_date, admin_id, status, "
        "first_ssn, second_ssn FROM user_ban WHERE status = %u ORDER BY no DESC", status);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&bans[count], 0, sizeof(UserBan));
        bans[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        bans[count].category = row[1] ? (unsigned char)atoi(row[1]) : 1;
        bans[count].m_id = row[2] ? (unsigned int)atoi(row[2]) : 0;
        bans[count].ban_term = row[3] ? (unsigned short)atoi(row[3]) : 0;
        bans[count].ban_reason = row[4] ? (unsigned char)atoi(row[4]) : 0;
        if (row[5]) strncpy(bans[count].detail_reason, row[5], sizeof(bans[count].detail_reason) - 1);
        bans[count].ban_date = row[6] ? (unsigned int)atoi(row[6]) : 0;
        if (row[7]) strncpy(bans[count].cancel_reason, row[7], sizeof(bans[count].cancel_reason) - 1);
        bans[count].cancel_date = row[8] ? (unsigned int)atoi(row[8]) : 0;
        bans[count].admin_id = row[9] ? (unsigned int)atoi(row[9]) : 0;
        bans[count].status = row[10] ? (unsigned char)atoi(row[10]) : 0;
        if (row[11]) strncpy(bans[count].first_ssn, row[11], sizeof(bans[count].first_ssn) - 1);
        if (row[12]) strncpy(bans[count].second_ssn, row[12], sizeof(bans[count].second_ssn) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get active ban for a member (status = 1) */
int UserBan_GetActiveByMemberId(DBConnectionManager* manager, unsigned int m_id, UserBan* ban) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !ban) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, category, m_id, ban_term, ban_reason, detail_reason, "
        "ban_date, cancel_reason, cancel_date, admin_id, status, "
        "first_ssn, second_ssn FROM user_ban WHERE m_id = %u AND status = 1 "
        "ORDER BY no DESC LIMIT 1", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(ban, 0, sizeof(UserBan));
    ban->no = row[0] ? (unsigned int)atoi(row[0]) : 0;
    ban->category = row[1] ? (unsigned char)atoi(row[1]) : 1;
    ban->m_id = row[2] ? (unsigned int)atoi(row[2]) : 0;
    ban->ban_term = row[3] ? (unsigned short)atoi(row[3]) : 0;
    ban->ban_reason = row[4] ? (unsigned char)atoi(row[4]) : 0;
    if (row[5]) strncpy(ban->detail_reason, row[5], sizeof(ban->detail_reason) - 1);
    ban->ban_date = row[6] ? (unsigned int)atoi(row[6]) : 0;
    if (row[7]) strncpy(ban->cancel_reason, row[7], sizeof(ban->cancel_reason) - 1);
    ban->cancel_date = row[8] ? (unsigned int)atoi(row[8]) : 0;
    ban->admin_id = row[9] ? (unsigned int)atoi(row[9]) : 0;
    ban->status = row[10] ? (unsigned char)atoi(row[10]) : 0;
    if (row[11]) strncpy(ban->first_ssn, row[11], sizeof(ban->first_ssn) - 1);
    if (row[12]) strncpy(ban->second_ssn, row[12], sizeof(ban->second_ssn) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

/* Update ban status */
int UserBan_UpdateStatus(DBConnectionManager* manager, unsigned int no, unsigned char new_status) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE user_ban SET status = %u WHERE no = %u", new_status, no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Cancel ban with reason */
int UserBan_CancelBan(DBConnectionManager* manager, unsigned int no,
                      const char* cancel_reason, unsigned int admin_id) {
    char query[MAX_QUERY_LEN];
    char escaped_reason[2048];
    DBQueryResult result;
    unsigned int current_time;

    if (!manager || !cancel_reason) return -1;

    current_time = (unsigned int)time(NULL);
    snprintf(escaped_reason, sizeof(escaped_reason), "%s", cancel_reason);

    snprintf(query, sizeof(query),
        "UPDATE user_ban SET cancel_reason = '%s', cancel_date = %u, "
        "admin_id = %u, status = 2 WHERE no = %u",
        escaped_reason, current_time, admin_id, no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Get bans by first SSN */
int UserBan_GetByFirstSSN(DBConnectionManager* manager, const char* first_ssn,
                          UserBan* bans, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !first_ssn || !bans || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, category, m_id, ban_term, ban_reason, detail_reason, "
        "ban_date, cancel_reason, cancel_date, admin_id, status, "
        "first_ssn, second_ssn FROM user_ban WHERE first_ssn = '%s' ORDER BY no DESC",
        first_ssn);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&bans[count], 0, sizeof(UserBan));
        bans[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        bans[count].category = row[1] ? (unsigned char)atoi(row[1]) : 1;
        bans[count].m_id = row[2] ? (unsigned int)atoi(row[2]) : 0;
        bans[count].ban_term = row[3] ? (unsigned short)atoi(row[3]) : 0;
        bans[count].ban_reason = row[4] ? (unsigned char)atoi(row[4]) : 0;
        if (row[5]) strncpy(bans[count].detail_reason, row[5], sizeof(bans[count].detail_reason) - 1);
        bans[count].ban_date = row[6] ? (unsigned int)atoi(row[6]) : 0;
        if (row[7]) strncpy(bans[count].cancel_reason, row[7], sizeof(bans[count].cancel_reason) - 1);
        bans[count].cancel_date = row[8] ? (unsigned int)atoi(row[8]) : 0;
        bans[count].admin_id = row[9] ? (unsigned int)atoi(row[9]) : 0;
        bans[count].status = row[10] ? (unsigned char)atoi(row[10]) : 0;
        if (row[11]) strncpy(bans[count].first_ssn, row[11], sizeof(bans[count].first_ssn) - 1);
        if (row[12]) strncpy(bans[count].second_ssn, row[12], sizeof(bans[count].second_ssn) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Print ban information */
void UserBan_PrintInfo(const UserBan* ban) {
    if (!ban) return;

    printf("User Ban Record:\n");
    printf("  No: %u\n", ban->no);
    printf("  Category: %u\n", ban->category);
    printf("  Member ID: %u\n", ban->m_id);
    printf("  Ban Term: %u\n", ban->ban_term);
    printf("  Ban Reason: %u\n", ban->ban_reason);
    printf("  Detail Reason: %s\n", ban->detail_reason);
    printf("  Ban Date: %u\n", ban->ban_date);
    printf("  Cancel Reason: %s\n", ban->cancel_reason);
    printf("  Cancel Date: %u\n", ban->cancel_date);
    printf("  Admin ID: %u\n", ban->admin_id);
    printf("  Status: %u\n", ban->status);
    printf("  First SSN: %s\n", ban->first_ssn);
    printf("  Second SSN: %s\n", ban->second_ssn);
}
