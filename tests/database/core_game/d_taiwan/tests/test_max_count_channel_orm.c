#include "max_count_channel_orm.h"
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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Max Count Channel ORM Test Suite ===\n\n");

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
        "DELETE FROM max_count_channel WHERE server_info IN (10, 11, 12)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    MaxCountChannel rec1, rec2, retrieved;
    int exists;

    printf("--- Test 1: Add ---\n");

    /* Prepare test data 1 */
    memset(&rec1, 0, sizeof(MaxCountChannel));
    rec1.server_info = 10;
    strcpy(rec1.gc_channeltype, "PVP_ARENA");
    rec1.mc_max = 5000;
    strcpy(rec1.mc_date, "2025-11-18 14:30:00");

    if (MaxCountChannel_Add(manager, &rec1) == 0) {
        printf("PASS: Added channel record 1 (server=%u, type=%s)\n",
               rec1.server_info, rec1.gc_channeltype);
    } else {
        printf("FAIL: Failed to add channel record 1\n");
    }

    /* Prepare test data 2 */
    memset(&rec2, 0, sizeof(MaxCountChannel));
    rec2.server_info = 10;
    strcpy(rec2.gc_channeltype, "DUNGEON");
    rec2.mc_max = 8500;
    strcpy(rec2.mc_date, "2025-11-18 15:00:00");

    if (MaxCountChannel_Add(manager, &rec2) == 0) {
        printf("PASS: Added channel record 2 (server=%u, type=%s)\n",
               rec2.server_info, rec2.gc_channeltype);
    } else {
        printf("FAIL: Failed to add channel record 2\n");
    }

    printf("\n--- Test 2: Exists ---\n");
    exists = MaxCountChannel_Exists(manager, 10, "PVP_ARENA");
    if (exists == 1) {
        printf("PASS: Channel record (10, PVP_ARENA) exists\n");
    } else {
        printf("FAIL: Channel record (10, PVP_ARENA) should exist\n");
    }

    exists = MaxCountChannel_Exists(manager, 99, "NONEXIST");
    if (exists == 0) {
        printf("PASS: Channel record (99, NONEXIST) does not exist\n");
    } else {
        printf("FAIL: Channel record (99, NONEXIST) should not exist\n");
    }

    printf("\n--- Test 3: Get ---\n");
    memset(&retrieved, 0, sizeof(MaxCountChannel));
    if (MaxCountChannel_Get(manager, 10, "PVP_ARENA", &retrieved) == 0) {
        printf("PASS: Retrieved channel record (10, PVP_ARENA)\n");
        MaxCountChannel_PrintInfo(&retrieved);

        /* Verify data */
        if (retrieved.server_info == 10 &&
            strcmp(retrieved.gc_channeltype, "PVP_ARENA") == 0 &&
            retrieved.mc_max == 5000 &&
            strcmp(retrieved.mc_date, "2025-11-18 14:30:00") == 0) {
            printf("PASS: All fields match\n");
        } else {
            printf("FAIL: Field mismatch\n");
        }
    } else {
        printf("FAIL: Failed to retrieve channel record\n");
    }

    printf("\n--- Test 4: Update ---\n");
    retrieved.mc_max = 5500;
    strcpy(retrieved.mc_date, "2025-11-18 16:00:00");

    if (MaxCountChannel_Update(manager, &retrieved) == 0) {
        printf("PASS: Updated channel record (10, PVP_ARENA)\n");

        /* Verify update */
        memset(&retrieved, 0, sizeof(MaxCountChannel));
        if (MaxCountChannel_Get(manager, 10, "PVP_ARENA", &retrieved) == 0) {
            if (retrieved.mc_max == 5500 &&
                strcmp(retrieved.mc_date, "2025-11-18 16:00:00") == 0) {
                printf("PASS: Update verified\n");
            } else {
                printf("FAIL: Update not reflected\n");
            }
        }
    } else {
        printf("FAIL: Failed to update channel record\n");
    }

    printf("\n--- Test 5: Delete ---\n");
    if (MaxCountChannel_Delete(manager, 10, "DUNGEON") == 0) {
        printf("PASS: Deleted channel record (10, DUNGEON)\n");

        /* Verify deletion */
        exists = MaxCountChannel_Exists(manager, 10, "DUNGEON");
        if (exists == 0) {
            printf("PASS: Channel record (10, DUNGEON) no longer exists\n");
        } else {
            printf("FAIL: Channel record (10, DUNGEON) still exists\n");
        }
    } else {
        printf("FAIL: Failed to delete channel record\n");
    }
}

void test_business_operations(DBConnectionManager* manager) {
    MaxCountChannel records[20];
    int actual_count;
    MaxCountChannel test_rec, max_rec;

    printf("\n--- Test 6: Add More Test Data ---\n");

    /* Add more records for testing */
    memset(&test_rec, 0, sizeof(MaxCountChannel));
    test_rec.server_info = 11;
    strcpy(test_rec.gc_channeltype, "PVP_ARENA");
    test_rec.mc_max = 6000;
    strcpy(test_rec.mc_date, "2025-11-18 17:00:00");
    MaxCountChannel_Add(manager, &test_rec);

    memset(&test_rec, 0, sizeof(MaxCountChannel));
    test_rec.server_info = 11;
    strcpy(test_rec.gc_channeltype, "DUNGEON");
    test_rec.mc_max = 9000;
    strcpy(test_rec.mc_date, "2025-11-18 17:30:00");
    MaxCountChannel_Add(manager, &test_rec);

    memset(&test_rec, 0, sizeof(MaxCountChannel));
    test_rec.server_info = 12;
    strcpy(test_rec.gc_channeltype, "PVP_ARENA");
    test_rec.mc_max = 4500;
    strcpy(test_rec.mc_date, "2025-11-18 18:00:00");
    MaxCountChannel_Add(manager, &test_rec);

    printf("PASS: Added 3 more test records\n");

    printf("\n--- Test 7: GetAll ---\n");
    if (MaxCountChannel_GetAll(manager, records, 20, &actual_count) == 0) {
        printf("PASS: Retrieved %d channel records\n", actual_count);
        for (int i = 0; i < actual_count; i++) {
            printf("  [%d] Server=%u, Type=%s, Max=%u, Date=%s\n",
                   i + 1, records[i].server_info, records[i].gc_channeltype,
                   records[i].mc_max, records[i].mc_date);
        }
    } else {
        printf("FAIL: Failed to retrieve all channel records\n");
    }

    printf("\n--- Test 8: GetByServer ---\n");
    if (MaxCountChannel_GetByServer(manager, 11, records, 20, &actual_count) == 0) {
        printf("PASS: Retrieved %d records for server 11\n", actual_count);
        for (int i = 0; i < actual_count; i++) {
            if (records[i].server_info != 11) {
                printf("FAIL: Record %d has wrong server %u\n", i, records[i].server_info);
            }
        }
    } else {
        printf("FAIL: Failed to retrieve records by server\n");
    }

    printf("\n--- Test 9: GetByChannelType ---\n");
    if (MaxCountChannel_GetByChannelType(manager, "PVP_ARENA", records, 20, &actual_count) == 0) {
        printf("PASS: Retrieved %d records for channel type PVP_ARENA\n", actual_count);
        for (int i = 0; i < actual_count; i++) {
            if (strcmp(records[i].gc_channeltype, "PVP_ARENA") != 0) {
                printf("FAIL: Record %d has wrong channel type %s\n",
                       i, records[i].gc_channeltype);
            }
        }
    } else {
        printf("FAIL: Failed to retrieve records by channel type\n");
    }

    printf("\n--- Test 10: GetByDateRange ---\n");
    if (MaxCountChannel_GetByDateRange(manager, "2025-11-18 14:00:00",
                                        "2025-11-18 18:00:00", records, 20, &actual_count) == 0) {
        printf("PASS: Retrieved %d records in date range\n", actual_count);
    } else {
        printf("FAIL: Failed to retrieve records by date range\n");
    }

    printf("\n--- Test 11: GetMaxRecord ---\n");
    memset(&max_rec, 0, sizeof(MaxCountChannel));
    if (MaxCountChannel_GetMaxRecord(manager, 11, &max_rec) == 0) {
        printf("PASS: Retrieved max record for server 11\n");
        MaxCountChannel_PrintInfo(&max_rec);
        if (max_rec.mc_max == 9000) {
            printf("PASS: Correct max value retrieved\n");
        } else {
            printf("FAIL: Wrong max value %u\n", max_rec.mc_max);
        }
    } else {
        printf("FAIL: Failed to retrieve max record\n");
    }
}
