#include "passwd_mod_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */

static int test_passed = 0;
static int test_failed = 0;

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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== PasswdModEntry ORM Test Suite ===\n\n");

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
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("=== All Tests Completed ===\n");

    return (test_failed == 0) ? 0 : 1;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM passwd_mod_entry WHERE m_id IN (100001, 100002, 100003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    PasswdModEntry entry, entries[10];
    int count;

    printf("[TEST] Basic CRUD Operations (Composite Primary Key)\n");

    /* Test 1: Add entries */
    printf("  [1] Add entry with specific time... ");
    memset(&entry, 0, sizeof(PasswdModEntry));
    entry.m_id = 100001;
    strcpy(entry.occ_time, "2025-11-18 10:00:00");
    strcpy(entry.ip, "192.168.1.100");
    strcpy(entry.pre_passwd, "old_password_hash_12345");

    if (PasswdModEntry_Add(manager, &entry) == 0) {
        printf("PASS\n");
        test_passed++;
    } else {
        printf("FAIL\n");
        test_failed++;
    }

    /* Test 2: Add entry with NOW() */
    printf("  [2] Add entry with NOW()... ");
    memset(&entry, 0, sizeof(PasswdModEntry));
    entry.m_id = 100002;
    entry.occ_time[0] = '\0';  /* Use NOW() */
    strcpy(entry.ip, "192.168.1.101");
    strcpy(entry.pre_passwd, "old_password_hash_67890");

    if (PasswdModEntry_Add(manager, &entry) == 0) {
        printf("PASS\n");
        test_passed++;
    } else {
        printf("FAIL\n");
        test_failed++;
    }

    /* Test 3: Get by composite primary key */
    printf("  [3] Get by composite key (m_id + occ_time)... ");
    if (PasswdModEntry_Get(manager, 100001, "2025-11-18 10:00:00", &entry) == 0) {
        printf("PASS\n");
        test_passed++;
        PasswdModEntry_PrintInfo(&entry);
    } else {
        printf("FAIL\n");
        test_failed++;
    }

    /* Test 4: Exists check */
    printf("  [4] Check if entry exists... ");
    if (PasswdModEntry_Exists(manager, 100001, "2025-11-18 10:00:00")) {
        printf("PASS (entry exists)\n");
        test_passed++;
    } else {
        printf("FAIL (entry should exist)\n");
        test_failed++;
    }

    /* Test 5: Update entry */
    printf("  [5] Update entry... ");
    memset(&entry, 0, sizeof(PasswdModEntry));
    entry.m_id = 100001;
    strcpy(entry.occ_time, "2025-11-18 10:00:00");
    strcpy(entry.ip, "192.168.1.200");
    strcpy(entry.pre_passwd, "updated_password_hash");

    if (PasswdModEntry_Update(manager, 100001, "2025-11-18 10:00:00", &entry) == 0) {
        printf("PASS\n");
        test_passed++;

        /* Verify update */
        if (PasswdModEntry_Get(manager, 100001, "2025-11-18 10:00:00", &entry) == 0) {
            if (strcmp(entry.ip, "192.168.1.200") == 0) {
                printf("    Verified: IP updated to %s\n", entry.ip);
            }
        }
    } else {
        printf("FAIL\n");
        test_failed++;
    }

    /* Test 6: Delete by composite key */
    printf("  [6] Delete by composite key... ");
    if (PasswdModEntry_Delete(manager, 100001, "2025-11-18 10:00:00") == 0) {
        printf("PASS\n");
        test_passed++;

        /* Verify deletion */
        if (!PasswdModEntry_Exists(manager, 100001, "2025-11-18 10:00:00")) {
            printf("    Verified: entry deleted\n");
        }
    } else {
        printf("FAIL\n");
        test_failed++;
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    PasswdModEntry entry, entries[10];
    int count;

    printf("[TEST] Business Operations\n");

    /* Clean up and add test data */
    cleanup_test_data(manager);

    /* Add multiple entries for same member */
    memset(&entry, 0, sizeof(PasswdModEntry));
    entry.m_id = 100001;
    strcpy(entry.occ_time, "2025-11-18 10:00:00");
    strcpy(entry.ip, "192.168.1.100");
    strcpy(entry.pre_passwd, "password_hash_1");
    PasswdModEntry_Add(manager, &entry);

    entry.m_id = 100001;
    strcpy(entry.occ_time, "2025-11-18 11:00:00");
    strcpy(entry.ip, "192.168.1.100");
    strcpy(entry.pre_passwd, "password_hash_2");
    PasswdModEntry_Add(manager, &entry);

    entry.m_id = 100002;
    strcpy(entry.occ_time, "2025-11-18 12:00:00");
    strcpy(entry.ip, "192.168.1.101");
    strcpy(entry.pre_passwd, "password_hash_3");
    PasswdModEntry_Add(manager, &entry);

    entry.m_id = 100003;
    strcpy(entry.occ_time, "2025-11-18 13:00:00");
    strcpy(entry.ip, "192.168.1.100");
    strcpy(entry.pre_passwd, "password_hash_4");
    PasswdModEntry_Add(manager, &entry);

    /* Test 1: Get by member ID */
    printf("  [1] Get by member ID (100001)... ");
    if (PasswdModEntry_GetByMemberID(manager, 100001, entries, 10, &count) == 0 && count == 2) {
        printf("PASS (found %d entries)\n", count);
        test_passed++;
        for (int i = 0; i < count; i++) {
            printf("    Entry %d: time=%s, ip=%s\n",
                   i+1, entries[i].occ_time, entries[i].ip);
        }
    } else {
        printf("FAIL (expected 2, got %d)\n", count);
        test_failed++;
    }

    /* Test 2: Get by IP address */
    printf("  [2] Get by IP (192.168.1.100)... ");
    if (PasswdModEntry_GetByIP(manager, "192.168.1.100", entries, 10, &count) == 0 && count == 3) {
        printf("PASS (found %d entries)\n", count);
        test_passed++;
    } else {
        printf("FAIL (expected 3, got %d)\n", count);
        test_failed++;
    }

    /* Test 3: Get by time range */
    printf("  [3] Get by time range... ");
    if (PasswdModEntry_GetByTimeRange(manager, "2025-11-18 10:30:00", "2025-11-18 12:30:00",
                                       entries, 10, &count) == 0 && count == 2) {
        printf("PASS (found %d entries)\n", count);
        test_passed++;
    } else {
        printf("FAIL (expected 2, got %d)\n", count);
        test_failed++;
    }

    /* Test 4: Get recent entries */
    printf("  [4] Get recent entries (limit 2)... ");
    if (PasswdModEntry_GetRecent(manager, entries, 2, &count) == 0 && count == 2) {
        printf("PASS (found %d entries)\n", count);
        test_passed++;
        printf("    Most recent: m_id=%d, time=%s\n",
               entries[0].m_id, entries[0].occ_time);
    } else {
        printf("FAIL (expected 2, got %d)\n", count);
        test_failed++;
    }

    /* Test 5: Delete all entries for a member */
    printf("  [5] Delete by member ID (100001)... ");
    if (PasswdModEntry_DeleteByMemberID(manager, 100001) == 0) {
        printf("PASS\n");
        test_passed++;

        /* Verify deletion */
        if (PasswdModEntry_GetByMemberID(manager, 100001, entries, 10, &count) == 0 && count == 0) {
            printf("    Verified: all entries for m_id=100001 deleted\n");
        }
    } else {
        printf("FAIL\n");
        test_failed++;
    }

    printf("\n");
}
