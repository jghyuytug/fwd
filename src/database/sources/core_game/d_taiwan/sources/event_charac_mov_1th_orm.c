#include "event_charac_mov_1th_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int EventCharacMov1th_Add(DBConnectionManager* manager, const EventCharacMov1th* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    if (record->reg_date[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO event_charac_mov_1th "
            "(m_id, server_id, charac_no, move_server_id, move_charac_no, move_check, event_code, reg_date) "
            "VALUES (%d, %u, %d, %u, %d, %d, '%s', '%s')",
            record->m_id, record->server_id, record->charac_no,
            record->move_server_id, record->move_charac_no, record->move_check,
            record->event_code, record->reg_date);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO event_charac_mov_1th "
            "(m_id, server_id, charac_no, move_server_id, move_charac_no, move_check, event_code, reg_date) "
            "VALUES (%d, %u, %d, %u, %d, %d, '%s', NOW())",
            record->m_id, record->server_id, record->charac_no,
            record->move_server_id, record->move_charac_no, record->move_check,
            record->event_code);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventCharacMov1th_Get(DBConnectionManager* manager, unsigned int id, EventCharacMov1th* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, m_id, server_id, charac_no, move_server_id, move_charac_no, "
        "move_check, event_code, reg_date "
        "FROM event_charac_mov_1th WHERE id = %u", id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventCharacMov1th));
    record->id = row[0] ? (unsigned int)atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
    record->charac_no = row[3] ? atoi(row[3]) : 0;
    record->move_server_id = row[4] ? (unsigned char)atoi(row[4]) : 0;
    record->move_charac_no = row[5] ? atoi(row[5]) : 0;
    record->move_check = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->event_code, row[7], sizeof(record->event_code) - 1);
    if (row[8]) strncpy(record->reg_date, row[8], sizeof(record->reg_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int EventCharacMov1th_Update(DBConnectionManager* manager, const EventCharacMov1th* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_charac_mov_1th SET "
        "m_id = %d, server_id = %u, charac_no = %d, "
        "move_server_id = %u, move_charac_no = %d, move_check = %d, "
        "event_code = '%s', reg_date = '%s' "
        "WHERE id = %u",
        record->m_id, record->server_id, record->charac_no,
        record->move_server_id, record->move_charac_no, record->move_check,
        record->event_code, record->reg_date, record->id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventCharacMov1th_Delete(DBConnectionManager* manager, unsigned int id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_charac_mov_1th WHERE id = %u", id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventCharacMov1th_Exists(DBConnectionManager* manager, unsigned int id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT 1 FROM event_charac_mov_1th WHERE id = %u", id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return 0;

    int exists = (DBQueryResult_FetchRow(&result, row) > 0);
    DBQueryResult_Free(&result);
    return exists;
}

int EventCharacMov1th_GetByMember(DBConnectionManager* manager, int m_id,
                                   EventCharacMov1th* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, m_id, server_id, charac_no, move_server_id, move_charac_no, "
        "move_check, event_code, reg_date "
        "FROM event_charac_mov_1th WHERE m_id = %d ORDER BY id", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventCharacMov1th));
        records[count].id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        records[count].charac_no = row[3] ? atoi(row[3]) : 0;
        records[count].move_server_id = row[4] ? (unsigned char)atoi(row[4]) : 0;
        records[count].move_charac_no = row[5] ? atoi(row[5]) : 0;
        records[count].move_check = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].event_code, row[7], sizeof(records[count].event_code) - 1);
        if (row[8]) strncpy(records[count].reg_date, row[8], sizeof(records[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventCharacMov1th_GetByCharacter(DBConnectionManager* manager, int m_id,
                                      unsigned char server_id, int charac_no,
                                      EventCharacMov1th* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, m_id, server_id, charac_no, move_server_id, move_charac_no, "
        "move_check, event_code, reg_date "
        "FROM event_charac_mov_1th WHERE m_id = %d AND server_id = %u AND charac_no = %d",
        m_id, server_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventCharacMov1th));
    record->id = row[0] ? (unsigned int)atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
    record->charac_no = row[3] ? atoi(row[3]) : 0;
    record->move_server_id = row[4] ? (unsigned char)atoi(row[4]) : 0;
    record->move_charac_no = row[5] ? atoi(row[5]) : 0;
    record->move_check = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->event_code, row[7], sizeof(record->event_code) - 1);
    if (row[8]) strncpy(record->reg_date, row[8], sizeof(record->reg_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int EventCharacMov1th_GetByEventCode(DBConnectionManager* manager, const char* event_code,
                                      EventCharacMov1th* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !event_code || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, m_id, server_id, charac_no, move_server_id, move_charac_no, "
        "move_check, event_code, reg_date "
        "FROM event_charac_mov_1th WHERE event_code = '%s' ORDER BY id", event_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventCharacMov1th));
        records[count].id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        records[count].charac_no = row[3] ? atoi(row[3]) : 0;
        records[count].move_server_id = row[4] ? (unsigned char)atoi(row[4]) : 0;
        records[count].move_charac_no = row[5] ? atoi(row[5]) : 0;
        records[count].move_check = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].event_code, row[7], sizeof(records[count].event_code) - 1);
        if (row[8]) strncpy(records[count].reg_date, row[8], sizeof(records[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventCharacMov1th_GetPendingMoves(DBConnectionManager* manager,
                                       EventCharacMov1th* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, m_id, server_id, charac_no, move_server_id, move_charac_no, "
        "move_check, event_code, reg_date "
        "FROM event_charac_mov_1th WHERE move_check = 0 ORDER BY id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventCharacMov1th));
        records[count].id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        records[count].charac_no = row[3] ? atoi(row[3]) : 0;
        records[count].move_server_id = row[4] ? (unsigned char)atoi(row[4]) : 0;
        records[count].move_charac_no = row[5] ? atoi(row[5]) : 0;
        records[count].move_check = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].event_code, row[7], sizeof(records[count].event_code) - 1);
        if (row[8]) strncpy(records[count].reg_date, row[8], sizeof(records[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventCharacMov1th_UpdateMoveCheck(DBConnectionManager* manager, unsigned int id, int move_check) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_charac_mov_1th SET move_check = %d WHERE id = %u",
        move_check, id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventCharacMov1th_CountByMember(DBConnectionManager* manager, int m_id, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !count) return -1;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_charac_mov_1th WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return 0;
}

void EventCharacMov1th_PrintInfo(const EventCharacMov1th* record) {
    if (!record) return;

    printf("=== Event Character Move 1th Record ===\n");
    printf("ID: %u\n", record->id);
    printf("Member ID: %d\n", record->m_id);
    printf("Server ID: %u -> %u\n", record->server_id, record->move_server_id);
    printf("Character No: %d -> %d\n", record->charac_no, record->move_charac_no);
    printf("Move Check: %d\n", record->move_check);
    printf("Event Code: %s\n", record->event_code);
    printf("Register Date: %s\n", record->reg_date);
    printf("=====================================\n");
}
