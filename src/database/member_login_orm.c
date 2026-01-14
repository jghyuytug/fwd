/**
 * DNF Game Server - MemberLogin ORM Implementation
 * Database: test_taiwan_login
 * Table: member_login
 */

#include "member_login_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ========================================================================
 * Utility Functions
 * ======================================================================== */

void MemberLogin_Init(MemberLogin* login) {
    if (!login) return;
    memset(login, 0, sizeof(MemberLogin));
    login->m_id = 0;
    login->login_time = 0;
    login->expire_time = 0;
    login->last_play_time = 0;
    login->total_account_fail = 0;
    login->account_fail = 0;
    login->report_cnt = 0;
    login->reliable_flag = 0;
    login->trade_gold_daily = 0;
    login->last_gift_time = 0;
    login->gift_cnt = 0;
    strcpy(login->login_ip, "");
    login->security_flag = 0;
    login->power_side = 0;
    login->dungeon_gain_gold = 0;
    login->school_id = 0;
    login->rating = 0.0f;
    login->cleanpad_point = 0;
    strcpy(login->tutorial_skipable, "0");
    login->event_charac_flag = 0;
    login->garena_token_key = 0;
}

void MemberLogin_Print(const MemberLogin* login) {
    if (!login) return;

    printf("================================================================================\n");
    printf("MemberLogin Details:\n");
    printf("================================================================================\n");
    printf("  Member ID:         %d\n", login->m_id);
    printf("  Login Time:        %u\n", login->login_time);
    printf("  Expire Time:       %u\n", login->expire_time);
    printf("  Last Play Time:    %u\n", login->last_play_time);
    printf("  Total Fail Count:  %u\n", login->total_account_fail);
    printf("  Current Fail:      %d\n", login->account_fail);
    printf("  Report Count:      %d\n", login->report_cnt);
    printf("  Reliable Flag:     %d\n", login->reliable_flag);
    printf("  Trade Gold Daily:  %u\n", login->trade_gold_daily);
    printf("  Last Gift Time:    %u\n", login->last_gift_time);
    printf("  Gift Count:        %u\n", login->gift_cnt);
    printf("  Login IP:          %s\n", login->login_ip);
    printf("  Security Flag:     %d\n", login->security_flag);
    printf("  Power Side:        %d\n", login->power_side);
    printf("  Dungeon Gold:      %u\n", login->dungeon_gain_gold);
    printf("  School ID:         %d\n", login->school_id);
    printf("  Rating:            %.2f\n", login->rating);
    printf("  Cleanpad Point:    %u\n", login->cleanpad_point);
    printf("  Tutorial Skip:     %s\n", login->tutorial_skipable);
    printf("  Event Charac Flag: %u\n", login->event_charac_flag);
    printf("  Garena Token:      %lld\n", login->garena_token_key);
    printf("================================================================================\n");
}

int MemberLogin_Copy(MemberLogin* dest, const MemberLogin* src) {
    if (!dest || !src) return -1;
    memcpy(dest, src, sizeof(MemberLogin));
    return 0;
}

/* ========================================================================
 * CRUD Operations
 * ======================================================================== */

int MemberLogin_Create(DBConnectionManager* manager, MemberLogin* login) {
    char query[1024];
    DBQueryResult result;
    int ret;

    if (!manager || !login) {
        printf("[MemberLogin ORM] Error: NULL parameter\n");
        return -1;
    }

    snprintf(query, sizeof(query),
             "INSERT INTO member_login (m_id, login_time, expire_time, last_play_time, "
             "total_account_fail, account_fail, report_cnt, reliable_flag, trade_gold_daily, "
             "last_gift_time, gift_cnt, login_ip, security_flag, power_side, dungeon_gain_gold, "
             "school_id, rating, cleanpad_point, tutorial_skipable, event_charac_flag, garena_token_key) "
             "VALUES (%d, %u, %u, %u, %u, %d, %d, %d, %u, %u, %u, '%s', %d, %d, %u, %d, %.2f, %u, '%s', %u, %lld)",
             login->m_id, login->login_time, login->expire_time, login->last_play_time,
             login->total_account_fail, login->account_fail, login->report_cnt, login->reliable_flag,
             login->trade_gold_daily, login->last_gift_time, login->gift_cnt, login->login_ip,
             login->security_flag, login->power_side, login->dungeon_gain_gold, login->school_id,
             login->rating, login->cleanpad_point, login->tutorial_skipable, login->event_charac_flag,
             login->garena_token_key);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret > 0) {
        printf("[MemberLogin ORM] Created login session for m_id=%d\n", login->m_id);
        ret = 0;
    } else {
        printf("[MemberLogin ORM] Failed to create login session for m_id=%d\n", login->m_id);
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberLogin_GetByID(DBConnectionManager* manager, int m_id, MemberLogin* login) {
    char query[512];
    char* values[21];
    DBQueryResult result;
    int ret;

    if (!manager || !login) return -1;

    snprintf(query, sizeof(query),
             "SELECT m_id, login_time, expire_time, last_play_time, total_account_fail, "
             "account_fail, report_cnt, reliable_flag, trade_gold_daily, last_gift_time, "
             "gift_cnt, login_ip, security_flag, power_side, dungeon_gain_gold, school_id, "
             "rating, cleanpad_point, tutorial_skipable, event_charac_flag, garena_token_key "
             "FROM member_login WHERE m_id = %d",
             m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* Parse fields */
    login->m_id = atoi(values[0]);
    login->login_time = (unsigned int)atoi(values[1]);
    login->expire_time = (unsigned int)atoi(values[2]);
    login->last_play_time = (unsigned int)atoi(values[3]);
    login->total_account_fail = (unsigned int)atoi(values[4]);
    login->account_fail = atoi(values[5]);
    login->report_cnt = atoi(values[6]);
    login->reliable_flag = atoi(values[7]);
    login->trade_gold_daily = (unsigned int)atoi(values[8]);
    login->last_gift_time = (unsigned int)atoi(values[9]);
    login->gift_cnt = (unsigned short)atoi(values[10]);
    strncpy(login->login_ip, values[11] ? values[11] : "", sizeof(login->login_ip) - 1);
    login->security_flag = atoi(values[12]);
    login->power_side = atoi(values[13]);
    login->dungeon_gain_gold = (unsigned int)atoi(values[14]);
    login->school_id = atoi(values[15]);
    login->rating = (float)atof(values[16]);
    login->cleanpad_point = (unsigned int)atoi(values[17]);
    strncpy(login->tutorial_skipable, values[18] ? values[18] : "0", sizeof(login->tutorial_skipable) - 1);
    login->event_charac_flag = (unsigned int)atoi(values[19]);
    login->garena_token_key = atoll(values[20]);

    DBQueryResult_Free(&result);
    return 0;
}

int MemberLogin_Update(DBConnectionManager* manager, MemberLogin* login) {
    char query[1024];
    DBQueryResult result;
    int ret;

    if (!manager || !login) return -1;

    snprintf(query, sizeof(query),
             "UPDATE member_login SET login_time=%u, expire_time=%u, last_play_time=%u, "
             "total_account_fail=%u, account_fail=%d, report_cnt=%d, reliable_flag=%d, "
             "trade_gold_daily=%u, last_gift_time=%u, gift_cnt=%u, login_ip='%s', "
             "security_flag=%d, power_side=%d, dungeon_gain_gold=%u, school_id=%d, "
             "rating=%.2f, cleanpad_point=%u, tutorial_skipable='%s', event_charac_flag=%u, "
             "garena_token_key=%lld WHERE m_id=%d",
             login->login_time, login->expire_time, login->last_play_time,
             login->total_account_fail, login->account_fail, login->report_cnt, login->reliable_flag,
             login->trade_gold_daily, login->last_gift_time, login->gift_cnt, login->login_ip,
             login->security_flag, login->power_side, login->dungeon_gain_gold, login->school_id,
             login->rating, login->cleanpad_point, login->tutorial_skipable, login->event_charac_flag,
             login->garena_token_key, login->m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret > 0) {
        printf("[MemberLogin ORM] Updated login session for m_id=%d\n", login->m_id);
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberLogin_Delete(DBConnectionManager* manager, int m_id) {
    char query[256];
    DBQueryResult result;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM member_login WHERE m_id = %d", m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    /* Query execution error */
    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* Delete succeeded (even if 0 rows affected) - idempotent operation */
    if (ret > 0) {
        printf("[MemberLogin ORM] Deleted login session for m_id=%d\n", m_id);
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ========================================================================
 * Auxiliary Query Functions
 * ======================================================================== */

bool MemberLogin_Exists(DBConnectionManager* manager, int m_id) {
    char query[256];
    char* values[1];
    DBQueryResult result;
    int ret;

    if (!manager) return false;

    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM member_login WHERE m_id = %d", m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return count > 0;
    }

    DBQueryResult_Free(&result);
    return false;
}

int MemberLogin_UpdateLoginTime(DBConnectionManager* manager, int m_id, const char* login_ip) {
    char query[512];
    DBQueryResult result;
    int ret;
    unsigned int current_time = (unsigned int)time(NULL);

    if (!manager || !login_ip) return -1;

    snprintf(query, sizeof(query),
             "UPDATE member_login SET login_time=%u, login_ip='%s' WHERE m_id=%d",
             current_time, login_ip, m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret > 0) {
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberLogin_UpdateLastPlayTime(DBConnectionManager* manager, int m_id) {
    char query[256];
    DBQueryResult result;
    int ret;
    unsigned int current_time = (unsigned int)time(NULL);

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE member_login SET last_play_time=%u WHERE m_id=%d",
             current_time, m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret > 0) {
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberLogin_IncrementFailCount(DBConnectionManager* manager, int m_id) {
    char query[256];
    DBQueryResult result;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE member_login SET account_fail=account_fail+1, total_account_fail=total_account_fail+1 WHERE m_id=%d",
             m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret > 0) {
        printf("[MemberLogin ORM] Incremented fail count for m_id=%d\n", m_id);
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberLogin_ResetFailCount(DBConnectionManager* manager, int m_id) {
    char query[256];
    DBQueryResult result;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE member_login SET account_fail=0 WHERE m_id=%d", m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret > 0) {
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberLogin_UpdateTradeGold(DBConnectionManager* manager, int m_id, unsigned int gold_amount) {
    char query[256];
    DBQueryResult result;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE member_login SET trade_gold_daily=%u WHERE m_id=%d",
             gold_amount, m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret > 0) {
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberLogin_AddDungeonGold(DBConnectionManager* manager, int m_id, unsigned int gold_amount) {
    char query[256];
    DBQueryResult result;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE member_login SET dungeon_gain_gold=dungeon_gain_gold+%u WHERE m_id=%d",
             gold_amount, m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret > 0) {
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberLogin_UpdateSecurityFlag(DBConnectionManager* manager, int m_id, int flag) {
    char query[256];
    DBQueryResult result;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE member_login SET security_flag=%d WHERE m_id=%d",
             flag, m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    /* Query execution error */
    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* Update succeeded (even if 0 rows affected) */
    if (ret > 0) {
        printf("[MemberLogin ORM] Updated security flag for m_id=%d\n", m_id);
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberLogin_GetByLoginIP(DBConnectionManager* manager, const char* login_ip, int* m_ids, int max_count) {
    char query[512];
    char* values[1];
    DBQueryResult result;
    int ret;
    int count = 0;

    if (!manager || !login_ip || !m_ids || max_count <= 0) return -1;

    snprintf(query, sizeof(query),
             "SELECT m_id FROM member_login WHERE login_ip = '%s' LIMIT %d",
             login_ip, max_count);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    /* Query execution error */
    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* Query succeeded, fetch results (may be 0 rows) */
    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        m_ids[count] = atoi(values[0]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count; /* Return 0 if no rows found, which is valid */
}

int MemberLogin_GetCount(DBConnectionManager* manager) {
    char query[128];
    char* values[1];
    DBQueryResult result;
    int ret;
    int count = -1;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_login");

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberLogin_GetActiveCount(DBConnectionManager* manager, unsigned int current_time) {
    char query[256];
    char* values[1];
    DBQueryResult result;
    int ret;
    int count = -1;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM member_login WHERE expire_time > %u",
             current_time);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberLogin_GetHighFailureAccounts(DBConnectionManager* manager, int threshold, MemberLogin* logins, int max_count) {
    char query[512];
    char* values[21];
    DBQueryResult result;
    int ret;
    int count = 0;

    if (!manager || !logins || max_count <= 0) return -1;

    snprintf(query, sizeof(query),
             "SELECT m_id, login_time, expire_time, last_play_time, total_account_fail, "
             "account_fail, report_cnt, reliable_flag, trade_gold_daily, last_gift_time, "
             "gift_cnt, login_ip, security_flag, power_side, dungeon_gain_gold, school_id, "
             "rating, cleanpad_point, tutorial_skipable, event_charac_flag, garena_token_key "
             "FROM member_login WHERE account_fail >= %d LIMIT %d",
             threshold, max_count);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        MemberLogin* login = &logins[count];

        login->m_id = atoi(values[0]);
        login->login_time = (unsigned int)atoi(values[1]);
        login->expire_time = (unsigned int)atoi(values[2]);
        login->last_play_time = (unsigned int)atoi(values[3]);
        login->total_account_fail = (unsigned int)atoi(values[4]);
        login->account_fail = atoi(values[5]);
        login->report_cnt = atoi(values[6]);
        login->reliable_flag = atoi(values[7]);
        login->trade_gold_daily = (unsigned int)atoi(values[8]);
        login->last_gift_time = (unsigned int)atoi(values[9]);
        login->gift_cnt = (unsigned short)atoi(values[10]);
        strncpy(login->login_ip, values[11] ? values[11] : "", sizeof(login->login_ip) - 1);
        login->security_flag = atoi(values[12]);
        login->power_side = atoi(values[13]);
        login->dungeon_gain_gold = (unsigned int)atoi(values[14]);
        login->school_id = atoi(values[15]);
        login->rating = (float)atof(values[16]);
        login->cleanpad_point = (unsigned int)atoi(values[17]);
        strncpy(login->tutorial_skipable, values[18] ? values[18] : "0", sizeof(login->tutorial_skipable) - 1);
        login->event_charac_flag = (unsigned int)atoi(values[19]);
        login->garena_token_key = atoll(values[20]);

        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberLogin_GetRecentlyLoggedIn(DBConnectionManager* manager, MemberLogin* logins, int max_count) {
    char query[512];
    char* values[21];
    DBQueryResult result;
    int ret;
    int count = 0;

    if (!manager || !logins || max_count <= 0) return -1;

    snprintf(query, sizeof(query),
             "SELECT m_id, login_time, expire_time, last_play_time, total_account_fail, "
             "account_fail, report_cnt, reliable_flag, trade_gold_daily, last_gift_time, "
             "gift_cnt, login_ip, security_flag, power_side, dungeon_gain_gold, school_id, "
             "rating, cleanpad_point, tutorial_skipable, event_charac_flag, garena_token_key "
             "FROM member_login ORDER BY login_time DESC LIMIT %d",
             max_count);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        MemberLogin* login = &logins[count];

        login->m_id = atoi(values[0]);
        login->login_time = (unsigned int)atoi(values[1]);
        login->expire_time = (unsigned int)atoi(values[2]);
        login->last_play_time = (unsigned int)atoi(values[3]);
        login->total_account_fail = (unsigned int)atoi(values[4]);
        login->account_fail = atoi(values[5]);
        login->report_cnt = atoi(values[6]);
        login->reliable_flag = atoi(values[7]);
        login->trade_gold_daily = (unsigned int)atoi(values[8]);
        login->last_gift_time = (unsigned int)atoi(values[9]);
        login->gift_cnt = (unsigned short)atoi(values[10]);
        strncpy(login->login_ip, values[11] ? values[11] : "", sizeof(login->login_ip) - 1);
        login->security_flag = atoi(values[12]);
        login->power_side = atoi(values[13]);
        login->dungeon_gain_gold = (unsigned int)atoi(values[14]);
        login->school_id = atoi(values[15]);
        login->rating = (float)atof(values[16]);
        login->cleanpad_point = (unsigned int)atoi(values[17]);
        strncpy(login->tutorial_skipable, values[18] ? values[18] : "0", sizeof(login->tutorial_skipable) - 1);
        login->event_charac_flag = (unsigned int)atoi(values[19]);
        login->garena_token_key = atoll(values[20]);

        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}
