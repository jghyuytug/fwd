#include "pswd_qstion_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int PswdQstion_Add(DBConnectionManager* manager, const PswdQstion* question) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !question) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO pswd_qstion (q_no, q_text) VALUES (%u, '%s')",
        question->q_no, question->q_text);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int PswdQstion_Get(DBConnectionManager* manager, unsigned char q_no, PswdQstion* question) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !question) return -1;

    snprintf(query, sizeof(query),
        "SELECT q_no, q_text FROM pswd_qstion WHERE q_no = %u", q_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(question, 0, sizeof(PswdQstion));
    question->q_no = row[0] ? (unsigned char)atoi(row[0]) : 0;
    if (row[1]) {
        strncpy(question->q_text, row[1], sizeof(question->q_text) - 1);
        question->q_text[sizeof(question->q_text) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PswdQstion_Update(DBConnectionManager* manager, const PswdQstion* question) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !question) return -1;

    snprintf(query, sizeof(query),
        "UPDATE pswd_qstion SET q_text = '%s' WHERE q_no = %u",
        question->q_text, question->q_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int PswdQstion_Delete(DBConnectionManager* manager, unsigned char q_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM pswd_qstion WHERE q_no = %u", q_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int PswdQstion_Exists(DBConnectionManager* manager, unsigned char q_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM pswd_qstion WHERE q_no = %u", q_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return exists > 0;
}

int PswdQstion_GetAll(DBConnectionManager* manager, PswdQstion* questions,
                       int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !questions || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT q_no, q_text FROM pswd_qstion ORDER BY q_no");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&questions[count], 0, sizeof(PswdQstion));
        questions[count].q_no = row[0] ? (unsigned char)atoi(row[0]) : 0;
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

int PswdQstion_GetByTextPattern(DBConnectionManager* manager, const char* pattern,
                                 PswdQstion* questions, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !pattern || !questions || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT q_no, q_text FROM pswd_qstion WHERE q_text LIKE '%%%s%%' ORDER BY q_no",
        pattern);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&questions[count], 0, sizeof(PswdQstion));
        questions[count].q_no = row[0] ? (unsigned char)atoi(row[0]) : 0;
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

int PswdQstion_Count(DBConnectionManager* manager, int* total_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !total_count) return -1;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM pswd_qstion");

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

void PswdQstion_PrintInfo(const PswdQstion* question) {
    if (!question) {
        printf("NULL question\n");
        return;
    }

    printf("PswdQstion Info:\n");
    printf("  Question No: %u\n", question->q_no);
    printf("  Question Text: %s\n", question->q_text);
}
