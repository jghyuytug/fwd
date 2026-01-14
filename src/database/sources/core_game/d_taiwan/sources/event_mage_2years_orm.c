#include "event_mage_2years_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int EventMage2years_Add(DBConnectionManager* manager, const EventMage2years* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    /* Note: charac_no is AUTO_INCREMENT, handle create_time and delete_time */
    if (record->create_time[0] && record->delete_time[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO event_mage_2years (m_id, server_info, charac_name, create_time, delete_time, delete_flag) "
            "VALUES (%d, %u, '%s', '%s', '%s', %d)",
            record->m_id, record->server_info, record->charac_name,
            record->create_time, record->delete_time, record->delete_flag);
    } else if (record->create_time[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO event_mage_2years (m_id, server_info, charac_name, create_time, delete_time, delete_flag) "
            "VALUES (%d, %u, '%s', '%s', NOW(), %d)",
            record->m_id, record->server_info, record->charac_name,
            record->create_time, record->delete_flag);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO event_mage_2years (m_id, server_info, charac_name, create_time, delete_time, delete_flag) "
            "VALUES (%d, %u, '%s', NOW(), NOW(), %d)",
            record->m_id, record->server_info, record->charac_name, record->delete_flag);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventMage2years_Get(DBConnectionManager* manager, int m_id, unsigned char server_info,
                         int charac_no, EventMage2years* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_info, charac_no, charac_name, create_time, delete_time, delete_flag "
        "FROM event_mage_2years WHERE m_id = %d AND server_info = %u AND charac_no = %d",
        m_id, server_info, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventMage2years));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->server_info = row[1] ? (unsigned char)atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->charac_name, row[3], sizeof(record->charac_name) - 1);
    if (row[4]) strncpy(record->create_time, row[4], sizeof(record->create_time) - 1);
    if (row[5]) strncpy(record->delete_time, row[5], sizeof(record->delete_time) - 1);
    record->delete_flag = row[6] ? (char)atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventMage2years_Update(DBConnectionManager* manager, const EventMage2years* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_mage_2years SET charac_name = '%s', create_time = '%s', "
        "delete_time = '%s', delete_flag = %d "
        "WHERE m_id = %d AND server_info = %u AND charac_no = %d",
        record->charac_name, record->create_time, record->delete_time, record->delete_flag,
        record->m_id, record->server_info, record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventMage2years_Delete(DBConnectionManager* manager, int m_id, unsigned char server_info,
                            int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_mage_2years WHERE m_id = %d AND server_info = %u AND charac_no = %d",
        m_id, server_info, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventMage2years_Exists(DBConnectionManager* manager, int m_id, unsigned char server_info,
                            int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT 1 FROM event_mage_2years WHERE m_id = %d AND server_info = %u AND charac_no = %d LIMIT 1",
        m_id, server_info, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = 1;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int EventMage2years_GetByMemberId(DBConnectionManager* manager, int m_id,
                                   EventMage2years* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_info, charac_no, charac_name, create_time, delete_time, delete_flag "
        "FROM event_mage_2years WHERE m_id = %d ORDER BY charac_no",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventMage2years));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_info = row[1] ? (unsigned char)atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].charac_name, row[3], sizeof(records[count].charac_name) - 1);
        if (row[4]) strncpy(records[count].create_time, row[4], sizeof(records[count].create_time) - 1);
        if (row[5]) strncpy(records[count].delete_time, row[5], sizeof(records[count].delete_time) - 1);
        records[count].delete_flag = row[6] ? (char)atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventMage2years_GetByServerInfo(DBConnectionManager* manager, unsigned char server_info,
                                     EventMage2years* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_info, charac_no, charac_name, create_time, delete_time, delete_flag "
        "FROM event_mage_2years WHERE server_info = %u ORDER BY m_id, charac_no",
        server_info);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventMage2years));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_info = row[1] ? (unsigned char)atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].charac_name, row[3], sizeof(records[count].charac_name) - 1);
        if (row[4]) strncpy(records[count].create_time, row[4], sizeof(records[count].create_time) - 1);
        if (row[5]) strncpy(records[count].delete_time, row[5], sizeof(records[count].delete_time) - 1);
        records[count].delete_flag = row[6] ? (char)atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventMage2years_GetActiveCharacters(DBConnectionManager* manager,
                                         EventMage2years* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_info, charac_no, charac_name, create_time, delete_time, delete_flag "
        "FROM event_mage_2years WHERE delete_flag = 0 ORDER BY create_time DESC");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventMage2years));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].server_info = row[1] ? (unsigned char)atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].charac_name, row[3], sizeof(records[count].charac_name) - 1);
        if (row[4]) strncpy(records[count].create_time, row[4], sizeof(records[count].create_time) - 1);
        if (row[5]) strncpy(records[count].delete_time, row[5], sizeof(records[count].delete_time) - 1);
        records[count].delete_flag = row[6] ? (char)atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventMage2years_SoftDelete(DBConnectionManager* manager, int m_id, unsigned char server_info,
                                int charac_no, const char* delete_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    if (delete_time && delete_time[0]) {
        snprintf(query, sizeof(query),
            "UPDATE event_mage_2years SET delete_flag = 1, delete_time = '%s' "
            "WHERE m_id = %d AND server_info = %u AND charac_no = %d",
            delete_time, m_id, server_info, charac_no);
    } else {
        snprintf(query, sizeof(query),
            "UPDATE event_mage_2years SET delete_flag = 1, delete_time = NOW() "
            "WHERE m_id = %d AND server_info = %u AND charac_no = %d",
            m_id, server_info, charac_no);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void EventMage2years_PrintInfo(const EventMage2years* record) {
    if (!record) return;

    printf("EventMage2years:\n");
    printf("  m_id: %d\n", record->m_id);
    printf("  server_info: %u\n", record->server_info);
    printf("  charac_no: %d\n", record->charac_no);
    printf("  charac_name: %s\n", record->charac_name);
    printf("  create_time: %s\n", record->create_time);
    printf("  delete_time: %s\n", record->delete_time);
    printf("  delete_flag: %d\n", record->delete_flag);
}
