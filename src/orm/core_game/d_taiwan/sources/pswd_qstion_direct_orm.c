#include "pswd_qstion_direct_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int PswdQstionDirect_Add(DBConnectionManager* manager, const PswdQstionDirect* question) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !question) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO pswd_qstion_direct (m_id, q_text) VALUES (%d, '%s')",
        question->m_id, question->q_text);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int PswdQstionDirect_Get(DBConnectionManager* manager, int m_id, PswdQstionDirect* question) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !question) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, q_text FROM pswd_qstion_direct WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(question, 0, sizeof(PswdQstionDirect));
    question->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) {
        strncpy(question->q_text, row[1], sizeof(question->q_text) - 1);
        question->q_text[sizeof(question->q_text) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PswdQstionDirect_Update(DBConnectionManager* manager, const PswdQstionDirect* question) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !question) return -1;

    snprintf(query, sizeof(query),
        "UPDATE pswd_qstion_direct SET q_text = '%s' WHERE m_id = %d",
        question->q_text, question->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int PswdQstionDirect_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM pswd_qstion_direct WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int PswdQstionDirect_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM pswd_qstion_direct WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return exists > 0;
}

int PswdQstionDirect_GetByTextPattern(DBConnectionManager* manager, const char* pattern,
                                       PswdQstionDirect* questions, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !pattern || !questions || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, q_text FROM pswd_qstion_direct WHERE q_text LIKE '%%%s%%' ORDER BY m_id",
        pattern);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&questions[count], 0, sizeof(PswdQstionDirect));
        questions[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(questions[count].q_text, row[1], sizeof(questions[count].q_text) - 1);
            questions[count].q_text[sizeof(questions[count].q_text) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int PswdQstionDirect_GetAll(DBConnectionManager* manager, PswdQstionDirect* questions,
                              int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !questions || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, q_text FROM pswd_qstion_direct ORDER BY m_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&questions[count], 0, sizeof(PswdQstionDirect));
        questions[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(questions[count].q_text, row[1], sizeof(questions[count].q_text) - 1);
            questions[count].q_text[sizeof(questions[count].q_text) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int PswdQstionDirect_Count(DBConnectionManager* manager, int* total_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !total_count) return -1;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM pswd_qstion_direct");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        *total_count = row[0] ? atoi(row[0]) : 0;
    } else {
        *total_count = 0;
    }

    DBQueryResult_Free(&result);
    return 0;
}

void PswdQstionDirect_PrintInfo(const PswdQstionDirect* question) {
    if (!question) {
        printf("NULL question\n");
        return;
    }

    printf("PswdQstionDirect Info:\n");
    printf("  Member ID: %d\n", question->m_id);
    printf("  Question Text: %s\n", question->q_text);
}
