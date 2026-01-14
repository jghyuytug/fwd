#include "event_server_message_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Test function declarations */
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

    printf("=== Event Server Message ORM Test Suite ===\n\n");

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
    char query[1024];

    snprintf(query, sizeof(query),
        "DELETE FROM event_server_message WHERE server_info IN (1, 2, 99)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    EventServerMessage msg;
    EventServerMessage retrieved;
    int ret;

    printf("[TEST] Basic CRUD Operations\n");

    /* Test 1: Add */
    memset(&msg, 0, sizeof(EventServerMessage));
    msg.server_info = 1;
    msg.channel_no = 5;
    msg.kind = 'A';
    msg.message_index = '1';
    strncpy(msg.charac_name, "TestCharacter", sizeof(msg.charac_name) - 1);
    strncpy(msg.message, "This is a test message from server", sizeof(msg.message) - 1);
    msg.update_time = (unsigned int)time(NULL);

    ret = EventServerMessage_Add(manager, &msg);
    printf("  Add message: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test 2: Exists */
    ret = EventServerMessage_Exists(manager, 1, 5, '1');
    printf("  Exists check: %s\n", ret == 1 ? "PASS" : "FAIL");

    /* Test 3: Get */
    memset(&retrieved, 0, sizeof(EventServerMessage));
    ret = EventServerMessage_Get(manager, 1, 5, '1', &retrieved);
    printf("  Get message: %s\n", ret == 0 ? "PASS" : "FAIL");

    if (ret == 0) {
        printf("    Retrieved: server=%u, channel=%u, kind=%c, index=%c\n",
               retrieved.server_info, retrieved.channel_no,
               retrieved.kind, retrieved.message_index);
        printf("    Character: %s\n", retrieved.charac_name);
        printf("    Message: %s\n", retrieved.message);
    }

    /* Test 4: Update */
    strncpy(retrieved.message, "Updated test message", sizeof(retrieved.message) - 1);
    retrieved.update_time = (unsigned int)time(NULL);
    ret = EventServerMessage_Update(manager, &retrieved);
    printf("  Update message: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test 5: Verify update */
    memset(&retrieved, 0, sizeof(EventServerMessage));
    ret = EventServerMessage_Get(manager, 1, 5, '1', &retrieved);
    if (ret == 0) {
        int match = (strncmp(retrieved.message, "Updated test message", 20) == 0);
        printf("  Verify update: %s\n", match ? "PASS" : "FAIL");
    }

    /* Test 6: Delete */
    ret = EventServerMessage_Delete(manager, 1, 5, '1');
    printf("  Delete message: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test 7: Verify deletion */
    ret = EventServerMessage_Exists(manager, 1, 5, '1');
    printf("  Verify deletion: %s\n", ret == 0 ? "PASS" : "FAIL");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    EventServerMessage messages[10];
    int count;
    int ret;
    int i;

    printf("[TEST] Business Operations\n");

    /* Add multiple messages for testing */
    for (i = 0; i < 3; i++) {
        EventServerMessage msg;
        memset(&msg, 0, sizeof(EventServerMessage));
        msg.server_info = 2;
        msg.channel_no = (unsigned char)(i + 1);
        msg.kind = 'B';
        msg.message_index = (char)('1' + i);
        snprintf(msg.charac_name, sizeof(msg.charac_name), "Character%d", i + 1);
        snprintf(msg.message, sizeof(msg.message), "Message %d from server 2", i + 1);
        msg.update_time = (unsigned int)time(NULL);
        EventServerMessage_Add(manager, &msg);
    }

    /* Test 1: GetByServer */
    memset(messages, 0, sizeof(messages));
    ret = EventServerMessage_GetByServer(manager, 2, messages, 10, &count);
    printf("  GetByServer: %s (found %d messages)\n",
           ret == 0 && count == 3 ? "PASS" : "FAIL", count);

    /* Test 2: GetByChannel */
    memset(messages, 0, sizeof(messages));
    ret = EventServerMessage_GetByChannel(manager, 2, 1, messages, 10, &count);
    printf("  GetByChannel: %s (found %d messages)\n",
           ret == 0 && count == 1 ? "PASS" : "FAIL", count);

    if (count > 0) {
        printf("    First message: %s\n", messages[0].message);
    }

    /* Test 3: GetAll */
    memset(messages, 0, sizeof(messages));
    ret = EventServerMessage_GetAll(manager, messages, 10, &count);
    printf("  GetAll: %s (found %d messages)\n",
           ret == 0 && count >= 3 ? "PASS" : "FAIL", count);

    /* Test 4: DeleteAll (cleanup) */
    ret = EventServerMessage_DeleteAll(manager);
    printf("  DeleteAll: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test 5: Verify all deleted */
    memset(messages, 0, sizeof(messages));
    ret = EventServerMessage_GetAll(manager, messages, 10, &count);
    printf("  Verify all deleted: %s (count=%d)\n",
           ret == 0 && count == 0 ? "PASS" : "FAIL", count);

    printf("\n");
}
