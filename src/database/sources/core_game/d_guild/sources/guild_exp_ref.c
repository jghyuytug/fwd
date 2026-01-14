#include "guild_exp_ref.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int GuildExpRef_Add(DBConnectionManager* manager, const GuildExpRef* ref) {
    if (!manager || !ref) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO guild_exp_ref (grade, exp) VALUES (%d, %u)",
        ref->grade, ref->exp);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int GuildExpRef_Get(DBConnectionManager* manager, int grade, GuildExpRef* ref) {
    if (!manager || !ref) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT grade, exp FROM guild_exp_ref WHERE grade = %d",
        grade);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) return ret;

    char* row[2];
    ret = DBQueryResult_FetchRow(&result, row);
    if (ret == 0) {
        ref->grade = atoi(row[0]);
        ref->exp = (unsigned int)atoi(row[1]);
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildExpRef_Update(DBConnectionManager* manager, const GuildExpRef* ref) {
    if (!manager || !ref) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "UPDATE guild_exp_ref SET exp = %u WHERE grade = %d",
        ref->exp, ref->grade);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int GuildExpRef_Delete(DBConnectionManager* manager, int grade) {
    if (!manager) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "DELETE FROM guild_exp_ref WHERE grade = %d",
        grade);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int GuildExpRef_Exists(DBConnectionManager* manager, int grade) {
    if (!manager) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_exp_ref WHERE grade = %d",
        grade);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) return ret;

    char* row[1];
    ret = DBQueryResult_FetchRow(&result, row);
    if (ret == 0) {
        ret = atoi(row[0]) > 0 ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildExpRef_GetAll(DBConnectionManager* manager, GuildExpRef** refs, int* count) {
    if (!manager || !refs || !count) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT grade, exp FROM guild_exp_ref ORDER BY grade");

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) return ret;

    // Count rows
    *count = 0;
    char* row[2];
    while (DBQueryResult_FetchRow(&result, row) == 0) {
        (*count)++;
    }

    if (*count == 0) {
        *refs = NULL;
        DBQueryResult_Free(&result);
        return 0;
    }

    // Allocate memory
    *refs = (GuildExpRef*)malloc(sizeof(GuildExpRef) * (*count));
    if (!*refs) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // Re-execute query to fetch data
    DBQueryResult_Free(&result);
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) {
        free(*refs);
        *refs = NULL;
        return ret;
    }

    int i = 0;
    while (DBQueryResult_FetchRow(&result, row) == 0 && i < *count) {
        (*refs)[i].grade = atoi(row[0]);
        (*refs)[i].exp = (unsigned int)atoi(row[1]);
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildExpRef_GetByExpRange(DBConnectionManager* manager, unsigned int min_exp, unsigned int max_exp, GuildExpRef** refs, int* count) {
    if (!manager || !refs || !count) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT grade, exp FROM guild_exp_ref WHERE exp BETWEEN %u AND %u ORDER BY grade",
        min_exp, max_exp);

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) return ret;

    // Count rows
    *count = 0;
    char* row[2];
    while (DBQueryResult_FetchRow(&result, row) == 0) {
        (*count)++;
    }

    if (*count == 0) {
        *refs = NULL;
        DBQueryResult_Free(&result);
        return 0;
    }

    // Allocate memory
    *refs = (GuildExpRef*)malloc(sizeof(GuildExpRef) * (*count));
    if (!*refs) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // Re-execute query
    DBQueryResult_Free(&result);
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) {
        free(*refs);
        *refs = NULL;
        return ret;
    }

    int i = 0;
    while (DBQueryResult_FetchRow(&result, row) == 0 && i < *count) {
        (*refs)[i].grade = atoi(row[0]);
        (*refs)[i].exp = (unsigned int)atoi(row[1]);
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildExpRef_GetMaxGrade(DBConnectionManager* manager, int* max_grade) {
    if (!manager || !max_grade) return -1;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT MAX(grade) FROM guild_exp_ref");

    DBQueryResult result;
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    if (ret != 0) return ret;

    char* row[1];
    ret = DBQueryResult_FetchRow(&result, row);
    if (ret == 0) {
        *max_grade = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}
