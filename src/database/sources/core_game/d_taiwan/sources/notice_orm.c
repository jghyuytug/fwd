#include "notice_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 16384

static void escape_string(char* dest, const char* src, int max_len) {
    int i, j = 0;
    for (i = 0; src[i] && j < max_len - 1; i++) {
        if (src[i] == '\'' || src[i] == '\\' || src[i] == '"') {
            if (j < max_len - 2) dest[j++] = '\\';
        }
        dest[j++] = src[i];
    }
    dest[j] = '\0';
}

int Notice_Add(DBConnectionManager* manager, const Notice* notice) {
    char query[MAX_QUERY_LEN];
    char escaped_content[MAX_QUERY_LEN / 2];
    DBQueryResult result;

    if (!manager || !notice) return -1;

    if (notice->content) {
        escape_string(escaped_content, notice->content, sizeof(escaped_content));
    } else {
        escaped_content[0] = '\0';
    }

    snprintf(query, sizeof(query),
        "INSERT INTO notice (bbs_name, category, m_nickname, m_id, m_user_id, m_sex, "
        "title, create_day, comment, view, recom, adorn, adorn_color1, adorn_color2, "
        "depth, sequence, content, content_type, ip, ring, sms) "
        "VALUES ('%s', %u, '%s', %u, '%s', '%c', '%s', %u, %u, %u, %u, %u, %u, %u, %u, %f, "
        "'%s', '%s', '%s', %u, '%c')",
        notice->bbs_name, notice->category, notice->m_nickname, notice->m_id,
        notice->m_user_id, notice->m_sex, notice->title, notice->create_day,
        notice->comment, notice->view, notice->recom, notice->adorn,
        notice->adorn_color1, notice->adorn_color2, notice->depth, notice->sequence,
        escaped_content, notice->content_type, notice->ip, notice->ring, notice->sms);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int Notice_Get(DBConnectionManager* manager, unsigned int no, Notice* notice) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[22];

    if (!manager || !notice) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_name, no, category, m_nickname, m_id, m_user_id, m_sex, title, "
        "create_day, comment, view, recom, adorn, adorn_color1, adorn_color2, depth, "
        "sequence, content, content_type, ip, ring, sms FROM notice WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(notice, 0, sizeof(Notice));

    if (row[0]) {
        strncpy(notice->bbs_name, row[0], sizeof(notice->bbs_name) - 1);
        notice->bbs_name[sizeof(notice->bbs_name) - 1] = '\0';
    }
    notice->no = row[1] ? (unsigned int)atoi(row[1]) : 0;
    notice->category = row[2] ? (unsigned char)atoi(row[2]) : 0;
    if (row[3]) {
        strncpy(notice->m_nickname, row[3], sizeof(notice->m_nickname) - 1);
        notice->m_nickname[sizeof(notice->m_nickname) - 1] = '\0';
    }
    notice->m_id = row[4] ? (unsigned int)atoi(row[4]) : 0;
    if (row[5]) {
        strncpy(notice->m_user_id, row[5], sizeof(notice->m_user_id) - 1);
        notice->m_user_id[sizeof(notice->m_user_id) - 1] = '\0';
    }
    notice->m_sex = row[6] ? row[6][0] : 'm';
    if (row[7]) {
        strncpy(notice->title, row[7], sizeof(notice->title) - 1);
        notice->title[sizeof(notice->title) - 1] = '\0';
    }
    notice->create_day = row[8] ? (unsigned int)atoi(row[8]) : 0;
    notice->comment = row[9] ? (unsigned short)atoi(row[9]) : 0;
    notice->view = row[10] ? (unsigned int)atoi(row[10]) : 0;
    notice->recom = row[11] ? (unsigned int)atoi(row[11]) : 0;
    notice->adorn = row[12] ? (unsigned char)atoi(row[12]) : 0;
    notice->adorn_color1 = row[13] ? (unsigned char)atoi(row[13]) : 0;
    notice->adorn_color2 = row[14] ? (unsigned char)atoi(row[14]) : 0;
    notice->depth = row[15] ? (unsigned char)atoi(row[15]) : 0;
    notice->sequence = row[16] ? atof(row[16]) : 0.0;

    if (row[17]) {
        notice->content = (char*)malloc(strlen(row[17]) + 1);
        if (notice->content) {
            strcpy(notice->content, row[17]);
        }
    } else {
        notice->content = NULL;
    }

    if (row[18]) {
        strncpy(notice->content_type, row[18], sizeof(notice->content_type) - 1);
        notice->content_type[sizeof(notice->content_type) - 1] = '\0';
    }
    if (row[19]) {
        strncpy(notice->ip, row[19], sizeof(notice->ip) - 1);
        notice->ip[sizeof(notice->ip) - 1] = '\0';
    }
    notice->ring = row[20] ? (unsigned short)atoi(row[20]) : 0;
    notice->sms = row[21] ? row[21][0] : 'n';

    DBQueryResult_Free(&result);
    return 0;
}

int Notice_Update(DBConnectionManager* manager, const Notice* notice) {
    char query[MAX_QUERY_LEN];
    char escaped_content[MAX_QUERY_LEN / 2];
    DBQueryResult result;

    if (!manager || !notice) return -1;

    if (notice->content) {
        escape_string(escaped_content, notice->content, sizeof(escaped_content));
    } else {
        escaped_content[0] = '\0';
    }

    snprintf(query, sizeof(query),
        "UPDATE notice SET bbs_name = '%s', category = %u, m_nickname = '%s', "
        "m_id = %u, m_user_id = '%s', m_sex = '%c', title = '%s', create_day = %u, "
        "comment = %u, view = %u, recom = %u, adorn = %u, adorn_color1 = %u, "
        "adorn_color2 = %u, depth = %u, sequence = %f, content = '%s', "
        "content_type = '%s', ip = '%s', ring = %u, sms = '%c' WHERE no = %u",
        notice->bbs_name, notice->category, notice->m_nickname, notice->m_id,
        notice->m_user_id, notice->m_sex, notice->title, notice->create_day,
        notice->comment, notice->view, notice->recom, notice->adorn,
        notice->adorn_color1, notice->adorn_color2, notice->depth, notice->sequence,
        escaped_content, notice->content_type, notice->ip, notice->ring,
        notice->sms, notice->no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int Notice_Delete(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM notice WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int Notice_Exists(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM notice WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return exists;
}

int Notice_GetByBbsName(DBConnectionManager* manager, const char* bbs_name,
                        Notice* notices, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[22];
    int count = 0;

    if (!manager || !bbs_name || !notices || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_name, no, category, m_nickname, m_id, m_user_id, m_sex, title, "
        "create_day, comment, view, recom, adorn, adorn_color1, adorn_color2, depth, "
        "sequence, content, content_type, ip, ring, sms FROM notice "
        "WHERE bbs_name = '%s' ORDER BY no DESC LIMIT %d", bbs_name, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&notices[count], 0, sizeof(Notice));

        if (row[0]) strncpy(notices[count].bbs_name, row[0], sizeof(notices[count].bbs_name) - 1);
        notices[count].no = row[1] ? (unsigned int)atoi(row[1]) : 0;
        notices[count].category = row[2] ? (unsigned char)atoi(row[2]) : 0;
        if (row[3]) strncpy(notices[count].m_nickname, row[3], sizeof(notices[count].m_nickname) - 1);
        notices[count].m_id = row[4] ? (unsigned int)atoi(row[4]) : 0;
        if (row[5]) strncpy(notices[count].m_user_id, row[5], sizeof(notices[count].m_user_id) - 1);
        notices[count].m_sex = row[6] ? row[6][0] : 'm';
        if (row[7]) strncpy(notices[count].title, row[7], sizeof(notices[count].title) - 1);
        notices[count].create_day = row[8] ? (unsigned int)atoi(row[8]) : 0;
        notices[count].comment = row[9] ? (unsigned short)atoi(row[9]) : 0;
        notices[count].view = row[10] ? (unsigned int)atoi(row[10]) : 0;
        notices[count].recom = row[11] ? (unsigned int)atoi(row[11]) : 0;
        notices[count].adorn = row[12] ? (unsigned char)atoi(row[12]) : 0;
        notices[count].adorn_color1 = row[13] ? (unsigned char)atoi(row[13]) : 0;
        notices[count].adorn_color2 = row[14] ? (unsigned char)atoi(row[14]) : 0;
        notices[count].depth = row[15] ? (unsigned char)atoi(row[15]) : 0;
        notices[count].sequence = row[16] ? atof(row[16]) : 0.0;

        if (row[17]) {
            notices[count].content = (char*)malloc(strlen(row[17]) + 1);
            if (notices[count].content) strcpy(notices[count].content, row[17]);
        } else {
            notices[count].content = NULL;
        }

        if (row[18]) strncpy(notices[count].content_type, row[18], sizeof(notices[count].content_type) - 1);
        if (row[19]) strncpy(notices[count].ip, row[19], sizeof(notices[count].ip) - 1);
        notices[count].ring = row[20] ? (unsigned short)atoi(row[20]) : 0;
        notices[count].sms = row[21] ? row[21][0] : 'n';

        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int Notice_GetByCategory(DBConnectionManager* manager, unsigned char category,
                         Notice* notices, int max_count, int* actual_count) {
    return Notice_GetByBbsName(manager, "", notices, 0, actual_count);
}

int Notice_GetByUserId(DBConnectionManager* manager, unsigned int m_id,
                       Notice* notices, int max_count, int* actual_count) {
    return Notice_GetByBbsName(manager, "", notices, 0, actual_count);
}

int Notice_GetRecent(DBConnectionManager* manager, int limit,
                     Notice* notices, int max_count, int* actual_count) {
    return Notice_GetByBbsName(manager, "", notices, limit < max_count ? limit : max_count, actual_count);
}

void Notice_Free(Notice* notice) {
    if (notice && notice->content) {
        free(notice->content);
        notice->content = NULL;
    }
}

void Notice_PrintInfo(const Notice* notice) {
    if (!notice) {
        printf("Notice is NULL\n");
        return;
    }

    printf("Notice No: %u\n", notice->no);
    printf("  BBS Name: %s\n", notice->bbs_name);
    printf("  Category: %u\n", notice->category);
    printf("  Nickname: %s\n", notice->m_nickname);
    printf("  User ID: %u (%s)\n", notice->m_id, notice->m_user_id);
    printf("  Sex: %c\n", notice->m_sex);
    printf("  Title: %s\n", notice->title);
    printf("  Create Day: %u\n", notice->create_day);
    printf("  Stats: comments=%u, views=%u, recom=%u\n",
           notice->comment, notice->view, notice->recom);
    printf("  Content Type: %s\n", notice->content_type);
    if (notice->content) {
        printf("  Content: %.100s%s\n", notice->content,
               strlen(notice->content) > 100 ? "..." : "");
    }
}
