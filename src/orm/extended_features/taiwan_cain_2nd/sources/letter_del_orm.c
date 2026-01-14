#include "letter_del_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LetterDel_Add(DBConnectionManager* manager, const LetterDel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO letter_del (sdate, letter_id, charac_no, send_charac_no, send_charac_name, letter_text, reg_date, stat) "
        "VALUES ('%s', %d, %d, %d, '%s', '%s', '%s', %d)",
        record->sdate, record->letter_id, record->charac_no, record->send_charac_no, record->send_charac_name, record->letter_text, record->reg_date, record->stat);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LetterDel_Get(DBConnectionManager* manager, const char* sdate, int letter_id, LetterDel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT sdate, letter_id, charac_no, send_charac_no, send_charac_name, letter_text, reg_date, stat FROM letter_del WHERE sdate = '%s' AND letter_id = %d",
        sdate, letter_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LetterDel));
    if (row[0]) strncpy(record->sdate, row[0], sizeof(record->sdate) - 1);
    record->letter_id = row[1] ? atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    record->send_charac_no = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->send_charac_name, row[4], sizeof(record->send_charac_name) - 1);
    if (row[5]) strncpy(record->letter_text, row[5], sizeof(record->letter_text) - 1);
    if (row[6]) strncpy(record->reg_date, row[6], sizeof(record->reg_date) - 1);
    record->stat = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LetterDel_GetAll(DBConnectionManager* manager, LetterDel* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT sdate, letter_id, charac_no, send_charac_no, send_charac_name, letter_text, reg_date, stat FROM letter_del");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LetterDel));
        if (row[0]) strncpy(records[count].sdate, row[0], sizeof(records[count].sdate) - 1);
        records[count].letter_id = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        records[count].send_charac_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].send_charac_name, row[4], sizeof(records[count].send_charac_name) - 1);
        if (row[5]) strncpy(records[count].letter_text, row[5], sizeof(records[count].letter_text) - 1);
        if (row[6]) strncpy(records[count].reg_date, row[6], sizeof(records[count].reg_date) - 1);
        records[count].stat = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
