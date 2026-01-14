#include "member_booster_gage_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int MemberBoosterGage_Add(DBConnectionManager* manager, const MemberBoosterGage* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "REPLACE INTO member_booster_gage (m_id, gage) VALUES (%u, %u)",
        record->m_id, record->gage);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberBoosterGage_Get(DBConnectionManager* manager, unsigned int m_id, MemberBoosterGage* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT m_id, gage FROM member_booster_gage WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->m_id = (unsigned int)atoi(values[0]);
        record->gage = (unsigned char)atoi(values[1]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberBoosterGage_Update(DBConnectionManager* manager, unsigned int m_id, unsigned char new_gage) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE member_booster_gage SET gage = %u WHERE m_id = %u",
        new_gage, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberBoosterGage_Delete(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM member_booster_gage WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberBoosterGage_Exists(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int exists = 0;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT 1 FROM member_booster_gage WHERE m_id = %u LIMIT 1",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = 1;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/*******************************************************************************
 * 业务查询实现
 ******************************************************************************/

int MemberBoosterGage_GetByGageRange(DBConnectionManager* manager,
                                      unsigned char min_gage, unsigned char max_gage,
                                      MemberBoosterGage* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT m_id, gage FROM member_booster_gage "
        "WHERE gage BETWEEN %u AND %u LIMIT %d",
        min_gage, max_gage, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(values, 0, sizeof(values));
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].gage = (unsigned char)atoi(values[1]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberBoosterGage_GetByMinGage(DBConnectionManager* manager,
                                    unsigned char min_gage,
                                    MemberBoosterGage* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT m_id, gage FROM member_booster_gage "
        "WHERE gage >= %u LIMIT %d",
        min_gage, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(values, 0, sizeof(values));
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].gage = (unsigned char)atoi(values[1]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberBoosterGage_GetByGageValue(DBConnectionManager* manager,
                                      unsigned char gage_value,
                                      MemberBoosterGage* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT m_id, gage FROM member_booster_gage "
        "WHERE gage = %u LIMIT %d",
        gage_value, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(values, 0, sizeof(values));
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].gage = (unsigned char)atoi(values[1]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberBoosterGage_GetZeroGage(DBConnectionManager* manager,
                                   MemberBoosterGage* records, int max_records) {
    return MemberBoosterGage_GetByGageValue(manager, 0, records, max_records);
}

int MemberBoosterGage_GetNonZeroGage(DBConnectionManager* manager,
                                      MemberBoosterGage* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT m_id, gage FROM member_booster_gage "
        "WHERE gage > 0 LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(values, 0, sizeof(values));
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].gage = (unsigned char)atoi(values[1]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberBoosterGage_GetTopGageMembers(DBConnectionManager* manager,
                                         MemberBoosterGage* records, int top_n) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || top_n <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT m_id, gage FROM member_booster_gage "
        "ORDER BY gage DESC LIMIT %d",
        top_n);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(values, 0, sizeof(values));
    while (count < top_n && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].gage = (unsigned char)atoi(values[1]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberBoosterGage_IncrementGage(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    /* 确保不超过 255 (TINYINT UNSIGNED 最大值) */
    snprintf(query, sizeof(query),
        "UPDATE member_booster_gage SET gage = IF(gage < 255, gage + 1, 255) WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberBoosterGage_DecrementGage(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    /* 确保不小于 0 */
    snprintf(query, sizeof(query),
        "UPDATE member_booster_gage SET gage = IF(gage > 0, gage - 1, 0) WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberBoosterGage_SetGageValue(DBConnectionManager* manager,
                                    unsigned char old_gage, unsigned char new_gage) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE member_booster_gage SET gage = %u WHERE gage = %u",
        new_gage, old_gage);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/*******************************************************************************
 * 批量操作实现
 ******************************************************************************/

int MemberBoosterGage_BulkAdd(DBConnectionManager* manager,
                               const MemberBoosterGage* records, int count) {
    int i;
    int success_count = 0;

    if (!manager || !records || count <= 0) {
        return -1;
    }

    for (i = 0; i < count; i++) {
        if (MemberBoosterGage_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int MemberBoosterGage_BulkDeleteByMemberIds(DBConnectionManager* manager,
                                              const unsigned int* m_ids, int count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    int i;
    char id_list[2048];
    int offset = 0;

    if (!manager || !m_ids || count <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(id_list, 0, sizeof(id_list));

    /* 构建 IN 子句的 ID 列表 */
    for (i = 0; i < count && offset < (int)sizeof(id_list) - 20; i++) {
        if (i > 0) {
            offset += snprintf(id_list + offset, sizeof(id_list) - offset, ",");
        }
        offset += snprintf(id_list + offset, sizeof(id_list) - offset, "%u", m_ids[i]);
    }

    snprintf(query, sizeof(query),
        "DELETE FROM member_booster_gage WHERE m_id IN (%s)",
        id_list);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void MemberBoosterGage_PrintInfo(const MemberBoosterGage* record) {
    if (!record) {
        printf("MemberBoosterGage record is NULL\n");
        return;
    }

    printf("=== MemberBoosterGage Record ===\n");
    printf("  m_id: %u\n", record->m_id);
    printf("  gage: %u\n", record->gage);
    printf("================================\n");
}
