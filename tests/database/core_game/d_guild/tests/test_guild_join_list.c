#include "guild_join_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void test_crud_operations(DBConnectionManager* manager) {
    printf("Testing CRUD operations...\n");

    // Test Add
    GuildJoinList join1 = {
        1001,
        10001,
        1,
        5001,
        "95",
        "Good player",
        "2025-11-19 10:00:00"
    };
    assert(GuildJoinList_Add(manager, &join1) == 0);
    printf("  Add: PASS\n");

    // Test Exists (composite key: guild_id, charac_no)
    assert(GuildJoinList_Exists(manager, 1001, 10001) == 1);
    printf("  Exists: PASS\n");

    // Test Get (composite key)
    GuildJoinList join2;
    assert(GuildJoinList_Get(manager, 1001, 10001, &join2) == 0);
    assert(join2.guild_id == 1001);
    assert(join2.charac_no == 10001);
    assert(strcmp(join2.memo, "Good player") == 0);
    printf("  Get: PASS\n");

    // Test Update
    strcpy(join2.memo, "Excellent player");
    assert(GuildJoinList_Update(manager, &join2) == 0);
    assert(GuildJoinList_Get(manager, 1001, 10001, &join2) == 0);
    assert(strcmp(join2.memo, "Excellent player") == 0);
    printf("  Update: PASS\n");

    // Test Delete (composite key)
    assert(GuildJoinList_Delete(manager, 1001, 10001) == 0);
    assert(GuildJoinList_Exists(manager, 1001, 10001) == 0);
    printf("  Delete: PASS\n");
}

void test_business_operations(DBConnectionManager* manager) {
    printf("Testing business operations...\n");

    // Add test data
    GuildJoinList joins[] = {
        {1001, 10001, 1, 5001, "95", "Player1", "2025-11-19 10:00:00"},
        {1001, 10002, 1, 5002, "96", "Player2", "2025-11-19 11:00:00"},
        {1002, 10003, 1, 5003, "97", "Player3", "2025-11-19 12:00:00"},
        {1001, 10004, 2, 5004, "98", "Player4", "2025-11-19 13:00:00"}
    };

    for (int i = 0; i < 4; i++) {
        assert(GuildJoinList_Add(manager, &joins[i]) == 0);
    }

    // Test GetByGuild
    GuildJoinList* guild_joins = NULL;
    int count = 0;
    assert(GuildJoinList_GetByGuild(manager, 1001, &guild_joins, &count) == 0);
    assert(count == 3); // 3 applicants for guild 1001
    assert(guild_joins != NULL);
    free(guild_joins);
    printf("  GetByGuild: PASS\n");

    // Test GetByCharacter
    GuildJoinList* char_joins = NULL;
    count = 0;
    assert(GuildJoinList_GetByCharacter(manager, 10001, &char_joins, &count) == 0);
    assert(count == 1); // 1 application by character 10001
    assert(char_joins != NULL);
    free(char_joins);
    printf("  GetByCharacter: PASS\n");

    // Test GetByServer
    GuildJoinList* server_joins = NULL;
    count = 0;
    assert(GuildJoinList_GetByServer(manager, 1, &server_joins, &count) == 0);
    assert(count == 3); // 3 applications on server 1
    assert(server_joins != NULL);
    free(server_joins);
    printf("  GetByServer: PASS\n");

    // Cleanup (must use both primary key fields)
    for (int i = 0; i < 4; i++) {
        GuildJoinList_Delete(manager, joins[i].guild_id, joins[i].charac_no);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("=== Guild Join List ORM Test ===\n\n");

    DBConnectionManager manager;
    memset(&manager, 0, sizeof(DBConnectionManager));

    if (DBConnectionManager_Initialize(&manager, argv[1]) != 0) {
        fprintf(stderr, "Failed to initialize DBConnectionManager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_GUILD) != 0) {
        fprintf(stderr, "Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    test_crud_operations(&manager);
    test_business_operations(&manager);

    DBConnectionManager_Cleanup(&manager);

    printf("\n=== All tests passed ===\n");
    return 0;
}
