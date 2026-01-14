/**
 * DNF Game Server - MemberInfo ORM Implementation
 * Database: test_d_taiwan
 * Table: member_info
 */

#include "member_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ========================================================================
 * Utility Functions
 * ======================================================================== */

void MemberInfo_Init(MemberInfo* member) {
    if (!member) return;
    memset(member, 0, sizeof(MemberInfo));
    member->m_id = 0;
    member->reg_date = 0;
    member->state = 1;
    member->q_no = 0;
    member->ssn_check = 0;
    member->slot = 8;
    member->hangame_flag = 0;
    member->hanmon_flag = 0;
    member->m_type = 0;
    strcpy(member->email_yn, "y");
}

void MemberInfo_Print(const MemberInfo* member) {
    if (!member) return;

    printf("================================================================================\n");
    printf("MemberInfo Details:\n");
    printf("================================================================================\n");
    printf("  Member ID:         %d\n", member->m_id);
    printf("  User ID:           %s\n", member->user_id);
    printf("  User Name:         %s\n", member->user_name);
    printf("  First SSN:         %s\n", member->first_ssn);
    printf("  Second SSN:        %s\n", member->second_ssn);
    printf("  Password:          %s\n", member->passwd);
    printf("  Mobile No:         %s\n", member->mobile_no);
    printf("  Registration Date: %d\n", member->reg_date);
    printf("  Email:             %s\n", member->email);
    printf("  Question No:       %d\n", member->q_no);
    printf("  Question Answer:   %s\n", member->q_answer);
    printf("  Update Date:       %s\n", member->updt_date);
    printf("  State:             %d\n", member->state);
    printf("  Nickname:          %s\n", member->nickname);
    printf("  Email YN:          %s\n", member->email_yn);
    printf("  SSN Check:         %d\n", member->ssn_check);
    printf("  Slot:              %u\n", member->slot);
    printf("  Last Play Time:    %s\n", member->last_play_time);
    printf("  Hangame Flag:      %d\n", member->hangame_flag);
    printf("  Hanmon Flag:       %d\n", member->hanmon_flag);
    printf("  Member Type:       %d\n", member->m_type);
    printf("================================================================================\n");
}

int MemberInfo_Copy(MemberInfo* dest, const MemberInfo* src) {
    if (!dest || !src) return -1;
    memcpy(dest, src, sizeof(MemberInfo));
    return 0;
}

/* ========================================================================
 * CRUD Operations
 * ======================================================================== */

int MemberInfo_Create(DBConnectionManager* manager, MemberInfo* member) {
    char query[2048];
    DBQueryResult result;
    int ret;

    if (!manager || !member) {
        printf("[MemberInfo ORM] Error: NULL parameter\n");
        return -1;
    }

    snprintf(query, sizeof(query),
             "INSERT INTO member_info (user_id, user_name, first_ssn, second_ssn, "
             "passwd, mobile_no, reg_date, email, q_no, q_answer, state, nickname, "
             "email_yn, ssn_check, slot, last_play_time, hangame_flag, hanmon_flag, m_type) "
             "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', %d, '%s', %d, '%s', %d, '%s', "
             "'%s', %d, %u, '%s', %d, %d, %d)",
             member->user_id, member->user_name, member->first_ssn, member->second_ssn,
             member->passwd, member->mobile_no, member->reg_date, member->email,
             member->q_no, member->q_answer, member->state, member->nickname,
             member->email_yn, member->ssn_check, member->slot, member->last_play_time,
             member->hangame_flag, member->hanmon_flag, member->m_type);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret > 0) {
        printf("[MemberInfo ORM] Created member: %s (ID will be auto-generated)\n", member->user_id);
        ret = 0;
    } else {
        printf("[MemberInfo ORM] Failed to create member: %s\n", member->user_id);
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberInfo_GetByID(DBConnectionManager* manager, int m_id, MemberInfo* member) {
    char query[512];
    char* values[21];
    DBQueryResult result;
    int ret;

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
             "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
             "reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, "
             "ssn_check, slot, last_play_time, hangame_flag, hanmon_flag, m_type "
             "FROM member_info WHERE m_id = %d",
             m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* Parse fields */
    member->m_id = atoi(values[0]);
    strncpy(member->user_id, values[1] ? values[1] : "", sizeof(member->user_id) - 1);
    strncpy(member->user_name, values[2] ? values[2] : "", sizeof(member->user_name) - 1);
    strncpy(member->first_ssn, values[3] ? values[3] : "", sizeof(member->first_ssn) - 1);
    strncpy(member->second_ssn, values[4] ? values[4] : "", sizeof(member->second_ssn) - 1);
    strncpy(member->passwd, values[5] ? values[5] : "", sizeof(member->passwd) - 1);
    strncpy(member->mobile_no, values[6] ? values[6] : "", sizeof(member->mobile_no) - 1);
    member->reg_date = atoi(values[7]);
    strncpy(member->email, values[8] ? values[8] : "", sizeof(member->email) - 1);
    member->q_no = atoi(values[9]);
    strncpy(member->q_answer, values[10] ? values[10] : "", sizeof(member->q_answer) - 1);
    strncpy(member->updt_date, values[11] ? values[11] : "", sizeof(member->updt_date) - 1);
    member->state = atoi(values[12]);
    strncpy(member->nickname, values[13] ? values[13] : "", sizeof(member->nickname) - 1);
    strncpy(member->email_yn, values[14] ? values[14] : "y", sizeof(member->email_yn) - 1);
    member->ssn_check = atoi(values[15]);
    member->slot = (unsigned int)atoi(values[16]);
    strncpy(member->last_play_time, values[17] ? values[17] : "", sizeof(member->last_play_time) - 1);
    member->hangame_flag = atoi(values[18]);
    member->hanmon_flag = atoi(values[19]);
    member->m_type = atoi(values[20]);

    DBQueryResult_Free(&result);
    return 0;
}

int MemberInfo_GetByUserID(DBConnectionManager* manager, const char* user_id, MemberInfo* member) {
    char query[512];
    char* values[21];
    DBQueryResult result;
    int ret;

    if (!manager || !user_id || !member) return -1;

    snprintf(query, sizeof(query),
             "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
             "reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, "
             "ssn_check, slot, last_play_time, hangame_flag, hanmon_flag, m_type "
             "FROM member_info WHERE user_id = '%s'",
             user_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* Parse fields (same as GetByID) */
    member->m_id = atoi(values[0]);
    strncpy(member->user_id, values[1] ? values[1] : "", sizeof(member->user_id) - 1);
    strncpy(member->user_name, values[2] ? values[2] : "", sizeof(member->user_name) - 1);
    strncpy(member->first_ssn, values[3] ? values[3] : "", sizeof(member->first_ssn) - 1);
    strncpy(member->second_ssn, values[4] ? values[4] : "", sizeof(member->second_ssn) - 1);
    strncpy(member->passwd, values[5] ? values[5] : "", sizeof(member->passwd) - 1);
    strncpy(member->mobile_no, values[6] ? values[6] : "", sizeof(member->mobile_no) - 1);
    member->reg_date = atoi(values[7]);
    strncpy(member->email, values[8] ? values[8] : "", sizeof(member->email) - 1);
    member->q_no = atoi(values[9]);
    strncpy(member->q_answer, values[10] ? values[10] : "", sizeof(member->q_answer) - 1);
    strncpy(member->updt_date, values[11] ? values[11] : "", sizeof(member->updt_date) - 1);
    member->state = atoi(values[12]);
    strncpy(member->nickname, values[13] ? values[13] : "", sizeof(member->nickname) - 1);
    strncpy(member->email_yn, values[14] ? values[14] : "y", sizeof(member->email_yn) - 1);
    member->ssn_check = atoi(values[15]);
    member->slot = (unsigned int)atoi(values[16]);
    strncpy(member->last_play_time, values[17] ? values[17] : "", sizeof(member->last_play_time) - 1);
    member->hangame_flag = atoi(values[18]);
    member->hanmon_flag = atoi(values[19]);
    member->m_type = atoi(values[20]);

    DBQueryResult_Free(&result);
    return 0;
}

int MemberInfo_GetByNickname(DBConnectionManager* manager, const char* nickname, MemberInfo* member) {
    char query[512];
    char* values[21];
    DBQueryResult result;
    int ret;

    if (!manager || !nickname || !member) return -1;

    snprintf(query, sizeof(query),
             "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
             "reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, "
             "ssn_check, slot, last_play_time, hangame_flag, hanmon_flag, m_type "
             "FROM member_info WHERE nickname = '%s'",
             nickname);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* Parse fields */
    member->m_id = atoi(values[0]);
    strncpy(member->user_id, values[1] ? values[1] : "", sizeof(member->user_id) - 1);
    strncpy(member->user_name, values[2] ? values[2] : "", sizeof(member->user_name) - 1);
    strncpy(member->first_ssn, values[3] ? values[3] : "", sizeof(member->first_ssn) - 1);
    strncpy(member->second_ssn, values[4] ? values[4] : "", sizeof(member->second_ssn) - 1);
    strncpy(member->passwd, values[5] ? values[5] : "", sizeof(member->passwd) - 1);
    strncpy(member->mobile_no, values[6] ? values[6] : "", sizeof(member->mobile_no) - 1);
    member->reg_date = atoi(values[7]);
    strncpy(member->email, values[8] ? values[8] : "", sizeof(member->email) - 1);
    member->q_no = atoi(values[9]);
    strncpy(member->q_answer, values[10] ? values[10] : "", sizeof(member->q_answer) - 1);
    strncpy(member->updt_date, values[11] ? values[11] : "", sizeof(member->updt_date) - 1);
    member->state = atoi(values[12]);
    strncpy(member->nickname, values[13] ? values[13] : "", sizeof(member->nickname) - 1);
    strncpy(member->email_yn, values[14] ? values[14] : "y", sizeof(member->email_yn) - 1);
    member->ssn_check = atoi(values[15]);
    member->slot = (unsigned int)atoi(values[16]);
    strncpy(member->last_play_time, values[17] ? values[17] : "", sizeof(member->last_play_time) - 1);
    member->hangame_flag = atoi(values[18]);
    member->hanmon_flag = atoi(values[19]);
    member->m_type = atoi(values[20]);

    DBQueryResult_Free(&result);
    return 0;
}

int MemberInfo_Update(DBConnectionManager* manager, MemberInfo* member) {
    char query[2048];
    DBQueryResult result;
    int ret;

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
             "UPDATE member_info SET user_id='%s', user_name='%s', first_ssn='%s', "
             "second_ssn='%s', passwd='%s', mobile_no='%s', reg_date=%d, email='%s', "
             "q_no=%d, q_answer='%s', state=%d, nickname='%s', email_yn='%s', "
             "ssn_check=%d, slot=%u, last_play_time='%s', hangame_flag=%d, "
             "hanmon_flag=%d, m_type=%d WHERE m_id=%d",
             member->user_id, member->user_name, member->first_ssn, member->second_ssn,
             member->passwd, member->mobile_no, member->reg_date, member->email,
             member->q_no, member->q_answer, member->state, member->nickname,
             member->email_yn, member->ssn_check, member->slot, member->last_play_time,
             member->hangame_flag, member->hanmon_flag, member->m_type, member->m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    /* Query execution error */
    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* Update succeeded (even if 0 rows affected - idempotent) */
    if (ret > 0) {
        printf("[MemberInfo ORM] Updated member: %d\n", member->m_id);
    }

    DBQueryResult_Free(&result);
    return 0;  /* Success even if 0 rows affected */
}

int MemberInfo_Delete(DBConnectionManager* manager, int m_id) {
    char query[256];
    DBQueryResult result;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM member_info WHERE m_id = %d", m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret > 0) {
        printf("[MemberInfo ORM] Deleted member: %d\n", m_id);
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

/* ========================================================================
 * Auxiliary Query Functions
 * ======================================================================== */

bool MemberInfo_Exists(DBConnectionManager* manager, int m_id) {
    char query[256];
    char* values[1];
    DBQueryResult result;
    int ret;

    if (!manager) return false;

    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM member_info WHERE m_id = %d", m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return count > 0;
    }

    DBQueryResult_Free(&result);
    return false;
}

bool MemberInfo_UserIDExists(DBConnectionManager* manager, const char* user_id) {
    char query[256];
    char* values[1];
    DBQueryResult result;
    int ret;

    if (!manager || !user_id) return false;

    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM member_info WHERE user_id = '%s'", user_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return count > 0;
    }

    DBQueryResult_Free(&result);
    return false;
}

bool MemberInfo_NicknameExists(DBConnectionManager* manager, const char* nickname) {
    char query[256];
    char* values[1];
    DBQueryResult result;
    int ret;

    if (!manager || !nickname) return false;

    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM member_info WHERE nickname = '%s'", nickname);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return count > 0;
    }

    DBQueryResult_Free(&result);
    return false;
}

bool MemberInfo_VerifyPassword(DBConnectionManager* manager, const char* user_id, const char* passwd) {
    char query[512];
    char* values[1];
    DBQueryResult result;
    int ret;

    if (!manager || !user_id || !passwd) return false;

    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM member_info WHERE user_id = '%s' AND passwd = '%s'",
             user_id, passwd);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return count > 0;
    }

    DBQueryResult_Free(&result);
    return false;
}

int MemberInfo_UpdatePassword(DBConnectionManager* manager, int m_id, const char* new_passwd) {
    char query[512];
    DBQueryResult result;
    int ret;

    if (!manager || !new_passwd) return -1;

    snprintf(query, sizeof(query),
             "UPDATE member_info SET passwd = '%s' WHERE m_id = %d",
             new_passwd, m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    /* Query execution error */
    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* Update succeeded (even if 0 rows affected - idempotent) */
    if (ret > 0) {
        printf("[MemberInfo ORM] Updated password for member: %d\n", m_id);
    }

    DBQueryResult_Free(&result);
    return 0;  /* Success even if 0 rows affected */
}

int MemberInfo_UpdateState(DBConnectionManager* manager, int m_id, int state) {
    char query[256];
    DBQueryResult result;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE member_info SET state = %d WHERE m_id = %d", state, m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret > 0) {
        printf("[MemberInfo ORM] Updated state for member: %d\n", m_id);
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberInfo_UpdateLastPlayTime(DBConnectionManager* manager, int m_id) {
    char query[256];
    DBQueryResult result;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE member_info SET last_play_time = NOW() WHERE m_id = %d", m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret > 0) {
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberInfo_GetCount(DBConnectionManager* manager) {
    char query[128];
    char* values[1];
    DBQueryResult result;
    int ret;
    int count = -1;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_info");

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberInfo_GetActiveCount(DBConnectionManager* manager) {
    char query[128];
    char* values[1];
    DBQueryResult result;
    int ret;
    int count = -1;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_info WHERE state = 1");

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberInfo_SearchByUserName(DBConnectionManager* manager, const char* user_name, MemberInfo* members, int max_count) {
    char query[512];
    char* values[21];
    DBQueryResult result;
    int ret;
    int count = 0;

    if (!manager || !user_name || !members || max_count <= 0) return -1;

    snprintf(query, sizeof(query),
             "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
             "reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, "
             "ssn_check, slot, last_play_time, hangame_flag, hanmon_flag, m_type "
             "FROM member_info WHERE user_name LIKE '%%%s%%' LIMIT %d",
             user_name, max_count);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        MemberInfo* member = &members[count];

        member->m_id = atoi(values[0]);
        strncpy(member->user_id, values[1] ? values[1] : "", sizeof(member->user_id) - 1);
        strncpy(member->user_name, values[2] ? values[2] : "", sizeof(member->user_name) - 1);
        strncpy(member->first_ssn, values[3] ? values[3] : "", sizeof(member->first_ssn) - 1);
        strncpy(member->second_ssn, values[4] ? values[4] : "", sizeof(member->second_ssn) - 1);
        strncpy(member->passwd, values[5] ? values[5] : "", sizeof(member->passwd) - 1);
        strncpy(member->mobile_no, values[6] ? values[6] : "", sizeof(member->mobile_no) - 1);
        member->reg_date = atoi(values[7]);
        strncpy(member->email, values[8] ? values[8] : "", sizeof(member->email) - 1);
        member->q_no = atoi(values[9]);
        strncpy(member->q_answer, values[10] ? values[10] : "", sizeof(member->q_answer) - 1);
        strncpy(member->updt_date, values[11] ? values[11] : "", sizeof(member->updt_date) - 1);
        member->state = atoi(values[12]);
        strncpy(member->nickname, values[13] ? values[13] : "", sizeof(member->nickname) - 1);
        strncpy(member->email_yn, values[14] ? values[14] : "y", sizeof(member->email_yn) - 1);
        member->ssn_check = atoi(values[15]);
        member->slot = (unsigned int)atoi(values[16]);
        strncpy(member->last_play_time, values[17] ? values[17] : "", sizeof(member->last_play_time) - 1);
        member->hangame_flag = atoi(values[18]);
        member->hanmon_flag = atoi(values[19]);
        member->m_type = atoi(values[20]);

        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberInfo_GetRecentlyRegistered(DBConnectionManager* manager, MemberInfo* members, int max_count) {
    char query[512];
    char* values[21];
    DBQueryResult result;
    int ret;
    int count = 0;

    if (!manager || !members || max_count <= 0) return -1;

    snprintf(query, sizeof(query),
             "SELECT m_id, user_id, user_name, first_ssn, second_ssn, passwd, mobile_no, "
             "reg_date, email, q_no, q_answer, updt_date, state, nickname, email_yn, "
             "ssn_check, slot, last_play_time, hangame_flag, hanmon_flag, m_type "
             "FROM member_info ORDER BY reg_date DESC LIMIT %d",
             max_count);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        MemberInfo* member = &members[count];

        member->m_id = atoi(values[0]);
        strncpy(member->user_id, values[1] ? values[1] : "", sizeof(member->user_id) - 1);
        strncpy(member->user_name, values[2] ? values[2] : "", sizeof(member->user_name) - 1);
        strncpy(member->first_ssn, values[3] ? values[3] : "", sizeof(member->first_ssn) - 1);
        strncpy(member->second_ssn, values[4] ? values[4] : "", sizeof(member->second_ssn) - 1);
        strncpy(member->passwd, values[5] ? values[5] : "", sizeof(member->passwd) - 1);
        strncpy(member->mobile_no, values[6] ? values[6] : "", sizeof(member->mobile_no) - 1);
        member->reg_date = atoi(values[7]);
        strncpy(member->email, values[8] ? values[8] : "", sizeof(member->email) - 1);
        member->q_no = atoi(values[9]);
        strncpy(member->q_answer, values[10] ? values[10] : "", sizeof(member->q_answer) - 1);
        strncpy(member->updt_date, values[11] ? values[11] : "", sizeof(member->updt_date) - 1);
        member->state = atoi(values[12]);
        strncpy(member->nickname, values[13] ? values[13] : "", sizeof(member->nickname) - 1);
        strncpy(member->email_yn, values[14] ? values[14] : "y", sizeof(member->email_yn) - 1);
        member->ssn_check = atoi(values[15]);
        member->slot = (unsigned int)atoi(values[16]);
        strncpy(member->last_play_time, values[17] ? values[17] : "", sizeof(member->last_play_time) - 1);
        member->hangame_flag = atoi(values[18]);
        member->hanmon_flag = atoi(values[19]);
        member->m_type = atoi(values[20]);

        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}
