#include "guild_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test counters */
static int test_passed = 0;
static int test_failed = 0;

/* Test data cleanup */
void cleanup_test_data(DBConnectionManager* manager);

/* Test functions */
void test_add_stat(DBConnectionManager* manager);
void test_get_stat(DBConnectionManager* manager);
void test_update_stat(DBConnectionManager* manager);
void test_exists_stat(DBConnectionManager* manager);
void test_get_by_date(DBConnectionManager* manager);
void test_get_by_date_and_level(DBConnectionManager* manager);
void test_get_by_server(DBConnectionManager* manager);
void test_delete_stat(DBConnectionManager* manager);
void test_delete_by_date(DBConnectionManager* manager);

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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_GUILD) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Guild Stat ORM Test Suite ===\n\n");

    /* Clean up before tests */
    cleanup_test_data(&manager);

    /* Run tests */
    test_add_stat(&manager);
    test_get_stat(&manager);
    test_exists_stat(&manager);
    test_update_stat(&manager);
    test_get_by_date(&manager);
    test_get_by_date_and_level(&manager);
    test_get_by_server(&manager);
    test_delete_stat(&manager);
    test_delete_by_date(&manager);

    /* Clean up after tests */
    cleanup_test_data(&manager);

    /* Print summary */
    printf("\n=== Test Summary ===\n");
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("Total:  %d\n", test_passed + test_failed);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);

    return (test_failed == 0) ? 0 : 1;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM guild_stat WHERE occ_date IN ('2025-01-01', '2025-01-02')");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
}

void test_add_stat(DBConnectionManager* manager) {
    GuildStat stat1, stat2;

    printf("[TEST] Add guild statistics\n");

    memset(&stat1, 0, sizeof(GuildStat));
    strcpy(stat1.occ_date, "2025-01-01");
    stat1.lev = 5;
    stat1.server_id = 1;
    stat1.create_no = 10;
    stat1.acc_create_no = 100;
    stat1.member_no = 50;
    stat1.acc_member_no = 500;
    stat1.avg_lev = 25.5f;
    stat1.avg_master_lev = 30.2f;
    stat1.expire_no = 2;
    stat1.new_account_no = 15;
    stat1.new_member_no = 20;
    stat1.acc_account_no = 120;

    memset(&stat2, 0, sizeof(GuildStat));
    strcpy(stat2.occ_date, "2025-01-01");
    stat2.lev = 10;
    stat2.server_id = 1;
    stat2.create_no = 5;
    stat2.acc_create_no = 105;
    stat2.member_no = 30;
    stat2.acc_member_no = 530;
    stat2.avg_lev = 35.8f;
    stat2.avg_master_lev = 40.5f;
    stat2.expire_no = 1;
    stat2.new_account_no = 8;
    stat2.new_member_no = 10;
    stat2.acc_account_no = 128;

    if (GuildStat_Add(manager, &stat1) == 0 && GuildStat_Add(manager, &stat2) == 0) {
        printf("  PASS: Guild statistics added\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to add guild statistics\n");
        test_failed++;
    }
}

void test_get_stat(DBConnectionManager* manager) {
    GuildStat stat;

    printf("[TEST] Get guild statistics\n");

    if (GuildStat_Get(manager, "2025-01-01", 5, 1, &stat) == 0) {
        printf("  PASS: Retrieved stat (date=%s, lev=%u, server=%u)\n",
               stat.occ_date, stat.lev, stat.server_id);
        printf("  Create No: %d, Member No: %d\n", stat.create_no, stat.member_no);
        test_passed++;
    } else {
        printf("  FAIL: Failed to retrieve stat\n");
        test_failed++;
    }
}

void test_exists_stat(DBConnectionManager* manager) {
    printf("[TEST] Check stat exists\n");

    if (GuildStat_Exists(manager, "2025-01-01", 5, 1)) {
        printf("  PASS: Stat exists\n");
        test_passed++;
    } else {
        printf("  FAIL: Stat should exist\n");
        test_failed++;
    }
}

void test_update_stat(DBConnectionManager* manager) {
    GuildStat stat;

    printf("[TEST] Update guild statistics\n");

    if (GuildStat_Get(manager, "2025-01-01", 5, 1, &stat) == 0) {
        stat.create_no = 15;
        stat.member_no = 60;
        stat.avg_lev = 28.5f;

        if (GuildStat_Update(manager, &stat) == 0) {
            printf("  PASS: Stat updated\n");
            test_passed++;
        } else {
            printf("  FAIL: Failed to update stat\n");
            test_failed++;
        }
    } else {
        printf("  FAIL: Failed to get stat for update\n");
        test_failed++;
    }
}

void test_get_by_date(DBConnectionManager* manager) {
    GuildStat stats[10];
    int count = 0;

    printf("[TEST] Get stats by date\n");

    if (GuildStat_GetByDate(manager, "2025-01-01", stats, 10, &count) == 0) {
        printf("  PASS: Retrieved %d stats for date 2025-01-01\n", count);
        for (int i = 0; i < count; i++) {
            printf("    Stat %d: lev=%u, server=%u, create=%d\n",
                   i + 1, stats[i].lev, stats[i].server_id, stats[i].create_no);
        }
        test_passed++;
    } else {
        printf("  FAIL: Failed to get stats by date\n");
        test_failed++;
    }
}

void test_get_by_date_and_level(DBConnectionManager* manager) {
    GuildStat stats[10];
    int count = 0;

    printf("[TEST] Get stats by date and level\n");

    if (GuildStat_GetByDateAndLevel(manager, "2025-01-01", 5, stats, 10, &count) == 0) {
        printf("  PASS: Retrieved %d stats for date 2025-01-01, level 5\n", count);
        test_passed++;
    } else {
        printf("  FAIL: Failed to get stats by date and level\n");
        test_failed++;
    }
}

void test_get_by_server(DBConnectionManager* manager) {
    GuildStat stats[10];
    int count = 0;

    printf("[TEST] Get stats by server\n");

    /* Add stat for another date to test */
    GuildStat stat3;
    memset(&stat3, 0, sizeof(GuildStat));
    strcpy(stat3.occ_date, "2025-01-02");
    stat3.lev = 5;
    stat3.server_id = 1;
    stat3.create_no = 12;
    stat3.acc_create_no = 112;
    stat3.member_no = 55;
    stat3.acc_member_no = 555;
    stat3.avg_lev = 26.0f;
    stat3.avg_master_lev = 31.0f;
    stat3.expire_no = 3;
    stat3.new_account_no = 18;
    stat3.new_member_no = 22;
    stat3.acc_account_no = 130;
    GuildStat_Add(manager, &stat3);

    if (GuildStat_GetByServer(manager, 1, stats, 10, &count) == 0) {
        printf("  PASS: Retrieved %d stats for server 1\n", count);
        test_passed++;
    } else {
        printf("  FAIL: Failed to get stats by server\n");
        test_failed++;
    }
}

void test_delete_stat(DBConnectionManager* manager) {
    printf("[TEST] Delete single stat\n");

    if (GuildStat_Delete(manager, "2025-01-01", 10, 1) == 0) {
        printf("  PASS: Stat deleted\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to delete stat\n");
        test_failed++;
    }
}

void test_delete_by_date(DBConnectionManager* manager) {
    int count = 0;

    printf("[TEST] Delete stats by date\n");

    if (GuildStat_DeleteByDate(manager, "2025-01-02") == 0) {
        GuildStat_GetByDate(manager, "2025-01-02", NULL, 0, &count);
        printf("  PASS: Deleted all stats for date 2025-01-02 (remaining: %d)\n", count);
        test_passed++;
    } else {
        printf("  FAIL: Failed to delete stats by date\n");
        test_failed++;
    }
}
