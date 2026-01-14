#include "power_war_statue_ranker.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void test_power_war_statue_ranker_crud(const char* config_file) {
    printf("Testing PowerWarStatueRanker CRUD operations...\n");

    DBConnectionManager manager;
    memset(&manager, 0, sizeof(DBConnectionManager));

    assert(DBConnectionManager_Initialize(&manager, config_file) == 0);
    assert(DBConnectionManager_Connect(&manager, DB_TYPE_GUILD) == 0);

    // Clean up
    PowerWarStatueRanker_Delete(&manager, 1);
    PowerWarStatueRanker_Delete(&manager, 2);

    // Test Add
    PowerWarStatueRanker record1 = {1, 1001, 1002, 1003};
    assert(PowerWarStatueRanker_Add(&manager, &record1) == 0);
    printf("  [PASS] Add operation\n");

    // Test Exists
    assert(PowerWarStatueRanker_Exists(&manager, 1) == 1);
    assert(PowerWarStatueRanker_Exists(&manager, 99) == 0);
    printf("  [PASS] Exists operation\n");

    // Test Get
    PowerWarStatueRanker retrieved;
    assert(PowerWarStatueRanker_Get(&manager, 1, &retrieved) == 0);
    assert(retrieved.server_id == 1);
    assert(retrieved.first_ranker == 1001);
    assert(retrieved.second_ranker == 1002);
    assert(retrieved.third_ranker == 1003);
    printf("  [PASS] Get operation\n");

    // Test Update
    retrieved.first_ranker = 2001;
    retrieved.third_ranker = 2003;
    assert(PowerWarStatueRanker_Update(&manager, &retrieved) == 0);

    PowerWarStatueRanker updated;
    assert(PowerWarStatueRanker_Get(&manager, 1, &updated) == 0);
    assert(updated.first_ranker == 2001);
    assert(updated.second_ranker == 1002);
    assert(updated.third_ranker == 2003);
    printf("  [PASS] Update operation\n");

    // Test Delete
    assert(PowerWarStatueRanker_Delete(&manager, 1) == 0);
    assert(PowerWarStatueRanker_Exists(&manager, 1) == 0);
    printf("  [PASS] Delete operation\n");

    DBConnectionManager_Cleanup(&manager);
    printf("PowerWarStatueRanker CRUD tests completed successfully.\n\n");
}

void test_power_war_statue_ranker_business(const char* config_file) {
    printf("Testing PowerWarStatueRanker business logic...\n");

    DBConnectionManager manager;
    memset(&manager, 0, sizeof(DBConnectionManager));

    assert(DBConnectionManager_Initialize(&manager, config_file) == 0);
    assert(DBConnectionManager_Connect(&manager, DB_TYPE_GUILD) == 0);

    // Clean up
    PowerWarStatueRanker_Delete(&manager, 1);
    PowerWarStatueRanker_Delete(&manager, 2);
    PowerWarStatueRanker_Delete(&manager, 3);

    // Add test data
    PowerWarStatueRanker rankers[] = {
        {1, 1001, 1002, 1003},
        {2, 2001, 2002, 2003},
        {3, 3001, 3002, 3003}
    };

    for (int i = 0; i < 3; i++) {
        assert(PowerWarStatueRanker_Add(&manager, &rankers[i]) == 0);
    }

    // Test GetAllServers
    PowerWarStatueRanker* all_servers = NULL;
    int server_count = 0;
    assert(PowerWarStatueRanker_GetAllServers(&manager, &all_servers, &server_count) == 0);
    assert(server_count == 3);
    assert(all_servers[0].server_id == 1);
    assert(all_servers[1].server_id == 2);
    assert(all_servers[2].server_id == 3);
    free(all_servers);
    printf("  [PASS] GetAllServers\n");

    // Test UpdateRanker
    assert(PowerWarStatueRanker_UpdateRanker(&manager, 1, 1, 5001) == 0);
    assert(PowerWarStatueRanker_UpdateRanker(&manager, 2, 3, 5003) == 0);

    PowerWarStatueRanker updated1, updated2;
    assert(PowerWarStatueRanker_Get(&manager, 1, &updated1) == 0);
    assert(updated1.first_ranker == 5001);

    assert(PowerWarStatueRanker_Get(&manager, 2, &updated2) == 0);
    assert(updated2.third_ranker == 5003);
    printf("  [PASS] UpdateRanker\n");

    // Test GetRankerPosition
    unsigned char found_server_id;
    int found_position;
    assert(PowerWarStatueRanker_GetRankerPosition(&manager, 5001, &found_server_id, &found_position) == 0);
    assert(found_server_id == 1);
    assert(found_position == 1);

    assert(PowerWarStatueRanker_GetRankerPosition(&manager, 2002, &found_server_id, &found_position) == 0);
    assert(found_server_id == 2);
    assert(found_position == 2);

    assert(PowerWarStatueRanker_GetRankerPosition(&manager, 5003, &found_server_id, &found_position) == 0);
    assert(found_server_id == 2);
    assert(found_position == 3);
    printf("  [PASS] GetRankerPosition\n");

    // Clean up
    for (int i = 0; i < 3; i++) {
        PowerWarStatueRanker_Delete(&manager, rankers[i].server_id);
    }

    DBConnectionManager_Cleanup(&manager);
    printf("PowerWarStatueRanker business logic tests completed successfully.\n\n");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("=== PowerWarStatueRanker ORM Test Suite ===\n\n");
    test_power_war_statue_ranker_crud(argv[1]);
    test_power_war_statue_ranker_business(argv[1]);
    printf("=== All PowerWarStatueRanker tests passed ===\n");
    return 0;
}
