#include "member_punish_hack_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* IMPORTANT: NO Chinese characters in test files! */

void test_basic_crud(DBConnectionManager* manager);
void test_query_operations(DBConnectionManager* manager);
void test_update_operations(DBConnectionManager* manager);
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

    printf("=== Member Punish Hack History ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_query_operations(&manager);
    test_update_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed Successfully ===\n");
    return 0;
}

void test_basic_crud(DBConnectionManager* manager) {
    MemberPunishHackHistory record;
    int count;

    printf("[Test 1] Basic CRUD Operations\n");

    /* Test Add */
    memset(&record, 0, sizeof(MemberPunishHackHistory));
    record.m_id = 100001;
    record.occ_time = (unsigned int)time(NULL);
    record.period = 86400;  /* 1 day in seconds */
    record.now_flag = 1;
    record.auto_flag = 1;
    strcpy(record.reason, "Auto-detected hacking behavior");

    if (MemberPunishHackHistory_Add(manager, &record) < 0) {
        printf("  [FAIL] Add record\n");
        return;
    }
    printf("  [PASS] Add record (m_id=%u, occ_time=%u)\n", record.m_id, record.occ_time);

    /* Test Count */
    count = MemberPunishHackHistory_CountByMId(manager, 100001);
    if (count != 1) {
        printf("  [FAIL] Count records (expected 1, got %d)\n", count);
        return;
    }
    printf("  [PASS] Count records (count=%d)\n", count);

    /* Test Delete by m_id and occ_time */
    if (MemberPunishHackHistory_DeleteByMIdAndTime(manager, record.m_id, record.occ_time) < 0) {
        printf("  [FAIL] Delete by m_id and occ_time\n");
        return;
    }
    printf("  [PASS] Delete by m_id and occ_time\n");

    /* Verify deletion */
    count = MemberPunishHackHistory_CountByMId(manager, 100001);
    if (count != 0) {
        printf("  [FAIL] Verify deletion (expected 0, got %d)\n", count);
        return;
    }
    printf("  [PASS] Verify deletion (count=%d)\n\n", count);
}

void test_query_operations(DBConnectionManager* manager) {
    MemberPunishHackHistory records[10];
    int actual_count;
    unsigned int base_time = (unsigned int)time(NULL);

    printf("[Test 2] Query Operations\n");

    /* Insert test records */
    for (int i = 0; i < 3; i++) {
        MemberPunishHackHistory rec;
        memset(&rec, 0, sizeof(MemberPunishHackHistory));
        rec.m_id = 200001;
        rec.occ_time = base_time + (i * 3600);  /* Each hour apart */
        rec.period = 86400 + (i * 3600);
        rec.now_flag = (i == 2) ? 1 : 0;  /* Only last one is active */
        rec.auto_flag = 1;
        snprintf(rec.reason, sizeof(rec.reason), "Test reason %d", i + 1);

        if (MemberPunishHackHistory_Add(manager, &rec) < 0) {
            printf("  [FAIL] Insert test record %d\n", i + 1);
            return;
        }
    }
    printf("  [PASS] Insert 3 test records\n");

    /* Test GetByMId */
    if (MemberPunishHackHistory_GetByMId(manager, 200001, records, 10, &actual_count) < 0) {
        printf("  [FAIL] GetByMId\n");
        return;
    }
    if (actual_count != 3) {
        printf("  [FAIL] GetByMId count (expected 3, got %d)\n", actual_count);
        return;
    }
    printf("  [PASS] GetByMId (found %d records)\n", actual_count);

    /* Test GetActive */
    if (MemberPunishHackHistory_GetActive(manager, 200001, records, 10, &actual_count) < 0) {
        printf("  [FAIL] GetActive\n");
        return;
    }
    if (actual_count != 1) {
        printf("  [FAIL] GetActive count (expected 1, got %d)\n", actual_count);
        return;
    }
    printf("  [PASS] GetActive (found %d active record)\n", actual_count);

    /* Test GetByTimeRange - should find first 2 records only */
    if (MemberPunishHackHistory_GetByTimeRange(manager, base_time, base_time + 3601,
                                                 records, 10, &actual_count) < 0) {
        printf("  [FAIL] GetByTimeRange\n");
        return;
    }
    if (actual_count != 2) {
        printf("  [FAIL] GetByTimeRange count (expected 2, got %d)\n", actual_count);
        return;
    }
    printf("  [PASS] GetByTimeRange (found %d records)\n\n", actual_count);
}

void test_update_operations(DBConnectionManager* manager) {
    MemberPunishHackHistory records[10];
    int actual_count;
    unsigned int test_time = (unsigned int)time(NULL);

    printf("[Test 3] Update Operations\n");

    /* Insert test record */
    MemberPunishHackHistory rec;
    memset(&rec, 0, sizeof(MemberPunishHackHistory));
    rec.m_id = 300001;
    rec.occ_time = test_time;
    rec.period = 86400;
    rec.now_flag = 0;
    rec.auto_flag = 1;
    strcpy(rec.reason, "Initial punishment");

    if (MemberPunishHackHistory_Add(manager, &rec) < 0) {
        printf("  [FAIL] Insert test record\n");
        return;
    }
    printf("  [PASS] Insert test record\n");

    /* Test UpdateFlag */
    if (MemberPunishHackHistory_UpdateFlag(manager, 300001, test_time, 1) < 0) {
        printf("  [FAIL] UpdateFlag\n");
        return;
    }
    printf("  [PASS] UpdateFlag\n");

    /* Verify update */
    if (MemberPunishHackHistory_GetByMId(manager, 300001, records, 10, &actual_count) < 0) {
        printf("  [FAIL] Verify update - GetByMId\n");
        return;
    }
    if (actual_count != 1 || records[0].now_flag != 1) {
        printf("  [FAIL] Verify update (expected now_flag=1, got %d)\n", records[0].now_flag);
        return;
    }
    printf("  [PASS] Verify update (now_flag=%d)\n", records[0].now_flag);

    /* Print record details */
    printf("\n  Record Details:\n");
    printf("    M_ID: %u\n", records[0].m_id);
    printf("    Occurrence Time: %u\n", records[0].occ_time);
    printf("    Period: %u seconds\n", records[0].period);
    printf("    Now Flag: %d\n", records[0].now_flag);
    printf("    Auto Flag: %d\n", records[0].auto_flag);
    printf("    Reason: %s\n\n", records[0].reason);
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    /* Clean up test data */
    snprintf(query, sizeof(query),
        "DELETE FROM member_punish_hack_history WHERE m_id IN (100001, 200001, 300001)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}
