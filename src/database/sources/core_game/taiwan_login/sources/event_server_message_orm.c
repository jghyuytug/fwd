#include "event_server_message_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int EventServerMessage_Add(DBConnectionManager* manager, const EventServerMessage* msg) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !msg) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_server_message "
        "(server_info, channel_no, kind, message_index, charac_name, message, update_time) "
        "VALUES (%u, %u, '%c', '%c', '%s', '%s', %u)",
        msg->server_info, msg->channel_no, msg->kind, msg->message_index,
        msg->charac_name, msg->message, msg->update_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventServerMessage_Get(DBConnectionManager* manager,
                           unsigned char server_info,
                           unsigned char channel_no,
                           char message_index,
                           EventServerMessage* msg) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !msg) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, channel_no, kind, message_index, charac_name, message, update_time "
        "FROM event_server_message "
        "WHERE server_info = %u AND channel_no = %u AND message_index = '%c'",
        server_info, channel_no, message_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(msg, 0, sizeof(EventServerMessage));
    msg->server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
    msg->channel_no = row[1] ? (unsigned char)atoi(row[1]) : 0;
    msg->kind = row[2] ? row[2][0] : '\0';
    msg->message_index = row[3] ? row[3][0] : '\0';
    if (row[4]) strncpy(msg->charac_name, row[4], sizeof(msg->charac_name) - 1);
    if (row[5]) strncpy(msg->message, row[5], sizeof(msg->message) - 1);
    msg->update_time = row[6] ? (unsigned int)strtoul(row[6], NULL, 10) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventServerMessage_Update(DBConnectionManager* manager, const EventServerMessage* msg) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !msg) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_server_message SET "
        "kind = '%c', charac_name = '%s', message = '%s', update_time = %u "
        "WHERE server_info = %u AND channel_no = %u AND message_index = '%c'",
        msg->kind, msg->charac_name, msg->message, msg->update_time,
        msg->server_info, msg->channel_no, msg->message_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventServerMessage_Delete(DBConnectionManager* manager,
                               unsigned char server_info,
                               unsigned char channel_no,
                               char message_index) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_server_message "
        "WHERE server_info = %u AND channel_no = %u AND message_index = '%c'",
        server_info, channel_no, message_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventServerMessage_Exists(DBConnectionManager* manager,
                               unsigned char server_info,
                               unsigned char channel_no,
                               char message_index) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_server_message "
        "WHERE server_info = %u AND channel_no = %u AND message_index = '%c'",
        server_info, channel_no, message_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0 && row[0]) {
        int count = atoi(row[0]);
        DBQueryResult_Free(&result);
        return count > 0;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventServerMessage_GetAll(DBConnectionManager* manager,
                               EventServerMessage* messages,
                               int max_count,
                               int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !messages || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, channel_no, kind, message_index, charac_name, message, update_time "
        "FROM event_server_message "
        "ORDER BY server_info, channel_no, message_index");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&messages[count], 0, sizeof(EventServerMessage));
        messages[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        messages[count].channel_no = row[1] ? (unsigned char)atoi(row[1]) : 0;
        messages[count].kind = row[2] ? row[2][0] : '\0';
        messages[count].message_index = row[3] ? row[3][0] : '\0';
        if (row[4]) strncpy(messages[count].charac_name, row[4], sizeof(messages[count].charac_name) - 1);
        if (row[5]) strncpy(messages[count].message, row[5], sizeof(messages[count].message) - 1);
        messages[count].update_time = row[6] ? (unsigned int)strtoul(row[6], NULL, 10) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventServerMessage_GetByServer(DBConnectionManager* manager,
                                    unsigned char server_info,
                                    EventServerMessage* messages,
                                    int max_count,
                                    int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !messages || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, channel_no, kind, message_index, charac_name, message, update_time "
        "FROM event_server_message "
        "WHERE server_info = %u "
        "ORDER BY channel_no, message_index",
        server_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&messages[count], 0, sizeof(EventServerMessage));
        messages[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        messages[count].channel_no = row[1] ? (unsigned char)atoi(row[1]) : 0;
        messages[count].kind = row[2] ? row[2][0] : '\0';
        messages[count].message_index = row[3] ? row[3][0] : '\0';
        if (row[4]) strncpy(messages[count].charac_name, row[4], sizeof(messages[count].charac_name) - 1);
        if (row[5]) strncpy(messages[count].message, row[5], sizeof(messages[count].message) - 1);
        messages[count].update_time = row[6] ? (unsigned int)strtoul(row[6], NULL, 10) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventServerMessage_GetByChannel(DBConnectionManager* manager,
                                     unsigned char server_info,
                                     unsigned char channel_no,
                                     EventServerMessage* messages,
                                     int max_count,
                                     int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !messages || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, channel_no, kind, message_index, charac_name, message, update_time "
        "FROM event_server_message "
        "WHERE server_info = %u AND channel_no = %u "
        "ORDER BY message_index",
        server_info, channel_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&messages[count], 0, sizeof(EventServerMessage));
        messages[count].server_info = row[0] ? (unsigned char)atoi(row[0]) : 0;
        messages[count].channel_no = row[1] ? (unsigned char)atoi(row[1]) : 0;
        messages[count].kind = row[2] ? row[2][0] : '\0';
        messages[count].message_index = row[3] ? row[3][0] : '\0';
        if (row[4]) strncpy(messages[count].charac_name, row[4], sizeof(messages[count].charac_name) - 1);
        if (row[5]) strncpy(messages[count].message, row[5], sizeof(messages[count].message) - 1);
        messages[count].update_time = row[6] ? (unsigned int)strtoul(row[6], NULL, 10) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventServerMessage_DeleteAll(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM event_server_message");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void EventServerMessage_PrintInfo(const EventServerMessage* msg) {
    if (!msg) return;

    printf("EventServerMessage:\n");
    printf("  Server Info: %u\n", msg->server_info);
    printf("  Channel No: %u\n", msg->channel_no);
    printf("  Kind: %c\n", msg->kind);
    printf("  Message Index: %c\n", msg->message_index);
    printf("  Character Name: %s\n", msg->charac_name);
    printf("  Message: %s\n", msg->message);
    printf("  Update Time: %u\n", msg->update_time);
}
