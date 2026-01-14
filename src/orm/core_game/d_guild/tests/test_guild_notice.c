#include "guild_notice.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void test_crud_operations(DBConnectionManager* manager) {
    printf("Testing CRUD operations...\n");

    // Test Add
    GuildNotice notice1 = {
        1001,
        "Welcome to our guild!",
        20251119
    };
    assert(GuildNotice_Add(manager, &notice1) == 0);
    printf("  Add: PASS\n");

    // Test Exists
    assert(GuildNotice_Exists(manager, 1001) == 1);
    printf("  Exists: PASS\n");

    // Test Get
    GuildNotice notice2;
    assert(GuildNotice_Get(manager, 1001, &notice2) == 0);
    assert(notice2.guild_id == 1001);
    assert(strcmp(notice2.notice, "Welcome to our guild!") == 0);
    assert(notice2.acc_date == 20251119);
    printf("  Get: PASS\n");

    // Test Update
    strcpy(notice2.notice, "Guild rules updated!");
    notice2.acc_date = 20251120;
    assert(GuildNotice_Update(manager, &notice2) == 0);
    assert(GuildNotice_Get(manager, 1001, &notice2) == 0);
    assert(strcmp(notice2.notice, "Guild rules updated!") == 0);
    assert(notice2.acc_date == 20251120);
    printf("  Update: PASS\n");

    // Test Delete
    assert(GuildNotice_Delete(manager, 1001) == 0);
    assert(GuildNotice_Exists(manager, 1001) == 0);
    printf("  Delete: PASS\n");
}

void test_business_operations(DBConnectionManager* manager) {
    printf("Testing business operations...\n");

    // Add test data
    GuildNotice notices[] = {
        {1001, "Notice 1", 20251115},
        {1002, "Notice 2", 20251116},
        {1003, "Notice 3", 20251117},
        {1004, "Notice 4", 20251118},
        {1005, "Notice 5", 20251119}
    };

    for (int i = 0; i < 5; i++) {
        assert(GuildNotice_Add(manager, &notices[i]) == 0);
    }

    // Test GetAll
    GuildNotice* all_notices = NULL;
    int count = 0;
    assert(GuildNotice_GetAll(manager, &all_notices, &count) == 0);
    assert(count == 5);
    assert(all_notices != NULL);
    free(all_notices);
    printf("  GetAll: PASS\n");

    // Test GetRecentNotices
    GuildNotice* recent_notices = NULL;
    count = 0;
    assert(GuildNotice_GetRecentNotices(manager, 3, &recent_notices, &count) == 0);
    assert(count == 3);
    assert(recent_notices != NULL);
    // Verify order (most recent first)
    assert(recent_notices[0].acc_date == 20251119);
    assert(recent_notices[1].acc_date == 20251118);
    assert(recent_notices[2].acc_date == 20251117);
    free(recent_notices);
    printf("  GetRecentNotices: PASS\n");

    // Test UpdateNotice
    assert(GuildNotice_UpdateNotice(manager, 1001, "Updated notice text", 20251120) == 0);
    GuildNotice updated;
    assert(GuildNotice_Get(manager, 1001, &updated) == 0);
    assert(strcmp(updated.notice, "Updated notice text") == 0);
    assert(updated.acc_date == 20251120);
    printf("  UpdateNotice: PASS\n");

    // Cleanup
    for (int i = 0; i < 5; i++) {
        GuildNotice_Delete(manager, notices[i].guild_id);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("=== Guild Notice ORM Test ===\n\n");

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
