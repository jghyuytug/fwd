#include "member_mousepass_history_201604_orm.h"
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

    printf("=== Member Mousepass History 201604 ORM Test Suite ===\n\n");

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
        "DELETE FROM member_mousepass_history_201604 WHERE m_id IN (100001, 100002)");
    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    MemberMousepassHistory201604 record;
    int exists;

    printf("Test 1: Add history record\n");
    test_total++;
    memset(&record, 0, sizeof(MemberMousepassHistory201604));
    strcpy(record.occ_time, "2016-04-15 10:00:00");
    record.m_id = 100001;
    strcpy(record.pre_mousepass, "oldpass123");
    record.modify_type = 1;
    strcpy(record.ip_info, "192.168.1.1");
    strcpy(record.port_info, "8080");

    if (MemberMousepassHistory201604_Add(manager, &record) == 0) {
        printf("  PASS: Record added successfully\n");
        test_passed++;
    } else {
        printf("  FAIL: Failed to add record\n");
    }

    printf("Test 2: Check if record exists\n");
    test_total++;
    exists = MemberMousepassHistory201604_Exists(manager, "2016-04-15 10:00:00", 100001);
    if (exists) {
        printf("  PASS: Record exists\n");
        test_passed++;
    } else {
        printf("  FAIL: Record not found\n");
    }

    printf("Test 3: Get record by composite key\n");
    test_total++;
    memset(&record, 0, sizeof(MemberMousepassHistory201604));
    if (MemberMousepassHistory201604_Get(manager, "2016-04-15 10:00:00", 100001, &record) == 0) {
        printf("  PASS: Record retrieved successfully\n");
        printf("    m_id: %d, ip: %s, port: %s\n",
               record.m_id, record.ip_info, record.port_info);
        test_passed++;
    } else {
        printf("  FAIL: Failed to retrieve record\n");
    }

    printf("Test 4: Delete record\n");
    test_total++;
    if (MemberMousepassHistory201604_Delete(manager, "2016-04-15 10:00:00", 100001) == 0) {
        exists = MemberMousepassHistory201604_Exists(manager, "2016-04-15 10:00:00", 100001);
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
    MemberMousepassHistory201604 record;
    MemberMousepassHistory201604 records[10];
    int actual_count = 0;

    printf("Test 5: Add multiple records\n");
    test_total++;

    memset(&record, 0, sizeof(MemberMousepassHistory201604));
    strcpy(record.occ_time, "2016-04-15 10:00:00");
    record.m_id = 100001;
    strcpy(record.pre_mousepass, "pass1");
    strcpy(record.ip_info, "192.168.1.1");
    strcpy(record.port_info, "8080");
    record.modify_type = 1;
    MemberMousepassHistory201604_Add(manager, &record);

    strcpy(record.occ_time, "2016-04-15 11:00:00");
    record.m_id = 100001;
    strcpy(record.pre_mousepass, "pass2");
    strcpy(record.ip_info, "192.168.1.2");
    record.modify_type = 2;
    MemberMousepassHistory201604_Add(manager, &record);

    strcpy(record.occ_time, "2016-04-15 12:00:00");
    record.m_id = 100002;
    strcpy(record.pre_mousepass, "pass3");
    strcpy(record.ip_info, "192.168.1.1");
    record.modify_type = 1;
    MemberMousepassHistory201604_Add(manager, &record);

    printf("  PASS: Multiple records added\n");
    test_passed++;

    printf("Test 6: Query by member ID\n");
    test_total++;
    if (MemberMousepassHistory201604_GetByMemberId(manager, 100001, records, 10, &actual_count) == 0) {
        if (actual_count >= 2) {
            printf("  PASS: Found %d records for m_id=100001\n", actual_count);
            test_passed++;
        } else {
            printf("  FAIL: Expected at least 2 records, found %d\n", actual_count);
        }
    } else {
        printf("  FAIL: Failed to query by member ID\n");
    }

    printf("Test 7: Query by IP\n");
    test_total++;
    if (MemberMousepassHistory201604_GetByIP(manager, "192.168.1.1", records, 10, &actual_count) == 0) {
        if (actual_count >= 2) {
            printf("  PASS: Found %d records for IP 192.168.1.1\n", actual_count);
            test_passed++;
        } else {
            printf("  FAIL: Expected at least 2 records, found %d\n", actual_count);
        }
    } else {
        printf("  FAIL: Failed to query by IP\n");
    }

    printf("Test 8: Query by time range\n");
    test_total++;
    if (MemberMousepassHistory201604_GetByTimeRange(manager,
                                                     "2016-04-15 09:00:00",
                                                     "2016-04-15 11:30:00",
                                                     records, 10, &actual_count) == 0) {
        if (actual_count >= 2) {
            printf("  PASS: Found %d records in time range\n", actual_count);
            test_passed++;
        } else {
            printf("  FAIL: Expected at least 2 records, found %d\n", actual_count);
        }
    } else {
        printf("  FAIL: Failed to query by time range\n");
    }
}
