#include "log_point_history_orm.h"
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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_BILLING) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Log Point History ORM Test Suite ===\n\n");

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

    snprintf(query, sizeof(query),
        "DELETE FROM log_point_history WHERE account_id IN ('testaccount001', 'testaccount002', 'testaccount003')");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    LogPointHistory log;
    unsigned int test_no;
    int ret;

    printf("[Test 1] Basic CRUD Operations\n");
    printf("--------------------------------\n");

    /* Test 1.1: Add new point log (AUTO_INCREMENT) */
    printf("1.1 Adding point log (auto-increment)... ");
    memset(&log, 0, sizeof(LogPointHistory));
    strcpy(log.account_id, "testaccount001");
    strcpy(log.charac_id, "testchar001");
    log.cera_point = 1000;
    log.command = 'A';
    log.charge_type = 1;
    log.free_charge_type = 0;
    log.item_id = 12345;
    strcpy(log.query_user, "testuser");
    strcpy(log.reg_date, "2025-11-18 10:00:00");

    ret = LogPointHistory_Add(manager, &log);
    if (ret == 0) {
        printf("OK\n");
    } else {
        printf("FAILED\n");
    }

    /* Test 1.2: Find the inserted record (by account since we don't know 'no') */
    printf("1.2 Finding inserted log by account... ");
    LogPointHistory logs[10];
    int actual_count = 0;
    ret = LogPointHistory_GetByAccount(manager, "testaccount001", logs, 10, &actual_count);
    if (ret == 0 && actual_count > 0) {
        test_no = logs[0].no;
        printf("OK (found no=%u)\n", test_no);
    } else {
        printf("FAILED\n");
        return;
    }

    /* Test 1.3: Check if log exists */
    printf("1.3 Checking if log exists (no=%u)... ", test_no);
    ret = LogPointHistory_Exists(manager, test_no);
    if (ret == 1) {
        printf("OK (exists)\n");
    } else {
        printf("FAILED (not found)\n");
    }

    /* Test 1.4: Get log by no */
    printf("1.4 Getting log (no=%u)... ", test_no);
    memset(&log, 0, sizeof(LogPointHistory));
    ret = LogPointHistory_Get(manager, test_no, &log);
    if (ret == 0 && log.no == test_no) {
        printf("OK\n");
        printf("    Point: %s, cmd=%c, point=%u, item=%u\n",
               log.account_id, log.command, log.cera_point, log.item_id);
    } else {
        printf("FAILED\n");
    }

    /* Test 1.5: Update log */
    printf("1.5 Updating log (no=%u)... ", test_no);
    log.cera_point = 1500;
    log.command = 'U';
    ret = LogPointHistory_Update(manager, &log);
    if (ret == 0) {
        printf("OK\n");
    } else {
        printf("FAILED\n");
    }

    /* Test 1.6: Verify update */
    printf("1.6 Verifying update... ");
    memset(&log, 0, sizeof(LogPointHistory));
    ret = LogPointHistory_Get(manager, test_no, &log);
    if (ret == 0 && log.command == 'U' && log.cera_point == 1500) {
        printf("OK (cmd=%c, point=%u)\n", log.command, log.cera_point);
    } else {
        printf("FAILED\n");
    }

    /* Test 1.7: Delete log */
    printf("1.7 Deleting log (no=%u)... ", test_no);
    ret = LogPointHistory_Delete(manager, test_no);
    if (ret == 0) {
        printf("OK\n");
    } else {
        printf("FAILED\n");
    }

    /* Test 1.8: Verify deletion */
    printf("1.8 Verifying deletion... ");
    ret = LogPointHistory_Exists(manager, test_no);
    if (ret == 0) {
        printf("OK (not exists)\n");
    } else {
        printf("FAILED (still exists)\n");
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    LogPointHistory logs[10];
    int actual_count = 0;
    int ret;

    printf("[Test 2] Business Operations\n");
    printf("-----------------------------\n");

    /* Test 2.1: Add multiple logs */
    printf("2.1 Adding multiple point logs... ");
    for (int i = 0; i < 3; i++) {
        LogPointHistory log;
        memset(&log, 0, sizeof(LogPointHistory));
        snprintf(log.account_id, sizeof(log.account_id), "testaccount00%d", i + 1);
        snprintf(log.charac_id, sizeof(log.charac_id), "testchar00%d", i + 1);
        log.cera_point = 500 * (i + 1);
        log.command = (i % 2 == 0) ? 'A' : 'U';
        log.charge_type = i;
        log.free_charge_type = 0;
        log.item_id = 10000 + i;
        strcpy(log.query_user, "batchtest");
        snprintf(log.reg_date, sizeof(log.reg_date), "2025-11-18 1%d:00:00", i);
        LogPointHistory_Add(manager, &log);
    }
    printf("OK (added 3 logs)\n");

    /* Test 2.2: Get all logs */
    printf("2.2 Getting all logs... ");
    memset(logs, 0, sizeof(logs));
    ret = LogPointHistory_GetAll(manager, logs, 10, &actual_count);
    if (ret == 0) {
        printf("OK (found %d logs)\n", actual_count);
        for (int i = 0; i < actual_count && i < 3; i++) {
            printf("    [%d] no=%u, %s, cmd=%c, point=%u\n",
                   i + 1, logs[i].no, logs[i].account_id, logs[i].command, logs[i].cera_point);
        }
    } else {
        printf("FAILED\n");
    }

    /* Test 2.3: Get by account */
    printf("2.3 Getting logs by account (testaccount001)... ");
    memset(logs, 0, sizeof(logs));
    ret = LogPointHistory_GetByAccount(manager, "testaccount001", logs, 10, &actual_count);
    if (ret == 0) {
        printf("OK (found %d logs)\n", actual_count);
    } else {
        printf("FAILED\n");
    }

    /* Test 2.4: Get by command (A=Add) */
    printf("2.4 Getting logs by command (A)... ");
    memset(logs, 0, sizeof(logs));
    ret = LogPointHistory_GetByCommand(manager, 'A', logs, 10, &actual_count);
    if (ret == 0) {
        printf("OK (found %d logs)\n", actual_count);
    } else {
        printf("FAILED\n");
    }

    /* Test 2.5: Get by date range */
    printf("2.5 Getting logs by date range... ");
    memset(logs, 0, sizeof(logs));
    ret = LogPointHistory_GetByDateRange(manager, "2025-11-18 00:00:00",
                                          "2025-11-18 23:59:59", logs, 10, &actual_count);
    if (ret == 0) {
        printf("OK (found %d logs)\n", actual_count);
    } else {
        printf("FAILED\n");
    }

    printf("\n");
}
