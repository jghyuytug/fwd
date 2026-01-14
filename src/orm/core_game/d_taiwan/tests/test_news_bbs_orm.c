#include "news_bbs_orm.h"
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
    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== News BBS ORM Test Suite ===\n\n");

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
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM news_bbs WHERE user_id IN ('testuser1', 'testuser2')");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    NewsBbs bbs;
    NewsBbs retrieved;
    int current_time = (int)time(NULL);

    printf("[Test 1] Add News BBS\n");
    memset(&bbs, 0, sizeof(NewsBbs));
    bbs.bbs_code = 1;
    bbs.emph_yn = 0;
    strncpy(bbs.user_id, "testuser1", sizeof(bbs.user_id) - 1);
    bbs.reg_date = current_time;
    bbs.html_yn = 1;
    strncpy(bbs.subject, "Test Post Subject", sizeof(bbs.subject) - 1);
    strncpy(bbs.body, "This is a test post body content.", sizeof(bbs.body) - 1);
    bbs.hits = 0;
    bbs.prev_no = 0;
    bbs.next_no = 0;
    bbs.updt_date = 0;
    bbs.use_yn = 1;
    strncpy(bbs.file_name, "test.jpg", sizeof(bbs.file_name) - 1);

    if (NewsBbs_Add(manager, &bbs) < 0) {
        printf("  FAILED: Could not add news bbs\n");
        return;
    }
    printf("  PASSED: News bbs added successfully\n");

    /* Get the auto-generated no by querying */
    NewsBbs temp_list[10];
    int count = 0;
    if (NewsBbs_GetByUserId(manager, "testuser1", temp_list, 10, &count) < 0 || count == 0) {
        printf("  FAILED: Could not retrieve added record\n");
        return;
    }
    unsigned int test_no = temp_list[0].no;
    printf("  Auto-generated no: %u\n", test_no);

    printf("\n[Test 2] Get News BBS by No\n");
    memset(&retrieved, 0, sizeof(NewsBbs));
    if (NewsBbs_GetByNo(manager, test_no, &retrieved) < 0) {
        printf("  FAILED: Could not get news bbs\n");
        return;
    }
    if (strcmp(retrieved.subject, "Test Post Subject") != 0) {
        printf("  FAILED: Subject mismatch\n");
        return;
    }
    printf("  PASSED: News bbs retrieved correctly\n");
    printf("  Subject: %s\n", retrieved.subject);
    printf("  User ID: %s\n", retrieved.user_id);

    printf("\n[Test 3] Update News BBS\n");
    strncpy(retrieved.subject, "Updated Subject", sizeof(retrieved.subject) - 1);
    retrieved.hits = 10;
    retrieved.updt_date = current_time + 3600;

    if (NewsBbs_Update(manager, &retrieved) < 0) {
        printf("  FAILED: Could not update news bbs\n");
        return;
    }
    printf("  PASSED: News bbs updated successfully\n");

    memset(&retrieved, 0, sizeof(NewsBbs));
    if (NewsBbs_GetByNo(manager, test_no, &retrieved) < 0) {
        printf("  FAILED: Could not verify update\n");
        return;
    }
    if (strcmp(retrieved.subject, "Updated Subject") != 0 || retrieved.hits != 10) {
        printf("  FAILED: Update verification failed\n");
        return;
    }
    printf("  Verified: Subject updated to '%s', hits = %u\n",
           retrieved.subject, retrieved.hits);

    printf("\n[Test 4] Exists Check\n");
    if (!NewsBbs_Exists(manager, test_no)) {
        printf("  FAILED: Exists check failed for valid record\n");
        return;
    }
    printf("  PASSED: Exists check confirmed record\n");

    if (NewsBbs_Exists(manager, 999999)) {
        printf("  FAILED: Exists returned true for non-existent record\n");
        return;
    }
    printf("  PASSED: Exists check rejected non-existent record\n");

    printf("\n[Test 5] Delete News BBS\n");
    if (NewsBbs_Delete(manager, test_no) < 0) {
        printf("  FAILED: Could not delete news bbs\n");
        return;
    }
    printf("  PASSED: News bbs deleted successfully\n");

    if (NewsBbs_Exists(manager, test_no)) {
        printf("  FAILED: Record still exists after deletion\n");
        return;
    }
    printf("  PASSED: Delete verification successful\n");
}

void test_business_operations(DBConnectionManager* manager) {
    NewsBbs bbs_list[10];
    int count = 0;
    int current_time = (int)time(NULL);

    printf("\n[Test 6] Add Multiple Posts\n");

    /* Add post 1 */
    NewsBbs bbs1;
    memset(&bbs1, 0, sizeof(NewsBbs));
    bbs1.bbs_code = 1;
    bbs1.emph_yn = 1;
    strncpy(bbs1.user_id, "testuser1", sizeof(bbs1.user_id) - 1);
    bbs1.reg_date = current_time;
    bbs1.html_yn = 0;
    strncpy(bbs1.subject, "First Post", sizeof(bbs1.subject) - 1);
    strncpy(bbs1.body, "This is the first test post.", sizeof(bbs1.body) - 1);
    bbs1.use_yn = 1;

    if (NewsBbs_Add(manager, &bbs1) < 0) {
        printf("  FAILED: Could not add first post\n");
        return;
    }

    /* Add post 2 */
    NewsBbs bbs2;
    memset(&bbs2, 0, sizeof(NewsBbs));
    bbs2.bbs_code = 1;
    bbs2.emph_yn = 0;
    strncpy(bbs2.user_id, "testuser2", sizeof(bbs2.user_id) - 1);
    bbs2.reg_date = current_time + 60;
    bbs2.html_yn = 1;
    strncpy(bbs2.subject, "Second Post", sizeof(bbs2.subject) - 1);
    strncpy(bbs2.body, "This is the second test post.", sizeof(bbs2.body) - 1);
    bbs2.use_yn = 1;

    if (NewsBbs_Add(manager, &bbs2) < 0) {
        printf("  FAILED: Could not add second post\n");
        return;
    }

    /* Add post 3 */
    NewsBbs bbs3;
    memset(&bbs3, 0, sizeof(NewsBbs));
    bbs3.bbs_code = 2;
    bbs3.emph_yn = 0;
    strncpy(bbs3.user_id, "testuser1", sizeof(bbs3.user_id) - 1);
    bbs3.reg_date = current_time + 120;
    bbs3.html_yn = 0;
    strncpy(bbs3.subject, "Third Post", sizeof(bbs3.subject) - 1);
    strncpy(bbs3.body, "This is the third test post.", sizeof(bbs3.body) - 1);
    bbs3.use_yn = 1;

    if (NewsBbs_Add(manager, &bbs3) < 0) {
        printf("  FAILED: Could not add third post\n");
        return;
    }
    printf("  PASSED: Added 3 test posts\n");

    printf("\n[Test 7] Get by BBS Code\n");
    memset(bbs_list, 0, sizeof(bbs_list));
    if (NewsBbs_GetByBbsCode(manager, 1, bbs_list, 10, &count) < 0) {
        printf("  FAILED: Could not get by bbs_code\n");
        return;
    }
    printf("  PASSED: Retrieved %d posts for bbs_code 1\n", count);
    if (count >= 2) {
        printf("  Post 1: %s (user: %s)\n", bbs_list[0].subject, bbs_list[0].user_id);
        printf("  Post 2: %s (user: %s)\n", bbs_list[1].subject, bbs_list[1].user_id);
    }

    printf("\n[Test 8] Get by User ID\n");
    memset(bbs_list, 0, sizeof(bbs_list));
    if (NewsBbs_GetByUserId(manager, "testuser1", bbs_list, 10, &count) < 0) {
        printf("  FAILED: Could not get by user_id\n");
        return;
    }
    printf("  PASSED: Retrieved %d posts for testuser1\n", count);
    for (int i = 0; i < count && i < 10; i++) {
        printf("  Post %d: %s (bbs_code: %u)\n",
               i + 1, bbs_list[i].subject, bbs_list[i].bbs_code);
    }

    printf("\n[Test 9] Increment Hits\n");
    if (count > 0) {
        unsigned int test_no = bbs_list[0].no;
        unsigned short old_hits = bbs_list[0].hits;

        if (NewsBbs_IncrementHits(manager, test_no) < 0) {
            printf("  FAILED: Could not increment hits\n");
            return;
        }

        NewsBbs updated;
        if (NewsBbs_GetByNo(manager, test_no, &updated) < 0) {
            printf("  FAILED: Could not verify hit increment\n");
            return;
        }

        if (updated.hits != old_hits + 1) {
            printf("  FAILED: Hits not incremented correctly\n");
            return;
        }
        printf("  PASSED: Hits incremented from %u to %u\n", old_hits, updated.hits);
    }

    printf("\n[Test 10] Set Prev/Next Links\n");
    if (count >= 2) {
        unsigned int no1 = bbs_list[0].no;
        unsigned int no2 = bbs_list[1].no;

        if (NewsBbs_SetPrevNext(manager, no1, 0, (int)no2) < 0) {
            printf("  FAILED: Could not set prev/next for first post\n");
            return;
        }

        if (NewsBbs_SetPrevNext(manager, no2, (int)no1, 0) < 0) {
            printf("  FAILED: Could not set prev/next for second post\n");
            return;
        }

        NewsBbs check;
        if (NewsBbs_GetByNo(manager, no1, &check) < 0) {
            printf("  FAILED: Could not verify prev/next\n");
            return;
        }

        if (check.next_no != (int)no2) {
            printf("  FAILED: Next link not set correctly\n");
            return;
        }
        printf("  PASSED: Prev/next links set correctly\n");
        printf("  Post 1 next_no: %d, Post 2 no: %u\n", check.next_no, no2);
    }

    printf("\n[Test 11] Get All Posts\n");
    memset(bbs_list, 0, sizeof(bbs_list));
    if (NewsBbs_GetAll(manager, bbs_list, 10, &count) < 0) {
        printf("  FAILED: Could not get all posts\n");
        return;
    }
    printf("  PASSED: Retrieved %d total posts (limited to 10)\n", count);
}
