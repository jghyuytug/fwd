#include "member_punish_info_history_2013_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */

void test_basic_crud(DBConnectionManager* manager);
void test_query_operations(DBConnectionManager* manager);
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

    printf("=== Member Punish Info History 2013 ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_query_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed Successfully ===\n");
    return 0;
}

void test_basic_crud(DBConnectionManager* manager) {
    MemberPunishInfoHistory2013 record;
    MemberPunishInfoHistory2013 fetched;

    printf("[Test 1] Basic CRUD Operations\n");

    /* Test Add with minimal fields */
    memset(&record, 0, sizeof(MemberPunishInfoHistory2013));
    record.m_id = 100001;
    record.punish_type = 1;
    strcpy(record.occ_time, "2013-01-15 10:30:00");
    record.punish_value = 7;
    record.apply_flag = 1;
    strcpy(record.start_time, "2013-01-15 10:30:00");
    strcpy(record.end_time, "2013-01-22 10:30:00");
    strcpy(record.admin_id, "admin001");
    strcpy(record.reason, "Illegal game behavior");
    record.is_kicked = 1;

    if (MemberPunishInfoHistory2013_Add(manager, &record) < 0) {
        printf("  [FAIL] Add record\n");
        return;
    }
    printf("  [PASS] Add record\n");

    /* Count records */
    int count = MemberPunishInfoHistory2013_CountByMId(manager, 100001);
    if (count < 1) {
        printf("  [FAIL] Count records (expected >= 1, got %d)\n", count);
        return;
    }
    printf("  [PASS] Count records (count=%d)\n", count);

    /* Get by m_id to retrieve the auto-generated 'no' */
    MemberPunishInfoHistory2013 records[10];
    int actual_count = 0;
    if (MemberPunishInfoHistory2013_GetByMId(manager, 100001, records, 10, &actual_count) < 0 || actual_count < 1) {
        printf("  [FAIL] Get record by m_id\n");
        return;
    }
    int test_no = records[0].no;
    printf("  [PASS] Get record by m_id (no=%d)\n", test_no);

    /* Test Get by primary key */
    if (MemberPunishInfoHistory2013_Get(manager, test_no, &fetched) < 0) {
        printf("  [FAIL] Get record by no\n");
        return;
    }
    if (fetched.m_id != 100001 || fetched.punish_type != 1) {
        printf("  [FAIL] Verify fetched record (m_id=%d, punish_type=%d)\n",
               fetched.m_id, fetched.punish_type);
        return;
    }
    printf("  [PASS] Get record by no and verify\n");

    /* Test Update */
    fetched.apply_flag = 0;
    strcpy(fetched.reason, "Updated reason - warning only");
    if (MemberPunishInfoHistory2013_Update(manager, &fetched) < 0) {
        printf("  [FAIL] Update record\n");
        return;
    }
    printf("  [PASS] Update record\n");

    /* Verify update */
    if (MemberPunishInfoHistory2013_Get(manager, test_no, &fetched) < 0) {
        printf("  [FAIL] Verify update - Get\n");
        return;
    }
    if (fetched.apply_flag != 0 || strcmp(fetched.reason, "Updated reason - warning only") != 0) {
        printf("  [FAIL] Verify update (apply_flag=%d, reason=%s)\n",
               fetched.apply_flag, fetched.reason);
        return;
    }
    printf("  [PASS] Verify update\n");

    /* Test Delete */
    if (MemberPunishInfoHistory2013_Delete(manager, test_no) < 0) {
        printf("  [FAIL] Delete record\n");
        return;
    }
    printf("  [PASS] Delete record\n");

    /* Verify deletion */
    if (MemberPunishInfoHistory2013_Get(manager, test_no, &fetched) == 0) {
        printf("  [FAIL] Verify deletion (record still exists)\n");
        return;
    }
    printf("  [PASS] Verify deletion\n\n");
}

void test_query_operations(DBConnectionManager* manager) {
    MemberPunishInfoHistory2013 records[10];
    int actual_count;

    printf("[Test 2] Query Operations\n");

    /* Insert test records with different punish types */
    for (int i = 0; i < 3; i++) {
        MemberPunishInfoHistory2013 rec;
        memset(&rec, 0, sizeof(MemberPunishInfoHistory2013));
        rec.m_id = 200001;
        rec.punish_type = i + 1;  /* Types: 1, 2, 3 */
        snprintf(rec.occ_time, sizeof(rec.occ_time), "2013-06-%02d 14:00:00", 10 + i);
        rec.punish_value = (i + 1) * 3;
        rec.apply_flag = (i == 2) ? 0 : 1;
        snprintf(rec.start_time, sizeof(rec.start_time), "2013-06-%02d 14:00:00", 10 + i);
        snprintf(rec.end_time, sizeof(rec.end_time), "2013-06-%02d 14:00:00", 17 + i);
        snprintf(rec.admin_id, sizeof(rec.admin_id), "admin%03d", 100 + i);
        snprintf(rec.reason, sizeof(rec.reason), "Test reason %d", i + 1);
        rec.is_kicked = (i % 2);

        if (MemberPunishInfoHistory2013_Add(manager, &rec) < 0) {
            printf("  [FAIL] Insert test record %d\n", i + 1);
            return;
        }
    }
    printf("  [PASS] Insert 3 test records\n");

    /* Test GetByMId */
    if (MemberPunishInfoHistory2013_GetByMId(manager, 200001, records, 10, &actual_count) < 0) {
        printf("  [FAIL] GetByMId\n");
        return;
    }
    if (actual_count != 3) {
        printf("  [FAIL] GetByMId count (expected 3, got %d)\n", actual_count);
        return;
    }
    printf("  [PASS] GetByMId (found %d records)\n", actual_count);

    /* Test GetByPunishType */
    if (MemberPunishInfoHistory2013_GetByPunishType(manager, 2, records, 10, &actual_count) < 0) {
        printf("  [FAIL] GetByPunishType\n");
        return;
    }
    if (actual_count != 1) {
        printf("  [FAIL] GetByPunishType count (expected 1, got %d)\n", actual_count);
        return;
    }
    printf("  [PASS] GetByPunishType (found %d record with type=2)\n", actual_count);

    /* Test GetByTimeRange - only first record (2013-06-10) */
    if (MemberPunishInfoHistory2013_GetByTimeRange(manager, "2013-06-10 00:00:00", "2013-06-10 23:59:59",
                                                     records, 10, &actual_count) < 0) {
        printf("  [FAIL] GetByTimeRange\n");
        return;
    }
    if (actual_count != 1) {
        printf("  [FAIL] GetByTimeRange count (expected 1, got %d)\n", actual_count);
        return;
    }
    printf("  [PASS] GetByTimeRange (found %d record in range)\n", actual_count);

    /* Test UpdateApplyFlag */
    if (MemberPunishInfoHistory2013_GetByMId(manager, 200001, records, 10, &actual_count) < 0) {
        printf("  [FAIL] GetByMId for update test\n");
        return;
    }
    int first_no = records[0].no;
    if (MemberPunishInfoHistory2013_UpdateApplyFlag(manager, first_no, 0) < 0) {
        printf("  [FAIL] UpdateApplyFlag\n");
        return;
    }
    printf("  [PASS] UpdateApplyFlag\n");

    /* Verify flag update */
    MemberPunishInfoHistory2013 updated;
    if (MemberPunishInfoHistory2013_Get(manager, first_no, &updated) < 0) {
        printf("  [FAIL] Verify flag update - Get\n");
        return;
    }
    if (updated.apply_flag != 0) {
        printf("  [FAIL] Verify flag update (expected 0, got %d)\n", updated.apply_flag);
        return;
    }
    printf("  [PASS] Verify flag update (apply_flag=%d)\n\n", updated.apply_flag);

    /* Print sample record details */
    printf("  Sample Record Details:\n");
    printf("    No: %d\n", updated.no);
    printf("    M_ID: %d\n", updated.m_id);
    printf("    Punish Type: %d\n", updated.punish_type);
    printf("    Occurrence Time: %s\n", updated.occ_time);
    printf("    Punish Value: %d days\n", updated.punish_value);
    printf("    Apply Flag: %d\n", updated.apply_flag);
    printf("    Admin ID: %s\n", updated.admin_id);
    printf("    Reason: %s\n\n", updated.reason);
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    /* Clean up test data */
    snprintf(query, sizeof(query),
        "DELETE FROM member_punish_info_history_2013 WHERE m_id IN (100001, 200001)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}
