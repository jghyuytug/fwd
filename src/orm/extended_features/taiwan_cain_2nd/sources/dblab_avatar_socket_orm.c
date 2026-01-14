#include "dblab_avatar_socket_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DblabAvatarSocket_Add(DBConnectionManager* manager, const DblabAvatarSocket* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dblab_avatar_socket (it_id, jewel_socket) "
        "VALUES (%d, '%s')",
        record->it_id, record->jewel_socket);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DblabAvatarSocket_Get(DBConnectionManager* manager, int it_id, DblabAvatarSocket* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_id, jewel_socket FROM dblab_avatar_socket WHERE it_id = %d",
        it_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DblabAvatarSocket));
    record->it_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->jewel_socket, row[1], sizeof(record->jewel_socket) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DblabAvatarSocket_GetAll(DBConnectionManager* manager, DblabAvatarSocket* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT it_id, jewel_socket FROM dblab_avatar_socket");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DblabAvatarSocket));
        records[count].it_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].jewel_socket, row[1], sizeof(records[count].jewel_socket) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
