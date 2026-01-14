#include "gift_ticket_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

/* Add - id is auto_increment, not included in INSERT */
int GiftTicketEntry_Add(DBConnectionManager* manager, const GiftTicketEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO gift_ticket_entry (gift_no, buyer_id, buyer_date, buyer_code, "
        "buyer_check, other_id, other_date, other_code, other_check, message) "
        "VALUES (%u, %u, %u, '%s', %u, %u, %u, '%s', %u, '%s')",
        entry->gift_no, entry->buyer_id, entry->buyer_date, entry->buyer_code,
        entry->buyer_check, entry->other_id, entry->other_date, entry->other_code,
        entry->other_check, entry->message);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get by primary key (id) */
int GiftTicketEntry_Get(DBConnectionManager* manager, unsigned int id, GiftTicketEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, gift_no, buyer_id, buyer_date, buyer_code, buyer_check, "
        "other_id, other_date, other_code, other_check, message "
        "FROM gift_ticket_entry WHERE id = %u", id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(entry, 0, sizeof(GiftTicketEntry));
    entry->id = row[0] ? (unsigned int)atoi(row[0]) : 0;
    entry->gift_no = row[1] ? (unsigned short)atoi(row[1]) : 0;
    entry->buyer_id = row[2] ? (unsigned int)atoi(row[2]) : 0;
    entry->buyer_date = row[3] ? (unsigned int)atoi(row[3]) : 0;
    if (row[4]) {
        strncpy(entry->buyer_code, row[4], sizeof(entry->buyer_code) - 1);
        entry->buyer_code[sizeof(entry->buyer_code) - 1] = '\0';
    }
    entry->buyer_check = row[5] ? (unsigned int)atoi(row[5]) : 0;
    entry->other_id = row[6] ? (unsigned int)atoi(row[6]) : 0;
    entry->other_date = row[7] ? (unsigned int)atoi(row[7]) : 0;
    if (row[8]) {
        strncpy(entry->other_code, row[8], sizeof(entry->other_code) - 1);
        entry->other_code[sizeof(entry->other_code) - 1] = '\0';
    }
    entry->other_check = row[9] ? (unsigned int)atoi(row[9]) : 0;
    if (row[10]) {
        strncpy(entry->message, row[10], sizeof(entry->message) - 1);
        entry->message[sizeof(entry->message) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* Update by primary key (id) */
int GiftTicketEntry_Update(DBConnectionManager* manager, const GiftTicketEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "UPDATE gift_ticket_entry SET gift_no = %u, buyer_id = %u, buyer_date = %u, "
        "buyer_code = '%s', buyer_check = %u, other_id = %u, other_date = %u, "
        "other_code = '%s', other_check = %u, message = '%s' WHERE id = %u",
        entry->gift_no, entry->buyer_id, entry->buyer_date, entry->buyer_code,
        entry->buyer_check, entry->other_id, entry->other_date, entry->other_code,
        entry->other_check, entry->message, entry->id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete by primary key (id) */
int GiftTicketEntry_Delete(DBConnectionManager* manager, unsigned int id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM gift_ticket_entry WHERE id = %u", id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if record exists */
int GiftTicketEntry_Exists(DBConnectionManager* manager, unsigned int id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM gift_ticket_entry WHERE id = %u", id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return exists;
}

/* Get by buyer_id (indexed field) */
int GiftTicketEntry_GetByBuyerId(DBConnectionManager* manager, unsigned int buyer_id,
                                  GiftTicketEntry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, gift_no, buyer_id, buyer_date, buyer_code, buyer_check, "
        "other_id, other_date, other_code, other_check, message "
        "FROM gift_ticket_entry WHERE buyer_id = %u ORDER BY id", buyer_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(GiftTicketEntry));
        entries[count].id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        entries[count].gift_no = row[1] ? (unsigned short)atoi(row[1]) : 0;
        entries[count].buyer_id = row[2] ? (unsigned int)atoi(row[2]) : 0;
        entries[count].buyer_date = row[3] ? (unsigned int)atoi(row[3]) : 0;
        if (row[4]) {
            strncpy(entries[count].buyer_code, row[4], sizeof(entries[count].buyer_code) - 1);
            entries[count].buyer_code[sizeof(entries[count].buyer_code) - 1] = '\0';
        }
        entries[count].buyer_check = row[5] ? (unsigned int)atoi(row[5]) : 0;
        entries[count].other_id = row[6] ? (unsigned int)atoi(row[6]) : 0;
        entries[count].other_date = row[7] ? (unsigned int)atoi(row[7]) : 0;
        if (row[8]) {
            strncpy(entries[count].other_code, row[8], sizeof(entries[count].other_code) - 1);
            entries[count].other_code[sizeof(entries[count].other_code) - 1] = '\0';
        }
        entries[count].other_check = row[9] ? (unsigned int)atoi(row[9]) : 0;
        if (row[10]) {
            strncpy(entries[count].message, row[10], sizeof(entries[count].message) - 1);
            entries[count].message[sizeof(entries[count].message) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get by other_id (indexed field) */
int GiftTicketEntry_GetByOtherId(DBConnectionManager* manager, unsigned int other_id,
                                  GiftTicketEntry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, gift_no, buyer_id, buyer_date, buyer_code, buyer_check, "
        "other_id, other_date, other_code, other_check, message "
        "FROM gift_ticket_entry WHERE other_id = %u ORDER BY id", other_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(GiftTicketEntry));
        entries[count].id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        entries[count].gift_no = row[1] ? (unsigned short)atoi(row[1]) : 0;
        entries[count].buyer_id = row[2] ? (unsigned int)atoi(row[2]) : 0;
        entries[count].buyer_date = row[3] ? (unsigned int)atoi(row[3]) : 0;
        if (row[4]) {
            strncpy(entries[count].buyer_code, row[4], sizeof(entries[count].buyer_code) - 1);
            entries[count].buyer_code[sizeof(entries[count].buyer_code) - 1] = '\0';
        }
        entries[count].buyer_check = row[5] ? (unsigned int)atoi(row[5]) : 0;
        entries[count].other_id = row[6] ? (unsigned int)atoi(row[6]) : 0;
        entries[count].other_date = row[7] ? (unsigned int)atoi(row[7]) : 0;
        if (row[8]) {
            strncpy(entries[count].other_code, row[8], sizeof(entries[count].other_code) - 1);
            entries[count].other_code[sizeof(entries[count].other_code) - 1] = '\0';
        }
        entries[count].other_check = row[9] ? (unsigned int)atoi(row[9]) : 0;
        if (row[10]) {
            strncpy(entries[count].message, row[10], sizeof(entries[count].message) - 1);
            entries[count].message[sizeof(entries[count].message) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get all records */
int GiftTicketEntry_GetAll(DBConnectionManager* manager, GiftTicketEntry* entries,
                            int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, gift_no, buyer_id, buyer_date, buyer_code, buyer_check, "
        "other_id, other_date, other_code, other_check, message "
        "FROM gift_ticket_entry ORDER BY id LIMIT %d", max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(GiftTicketEntry));
        entries[count].id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        entries[count].gift_no = row[1] ? (unsigned short)atoi(row[1]) : 0;
        entries[count].buyer_id = row[2] ? (unsigned int)atoi(row[2]) : 0;
        entries[count].buyer_date = row[3] ? (unsigned int)atoi(row[3]) : 0;
        if (row[4]) {
            strncpy(entries[count].buyer_code, row[4], sizeof(entries[count].buyer_code) - 1);
            entries[count].buyer_code[sizeof(entries[count].buyer_code) - 1] = '\0';
        }
        entries[count].buyer_check = row[5] ? (unsigned int)atoi(row[5]) : 0;
        entries[count].other_id = row[6] ? (unsigned int)atoi(row[6]) : 0;
        entries[count].other_date = row[7] ? (unsigned int)atoi(row[7]) : 0;
        if (row[8]) {
            strncpy(entries[count].other_code, row[8], sizeof(entries[count].other_code) - 1);
            entries[count].other_code[sizeof(entries[count].other_code) - 1] = '\0';
        }
        entries[count].other_check = row[9] ? (unsigned int)atoi(row[9]) : 0;
        if (row[10]) {
            strncpy(entries[count].message, row[10], sizeof(entries[count].message) - 1);
            entries[count].message[sizeof(entries[count].message) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Print entry information */
void GiftTicketEntry_PrintInfo(const GiftTicketEntry* entry) {
    if (!entry) {
        printf("GiftTicketEntry: NULL\n");
        return;
    }

    printf("GiftTicketEntry:\n");
    printf("  id: %u\n", entry->id);
    printf("  gift_no: %u\n", entry->gift_no);
    printf("  buyer_id: %u\n", entry->buyer_id);
    printf("  buyer_date: %u\n", entry->buyer_date);
    printf("  buyer_code: %s\n", entry->buyer_code);
    printf("  buyer_check: %u\n", entry->buyer_check);
    printf("  other_id: %u\n", entry->other_id);
    printf("  other_date: %u\n", entry->other_date);
    printf("  other_code: %s\n", entry->other_code);
    printf("  other_check: %u\n", entry->other_check);
    printf("  message: %s\n", entry->message);
}
