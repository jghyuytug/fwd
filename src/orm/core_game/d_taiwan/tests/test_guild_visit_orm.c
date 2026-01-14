#include "guild_visit_orm.h"
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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_GUILD) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Guild Visit ORM Test Suite ===\n\n");

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
        "DELETE FROM guild_visit WHERE guild_id IN (1001, 2001, 2002, 2003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    GuildVisit visit, retrieved;
    int ret;

    printf("--- Test 1: Add Guild Visit Record ---\n");
    memset(&visit, 0, sizeof(GuildVisit));
    visit.guild_id = 1001;
    visit.server_id = 1;
    visit.total_visit = 100;
    visit.today_visit = 10;

    ret = GuildVisit_Add(manager, &visit);
    if (ret == 0) {
        printf("PASS: Added guild visit record successfully\n");
    } else {
        printf("FAIL: Failed to add guild visit record (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 2: Get Guild Visit Statistics ---\n");
    memset(&retrieved, 0, sizeof(GuildVisit));
    ret = GuildVisit_Get(manager, 1001, &retrieved);
    if (ret == 0) {
        printf("PASS: Retrieved guild visit statistics (guild_id=%d)\n", retrieved.guild_id);
        GuildVisit_PrintInfo(&retrieved);
    } else {
        printf("FAIL: Failed to retrieve guild visit statistics (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 3: Update Guild Visit Statistics ---\n");
    retrieved.total_visit = 200;
    retrieved.today_visit = 20;
    ret = GuildVisit_Update(manager, &retrieved);
    if (ret == 0) {
        printf("PASS: Updated guild visit statistics successfully\n");
    } else {
        printf("FAIL: Failed to update guild visit statistics (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 4: Verify Update ---\n");
    memset(&retrieved, 0, sizeof(GuildVisit));
    ret = GuildVisit_Get(manager, 1001, &retrieved);
    if (ret == 0 && retrieved.total_visit == 200 && retrieved.today_visit == 20) {
        printf("PASS: Update verified (total=%d, today=%d)\n",
               retrieved.total_visit, retrieved.today_visit);
    } else {
        printf("FAIL: Update verification failed\n");
    }

    printf("\n--- Test 5: Check Exists ---\n");
    ret = GuildVisit_Exists(manager, 1001);
    if (ret == 1) {
        printf("PASS: Guild visit record exists (guild_id=1001)\n");
    } else {
        printf("FAIL: Exists check failed (ret=%d)\n", ret);
    }

    printf("\n--- Test 6: Delete Guild Visit Record ---\n");
    ret = GuildVisit_Delete(manager, 1001);
    if (ret == 0) {
        printf("PASS: Deleted guild visit record successfully\n");
    } else {
        printf("FAIL: Failed to delete guild visit record (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 7: Verify Deletion ---\n");
    ret = GuildVisit_Exists(manager, 1001);
    if (ret == 0) {
        printf("PASS: Guild visit record no longer exists\n");
    } else {
        printf("FAIL: Deletion verification failed (ret=%d)\n", ret);
    }
}

void test_business_operations(DBConnectionManager* manager) {
    GuildVisit visits[10];
    int count = 0;
    int ret;

    printf("\n--- Test 8: Add Multiple Visit Records ---\n");
    for (int i = 0; i < 3; i++) {
        GuildVisit visit;
        memset(&visit, 0, sizeof(GuildVisit));
        visit.guild_id = 2001 + i;
        visit.server_id = 2;
        visit.total_visit = 1000 + (i * 100);
        visit.today_visit = 50 + (i * 10);

        ret = GuildVisit_Add(manager, &visit);
        if (ret != 0) {
            printf("FAIL: Failed to add visit record %d (ret=%d)\n", i + 1, ret);
            return;
        }
    }
    printf("PASS: Added 3 visit records for server 2\n");

    printf("\n--- Test 9: Increment Visit Counters ---\n");
    ret = GuildVisit_IncrementVisit(manager, 2001, 10, 5);
    if (ret == 0) {
        GuildVisit updated;
        GuildVisit_Get(manager, 2001, &updated);
        if (updated.total_visit == 1010 && updated.today_visit == 55) {
            printf("PASS: Visit counters incremented (total=%d, today=%d)\n",
                   updated.total_visit, updated.today_visit);
        } else {
            printf("FAIL: Visit counters not incremented correctly (total=%d, today=%d)\n",
                   updated.total_visit, updated.today_visit);
        }
    } else {
        printf("FAIL: Failed to increment visit counters (ret=%d)\n", ret);
    }

    printf("\n--- Test 10: Get Visits by Server ---\n");
    ret = GuildVisit_GetByServer(manager, 2, visits, 10, &count);
    if (ret == 0 && count == 3) {
        printf("PASS: Retrieved %d visit records for server 2\n", count);
        for (int i = 0; i < count; i++) {
            printf("  Guild %d: total=%d, today=%d\n",
                   visits[i].guild_id, visits[i].total_visit, visits[i].today_visit);
        }
    } else {
        printf("FAIL: Failed to retrieve visits by server (ret=%d, count=%d, expected=3)\n",
               ret, count);
    }

    printf("\n--- Test 11: Get Top Visited Guilds ---\n");
    ret = GuildVisit_GetTopVisited(manager, 2, visits, &count);
    if (ret == 0 && count == 2) {
        printf("PASS: Retrieved top %d visited guilds\n", count);
        for (int i = 0; i < count; i++) {
            printf("  Rank %d: guild_id=%d, total=%d\n",
                   i + 1, visits[i].guild_id, visits[i].total_visit);
        }
        /* Verify sorted by total_visit descending */
        if (visits[0].total_visit >= visits[1].total_visit) {
            printf("PASS: Results sorted correctly by total_visit\n");
        } else {
            printf("FAIL: Results not sorted correctly\n");
        }
    } else {
        printf("FAIL: Failed to get top visited guilds (ret=%d, count=%d)\n", ret, count);
    }

    printf("\n--- Test 12: Reset Today's Visits ---\n");
    ret = GuildVisit_ResetTodayVisits(manager);
    if (ret == 0) {
        printf("PASS: Reset today's visits for all guilds\n");

        /* Verify reset */
        GuildVisit check;
        GuildVisit_Get(manager, 2001, &check);
        if (check.today_visit == 0) {
            printf("PASS: Today's visit count reset verified (total=%d, today=%d)\n",
                   check.total_visit, check.today_visit);
        } else {
            printf("FAIL: Today's visit count not reset (today=%d)\n", check.today_visit);
        }
    } else {
        printf("FAIL: Failed to reset today's visits (ret=%d)\n", ret);
    }
}
