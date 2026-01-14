#include "event_goldcard_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */
void test_basic_crud(DBConnectionManager* manager);
void test_business_operations(DBConnectionManager* manager);
void cleanup_test_data(DBConnectionManager* manager);

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize connection manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Event Goldcard Info ORM Test Suite ===\n\n");

    cleanup_test_data(&manager);

    test_basic_crud(&manager);
    test_business_operations(&manager);

    cleanup_test_data(&manager);

    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void test_basic_crud(DBConnectionManager* manager) {
    printf("--- Test 1: Basic CRUD Operations ---\n");

    EventGoldcardInfo info;
    memset(&info, 0, sizeof(EventGoldcardInfo));
    info.m_id = 100001;
    info.coupon = 50;

    printf("Adding goldcard info...\n");
    if (EventGoldcardInfo_Add(manager, &info) == 0) {
        printf("OK Add successful\n");
    } else {
        printf("ERROR Add failed\n");
        return;
    }

    EventGoldcardInfo retrieved;
    printf("\nRetrieving goldcard info...\n");
    if (EventGoldcardInfo_Get(manager, 100001, &retrieved) == 0) {
        printf("OK Get successful\n");
        EventGoldcardInfo_PrintInfo(&retrieved);
    } else {
        printf("ERROR Get failed\n");
        return;
    }

    printf("\nChecking existence...\n");
    if (EventGoldcardInfo_Exists(manager, 100001)) {
        printf("OK Record exists\n");
    } else {
        printf("ERROR Exists check failed\n");
    }

    printf("\nUpdating coupon to 100...\n");
    retrieved.coupon = 100;
    if (EventGoldcardInfo_Update(manager, &retrieved) == 0) {
        printf("OK Update successful\n");

        EventGoldcardInfo updated;
        if (EventGoldcardInfo_Get(manager, 100001, &updated) == 0) {
            printf("  Coupon: %u -> %u\n", info.coupon, updated.coupon);
        }
    } else {
        printf("ERROR Update failed\n");
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    printf("--- Test 2: Business Operations ---\n");

    EventGoldcardInfo infos[5];
    memset(infos, 0, sizeof(infos));

    for (int i = 0; i < 3; i++) {
        infos[i].m_id = 200001 + i;
        infos[i].coupon = 10 + (i * 10);
        EventGoldcardInfo_Add(manager, &infos[i]);
    }
    printf("Added 3 goldcard records\n");

    printf("\nGetting all records...\n");
    EventGoldcardInfo all_records[10];
    int count = 0;
    if (EventGoldcardInfo_GetAll(manager, all_records, 10, &count) == 0) {
        printf("OK Found %d records\n", count);
        for (int i = 0; i < count; i++) {
            printf("  [%d] m_id=%d, coupon=%u\n",
                   i + 1, all_records[i].m_id, all_records[i].coupon);
        }
    } else {
        printf("ERROR Query failed\n");
    }

    printf("\nAdding 20 coupons to member 200001...\n");
    if (EventGoldcardInfo_AddCoupon(manager, 200001, 20) == 0) {
        printf("OK AddCoupon successful\n");

        EventGoldcardInfo updated;
        if (EventGoldcardInfo_Get(manager, 200001, &updated) == 0) {
            printf("  Coupon after add: %u\n", updated.coupon);
        }
    } else {
        printf("ERROR AddCoupon failed\n");
    }

    printf("\nUsing 15 coupons from member 200001...\n");
    if (EventGoldcardInfo_UseCoupon(manager, 200001, 15) == 0) {
        printf("OK UseCoupon successful\n");

        EventGoldcardInfo updated;
        if (EventGoldcardInfo_Get(manager, 200001, &updated) == 0) {
            printf("  Coupon after use: %u\n", updated.coupon);
        }
    } else {
        printf("ERROR UseCoupon failed\n");
    }

    printf("\nCounting members with 0-50 coupons...\n");
    int range_count = 0;
    if (EventGoldcardInfo_CountByCouponRange(manager, 0, 50, &range_count) == 0) {
        printf("OK Count: %d members\n", range_count);
    } else {
        printf("ERROR Count failed\n");
    }

    printf("\n");
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM event_goldcard_info WHERE m_id >= 100001 AND m_id <= 200003");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}
