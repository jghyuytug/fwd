#include "log_growth_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* ========== CRUD Operations ========== */

int LogGrowth_Add(DBConnectionManager* manager, const LogGrowth* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "INSERT INTO log_growth (m_id, server_info, charac_no, charac_name, job, grow_type, occ_time) "
        "VALUES (%d, %u, %d, '%s', %u, %u, '%s')",
        record->m_id,
        record->server_info,
        record->charac_no,
        record->charac_name,
        record->job,
        record->grow_type,
        record->occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogGrowth_Get(DBConnectionManager* manager,
                   int m_id,
                   unsigned char server_info,
                   int charac_no,
                   LogGrowth* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, server_info, charac_no, charac_name, job, grow_type, occ_time "
        "FROM log_growth "
        "WHERE m_id = %d AND server_info = %u AND charac_no = %d",
        m_id, server_info, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogGrowth));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->server_info = row[1] ? (unsigned char)atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    if (row[3]) {
        strncpy(record->charac_name, row[3], sizeof(record->charac_name) - 1);
        record->charac_name[sizeof(record->charac_name) - 1] = '\0';
    }
    record->job = row[4] ? (unsigned char)atoi(row[4]) : 0;
    record->grow_type = row[5] ? (unsigned char)atoi(row[5]) : 0;
    if (row[6]) {
        strncpy(record->occ_time, row[6], sizeof(record->occ_time) - 1);
        record->occ_time[sizeof(record->occ_time) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogGrowth_Update(DBConnectionManager* manager, const LogGrowth* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE log_growth SET "
        "charac_name = '%s', job = %u, grow_type = %u, occ_time = '%s' "
        "WHERE m_id = %d AND server_info = %u AND charac_no = %d",
        record->charac_name,
        record->job,
        record->grow_type,
        record->occ_time,
        record->m_id,
        record->server_info,
        record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogGrowth_Delete(DBConnectionManager* manager,
                      int m_id,
                      unsigned char server_info,
                      int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM log_growth WHERE m_id = %d AND server_info = %u AND charac_no = %d",
        m_id, server_info, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogGrowth_Exists(DBConnectionManager* manager,
                      int m_id,
                      unsigned char server_info,
                      int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM log_growth WHERE m_id = %d AND server_info = %u AND charac_no = %d",
        m_id, server_info, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return (count > 0) ? 1 : 0;
}

/* ========== Business Operations ========== */

int LogGrowth_GetByMemberId(DBConnectionManager* manager,
                             int m_id,
                             LogGrowth* records,
                             int max_count,
                             int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, server_info, charac_no, charac_name, job, grow_type, occ_time "
        "FROM log_growth "
        "WHERE m_id = %d "
        "ORDER BY occ_time DESC",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogGrowth));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_info = row[1] ? (unsigned char)atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) {
            strncpy(records[count].charac_name, row[3], sizeof(records[count].charac_name) - 1);
            records[count].charac_name[sizeof(records[count].charac_name) - 1] = '\0';
        }
        records[count].job = row[4] ? (unsigned char)atoi(row[4]) : 0;
        records[count].grow_type = row[5] ? (unsigned char)atoi(row[5]) : 0;
        if (row[6]) {
            strncpy(records[count].occ_time, row[6], sizeof(records[count].occ_time) - 1);
            records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogGrowth_GetByCharacter(DBConnectionManager* manager,
                               int m_id,
                               unsigned char server_info,
                               int charac_no,
                               LogGrowth* records,
                               int max_count,
                               int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, server_info, charac_no, charac_name, job, grow_type, occ_time "
        "FROM log_growth "
        "WHERE m_id = %d AND server_info = %u AND charac_no = %d "
        "ORDER BY occ_time DESC",
        m_id, server_info, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogGrowth));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_info = row[1] ? (unsigned char)atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) {
            strncpy(records[count].charac_name, row[3], sizeof(records[count].charac_name) - 1);
            records[count].charac_name[sizeof(records[count].charac_name) - 1] = '\0';
        }
        records[count].job = row[4] ? (unsigned char)atoi(row[4]) : 0;
        records[count].grow_type = row[5] ? (unsigned char)atoi(row[5]) : 0;
        if (row[6]) {
            strncpy(records[count].occ_time, row[6], sizeof(records[count].occ_time) - 1);
            records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogGrowth_GetByJob(DBConnectionManager* manager,
                        unsigned char job,
                        LogGrowth* records,
                        int max_count,
                        int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, server_info, charac_no, charac_name, job, grow_type, occ_time "
        "FROM log_growth "
        "WHERE job = %u "
        "ORDER BY occ_time DESC",
        job);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogGrowth));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_info = row[1] ? (unsigned char)atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) {
            strncpy(records[count].charac_name, row[3], sizeof(records[count].charac_name) - 1);
            records[count].charac_name[sizeof(records[count].charac_name) - 1] = '\0';
        }
        records[count].job = row[4] ? (unsigned char)atoi(row[4]) : 0;
        records[count].grow_type = row[5] ? (unsigned char)atoi(row[5]) : 0;
        if (row[6]) {
            strncpy(records[count].occ_time, row[6], sizeof(records[count].occ_time) - 1);
            records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogGrowth_GetByGrowType(DBConnectionManager* manager,
                              unsigned char grow_type,
                              LogGrowth* records,
                              int max_count,
                              int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, server_info, charac_no, charac_name, job, grow_type, occ_time "
        "FROM log_growth "
        "WHERE grow_type = %u "
        "ORDER BY occ_time DESC",
        grow_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogGrowth));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_info = row[1] ? (unsigned char)atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) {
            strncpy(records[count].charac_name, row[3], sizeof(records[count].charac_name) - 1);
            records[count].charac_name[sizeof(records[count].charac_name) - 1] = '\0';
        }
        records[count].job = row[4] ? (unsigned char)atoi(row[4]) : 0;
        records[count].grow_type = row[5] ? (unsigned char)atoi(row[5]) : 0;
        if (row[6]) {
            strncpy(records[count].occ_time, row[6], sizeof(records[count].occ_time) - 1);
            records[count].occ_time[sizeof(records[count].occ_time) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* ========== Utility Functions ========== */

void LogGrowth_PrintInfo(const LogGrowth* record) {
    if (!record) {
        printf("LogGrowth: NULL\n");
        return;
    }

    printf("LogGrowth:\n");
    printf("  m_id: %d\n", record->m_id);
    printf("  server_info: %u\n", record->server_info);
    printf("  charac_no: %d\n", record->charac_no);
    printf("  charac_name: %s\n", record->charac_name);
    printf("  job: %u\n", record->job);
    printf("  grow_type: %u\n", record->grow_type);
    printf("  occ_time: %s\n", record->occ_time);
}
