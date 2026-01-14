#include "guild_halloffame_html_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 131072  /* Large buffer for TEXT fields */

/* Create a new GuildHalloffameHtml structure with allocated memory */
GuildHalloffameHtml* GuildHalloffameHtml_Create(void) {
    GuildHalloffameHtml* hall = (GuildHalloffameHtml*)malloc(sizeof(GuildHalloffameHtml));
    if (!hall) return NULL;

    memset(hall, 0, sizeof(GuildHalloffameHtml));
    hall->html = (char*)malloc(MAX_HTML_SIZE + 1);
    if (!hall->html) {
        free(hall);
        return NULL;
    }
    hall->html[0] = '\0';
    hall->html_length = 0;

    return hall;
}

/* Free GuildHalloffameHtml structure */
void GuildHalloffameHtml_Free(GuildHalloffameHtml* hall) {
    if (!hall) return;
    if (hall->html) {
        free(hall->html);
    }
    free(hall);
}

/* Add a new hall of fame HTML */
int GuildHalloffameHtml_Add(DBConnectionManager* manager, const GuildHalloffameHtml* hall) {
    char* query;
    DBQueryResult result;

    if (!manager || !hall || !hall->html) return -1;

    query = (char*)malloc(MAX_QUERY_LEN);
    if (!query) return -1;

    snprintf(query, MAX_QUERY_LEN,
        "INSERT INTO guild_halloffame_html (fame_id, title, html) "
        "VALUES (%d, '%s', '%s')",
        hall->fame_id, hall->title, hall->html);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        free(query);
        return -1;
    }

    DBQueryResult_Free(&result);
    free(query);
    return 0;
}

/* Get hall of fame HTML by fame_id */
int GuildHalloffameHtml_Get(DBConnectionManager* manager, int fame_id, GuildHalloffameHtml* hall) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    char* html_ptr;

    if (!manager || !hall) return -1;

    /* Save html pointer before memset */
    html_ptr = hall->html;

    snprintf(query, sizeof(query),
        "SELECT fame_id, title, html FROM guild_halloffame_html WHERE fame_id = %d",
        fame_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(hall, 0, sizeof(GuildHalloffameHtml));
    hall->html = html_ptr;  /* Restore html pointer */
    hall->fame_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(hall->title, row[1], sizeof(hall->title) - 1);

    if (row[2] && hall->html) {
        hall->html_length = strlen(row[2]);
        if (hall->html_length > MAX_HTML_SIZE) {
            hall->html_length = MAX_HTML_SIZE;
        }
        strncpy(hall->html, row[2], hall->html_length);
        hall->html[hall->html_length] = '\0';
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Update hall of fame HTML */
int GuildHalloffameHtml_Update(DBConnectionManager* manager, const GuildHalloffameHtml* hall) {
    char* query;
    DBQueryResult result;

    if (!manager || !hall || !hall->html) return -1;

    query = (char*)malloc(MAX_QUERY_LEN);
    if (!query) return -1;

    snprintf(query, MAX_QUERY_LEN,
        "UPDATE guild_halloffame_html SET title = '%s', html = '%s' WHERE fame_id = %d",
        hall->title, hall->html, hall->fame_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        free(query);
        return -1;
    }

    DBQueryResult_Free(&result);
    free(query);
    return 0;
}

/* Delete hall of fame HTML */
int GuildHalloffameHtml_Delete(DBConnectionManager* manager, int fame_id) {
    char query[1024];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_halloffame_html WHERE fame_id = %d", fame_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if hall of fame HTML exists */
int GuildHalloffameHtml_Exists(DBConnectionManager* manager, int fame_id) {
    char query[1024];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_halloffame_html WHERE fame_id = %d", fame_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Get all hall of fame HTMLs */
int GuildHalloffameHtml_GetAll(DBConnectionManager* manager, GuildHalloffameHtml** halls,
                                int max_count, int* actual_count) {
    char query[1024];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !halls || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT fame_id, title, html FROM guild_halloffame_html ORDER BY fame_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        halls[count]->fame_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(halls[count]->title, row[1], sizeof(halls[count]->title) - 1);

        if (row[2] && halls[count]->html) {
            halls[count]->html_length = strlen(row[2]);
            if (halls[count]->html_length > MAX_HTML_SIZE) {
                halls[count]->html_length = MAX_HTML_SIZE;
            }
            strncpy(halls[count]->html, row[2], halls[count]->html_length);
            halls[count]->html[halls[count]->html_length] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get hall of fame HTML by title */
int GuildHalloffameHtml_GetByTitle(DBConnectionManager* manager, const char* title,
                                   GuildHalloffameHtml* hall) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    char* html_ptr;

    if (!manager || !title || !hall) return -1;

    /* Save html pointer before memset */
    html_ptr = hall->html;

    snprintf(query, sizeof(query),
        "SELECT fame_id, title, html FROM guild_halloffame_html WHERE title = '%s'",
        title);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(hall, 0, sizeof(GuildHalloffameHtml));
    hall->html = html_ptr;  /* Restore html pointer */
    hall->fame_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(hall->title, row[1], sizeof(hall->title) - 1);

    if (row[2] && hall->html) {
        hall->html_length = strlen(row[2]);
        if (hall->html_length > MAX_HTML_SIZE) {
            hall->html_length = MAX_HTML_SIZE;
        }
        strncpy(hall->html, row[2], hall->html_length);
        hall->html[hall->html_length] = '\0';
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Update only HTML content */
int GuildHalloffameHtml_UpdateHtml(DBConnectionManager* manager, int fame_id,
                                   const char* new_html) {
    char* query;
    DBQueryResult result;

    if (!manager || !new_html) return -1;

    query = (char*)malloc(MAX_QUERY_LEN);
    if (!query) return -1;

    snprintf(query, MAX_QUERY_LEN,
        "UPDATE guild_halloffame_html SET html = '%s' WHERE fame_id = %d",
        new_html, fame_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        free(query);
        return -1;
    }

    DBQueryResult_Free(&result);
    free(query);
    return 0;
}

/* Print hall of fame HTML information */
void GuildHalloffameHtml_PrintInfo(const GuildHalloffameHtml* hall) {
    if (!hall) return;

    printf("Guild Hall of Fame HTML Info:\n");
    printf("  Fame ID: %d\n", hall->fame_id);
    printf("  Title: %s\n", hall->title);
    printf("  HTML Length: %u bytes\n", hall->html_length);
    if (hall->html && hall->html_length > 0) {
        printf("  HTML Preview: %.100s%s\n",
               hall->html,
               hall->html_length > 100 ? "..." : "");
    }
}
