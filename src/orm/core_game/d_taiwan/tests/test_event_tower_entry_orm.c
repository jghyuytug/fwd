#include "event_tower_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int test_passed = 0;
static int test_failed = 0;

#define TEST_ASSERT(condition, message) do { \
    if (condition) { \
        printf("[PASS] %s\n", message); \
        test_passed++; \
    } else { \
        printf("[FAIL] %s\n", message); \
        test_failed++; \
    } \
} while(0)

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

    printf("=== Event Tower Entry ORM Test Suite ===\n\n");

    cleanup_test_data(&manager);
    test_basic_crud(&manager);
    test_business_operations(&manager);
    cleanup_test_data(&manager);

    DBConnectionManager_Cleanup(&manager);

    printf("\n=== Test Summary ===\n");
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("Total:  %d\n", test_passed + test_failed);

    return test_failed > 0 ? 1 : 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM event_tower_entry WHERE m_id IN (100001, 100002, 100003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    EventTowerEntry entry;
    int ret;

    printf("--- Test: Basic CRUD Operations ---\n");

    memset(&entry, 0, sizeof(EventTowerEntry));
    entry.m_id = 100001;
    entry.occ_date = 20250101;
    entry.occ_check = 1;
    entry.server_id = 1;
    entry.charac_no = 12345;
    entry.item1_no = 1001;
    entry.item1_check = 1;
    entry.item2_no = 1002;
    entry.item2_check = 0;
    entry.item3_no = 1003;
    entry.item3_check = 1;

    ret = EventTowerEntry_Add(manager, &entry);
    TEST_ASSERT(ret == 0, "Add entry");

    ret = EventTowerEntry_Exists(manager, 100001);
    TEST_ASSERT(ret == 1, "Entry exists after add");

    memset(&entry, 0, sizeof(EventTowerEntry));
    ret = EventTowerEntry_Get(manager, 100001, &entry);
    TEST_ASSERT(ret == 0, "Get entry");
    TEST_ASSERT(entry.m_id == 100001, "Get - m_id correct");
    TEST_ASSERT(entry.occ_date == 20250101, "Get - occ_date correct");
    TEST_ASSERT(entry.item1_no == 1001, "Get - item1_no correct");
    TEST_ASSERT(entry.item1_check == 1, "Get - item1_check correct");

    entry.occ_check = 2;
    entry.item2_check = 1;
    ret = EventTowerEntry_Update(manager, &entry);
    TEST_ASSERT(ret == 0, "Update entry");

    memset(&entry, 0, sizeof(EventTowerEntry));
    ret = EventTowerEntry_Get(manager, 100001, &entry);
    TEST_ASSERT(ret == 0, "Get updated entry");
    TEST_ASSERT(entry.occ_check == 2, "Update - occ_check changed");
    TEST_ASSERT(entry.item2_check == 1, "Update - item2_check changed");

    unsigned int item_no, item_check;
    ret = EventTowerEntry_CheckItem(manager, 100001, 1, &item_no, &item_check);
    TEST_ASSERT(ret == 0 && item_no == 1001 && item_check == 1, "CheckItem - item1");

    ret = EventTowerEntry_Delete(manager, 100001);
    TEST_ASSERT(ret == 0, "Delete entry");

    ret = EventTowerEntry_Exists(manager, 100001);
    TEST_ASSERT(ret == 0, "Entry not exists after delete");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    EventTowerEntry entries[10];
    EventTowerEntry test_entry;
    int actual_count;
    int ret;

    printf("--- Test: Business Operations ---\n");

    memset(&test_entry, 0, sizeof(EventTowerEntry));
    test_entry.m_id = 100002;
    test_entry.occ_date = 20250115;
    test_entry.server_id = 1;
    EventTowerEntry_Add(manager, &test_entry);

    test_entry.m_id = 100003;
    test_entry.occ_date = 20250120;
    test_entry.server_id = 2;
    EventTowerEntry_Add(manager, &test_entry);

    memset(entries, 0, sizeof(entries));
    ret = EventTowerEntry_GetByDateRange(manager, 20250110, 20250120, entries, 10, &actual_count);
    TEST_ASSERT(ret == 0, "GetByDateRange operation");
    TEST_ASSERT(actual_count == 2, "GetByDateRange - found 2 entries");

    memset(entries, 0, sizeof(entries));
    ret = EventTowerEntry_GetByServerId(manager, 1, entries, 10, &actual_count);
    TEST_ASSERT(ret == 0, "GetByServerId operation");
    TEST_ASSERT(actual_count == 1, "GetByServerId - found 1 entry with server_id=1");

    memset(entries, 0, sizeof(entries));
    ret = EventTowerEntry_GetAll(manager, entries, 10, &actual_count);
    TEST_ASSERT(ret == 0, "GetAll operation");
    TEST_ASSERT(actual_count >= 2, "GetAll - found at least 2 entries");

    printf("\n");
}
