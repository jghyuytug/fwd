#include "fair_pvp_score_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int FairPvpScore_Add(DBConnectionManager* manager, const FairPvpScore* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO fair_pvp_score (charac_no, private_win, private_lose, private_draw, relay_battle_win, relay_battle_lose, relay_battle_draw, relay_battle_2kill, successive_win, relay_battle_3kill, max_successive_win, daily_play_count, last_play_time, give_item) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d)",
        record->charac_no, record->private_win, record->private_lose, record->private_draw, record->relay_battle_win, record->relay_battle_lose, record->relay_battle_draw, record->relay_battle_2kill, record->successive_win, record->relay_battle_3kill, record->max_successive_win, record->daily_play_count, record->last_play_time, record->give_item);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int FairPvpScore_Get(DBConnectionManager* manager, int charac_no, FairPvpScore* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, private_win, private_lose, private_draw, relay_battle_win, relay_battle_lose, relay_battle_draw, relay_battle_2kill, successive_win, relay_battle_3kill, max_successive_win, daily_play_count, last_play_time, give_item FROM fair_pvp_score WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(FairPvpScore));
    record->charac_no = row[0] ? atoi(row[0]) : 0;
    record->private_win = row[1] ? atoi(row[1]) : 0;
    record->private_lose = row[2] ? atoi(row[2]) : 0;
    record->private_draw = row[3] ? atoi(row[3]) : 0;
    record->relay_battle_win = row[4] ? atoi(row[4]) : 0;
    record->relay_battle_lose = row[5] ? atoi(row[5]) : 0;
    record->relay_battle_draw = row[6] ? atoi(row[6]) : 0;
    record->relay_battle_2kill = row[7] ? atoi(row[7]) : 0;
    record->successive_win = row[8] ? atoi(row[8]) : 0;
    record->relay_battle_3kill = row[9] ? atoi(row[9]) : 0;
    record->max_successive_win = row[10] ? atoi(row[10]) : 0;
    record->daily_play_count = row[11] ? atoi(row[11]) : 0;
    if (row[12]) strncpy(record->last_play_time, row[12], sizeof(record->last_play_time) - 1);
    record->give_item = row[13] ? atoi(row[13]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int FairPvpScore_GetAll(DBConnectionManager* manager, FairPvpScore* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, private_win, private_lose, private_draw, relay_battle_win, relay_battle_lose, relay_battle_draw, relay_battle_2kill, successive_win, relay_battle_3kill, max_successive_win, daily_play_count, last_play_time, give_item FROM fair_pvp_score");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(FairPvpScore));
        records[count].charac_no = row[0] ? atoi(row[0]) : 0;
        records[count].private_win = row[1] ? atoi(row[1]) : 0;
        records[count].private_lose = row[2] ? atoi(row[2]) : 0;
        records[count].private_draw = row[3] ? atoi(row[3]) : 0;
        records[count].relay_battle_win = row[4] ? atoi(row[4]) : 0;
        records[count].relay_battle_lose = row[5] ? atoi(row[5]) : 0;
        records[count].relay_battle_draw = row[6] ? atoi(row[6]) : 0;
        records[count].relay_battle_2kill = row[7] ? atoi(row[7]) : 0;
        records[count].successive_win = row[8] ? atoi(row[8]) : 0;
        records[count].relay_battle_3kill = row[9] ? atoi(row[9]) : 0;
        records[count].max_successive_win = row[10] ? atoi(row[10]) : 0;
        records[count].daily_play_count = row[11] ? atoi(row[11]) : 0;
        if (row[12]) strncpy(records[count].last_play_time, row[12], sizeof(records[count].last_play_time) - 1);
        records[count].give_item = row[13] ? atoi(row[13]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
