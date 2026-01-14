#include "ip_monitor_punish_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    if (DBConnectionManager_Connect(&manager, DB_TYPE_LOGIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== IP Monitor Punish ORM Test Suite ===\n\n");

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
        "DELETE FROM ip_monitor_punish WHERE ip IN ('192.168.1.100', '192.168.1.101', '10.0.0.1')");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    IpMonitorPunish punish;
    IpMonitorPunish retrieved;
    int ret;

    printf("[TEST] Basic CRUD Operations\n");

    /* Test 1: Add */
    memset(&punish, 0, sizeof(IpMonitorPunish));
    strncpy(punish.ip, "192.168.1.100", sizeof(punish.ip) - 1);
    punish.type = 1;
    punish.m_id_cnt = 5;
    strncpy(punish.start_time, "2025-11-19 10:00:00", sizeof(punish.start_time) - 1);
    strncpy(punish.end_time, "2025-11-20 10:00:00", sizeof(punish.end_time) - 1);

    ret = IpMonitorPunish_Add(manager, &punish);
    printf("  Add record (ip=192.168.1.100, type=1): %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test 2: Exists */
    ret = IpMonitorPunish_Exists(manager, "192.168.1.100", 1);
    printf("  Exists check: %s\n", ret == 1 ? "PASS" : "FAIL");

    /* Test 3: Get */
    memset(&retrieved, 0, sizeof(IpMonitorPunish));
    ret = IpMonitorPunish_Get(manager, "192.168.1.100", 1, &retrieved);
    printf("  Get record: %s\n", ret == 0 ? "PASS" : "FAIL");

    if (ret == 0) {
        int match = (strcmp(retrieved.ip, "192.168.1.100") == 0 &&
                     retrieved.type == 1 && retrieved.m_id_cnt == 5);
        printf("  Verify data: %s (ip=%s, type=%d, cnt=%u)\n",
               match ? "PASS" : "FAIL", retrieved.ip, retrieved.type, retrieved.m_id_cnt);
        printf("    start_time: %s\n", retrieved.start_time);
        printf("    end_time: %s\n", retrieved.end_time);
    }

    /* Test 4: Update */
    retrieved.m_id_cnt = 10;
    strncpy(retrieved.end_time, "2025-11-21 10:00:00", sizeof(retrieved.end_time) - 1);
    ret = IpMonitorPunish_Update(manager, &retrieved);
    printf("  Update record: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test 5: Verify update */
    memset(&retrieved, 0, sizeof(IpMonitorPunish));
    ret = IpMonitorPunish_Get(manager, "192.168.1.100", 1, &retrieved);
    if (ret == 0) {
        int match = (retrieved.m_id_cnt == 10 &&
                     strcmp(retrieved.end_time, "2025-11-21 10:00:00") == 0);
        printf("  Verify update: %s (cnt=%u, end_time=%s)\n",
               match ? "PASS" : "FAIL", retrieved.m_id_cnt, retrieved.end_time);
    }

    /* Test 6: Delete */
    ret = IpMonitorPunish_Delete(manager, "192.168.1.100", 1);
    printf("  Delete record: %s\n", ret == 0 ? "PASS" : "FAIL");

    /* Test 7: Verify deletion */
    ret = IpMonitorPunish_Exists(manager, "192.168.1.100", 1);
    printf("  Verify deletion: %s\n", ret == 0 ? "PASS" : "FAIL");

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    IpMonitorPunish punishes[10];
    int count;
    int ret;
    int i;

    printf("[TEST] Business Operations\n");

    /* Add multiple records */
    for (i = 0; i < 3; i++) {
        IpMonitorPunish p;
        memset(&p, 0, sizeof(IpMonitorPunish));
        snprintf(p.ip, sizeof(p.ip), "192.168.1.%d", 100 + i);
        p.type = (signed char)(i % 2);
        p.m_id_cnt = (unsigned short)(5 + i);
        strncpy(p.start_time, "2025-11-19 10:00:00", sizeof(p.start_time) - 1);
        strncpy(p.end_time, "2025-11-20 10:00:00", sizeof(p.end_time) - 1);
        IpMonitorPunish_Add(manager, &p);
    }

    /* Test 1: GetAll */
    memset(punishes, 0, sizeof(punishes));
    ret = IpMonitorPunish_GetAll(manager, punishes, 10, &count);
    printf("  GetAll: %s (found %d records)\n",
           ret == 0 && count == 3 ? "PASS" : "FAIL", count);

    /* Test 2: GetByIp */
    memset(punishes, 0, sizeof(punishes));
    ret = IpMonitorPunish_GetByIp(manager, "192.168.1.100", punishes, 10, &count);
    printf("  GetByIp (192.168.1.100): %s (found %d records)\n",
           ret == 0 && count == 1 ? "PASS" : "FAIL", count);

    /* Test 3: GetByType */
    memset(punishes, 0, sizeof(punishes));
    ret = IpMonitorPunish_GetByType(manager, 0, punishes, 10, &count);
    printf("  GetByType (type=0): %s (found %d records)\n",
           ret == 0 && count >= 1 ? "PASS" : "FAIL", count);

    /* Test 4: DeleteExpired */
    ret = IpMonitorPunish_DeleteExpired(manager, "2025-11-19 09:00:00");
    printf("  DeleteExpired (before 2025-11-19 09:00:00): %s\n",
           ret == 0 ? "PASS" : "FAIL");

    /* Test 5: Verify after delete expired */
    memset(punishes, 0, sizeof(punishes));
    ret = IpMonitorPunish_GetAll(manager, punishes, 10, &count);
    printf("  Verify expired deletion: %s (remaining %d records)\n",
           ret == 0 && count == 3 ? "PASS" : "FAIL", count);

    /* Test 6: PrintInfo */
    if (count > 0) {
        printf("  PrintInfo test:\n");
        IpMonitorPunish_PrintInfo(&punishes[0]);
    }

    printf("\n");
}
