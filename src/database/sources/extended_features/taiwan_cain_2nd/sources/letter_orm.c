#include "letter_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Letter_Add(DBConnectionManager* manager, const Letter* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO letter (charac_no, send_charac_no, send_charac_name, letter_text, reg_date, stat) "
        "VALUES (%d, %d, '%s', '%s', '%s', %d)",
        record->charac_no, record->send_charac_no, record->send_charac_name, record->letter_text, record->reg_date, record->stat);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Letter_Get(DBConnectionManager* manager, int letter_id, Letter* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT letter_id, charac_no, send_charac_no, send_charac_name, letter_text, reg_date, stat FROM letter WHERE letter_id = %d",
        letter_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Letter));
    record->letter_id = row[0] ? atoi(row[0]) : 0;
    record->charac_no = row[1] ? atoi(row[1]) : 0;
    record->send_charac_no = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->send_charac_name, row[3], sizeof(record->send_charac_name) - 1);
    if (row[4]) strncpy(record->letter_text, row[4], sizeof(record->letter_text) - 1);
    if (row[5]) strncpy(record->reg_date, row[5], sizeof(record->reg_date) - 1);
    record->stat = row[6] ? atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int Letter_GetAll(DBConnectionManager* manager, Letter* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT letter_id, charac_no, send_charac_no, send_charac_name, letter_text, reg_date, stat FROM letter");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Letter));
        records[count].letter_id = row[0] ? atoi(row[0]) : 0;
        records[count].charac_no = row[1] ? atoi(row[1]) : 0;
        records[count].send_charac_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].send_charac_name, row[3], sizeof(records[count].send_charac_name) - 1);
        if (row[4]) strncpy(records[count].letter_text, row[4], sizeof(records[count].letter_text) - 1);
        if (row[5]) strncpy(records[count].reg_date, row[5], sizeof(records[count].reg_date) - 1);
        records[count].stat = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
