#include "event_visit_room_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test counters */
static int test_total = 0;
static int test_passed = 0;
static int test_failed = 0;

/* Test result reporting */
void test_result(const char* test_name, int passed) {
    test_total++;
    if (passed) {
        test_passed++;
        printf("[PASS] %s\n", test_name);
    } else {
        test_failed++;
        printf("[FAIL] %s\n", test_name);
    }
}

/* Cleanup test data */
void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM event_visit_room_info WHERE charac_no IN (100001, 100002, 100003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);
    DBQueryResult_Free(&result);
}

/* Test basic CRUD operations */
void test_basic_crud(DBConnectionManager* manager) {
    EventVisitRoomInfo info;
    int ret;

    printf("\n=== Test Basic CRUD Operations ===\n");

    /* Test Add with BLOB data */
    memset(&info, 0, sizeof(EventVisitRoomInfo));
    info.charac_no = 100001;
    info.visit_cnt = 5;

    /* Create sample BLOB data (array of character IDs) */
    unsigned int sample_ids[] = {200001, 200002, 200003, 200004, 200005};
    memcpy(info.visit_charac_no, sample_ids, sizeof(sample_ids));
    info.visit_charac_no_len = sizeof(sample_ids);

    strcpy(info.update_time, "2025-11-18 10:00:00");

    ret = EventVisitRoomInfo_Add(manager, &info);
    test_result("EventVisitRoomInfo_Add (with BLOB)", ret == 0);

    /* Test Exists */
    ret = EventVisitRoomInfo_Exists(manager, 100001);
    test_result("EventVisitRoomInfo_Exists (should exist)", ret == 1);

    /* Test Get */
    EventVisitRoomInfo retrieved;
    memset(&retrieved, 0, sizeof(EventVisitRoomInfo));
    ret = EventVisitRoomInfo_Get(manager, 100001, &retrieved);
    test_result("EventVisitRoomInfo_Get", ret == 0 && retrieved.charac_no == 100001);
    test_result("Verify visit_cnt", retrieved.visit_cnt == 5);
    test_result("Verify BLOB length", retrieved.visit_charac_no_len == sizeof(sample_ids));

    if (ret == 0) {
        printf("\nRetrieved info:\n");
        EventVisitRoomInfo_PrintInfo(&retrieved);

        /* Verify BLOB data integrity */
        int blob_match = (memcmp(retrieved.visit_charac_no,
                                  sample_ids, sizeof(sample_ids)) == 0);
        test_result("Verify BLOB data integrity", blob_match);
    }

    /* Test Update */
    retrieved.visit_cnt = 10;
    strcpy(retrieved.update_time, "2025-11-18 12:00:00");

    ret = EventVisitRoomInfo_Update(manager, &retrieved);
    test_result("EventVisitRoomInfo_Update", ret == 0);

    /* Verify update */
    EventVisitRoomInfo updated;
    memset(&updated, 0, sizeof(EventVisitRoomInfo));
    ret = EventVisitRoomInfo_Get(manager, 100001, &updated);
    test_result("Verify Update (visit_cnt)", ret == 0 && updated.visit_cnt == 10);

    /* Test Delete */
    ret = EventVisitRoomInfo_Delete(manager, 100001);
    test_result("EventVisitRoomInfo_Delete", ret == 0);

    /* Verify deletion */
    ret = EventVisitRoomInfo_Exists(manager, 100001);
    test_result("EventVisitRoomInfo_Exists (after delete)", ret == 0);
}

/* Test business operations */
void test_business_operations(DBConnectionManager* manager) {
    EventVisitRoomInfo infos[10];
    int actual_count;
    int ret;

    printf("\n=== Test Business Operations ===\n");

    /* Add test data */
    for (int i = 0; i < 3; i++) {
        EventVisitRoomInfo info;
        memset(&info, 0, sizeof(EventVisitRoomInfo));

        info.charac_no = 100001 + i;
        info.visit_cnt = 5 + (i * 3);

        unsigned int sample_id = 200001 + i;
        memcpy(info.visit_charac_no, &sample_id, sizeof(sample_id));
        info.visit_charac_no_len = sizeof(sample_id);

        snprintf(info.update_time, sizeof(info.update_time),
                 "2025-11-18 %02d:00:00", 10 + i);

        EventVisitRoomInfo_Add(manager, &info);
    }

    /* Test GetAll */
    memset(infos, 0, sizeof(infos));
    ret = EventVisitRoomInfo_GetAll(manager, infos, 10, &actual_count);
    test_result("GetAll", ret == 0 && actual_count >= 3);
    printf("  Found %d records (ordered by update_time DESC)\n", actual_count);

    /* Test GetByVisitCount */
    memset(infos, 0, sizeof(infos));
    ret = EventVisitRoomInfo_GetByVisitCount(manager, 8, infos, 10, &actual_count);
    test_result("GetByVisitCount (>= 8)", ret == 0);
    printf("  Found %d records with visit_cnt >= 8\n", actual_count);

    /* Test IncrementVisitCount */
    ret = EventVisitRoomInfo_IncrementVisitCount(manager, 100001);
    test_result("IncrementVisitCount", ret == 0);

    /* Verify visit count incremented */
    EventVisitRoomInfo info;
    memset(&info, 0, sizeof(EventVisitRoomInfo));
    ret = EventVisitRoomInfo_Get(manager, 100001, &info);
    test_result("Verify visit count incremented", ret == 0 && info.visit_cnt == 6);

    /* Test GetRecentUpdates */
    memset(infos, 0, sizeof(infos));
    ret = EventVisitRoomInfo_GetRecentUpdates(manager, "2025-11-18 00:00:00",
                                               infos, 10, &actual_count);
    test_result("GetRecentUpdates", ret == 0 && actual_count >= 3);
    printf("  Found %d records since 2025-11-18 00:00:00\n", actual_count);
}

/* Main test function */
int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* Initialize connection manager */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize connection manager\n");
        return 1;
    }

    /* Connect to database */
    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== EventVisitRoomInfo ORM Test Suite ===\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    /* Print summary */
    printf("\n=== Test Summary ===\n");
    printf("Total: %d\n", test_total);
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("Success Rate: %.1f%%\n",
           test_total > 0 ? (test_passed * 100.0 / test_total) : 0.0);

    return test_failed > 0 ? 1 : 0;
}
