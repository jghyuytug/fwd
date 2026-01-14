#include "event_used_fatigue_at_mage_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int EventUsedFatigueAtMage_Add(DBConnectionManager* manager, const EventUsedFatigueAtMage* event) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !event) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_used_fatigue_at_mage (charac_no, fatigue_quantity) "
        "VALUES (%u, %u)",
        event->charac_no, event->fatigue_quantity);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventUsedFatigueAtMage_Get(DBConnectionManager* manager, unsigned int charac_no,
                                EventUsedFatigueAtMage* event) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !event) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, fatigue_quantity FROM event_used_fatigue_at_mage "
        "WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(event, 0, sizeof(EventUsedFatigueAtMage));
    event->charac_no = row[0] ? (unsigned int)atoi(row[0]) : 0;
    event->fatigue_quantity = row[1] ? (unsigned int)atoi(row[1]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventUsedFatigueAtMage_Update(DBConnectionManager* manager, const EventUsedFatigueAtMage* event) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !event) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_used_fatigue_at_mage SET fatigue_quantity = %u "
        "WHERE charac_no = %u",
        event->fatigue_quantity, event->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventUsedFatigueAtMage_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_used_fatigue_at_mage WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventUsedFatigueAtMage_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_used_fatigue_at_mage WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count > 0;
}

int EventUsedFatigueAtMage_GetAll(DBConnectionManager* manager, EventUsedFatigueAtMage* events,
                                   int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !events || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, fatigue_quantity FROM event_used_fatigue_at_mage "
        "ORDER BY fatigue_quantity DESC LIMIT %d",
        max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&events[count], 0, sizeof(EventUsedFatigueAtMage));
        events[count].charac_no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        events[count].fatigue_quantity = row[1] ? (unsigned int)atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventUsedFatigueAtMage_GetByFatigueRange(DBConnectionManager* manager,
                                              unsigned int min_fatigue, unsigned int max_fatigue,
                                              EventUsedFatigueAtMage* events,
                                              int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !events || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, fatigue_quantity FROM event_used_fatigue_at_mage "
        "WHERE fatigue_quantity BETWEEN %u AND %u "
        "ORDER BY fatigue_quantity DESC",
        min_fatigue, max_fatigue);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&events[count], 0, sizeof(EventUsedFatigueAtMage));
        events[count].charac_no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        events[count].fatigue_quantity = row[1] ? (unsigned int)atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventUsedFatigueAtMage_IncreaseFatigue(DBConnectionManager* manager,
                                            unsigned int charac_no, unsigned int amount) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || amount == 0) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_used_fatigue_at_mage SET fatigue_quantity = fatigue_quantity + %u "
        "WHERE charac_no = %u",
        amount, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventUsedFatigueAtMage_GetTotalFatigue(DBConnectionManager* manager, unsigned long long* total) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !total) return -1;

    snprintf(query, sizeof(query),
        "SELECT SUM(fatigue_quantity) FROM event_used_fatigue_at_mage");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *total = row[0] ? (unsigned long long)strtoull(row[0], NULL, 10) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

void EventUsedFatigueAtMage_PrintInfo(const EventUsedFatigueAtMage* event) {
    if (!event) return;

    printf("=== Event Used Fatigue At Mage ===\n");
    printf("Character No: %u\n", event->charac_no);
    printf("Fatigue Quantity: %u\n", event->fatigue_quantity);
    printf("===================================\n");
}
