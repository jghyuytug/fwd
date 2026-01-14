#include "dnf_magazine_quiz_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfMagazineQuiz_Add(DBConnectionManager* manager, const DnfMagazineQuiz* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_magazine_quiz (magazine_id, m_id, occ_time) "
        "VALUES (%d, %d, '%s')",
        record->magazine_id, record->m_id, record->occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMagazineQuiz_Get(DBConnectionManager* manager, int magazine_id, int m_id, DnfMagazineQuiz* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT magazine_id, m_id, occ_time FROM dnf_magazine_quiz WHERE magazine_id = %d AND m_id = %d",
        magazine_id, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfMagazineQuiz));
    record->magazine_id = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->occ_time, row[2], sizeof(record->occ_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMagazineQuiz_GetAll(DBConnectionManager* manager, DnfMagazineQuiz* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT magazine_id, m_id, occ_time FROM dnf_magazine_quiz");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfMagazineQuiz));
        records[count].magazine_id = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].occ_time, row[2], sizeof(records[count].occ_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
