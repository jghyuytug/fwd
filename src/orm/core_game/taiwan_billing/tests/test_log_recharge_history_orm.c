#include "log_recharge_history_orm.h"
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

    if (DBConnectionManager_Connect(&manager, DB_TYPE_BILLING) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Log Recharge History ORM Test Suite ===\n\n");

    cleanup_test_data(&manager);
    test_basic_crud(&manager);
    test_business_operations(&manager);
    cleanup_test_data(&manager);

    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM log_recharge_history WHERE tran_id IN (100001, 200001, 200002, 200003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    LogRechargeHistory log;
    int ret;

    printf("[Test 1] Basic CRUD Operations\n");
    printf("--------------------------------\n");

    printf("1.1 Adding recharge log (tran_id=100001)... ");
    memset(&log, 0, sizeof(LogRechargeHistory));
    log.tran_id = 100001;
    strcpy(log.order_tran_id, "ORD2025111800001");
    log.tran_state = 1;
    strcpy(log.account_id, "testaccount001");
    strcpy(log.charac_id, "testchar001");
    log.cera = 10000;
    log.befor_cera = 5000;
    log.after_cera = 15000;
    log.charge_type = 1;
    strcpy(log.query_user, "admin");
    strcpy(log.occ_date, "2025-11-18 10:00:00");

    ret = LogRechargeHistory_Add(manager, &log);
    printf("%s\n", (ret == 0) ? "OK" : "FAILED");

    printf("1.2 Checking if log exists... ");
    ret = LogRechargeHistory_Exists(manager, 100001);
    printf("%s\n", (ret == 1) ? "OK (exists)" : "FAILED");

    printf("1.3 Getting log (tran_id=100001)... ");
    memset(&log, 0, sizeof(LogRechargeHistory));
    ret = LogRechargeHistory_Get(manager, 100001, &log);
    if (ret == 0 && log.tran_id == 100001) {
        printf("OK\n");
        printf("    Recharge: %s, account=%s, cera=%u->%u\n",
               log.order_tran_id, log.account_id, log.befor_cera, log.after_cera);
    } else {
        printf("FAILED\n");
    }

    printf("1.4 Updating log... ");
    log.tran_state = 2;
    log.cera = 12000;
    ret = LogRechargeHistory_Update(manager, &log);
    printf("%s\n", (ret == 0) ? "OK" : "FAILED");

    printf("1.5 Verifying update... ");
    memset(&log, 0, sizeof(LogRechargeHistory));
    ret = LogRechargeHistory_Get(manager, 100001, &log);
    printf("%s\n", (ret == 0 && log.tran_state == 2 && log.cera == 12000) ?
           "OK (state=2, cera=12000)" : "FAILED");

    printf("1.6 Deleting log... ");
    ret = LogRechargeHistory_Delete(manager, 100001);
    printf("%s\n", (ret == 0) ? "OK" : "FAILED");

    printf("1.7 Verifying deletion... ");
    ret = LogRechargeHistory_Exists(manager, 100001);
    printf("%s\n\n", (ret == 0) ? "OK (not exists)" : "FAILED");
}

void test_business_operations(DBConnectionManager* manager) {
    LogRechargeHistory logs[10];
    int actual_count = 0;
    int ret;

    printf("[Test 2] Business Operations\n");
    printf("-----------------------------\n");

    printf("2.1 Adding multiple recharge logs... ");
    for (int i = 0; i < 3; i++) {
        LogRechargeHistory log;
        memset(&log, 0, sizeof(LogRechargeHistory));
        log.tran_id = 200001 + i;
        snprintf(log.order_tran_id, sizeof(log.order_tran_id), "ORD202511180000%d", i + 2);
        log.tran_state = 1;
        snprintf(log.account_id, sizeof(log.account_id), "account%03d", i + 1);
        snprintf(log.charac_id, sizeof(log.charac_id), "char%03d", i + 1);
        log.cera = 5000 * (i + 1);
        log.befor_cera = 10000;
        log.after_cera = 10000 + log.cera;
        log.charge_type = 1;
        strcpy(log.query_user, "system");
        snprintf(log.occ_date, sizeof(log.occ_date), "2025-11-18 1%d:00:00", i);
        LogRechargeHistory_Add(manager, &log);
    }
    printf("OK (added 3 logs)\n");

    printf("2.2 Getting all logs... ");
    memset(logs, 0, sizeof(logs));
    ret = LogRechargeHistory_GetAll(manager, logs, 10, &actual_count);
    if (ret == 0) {
        printf("OK (found %d logs)\n", actual_count);
        for (int i = 0; i < actual_count && i < 3; i++) {
            printf("    [%d] tran_id=%llu, account=%s, cera=%u\n",
                   i + 1, logs[i].tran_id, logs[i].account_id, logs[i].cera);
        }
    } else {
        printf("FAILED\n");
    }

    printf("2.3 Getting by account (account001)... ");
    memset(logs, 0, sizeof(logs));
    ret = LogRechargeHistory_GetByAccount(manager, "account001", logs, 10, &actual_count);
    printf("%s\n", (ret == 0) ? "OK (found " : "FAILED");
    if (ret == 0) printf("%d logs)", actual_count);

    printf("2.4 Getting by character (char001)... ");
    memset(logs, 0, sizeof(logs));
    ret = LogRechargeHistory_GetByCharac(manager, "char001", logs, 10, &actual_count);
    printf("%s\n", (ret == 0) ? "OK (found " : "FAILED");
    if (ret == 0) printf("%d logs)", actual_count);

    printf("2.5 Getting by date range... ");
    memset(logs, 0, sizeof(logs));
    ret = LogRechargeHistory_GetByDateRange(manager, "2025-11-18 00:00:00",
                                             "2025-11-18 23:59:59", logs, 10, &actual_count);
    printf("%s\n\n", (ret == 0) ? "OK (found " : "FAILED");
    if (ret == 0) printf("%d logs)", actual_count);
}
