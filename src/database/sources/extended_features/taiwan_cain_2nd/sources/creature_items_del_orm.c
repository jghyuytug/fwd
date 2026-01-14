#include "creature_items_del_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CreatureItemsDel_Add(DBConnectionManager* manager, const CreatureItemsDel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO creature_items_del (sdate, ui_id, charac_no, slot, it_id, reg_date, name, stomach, exp, endurance, creature_type, no_charge, stat, item_lock_key, ipg_agency_no, expire_date, delete_date) "
        "VALUES ('%s', %d, %d, %d, %d, '%s', '%s', %d, %d, %d, %d, %d, %d, %d, '%s', '%s', '%s')",
        record->sdate, record->ui_id, record->charac_no, record->slot, record->it_id, record->reg_date, record->name, record->stomach, record->exp, record->endurance, record->creature_type, record->no_charge, record->stat, record->item_lock_key, record->ipg_agency_no, record->expire_date, record->delete_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CreatureItemsDel_Get(DBConnectionManager* manager, const char* sdate, int ui_id, CreatureItemsDel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT sdate, ui_id, charac_no, slot, it_id, reg_date, name, stomach, exp, endurance, creature_type, no_charge, stat, item_lock_key, ipg_agency_no, expire_date, delete_date FROM creature_items_del WHERE sdate = '%s' AND ui_id = %d",
        sdate, ui_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CreatureItemsDel));
    if (row[0]) strncpy(record->sdate, row[0], sizeof(record->sdate) - 1);
    record->ui_id = row[1] ? atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    record->slot = row[3] ? atoi(row[3]) : 0;
    record->it_id = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->reg_date, row[5], sizeof(record->reg_date) - 1);
    if (row[6]) strncpy(record->name, row[6], sizeof(record->name) - 1);
    record->stomach = row[7] ? atoi(row[7]) : 0;
    record->exp = row[8] ? atoi(row[8]) : 0;
    record->endurance = row[9] ? atoi(row[9]) : 0;
    record->creature_type = row[10] ? atoi(row[10]) : 0;
    record->no_charge = row[11] ? atoi(row[11]) : 0;
    record->stat = row[12] ? atoi(row[12]) : 0;
    record->item_lock_key = row[13] ? atoi(row[13]) : 0;
    if (row[14]) strncpy(record->ipg_agency_no, row[14], sizeof(record->ipg_agency_no) - 1);
    if (row[15]) strncpy(record->expire_date, row[15], sizeof(record->expire_date) - 1);
    if (row[16]) strncpy(record->delete_date, row[16], sizeof(record->delete_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int CreatureItemsDel_GetAll(DBConnectionManager* manager, CreatureItemsDel* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[17];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT sdate, ui_id, charac_no, slot, it_id, reg_date, name, stomach, exp, endurance, creature_type, no_charge, stat, item_lock_key, ipg_agency_no, expire_date, delete_date FROM creature_items_del");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CreatureItemsDel));
        if (row[0]) strncpy(records[count].sdate, row[0], sizeof(records[count].sdate) - 1);
        records[count].ui_id = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        records[count].slot = row[3] ? atoi(row[3]) : 0;
        records[count].it_id = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].reg_date, row[5], sizeof(records[count].reg_date) - 1);
        if (row[6]) strncpy(records[count].name, row[6], sizeof(records[count].name) - 1);
        records[count].stomach = row[7] ? atoi(row[7]) : 0;
        records[count].exp = row[8] ? atoi(row[8]) : 0;
        records[count].endurance = row[9] ? atoi(row[9]) : 0;
        records[count].creature_type = row[10] ? atoi(row[10]) : 0;
        records[count].no_charge = row[11] ? atoi(row[11]) : 0;
        records[count].stat = row[12] ? atoi(row[12]) : 0;
        records[count].item_lock_key = row[13] ? atoi(row[13]) : 0;
        if (row[14]) strncpy(records[count].ipg_agency_no, row[14], sizeof(records[count].ipg_agency_no) - 1);
        if (row[15]) strncpy(records[count].expire_date, row[15], sizeof(records[count].expire_date) - 1);
        if (row[16]) strncpy(records[count].delete_date, row[16], sizeof(records[count].delete_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
