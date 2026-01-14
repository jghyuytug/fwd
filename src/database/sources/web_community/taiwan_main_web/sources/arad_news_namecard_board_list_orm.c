#include "arad_news_namecard_board_list_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AradNewsNamecardBoardList_Add(DBConnectionManager* manager, const AradNewsNamecardBoardList* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO arad_news_namecard_board_list (m_id, board_type, board_no, create_time) "
        "VALUES (%d, %d, %d, %d)",
        record->m_id, record->board_type, record->board_no, record->create_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AradNewsNamecardBoardList_Get(DBConnectionManager* manager, int m_id, signed char board_type, int board_no, AradNewsNamecardBoardList* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, board_type, board_no, create_time FROM arad_news_namecard_board_list WHERE m_id = %d AND board_type = %d AND board_no = %d",
        m_id, board_type, board_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AradNewsNamecardBoardList));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->board_type = row[1] ? atoi(row[1]) : 0;
    record->board_no = row[2] ? atoi(row[2]) : 0;
    record->create_time = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AradNewsNamecardBoardList_GetAll(DBConnectionManager* manager, AradNewsNamecardBoardList* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, board_type, board_no, create_time FROM arad_news_namecard_board_list");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AradNewsNamecardBoardList));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].board_type = row[1] ? atoi(row[1]) : 0;
        records[count].board_no = row[2] ? atoi(row[2]) : 0;
        records[count].create_time = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
