#include "guild_introduce_orm.h"
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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_GUILD) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Guild Introduce ORM Test Suite ===\n\n");

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
        "DELETE FROM guild_introduce WHERE guild_id IN (1001, 2001, 2002, 2003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    GuildIntroduce intro, retrieved;
    int ret;

    printf("--- Test 1: Add Guild Introduction ---\n");
    memset(&intro, 0, sizeof(GuildIntroduce));
    intro.guild_id = 1001;
    intro.server_id = 1;
    strcpy(intro.introduce, "Welcome to our guild! We are a friendly community.");

    ret = GuildIntroduce_Add(manager, &intro);
    if (ret == 0) {
        printf("PASS: Added guild introduction successfully\n");
    } else {
        printf("FAIL: Failed to add guild introduction (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 2: Get Guild Introduction ---\n");
    memset(&retrieved, 0, sizeof(GuildIntroduce));
    ret = GuildIntroduce_Get(manager, 1001, &retrieved);
    if (ret == 0) {
        printf("PASS: Retrieved guild introduction (guild_id=%d)\n", retrieved.guild_id);
        GuildIntroduce_PrintInfo(&retrieved);
    } else {
        printf("FAIL: Failed to retrieve guild introduction (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 3: Update Guild Introduction ---\n");
    strcpy(retrieved.introduce, "Updated: Join us for epic adventures!");
    ret = GuildIntroduce_Update(manager, &retrieved);
    if (ret == 0) {
        printf("PASS: Updated guild introduction successfully\n");
    } else {
        printf("FAIL: Failed to update guild introduction (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 4: Verify Update ---\n");
    memset(&retrieved, 0, sizeof(GuildIntroduce));
    ret = GuildIntroduce_Get(manager, 1001, &retrieved);
    if (ret == 0 && strcmp(retrieved.introduce, "Updated: Join us for epic adventures!") == 0) {
        printf("PASS: Update verified (introduce=%s)\n", retrieved.introduce);
    } else {
        printf("FAIL: Update verification failed\n");
    }

    printf("\n--- Test 5: Check Exists ---\n");
    ret = GuildIntroduce_Exists(manager, 1001);
    if (ret == 1) {
        printf("PASS: Guild introduction exists (guild_id=1001)\n");
    } else {
        printf("FAIL: Exists check failed (ret=%d)\n", ret);
    }

    printf("\n--- Test 6: Delete Guild Introduction ---\n");
    ret = GuildIntroduce_Delete(manager, 1001);
    if (ret == 0) {
        printf("PASS: Deleted guild introduction successfully\n");
    } else {
        printf("FAIL: Failed to delete guild introduction (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 7: Verify Deletion ---\n");
    ret = GuildIntroduce_Exists(manager, 1001);
    if (ret == 0) {
        printf("PASS: Guild introduction no longer exists\n");
    } else {
        printf("FAIL: Deletion verification failed (ret=%d)\n", ret);
    }
}

void test_business_operations(DBConnectionManager* manager) {
    GuildIntroduce intros[10];
    int count = 0;
    int ret;

    printf("\n--- Test 8: Add Multiple Introductions for Same Server ---\n");
    for (int i = 0; i < 3; i++) {
        GuildIntroduce intro;
        memset(&intro, 0, sizeof(GuildIntroduce));
        intro.guild_id = 2001 + i;
        intro.server_id = 2;
        snprintf(intro.introduce, sizeof(intro.introduce),
                 "Guild %d: We are looking for active players!", i + 1);

        ret = GuildIntroduce_Add(manager, &intro);
        if (ret != 0) {
            printf("FAIL: Failed to add introduction %d (ret=%d)\n", i + 1, ret);
            return;
        }
    }
    printf("PASS: Added 3 introductions for server 2\n");

    printf("\n--- Test 9: Get Introductions by Server ---\n");
    ret = GuildIntroduce_GetByServer(manager, 2, intros, 10, &count);
    if (ret == 0 && count == 3) {
        printf("PASS: Retrieved %d introductions for server 2\n", count);
        for (int i = 0; i < count; i++) {
            printf("  Guild %d: %s\n", intros[i].guild_id, intros[i].introduce);
        }
    } else {
        printf("FAIL: Failed to retrieve introductions (ret=%d, count=%d, expected=3)\n",
               ret, count);
    }

    printf("\n--- Test 10: Update Introduction Text ---\n");
    ret = GuildIntroduce_UpdateIntroduce(manager, 2001,
                                          "Modified: Join our competitive guild!");
    if (ret == 0) {
        GuildIntroduce updated;
        GuildIntroduce_Get(manager, 2001, &updated);
        if (strcmp(updated.introduce, "Modified: Join our competitive guild!") == 0) {
            printf("PASS: Introduction text updated successfully\n");
        } else {
            printf("FAIL: Introduction text not updated correctly\n");
        }
    } else {
        printf("FAIL: Failed to update introduction text (ret=%d)\n", ret);
    }

    printf("\n--- Test 11: Verify All Guilds Have Introductions ---\n");
    int all_exist = 1;
    for (int i = 0; i < 3; i++) {
        if (!GuildIntroduce_Exists(manager, 2001 + i)) {
            all_exist = 0;
            break;
        }
    }
    if (all_exist) {
        printf("PASS: All guild introductions exist\n");
    } else {
        printf("FAIL: Some introductions are missing\n");
    }
}
