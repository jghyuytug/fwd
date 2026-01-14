#include "event_goldcard_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int EventGoldcardInfo_Add(DBConnectionManager* manager, const EventGoldcardInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_goldcard_info (m_id, coupon) VALUES (%d, %u)",
        info->m_id, info->coupon);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardInfo_Get(DBConnectionManager* manager, int m_id, EventGoldcardInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, coupon FROM event_goldcard_info WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(info, 0, sizeof(EventGoldcardInfo));
    info->m_id = row[0] ? atoi(row[0]) : 0;
    info->coupon = row[1] ? (unsigned short)atoi(row[1]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardInfo_Update(DBConnectionManager* manager, const EventGoldcardInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_goldcard_info SET coupon = %u WHERE m_id = %d",
        info->coupon, info->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardInfo_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_goldcard_info WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardInfo_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT 1 FROM event_goldcard_info WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    int exists = (DBQueryResult_FetchRow(&result, row) > 0);
    DBQueryResult_Free(&result);
    return exists;
}

int EventGoldcardInfo_GetAll(DBConnectionManager* manager, EventGoldcardInfo* infos,
                              int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, coupon FROM event_goldcard_info ORDER BY m_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(EventGoldcardInfo));
        infos[count].m_id = row[0] ? atoi(row[0]) : 0;
        infos[count].coupon = row[1] ? (unsigned short)atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardInfo_AddCoupon(DBConnectionManager* manager, int m_id, unsigned short amount) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_goldcard_info SET coupon = coupon + %u WHERE m_id = %d",
        amount, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardInfo_UseCoupon(DBConnectionManager* manager, int m_id, unsigned short amount) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_goldcard_info SET coupon = coupon - %u WHERE m_id = %d AND coupon >= %u",
        amount, m_id, amount);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardInfo_CountByCouponRange(DBConnectionManager* manager,
                                          unsigned short min_coupon, unsigned short max_coupon,
                                          int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !count) return -1;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_goldcard_info WHERE coupon BETWEEN %u AND %u",
        min_coupon, max_coupon);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return 0;
}

void EventGoldcardInfo_PrintInfo(const EventGoldcardInfo* info) {
    if (!info) return;

    printf("=== Event Goldcard Info ===\n");
    printf("Member ID: %d\n", info->m_id);
    printf("Coupon: %u\n", info->coupon);
    printf("===========================\n");
}
