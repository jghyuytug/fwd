#include "guild_grade_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void test_crud_operations(DBConnectionManager* manager) {
    printf("Testing CRUD operations...\n");

    // Test Add (auto_increment - no id parameter)
    GuildGradeLog log1 = {
        0, // id is auto-generated
        1001,
        5001,
        1,
        10001,
        "TestChar",
        "2025-11-19 10:00:00",
        1,
        2,
        "Promotion",
        9001,
        "AdminUser"
    };

    int new_id = 0;
    assert(GuildGradeLog_Add(manager, &log1, &new_id) == 0);
    assert(new_id > 0);
    printf("  Add (auto_increment): PASS (new_id = %d)\n", new_id);

    // Test Get
    GuildGradeLog log2;
    assert(GuildGradeLog_Get(manager, new_id, &log2) == 0);
    assert(log2.id == new_id);
    assert(log2.guild_id == 1001);
    assert(strcmp(log2.charac_name, "TestChar") == 0);
    printf("  Get: PASS\n");

    // Test Delete
    assert(GuildGradeLog_Delete(manager, new_id) == 0);
    assert(GuildGradeLog_Get(manager, new_id, &log2) != 0);
    printf("  Delete: PASS\n");
}

void test_business_operations(DBConnectionManager* manager) {
    printf("Testing business operations...\n");

    // Add test data
    GuildGradeLog logs[] = {
        {0, 1001, 5001, 1, 10001, "Char1", "2025-11-19 10:00:00", 1, 2, "Promotion", 9001, "Admin1"},
        {0, 1001, 5002, 1, 10002, "Char2", "2025-11-19 11:00:00", 2, 3, "Promotion", 9001, "Admin1"},
        {0, 1002, 5003, 1, 10003, "Char3", "2025-11-19 12:00:00", 1, 2, "Promotion", 9002, "Admin2"},
        {0, 1001, 5004, 2, 10004, "Char4", "2025-11-20 10:00:00", 3, 2, "Demotion", 9001, "Admin1"}
    };

    int ids[4];
    for (int i = 0; i < 4; i++) {
        assert(GuildGradeLog_Add(manager, &logs[i], &ids[i]) == 0);
    }

    // Test GetByGuild
    GuildGradeLog* guild_logs = NULL;
    int count = 0;
    assert(GuildGradeLog_GetByGuild(manager, 1001, &guild_logs, &count) == 0);
    assert(count == 3); // 3 logs for guild 1001
    assert(guild_logs != NULL);
    free(guild_logs);
    printf("  GetByGuild: PASS\n");

    // Test GetByCharacter
    GuildGradeLog* char_logs = NULL;
    count = 0;
    assert(GuildGradeLog_GetByCharacter(manager, 1, 10001, &char_logs, &count) == 0);
    assert(count == 1); // 1 log for character 10001 on server 1
    assert(char_logs != NULL);
    free(char_logs);
    printf("  GetByCharacter: PASS\n");

    // Test GetByDateRange
    GuildGradeLog* date_logs = NULL;
    count = 0;
    assert(GuildGradeLog_GetByDateRange(manager, "2025-11-19 00:00:00", "2025-11-19 23:59:59", &date_logs, &count) == 0);
    assert(count == 3); // 3 logs on 2025-11-19
    assert(date_logs != NULL);
    free(date_logs);
    printf("  GetByDateRange: PASS\n");

    // Cleanup
    for (int i = 0; i < 4; i++) {
        GuildGradeLog_Delete(manager, ids[i]);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("=== Guild Grade Log ORM Test ===\n\n");

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
