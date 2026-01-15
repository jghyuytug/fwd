#include "power_war_user_rank.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void test_power_war_user_rank_crud(const char* config_file) {
    printf("Testing PowerWarUserRank CRUD operations...\n");

    DBConnectionManager manager;
    memset(&manager, 0, sizeof(DBConnectionManager));

    assert(DBConnectionManager_Initialize(&manager, config_file) == 0);
    assert(DBConnectionManager_Connect(&manager, DB_TYPE_GUILD) == 0);

    // Clean up - IMPORTANT: composite key requires both parameters
    PowerWarUserRank_Delete(&manager, 10001, 1);
    PowerWarUserRank_Delete(&manager, 10002, 1);

    // Test Add
    PowerWarUserRank record1 = {1, 1, 10001, 50000, 1};
    assert(PowerWarUserRank_Add(&manager, &record1) == 0);
    printf("  [PASS] Add operation\n");

    // Test Exists - IMPORTANT: composite key requires both parameters
    assert(PowerWarUserRank_Exists(&manager, 10001, 1) == 1);
    assert(PowerWarUserRank_Exists(&manager, 10001, 2) == 0); // Different server
    assert(PowerWarUserRank_Exists(&manager, 99999, 1) == 0); // Different charac
    printf("  [PASS] Exists operation\n");

    // Test Get - IMPORTANT: composite key requires both parameters
    PowerWarUserRank retrieved;
    assert(PowerWarUserRank_Get(&manager, 10001, 1, &retrieved) == 0);
    assert(retrieved.server_id == 1);
    assert(retrieved.rank == 1);
    assert(retrieved.charac_no == 10001);
    assert(retrieved.power_war_point == 50000);
    assert(retrieved.power_side == 1);
    printf("  [PASS] Get operation\n");

    // Test Update
    retrieved.rank = 2;
    retrieved.power_war_point = 55000;
    assert(PowerWarUserRank_Update(&manager, &retrieved) == 0);

    PowerWarUserRank updated;
    assert(PowerWarUserRank_Get(&manager, 10001, 1, &updated) == 0);
    assert(updated.rank == 2);
    assert(updated.power_war_point == 55000);
    printf("  [PASS] Update operation\n");

    // Test Delete - IMPORTANT: composite key requires both parameters
    assert(PowerWarUserRank_Delete(&manager, 10001, 1) == 0);
    assert(PowerWarUserRank_Exists(&manager, 10001, 1) == 0);
    printf("  [PASS] Delete operation\n");

    DBConnectionManager_Cleanup(&manager);
    printf("PowerWarUserRank CRUD tests completed successfully.\n\n");
}

void test_power_war_user_rank_business(const char* config_file) {
    printf("Testing PowerWarUserRank business logic...\n");

    DBConnectionManager manager;
    memset(&manager, 0, sizeof(DBConnectionManager));

    assert(DBConnectionManager_Initialize(&manager, config_file) == 0);
    assert(DBConnectionManager_Connect(&manager, DB_TYPE_GUILD) == 0);

    // Clean up
    PowerWarUserRank_Delete(&manager, 10001, 1);
    PowerWarUserRank_Delete(&manager, 10002, 1);
    PowerWarUserRank_Delete(&manager, 10003, 1);
    PowerWarUserRank_Delete(&manager, 10004, 2);

    // Add test data
    PowerWarUserRank ranks[] = {
        {1, 1, 10001, 50000, 1},
        {1, 2, 10002, 45000, 1},
        {1, 3, 10003, 40000, 2},
        {2, 1, 10004, 60000, 2}
    };

    for (int i = 0; i < 4; i++) {
        assert(PowerWarUserRank_Add(&manager, &ranks[i]) == 0);
    }

    // Test GetByServer
    PowerWarUserRank* server_ranks = NULL;
    int server_count = 0;
    assert(PowerWarUserRank_GetByServer(&manager, 1, &server_ranks, &server_count) == 0);
    assert(server_count == 3);
    assert(server_ranks[0].rank == 1);
    assert(server_ranks[1].rank == 2);
    assert(server_ranks[2].rank == 3);
    free(server_ranks);
    printf("  [PASS] GetByServer\n");

    // Test GetBySide
    PowerWarUserRank* side_ranks = NULL;
    int side_count = 0;
    assert(PowerWarUserRank_GetBySide(&manager, 1, &side_ranks, &side_count) == 0);
    assert(side_count == 2);
    assert(side_ranks[0].charac_no == 10001); // Highest points
    assert(side_ranks[0].power_war_point == 50000);
    free(side_ranks);
    printf("  [PASS] GetBySide\n");

    // Test GetTopRanks
    PowerWarUserRank* top_ranks = NULL;
    int top_count = 0;
    assert(PowerWarUserRank_GetTopRanks(&manager, 1, 2, &top_ranks, &top_count) == 0);
    assert(top_count == 2);
    assert(top_ranks[0].rank == 1);
    assert(top_ranks[1].rank == 2);
    free(top_ranks);
    printf("  [PASS] GetTopRanks\n");

    // Test composite key uniqueness - same charac_no, different server_id
    PowerWarUserRank record_cross_server = {2, 5, 10001, 30000, 1};
    assert(PowerWarUserRank_Add(&manager, &record_cross_server) == 0);
    assert(PowerWarUserRank_Exists(&manager, 10001, 1) == 1); // Original still exists
    assert(PowerWarUserRank_Exists(&manager, 10001, 2) == 1); // New record exists
    PowerWarUserRank_Delete(&manager, 10001, 2); // Clean up
    printf("  [PASS] Composite key uniqueness\n");

    // Clean up
    for (int i = 0; i < 4; i++) {
        PowerWarUserRank_Delete(&manager, ranks[i].charac_no, ranks[i].server_id);
    }

    DBConnectionManager_Cleanup(&manager);
    printf("PowerWarUserRank business logic tests completed successfully.\n\n");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("=== PowerWarUserRank ORM Test Suite ===\n\n");
    test_power_war_user_rank_crud(argv[1]);
    test_power_war_user_rank_business(argv[1]);
    printf("=== All PowerWarUserRank tests passed ===\n");
    return 0;
}
