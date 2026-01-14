#include "member_join_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/* ======================================== CRUD Operations ======================================== */

int MemberJoinInfo_Add(DBConnectionManager* manager, const MemberJoinInfo* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO member_join_info "
        "(m_id, reg_date, ip, contry_code, login_time, error_type, login_ip, game_use_history) "
        "VALUES (%u, %d, '%s', %u, %d, %u, '%s', %u)",
        record->m_id, record->reg_date, record->ip, record->contry_code,
        record->login_time, record->error_type, record->login_ip, record->game_use_history);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberJoinInfo_Get(DBConnectionManager* manager, unsigned int m_id, MemberJoinInfo* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, reg_date, ip, contry_code, login_time, error_type, login_ip, game_use_history "
        "FROM member_join_info WHERE m_id = %u",
        m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = DBQueryResult_FetchRow(&result, values);

    if (ret > 0) {
        record->m_id = (unsigned int)atoi(values[0]);
        record->reg_date = atoi(values[1]);
        strncpy(record->ip, values[2], sizeof(record->ip) - 1);
        record->contry_code = (unsigned char)atoi(values[3]);
        record->login_time = atoi(values[4]);
        record->error_type = (unsigned char)atoi(values[5]);
        strncpy(record->login_ip, values[6], sizeof(record->login_ip) - 1);
        record->game_use_history = (unsigned char)atoi(values[7]);
        record->ip[sizeof(record->ip) - 1] = '\0';
        record->login_ip[sizeof(record->login_ip) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int MemberJoinInfo_Update(DBConnectionManager* manager, const MemberJoinInfo* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_join_info SET reg_date = %d, ip = '%s', contry_code = %u, "
        "login_time = %d, error_type = %u, login_ip = '%s', game_use_history = %u "
        "WHERE m_id = %u",
        record->reg_date, record->ip, record->contry_code, record->login_time,
        record->error_type, record->login_ip, record->game_use_history, record->m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberJoinInfo_Delete(DBConnectionManager* manager, unsigned int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM member_join_info WHERE m_id = %u", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberJoinInfo_Exists(DBConnectionManager* manager, unsigned int m_id) {
    if (!manager) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_join_info WHERE m_id = %u", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    char* values[MAX_FIELDS];
    int exists = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ======================================== Business Queries ======================================== */

int MemberJoinInfo_GetByRegIP(DBConnectionManager* manager, const char* ip,
                                MemberJoinInfo* records, int max_count) {
    if (!manager || !ip || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, reg_date, ip, contry_code, login_time, error_type, login_ip, game_use_history "
        "FROM member_join_info WHERE ip = '%s' LIMIT %d",
        ip, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].reg_date = atoi(values[1]);
        strncpy(records[count].ip, values[2], sizeof(records[count].ip) - 1);
        records[count].contry_code = (unsigned char)atoi(values[3]);
        records[count].login_time = atoi(values[4]);
        records[count].error_type = (unsigned char)atoi(values[5]);
        strncpy(records[count].login_ip, values[6], sizeof(records[count].login_ip) - 1);
        records[count].game_use_history = (unsigned char)atoi(values[7]);
        records[count].ip[sizeof(records[count].ip) - 1] = '\0';
        records[count].login_ip[sizeof(records[count].login_ip) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberJoinInfo_GetByCountryCode(DBConnectionManager* manager, unsigned char contry_code,
                                      MemberJoinInfo* records, int max_count) {
    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, reg_date, ip, contry_code, login_time, error_type, login_ip, game_use_history "
        "FROM member_join_info WHERE contry_code = %u LIMIT %d",
        contry_code, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].reg_date = atoi(values[1]);
        strncpy(records[count].ip, values[2], sizeof(records[count].ip) - 1);
        records[count].contry_code = (unsigned char)atoi(values[3]);
        records[count].login_time = atoi(values[4]);
        records[count].error_type = (unsigned char)atoi(values[5]);
        strncpy(records[count].login_ip, values[6], sizeof(records[count].login_ip) - 1);
        records[count].game_use_history = (unsigned char)atoi(values[7]);
        records[count].ip[sizeof(records[count].ip) - 1] = '\0';
        records[count].login_ip[sizeof(records[count].login_ip) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberJoinInfo_GetByRegDateRange(DBConnectionManager* manager, int start_date, int end_date,
                                       MemberJoinInfo* records, int max_count) {
    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, reg_date, ip, contry_code, login_time, error_type, login_ip, game_use_history "
        "FROM member_join_info WHERE reg_date BETWEEN %d AND %d LIMIT %d",
        start_date, end_date, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].reg_date = atoi(values[1]);
        strncpy(records[count].ip, values[2], sizeof(records[count].ip) - 1);
        records[count].contry_code = (unsigned char)atoi(values[3]);
        records[count].login_time = atoi(values[4]);
        records[count].error_type = (unsigned char)atoi(values[5]);
        strncpy(records[count].login_ip, values[6], sizeof(records[count].login_ip) - 1);
        records[count].game_use_history = (unsigned char)atoi(values[7]);
        records[count].ip[sizeof(records[count].ip) - 1] = '\0';
        records[count].login_ip[sizeof(records[count].login_ip) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberJoinInfo_GetByErrorType(DBConnectionManager* manager, unsigned char error_type,
                                    MemberJoinInfo* records, int max_count) {
    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, reg_date, ip, contry_code, login_time, error_type, login_ip, game_use_history "
        "FROM member_join_info WHERE error_type = %u LIMIT %d",
        error_type, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].reg_date = atoi(values[1]);
        strncpy(records[count].ip, values[2], sizeof(records[count].ip) - 1);
        records[count].contry_code = (unsigned char)atoi(values[3]);
        records[count].login_time = atoi(values[4]);
        records[count].error_type = (unsigned char)atoi(values[5]);
        strncpy(records[count].login_ip, values[6], sizeof(records[count].login_ip) - 1);
        records[count].game_use_history = (unsigned char)atoi(values[7]);
        records[count].ip[sizeof(records[count].ip) - 1] = '\0';
        records[count].login_ip[sizeof(records[count].login_ip) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberJoinInfo_GetByGameUseHistory(DBConnectionManager* manager, unsigned char game_use_history,
                                         MemberJoinInfo* records, int max_count) {
    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, reg_date, ip, contry_code, login_time, error_type, login_ip, game_use_history "
        "FROM member_join_info WHERE game_use_history = %u LIMIT %d",
        game_use_history, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].reg_date = atoi(values[1]);
        strncpy(records[count].ip, values[2], sizeof(records[count].ip) - 1);
        records[count].contry_code = (unsigned char)atoi(values[3]);
        records[count].login_time = atoi(values[4]);
        records[count].error_type = (unsigned char)atoi(values[5]);
        strncpy(records[count].login_ip, values[6], sizeof(records[count].login_ip) - 1);
        records[count].game_use_history = (unsigned char)atoi(values[7]);
        records[count].ip[sizeof(records[count].ip) - 1] = '\0';
        records[count].login_ip[sizeof(records[count].login_ip) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberJoinInfo_GetRecentRegistered(DBConnectionManager* manager, MemberJoinInfo* records,
                                         int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, reg_date, ip, contry_code, login_time, error_type, login_ip, game_use_history "
        "FROM member_join_info ORDER BY reg_date DESC LIMIT %d",
        limit);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < limit && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].reg_date = atoi(values[1]);
        strncpy(records[count].ip, values[2], sizeof(records[count].ip) - 1);
        records[count].contry_code = (unsigned char)atoi(values[3]);
        records[count].login_time = atoi(values[4]);
        records[count].error_type = (unsigned char)atoi(values[5]);
        strncpy(records[count].login_ip, values[6], sizeof(records[count].login_ip) - 1);
        records[count].game_use_history = (unsigned char)atoi(values[7]);
        records[count].ip[sizeof(records[count].ip) - 1] = '\0';
        records[count].login_ip[sizeof(records[count].login_ip) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberJoinInfo_GetAll(DBConnectionManager* manager, MemberJoinInfo* records,
                           int offset, int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, reg_date, ip, contry_code, login_time, error_type, login_ip, game_use_history "
        "FROM member_join_info LIMIT %d, %d",
        offset, limit);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < limit && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].reg_date = atoi(values[1]);
        strncpy(records[count].ip, values[2], sizeof(records[count].ip) - 1);
        records[count].contry_code = (unsigned char)atoi(values[3]);
        records[count].login_time = atoi(values[4]);
        records[count].error_type = (unsigned char)atoi(values[5]);
        strncpy(records[count].login_ip, values[6], sizeof(records[count].login_ip) - 1);
        records[count].game_use_history = (unsigned char)atoi(values[7]);
        records[count].ip[sizeof(records[count].ip) - 1] = '\0';
        records[count].login_ip[sizeof(records[count].login_ip) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberJoinInfo_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_join_info");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ======================================== Bulk Operations ======================================== */

int MemberJoinInfo_BulkAdd(DBConnectionManager* manager, const MemberJoinInfo* records,
                            int count) {
    if (!manager || !records || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (MemberJoinInfo_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int MemberJoinInfo_BulkDelete(DBConnectionManager* manager, const unsigned int* m_ids,
                               int count) {
    if (!manager || !m_ids || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        if (MemberJoinInfo_Delete(manager, m_ids[i]) != 0) {
            return -1;
        }
    }

    return 0;
}

/* ======================================== Utility Functions ======================================== */

void MemberJoinInfo_PrintInfo(const MemberJoinInfo* record) {
    if (!record) {
        printf("MemberJoinInfo is NULL\n");
        return;
    }

    printf("=== Member Join Info ===\n");
    printf("  m_id:             %u\n", record->m_id);
    printf("  reg_date:         %d\n", record->reg_date);
    printf("  ip:               %s\n", record->ip);
    printf("  contry_code:      %u\n", record->contry_code);
    printf("  login_time:       %d\n", record->login_time);
    printf("  error_type:       %u\n", record->error_type);
    printf("  login_ip:         %s\n", record->login_ip);
    printf("  game_use_history: %u\n", record->game_use_history);
    printf("========================\n");
}
