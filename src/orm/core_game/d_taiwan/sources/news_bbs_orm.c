#include "news_bbs_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

/* Helper function to escape special characters in text */
static void escape_string(char* dest, const char* src, int max_len) {
    int i, j = 0;
    for (i = 0; src[i] && j < max_len - 2; i++) {
        if (src[i] == '\'' || src[i] == '\"' || src[i] == '\\') {
            dest[j++] = '\\';
        }
        dest[j++] = src[i];
    }
    dest[j] = '\0';
}

int NewsBbs_Add(DBConnectionManager* manager, const NewsBbs* bbs) {
    char query[MAX_QUERY_LEN];
    char escaped_subject[102];
    char escaped_body[8192];
    char escaped_file_name[102];
    DBQueryResult result;

    if (!manager || !bbs) return -1;

    escape_string(escaped_subject, bbs->subject, sizeof(escaped_subject));
    escape_string(escaped_body, bbs->body, sizeof(escaped_body));

    if (bbs->file_name[0]) {
        escape_string(escaped_file_name, bbs->file_name, sizeof(escaped_file_name));
        snprintf(query, sizeof(query),
            "INSERT INTO news_bbs (bbs_code, emph_yn, user_id, reg_date, html_yn, "
            "subject, body, hits, prev_no, next_no, updt_date, use_yn, file_name) "
            "VALUES (%u, %u, '%s', %d, %u, '%s', '%s', %u, %d, %d, %d, %u, '%s')",
            bbs->bbs_code, bbs->emph_yn, bbs->user_id, bbs->reg_date, bbs->html_yn,
            escaped_subject, escaped_body, bbs->hits, bbs->prev_no, bbs->next_no,
            bbs->updt_date, bbs->use_yn, escaped_file_name);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO news_bbs (bbs_code, emph_yn, user_id, reg_date, html_yn, "
            "subject, body, hits, prev_no, next_no, updt_date, use_yn) "
            "VALUES (%u, %u, '%s', %d, %u, '%s', '%s', %u, %d, %d, %d, %u)",
            bbs->bbs_code, bbs->emph_yn, bbs->user_id, bbs->reg_date, bbs->html_yn,
            escaped_subject, escaped_body, bbs->hits, bbs->prev_no, bbs->next_no,
            bbs->updt_date, bbs->use_yn);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int NewsBbs_GetByNo(DBConnectionManager* manager, unsigned int no, NewsBbs* bbs) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];

    if (!manager || !bbs) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_code, emph_yn, no, user_id, reg_date, html_yn, subject, body, "
        "hits, prev_no, next_no, updt_date, use_yn, file_name "
        "FROM news_bbs WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(bbs, 0, sizeof(NewsBbs));
    bbs->bbs_code = row[0] ? (unsigned char)atoi(row[0]) : 0;
    bbs->emph_yn = row[1] ? (unsigned char)atoi(row[1]) : 0;
    bbs->no = row[2] ? (unsigned int)atoi(row[2]) : 0;
    if (row[3]) strncpy(bbs->user_id, row[3], sizeof(bbs->user_id) - 1);
    bbs->reg_date = row[4] ? atoi(row[4]) : 0;
    bbs->html_yn = row[5] ? (unsigned char)atoi(row[5]) : 0;
    if (row[6]) strncpy(bbs->subject, row[6], sizeof(bbs->subject) - 1);
    if (row[7]) strncpy(bbs->body, row[7], sizeof(bbs->body) - 1);
    bbs->hits = row[8] ? (unsigned short)atoi(row[8]) : 0;
    bbs->prev_no = row[9] ? atoi(row[9]) : 0;
    bbs->next_no = row[10] ? atoi(row[10]) : 0;
    bbs->updt_date = row[11] ? atoi(row[11]) : 0;
    bbs->use_yn = row[12] ? (unsigned char)atoi(row[12]) : 1;
    if (row[13]) strncpy(bbs->file_name, row[13], sizeof(bbs->file_name) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int NewsBbs_Update(DBConnectionManager* manager, const NewsBbs* bbs) {
    char query[MAX_QUERY_LEN];
    char escaped_subject[102];
    char escaped_body[8192];
    char escaped_file_name[102];
    DBQueryResult result;

    if (!manager || !bbs) return -1;

    escape_string(escaped_subject, bbs->subject, sizeof(escaped_subject));
    escape_string(escaped_body, bbs->body, sizeof(escaped_body));

    if (bbs->file_name[0]) {
        escape_string(escaped_file_name, bbs->file_name, sizeof(escaped_file_name));
        snprintf(query, sizeof(query),
            "UPDATE news_bbs SET bbs_code = %u, emph_yn = %u, user_id = '%s', "
            "reg_date = %d, html_yn = %u, subject = '%s', body = '%s', hits = %u, "
            "prev_no = %d, next_no = %d, updt_date = %d, use_yn = %u, file_name = '%s' "
            "WHERE no = %u",
            bbs->bbs_code, bbs->emph_yn, bbs->user_id, bbs->reg_date, bbs->html_yn,
            escaped_subject, escaped_body, bbs->hits, bbs->prev_no, bbs->next_no,
            bbs->updt_date, bbs->use_yn, escaped_file_name, bbs->no);
    } else {
        snprintf(query, sizeof(query),
            "UPDATE news_bbs SET bbs_code = %u, emph_yn = %u, user_id = '%s', "
            "reg_date = %d, html_yn = %u, subject = '%s', body = '%s', hits = %u, "
            "prev_no = %d, next_no = %d, updt_date = %d, use_yn = %u, file_name = NULL "
            "WHERE no = %u",
            bbs->bbs_code, bbs->emph_yn, bbs->user_id, bbs->reg_date, bbs->html_yn,
            escaped_subject, escaped_body, bbs->hits, bbs->prev_no, bbs->next_no,
            bbs->updt_date, bbs->use_yn, bbs->no);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int NewsBbs_Delete(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM news_bbs WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int NewsBbs_Exists(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM news_bbs WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count > 0;
}

int NewsBbs_GetByBbsCode(DBConnectionManager* manager, unsigned char bbs_code,
                         NewsBbs* bbs_list, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];
    int count = 0;

    if (!manager || !bbs_list || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_code, emph_yn, no, user_id, reg_date, html_yn, subject, body, "
        "hits, prev_no, next_no, updt_date, use_yn, file_name "
        "FROM news_bbs WHERE bbs_code = %u AND use_yn = 1 ORDER BY no DESC", bbs_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&bbs_list[count], 0, sizeof(NewsBbs));
        bbs_list[count].bbs_code = row[0] ? (unsigned char)atoi(row[0]) : 0;
        bbs_list[count].emph_yn = row[1] ? (unsigned char)atoi(row[1]) : 0;
        bbs_list[count].no = row[2] ? (unsigned int)atoi(row[2]) : 0;
        if (row[3]) strncpy(bbs_list[count].user_id, row[3], sizeof(bbs_list[count].user_id) - 1);
        bbs_list[count].reg_date = row[4] ? atoi(row[4]) : 0;
        bbs_list[count].html_yn = row[5] ? (unsigned char)atoi(row[5]) : 0;
        if (row[6]) strncpy(bbs_list[count].subject, row[6], sizeof(bbs_list[count].subject) - 1);
        if (row[7]) strncpy(bbs_list[count].body, row[7], sizeof(bbs_list[count].body) - 1);
        bbs_list[count].hits = row[8] ? (unsigned short)atoi(row[8]) : 0;
        bbs_list[count].prev_no = row[9] ? atoi(row[9]) : 0;
        bbs_list[count].next_no = row[10] ? atoi(row[10]) : 0;
        bbs_list[count].updt_date = row[11] ? atoi(row[11]) : 0;
        bbs_list[count].use_yn = row[12] ? (unsigned char)atoi(row[12]) : 1;
        if (row[13]) strncpy(bbs_list[count].file_name, row[13], sizeof(bbs_list[count].file_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int NewsBbs_GetAll(DBConnectionManager* manager, NewsBbs* bbs_list,
                   int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];
    int count = 0;

    if (!manager || !bbs_list || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_code, emph_yn, no, user_id, reg_date, html_yn, subject, body, "
        "hits, prev_no, next_no, updt_date, use_yn, file_name "
        "FROM news_bbs WHERE use_yn = 1 ORDER BY no DESC LIMIT %d", max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&bbs_list[count], 0, sizeof(NewsBbs));
        bbs_list[count].bbs_code = row[0] ? (unsigned char)atoi(row[0]) : 0;
        bbs_list[count].emph_yn = row[1] ? (unsigned char)atoi(row[1]) : 0;
        bbs_list[count].no = row[2] ? (unsigned int)atoi(row[2]) : 0;
        if (row[3]) strncpy(bbs_list[count].user_id, row[3], sizeof(bbs_list[count].user_id) - 1);
        bbs_list[count].reg_date = row[4] ? atoi(row[4]) : 0;
        bbs_list[count].html_yn = row[5] ? (unsigned char)atoi(row[5]) : 0;
        if (row[6]) strncpy(bbs_list[count].subject, row[6], sizeof(bbs_list[count].subject) - 1);
        if (row[7]) strncpy(bbs_list[count].body, row[7], sizeof(bbs_list[count].body) - 1);
        bbs_list[count].hits = row[8] ? (unsigned short)atoi(row[8]) : 0;
        bbs_list[count].prev_no = row[9] ? atoi(row[9]) : 0;
        bbs_list[count].next_no = row[10] ? atoi(row[10]) : 0;
        bbs_list[count].updt_date = row[11] ? atoi(row[11]) : 0;
        bbs_list[count].use_yn = row[12] ? (unsigned char)atoi(row[12]) : 1;
        if (row[13]) strncpy(bbs_list[count].file_name, row[13], sizeof(bbs_list[count].file_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int NewsBbs_IncrementHits(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE news_bbs SET hits = hits + 1 WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int NewsBbs_GetByUserId(DBConnectionManager* manager, const char* user_id,
                        NewsBbs* bbs_list, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];
    int count = 0;

    if (!manager || !user_id || !bbs_list || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_code, emph_yn, no, user_id, reg_date, html_yn, subject, body, "
        "hits, prev_no, next_no, updt_date, use_yn, file_name "
        "FROM news_bbs WHERE user_id = '%s' AND use_yn = 1 ORDER BY no DESC", user_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&bbs_list[count], 0, sizeof(NewsBbs));
        bbs_list[count].bbs_code = row[0] ? (unsigned char)atoi(row[0]) : 0;
        bbs_list[count].emph_yn = row[1] ? (unsigned char)atoi(row[1]) : 0;
        bbs_list[count].no = row[2] ? (unsigned int)atoi(row[2]) : 0;
        if (row[3]) strncpy(bbs_list[count].user_id, row[3], sizeof(bbs_list[count].user_id) - 1);
        bbs_list[count].reg_date = row[4] ? atoi(row[4]) : 0;
        bbs_list[count].html_yn = row[5] ? (unsigned char)atoi(row[5]) : 0;
        if (row[6]) strncpy(bbs_list[count].subject, row[6], sizeof(bbs_list[count].subject) - 1);
        if (row[7]) strncpy(bbs_list[count].body, row[7], sizeof(bbs_list[count].body) - 1);
        bbs_list[count].hits = row[8] ? (unsigned short)atoi(row[8]) : 0;
        bbs_list[count].prev_no = row[9] ? atoi(row[9]) : 0;
        bbs_list[count].next_no = row[10] ? atoi(row[10]) : 0;
        bbs_list[count].updt_date = row[11] ? atoi(row[11]) : 0;
        bbs_list[count].use_yn = row[12] ? (unsigned char)atoi(row[12]) : 1;
        if (row[13]) strncpy(bbs_list[count].file_name, row[13], sizeof(bbs_list[count].file_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int NewsBbs_SetPrevNext(DBConnectionManager* manager, unsigned int no,
                        int prev_no, int next_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE news_bbs SET prev_no = %d, next_no = %d WHERE no = %u",
        prev_no, next_no, no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

void NewsBbs_PrintInfo(const NewsBbs* bbs) {
    if (!bbs) return;

    printf("=== News BBS Info ===\n");
    printf("No: %u\n", bbs->no);
    printf("BBS Code: %u\n", bbs->bbs_code);
    printf("Emphasis: %u\n", bbs->emph_yn);
    printf("User ID: %s\n", bbs->user_id);
    printf("Subject: %s\n", bbs->subject);
    printf("Body: %s\n", bbs->body);
    printf("Hits: %u\n", bbs->hits);
    printf("Reg Date: %d\n", bbs->reg_date);
    printf("HTML: %u\n", bbs->html_yn);
    printf("Prev No: %d\n", bbs->prev_no);
    printf("Next No: %d\n", bbs->next_no);
    printf("Update Date: %d\n", bbs->updt_date);
    printf("Use: %u\n", bbs->use_yn);
    printf("File: %s\n", bbs->file_name[0] ? bbs->file_name : "(none)");
    printf("====================\n");
}
