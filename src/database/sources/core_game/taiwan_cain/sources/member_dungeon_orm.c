#include "member_dungeon_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192  /* Larger for TEXT fields */
#define MAX_FIELDS 10

/* Helper function to escape strings for SQL (basic implementation) */
static void escape_string(char* dest, const char* src, int max_len) {
    int i, j;
    int src_len = strlen(src);

    for (i = 0, j = 0; i < src_len && j < max_len - 2; i++) {
        if (src[i] == '\'' || src[i] == '\\' || src[i] == '\"') {
            dest[j++] = '\\';
        }
        dest[j++] = src[i];
    }
    dest[j] = '\0';
}

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int MemberDungeon_Add(DBConnectionManager* manager, const MemberDungeon* record) {
    char query[MAX_QUERY_LEN];
    char escaped_dungeon[MAX_DUNGEON_DATA_LEN * 2];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(escaped_dungeon, 0, sizeof(escaped_dungeon));

    escape_string(escaped_dungeon, record->dungeon, sizeof(escaped_dungeon));

    snprintf(query, sizeof(query),
        "REPLACE INTO member_dungeon (m_id, dungeon) VALUES (%u, '%s')",
        record->m_id, escaped_dungeon);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberDungeon_Get(DBConnectionManager* manager, unsigned int m_id, MemberDungeon* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(MemberDungeon));

    snprintf(query, sizeof(query),
        "SELECT m_id, dungeon FROM member_dungeon WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->m_id = (unsigned int)atoi(values[0]);
        if (values[1]) {
            strncpy(record->dungeon, values[1], MAX_DUNGEON_DATA_LEN - 1);
            record->dungeon[MAX_DUNGEON_DATA_LEN - 1] = '\0';
        }
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberDungeon_Update(DBConnectionManager* manager, unsigned int m_id, const char* new_dungeon_data) {
    char query[MAX_QUERY_LEN];
    char escaped_dungeon[MAX_DUNGEON_DATA_LEN * 2];
    DBQueryResult result;

    if (!manager || !new_dungeon_data) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(escaped_dungeon, 0, sizeof(escaped_dungeon));

    escape_string(escaped_dungeon, new_dungeon_data, sizeof(escaped_dungeon));

    snprintf(query, sizeof(query),
        "UPDATE member_dungeon SET dungeon = '%s' WHERE m_id = %u",
        escaped_dungeon, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberDungeon_Delete(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM member_dungeon WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberDungeon_Exists(DBConnectionManager* manager, unsigned int m_id) {
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
        "SELECT 1 FROM member_dungeon WHERE m_id = %u LIMIT 1",
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

int MemberDungeon_GetEmptyDungeonMembers(DBConnectionManager* manager,
                                          MemberDungeon* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT m_id, dungeon FROM member_dungeon "
        "WHERE dungeon = '' OR dungeon IS NULL LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(values, 0, sizeof(values));
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        if (values[1]) {
            strncpy(records[count].dungeon, values[1], MAX_DUNGEON_DATA_LEN - 1);
            records[count].dungeon[MAX_DUNGEON_DATA_LEN - 1] = '\0';
        } else {
            records[count].dungeon[0] = '\0';
        }
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberDungeon_GetNonEmptyDungeonMembers(DBConnectionManager* manager,
                                              MemberDungeon* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT m_id, dungeon FROM member_dungeon "
        "WHERE dungeon != '' AND dungeon IS NOT NULL LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(values, 0, sizeof(values));
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        if (values[1]) {
            strncpy(records[count].dungeon, values[1], MAX_DUNGEON_DATA_LEN - 1);
            records[count].dungeon[MAX_DUNGEON_DATA_LEN - 1] = '\0';
        } else {
            records[count].dungeon[0] = '\0';
        }
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberDungeon_GetDungeonLength(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int length = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT LENGTH(dungeon) FROM member_dungeon WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        length = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return length;
}

int MemberDungeon_SearchDungeonByKeyword(DBConnectionManager* manager,
                                          const char* keyword,
                                          MemberDungeon* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char escaped_keyword[512];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !keyword || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(escaped_keyword, 0, sizeof(escaped_keyword));

    escape_string(escaped_keyword, keyword, sizeof(escaped_keyword));

    snprintf(query, sizeof(query),
        "SELECT m_id, dungeon FROM member_dungeon "
        "WHERE dungeon LIKE '%%%s%%' LIMIT %d",
        escaped_keyword, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(values, 0, sizeof(values));
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        if (values[1]) {
            strncpy(records[count].dungeon, values[1], MAX_DUNGEON_DATA_LEN - 1);
            records[count].dungeon[MAX_DUNGEON_DATA_LEN - 1] = '\0';
        } else {
            records[count].dungeon[0] = '\0';
        }
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberDungeon_ClearDungeonData(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE member_dungeon SET dungeon = '' WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberDungeon_AppendDungeonData(DBConnectionManager* manager,
                                     unsigned int m_id, const char* append_data) {
    char query[MAX_QUERY_LEN];
    char escaped_data[MAX_DUNGEON_DATA_LEN * 2];
    DBQueryResult result;

    if (!manager || !append_data) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(escaped_data, 0, sizeof(escaped_data));

    escape_string(escaped_data, append_data, sizeof(escaped_data));

    snprintf(query, sizeof(query),
        "UPDATE member_dungeon SET dungeon = CONCAT(dungeon, '%s') WHERE m_id = %u",
        escaped_data, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberDungeon_GetMembersByDungeonSize(DBConnectionManager* manager,
                                            int min_size, int max_size,
                                            MemberDungeon* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT m_id, dungeon FROM member_dungeon "
        "WHERE LENGTH(dungeon) BETWEEN %d AND %d LIMIT %d",
        min_size, max_size, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(values, 0, sizeof(values));
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        if (values[1]) {
            strncpy(records[count].dungeon, values[1], MAX_DUNGEON_DATA_LEN - 1);
            records[count].dungeon[MAX_DUNGEON_DATA_LEN - 1] = '\0';
        } else {
            records[count].dungeon[0] = '\0';
        }
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberDungeon_CountNonEmptyDungeons(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_dungeon "
        "WHERE dungeon != '' AND dungeon IS NOT NULL");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberDungeon_TruncateDungeonData(DBConnectionManager* manager,
                                       unsigned int m_id, int max_length) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || max_length < 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE member_dungeon SET dungeon = LEFT(dungeon, %d) WHERE m_id = %u",
        max_length, m_id);

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

int MemberDungeon_BulkAdd(DBConnectionManager* manager,
                           const MemberDungeon* records, int count) {
    int i;
    int success_count = 0;

    if (!manager || !records || count <= 0) {
        return -1;
    }

    for (i = 0; i < count; i++) {
        if (MemberDungeon_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int MemberDungeon_BulkDeleteByMemberIds(DBConnectionManager* manager,
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
        "DELETE FROM member_dungeon WHERE m_id IN (%s)",
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

void MemberDungeon_PrintInfo(const MemberDungeon* record) {
    if (!record) {
        printf("MemberDungeon record is NULL\n");
        return;
    }

    printf("=== MemberDungeon Record ===\n");
    printf("  m_id: %u\n", record->m_id);
    printf("  dungeon data length: %lu\n", (unsigned long)strlen(record->dungeon));
    if (strlen(record->dungeon) > 0) {
        printf("  dungeon data (first 100 chars): %.100s%s\n",
               record->dungeon,
               strlen(record->dungeon) > 100 ? "..." : "");
    } else {
        printf("  dungeon data: (empty)\n");
    }
    printf("============================\n");
}
