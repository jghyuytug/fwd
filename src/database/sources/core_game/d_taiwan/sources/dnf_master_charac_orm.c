#include "dnf_master_charac_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/* ==================== CRUD 操作 ==================== */

int DnfMasterCharac_Add(DBConnectionManager* manager, const DnfMasterCharac* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    /* 有复合主键，可以使用 REPLACE INTO */
    snprintf(query, sizeof(query),
        "REPLACE INTO dnf_master_charac (m_id, global_type, server_id, charac_no, charac_name, job, lev) "
        "VALUES (%u, %u, %u, %u, '%s', %u, %u)",
        record->m_id, record->global_type, record->server_id, record->charac_no,
        record->charac_name, record->job, record->lev);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMasterCharac_Get(DBConnectionManager* manager, unsigned int m_id, unsigned char global_type,
                        unsigned char server_id, DnfMasterCharac* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, global_type, server_id, charac_no, charac_name, job, lev "
        "FROM dnf_master_charac WHERE m_id = %u AND global_type = %u AND server_id = %u",
        m_id, global_type, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    if (DBQueryResult_FetchRow(&result, values) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    record->m_id = (unsigned int)atoi(values[0]);
    record->global_type = (unsigned char)atoi(values[1]);
    record->server_id = (unsigned char)atoi(values[2]);
    record->charac_no = (unsigned int)atoi(values[3]);
    strncpy(record->charac_name, values[4], sizeof(record->charac_name) - 1);
    record->charac_name[sizeof(record->charac_name) - 1] = '\0';
    record->job = (unsigned char)atoi(values[5]);
    record->lev = (unsigned char)atoi(values[6]);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMasterCharac_Update(DBConnectionManager* manager, const DnfMasterCharac* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_master_charac SET charac_no = %u, charac_name = '%s', job = %u, lev = %u "
        "WHERE m_id = %u AND global_type = %u AND server_id = %u",
        record->charac_no, record->charac_name, record->job, record->lev,
        record->m_id, record->global_type, record->server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMasterCharac_Delete(DBConnectionManager* manager, unsigned int m_id, unsigned char global_type,
                           unsigned char server_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_master_charac WHERE m_id = %u AND global_type = %u AND server_id = %u",
        m_id, global_type, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMasterCharac_Exists(DBConnectionManager* manager, unsigned int m_id, unsigned char global_type,
                           unsigned char server_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) as cnt FROM dnf_master_charac WHERE m_id = %u AND global_type = %u AND server_id = %u",
        m_id, global_type, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return 0;
    }

    char* values[MAX_FIELDS];
    int exists = 0;

    if (DBQueryResult_FetchRow(&result, values) >= 0) {
        exists = (atoi(values[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 专项操作 ==================== */

int DnfMasterCharac_UpdateLevel(DBConnectionManager* manager, unsigned int m_id, unsigned char global_type,
                                unsigned char server_id, unsigned char lev) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_master_charac SET lev = %u WHERE m_id = %u AND global_type = %u AND server_id = %u",
        lev, m_id, global_type, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMasterCharac_UpdateCharacNo(DBConnectionManager* manager, unsigned int m_id, unsigned char global_type,
                                   unsigned char server_id, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_master_charac SET charac_no = %u WHERE m_id = %u AND global_type = %u AND server_id = %u",
        charac_no, m_id, global_type, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMasterCharac_SetMasterCharac(DBConnectionManager* manager, unsigned int m_id, unsigned char global_type,
                                    unsigned char server_id, unsigned int charac_no, const char* charac_name,
                                    unsigned char job, unsigned char lev) {
    DnfMasterCharac record;

    if (!charac_name) {
        return -1;
    }

    record.m_id = m_id;
    record.global_type = global_type;
    record.server_id = server_id;
    record.charac_no = charac_no;
    strncpy(record.charac_name, charac_name, sizeof(record.charac_name) - 1);
    record.charac_name[sizeof(record.charac_name) - 1] = '\0';
    record.job = job;
    record.lev = lev;

    return DnfMasterCharac_Add(manager, &record);
}

/* ==================== 查询操作 ==================== */

int DnfMasterCharac_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) as cnt FROM dnf_master_charac");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    char* values[MAX_FIELDS];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) >= 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int DnfMasterCharac_GetByMid(DBConnectionManager* manager, unsigned int m_id, DnfMasterCharac** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT m_id, global_type, server_id, charac_no, charac_name, job, lev "
        "FROM dnf_master_charac WHERE m_id = %u ORDER BY global_type, server_id",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    *records = (DnfMasterCharac*)malloc(sizeof(DnfMasterCharac) * result.row_count);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    *count = 0;

    while (DBQueryResult_FetchRow(&result, values) >= 0) {
        DnfMasterCharac* rec = &(*records)[*count];
        rec->m_id = (unsigned int)atoi(values[0]);
        rec->global_type = (unsigned char)atoi(values[1]);
        rec->server_id = (unsigned char)atoi(values[2]);
        rec->charac_no = (unsigned int)atoi(values[3]);
        strncpy(rec->charac_name, values[4], sizeof(rec->charac_name) - 1);
        rec->charac_name[sizeof(rec->charac_name) - 1] = '\0';
        rec->job = (unsigned char)atoi(values[5]);
        rec->lev = (unsigned char)atoi(values[6]);
        (*count)++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMasterCharac_GetByServer(DBConnectionManager* manager, unsigned char server_id, DnfMasterCharac** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT m_id, global_type, server_id, charac_no, charac_name, job, lev "
        "FROM dnf_master_charac WHERE server_id = %u ORDER BY m_id, global_type",
        server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    *records = (DnfMasterCharac*)malloc(sizeof(DnfMasterCharac) * result.row_count);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    *count = 0;

    while (DBQueryResult_FetchRow(&result, values) >= 0) {
        DnfMasterCharac* rec = &(*records)[*count];
        rec->m_id = (unsigned int)atoi(values[0]);
        rec->global_type = (unsigned char)atoi(values[1]);
        rec->server_id = (unsigned char)atoi(values[2]);
        rec->charac_no = (unsigned int)atoi(values[3]);
        strncpy(rec->charac_name, values[4], sizeof(rec->charac_name) - 1);
        rec->charac_name[sizeof(rec->charac_name) - 1] = '\0';
        rec->job = (unsigned char)atoi(values[5]);
        rec->lev = (unsigned char)atoi(values[6]);
        (*count)++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMasterCharac_GetByGlobalType(DBConnectionManager* manager, unsigned char global_type, DnfMasterCharac** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT m_id, global_type, server_id, charac_no, charac_name, job, lev "
        "FROM dnf_master_charac WHERE global_type = %u ORDER BY m_id, server_id",
        global_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    *records = (DnfMasterCharac*)malloc(sizeof(DnfMasterCharac) * result.row_count);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    *count = 0;

    while (DBQueryResult_FetchRow(&result, values) >= 0) {
        DnfMasterCharac* rec = &(*records)[*count];
        rec->m_id = (unsigned int)atoi(values[0]);
        rec->global_type = (unsigned char)atoi(values[1]);
        rec->server_id = (unsigned char)atoi(values[2]);
        rec->charac_no = (unsigned int)atoi(values[3]);
        strncpy(rec->charac_name, values[4], sizeof(rec->charac_name) - 1);
        rec->charac_name[sizeof(rec->charac_name) - 1] = '\0';
        rec->job = (unsigned char)atoi(values[5]);
        rec->lev = (unsigned char)atoi(values[6]);
        (*count)++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMasterCharac_GetByCharacNo(DBConnectionManager* manager, unsigned int charac_no, DnfMasterCharac** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT m_id, global_type, server_id, charac_no, charac_name, job, lev "
        "FROM dnf_master_charac WHERE charac_no = %u ORDER BY m_id, global_type, server_id",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    *records = (DnfMasterCharac*)malloc(sizeof(DnfMasterCharac) * result.row_count);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    *count = 0;

    while (DBQueryResult_FetchRow(&result, values) >= 0) {
        DnfMasterCharac* rec = &(*records)[*count];
        rec->m_id = (unsigned int)atoi(values[0]);
        rec->global_type = (unsigned char)atoi(values[1]);
        rec->server_id = (unsigned char)atoi(values[2]);
        rec->charac_no = (unsigned int)atoi(values[3]);
        strncpy(rec->charac_name, values[4], sizeof(rec->charac_name) - 1);
        rec->charac_name[sizeof(rec->charac_name) - 1] = '\0';
        rec->job = (unsigned char)atoi(values[5]);
        rec->lev = (unsigned char)atoi(values[6]);
        (*count)++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 批量操作 ==================== */

int DnfMasterCharac_Clear(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "DELETE FROM dnf_master_charac");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMasterCharac_DeleteByMid(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_master_charac WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMasterCharac_DeleteByServer(DBConnectionManager* manager, unsigned char server_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_master_charac WHERE server_id = %u",
        server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 工具函数 ==================== */

void DnfMasterCharac_PrintInfo(const DnfMasterCharac* record) {
    if (!record) {
        printf("DnfMasterCharac record is NULL\n");
        return;
    }

    printf("========================================\n");
    printf("DnfMasterCharac 信息:\n");
    printf("  会员ID: %u\n", record->m_id);
    printf("  全局类型: %u\n", record->global_type);
    printf("  服务器ID: %u\n", record->server_id);
    printf("  角色编号: %u\n", record->charac_no);
    printf("  角色名称: %s\n", record->charac_name);
    printf("  职业: %u\n", record->job);
    printf("  等级: %u\n", record->lev);
    printf("========================================\n");
}
