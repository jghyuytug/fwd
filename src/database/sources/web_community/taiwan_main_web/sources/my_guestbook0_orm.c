#include "my_guestbook0_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MyGuestbook0_Add(DBConnectionManager* manager, const MyGuestbook0* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO my_guestbook0 (master, m_nickname, m_id, m_user_id, m_sex, comment_text, emoticon, ip, create_day) "
        "VALUES (%d, '%s', %d, '%s', %d, '%s', %d, '%s', %d)",
        record->master, record->m_nickname, record->m_id, record->m_user_id, record->m_sex, record->comment_text, record->emoticon, record->ip, record->create_day);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MyGuestbook0_Get(DBConnectionManager* manager, int master, int no, MyGuestbook0* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, master, m_nickname, m_id, m_user_id, m_sex, comment_text, emoticon, ip, create_day FROM my_guestbook0 WHERE master = %d AND no = %d",
        master, no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MyGuestbook0));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->master = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->m_nickname, row[2], sizeof(record->m_nickname) - 1);
    record->m_id = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->m_user_id, row[4], sizeof(record->m_user_id) - 1);
    record->m_sex = row[5] ? row[5][0] : '\0';
    if (row[6]) strncpy(record->comment_text, row[6], sizeof(record->comment_text) - 1);
    record->emoticon = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->ip, row[8], sizeof(record->ip) - 1);
    record->create_day = row[9] ? atoi(row[9]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int MyGuestbook0_GetAll(DBConnectionManager* manager, MyGuestbook0* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, master, m_nickname, m_id, m_user_id, m_sex, comment_text, emoticon, ip, create_day FROM my_guestbook0");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MyGuestbook0));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].master = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].m_nickname, row[2], sizeof(records[count].m_nickname) - 1);
        records[count].m_id = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].m_user_id, row[4], sizeof(records[count].m_user_id) - 1);
        records[count].m_sex = row[5] ? row[5][0] : '\0';
        if (row[6]) strncpy(records[count].comment_text, row[6], sizeof(records[count].comment_text) - 1);
        records[count].emoticon = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].ip, row[8], sizeof(records[count].ip) - 1);
        records[count].create_day = row[9] ? atoi(row[9]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
