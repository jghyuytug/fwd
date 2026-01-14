#include "power_war.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void test_power_war_crud(const char* config_file) {
    printf("Testing PowerWar CRUD operations...\n");

    DBConnectionManager manager;
    memset(&manager, 0, sizeof(DBConnectionManager));

    assert(DBConnectionManager_Initialize(&manager, config_file) == 0);
    assert(DBConnectionManager_Connect(&manager, DB_TYPE_GUILD) == 0);

    // Clean up any existing test data
    PowerWar_Delete(&manager, 1);
    PowerWar_Delete(&manager, 2);
    PowerWar_Delete(&manager, 3);

    // Test Add
    PowerWar record1 = {1, 5000, 3000, 1, "2025-01-15 10:30:00"};
    assert(PowerWar_Add(&manager, &record1) == 0);
    printf("  [PASS] Add operation\n");

    // Test Exists
    assert(PowerWar_Exists(&manager, 1) == 1);
    assert(PowerWar_Exists(&manager, 99) == 0);
    printf("  [PASS] Exists operation\n");

    // Test Get
    PowerWar retrieved;
    assert(PowerWar_Get(&manager, 1, &retrieved) == 0);
    assert(retrieved.server_id == 1);
    assert(retrieved.a_side_point == 5000);
    assert(retrieved.b_side_point == 3000);
    assert(retrieved.winner_side == 1);
    assert(strcmp(retrieved.occ_time, "2025-01-15 10:30:00") == 0);
    printf("  [PASS] Get operation\n");

    // Test Update
    retrieved.a_side_point = 8000;
    retrieved.winner_side = 0;
    assert(PowerWar_Update(&manager, &retrieved) == 0);

    PowerWar updated;
    assert(PowerWar_Get(&manager, 1, &updated) == 0);
    assert(updated.a_side_point == 8000);
    assert(updated.winner_side == 0);
    printf("  [PASS] Update operation\n");

    // Test Delete
    assert(PowerWar_Delete(&manager, 1) == 0);
    assert(PowerWar_Exists(&manager, 1) == 0);
    printf("  [PASS] Delete operation\n");

    DBConnectionManager_Cleanup(&manager);
    printf("PowerWar CRUD tests completed successfully.\n\n");
}

void test_power_war_business(const char* config_file) {
    printf("Testing PowerWar business logic...\n");

    DBConnectionManager manager;
    memset(&manager, 0, sizeof(DBConnectionManager));

    assert(DBConnectionManager_Initialize(&manager, config_file) == 0);
    assert(DBConnectionManager_Connect(&manager, DB_TYPE_GUILD) == 0);

    // Clean up
    PowerWar_Delete(&manager, 1);
    PowerWar_Delete(&manager, 2);
    PowerWar_Delete(&manager, 3);

    // Add test data
    PowerWar wars[] = {
        {1, 10000, 8000, 1, "2025-01-10 12:00:00"},
        {2, 5000, 7000, 2, "2025-01-15 14:00:00"},
        {3, 3000, 2000, 0, "2025-01-20 16:00:00"}
    };

    for (int i = 0; i < 3; i++) {
        assert(PowerWar_Add(&manager, &wars[i]) == 0);
    }

    // Test GetWinnerServers
    PowerWar* winners = NULL;
    int winner_count = 0;
    assert(PowerWar_GetWinnerServers(&manager, &winners, &winner_count) == 0);
    assert(winner_count == 2); // servers 1 and 2 have winners
    assert(winners[0].server_id == 1);
    assert(winners[1].server_id == 2);
    free(winners);
    printf("  [PASS] GetWinnerServers\n");

    // Test GetByDateRange
    PowerWar* range_results = NULL;
    int range_count = 0;
    assert(PowerWar_GetByDateRange(&manager, "2025-01-14 00:00:00", "2025-01-21 00:00:00", &range_results, &range_count) == 0);
    assert(range_count == 2); // servers 2 and 3
    free(range_results);
    printf("  [PASS] GetByDateRange\n");

    // Test GetLeaderboard
    PowerWar* leaderboard = NULL;
    int lb_count = 0;
    assert(PowerWar_GetLeaderboard(&manager, 2, &leaderboard, &lb_count) == 0);
    assert(lb_count == 2);
    assert(leaderboard[0].server_id == 1); // 18000 total points
    assert(leaderboard[1].server_id == 2); // 12000 total points
    free(leaderboard);
    printf("  [PASS] GetLeaderboard\n");

    // Clean up
    for (int i = 0; i < 3; i++) {
        PowerWar_Delete(&manager, wars[i].server_id);
    }

    DBConnectionManager_Cleanup(&manager);
    printf("PowerWar business logic tests completed successfully.\n\n");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("=== PowerWar ORM Test Suite ===\n\n");
    test_power_war_crud(argv[1]);
    test_power_war_business(argv[1]);
    printf("=== All PowerWar tests passed ===\n");
    return 0;
}
