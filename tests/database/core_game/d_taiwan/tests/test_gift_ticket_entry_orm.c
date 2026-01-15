#include "gift_ticket_entry_orm.h"
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

    printf("=== GiftTicketEntry ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    /* Delete test records by buyer_id and other_id */
    snprintf(query, sizeof(query),
        "DELETE FROM gift_ticket_entry WHERE buyer_id IN (100001, 100002, 200001) "
        "OR other_id IN (300001, 300002)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    GiftTicketEntry entry1, entry2;
    int ret;

    printf("--- Test 1: Add Gift Ticket Entries ---\n");

    /* Prepare test data 1 */
    memset(&entry1, 0, sizeof(GiftTicketEntry));
    entry1.gift_no = 100;
    entry1.buyer_id = 100001;
    entry1.buyer_date = 20251117;
    strcpy(entry1.buyer_code, "BUY001");
    entry1.buyer_check = 12345;
    entry1.other_id = 300001;
    entry1.other_date = 20251118;
    strcpy(entry1.other_code, "REC001");
    entry1.other_check = 54321;
    strcpy(entry1.message, "Happy Birthday!");

    ret = GiftTicketEntry_Add(manager, &entry1);
    printf("Add entry1 (buyer_id=100001): %s\n", ret == 0 ? "OK" : "FAIL");

    /* Prepare test data 2 */
    memset(&entry2, 0, sizeof(GiftTicketEntry));
    entry2.gift_no = 200;
    entry2.buyer_id = 100002;
    entry2.buyer_date = 20251117;
    strcpy(entry2.buyer_code, "BUY002");
    entry2.buyer_check = 11111;
    entry2.other_id = 300002;
    entry2.other_date = 20251119;
    strcpy(entry2.other_code, "REC002");
    entry2.other_check = 22222;
    strcpy(entry2.message, "Thank you!");

    ret = GiftTicketEntry_Add(manager, &entry2);
    printf("Add entry2 (buyer_id=100002): %s\n", ret == 0 ? "OK" : "FAIL");

    printf("\n--- Test 2: Get All Gift Ticket Entries ---\n");
    GiftTicketEntry all_entries[100];
    int count = 0;

    ret = GiftTicketEntry_GetAll(manager, all_entries, 100, &count);
    printf("GetAll: %s, Found %d records\n", ret == 0 ? "OK" : "FAIL", count);

    if (count > 0) {
        printf("First record:\n");
        GiftTicketEntry_PrintInfo(&all_entries[0]);
    }

    printf("\n--- Test 3: Get by buyer_id ---\n");
    GiftTicketEntry entries_by_buyer[10];
    int buyer_count = 0;

    ret = GiftTicketEntry_GetByBuyerId(manager, 100001, entries_by_buyer, 10, &buyer_count);
    printf("GetByBuyerId (buyer_id=100001): %s, Found %d records\n",
           ret == 0 ? "OK" : "FAIL", buyer_count);

    if (buyer_count > 0) {
        GiftTicketEntry_PrintInfo(&entries_by_buyer[0]);
    }

    printf("\n--- Test 4: Update Gift Ticket Entry ---\n");
    if (buyer_count > 0) {
        entries_by_buyer[0].gift_no = 150;
        strcpy(entries_by_buyer[0].message, "Updated message!");

        ret = GiftTicketEntry_Update(manager, &entries_by_buyer[0]);
        printf("Update entry (id=%u): %s\n", entries_by_buyer[0].id,
               ret == 0 ? "OK" : "FAIL");

        /* Verify update */
        GiftTicketEntry updated;
        ret = GiftTicketEntry_Get(manager, entries_by_buyer[0].id, &updated);
        printf("Verify update: %s, gift_no=%u, message=%s\n",
               ret == 0 ? "OK" : "FAIL", updated.gift_no, updated.message);
    }

    printf("\n--- Test 5: Exists Check ---\n");
    if (buyer_count > 0) {
        int exists = GiftTicketEntry_Exists(manager, entries_by_buyer[0].id);
        printf("Exists (id=%u): %s\n", entries_by_buyer[0].id,
               exists ? "YES" : "NO");

        int not_exists = GiftTicketEntry_Exists(manager, 999999);
        printf("Exists (id=999999): %s\n", not_exists ? "YES" : "NO");
    }

    printf("\n--- Test 6: Delete Gift Ticket Entry ---\n");
    if (buyer_count > 0) {
        ret = GiftTicketEntry_Delete(manager, entries_by_buyer[0].id);
        printf("Delete entry (id=%u): %s\n", entries_by_buyer[0].id,
               ret == 0 ? "OK" : "FAIL");

        /* Verify deletion */
        int exists = GiftTicketEntry_Exists(manager, entries_by_buyer[0].id);
        printf("Verify deletion: %s\n", exists ? "FAIL (still exists)" : "OK (deleted)");
    }
}

void test_business_operations(DBConnectionManager* manager) {
    GiftTicketEntry test_entries[2];
    int ret;

    printf("\n--- Test 7: Business Query - GetByOtherId ---\n");

    /* Add test data with same other_id */
    memset(&test_entries[0], 0, sizeof(GiftTicketEntry));
    test_entries[0].gift_no = 300;
    test_entries[0].buyer_id = 200001;
    test_entries[0].buyer_date = 20251117;
    strcpy(test_entries[0].buyer_code, "BUY003");
    test_entries[0].buyer_check = 33333;
    test_entries[0].other_id = 300001;
    test_entries[0].other_date = 20251120;
    strcpy(test_entries[0].other_code, "REC003");
    test_entries[0].other_check = 44444;
    strcpy(test_entries[0].message, "Congratulations!");
    GiftTicketEntry_Add(manager, &test_entries[0]);

    /* Query by other_id */
    GiftTicketEntry entries_by_other[10];
    int other_count = 0;

    ret = GiftTicketEntry_GetByOtherId(manager, 300001, entries_by_other, 10, &other_count);
    printf("GetByOtherId (other_id=300001): %s, Found %d records\n",
           ret == 0 ? "OK" : "FAIL", other_count);

    for (int i = 0; i < other_count; i++) {
        printf("  Record %d: buyer_id=%u, other_id=%u, message=%s\n",
               i + 1, entries_by_other[i].buyer_id, entries_by_other[i].other_id,
               entries_by_other[i].message);
    }

    printf("\n--- Test 8: VARCHAR Field Handling ---\n");
    if (other_count > 0) {
        printf("Testing VARCHAR fields:\n");
        printf("  buyer_code: '%s' (length=%zu)\n",
               entries_by_other[0].buyer_code, strlen(entries_by_other[0].buyer_code));
        printf("  other_code: '%s' (length=%zu)\n",
               entries_by_other[0].other_code, strlen(entries_by_other[0].other_code));
        printf("  message: '%s' (length=%zu)\n",
               entries_by_other[0].message, strlen(entries_by_other[0].message));
    }
}
