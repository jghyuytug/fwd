#include "event_arad_birthday_6th_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 2048
#define MAX_FIELDS 8

int EventAradBirthday6th_Add(DBConnectionManager* mgr, const EventAradBirthday6th* rec) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!mgr || !rec) return -1;

    snprintf(query, sizeof(query), "REPLACE INTO event_arad_birthday_6th (server, point) VALUES (%u, %u)",
        rec->server, rec->point);

    if (DBConnectionManager_ExecuteQuery(mgr, DB_TYPE_TAIWAN, query, &result) < 0) return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventAradBirthday6th_Get(DBConnectionManager* mgr, unsigned int server, EventAradBirthday6th* rec) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!mgr || !rec) return -1;

    snprintf(query, sizeof(query), "SELECT server, point FROM event_arad_birthday_6th WHERE server = %u", server);

    if (DBConnectionManager_ExecuteQuery(mgr, DB_TYPE_TAIWAN, query, &result) < 0) return -1;

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    rec->server = (unsigned int)atoi(values[0]);
    rec->point = (unsigned int)atoi(values[1]);

    DBQueryResult_Free(&result);
    return 0;
}

int EventAradBirthday6th_Update(DBConnectionManager* mgr, const EventAradBirthday6th* rec) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!mgr || !rec) return -1;

    snprintf(query, sizeof(query), "UPDATE event_arad_birthday_6th SET point = %u WHERE server = %u",
        rec->point, rec->server);

    if (DBConnectionManager_ExecuteQuery(mgr, DB_TYPE_TAIWAN, query, &result) < 0) return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventAradBirthday6th_Delete(DBConnectionManager* mgr, unsigned int server) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!mgr) return -1;

    snprintf(query, sizeof(query), "DELETE FROM event_arad_birthday_6th WHERE server = %u", server);

    if (DBConnectionManager_ExecuteQuery(mgr, DB_TYPE_TAIWAN, query, &result) < 0) return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventAradBirthday6th_Exists(DBConnectionManager* mgr, unsigned int server) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int exists = 0;

    if (!mgr) return -1;

    snprintf(query, sizeof(query), "SELECT 1 FROM event_arad_birthday_6th WHERE server = %u LIMIT 1", server);

    if (DBConnectionManager_ExecuteQuery(mgr, DB_TYPE_TAIWAN, query, &result) < 0) return -1;

    if (DBQueryResult_FetchRow(&result, values) > 0) exists = 1;

    DBQueryResult_Free(&result);
    return exists;
}

int EventAradBirthday6th_GetAll(DBConnectionManager* mgr, EventAradBirthday6th** recs, int* count) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int capacity = 10, idx = 0;

    if (!mgr || !recs || !count) return -1;

    *recs = NULL;
    *count = 0;

    snprintf(query, sizeof(query), "SELECT server, point FROM event_arad_birthday_6th ORDER BY server");

    if (DBConnectionManager_ExecuteQuery(mgr, DB_TYPE_TAIWAN, query, &result) < 0) return -1;

    *recs = (EventAradBirthday6th*)malloc(sizeof(EventAradBirthday6th) * capacity);
    if (!*recs) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            EventAradBirthday6th* temp = (EventAradBirthday6th*)realloc(*recs, sizeof(EventAradBirthday6th) * capacity);
            if (!temp) {
                free(*recs);
                *recs = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *recs = temp;
        }

        (*recs)[idx].server = (unsigned int)atoi(values[0]);
        (*recs)[idx].point = (unsigned int)atoi(values[1]);
        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

int EventAradBirthday6th_GetTopServers(DBConnectionManager* mgr, int limit, EventAradBirthday6th** recs, int* count) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int capacity = limit > 0 ? limit : 10, idx = 0;

    if (!mgr || !recs || !count) return -1;

    *recs = NULL;
    *count = 0;

    snprintf(query, sizeof(query), "SELECT server, point FROM event_arad_birthday_6th ORDER BY point DESC LIMIT %d", limit);

    if (DBConnectionManager_ExecuteQuery(mgr, DB_TYPE_TAIWAN, query, &result) < 0) return -1;

    *recs = (EventAradBirthday6th*)malloc(sizeof(EventAradBirthday6th) * capacity);
    if (!*recs) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        (*recs)[idx].server = (unsigned int)atoi(values[0]);
        (*recs)[idx].point = (unsigned int)atoi(values[1]);
        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

int EventAradBirthday6th_AddPoints(DBConnectionManager* mgr, unsigned int server, unsigned int points) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!mgr) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_arad_birthday_6th (server, point) VALUES (%u, %u) "
        "ON DUPLICATE KEY UPDATE point = point + %u",
        server, points, points);

    if (DBConnectionManager_ExecuteQuery(mgr, DB_TYPE_TAIWAN, query, &result) < 0) return -1;

    DBQueryResult_Free(&result);
    return 0;
}

unsigned int EventAradBirthday6th_GetTotalPoints(DBConnectionManager* mgr) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    unsigned int total = 0;

    if (!mgr) return 0;

    snprintf(query, sizeof(query), "SELECT SUM(point) FROM event_arad_birthday_6th");

    if (DBConnectionManager_ExecuteQuery(mgr, DB_TYPE_TAIWAN, query, &result) < 0) return 0;

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        total = (unsigned int)atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return total;
}

void EventAradBirthday6th_PrintInfo(const EventAradBirthday6th* rec) {
    if (!rec) return;

    printf("=== Arad 6th Birthday Event ===\n");
    printf("Server: %u\n", rec->server);
    printf("Point:  %u\n", rec->point);
    printf("===============================\n");
}

void EventAradBirthday6th_FreeRecords(EventAradBirthday6th* recs) {
    if (recs) free(recs);
}
