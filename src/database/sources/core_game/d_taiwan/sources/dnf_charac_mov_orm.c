#include "dnf_charac_mov_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 2048

/* ==================== CRUD 操作实现 ==================== */

int DnfCharacMov_Add(DBConnectionManager* manager, const DnfCharacMov* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_charac_mov (m_id, server_id, charac_no, move_server_id, move_charac_no, move_check) "
        "VALUES (%d, %d, %d, %d, %d, %d)",
        record->m_id, record->server_id, record->charac_no,
        record->move_server_id, record->move_charac_no, record->move_check);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);

    /* 获取自增ID */
    memset(&result, 0, sizeof(DBQueryResult));
    snprintf(query, sizeof(query), "SELECT LAST_INSERT_ID()");
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int new_id = atoi(values[0]);
    DBQueryResult_Free(&result);
    return new_id;
}

int DnfCharacMov_Get(DBConnectionManager* manager, int id, DnfCharacMov* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT id, m_id, server_id, charac_no, move_server_id, move_charac_no, move_check "
        "FROM dnf_charac_mov WHERE id = %d",
        id);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 获取行数据 */
    char* values[7];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 解析结果 */
    record->id = atoi(values[0]);
    record->m_id = atoi(values[1]);
    record->server_id = atoi(values[2]);
    record->charac_no = atoi(values[3]);
    record->move_server_id = atoi(values[4]);
    record->move_charac_no = atoi(values[5]);
    record->move_check = atoi(values[6]);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfCharacMov_GetByCharacter(DBConnectionManager* manager, int m_id, int server_id, int charac_no, DnfCharacMov* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT id, m_id, server_id, charac_no, move_server_id, move_charac_no, move_check "
        "FROM dnf_charac_mov WHERE m_id = %d AND server_id = %d AND charac_no = %d",
        m_id, server_id, charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 获取行数据 */
    char* values[7];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 解析结果 */
    record->id = atoi(values[0]);
    record->m_id = atoi(values[1]);
    record->server_id = atoi(values[2]);
    record->charac_no = atoi(values[3]);
    record->move_server_id = atoi(values[4]);
    record->move_charac_no = atoi(values[5]);
    record->move_check = atoi(values[6]);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfCharacMov_Update(DBConnectionManager* manager, const DnfCharacMov* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_charac_mov SET m_id=%d, server_id=%d, charac_no=%d, "
        "move_server_id=%d, move_charac_no=%d, move_check=%d WHERE id=%d",
        record->m_id, record->server_id, record->charac_no,
        record->move_server_id, record->move_charac_no, record->move_check,
        record->id);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfCharacMov_Delete(DBConnectionManager* manager, int id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_charac_mov WHERE id = %d", id);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfCharacMov_Exists(DBConnectionManager* manager, int id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM dnf_charac_mov WHERE id = %d LIMIT 1", id);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    int exists = (ret > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 专项操作实现 ==================== */

int DnfCharacMov_UpdateMoveCheck(DBConnectionManager* manager, int id, int move_check) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_charac_mov SET move_check=%d WHERE id=%d",
        move_check, id);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfCharacMov_GetByMid(DBConnectionManager* manager, int m_id, DnfCharacMov* records, int max_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT id, m_id, server_id, charac_no, move_server_id, move_charac_no, move_check "
        "FROM dnf_charac_mov WHERE m_id = %d LIMIT %d",
        m_id, max_count);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[7];
    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].id = atoi(values[0]);
        records[count].m_id = atoi(values[1]);
        records[count].server_id = atoi(values[2]);
        records[count].charac_no = atoi(values[3]);
        records[count].move_server_id = atoi(values[4]);
        records[count].move_charac_no = atoi(values[5]);
        records[count].move_check = atoi(values[6]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int DnfCharacMov_GetBySourceServer(DBConnectionManager* manager, int server_id, DnfCharacMov* records, int max_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT id, m_id, server_id, charac_no, move_server_id, move_charac_no, move_check "
        "FROM dnf_charac_mov WHERE server_id = %d LIMIT %d",
        server_id, max_count);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[7];
    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].id = atoi(values[0]);
        records[count].m_id = atoi(values[1]);
        records[count].server_id = atoi(values[2]);
        records[count].charac_no = atoi(values[3]);
        records[count].move_server_id = atoi(values[4]);
        records[count].move_charac_no = atoi(values[5]);
        records[count].move_check = atoi(values[6]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int DnfCharacMov_GetByTargetServer(DBConnectionManager* manager, int move_server_id, DnfCharacMov* records, int max_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT id, m_id, server_id, charac_no, move_server_id, move_charac_no, move_check "
        "FROM dnf_charac_mov WHERE move_server_id = %d LIMIT %d",
        move_server_id, max_count);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[7];
    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].id = atoi(values[0]);
        records[count].m_id = atoi(values[1]);
        records[count].server_id = atoi(values[2]);
        records[count].charac_no = atoi(values[3]);
        records[count].move_server_id = atoi(values[4]);
        records[count].move_charac_no = atoi(values[5]);
        records[count].move_check = atoi(values[6]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 查询操作实现 ==================== */

int DnfCharacMov_CountByMid(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_charac_mov WHERE m_id = %d", m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);
    return count;
}

int DnfCharacMov_CountByMoveCheck(DBConnectionManager* manager, int move_check) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_charac_mov WHERE move_check = %d", move_check);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);
    return count;
}

int DnfCharacMov_CountTotal(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM dnf_charac_mov");

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 批量操作实现 ==================== */

int DnfCharacMov_BatchDelete(DBConnectionManager* manager, const int* ids, int count) {
    char query[MAX_QUERY_LEN];
    char id_list[1024];
    DBQueryResult result;

    if (!manager || !ids || count <= 0) {
        return -1;
    }

    /* 构建 id 列表 */
    id_list[0] = '\0';
    for (int i = 0; i < count; i++) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d", ids[i]);
        if (i > 0) {
            strcat(id_list, ",");
        }
        strcat(id_list, buf);
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_charac_mov WHERE id IN (%s)", id_list);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 工具函数实现 ==================== */

void DnfCharacMov_PrintInfo(const DnfCharacMov* record) {
    if (!record) {
        printf("DnfCharacMov: NULL\n");
        return;
    }

    printf("DnfCharacMov:\n");
    printf("  id: %d\n", record->id);
    printf("  m_id: %d\n", record->m_id);
    printf("  server_id: %d -> move_server_id: %d\n", record->server_id, record->move_server_id);
    printf("  charac_no: %d -> move_charac_no: %d\n", record->charac_no, record->move_charac_no);
    printf("  move_check: %d\n", record->move_check);
}
