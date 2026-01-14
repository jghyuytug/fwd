#include "passwd_mod_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int PasswdModEntry_Add(DBConnectionManager* manager, const PasswdModEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    if (entry->occ_time[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO passwd_mod_entry (m_id, occ_time, ip, pre_passwd) "
            "VALUES (%d, '%s', '%s', '%s')",
            entry->m_id, entry->occ_time, entry->ip, entry->pre_passwd);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO passwd_mod_entry (m_id, occ_time, ip, pre_passwd) "
            "VALUES (%d, NOW(), '%s', '%s')",
            entry->m_id, entry->ip, entry->pre_passwd);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int PasswdModEntry_Get(DBConnectionManager* manager, int m_id, const char* occ_time,
                       PasswdModEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !occ_time || !entry) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, ip, pre_passwd FROM passwd_mod_entry "
        "WHERE m_id = %d AND occ_time = '%s'",
        m_id, occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(entry, 0, sizeof(PasswdModEntry));
    entry->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) {
        strncpy(entry->occ_time, row[1], sizeof(entry->occ_time) - 1);
        entry->occ_time[sizeof(entry->occ_time) - 1] = '\0';
    }
    if (row[2]) {
        strncpy(entry->ip, row[2], sizeof(entry->ip) - 1);
        entry->ip[sizeof(entry->ip) - 1] = '\0';
    }
    if (row[3]) {
        strncpy(entry->pre_passwd, row[3], sizeof(entry->pre_passwd) - 1);
        entry->pre_passwd[sizeof(entry->pre_passwd) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PasswdModEntry_Update(DBConnectionManager* manager, int m_id, const char* occ_time,
                          const PasswdModEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_time || !entry) return -1;

    snprintf(query, sizeof(query),
        "UPDATE passwd_mod_entry SET ip = '%s', pre_passwd = '%s' "
        "WHERE m_id = %d AND occ_time = '%s'",
        entry->ip, entry->pre_passwd, m_id, occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int PasswdModEntry_Delete(DBConnectionManager* manager, int m_id, const char* occ_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_time) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM passwd_mod_entry WHERE m_id = %d AND occ_time = '%s'",
        m_id, occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int PasswdModEntry_Exists(DBConnectionManager* manager, int m_id, const char* occ_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager || !occ_time) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM passwd_mod_entry WHERE m_id = %d AND occ_time = '%s'",
        m_id, occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return exists > 0;
}

int PasswdModEntry_GetByMemberID(DBConnectionManager* manager, int m_id,
                                  PasswdModEntry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, ip, pre_passwd FROM passwd_mod_entry "
        "WHERE m_id = %d ORDER BY occ_time DESC",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(PasswdModEntry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(entries[count].occ_time, row[1], sizeof(entries[count].occ_time) - 1);
            entries[count].occ_time[sizeof(entries[count].occ_time) - 1] = '\0';
        }
        if (row[2]) {
            strncpy(entries[count].ip, row[2], sizeof(entries[count].ip) - 1);
            entries[count].ip[sizeof(entries[count].ip) - 1] = '\0';
        }
        if (row[3]) {
            strncpy(entries[count].pre_passwd, row[3], sizeof(entries[count].pre_passwd) - 1);
            entries[count].pre_passwd[sizeof(entries[count].pre_passwd) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int PasswdModEntry_GetByIP(DBConnectionManager* manager, const char* ip,
                           PasswdModEntry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !ip || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, ip, pre_passwd FROM passwd_mod_entry "
        "WHERE ip = '%s' ORDER BY occ_time DESC",
        ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(PasswdModEntry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(entries[count].occ_time, row[1], sizeof(entries[count].occ_time) - 1);
            entries[count].occ_time[sizeof(entries[count].occ_time) - 1] = '\0';
        }
        if (row[2]) {
            strncpy(entries[count].ip, row[2], sizeof(entries[count].ip) - 1);
            entries[count].ip[sizeof(entries[count].ip) - 1] = '\0';
        }
        if (row[3]) {
            strncpy(entries[count].pre_passwd, row[3], sizeof(entries[count].pre_passwd) - 1);
            entries[count].pre_passwd[sizeof(entries[count].pre_passwd) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int PasswdModEntry_GetByTimeRange(DBConnectionManager* manager, const char* start_time,
                                   const char* end_time, PasswdModEntry* entries,
                                   int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !start_time || !end_time || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, ip, pre_passwd FROM passwd_mod_entry "
        "WHERE occ_time BETWEEN '%s' AND '%s' ORDER BY occ_time DESC",
        start_time, end_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(PasswdModEntry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(entries[count].occ_time, row[1], sizeof(entries[count].occ_time) - 1);
            entries[count].occ_time[sizeof(entries[count].occ_time) - 1] = '\0';
        }
        if (row[2]) {
            strncpy(entries[count].ip, row[2], sizeof(entries[count].ip) - 1);
            entries[count].ip[sizeof(entries[count].ip) - 1] = '\0';
        }
        if (row[3]) {
            strncpy(entries[count].pre_passwd, row[3], sizeof(entries[count].pre_passwd) - 1);
            entries[count].pre_passwd[sizeof(entries[count].pre_passwd) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int PasswdModEntry_GetRecent(DBConnectionManager* manager, PasswdModEntry* entries,
                              int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, ip, pre_passwd FROM passwd_mod_entry "
        "ORDER BY occ_time DESC LIMIT %d",
        max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(PasswdModEntry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(entries[count].occ_time, row[1], sizeof(entries[count].occ_time) - 1);
            entries[count].occ_time[sizeof(entries[count].occ_time) - 1] = '\0';
        }
        if (row[2]) {
            strncpy(entries[count].ip, row[2], sizeof(entries[count].ip) - 1);
            entries[count].ip[sizeof(entries[count].ip) - 1] = '\0';
        }
        if (row[3]) {
            strncpy(entries[count].pre_passwd, row[3], sizeof(entries[count].pre_passwd) - 1);
            entries[count].pre_passwd[sizeof(entries[count].pre_passwd) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int PasswdModEntry_DeleteByMemberID(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM passwd_mod_entry WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void PasswdModEntry_PrintInfo(const PasswdModEntry* entry) {
    if (!entry) {
        printf("NULL entry\n");
        return;
    }

    printf("PasswdModEntry Info:\n");
    printf("  Member ID: %d\n", entry->m_id);
    printf("  Occur Time: %s\n", entry->occ_time);
    printf("  IP Address: %s\n", entry->ip);
    printf("  Previous Password: %s\n", entry->pre_passwd);
}
