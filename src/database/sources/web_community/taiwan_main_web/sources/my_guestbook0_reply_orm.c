#include "my_guestbook0_reply_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MyGuestbook0Reply_Add(DBConnectionManager* manager, const MyGuestbook0Reply* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO my_guestbook0_reply (no, comment_reply) "
        "VALUES (%d, '%s')",
        record->no, record->comment_reply);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MyGuestbook0Reply_Get(DBConnectionManager* manager, int r_id, MyGuestbook0Reply* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT r_id, no, comment_reply FROM my_guestbook0_reply WHERE r_id = %d",
        r_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MyGuestbook0Reply));
    record->r_id = row[0] ? atoi(row[0]) : 0;
    record->no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->comment_reply, row[2], sizeof(record->comment_reply) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MyGuestbook0Reply_GetAll(DBConnectionManager* manager, MyGuestbook0Reply* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT r_id, no, comment_reply FROM my_guestbook0_reply");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MyGuestbook0Reply));
        records[count].r_id = row[0] ? atoi(row[0]) : 0;
        records[count].no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].comment_reply, row[2], sizeof(records[count].comment_reply) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
