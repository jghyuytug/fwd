#include "event_tower_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int EventTowerEntry_Add(DBConnectionManager* manager, const EventTowerEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_tower_entry (m_id, occ_date, occ_check, server_id, charac_no, "
        "item1_no, item1_check, item2_no, item2_check, item3_no, item3_check) "
        "VALUES (%d, %u, %u, %d, %d, %u, %u, %u, %u, %u, %u)",
        entry->m_id, entry->occ_date, entry->occ_check, entry->server_id, entry->charac_no,
        entry->item1_no, entry->item1_check, entry->item2_no, entry->item2_check,
        entry->item3_no, entry->item3_check);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventTowerEntry_Get(DBConnectionManager* manager, int m_id, EventTowerEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, occ_check, server_id, charac_no, "
        "item1_no, item1_check, item2_no, item2_check, item3_no, item3_check "
        "FROM event_tower_entry WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(entry, 0, sizeof(EventTowerEntry));
    entry->m_id = row[0] ? atoi(row[0]) : 0;
    entry->occ_date = row[1] ? (unsigned int)strtoul(row[1], NULL, 10) : 0;
    entry->occ_check = row[2] ? (unsigned int)strtoul(row[2], NULL, 10) : 0;
    entry->server_id = row[3] ? (char)atoi(row[3]) : 0;
    entry->charac_no = row[4] ? atoi(row[4]) : 0;
    entry->item1_no = row[5] ? (unsigned int)strtoul(row[5], NULL, 10) : 0;
    entry->item1_check = row[6] ? (unsigned int)strtoul(row[6], NULL, 10) : 0;
    entry->item2_no = row[7] ? (unsigned int)strtoul(row[7], NULL, 10) : 0;
    entry->item2_check = row[8] ? (unsigned int)strtoul(row[8], NULL, 10) : 0;
    entry->item3_no = row[9] ? (unsigned int)strtoul(row[9], NULL, 10) : 0;
    entry->item3_check = row[10] ? (unsigned int)strtoul(row[10], NULL, 10) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventTowerEntry_Update(DBConnectionManager* manager, const EventTowerEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_tower_entry SET occ_date = %u, occ_check = %u, server_id = %d, "
        "charac_no = %d, item1_no = %u, item1_check = %u, item2_no = %u, item2_check = %u, "
        "item3_no = %u, item3_check = %u WHERE m_id = %d",
        entry->occ_date, entry->occ_check, entry->server_id, entry->charac_no,
        entry->item1_no, entry->item1_check, entry->item2_no, entry->item2_check,
        entry->item3_no, entry->item3_check, entry->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventTowerEntry_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_tower_entry WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventTowerEntry_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT 1 FROM event_tower_entry WHERE m_id = %d LIMIT 1", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = 1;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int EventTowerEntry_GetByDateRange(DBConnectionManager* manager, unsigned int start_date,
                                    unsigned int end_date, EventTowerEntry* entries,
                                    int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, occ_check, server_id, charac_no, "
        "item1_no, item1_check, item2_no, item2_check, item3_no, item3_check "
        "FROM event_tower_entry WHERE occ_date >= %u AND occ_date <= %u ORDER BY occ_date",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventTowerEntry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        entries[count].occ_date = row[1] ? (unsigned int)strtoul(row[1], NULL, 10) : 0;
        entries[count].occ_check = row[2] ? (unsigned int)strtoul(row[2], NULL, 10) : 0;
        entries[count].server_id = row[3] ? (char)atoi(row[3]) : 0;
        entries[count].charac_no = row[4] ? atoi(row[4]) : 0;
        entries[count].item1_no = row[5] ? (unsigned int)strtoul(row[5], NULL, 10) : 0;
        entries[count].item1_check = row[6] ? (unsigned int)strtoul(row[6], NULL, 10) : 0;
        entries[count].item2_no = row[7] ? (unsigned int)strtoul(row[7], NULL, 10) : 0;
        entries[count].item2_check = row[8] ? (unsigned int)strtoul(row[8], NULL, 10) : 0;
        entries[count].item3_no = row[9] ? (unsigned int)strtoul(row[9], NULL, 10) : 0;
        entries[count].item3_check = row[10] ? (unsigned int)strtoul(row[10], NULL, 10) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventTowerEntry_GetByServerId(DBConnectionManager* manager, char server_id,
                                   EventTowerEntry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, occ_check, server_id, charac_no, "
        "item1_no, item1_check, item2_no, item2_check, item3_no, item3_check "
        "FROM event_tower_entry WHERE server_id = %d ORDER BY m_id",
        server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventTowerEntry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        entries[count].occ_date = row[1] ? (unsigned int)strtoul(row[1], NULL, 10) : 0;
        entries[count].occ_check = row[2] ? (unsigned int)strtoul(row[2], NULL, 10) : 0;
        entries[count].server_id = row[3] ? (char)atoi(row[3]) : 0;
        entries[count].charac_no = row[4] ? atoi(row[4]) : 0;
        entries[count].item1_no = row[5] ? (unsigned int)strtoul(row[5], NULL, 10) : 0;
        entries[count].item1_check = row[6] ? (unsigned int)strtoul(row[6], NULL, 10) : 0;
        entries[count].item2_no = row[7] ? (unsigned int)strtoul(row[7], NULL, 10) : 0;
        entries[count].item2_check = row[8] ? (unsigned int)strtoul(row[8], NULL, 10) : 0;
        entries[count].item3_no = row[9] ? (unsigned int)strtoul(row[9], NULL, 10) : 0;
        entries[count].item3_check = row[10] ? (unsigned int)strtoul(row[10], NULL, 10) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventTowerEntry_GetAll(DBConnectionManager* manager, EventTowerEntry* entries,
                            int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, occ_check, server_id, charac_no, "
        "item1_no, item1_check, item2_no, item2_check, item3_no, item3_check "
        "FROM event_tower_entry ORDER BY m_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventTowerEntry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        entries[count].occ_date = row[1] ? (unsigned int)strtoul(row[1], NULL, 10) : 0;
        entries[count].occ_check = row[2] ? (unsigned int)strtoul(row[2], NULL, 10) : 0;
        entries[count].server_id = row[3] ? (char)atoi(row[3]) : 0;
        entries[count].charac_no = row[4] ? atoi(row[4]) : 0;
        entries[count].item1_no = row[5] ? (unsigned int)strtoul(row[5], NULL, 10) : 0;
        entries[count].item1_check = row[6] ? (unsigned int)strtoul(row[6], NULL, 10) : 0;
        entries[count].item2_no = row[7] ? (unsigned int)strtoul(row[7], NULL, 10) : 0;
        entries[count].item2_check = row[8] ? (unsigned int)strtoul(row[8], NULL, 10) : 0;
        entries[count].item3_no = row[9] ? (unsigned int)strtoul(row[9], NULL, 10) : 0;
        entries[count].item3_check = row[10] ? (unsigned int)strtoul(row[10], NULL, 10) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventTowerEntry_CheckItem(DBConnectionManager* manager, int m_id, int item_index,
                               unsigned int* item_no, unsigned int* item_check) {
    EventTowerEntry entry;

    if (!manager || !item_no || !item_check) return -1;
    if (item_index < 1 || item_index > 3) return -1;

    if (EventTowerEntry_Get(manager, m_id, &entry) < 0)
        return -1;

    switch (item_index) {
        case 1:
            *item_no = entry.item1_no;
            *item_check = entry.item1_check;
            break;
        case 2:
            *item_no = entry.item2_no;
            *item_check = entry.item2_check;
            break;
        case 3:
            *item_no = entry.item3_no;
            *item_check = entry.item3_check;
            break;
        default:
            return -1;
    }

    return 0;
}

void EventTowerEntry_PrintInfo(const EventTowerEntry* entry) {
    if (!entry) return;

    printf("EventTowerEntry:\n");
    printf("  m_id: %d\n", entry->m_id);
    printf("  occ_date: %u\n", entry->occ_date);
    printf("  occ_check: %u\n", entry->occ_check);
    printf("  server_id: %d\n", entry->server_id);
    printf("  charac_no: %d\n", entry->charac_no);
    printf("  item1: no=%u, check=%u\n", entry->item1_no, entry->item1_check);
    printf("  item2: no=%u, check=%u\n", entry->item2_no, entry->item2_check);
    printf("  item3: no=%u, check=%u\n", entry->item3_no, entry->item3_check);
}
