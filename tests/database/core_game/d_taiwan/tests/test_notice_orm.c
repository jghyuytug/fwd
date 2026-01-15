#include "notice_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* IMPORTANT: NO Chinese characters in test files! */

static void test_basic_crud(DBConnectionManager* manager);
static void test_business_operations(DBConnectionManager* manager);
static void cleanup_test_data(DBConnectionManager* manager);

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

    printf("=== Notice ORM Test Suite ===\n\n");

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
        "DELETE FROM notice WHERE bbs_name = 'test_bbs'");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    Notice notice;
    int result;

    printf("[Test 1] CRUD Operations\n");

    /* Test Add */
    printf("  1.1 Add notice... ");
    memset(&notice, 0, sizeof(Notice));
    strcpy(notice.bbs_name, "test_bbs");
    notice.category = 1;
    strcpy(notice.m_nickname, "TestUser");
    notice.m_id = 100001;
    strcpy(notice.m_user_id, "testuser001");
    notice.m_sex = 'm';
    strcpy(notice.title, "Test Notice Title");
    notice.create_day = (unsigned int)time(NULL);
    notice.comment = 0;
    notice.view = 0;
    notice.recom = 0;
    notice.adorn = 0;
    notice.adorn_color1 = 0;
    notice.adorn_color2 = 0;
    notice.depth = 0;
    notice.sequence = 1.0;
    notice.content = (char*)malloc(100);
    strcpy(notice.content, "This is a test notice content");
    strcpy(notice.content_type, "text");
    strcpy(notice.ip, "127.0.0.1");
    notice.ring = 0;
    notice.sms = 'n';

    result = Notice_Add(manager, &notice);
    if (result == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }
    free(notice.content);

    /* Test Add second notice */
    printf("  1.2 Add second notice... ");
    memset(&notice, 0, sizeof(Notice));
    strcpy(notice.bbs_name, "test_bbs");
    notice.category = 2;
    strcpy(notice.m_nickname, "TestUser2");
    notice.m_id = 100002;
    strcpy(notice.m_user_id, "testuser002");
    notice.m_sex = 'f';
    strcpy(notice.title, "Second Test Notice");
    notice.create_day = (unsigned int)time(NULL);
    notice.sequence = 2.0;
    notice.content = (char*)malloc(100);
    strcpy(notice.content, "Second test content");
    strcpy(notice.content_type, "br");
    strcpy(notice.ip, "192.168.1.1");
    notice.sms = 'y';

    result = Notice_Add(manager, &notice);
    if (result == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }
    free(notice.content);

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    Notice notices[10];
    int actual_count;
    int result;
    int i;

    printf("[Test 2] Business Operations\n");

    /* Test GetByBbsName */
    printf("  2.1 Get notices by bbs_name=test_bbs... ");
    result = Notice_GetByBbsName(manager, "test_bbs", notices, 10, &actual_count);
    if (result == 0 && actual_count >= 2) {
        printf("PASS (found %d notices)\n", actual_count);
        for (i = 0; i < actual_count; i++) {
            printf("      - Notice %u: %s by %s\n",
                   notices[i].no, notices[i].title, notices[i].m_nickname);
            Notice_Free(&notices[i]);
        }
    } else {
        printf("FAIL (result=%d, count=%d)\n", result, actual_count);
    }

    /* Test PrintInfo on first notice if exists */
    if (actual_count > 0) {
        printf("  2.2 Print first notice info:\n");
        result = Notice_GetByBbsName(manager, "test_bbs", notices, 1, &actual_count);
        if (result == 0 && actual_count > 0) {
            Notice_PrintInfo(&notices[0]);
            Notice_Free(&notices[0]);
        }
    }

    printf("\n");
}
