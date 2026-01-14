#include "member_punish_info_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test counters */
static int test_passed = 0;
static int test_failed = 0;

/* Test data cleanup */
void cleanup_test_data(DBConnectionManager* manager);

/* Test functions */
void test_add_record(DBConnectionManager* manager);
void test_get_record(DBConnectionManager* manager);
void test_update_record(DBConnectionManager* manager);
void test_exists_record(DBConnectionManager* manager);
void test_get_by_member_id(DBConnectionManager* manager);
void test_get_by_punish_type(DBConnectionManager* manager);
void test_get_by_admin(DBConnectionManager* manager);
void test_get_by_time_range(DBConnectionManager* manager);
void test_delete_record(DBConnectionManager* manager);

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

    printf("=== Member Punish Info History ORM Test Suite ===\n\n");

    /* Clean up before tests */
    cleanup_test_data(&manager);

    /* Run tests */
    test_add_record(&manager);
    test_get_record(&manager);
    test_exists_record(&manager);
    test_update_record(&manager);
    test_get_by_member_id(&manager);
    test_get_by_punish_type(&manager);
    test_get_by_admin(&manager);
    test_get_by_time_range(&manager);
    test_delete_record(&manager);

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
        "DELETE FROM member_punish_info_history WHERE m_id IN (100001, 100002)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_add_record(DBConnectionManager* manager) {
    MemberPunishInfoHistory record;

    printf("[TEST] Add punish record\n");

    memset(&record, 0, sizeof(MemberPunishInfoHistory));
    record.m_id = 100001;
    record.punish_type = 1;
    strcpy(record.occ_time, "2025-11-17 10:00:00");
    record.punish_value = 7;
    record.apply_flag = 1;
    strcpy(record.start_time, "2025-11-17 10:00:00");
    strcpy(record.end_time, "2025-11-24 10:00:00");
    strcpy(record.admin_id, "admin001");
    strcpy(record.reason, "Violation of game rules");
    record.is_kicked = 0;
    strcpy(record.first_ssn, "900101");
    strcpy(record.second_ssn, "1234567");

    if (MemberPunishInfoHistory_Add(manager, &record) == 0) {
        printf("  PASS: Punish record added\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to add punish record\n");
        test_failed++;
    }
}

void test_get_record(DBConnectionManager* manager) {
    MemberPunishInfoHistory records[10];
    int count = 0;

    printf("[TEST] Get punish record\n");

    /* Use GetByMemberId to get the record first */
    if (MemberPunishInfoHistory_GetByMemberId(manager, 100001, records, 10, &count) == 0 && count > 0) {
        MemberPunishInfoHistory record;
        if (MemberPunishInfoHistory_Get(manager, records[0].no, &record) == 0) {
            printf("  PASS: Retrieved record (no=%u, m_id=%d, type=%d)\n",
                   record.no, record.m_id, record.punish_type);
            test_passed++;
        } else {
            printf("  FAIL: Failed to retrieve record by no\n");
            test_failed++;
        }
    } else {
        printf("  FAIL: Failed to get record for test\n");
        test_failed++;
    }
}

void test_exists_record(DBConnectionManager* manager) {
    MemberPunishInfoHistory records[10];
    int count = 0;

    printf("[TEST] Check record exists\n");

    if (MemberPunishInfoHistory_GetByMemberId(manager, 100001, records, 10, &count) == 0 && count > 0) {
        if (MemberPunishInfoHistory_Exists(manager, records[0].no)) {
            printf("  PASS: Record exists (no=%u)\n", records[0].no);
            test_passed++;
        } else {
            printf("  FAIL: Record should exist\n");
            test_failed++;
        }
    } else {
        printf("  FAIL: Failed to get record for exists test\n");
        test_failed++;
    }
}

void test_update_record(DBConnectionManager* manager) {
    MemberPunishInfoHistory records[10];
    int count = 0;

    printf("[TEST] Update punish record\n");

    if (MemberPunishInfoHistory_GetByMemberId(manager, 100001, records, 10, &count) == 0 && count > 0) {
        records[0].punish_value = 14;
        strcpy(records[0].reason, "Repeated violation");

        if (MemberPunishInfoHistory_Update(manager, &records[0]) == 0) {
            printf("  PASS: Record updated (no=%u)\n", records[0].no);
            test_passed++;
        } else {
            printf("  FAIL: Failed to update record\n");
            test_failed++;
        }
    } else {
        printf("  FAIL: Failed to get record for update\n");
        test_failed++;
    }
}

void test_get_by_member_id(DBConnectionManager* manager) {
    MemberPunishInfoHistory records[10];
    int count = 0;

    printf("[TEST] Get records by member ID\n");

    /* Add another record */
    MemberPunishInfoHistory record2;
    memset(&record2, 0, sizeof(MemberPunishInfoHistory));
    record2.m_id = 100001;
    record2.punish_type = 2;
    strcpy(record2.occ_time, "2025-11-18 10:00:00");
    record2.punish_value = 30;
    record2.apply_flag = 1;
    strcpy(record2.start_time, "2025-11-18 10:00:00");
    strcpy(record2.end_time, "2025-12-18 10:00:00");
    strcpy(record2.admin_id, "admin001");
    strcpy(record2.reason, "Severe violation");
    record2.is_kicked = 1;
    MemberPunishInfoHistory_Add(manager, &record2);

    if (MemberPunishInfoHistory_GetByMemberId(manager, 100001, records, 10, &count) == 0) {
        printf("  PASS: Retrieved %d records for m_id=100001\n", count);
        test_passed++;
    } else {
        printf("  FAIL: Failed to get records by member ID\n");
        test_failed++;
    }
}

void test_get_by_punish_type(DBConnectionManager* manager) {
    MemberPunishInfoHistory records[10];
    int count = 0;

    printf("[TEST] Get records by punish type\n");

    if (MemberPunishInfoHistory_GetByPunishType(manager, 1, records, 10, &count) == 0) {
        printf("  PASS: Retrieved %d records for type=1\n", count);
        test_passed++;
    } else {
        printf("  FAIL: Failed to get records by punish type\n");
        test_failed++;
    }
}

void test_get_by_admin(DBConnectionManager* manager) {
    MemberPunishInfoHistory records[10];
    int count = 0;

    printf("[TEST] Get records by admin\n");

    if (MemberPunishInfoHistory_GetByAdmin(manager, "admin001", records, 10, &count) == 0) {
        printf("  PASS: Retrieved %d records for admin=admin001\n", count);
        test_passed++;
    } else {
        printf("  FAIL: Failed to get records by admin\n");
        test_failed++;
    }
}

void test_get_by_time_range(DBConnectionManager* manager) {
    MemberPunishInfoHistory records[10];
    int count = 0;

    printf("[TEST] Get records by time range\n");

    if (MemberPunishInfoHistory_GetByTimeRange(manager, "2025-11-17 00:00:00",
                                                 "2025-11-19 00:00:00", records, 10, &count) == 0) {
        printf("  PASS: Retrieved %d records in time range\n", count);
        test_passed++;
    } else {
        printf("  FAIL: Failed to get records by time range\n");
        test_failed++;
    }
}

void test_delete_record(DBConnectionManager* manager) {
    int count = 0;

    printf("[TEST] Delete records by member ID\n");

    if (MemberPunishInfoHistory_DeleteByMemberId(manager, 100001) == 0) {
        MemberPunishInfoHistory_GetByMemberId(manager, 100001, NULL, 0, &count);
        printf("  PASS: Deleted records for m_id=100001 (remaining: %d)\n", count);
        test_passed++;
    } else {
        printf("  FAIL: Failed to delete records\n");
        test_failed++;
    }
}
