#include "gm_manifest_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int GmManifest_Add(DBConnectionManager* manager, const GmManifest* gm) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !gm) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO gm_manifest (m_id, level) VALUES (%d, %u)",
        gm->m_id, gm->level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int GmManifest_Get(DBConnectionManager* manager, int m_id, GmManifest* gm) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !gm) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, level FROM gm_manifest WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(gm, 0, sizeof(GmManifest));
    gm->m_id = row[0] ? atoi(row[0]) : 0;
    gm->level = row[1] ? (unsigned char)atoi(row[1]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GmManifest_Update(DBConnectionManager* manager, const GmManifest* gm) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !gm) return -1;

    snprintf(query, sizeof(query),
        "UPDATE gm_manifest SET level = %u WHERE m_id = %d",
        gm->level, gm->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int GmManifest_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM gm_manifest WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int GmManifest_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM gm_manifest WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0 && row[0]) {
        int count = atoi(row[0]);
        DBQueryResult_Free(&result);
        return count > 0;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GmManifest_GetAll(DBConnectionManager* manager,
                      GmManifest* gms,
                      int max_count,
                      int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !gms || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, level FROM gm_manifest ORDER BY m_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&gms[count], 0, sizeof(GmManifest));
        gms[count].m_id = row[0] ? atoi(row[0]) : 0;
        gms[count].level = row[1] ? (unsigned char)atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int GmManifest_GetByLevel(DBConnectionManager* manager,
                          unsigned char level,
                          GmManifest* gms,
                          int max_count,
                          int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !gms || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, level FROM gm_manifest WHERE level = %u ORDER BY m_id",
        level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&gms[count], 0, sizeof(GmManifest));
        gms[count].m_id = row[0] ? atoi(row[0]) : 0;
        gms[count].level = row[1] ? (unsigned char)atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int GmManifest_GetCount(DBConnectionManager* manager, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !count) return -1;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM gm_manifest");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) > 0 && row[0]) {
        *count = atoi(row[0]);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

void GmManifest_PrintInfo(const GmManifest* gm) {
    if (!gm) return;

    printf("GM Manifest:\n");
    printf("  Member ID: %d\n", gm->m_id);
    printf("  GM Level: %u\n", gm->level);
}
