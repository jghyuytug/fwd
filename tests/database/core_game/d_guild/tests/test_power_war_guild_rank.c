#include "power_war_guild_rank.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void test_power_war_guild_rank_crud(const char* config_file) {
    printf("Testing PowerWarGuildRank CRUD operations...\n");

    DBConnectionManager manager;
    memset(&manager, 0, sizeof(DBConnectionManager));

    assert(DBConnectionManager_Initialize(&manager, config_file) == 0);
    assert(DBConnectionManager_Connect(&manager, DB_TYPE_GUILD) == 0);

    // Clean up
    PowerWarGuildRank_Delete(&manager, 1001);
    PowerWarGuildRank_Delete(&manager, 1002);

    // Test Add
    PowerWarGuildRank record1 = {1, 1, 1001, 50000, 1};
    assert(PowerWarGuildRank_Add(&manager, &record1) == 0);
    printf("  [PASS] Add operation\n");

    // Test Exists
    assert(PowerWarGuildRank_Exists(&manager, 1001) == 1);
    assert(PowerWarGuildRank_Exists(&manager, 9999) == 0);
    printf("  [PASS] Exists operation\n");

    // Test Get
    PowerWarGuildRank retrieved;
    assert(PowerWarGuildRank_Get(&manager, 1001, &retrieved) == 0);
    assert(retrieved.server_id == 1);
    assert(retrieved.rank == 1);
    assert(retrieved.guild_id == 1001);
    assert(retrieved.power_war_point == 50000);
    assert(retrieved.power_side == 1);
    printf("  [PASS] Get operation\n");

    // Test Update
    retrieved.rank = 2;
    retrieved.power_war_point = 55000;
    assert(PowerWarGuildRank_Update(&manager, &retrieved) == 0);

    PowerWarGuildRank updated;
    assert(PowerWarGuildRank_Get(&manager, 1001, &updated) == 0);
    assert(updated.rank == 2);
    assert(updated.power_war_point == 55000);
    printf("  [PASS] Update operation\n");

    // Test Delete
    assert(PowerWarGuildRank_Delete(&manager, 1001) == 0);
    assert(PowerWarGuildRank_Exists(&manager, 1001) == 0);
    printf("  [PASS] Delete operation\n");

    DBConnectionManager_Cleanup(&manager);
    printf("PowerWarGuildRank CRUD tests completed successfully.\n\n");
}

void test_power_war_guild_rank_business(const char* config_file) {
    printf("Testing PowerWarGuildRank business logic...\n");

    DBConnectionManager manager;
    memset(&manager, 0, sizeof(DBConnectionManager));

    assert(DBConnectionManager_Initialize(&manager, config_file) == 0);
    assert(DBConnectionManager_Connect(&manager, DB_TYPE_GUILD) == 0);

    // Clean up
    PowerWarGuildRank_Delete(&manager, 1001);
    PowerWarGuildRank_Delete(&manager, 1002);
    PowerWarGuildRank_Delete(&manager, 1003);
    PowerWarGuildRank_Delete(&manager, 1004);

    // Add test data
    PowerWarGuildRank ranks[] = {
        {1, 1, 1001, 50000, 1},
        {1, 2, 1002, 45000, 1},
        {1, 3, 1003, 40000, 2},
        {2, 1, 1004, 60000, 2}
    };

    for (int i = 0; i < 4; i++) {
        assert(PowerWarGuildRank_Add(&manager, &ranks[i]) == 0);
    }

    // Test GetByServer
    PowerWarGuildRank* server_ranks = NULL;
    int server_count = 0;
    assert(PowerWarGuildRank_GetByServer(&manager, 1, &server_ranks, &server_count) == 0);
    assert(server_count == 3);
    assert(server_ranks[0].rank == 1);
    assert(server_ranks[1].rank == 2);
    free(server_ranks);
    printf("  [PASS] GetByServer\n");

    // Test GetBySide
    PowerWarGuildRank* side_ranks = NULL;
    int side_count = 0;
    assert(PowerWarGuildRank_GetBySide(&manager, 1, &side_ranks, &side_count) == 0);
    assert(side_count == 2);
    assert(side_ranks[0].guild_id == 1001); // Highest points
    free(side_ranks);
    printf("  [PASS] GetBySide\n");

    // Test GetTopRanks
    PowerWarGuildRank* top_ranks = NULL;
    int top_count = 0;
    assert(PowerWarGuildRank_GetTopRanks(&manager, 1, 2, &top_ranks, &top_count) == 0);
    assert(top_count == 2);
    assert(top_ranks[0].rank == 1);
    assert(top_ranks[1].rank == 2);
    free(top_ranks);
    printf("  [PASS] GetTopRanks\n");

    // Clean up
    for (int i = 0; i < 4; i++) {
        PowerWarGuildRank_Delete(&manager, ranks[i].guild_id);
    }

    DBConnectionManager_Cleanup(&manager);
    printf("PowerWarGuildRank business logic tests completed successfully.\n\n");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("=== PowerWarGuildRank ORM Test Suite ===\n\n");
    test_power_war_guild_rank_crud(argv[1]);
    test_power_war_guild_rank_business(argv[1]);
    printf("=== All PowerWarGuildRank tests passed ===\n");
    return 0;
}
