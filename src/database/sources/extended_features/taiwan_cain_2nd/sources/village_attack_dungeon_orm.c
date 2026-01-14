#include "village_attack_dungeon_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int VillageAttackDungeon_Add(DBConnectionManager* manager, const VillageAttackDungeon* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO village_attack_dungeon (occ_date, charac_no, attack_count, revenge_dungeon) "
        "VALUES ('%s', %d, %d, %d)",
        record->occ_date, record->charac_no, record->attack_count, record->revenge_dungeon);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int VillageAttackDungeon_Get(DBConnectionManager* manager, const char* occ_date, int charac_no, VillageAttackDungeon* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, charac_no, attack_count, revenge_dungeon FROM village_attack_dungeon WHERE occ_date = '%s' AND charac_no = %d",
        occ_date, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(VillageAttackDungeon));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->charac_no = row[1] ? atoi(row[1]) : 0;
    record->attack_count = row[2] ? atoi(row[2]) : 0;
    record->revenge_dungeon = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int VillageAttackDungeon_GetAll(DBConnectionManager* manager, VillageAttackDungeon* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, charac_no, attack_count, revenge_dungeon FROM village_attack_dungeon");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(VillageAttackDungeon));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].charac_no = row[1] ? atoi(row[1]) : 0;
        records[count].attack_count = row[2] ? atoi(row[2]) : 0;
        records[count].revenge_dungeon = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
