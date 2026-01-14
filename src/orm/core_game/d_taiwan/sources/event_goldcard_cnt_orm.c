#include "event_goldcard_cnt_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FIELDS 3
#define MAX_QUERY_LEN 4096

/* CRUD operations */

int EventGoldcardCnt_Add(DBConnectionManager* manager, const EventGoldcardCnt* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "REPLACE INTO event_goldcard_cnt (item_no, occ_date, cnt) "
        "VALUES (%u, '%s', %d)",
        record->item_no, record->occ_date, record->cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardCnt_Get(DBConnectionManager* manager, unsigned int item_no, const char* occ_date, EventGoldcardCnt* record) {
    char query[MAX_QUERY_LEN];
    char* row[MAX_FIELDS];
    DBQueryResult result;

    if (!manager || !occ_date || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT item_no, occ_date, cnt FROM event_goldcard_cnt WHERE item_no = %u AND occ_date = '%s'",
        item_no, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(EventGoldcardCnt));
    record->item_no = row[0] ? (unsigned int)atoi(row[0]) : 0;
    if (row[1]) {
        strncpy(record->occ_date, row[1], sizeof(record->occ_date) - 1);
        record->occ_date[sizeof(record->occ_date) - 1] = '\0';
    }
    record->cnt = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardCnt_Update(DBConnectionManager* manager, const EventGoldcardCnt* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_goldcard_cnt SET cnt = %d WHERE item_no = %u AND occ_date = '%s'",
        record->cnt, record->item_no, record->occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardCnt_Delete(DBConnectionManager* manager, unsigned int item_no, const char* occ_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_date) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_goldcard_cnt WHERE item_no = %u AND occ_date = '%s'",
        item_no, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardCnt_Exists(DBConnectionManager* manager, unsigned int item_no, const char* occ_date) {
    char query[MAX_QUERY_LEN];
    char* row[1];
    DBQueryResult result;
    int exists = 0;

    if (!manager || !occ_date) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_goldcard_cnt WHERE item_no = %u AND occ_date = '%s'",
        item_no, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = row[0] ? atoi(row[0]) > 0 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Business operations */

int EventGoldcardCnt_GetByItemNo(DBConnectionManager* manager, unsigned int item_no,
                                  EventGoldcardCnt* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    char* row[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT item_no, occ_date, cnt FROM event_goldcard_cnt WHERE item_no = %u ORDER BY occ_date DESC LIMIT %d",
        item_no, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        *actual_count = 0;
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventGoldcardCnt));
        records[count].item_no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(records[count].occ_date, row[1], sizeof(records[count].occ_date) - 1);
            records[count].occ_date[sizeof(records[count].occ_date) - 1] = '\0';
        }
        records[count].cnt = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardCnt_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date,
                                     EventGoldcardCnt* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    char* row[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !start_date || !end_date || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT item_no, occ_date, cnt FROM event_goldcard_cnt "
        "WHERE occ_date >= '%s' AND occ_date <= '%s' ORDER BY occ_date DESC, item_no LIMIT %d",
        start_date, end_date, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        *actual_count = 0;
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(EventGoldcardCnt));
        records[count].item_no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(records[count].occ_date, row[1], sizeof(records[count].occ_date) - 1);
            records[count].occ_date[sizeof(records[count].occ_date) - 1] = '\0';
        }
        records[count].cnt = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardCnt_IncrementCount(DBConnectionManager* manager, unsigned int item_no, const char* occ_date, int increment) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_date) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_goldcard_cnt (item_no, occ_date, cnt) VALUES (%u, '%s', %d) "
        "ON DUPLICATE KEY UPDATE cnt = cnt + %d",
        item_no, occ_date, increment, increment);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardCnt_GetTotalByItem(DBConnectionManager* manager, unsigned int item_no) {
    char query[MAX_QUERY_LEN];
    char* row[1];
    DBQueryResult result;
    int total = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT SUM(cnt) FROM event_goldcard_cnt WHERE item_no = %u", item_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        total = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return total;
}

int EventGoldcardCnt_GetTotalByDate(DBConnectionManager* manager, const char* occ_date) {
    char query[MAX_QUERY_LEN];
    char* row[1];
    DBQueryResult result;
    int total = 0;

    if (!manager || !occ_date) return 0;

    snprintf(query, sizeof(query),
        "SELECT SUM(cnt) FROM event_goldcard_cnt WHERE occ_date = '%s'", occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        total = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return total;
}

void EventGoldcardCnt_PrintInfo(const EventGoldcardCnt* record) {
    if (!record) return;

    printf("EventGoldcardCnt:\n");
    printf("  Item No: %u\n", record->item_no);
    printf("  Occur Date: %s\n", record->occ_date);
    printf("  Count: %d\n", record->cnt);
}
