#include "log_point_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogPointHistory_Add(DBConnectionManager* manager, const LogPointHistory* log) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !log) return -1;

    /* AUTO_INCREMENT field 'no' is not specified in INSERT */
    if (log->reg_date[0]) {
        /* User provided timestamp */
        snprintf(query, sizeof(query),
            "INSERT INTO log_point_history (account_id, charac_id, cera_point, "
            "command, charge_type, free_charge_type, item_id, query_user, reg_date) "
            "VALUES ('%s', '%s', %u, '%c', %d, %d, %u, '%s', '%s')",
            log->account_id, log->charac_id, log->cera_point, log->command,
            log->charge_type, log->free_charge_type, log->item_id,
            log->query_user, log->reg_date);
    } else {
        /* Use database current timestamp */
        snprintf(query, sizeof(query),
            "INSERT INTO log_point_history (account_id, charac_id, cera_point, "
            "command, charge_type, free_charge_type, item_id, query_user, reg_date) "
            "VALUES ('%s', '%s', %u, '%c', %d, %d, %u, '%s', NOW())",
            log->account_id, log->charac_id, log->cera_point, log->command,
            log->charge_type, log->free_charge_type, log->item_id,
            log->query_user);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogPointHistory_Get(DBConnectionManager* manager, unsigned int no, LogPointHistory* log) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !log) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, account_id, charac_id, cera_point, command, charge_type, "
        "free_charge_type, item_id, query_user, reg_date FROM log_point_history WHERE no = %u",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(log, 0, sizeof(LogPointHistory));
    log->no = row[0] ? (unsigned int)atoi(row[0]) : 0;
    if (row[1]) strncpy(log->account_id, row[1], sizeof(log->account_id) - 1);
    if (row[2]) strncpy(log->charac_id, row[2], sizeof(log->charac_id) - 1);
    log->cera_point = row[3] ? (unsigned int)atoi(row[3]) : 0;
    log->command = row[4] ? row[4][0] : 'A';
    log->charge_type = row[5] ? (char)atoi(row[5]) : 0;
    log->free_charge_type = row[6] ? (char)atoi(row[6]) : 0;
    log->item_id = row[7] ? (unsigned int)atoi(row[7]) : 0;
    if (row[8]) strncpy(log->query_user, row[8], sizeof(log->query_user) - 1);
    if (row[9]) strncpy(log->reg_date, row[9], sizeof(log->reg_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int LogPointHistory_Update(DBConnectionManager* manager, const LogPointHistory* log) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !log) return -1;

    snprintf(query, sizeof(query),
        "UPDATE log_point_history SET account_id = '%s', charac_id = '%s', "
        "cera_point = %u, command = '%c', charge_type = %d, free_charge_type = %d, "
        "item_id = %u, query_user = '%s', reg_date = '%s' WHERE no = %u",
        log->account_id, log->charac_id, log->cera_point, log->command,
        log->charge_type, log->free_charge_type, log->item_id,
        log->query_user, log->reg_date, log->no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogPointHistory_Delete(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM log_point_history WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LogPointHistory_Exists(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM log_point_history WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int LogPointHistory_GetAll(DBConnectionManager* manager, LogPointHistory* logs,
                            int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, account_id, charac_id, cera_point, command, charge_type, "
        "free_charge_type, item_id, query_user, reg_date FROM log_point_history "
        "ORDER BY reg_date DESC LIMIT %d", max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogPointHistory));
        logs[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) strncpy(logs[count].account_id, row[1], sizeof(logs[count].account_id) - 1);
        if (row[2]) strncpy(logs[count].charac_id, row[2], sizeof(logs[count].charac_id) - 1);
        logs[count].cera_point = row[3] ? (unsigned int)atoi(row[3]) : 0;
        logs[count].command = row[4] ? row[4][0] : 'A';
        logs[count].charge_type = row[5] ? (char)atoi(row[5]) : 0;
        logs[count].free_charge_type = row[6] ? (char)atoi(row[6]) : 0;
        logs[count].item_id = row[7] ? (unsigned int)atoi(row[7]) : 0;
        if (row[8]) strncpy(logs[count].query_user, row[8], sizeof(logs[count].query_user) - 1);
        if (row[9]) strncpy(logs[count].reg_date, row[9], sizeof(logs[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogPointHistory_GetByAccount(DBConnectionManager* manager, const char* account_id,
                                  LogPointHistory* logs, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !account_id || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, account_id, charac_id, cera_point, command, charge_type, "
        "free_charge_type, item_id, query_user, reg_date FROM log_point_history "
        "WHERE account_id = '%s' ORDER BY reg_date DESC LIMIT %d",
        account_id, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogPointHistory));
        logs[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) strncpy(logs[count].account_id, row[1], sizeof(logs[count].account_id) - 1);
        if (row[2]) strncpy(logs[count].charac_id, row[2], sizeof(logs[count].charac_id) - 1);
        logs[count].cera_point = row[3] ? (unsigned int)atoi(row[3]) : 0;
        logs[count].command = row[4] ? row[4][0] : 'A';
        logs[count].charge_type = row[5] ? (char)atoi(row[5]) : 0;
        logs[count].free_charge_type = row[6] ? (char)atoi(row[6]) : 0;
        logs[count].item_id = row[7] ? (unsigned int)atoi(row[7]) : 0;
        if (row[8]) strncpy(logs[count].query_user, row[8], sizeof(logs[count].query_user) - 1);
        if (row[9]) strncpy(logs[count].reg_date, row[9], sizeof(logs[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogPointHistory_GetByCommand(DBConnectionManager* manager, char command,
                                  LogPointHistory* logs, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, account_id, charac_id, cera_point, command, charge_type, "
        "free_charge_type, item_id, query_user, reg_date FROM log_point_history "
        "WHERE command = '%c' ORDER BY reg_date DESC LIMIT %d",
        command, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogPointHistory));
        logs[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) strncpy(logs[count].account_id, row[1], sizeof(logs[count].account_id) - 1);
        if (row[2]) strncpy(logs[count].charac_id, row[2], sizeof(logs[count].charac_id) - 1);
        logs[count].cera_point = row[3] ? (unsigned int)atoi(row[3]) : 0;
        logs[count].command = row[4] ? row[4][0] : 'A';
        logs[count].charge_type = row[5] ? (char)atoi(row[5]) : 0;
        logs[count].free_charge_type = row[6] ? (char)atoi(row[6]) : 0;
        logs[count].item_id = row[7] ? (unsigned int)atoi(row[7]) : 0;
        if (row[8]) strncpy(logs[count].query_user, row[8], sizeof(logs[count].query_user) - 1);
        if (row[9]) strncpy(logs[count].reg_date, row[9], sizeof(logs[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LogPointHistory_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                    const char* end_date, LogPointHistory* logs,
                                    int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !start_date || !end_date || !logs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, account_id, charac_id, cera_point, command, charge_type, "
        "free_charge_type, item_id, query_user, reg_date FROM log_point_history "
        "WHERE reg_date BETWEEN '%s' AND '%s' ORDER BY reg_date DESC LIMIT %d",
        start_date, end_date, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(LogPointHistory));
        logs[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) strncpy(logs[count].account_id, row[1], sizeof(logs[count].account_id) - 1);
        if (row[2]) strncpy(logs[count].charac_id, row[2], sizeof(logs[count].charac_id) - 1);
        logs[count].cera_point = row[3] ? (unsigned int)atoi(row[3]) : 0;
        logs[count].command = row[4] ? row[4][0] : 'A';
        logs[count].charge_type = row[5] ? (char)atoi(row[5]) : 0;
        logs[count].free_charge_type = row[6] ? (char)atoi(row[6]) : 0;
        logs[count].item_id = row[7] ? (unsigned int)atoi(row[7]) : 0;
        if (row[8]) strncpy(logs[count].query_user, row[8], sizeof(logs[count].query_user) - 1);
        if (row[9]) strncpy(logs[count].reg_date, row[9], sizeof(logs[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void LogPointHistory_PrintInfo(const LogPointHistory* log) {
    if (!log) {
        printf("LogPointHistory: NULL\n");
        return;
    }

    printf("LogPointHistory:\n");
    printf("  no: %u\n", log->no);
    printf("  account_id: %s\n", log->account_id);
    printf("  charac_id: %s\n", log->charac_id);
    printf("  cera_point: %u\n", log->cera_point);
    printf("  command: %c\n", log->command);
    printf("  charge_type: %d\n", log->charge_type);
    printf("  free_charge_type: %d\n", log->free_charge_type);
    printf("  item_id: %u\n", log->item_id);
    printf("  query_user: %s\n", log->query_user);
    printf("  reg_date: %s\n", log->reg_date);
}
