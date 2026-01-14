#include "db/db_quest.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

#include "game_server_context.h"

int db_save_quest_progress(uint32_t charac_no, uint32_t quest_id, uint8_t state, uint16_t progress) {
    if (!g_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "REPLACE INTO quest_log (charac_no, quest_id, state, progress, update_time) "
             "VALUES (%u, %u, %u, %u, NOW())",
             charac_no, quest_id, state, progress);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Save quest progress failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    printf("[DB] Saved quest progress: charac=%u, quest=%u, state=%u, progress=%u\n",
           charac_no, quest_id, state, progress);
    return 0;
}

int db_load_quest_progress(uint32_t charac_no, DBQuestProgress* quests, int max_count) {
    if (!g_db_charac2 || !quests) return 0;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT quest_id, state, progress FROM quest_log "
             "WHERE charac_no=%u AND state IN (1,2) ORDER BY quest_id LIMIT %d",
             charac_no, max_count);

    if (mysql_query(g_db_charac2, query)) {
        return 0;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) return 0;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) && count < max_count) {
        quests[count].quest_id = row[0] ? atoi(row[0]) : 0;
        quests[count].state = row[1] ? atoi(row[1]) : 0;
        quests[count].progress = row[2] ? atoi(row[2]) : 0;
        quests[count].goal = 10;
        count++;
    }

    mysql_free_result(result);
    printf("[DB] Loaded %d quest progress entries for charac=%u\n", count, charac_no);
    return count;
}

int db_load_daily_quests(uint32_t charac_no, DailyQuestProgress* quests, int max_count) {
    if (!g_db_charac || !quests) return -1;

    memset(quests, 0, sizeof(DailyQuestProgress) * max_count);

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT quest_id, progress, completed, claimed FROM daily_quests "
             "WHERE charac_no=%u AND DATE(assign_date)=CURDATE()",
             charac_no);

    if (mysql_query(g_db_charac, query)) {
        return 0;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return 0;

    MYSQL_ROW row;
    int count = 0;
    while ((row = mysql_fetch_row(result)) != NULL && count < max_count) {
        quests[count].quest_id = (uint16_t)atoi(row[0]);
        quests[count].progress = (uint32_t)atoi(row[1]);
        quests[count].completed = (uint8_t)atoi(row[2]);
        quests[count].claimed = (uint8_t)atoi(row[3]);
        count++;
    }

    mysql_free_result(result);
    return count;
}

int db_save_daily_quest(uint32_t charac_no, uint16_t quest_id, uint32_t progress,
                        uint8_t completed, uint8_t claimed) {
    if (!g_db_charac) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "REPLACE INTO daily_quests (charac_no, quest_id, assign_date, progress, completed, claimed) "
             "VALUES (%u, %u, CURDATE(), %u, %u, %u)",
             charac_no, quest_id, progress, completed, claimed);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Save daily quest failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }
    return 0;
}

int db_load_weekly_quests(uint32_t charac_no, DailyQuestProgress* quests, int max_count) {
    if (!g_db_charac || !quests) return -1;

    memset(quests, 0, sizeof(DailyQuestProgress) * max_count);

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT quest_id, progress, completed, claimed FROM weekly_quests "
             "WHERE charac_no=%u AND WEEK(assign_date, 1)=WEEK(CURDATE(), 1)",
             charac_no);

    if (mysql_query(g_db_charac, query)) {
        return 0;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return 0;

    MYSQL_ROW row;
    int count = 0;
    while ((row = mysql_fetch_row(result)) != NULL && count < max_count) {
        quests[count].quest_id = (uint16_t)atoi(row[0]);
        quests[count].progress = (uint32_t)atoi(row[1]);
        quests[count].completed = (uint8_t)atoi(row[2]);
        quests[count].claimed = (uint8_t)atoi(row[3]);
        count++;
    }

    mysql_free_result(result);
    return count;
}

int db_save_weekly_quest(uint32_t charac_no, uint16_t quest_id, uint32_t progress,
                         uint8_t completed, uint8_t claimed) {
    if (!g_db_charac) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "REPLACE INTO weekly_quests (charac_no, quest_id, assign_date, progress, completed, claimed) "
             "VALUES (%u, %u, CURDATE(), %u, %u, %u)",
             charac_no, quest_id, progress, completed, claimed);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Save weekly quest failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }
    return 0;
}
