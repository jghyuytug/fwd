#include "dnf_gamein_notice_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* CRUD Operations Implementation */

int DnfGameinNotice_Add(DBConnectionManager* manager, const DnfGameinNotice* notice) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !notice) {
        return -1;
    }

    /* Handle datetime - support custom time or NOW() */
    if (notice->reg_time[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO dnf_gamein_notice (img_name, server_id, reg_time, open_flag) "
            "VALUES ('%s', %u, '%s', '%c')",
            notice->img_name, notice->server_id, notice->reg_time, notice->open_flag);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO dnf_gamein_notice (img_name, server_id, reg_time, open_flag) "
            "VALUES ('%s', %u, NOW(), '%c')",
            notice->img_name, notice->server_id, notice->open_flag);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameinNotice_Get(DBConnectionManager* manager, unsigned int no, DnfGameinNotice* notice) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !notice) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT no, img_name, server_id, reg_time, open_flag FROM dnf_gamein_notice WHERE no = %u",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(notice, 0, sizeof(DnfGameinNotice));
    notice->no = row[0] ? (unsigned int)atoi(row[0]) : 0;
    if (row[1]) {
        strncpy(notice->img_name, row[1], sizeof(notice->img_name) - 1);
        notice->img_name[sizeof(notice->img_name) - 1] = '\0';
    }
    notice->server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
    if (row[3]) {
        strncpy(notice->reg_time, row[3], sizeof(notice->reg_time) - 1);
        notice->reg_time[sizeof(notice->reg_time) - 1] = '\0';
    }
    notice->open_flag = row[4] ? row[4][0] : 'n';

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameinNotice_Update(DBConnectionManager* manager, const DnfGameinNotice* notice) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !notice) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_gamein_notice SET img_name = '%s', server_id = %u, "
        "reg_time = '%s', open_flag = '%c' WHERE no = %u",
        notice->img_name, notice->server_id, notice->reg_time, notice->open_flag, notice->no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameinNotice_Delete(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_gamein_notice WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameinNotice_Exists(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_gamein_notice WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count > 0 ? 1 : 0;
}

/* Business Operations Implementation */

int DnfGameinNotice_GetByServerId(DBConnectionManager* manager, unsigned char server_id,
                                   DnfGameinNotice* notices, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !notices || !actual_count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT no, img_name, server_id, reg_time, open_flag "
        "FROM dnf_gamein_notice WHERE server_id = %u ORDER BY no DESC",
        server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&notices[count], 0, sizeof(DnfGameinNotice));
        notices[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(notices[count].img_name, row[1], sizeof(notices[count].img_name) - 1);
            notices[count].img_name[sizeof(notices[count].img_name) - 1] = '\0';
        }
        notices[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        if (row[3]) {
            strncpy(notices[count].reg_time, row[3], sizeof(notices[count].reg_time) - 1);
            notices[count].reg_time[sizeof(notices[count].reg_time) - 1] = '\0';
        }
        notices[count].open_flag = row[4] ? row[4][0] : 'n';
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameinNotice_GetByOpenFlag(DBConnectionManager* manager, char open_flag,
                                  DnfGameinNotice* notices, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !notices || !actual_count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT no, img_name, server_id, reg_time, open_flag "
        "FROM dnf_gamein_notice WHERE open_flag = '%c' ORDER BY no DESC",
        open_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&notices[count], 0, sizeof(DnfGameinNotice));
        notices[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(notices[count].img_name, row[1], sizeof(notices[count].img_name) - 1);
            notices[count].img_name[sizeof(notices[count].img_name) - 1] = '\0';
        }
        notices[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        if (row[3]) {
            strncpy(notices[count].reg_time, row[3], sizeof(notices[count].reg_time) - 1);
            notices[count].reg_time[sizeof(notices[count].reg_time) - 1] = '\0';
        }
        notices[count].open_flag = row[4] ? row[4][0] : 'n';
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfGameinNotice_GetAll(DBConnectionManager* manager, DnfGameinNotice* notices,
                           int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !notices || !actual_count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT no, img_name, server_id, reg_time, open_flag "
        "FROM dnf_gamein_notice ORDER BY no DESC");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&notices[count], 0, sizeof(DnfGameinNotice));
        notices[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) {
            strncpy(notices[count].img_name, row[1], sizeof(notices[count].img_name) - 1);
            notices[count].img_name[sizeof(notices[count].img_name) - 1] = '\0';
        }
        notices[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        if (row[3]) {
            strncpy(notices[count].reg_time, row[3], sizeof(notices[count].reg_time) - 1);
            notices[count].reg_time[sizeof(notices[count].reg_time) - 1] = '\0';
        }
        notices[count].open_flag = row[4] ? row[4][0] : 'n';
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Utility Functions Implementation */

void DnfGameinNotice_PrintInfo(const DnfGameinNotice* notice) {
    if (!notice) {
        printf("Notice is NULL\n");
        return;
    }

    printf("Notice ID: %u\n", notice->no);
    printf("  Image Name: %s\n", notice->img_name);
    printf("  Server ID: %u\n", notice->server_id);
    printf("  Registration Time: %s\n", notice->reg_time);
    printf("  Open Flag: %c\n", notice->open_flag);
}
