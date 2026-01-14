#include "guild_recommend_orm.h"
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

    printf("=== Guild Recommend ORM Test Suite ===\n\n");

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
        "DELETE FROM guild_recommend WHERE no IN (1001, 2001, 2002, 2003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    GuildRecommend recommend, retrieved;
    int ret;

    printf("--- Test 1: Add Guild Recommendation ---\n");
    memset(&recommend, 0, sizeof(GuildRecommend));
    recommend.no = 1001;
    recommend.guild_id = 100;
    recommend.server_id = 1;
    recommend.charac_no = 50001;
    strcpy(recommend.charac_name, "TestCharacter");
    strcpy(recommend.comment, "Great guild for beginners!");
    strcpy(recommend.recommend_time, "2025-11-17 10:00:00");

    ret = GuildRecommend_Add(manager, &recommend);
    if (ret == 0) {
        printf("PASS: Added guild recommendation successfully\n");
    } else {
        printf("FAIL: Failed to add guild recommendation (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 2: Get Guild Recommendation ---\n");
    memset(&retrieved, 0, sizeof(GuildRecommend));
    ret = GuildRecommend_Get(manager, 1001, &retrieved);
    if (ret == 0) {
        printf("PASS: Retrieved guild recommendation (no=%d)\n", retrieved.no);
        GuildRecommend_PrintInfo(&retrieved);
    } else {
        printf("FAIL: Failed to retrieve guild recommendation (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 3: Update Guild Recommendation ---\n");
    strcpy(retrieved.comment, "Updated comment: Excellent community!");
    ret = GuildRecommend_Update(manager, &retrieved);
    if (ret == 0) {
        printf("PASS: Updated guild recommendation successfully\n");
    } else {
        printf("FAIL: Failed to update guild recommendation (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 4: Verify Update ---\n");
    memset(&retrieved, 0, sizeof(GuildRecommend));
    ret = GuildRecommend_Get(manager, 1001, &retrieved);
    if (ret == 0 && strcmp(retrieved.comment, "Updated comment: Excellent community!") == 0) {
        printf("PASS: Update verified (comment=%s)\n", retrieved.comment);
    } else {
        printf("FAIL: Update verification failed\n");
    }

    printf("\n--- Test 5: Check Exists ---\n");
    ret = GuildRecommend_Exists(manager, 1001);
    if (ret == 1) {
        printf("PASS: Guild recommendation exists (no=1001)\n");
    } else {
        printf("FAIL: Exists check failed (ret=%d)\n", ret);
    }

    printf("\n--- Test 6: Delete Guild Recommendation ---\n");
    ret = GuildRecommend_Delete(manager, 1001);
    if (ret == 0) {
        printf("PASS: Deleted guild recommendation successfully\n");
    } else {
        printf("FAIL: Failed to delete guild recommendation (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 7: Verify Deletion ---\n");
    ret = GuildRecommend_Exists(manager, 1001);
    if (ret == 0) {
        printf("PASS: Guild recommendation no longer exists\n");
    } else {
        printf("FAIL: Deletion verification failed (ret=%d)\n", ret);
    }
}

void test_business_operations(DBConnectionManager* manager) {
    GuildRecommend recommends[10];
    int count = 0;
    int ret;

    printf("\n--- Test 8: Add Multiple Recommendations for Same Guild ---\n");
    for (int i = 0; i < 3; i++) {
        GuildRecommend recommend;
        memset(&recommend, 0, sizeof(GuildRecommend));
        recommend.no = 2001 + i;
        recommend.guild_id = 200;
        recommend.server_id = 1;
        recommend.charac_no = 60001 + i;
        snprintf(recommend.charac_name, sizeof(recommend.charac_name),
                 "Player%d", i + 1);
        snprintf(recommend.comment, sizeof(recommend.comment),
                 "Recommendation comment %d", i + 1);
        snprintf(recommend.recommend_time, sizeof(recommend.recommend_time),
                 "2025-11-17 10:%02d:00", i);

        ret = GuildRecommend_Add(manager, &recommend);
        if (ret != 0) {
            printf("FAIL: Failed to add recommendation %d (ret=%d)\n", i + 1, ret);
            return;
        }
    }
    printf("PASS: Added 3 recommendations for guild 200\n");

    printf("\n--- Test 9: Get Recommendations by Guild ---\n");
    ret = GuildRecommend_GetByGuild(manager, 200, recommends, 10, &count);
    if (ret == 0 && count == 3) {
        printf("PASS: Retrieved %d recommendations for guild 200\n", count);
        for (int i = 0; i < count; i++) {
            printf("  Rec %d: no=%d, charac=%s, comment=%s\n",
                   i + 1, recommends[i].no, recommends[i].charac_name,
                   recommends[i].comment);
        }
    } else {
        printf("FAIL: Failed to retrieve recommendations (ret=%d, count=%d, expected=3)\n",
               ret, count);
    }

    printf("\n--- Test 10: Get Recommendations by Character ---\n");
    ret = GuildRecommend_GetByCharacter(manager, 60001, recommends, 10, &count);
    if (ret == 0 && count == 1) {
        printf("PASS: Retrieved %d recommendation(s) by character 60001\n", count);
        printf("  Recommendation: charac_name=%s, comment=%s\n",
               recommends[0].charac_name, recommends[0].comment);
    } else {
        printf("FAIL: Failed to retrieve by character (ret=%d, count=%d)\n", ret, count);
    }

    printf("\n--- Test 11: Count Recommendations by Guild ---\n");
    ret = GuildRecommend_CountByGuild(manager, 200, &count);
    if (ret == 0 && count == 3) {
        printf("PASS: Guild 200 has %d recommendations\n", count);
    } else {
        printf("FAIL: Count failed (ret=%d, count=%d, expected=3)\n", ret, count);
    }

    printf("\n--- Test 12: Update Comment ---\n");
    ret = GuildRecommend_UpdateComment(manager, 2001, "Modified recommendation text");
    if (ret == 0) {
        GuildRecommend updated;
        GuildRecommend_Get(manager, 2001, &updated);
        if (strcmp(updated.comment, "Modified recommendation text") == 0) {
            printf("PASS: Comment updated successfully\n");
        } else {
            printf("FAIL: Comment not updated correctly\n");
        }
    } else {
        printf("FAIL: Failed to update comment (ret=%d)\n", ret);
    }

    printf("\n--- Test 13: Delete All Recommendations for Guild ---\n");
    ret = GuildRecommend_DeleteByGuild(manager, 200);
    if (ret == 0) {
        printf("PASS: Deleted all recommendations for guild 200\n");
    } else {
        printf("FAIL: Failed to delete recommendations (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 14: Verify All Recommendations Deleted ---\n");
    ret = GuildRecommend_CountByGuild(manager, 200, &count);
    if (ret == 0 && count == 0) {
        printf("PASS: All recommendations deleted (count=0)\n");
    } else {
        printf("FAIL: Recommendations still exist (ret=%d, count=%d)\n", ret, count);
    }
}
