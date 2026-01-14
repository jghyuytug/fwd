#include "event_visit_room_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

/* Helper function to convert binary data to hex string */
static void bin_to_hex(const unsigned char* bin, unsigned int bin_len, char* hex) {
    const char hex_chars[] = "0123456789ABCDEF";
    unsigned int i;
    for (i = 0; i < bin_len; i++) {
        hex[i * 2] = hex_chars[(bin[i] >> 4) & 0x0F];
        hex[i * 2 + 1] = hex_chars[bin[i] & 0x0F];
    }
    hex[bin_len * 2] = '\0';
}

/* Helper function to convert hex string to binary data */
static unsigned int hex_to_bin(const char* hex, unsigned char* bin, unsigned int max_len) {
    unsigned int len = strlen(hex);
    unsigned int bin_len = len / 2;
    unsigned int i;

    if (bin_len > max_len) bin_len = max_len;

    for (i = 0; i < bin_len; i++) {
        char hi = hex[i * 2];
        char lo = hex[i * 2 + 1];

        unsigned char h = (hi >= '0' && hi <= '9') ? (hi - '0') :
                         (hi >= 'A' && hi <= 'F') ? (hi - 'A' + 10) :
                         (hi >= 'a' && hi <= 'f') ? (hi - 'a' + 10) : 0;
        unsigned char l = (lo >= '0' && lo <= '9') ? (lo - '0') :
                         (lo >= 'A' && lo <= 'F') ? (lo - 'A' + 10) :
                         (lo >= 'a' && lo <= 'f') ? (lo - 'a' + 10) : 0;

        bin[i] = (h << 4) | l;
    }

    return bin_len;
}

int EventVisitRoomInfo_Add(DBConnectionManager* manager, const EventVisitRoomInfo* info) {
    char query[MAX_QUERY_LEN];
    char hex_blob[MAX_VISIT_CHARAC_BLOB_SIZE * 2 + 1];
    DBQueryResult result;

    if (!manager || !info) return -1;

    bin_to_hex(info->visit_charac_no, info->visit_charac_no_len, hex_blob);

    snprintf(query, sizeof(query),
        "INSERT INTO event_visit_room_info "
        "(charac_no, visit_cnt, visit_charac_no, update_time) "
        "VALUES (%u, %u, 0x%s, '%s')",
        info->charac_no, info->visit_cnt, hex_blob, info->update_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventVisitRoomInfo_Get(DBConnectionManager* manager, unsigned int charac_no,
                           EventVisitRoomInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, visit_cnt, HEX(visit_charac_no), update_time "
        "FROM event_visit_room_info WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(info, 0, sizeof(EventVisitRoomInfo));
    info->charac_no = row[0] ? (unsigned int)atoi(row[0]) : 0;
    info->visit_cnt = row[1] ? (unsigned char)atoi(row[1]) : 0;

    if (row[2]) {
        info->visit_charac_no_len = hex_to_bin(row[2], info->visit_charac_no,
                                                 MAX_VISIT_CHARAC_BLOB_SIZE);
    } else {
        info->visit_charac_no_len = 0;
    }

    if (row[3]) strncpy(info->update_time, row[3], sizeof(info->update_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int EventVisitRoomInfo_Update(DBConnectionManager* manager, const EventVisitRoomInfo* info) {
    char query[MAX_QUERY_LEN];
    char hex_blob[MAX_VISIT_CHARAC_BLOB_SIZE * 2 + 1];
    DBQueryResult result;

    if (!manager || !info) return -1;

    bin_to_hex(info->visit_charac_no, info->visit_charac_no_len, hex_blob);

    snprintf(query, sizeof(query),
        "UPDATE event_visit_room_info SET "
        "visit_cnt = %u, visit_charac_no = 0x%s, update_time = '%s' "
        "WHERE charac_no = %u",
        info->visit_cnt, hex_blob, info->update_time, info->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventVisitRoomInfo_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_visit_room_info WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventVisitRoomInfo_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_visit_room_info WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count > 0;
}

int EventVisitRoomInfo_GetAll(DBConnectionManager* manager, EventVisitRoomInfo* infos,
                              int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, visit_cnt, HEX(visit_charac_no), update_time "
        "FROM event_visit_room_info ORDER BY update_time DESC LIMIT %d",
        max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(EventVisitRoomInfo));
        infos[count].charac_no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        infos[count].visit_cnt = row[1] ? (unsigned char)atoi(row[1]) : 0;

        if (row[2]) {
            infos[count].visit_charac_no_len = hex_to_bin(row[2],
                                                            infos[count].visit_charac_no,
                                                            MAX_VISIT_CHARAC_BLOB_SIZE);
        } else {
            infos[count].visit_charac_no_len = 0;
        }

        if (row[3]) strncpy(infos[count].update_time, row[3],
                            sizeof(infos[count].update_time) - 1);

        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventVisitRoomInfo_GetByVisitCount(DBConnectionManager* manager, unsigned char min_cnt,
                                        EventVisitRoomInfo* infos, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, visit_cnt, HEX(visit_charac_no), update_time "
        "FROM event_visit_room_info WHERE visit_cnt >= %u "
        "ORDER BY visit_cnt DESC",
        min_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(EventVisitRoomInfo));
        infos[count].charac_no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        infos[count].visit_cnt = row[1] ? (unsigned char)atoi(row[1]) : 0;

        if (row[2]) {
            infos[count].visit_charac_no_len = hex_to_bin(row[2],
                                                            infos[count].visit_charac_no,
                                                            MAX_VISIT_CHARAC_BLOB_SIZE);
        } else {
            infos[count].visit_charac_no_len = 0;
        }

        if (row[3]) strncpy(infos[count].update_time, row[3],
                            sizeof(infos[count].update_time) - 1);

        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventVisitRoomInfo_IncrementVisitCount(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_visit_room_info SET "
        "visit_cnt = visit_cnt + 1, update_time = NOW() "
        "WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventVisitRoomInfo_GetRecentUpdates(DBConnectionManager* manager, const char* since_time,
                                         EventVisitRoomInfo* infos, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !since_time || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, visit_cnt, HEX(visit_charac_no), update_time "
        "FROM event_visit_room_info WHERE update_time >= '%s' "
        "ORDER BY update_time DESC",
        since_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(EventVisitRoomInfo));
        infos[count].charac_no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        infos[count].visit_cnt = row[1] ? (unsigned char)atoi(row[1]) : 0;

        if (row[2]) {
            infos[count].visit_charac_no_len = hex_to_bin(row[2],
                                                            infos[count].visit_charac_no,
                                                            MAX_VISIT_CHARAC_BLOB_SIZE);
        } else {
            infos[count].visit_charac_no_len = 0;
        }

        if (row[3]) strncpy(infos[count].update_time, row[3],
                            sizeof(infos[count].update_time) - 1);

        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void EventVisitRoomInfo_PrintInfo(const EventVisitRoomInfo* info) {
    if (!info) return;

    printf("=== Event Visit Room Info ===\n");
    printf("Character No: %u\n", info->charac_no);
    printf("Visit Count: %u\n", info->visit_cnt);
    printf("Visit Charac Blob Size: %u bytes\n", info->visit_charac_no_len);
    printf("Update Time: %s\n", info->update_time);
    printf("==============================\n");
}
