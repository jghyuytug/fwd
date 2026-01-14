#include "admin_member_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AdminMember_Add(DBConnectionManager* manager, const AdminMember* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO admin_member (user_id, password, name, email, phone, msn, comment, reg_date, confirm, level, level_group1, level_group2, level_group3, level_group4, level_group5, level_group6) "
        "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
        record->user_id, record->password, record->name, record->email, record->phone, record->msn, record->comment, record->reg_date, record->confirm, record->level, record->level_group1, record->level_group2, record->level_group3, record->level_group4, record->level_group5, record->level_group6);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AdminMember_Get(DBConnectionManager* manager, int no, AdminMember* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, user_id, password, name, email, phone, msn, comment, reg_date, confirm, level, level_group1, level_group2, level_group3, level_group4, level_group5, level_group6 FROM admin_member WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AdminMember));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->user_id, row[1], sizeof(record->user_id) - 1);
    if (row[2]) strncpy(record->password, row[2], sizeof(record->password) - 1);
    if (row[3]) strncpy(record->name, row[3], sizeof(record->name) - 1);
    if (row[4]) strncpy(record->email, row[4], sizeof(record->email) - 1);
    if (row[5]) strncpy(record->phone, row[5], sizeof(record->phone) - 1);
    if (row[6]) strncpy(record->msn, row[6], sizeof(record->msn) - 1);
    if (row[7]) strncpy(record->comment, row[7], sizeof(record->comment) - 1);
    record->reg_date = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->confirm, row[9], sizeof(record->confirm) - 1);
    if (row[10]) strncpy(record->level, row[10], sizeof(record->level) - 1);
    if (row[11]) strncpy(record->level_group1, row[11], sizeof(record->level_group1) - 1);
    if (row[12]) strncpy(record->level_group2, row[12], sizeof(record->level_group2) - 1);
    if (row[13]) strncpy(record->level_group3, row[13], sizeof(record->level_group3) - 1);
    if (row[14]) strncpy(record->level_group4, row[14], sizeof(record->level_group4) - 1);
    if (row[15]) strncpy(record->level_group5, row[15], sizeof(record->level_group5) - 1);
    if (row[16]) strncpy(record->level_group6, row[16], sizeof(record->level_group6) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int AdminMember_GetAll(DBConnectionManager* manager, AdminMember* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, user_id, password, name, email, phone, msn, comment, reg_date, confirm, level, level_group1, level_group2, level_group3, level_group4, level_group5, level_group6 FROM admin_member");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AdminMember));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].user_id, row[1], sizeof(records[count].user_id) - 1);
        if (row[2]) strncpy(records[count].password, row[2], sizeof(records[count].password) - 1);
        if (row[3]) strncpy(records[count].name, row[3], sizeof(records[count].name) - 1);
        if (row[4]) strncpy(records[count].email, row[4], sizeof(records[count].email) - 1);
        if (row[5]) strncpy(records[count].phone, row[5], sizeof(records[count].phone) - 1);
        if (row[6]) strncpy(records[count].msn, row[6], sizeof(records[count].msn) - 1);
        if (row[7]) strncpy(records[count].comment, row[7], sizeof(records[count].comment) - 1);
        records[count].reg_date = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].confirm, row[9], sizeof(records[count].confirm) - 1);
        if (row[10]) strncpy(records[count].level, row[10], sizeof(records[count].level) - 1);
        if (row[11]) strncpy(records[count].level_group1, row[11], sizeof(records[count].level_group1) - 1);
        if (row[12]) strncpy(records[count].level_group2, row[12], sizeof(records[count].level_group2) - 1);
        if (row[13]) strncpy(records[count].level_group3, row[13], sizeof(records[count].level_group3) - 1);
        if (row[14]) strncpy(records[count].level_group4, row[14], sizeof(records[count].level_group4) - 1);
        if (row[15]) strncpy(records[count].level_group5, row[15], sizeof(records[count].level_group5) - 1);
        if (row[16]) strncpy(records[count].level_group6, row[16], sizeof(records[count].level_group6) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
