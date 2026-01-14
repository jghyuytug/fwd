#include "guild_grade_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int GuildGradeLog_Add(DBConnectionManager* manager, const GuildGradeLog* log, int* new_id) {
    if (!manager || !log) return -1;

    char query[2048];
    snprintf(query, sizeof(query),
        "INSERT INTO guild_grade_log (guild_id, m_id, server_id, charac_no, charac_name, "
        "occ_time, grade_prev, grade_next, reason, admin_no, admin_name) "
        "VALUES (%d, %d, %d, %d, '%s', '%s', %d, %d, '%s', %d, '%s')",
        log->guild_id, log->m_id, log->server_id, log->charac_no, log->charac_name,
        log->occ_time, log->grade_prev, log->grade_next, log->reason,
        log->admin_no, log->admin_name);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret > 0) {
        // Get last insert ID using LAST_INSERT_ID()
        if (new_id) {
            char id_query[128];
            snprintf(id_query, sizeof(id_query), "SELECT LAST_INSERT_ID()");
            DBQueryResult id_result;
            if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, id_query, &id_result) > 0) {
                char* row[1];
                if (DBQueryResult_FetchRow(&id_result, row) == 0) {
                    *new_id = atoi(row[0]);
                }
                DBQueryResult_Free(&id_result);
            }
        }
    }
    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int GuildGradeLog_Get(DBConnectionManager* manager, int id, GuildGradeLog* log) {
    if (!manager || !log) return -1;

    char query[1024];
    snprintf(query, sizeof(query),
        "SELECT id, guild_id, m_id, server_id, charac_no, charac_name, occ_time, "
        "grade_prev, grade_next, reason, admin_no, admin_name "
        "FROM guild_grade_log WHERE id = %d",
        id);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) return ret;

    char* row[12];
    ret = DBQueryResult_FetchRow(&result, row);
    if (ret == 0) {
        log->id = atoi(row[0]);
        log->guild_id = atoi(row[1]);
        log->m_id = atoi(row[2]);
        log->server_id = (char)atoi(row[3]);
        log->charac_no = atoi(row[4]);
        strncpy(log->charac_name, row[5], sizeof(log->charac_name) - 1);
        log->charac_name[sizeof(log->charac_name) - 1] = '\0';
        strncpy(log->occ_time, row[6], sizeof(log->occ_time) - 1);
        log->occ_time[sizeof(log->occ_time) - 1] = '\0';
        log->grade_prev = (char)atoi(row[7]);
        log->grade_next = (char)atoi(row[8]);
        strncpy(log->reason, row[9], sizeof(log->reason) - 1);
        log->reason[sizeof(log->reason) - 1] = '\0';
        log->admin_no = atoi(row[10]);
        strncpy(log->admin_name, row[11], sizeof(log->admin_name) - 1);
        log->admin_name[sizeof(log->admin_name) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildGradeLog_Delete(DBConnectionManager* manager, int id) {
    if (!manager) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "DELETE FROM guild_grade_log WHERE id = %d",
        id);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int GuildGradeLog_GetByGuild(DBConnectionManager* manager, int guild_id, GuildGradeLog** logs, int* count) {
    if (!manager || !logs || !count) return -1;

    char query[1024];
    snprintf(query, sizeof(query),
        "SELECT id, guild_id, m_id, server_id, charac_no, charac_name, occ_time, "
        "grade_prev, grade_next, reason, admin_no, admin_name "
        "FROM guild_grade_log WHERE guild_id = %d ORDER BY occ_time DESC",
        guild_id);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) return ret;

    // Count rows
    *count = 0;
    char* row[12];
    while (DBQueryResult_FetchRow(&result, row) == 0) {
        (*count)++;
    }

    if (*count == 0) {
        *logs = NULL;
        DBQueryResult_Free(&result);
        return 0;
    }

    // Allocate memory
    *logs = (GuildGradeLog*)malloc(sizeof(GuildGradeLog) * (*count));
    if (!*logs) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // Re-execute query
    DBQueryResult_Free(&result);
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) {
        free(*logs);
        *logs = NULL;
        return ret;
    }

    int i = 0;
    while (DBQueryResult_FetchRow(&result, row) == 0 && i < *count) {
        (*logs)[i].id = atoi(row[0]);
        (*logs)[i].guild_id = atoi(row[1]);
        (*logs)[i].m_id = atoi(row[2]);
        (*logs)[i].server_id = (char)atoi(row[3]);
        (*logs)[i].charac_no = atoi(row[4]);
        strncpy((*logs)[i].charac_name, row[5], sizeof((*logs)[i].charac_name) - 1);
        (*logs)[i].charac_name[sizeof((*logs)[i].charac_name) - 1] = '\0';
        strncpy((*logs)[i].occ_time, row[6], sizeof((*logs)[i].occ_time) - 1);
        (*logs)[i].occ_time[sizeof((*logs)[i].occ_time) - 1] = '\0';
        (*logs)[i].grade_prev = (char)atoi(row[7]);
        (*logs)[i].grade_next = (char)atoi(row[8]);
        strncpy((*logs)[i].reason, row[9], sizeof((*logs)[i].reason) - 1);
        (*logs)[i].reason[sizeof((*logs)[i].reason) - 1] = '\0';
        (*logs)[i].admin_no = atoi(row[10]);
        strncpy((*logs)[i].admin_name, row[11], sizeof((*logs)[i].admin_name) - 1);
        (*logs)[i].admin_name[sizeof((*logs)[i].admin_name) - 1] = '\0';
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildGradeLog_GetByCharacter(DBConnectionManager* manager, int server_id, int charac_no, GuildGradeLog** logs, int* count) {
    if (!manager || !logs || !count) return -1;

    char query[1024];
    snprintf(query, sizeof(query),
        "SELECT id, guild_id, m_id, server_id, charac_no, charac_name, occ_time, "
        "grade_prev, grade_next, reason, admin_no, admin_name "
        "FROM guild_grade_log WHERE server_id = %d AND charac_no = %d ORDER BY occ_time DESC",
        server_id, charac_no);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) return ret;

    // Count rows
    *count = 0;
    char* row[12];
    while (DBQueryResult_FetchRow(&result, row) == 0) {
        (*count)++;
    }

    if (*count == 0) {
        *logs = NULL;
        DBQueryResult_Free(&result);
        return 0;
    }

    // Allocate memory
    *logs = (GuildGradeLog*)malloc(sizeof(GuildGradeLog) * (*count));
    if (!*logs) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // Re-execute query
    DBQueryResult_Free(&result);
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) {
        free(*logs);
        *logs = NULL;
        return ret;
    }

    int i = 0;
    while (DBQueryResult_FetchRow(&result, row) == 0 && i < *count) {
        (*logs)[i].id = atoi(row[0]);
        (*logs)[i].guild_id = atoi(row[1]);
        (*logs)[i].m_id = atoi(row[2]);
        (*logs)[i].server_id = (char)atoi(row[3]);
        (*logs)[i].charac_no = atoi(row[4]);
        strncpy((*logs)[i].charac_name, row[5], sizeof((*logs)[i].charac_name) - 1);
        (*logs)[i].charac_name[sizeof((*logs)[i].charac_name) - 1] = '\0';
        strncpy((*logs)[i].occ_time, row[6], sizeof((*logs)[i].occ_time) - 1);
        (*logs)[i].occ_time[sizeof((*logs)[i].occ_time) - 1] = '\0';
        (*logs)[i].grade_prev = (char)atoi(row[7]);
        (*logs)[i].grade_next = (char)atoi(row[8]);
        strncpy((*logs)[i].reason, row[9], sizeof((*logs)[i].reason) - 1);
        (*logs)[i].reason[sizeof((*logs)[i].reason) - 1] = '\0';
        (*logs)[i].admin_no = atoi(row[10]);
        strncpy((*logs)[i].admin_name, row[11], sizeof((*logs)[i].admin_name) - 1);
        (*logs)[i].admin_name[sizeof((*logs)[i].admin_name) - 1] = '\0';
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildGradeLog_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date, GuildGradeLog** logs, int* count) {
    if (!manager || !start_date || !end_date || !logs || !count) return -1;

    char query[1024];
    snprintf(query, sizeof(query),
        "SELECT id, guild_id, m_id, server_id, charac_no, charac_name, occ_time, "
        "grade_prev, grade_next, reason, admin_no, admin_name "
        "FROM guild_grade_log WHERE occ_time BETWEEN '%s' AND '%s' ORDER BY occ_time DESC",
        start_date, end_date);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) return ret;

    // Count rows
    *count = 0;
    char* row[12];
    while (DBQueryResult_FetchRow(&result, row) == 0) {
        (*count)++;
    }

    if (*count == 0) {
        *logs = NULL;
        DBQueryResult_Free(&result);
        return 0;
    }

    // Allocate memory
    *logs = (GuildGradeLog*)malloc(sizeof(GuildGradeLog) * (*count));
    if (!*logs) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // Re-execute query
    DBQueryResult_Free(&result);
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) {
        free(*logs);
        *logs = NULL;
        return ret;
    }

    int i = 0;
    while (DBQueryResult_FetchRow(&result, row) == 0 && i < *count) {
        (*logs)[i].id = atoi(row[0]);
        (*logs)[i].guild_id = atoi(row[1]);
        (*logs)[i].m_id = atoi(row[2]);
        (*logs)[i].server_id = (char)atoi(row[3]);
        (*logs)[i].charac_no = atoi(row[4]);
        strncpy((*logs)[i].charac_name, row[5], sizeof((*logs)[i].charac_name) - 1);
        (*logs)[i].charac_name[sizeof((*logs)[i].charac_name) - 1] = '\0';
        strncpy((*logs)[i].occ_time, row[6], sizeof((*logs)[i].occ_time) - 1);
        (*logs)[i].occ_time[sizeof((*logs)[i].occ_time) - 1] = '\0';
        (*logs)[i].grade_prev = (char)atoi(row[7]);
        (*logs)[i].grade_next = (char)atoi(row[8]);
        strncpy((*logs)[i].reason, row[9], sizeof((*logs)[i].reason) - 1);
        (*logs)[i].reason[sizeof((*logs)[i].reason) - 1] = '\0';
        (*logs)[i].admin_no = atoi(row[10]);
        strncpy((*logs)[i].admin_name, row[11], sizeof((*logs)[i].admin_name) - 1);
        (*logs)[i].admin_name[sizeof((*logs)[i].admin_name) - 1] = '\0';
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}
