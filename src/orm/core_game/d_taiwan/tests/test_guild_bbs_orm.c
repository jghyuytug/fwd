#include "guild_bbs_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

    printf("=== Guild BBS ORM Test Suite ===\n\n");

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
        "DELETE FROM guild_bbs WHERE m_id IN (100001, 200001)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    GuildBbs bbs, retrieved;
    int ret;
    unsigned int current_time = (unsigned int)time(NULL);

    printf("--- Test 1: Add Guild BBS Post ---\n");
    memset(&bbs, 0, sizeof(GuildBbs));
    bbs.bd_id = 1;
    bbs.empyn = 0;
    bbs.mgno = 1001;
    bbs.open = 1;
    bbs.main = 1;
    bbs.reg_date = current_time;
    bbs.mod_date = current_time;
    bbs.hits = 0;
    bbs.body_type = 'T';
    bbs.m_id = 100001;
    strcpy(bbs.reg_id, "testuser01");
    strcpy(bbs.subject, "Welcome to guild forum");

    ret = GuildBbs_Add(manager, &bbs);
    if (ret == 0) {
        printf("PASS: Added guild BBS post successfully\n");
    } else {
        printf("FAIL: Failed to add guild BBS post (ret=%d)\n", ret);
        return;
    }

    /* Note: gno is auto-generated, need to retrieve to get the actual value */
    printf("\n--- Test 2: Get Guild BBS Post (retrieve by board) ---\n");
    GuildBbs posts[10];
    int count = 0;
    ret = GuildBbs_GetByBoard(manager, 1, posts, 10, &count);
    if (ret == 0 && count > 0) {
        printf("PASS: Retrieved %d posts from board\n", count);
        printf("First post subject: %s\n", posts[0].subject);
        /* Save gno for later tests */
        bbs.gno = posts[0].gno;
    } else {
        printf("FAIL: Failed to retrieve posts (ret=%d, count=%d)\n", ret, count);
        return;
    }

    printf("\n--- Test 3: Get Guild BBS Post by gno ---\n");
    memset(&retrieved, 0, sizeof(GuildBbs));
    ret = GuildBbs_Get(manager, bbs.gno, &retrieved);
    if (ret == 0) {
        printf("PASS: Retrieved guild BBS post (gno=%u)\n", retrieved.gno);
        GuildBbs_PrintInfo(&retrieved);
    } else {
        printf("FAIL: Failed to retrieve guild BBS post (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 4: Update Guild BBS Post ---\n");
    strcpy(retrieved.subject, "Updated forum topic");
    retrieved.hits = 10;
    ret = GuildBbs_Update(manager, &retrieved);
    if (ret == 0) {
        printf("PASS: Updated guild BBS post successfully\n");
    } else {
        printf("FAIL: Failed to update guild BBS post (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 5: Verify Update ---\n");
    memset(&retrieved, 0, sizeof(GuildBbs));
    ret = GuildBbs_Get(manager, bbs.gno, &retrieved);
    if (ret == 0 && strcmp(retrieved.subject, "Updated forum topic") == 0) {
        printf("PASS: Update verified (subject=%s, hits=%u)\n",
               retrieved.subject, retrieved.hits);
    } else {
        printf("FAIL: Update verification failed\n");
    }

    printf("\n--- Test 6: Check Exists ---\n");
    ret = GuildBbs_Exists(manager, bbs.gno);
    if (ret == 1) {
        printf("PASS: Guild BBS post exists (gno=%u)\n", bbs.gno);
    } else {
        printf("FAIL: Exists check failed (ret=%d)\n", ret);
    }

    printf("\n--- Test 7: Delete Guild BBS Post ---\n");
    ret = GuildBbs_Delete(manager, bbs.gno);
    if (ret == 0) {
        printf("PASS: Deleted guild BBS post successfully\n");
    } else {
        printf("FAIL: Failed to delete guild BBS post (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 8: Verify Deletion ---\n");
    ret = GuildBbs_Exists(manager, bbs.gno);
    if (ret == 0) {
        printf("PASS: Guild BBS post no longer exists\n");
    } else {
        printf("FAIL: Deletion verification failed (ret=%d)\n", ret);
    }
}

void test_business_operations(DBConnectionManager* manager) {
    GuildBbs posts[10];
    int count = 0;
    int ret;
    unsigned int current_time = (unsigned int)time(NULL);

    printf("\n--- Test 9: Add Multiple Posts ---\n");
    for (int i = 0; i < 3; i++) {
        GuildBbs bbs;
        memset(&bbs, 0, sizeof(GuildBbs));
        bbs.bd_id = 2;
        bbs.empyn = (i % 2);
        bbs.mgno = 2001 + i;
        bbs.open = 1;
        bbs.main = (i == 0) ? 1 : 0;
        bbs.reg_date = current_time + i;
        bbs.mod_date = current_time + i;
        bbs.hits = i * 5;
        bbs.body_type = 'T';
        bbs.m_id = 200001;
        strcpy(bbs.reg_id, "testuser02");
        snprintf(bbs.subject, sizeof(bbs.subject), "Topic %d", i + 1);

        ret = GuildBbs_Add(manager, &bbs);
        if (ret != 0) {
            printf("FAIL: Failed to add post %d (ret=%d)\n", i + 1, ret);
            return;
        }
    }
    printf("PASS: Added 3 posts to board 2\n");

    printf("\n--- Test 10: Get Posts by Board ---\n");
    ret = GuildBbs_GetByBoard(manager, 2, posts, 10, &count);
    if (ret == 0 && count == 3) {
        printf("PASS: Retrieved %d posts from board 2\n", count);
        for (int i = 0; i < count; i++) {
            printf("  Post %d: gno=%u, subject=%s, hits=%u\n",
                   i + 1, posts[i].gno, posts[i].subject, posts[i].hits);
        }
    } else {
        printf("FAIL: Failed to retrieve posts (ret=%d, count=%d, expected=3)\n",
               ret, count);
    }

    printf("\n--- Test 11: Get Posts by Board and Empyn ---\n");
    ret = GuildBbs_GetByBoardAndEmpyn(manager, 2, 0, posts, 10, &count);
    if (ret == 0) {
        printf("PASS: Retrieved %d posts with empyn=0\n", count);
        for (int i = 0; i < count; i++) {
            printf("  Post %d: gno=%u, empyn=%u\n",
                   i + 1, posts[i].gno, posts[i].empyn);
        }
    } else {
        printf("FAIL: Failed to retrieve posts by empyn (ret=%d)\n", ret);
    }

    printf("\n--- Test 12: Get Posts by Member ---\n");
    ret = GuildBbs_GetByMember(manager, 200001, posts, 10, &count);
    if (ret == 0 && count == 3) {
        printf("PASS: Retrieved %d posts by member 200001\n", count);
    } else {
        printf("FAIL: Failed to retrieve posts by member (ret=%d, count=%d)\n",
               ret, count);
    }

    printf("\n--- Test 13: Increment Hits ---\n");
    if (count > 0) {
        unsigned int gno = posts[0].gno;
        unsigned int old_hits = posts[0].hits;

        ret = GuildBbs_IncrementHits(manager, gno);
        if (ret == 0) {
            GuildBbs updated;
            GuildBbs_Get(manager, gno, &updated);
            if (updated.hits == old_hits + 1) {
                printf("PASS: Hits incremented (old=%u, new=%u)\n",
                       old_hits, updated.hits);
            } else {
                printf("FAIL: Hits not incremented correctly (old=%u, new=%u)\n",
                       old_hits, updated.hits);
            }
        } else {
            printf("FAIL: Failed to increment hits (ret=%d)\n", ret);
        }
    }

    printf("\n--- Test 14: Update Subject ---\n");
    if (count > 0) {
        unsigned int gno = posts[0].gno;
        const char* new_subject = "Modified subject title";

        ret = GuildBbs_UpdateSubject(manager, gno, new_subject);
        if (ret == 0) {
            GuildBbs updated;
            GuildBbs_Get(manager, gno, &updated);
            if (strcmp(updated.subject, new_subject) == 0) {
                printf("PASS: Subject updated successfully\n");
            } else {
                printf("FAIL: Subject not updated correctly\n");
            }
        } else {
            printf("FAIL: Failed to update subject (ret=%d)\n", ret);
        }
    }
}
