#include "slang_list_name_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

#define MAX_QUERY_LEN 4096

/* Helper function to escape special characters */
static void escape_string(char* dest, const char* src, int max_len) {
    int i, j = 0;
    for (i = 0; src[i] && j < max_len - 2; i++) {
        if (src[i] == '\'' || src[i] == '\"' || src[i] == '\\') {
            dest[j++] = '\\';
        }
        dest[j++] = src[i];
    }
    dest[j] = '\0';
}

int SlangListName_Add(DBConnectionManager* manager, const char* slang) {
    char query[MAX_QUERY_LEN];
    char escaped_slang[308];
    DBQueryResult result;

    if (!manager || !slang || !slang[0]) return -1;

    escape_string(escaped_slang, slang, sizeof(escaped_slang));

    snprintf(query, sizeof(query),
        "INSERT INTO slang_list_name (slang) VALUES ('%s')", escaped_slang);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SlangListName_Exists(DBConnectionManager* manager, const char* slang) {
    char query[MAX_QUERY_LEN];
    char escaped_slang[308];
    DBQueryResult result;
    char* row[1];

    if (!manager || !slang || !slang[0]) return 0;

    escape_string(escaped_slang, slang, sizeof(escaped_slang));

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM slang_list_name WHERE slang = '%s'", escaped_slang);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count > 0;
}

int SlangListName_Delete(DBConnectionManager* manager, const char* slang) {
    char query[MAX_QUERY_LEN];
    char escaped_slang[308];
    DBQueryResult result;

    if (!manager || !slang || !slang[0]) return -1;

    escape_string(escaped_slang, slang, sizeof(escaped_slang));

    snprintf(query, sizeof(query),
        "DELETE FROM slang_list_name WHERE slang = '%s'", escaped_slang);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SlangListName_GetAll(DBConnectionManager* manager, SlangListName* slang_list,
                         int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count = 0;

    if (!manager || !slang_list || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT slang FROM slang_list_name ORDER BY slang LIMIT %d", max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&slang_list[count], 0, sizeof(SlangListName));
        if (row[0]) {
            strncpy(slang_list[count].slang, row[0], sizeof(slang_list[count].slang) - 1);
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int SlangListName_Count(DBConnectionManager* manager, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !count) return -1;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM slang_list_name");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return 0;
}

int SlangListName_Clear(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM slang_list_name");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

void SlangListName_PrintInfo(const SlangListName* slang) {
    if (!slang) return;

    printf("=== Slang Name Info ===\n");
    printf("Slang: %s\n", slang->slang);
    printf("=======================\n");
}

int SlangListName_IsFiltered(DBConnectionManager* manager, const char* text) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int is_filtered = 0;

    if (!manager || !text || !text[0]) return 0;

    /* Get all slang words */
    snprintf(query, sizeof(query), "SELECT slang FROM slang_list_name");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return 0;
    }

    /* Check if text contains any slang word (case-insensitive) */
    while (DBQueryResult_FetchRow(&result, row) > 0) {
        if (row[0] && row[0][0]) {
            /* Simple substring search (case-insensitive) */
            const char* p = text;
            const char* slang = row[0];
            int slang_len = strlen(slang);

            while (*p) {
                if (strncasecmp(p, slang, slang_len) == 0) {
                    is_filtered = 1;
                    break;
                }
                p++;
            }

            if (is_filtered) break;
        }
    }

    DBQueryResult_Free(&result);
    return is_filtered;
}
