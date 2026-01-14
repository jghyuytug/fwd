#include "gm_manifest_notuse_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* Add new GM manifest record */
int GmManifestNotuse_Add(DBConnectionManager* manager, const GmManifestNotuse* gm) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !gm) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO gm_manifest_notuse (m_id, level) VALUES (%d, %u)",
        gm->m_id, gm->level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get by primary key (m_id) */
int GmManifestNotuse_Get(DBConnectionManager* manager, int m_id, GmManifestNotuse* gm) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !gm) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, level FROM gm_manifest_notuse WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(gm, 0, sizeof(GmManifestNotuse));
    gm->m_id = row[0] ? atoi(row[0]) : 0;
    gm->level = row[1] ? (unsigned char)atoi(row[1]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

/* Update by primary key (m_id) */
int GmManifestNotuse_Update(DBConnectionManager* manager, const GmManifestNotuse* gm) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !gm) return -1;

    snprintf(query, sizeof(query),
        "UPDATE gm_manifest_notuse SET level = %u WHERE m_id = %d",
        gm->level, gm->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete by primary key (m_id) */
int GmManifestNotuse_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM gm_manifest_notuse WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if record exists */
int GmManifestNotuse_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM gm_manifest_notuse WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return exists;
}

/* Get by level */
int GmManifestNotuse_GetByLevel(DBConnectionManager* manager, unsigned char level,
                                 GmManifestNotuse* gms, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !gms || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, level FROM gm_manifest_notuse WHERE level = %u ORDER BY m_id", level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&gms[count], 0, sizeof(GmManifestNotuse));
        gms[count].m_id = row[0] ? atoi(row[0]) : 0;
        gms[count].level = row[1] ? (unsigned char)atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get all records */
int GmManifestNotuse_GetAll(DBConnectionManager* manager, GmManifestNotuse* gms,
                             int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !gms || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, level FROM gm_manifest_notuse ORDER BY m_id LIMIT %d", max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&gms[count], 0, sizeof(GmManifestNotuse));
        gms[count].m_id = row[0] ? atoi(row[0]) : 0;
        gms[count].level = row[1] ? (unsigned char)atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Print GM manifest information */
void GmManifestNotuse_PrintInfo(const GmManifestNotuse* gm) {
    if (!gm) {
        printf("GmManifestNotuse: NULL\n");
        return;
    }

    printf("GmManifestNotuse:\n");
    printf("  m_id: %d\n", gm->m_id);
    printf("  level: %u\n", gm->level);
}
