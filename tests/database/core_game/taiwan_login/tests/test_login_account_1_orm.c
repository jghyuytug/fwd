#include "login_account_1_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int test_total = 0;
static int test_passed = 0;

void cleanup_test_data(DBConnectionManager* manager);
void test_basic_crud(DBConnectionManager* manager);
void test_business_operations(DBConnectionManager* manager);

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

    if (DBConnectionManager_Connect(&manager, DB_TYPE_LOGIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Login Account 1 ORM Test Suite ===\n\n");

    cleanup_test_data(&manager);
    test_basic_crud(&manager);
    test_business_operations(&manager);
    cleanup_test_data(&manager);

    DBConnectionManager_Cleanup(&manager);
    printf("\n=== Test Results: %d/%d passed ===\n", test_passed, test_total);
    return (test_passed == test_total) ? 0 : 1;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM login_account_1 WHERE m_id IN (100001, 100002, 100003)");
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    LoginAccount1 record;
    int exists;

    printf("Test 1: Add login account\n");
    test_total++;
    memset(&record, 0, sizeof(LoginAccount1));
    record.m_id = 100001;
    record.m_channel_no = 1;
    record.login_status = 1;
    strcpy(record.last_login_date, "2025-11-19 16:00:00");
    strcpy(record.login_ip, "192.168.1.100");

    if (LoginAccount1_Add(manager, &record) == 0) {
        printf("  PASS: Record added successfully\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to add record\n");
    }

    printf("Test 2: Check if record exists\n");
    test_total++;
    exists = LoginAccount1_Exists(manager, 100001);
    if (exists) {
        printf("  PASS: Record exists\n");
        test_passed++;
    } else {
        printf("  FAIL: Record not found\n");
    }

    printf("Test 3: Get login account\n");
    test_total++;
    memset(&record, 0, sizeof(LoginAccount1));
    if (LoginAccount1_Get(manager, 100001, &record) == 0) {
        printf("  PASS: Record retrieved successfully\n");
        printf("    m_id: %u, channel: %d, status: %u, ip: %s\n",
               record.m_id, record.m_channel_no, record.login_status, record.login_ip);
        test_passed++;
    } else {
        printf("  FAIL: Failed to retrieve record\n");
    }

    printf("Test 4: Update login account\n");
    test_total++;
    record.login_status = 0;
    strcpy(record.login_ip, "192.168.1.101");
    if (LoginAccount1_Update(manager, &record) == 0) {
        memset(&record, 0, sizeof(LoginAccount1));
        LoginAccount1_Get(manager, 100001, &record);
        if (record.login_status == 0 && strcmp(record.login_ip, "192.168.1.101") == 0) {
            printf("  PASS: Record updated successfully\n");
            test_passed++;
        } else {
            printf("  FAIL: Record not updated correctly\n");
        }
    } else {
        printf("  FAIL: Failed to update record\n");
    }

    printf("Test 5: Delete login account\n");
    test_total++;
    if (LoginAccount1_Delete(manager, 100001) == 0) {
        exists = LoginAccount1_Exists(manager, 100001);
        if (!exists) {
            printf("  PASS: Record deleted successfully\n");
            test_passed++;
        } else {
            printf("  FAIL: Record still exists after deletion\n");
        }
    } else {
        printf("  FAIL: Failed to delete record\n");
    }
}

void test_business_operations(DBConnectionManager* manager) {
    LoginAccount1 record;
    LoginAccount1 records[10];
    int actual_count = 0;

    printf("Test 6: Add multiple records\n");
    test_total++;

    memset(&record, 0, sizeof(LoginAccount1));
    record.m_id = 100001;
    record.m_channel_no = 1;
    record.login_status = 1;
    strcpy(record.last_login_date, "2025-11-19 16:00:00");
    strcpy(record.login_ip, "192.168.1.100");
    LoginAccount1_Add(manager, &record);

    record.m_id = 100002;
    record.m_channel_no = 1;
    record.login_status = 1;
    strcpy(record.login_ip, "192.168.1.101");
    LoginAccount1_Add(manager, &record);

    record.m_id = 100003;
    record.m_channel_no = 2;
    record.login_status = 0;
    strcpy(record.login_ip, "192.168.1.102");
    LoginAccount1_Add(manager, &record);

    printf("  PASS: Multiple records added\n");
    test_passed++;

    printf("Test 7: Get by login status\n");
    test_total++;
    if (LoginAccount1_GetByStatus(manager, 1, records, 10, &actual_count) == 0) {
        if (actual_count >= 2) {
            printf("  PASS: Found %d records with status=1\n", actual_count);
            test_passed++;
        } else {
            printf("  FAIL: Expected at least 2 records, found %d\n", actual_count);
        }
    } else {
        printf("  FAIL: Failed to query by status\n");
    }

    printf("Test 8: Get by channel\n");
    test_total++;
    if (LoginAccount1_GetByChannel(manager, 1, records, 10, &actual_count) == 0) {
        if (actual_count >= 2) {
            printf("  PASS: Found %d records in channel 1\n", actual_count);
            test_passed++;
        } else {
            printf("  FAIL: Expected at least 2 records, found %d\n", actual_count);
        }
    } else {
        printf("  FAIL: Failed to query by channel\n");
    }

    printf("Test 9: Update login status\n");
    test_total++;
    if (LoginAccount1_UpdateLoginStatus(manager, 100001, 0) == 0) {
        memset(&record, 0, sizeof(LoginAccount1));
        LoginAccount1_Get(manager, 100001, &record);
        if (record.login_status == 0) {
            printf("  PASS: Login status updated to 0\n");
            test_passed++;
        } else {
            printf("  FAIL: Login status not updated correctly\n");
        }
    } else {
        printf("  FAIL: Failed to update login status\n");
    }

    printf("Test 10: Update last login\n");
    test_total++;
    if (LoginAccount1_UpdateLastLogin(manager, 100001, "2025-11-19 17:00:00", "192.168.1.200") == 0) {
        memset(&record, 0, sizeof(LoginAccount1));
        LoginAccount1_Get(manager, 100001, &record);
        if (strcmp(record.login_ip, "192.168.1.200") == 0) {
            printf("  PASS: Last login updated\n");
            test_passed++;
        } else {
            printf("  FAIL: Last login not updated correctly\n");
        }
    } else {
        printf("  FAIL: Failed to update last login\n");
    }
}
