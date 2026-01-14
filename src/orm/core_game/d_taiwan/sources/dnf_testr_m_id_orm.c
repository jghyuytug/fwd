#include "dnf_testr_m_id_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* CRUD 操作实现 */

int DnfTestrMId_Add(DBConnectionManager* manager, const DnfTestrMId* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !member) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO dnf_testr_m_id (m_id, sex) VALUES (%d, %u)",
        member->m_id, member->sex);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfTestrMId_Get(DBConnectionManager* manager, int m_id, DnfTestrMId* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !member) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, sex FROM dnf_testr_m_id WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(member, 0, sizeof(DnfTestrMId));
    member->m_id = row[0] ? atoi(row[0]) : 0;
    member->sex = row[1] ? (unsigned char)atoi(row[1]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfTestrMId_Update(DBConnectionManager* manager, const DnfTestrMId* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !member) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_testr_m_id SET sex = %u WHERE m_id = %d",
        member->sex, member->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfTestrMId_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_testr_m_id WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfTestrMId_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_testr_m_id WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count > 0 ? 1 : 0;
}

/* 业务操作实现 */

int DnfTestrMId_GetAll(DBConnectionManager* manager, DnfTestrMId* members,
                        int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !members || !actual_count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, sex FROM dnf_testr_m_id ORDER BY m_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&members[count], 0, sizeof(DnfTestrMId));
        members[count].m_id = row[0] ? atoi(row[0]) : 0;
        members[count].sex = row[1] ? (unsigned char)atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfTestrMId_GetBySex(DBConnectionManager* manager, unsigned char sex,
                          DnfTestrMId* members, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !members || !actual_count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, sex FROM dnf_testr_m_id WHERE sex = %u ORDER BY m_id", sex);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&members[count], 0, sizeof(DnfTestrMId));
        members[count].m_id = row[0] ? atoi(row[0]) : 0;
        members[count].sex = row[1] ? (unsigned char)atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfTestrMId_SetSex(DBConnectionManager* manager, int m_id, unsigned char sex) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_testr_m_id SET sex = %u WHERE m_id = %d", sex, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfTestrMId_CountAll(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM dnf_testr_m_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count;
}

int DnfTestrMId_CountBySex(DBConnectionManager* manager, unsigned char sex) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_testr_m_id WHERE sex = %u", sex);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count;
}

int DnfTestrMId_DeleteAll(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "DELETE FROM dnf_testr_m_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* 工具函数实现 */

void DnfTestrMId_PrintInfo(const DnfTestrMId* member) {
    if (!member) {
        return;
    }

    printf("=== DNF测试会员信息 ===\n");
    printf("会员ID: %d\n", member->m_id);
    printf("性别: %u (%s)\n", member->sex, member->sex == 0 ? "女" : "男");
    printf("======================\n");
}
