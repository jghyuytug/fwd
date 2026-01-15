#include "member_key_option_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test counters */
static int test_total = 0;
static int test_passed = 0;

void test_basic_crud(DBConnectionManager* manager);
void test_business_operations(DBConnectionManager* manager);
void cleanup_test_data(DBConnectionManager* manager);

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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_LOGIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== MemberKeyOption ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    printf("\n=== Test Results ===\n");
    printf("Total: %d, Passed: %d, Failed: %d\n",
           test_total, test_passed, test_total - test_passed);
    printf("\n=== All Tests Completed ===\n");

    return (test_total == test_passed) ? 0 : 1;
}

void test_basic_crud(DBConnectionManager* manager) {
    MemberKeyOption option;
    unsigned char test_data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0xAB, 0xCD, 0xEF};

    printf("[TEST] Basic CRUD Operations\n");

    /* Test 1: Add new key option */
    test_total++;
    memset(&option, 0, sizeof(MemberKeyOption));
    option.m_id = 100001ULL;
    option.key_type = 1;
    option.key_option = test_data;
    option.key_option_len = sizeof(test_data);

    if (MemberKeyOption_Add(manager, &option) == 0) {
        printf("  [PASS] Add key option\n");
        test_passed++;
    } else {
        printf("  [FAIL] Add key option\n");
    }

    /* Test 2: Check exists */
    test_total++;
    if (MemberKeyOption_Exists(manager, 100001ULL, 1) == 1) {
        printf("  [PASS] Key option exists\n");
        test_passed++;
    } else {
        printf("  [FAIL] Key option should exist\n");
    }

    /* Test 3: Get key option */
    test_total++;
    MemberKeyOption retrieved;
    memset(&retrieved, 0, sizeof(MemberKeyOption));

    if (MemberKeyOption_Get(manager, 100001ULL, 1, &retrieved) == 0) {
        if (retrieved.m_id == 100001ULL && retrieved.key_type == 1 &&
            retrieved.key_option_len == sizeof(test_data) &&
            memcmp(retrieved.key_option, test_data, sizeof(test_data)) == 0) {
            printf("  [PASS] Get key option with correct data\n");
            test_passed++;
        } else {
            printf("  [FAIL] Get key option with incorrect data\n");
        }
        MemberKeyOption_Free(&retrieved);
    } else {
        printf("  [FAIL] Get key option\n");
    }

    /* Test 4: Update key option */
    test_total++;
    unsigned char updated_data[] = {0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
    option.key_option = updated_data;
    option.key_option_len = sizeof(updated_data);

    if (MemberKeyOption_Update(manager, &option) == 0) {
        memset(&retrieved, 0, sizeof(MemberKeyOption));
        if (MemberKeyOption_Get(manager, 100001ULL, 1, &retrieved) == 0) {
            if (retrieved.key_option_len == sizeof(updated_data) &&
                memcmp(retrieved.key_option, updated_data, sizeof(updated_data)) == 0) {
                printf("  [PASS] Update key option\n");
                test_passed++;
            } else {
                printf("  [FAIL] Update key option - data mismatch\n");
            }
            MemberKeyOption_Free(&retrieved);
        } else {
            printf("  [FAIL] Update key option - cannot retrieve\n");
        }
    } else {
        printf("  [FAIL] Update key option\n");
    }

    /* Test 5: Delete key option */
    test_total++;
    if (MemberKeyOption_Delete(manager, 100001ULL, 1) == 0) {
        if (MemberKeyOption_Exists(manager, 100001ULL, 1) == 0) {
            printf("  [PASS] Delete key option\n");
            test_passed++;
        } else {
            printf("  [FAIL] Delete key option - still exists\n");
        }
    } else {
        printf("  [FAIL] Delete key option\n");
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    MemberKeyOption options[5];
    unsigned char test_data1[] = {0x11, 0x22, 0x33};
    unsigned char test_data2[] = {0x44, 0x55, 0x66, 0x77};
    unsigned char test_data3[] = {0x88, 0x99, 0xAA, 0xBB, 0xCC};
    int actual_count;

    printf("[TEST] Business Operations\n");

    /* Test 6: Add multiple key options for same member */
    test_total++;
    memset(&options[0], 0, sizeof(MemberKeyOption));
    options[0].m_id = 200001ULL;
    options[0].key_type = 1;
    options[0].key_option = test_data1;
    options[0].key_option_len = sizeof(test_data1);
    MemberKeyOption_Add(manager, &options[0]);

    memset(&options[1], 0, sizeof(MemberKeyOption));
    options[1].m_id = 200001ULL;
    options[1].key_type = 2;
    options[1].key_option = test_data2;
    options[1].key_option_len = sizeof(test_data2);
    MemberKeyOption_Add(manager, &options[1]);

    memset(&options[2], 0, sizeof(MemberKeyOption));
    options[2].m_id = 200001ULL;
    options[2].key_type = 3;
    options[2].key_option = test_data3;
    options[2].key_option_len = sizeof(test_data3);

    if (MemberKeyOption_Add(manager, &options[2]) == 0) {
        printf("  [PASS] Add multiple key options\n");
        test_passed++;
    } else {
        printf("  [FAIL] Add multiple key options\n");
    }

    /* Test 7: Get all key options by member ID */
    test_total++;
    MemberKeyOption retrieved[5];
    memset(retrieved, 0, sizeof(retrieved));

    if (MemberKeyOption_GetByMemberId(manager, 200001ULL, retrieved, 5, &actual_count) == 0) {
        if (actual_count == 3) {
            printf("  [PASS] Get all key options by member ID (count=%d)\n", actual_count);
            test_passed++;

            /* Print details */
            for (int i = 0; i < actual_count; i++) {
                printf("    Key type %u: %zu bytes\n",
                       retrieved[i].key_type, retrieved[i].key_option_len);
            }
        } else {
            printf("  [FAIL] Get all key options - expected 3, got %d\n", actual_count);
        }
        MemberKeyOption_FreeArray(retrieved, actual_count);
    } else {
        printf("  [FAIL] Get all key options by member ID\n");
    }

    /* Test 8: Delete all key options by member ID */
    test_total++;
    if (MemberKeyOption_DeleteByMemberId(manager, 200001ULL) == 0) {
        memset(retrieved, 0, sizeof(retrieved));
        MemberKeyOption_GetByMemberId(manager, 200001ULL, retrieved, 5, &actual_count);
        if (actual_count == 0) {
            printf("  [PASS] Delete all key options by member ID\n");
            test_passed++;
        } else {
            printf("  [FAIL] Delete all key options - %d still remain\n", actual_count);
            MemberKeyOption_FreeArray(retrieved, actual_count);
        }
    } else {
        printf("  [FAIL] Delete all key options by member ID\n");
    }

    printf("\n");
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM member_key_option WHERE m_id IN (100001, 200001)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}
