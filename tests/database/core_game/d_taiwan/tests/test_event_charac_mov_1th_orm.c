#include "event_charac_mov_1th_orm.h"
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

    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize connection manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Event Character Move 1th ORM Test Suite ===\n\n");

    cleanup_test_data(&manager);

    test_basic_crud(&manager);
    test_business_operations(&manager);

    cleanup_test_data(&manager);

    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void test_basic_crud(DBConnectionManager* manager) {
    printf("--- Test 1: Basic CRUD Operations ---\n");

    EventCharacMov1th record;
    memset(&record, 0, sizeof(EventCharacMov1th));
    record.m_id = 100001;
    record.server_id = 1;
    record.charac_no = 500001;
    record.move_server_id = 2;
    record.move_charac_no = 600001;
    record.move_check = 0;
    strncpy(record.event_code, "CHARAC_MOV_2009_Q1", sizeof(record.event_code) - 1);
    strncpy(record.reg_date, "2009-03-15 10:30:00", sizeof(record.reg_date) - 1);

    printf("Adding record...\n");
    if (EventCharacMov1th_Add(manager, &record) == 0) {
        printf("✓ Add successful\n");
    } else {
        printf("✗ Add failed\n");
        return;
    }

    EventCharacMov1th retrieved;
    printf("\nRetrieving by character...\n");
    if (EventCharacMov1th_GetByCharacter(manager, 100001, 1, 500001, &retrieved) == 0) {
        printf("✓ Get successful\n");
        EventCharacMov1th_PrintInfo(&retrieved);
    } else {
        printf("✗ Get failed\n");
        return;
    }

    printf("\nChecking existence...\n");
    if (EventCharacMov1th_Exists(manager, retrieved.id)) {
        printf("✓ Record exists (ID: %u)\n", retrieved.id);
    } else {
        printf("✗ Exists check failed\n");
    }

    printf("\nUpdating move_check to 1...\n");
    if (EventCharacMov1th_UpdateMoveCheck(manager, retrieved.id, 1) == 0) {
        printf("✓ Update successful\n");

        EventCharacMov1th updated;
        if (EventCharacMov1th_Get(manager, retrieved.id, &updated) == 0) {
            printf("  Move Check: %d -> %d\n", retrieved.move_check, updated.move_check);
        }
    } else {
        printf("✗ Update failed\n");
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    printf("--- Test 2: Business Operations ---\n");

    EventCharacMov1th records[5];
    memset(records, 0, sizeof(records));

    for (int i = 0; i < 3; i++) {
        records[i].m_id = 200001;
        records[i].server_id = 1;
        records[i].charac_no = 700001 + i;
        records[i].move_server_id = 2;
        records[i].move_charac_no = 800001 + i;
        records[i].move_check = (i % 2);
        strncpy(records[i].event_code, "CHARAC_MOV_2009_Q2", sizeof(records[i].event_code) - 1);
        strncpy(records[i].reg_date, "2009-06-20 14:00:00", sizeof(records[i].reg_date) - 1);

        EventCharacMov1th_Add(manager, &records[i]);
    }
    printf("Added 3 records for member 200001\n");

    printf("\nGetting all moves for member 200001...\n");
    EventCharacMov1th member_records[10];
    int count = 0;
    if (EventCharacMov1th_GetByMember(manager, 200001, member_records, 10, &count) == 0) {
        printf("✓ Found %d records\n", count);
        for (int i = 0; i < count; i++) {
            printf("  [%d] Char %d -> %d (check: %d)\n",
                   i + 1, member_records[i].charac_no,
                   member_records[i].move_charac_no,
                   member_records[i].move_check);
        }
    } else {
        printf("✗ Query failed\n");
    }

    printf("\nCounting moves for member 200001...\n");
    int total_count = 0;
    if (EventCharacMov1th_CountByMember(manager, 200001, &total_count) == 0) {
        printf("✓ Total count: %d\n", total_count);
    } else {
        printf("✗ Count failed\n");
    }

    printf("\nGetting pending moves (move_check = 0)...\n");
    EventCharacMov1th pending[10];
    int pending_count = 0;
    if (EventCharacMov1th_GetPendingMoves(manager, pending, 10, &pending_count) == 0) {
        printf("✓ Found %d pending moves\n", pending_count);
        for (int i = 0; i < pending_count; i++) {
            printf("  [%d] ID=%u, m_id=%d, char=%d\n",
                   i + 1, pending[i].id, pending[i].m_id, pending[i].charac_no);
        }
    } else {
        printf("✗ Query failed\n");
    }

    printf("\nGetting by event code 'CHARAC_MOV_2009_Q2'...\n");
    EventCharacMov1th event_records[10];
    int event_count = 0;
    if (EventCharacMov1th_GetByEventCode(manager, "CHARAC_MOV_2009_Q2", event_records, 10, &event_count) == 0) {
        printf("✓ Found %d records for event\n", event_count);
    } else {
        printf("✗ Query failed\n");
    }

    printf("\n");
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM event_charac_mov_1th WHERE m_id IN (100001, 200001)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);
    DBQueryResult_Free(&result);
}
