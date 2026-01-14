#include "opencast_editor_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int OpencastEditor_Add(DBConnectionManager* manager, const OpencastEditor* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO opencast_editor (m_id, reg_time, cast_name, cate_no, cast_intro, name, homepage, contact, email, editor_intro, mng_type, order_num, grade) "
        "VALUES (%d, '%s', '%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', %d, %d, %d)",
        record->m_id, record->reg_time, record->cast_name, record->cate_no, record->cast_intro, record->name, record->homepage, record->contact, record->email, record->editor_intro, record->mng_type, record->order_num, record->grade);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int OpencastEditor_Get(DBConnectionManager* manager, int m_id, OpencastEditor* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, reg_time, cast_name, cate_no, cast_intro, name, homepage, contact, email, editor_intro, mng_type, order_num, grade FROM opencast_editor WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(OpencastEditor));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->reg_time, row[1], sizeof(record->reg_time) - 1);
    if (row[2]) strncpy(record->cast_name, row[2], sizeof(record->cast_name) - 1);
    record->cate_no = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->cast_intro, row[4], sizeof(record->cast_intro) - 1);
    if (row[5]) strncpy(record->name, row[5], sizeof(record->name) - 1);
    if (row[6]) strncpy(record->homepage, row[6], sizeof(record->homepage) - 1);
    if (row[7]) strncpy(record->contact, row[7], sizeof(record->contact) - 1);
    if (row[8]) strncpy(record->email, row[8], sizeof(record->email) - 1);
    if (row[9]) strncpy(record->editor_intro, row[9], sizeof(record->editor_intro) - 1);
    record->mng_type = row[10] ? atoi(row[10]) : 0;
    record->order_num = row[11] ? atoi(row[11]) : 0;
    record->grade = row[12] ? atoi(row[12]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int OpencastEditor_GetAll(DBConnectionManager* manager, OpencastEditor* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, reg_time, cast_name, cate_no, cast_intro, name, homepage, contact, email, editor_intro, mng_type, order_num, grade FROM opencast_editor");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(OpencastEditor));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].reg_time, row[1], sizeof(records[count].reg_time) - 1);
        if (row[2]) strncpy(records[count].cast_name, row[2], sizeof(records[count].cast_name) - 1);
        records[count].cate_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].cast_intro, row[4], sizeof(records[count].cast_intro) - 1);
        if (row[5]) strncpy(records[count].name, row[5], sizeof(records[count].name) - 1);
        if (row[6]) strncpy(records[count].homepage, row[6], sizeof(records[count].homepage) - 1);
        if (row[7]) strncpy(records[count].contact, row[7], sizeof(records[count].contact) - 1);
        if (row[8]) strncpy(records[count].email, row[8], sizeof(records[count].email) - 1);
        if (row[9]) strncpy(records[count].editor_intro, row[9], sizeof(records[count].editor_intro) - 1);
        records[count].mng_type = row[10] ? atoi(row[10]) : 0;
        records[count].order_num = row[11] ? atoi(row[11]) : 0;
        records[count].grade = row[12] ? atoi(row[12]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
