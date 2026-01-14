#include "gift_ticket_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GiftTicketEntry_Add(DBConnectionManager* manager, const GiftTicketEntry* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO gift_ticket_entry (gift_no, buyer_id, buyer_date, buyer_code, buyer_check, other_id, other_date, other_code, other_check, message) "
        "VALUES (%d, %d, %d, '%s', %d, %d, %d, '%s', %d, '%s')",
        record->gift_no, record->buyer_id, record->buyer_date, record->buyer_code, record->buyer_check, record->other_id, record->other_date, record->other_code, record->other_check, record->message);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GiftTicketEntry_Get(DBConnectionManager* manager, int id, GiftTicketEntry* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, gift_no, buyer_id, buyer_date, buyer_code, buyer_check, other_id, other_date, other_code, other_check, message FROM gift_ticket_entry WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GiftTicketEntry));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->gift_no = row[1] ? atoi(row[1]) : 0;
    record->buyer_id = row[2] ? atoi(row[2]) : 0;
    record->buyer_date = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->buyer_code, row[4], sizeof(record->buyer_code) - 1);
    record->buyer_check = row[5] ? atoi(row[5]) : 0;
    record->other_id = row[6] ? atoi(row[6]) : 0;
    record->other_date = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->other_code, row[8], sizeof(record->other_code) - 1);
    record->other_check = row[9] ? atoi(row[9]) : 0;
    if (row[10]) strncpy(record->message, row[10], sizeof(record->message) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GiftTicketEntry_GetAll(DBConnectionManager* manager, GiftTicketEntry* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, gift_no, buyer_id, buyer_date, buyer_code, buyer_check, other_id, other_date, other_code, other_check, message FROM gift_ticket_entry");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GiftTicketEntry));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].gift_no = row[1] ? atoi(row[1]) : 0;
        records[count].buyer_id = row[2] ? atoi(row[2]) : 0;
        records[count].buyer_date = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].buyer_code, row[4], sizeof(records[count].buyer_code) - 1);
        records[count].buyer_check = row[5] ? atoi(row[5]) : 0;
        records[count].other_id = row[6] ? atoi(row[6]) : 0;
        records[count].other_date = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].other_code, row[8], sizeof(records[count].other_code) - 1);
        records[count].other_check = row[9] ? atoi(row[9]) : 0;
        if (row[10]) strncpy(records[count].message, row[10], sizeof(records[count].message) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
