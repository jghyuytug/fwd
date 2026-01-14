/**
 * @file test_charac_quest_orm.c
 * @brief Test suite for Character Quest ORM module
 *
 * Database: test_taiwan_cain (DB_TYPE_CAIN = 2)
 * Comprehensive tests for quest system management
 */

#include "charac_quest_orm.h"
#include "db_connection_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ANSI color codes */
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_RESET "\033[0m"

/* Print macros */
#define print_test(msg) printf(COLOR_BLUE "[TEST] %s" COLOR_RESET "\n", msg)
#define print_success(msg) printf(COLOR_GREEN "[PASS] %s" COLOR_RESET "\n", msg)
#define print_error(msg) printf(COLOR_RED "[FAIL] %s" COLOR_RESET "\n", msg)
#define print_info(msg) printf(COLOR_YELLOW "[INFO] %s" COLOR_RESET "\n", msg)

/* Test helper - initialize quest data with test values */
void init_test_quest(CharacQuest* quest, int charac_no) {
    int i;

    memset(quest, 0, sizeof(CharacQuest));
    quest->charac_no = charac_no;

    /* Set some test data in quest categories */
    for (i = 0; i < QUEST_DATA_SIZE; i++) {
        quest->quest_10[i] = (unsigned char)(i % 256);
        quest->quest_20[i] = (unsigned char)((i + 10) % 256);
        quest->quest_30[i] = (unsigned char)((i + 20) % 256);
    }

    /* Set play slots */
    quest->play_1 = 100;
    quest->play_1_trigger = 1000;
    quest->play_2 = 200;
    quest->play_2_trigger = 2000;
    quest->play_3 = 300;
    quest->play_3_trigger = 3000;
}

/* Test 1: Create quest records */
int test_create_quests(DBConnectionManager* manager) {
    CharacQuest quest1, quest2;
    int ret;

    print_test("Test 1: Create Quest Records");

    init_test_quest(&quest1, 10001);
    init_test_quest(&quest2, 10002);

    /* Modify quest2 to have different values */
    quest2.play_1 = 500;
    quest2.play_1_trigger = 5000;

    ret = CharacQuest_Create(manager, &quest1);
    if (ret != 0) {
        print_error("Failed to create quest1");
        return 0;
    }

    ret = CharacQuest_Create(manager, &quest2);
    if (ret != 0) {
        print_error("Failed to create quest2");
        return 0;
    }

    print_success("Created 2 quest records");
    return 1;
}

/* Test 2: Query quest records */
int test_query_quests(DBConnectionManager* manager) {
    CharacQuest quest;
    int ret;
    int i;
    int data_ok = 1;

    print_test("Test 2: Query Quest Records");

    ret = CharacQuest_GetByCharacNo(manager, 10001, &quest);
    if (ret != 0) {
        print_error("Failed to query quest for charac_no=10001");
        return 0;
    }

    /* Verify basic data */
    if (quest.charac_no != 10001) {
        print_error("Character number mismatch");
        return 0;
    }

    if (quest.play_1 != 100 || quest.play_1_trigger != 1000) {
        print_error("Play slot 1 data mismatch");
        printf("  Expected: play_1=100, trigger=1000\n");
        printf("  Got: play_1=%u, trigger=%d\n", quest.play_1, quest.play_1_trigger);
        return 0;
    }

    /* Verify some binary quest data */
    for (i = 0; i < 10 && data_ok; i++) {
        if (quest.quest_10[i] != (unsigned char)(i % 256)) {
            data_ok = 0;
            printf("  Quest data mismatch at byte %d\n", i);
        }
    }

    if (!data_ok) {
        print_error("Quest binary data verification failed");
        return 0;
    }

    print_success("Query quest data verified successfully");
    return 1;
}

/* Test 3: Update quest record */
int test_update_quest(DBConnectionManager* manager) {
    CharacQuest quest;
    int ret;

    print_test("Test 3: Update Quest Record");

    /* Get existing quest */
    ret = CharacQuest_GetByCharacNo(manager, 10002, &quest);
    if (ret != 0) {
        print_error("Failed to get quest for update");
        return 0;
    }

    /* Update play slots */
    quest.play_2 = 999;
    quest.play_2_trigger = 9999;
    quest.play_3 = 888;
    quest.play_3_trigger = 8888;

    ret = CharacQuest_Update(manager, &quest);
    if (ret != 0) {
        print_error("Failed to update quest");
        return 0;
    }

    /* Verify update */
    ret = CharacQuest_GetByCharacNo(manager, 10002, &quest);
    if (ret != 0 || quest.play_2 != 999 || quest.play_2_trigger != 9999) {
        print_error("Update verification failed");
        return 0;
    }

    print_success("Quest updated successfully");
    return 1;
}

/* Test 4: Existence check */
int test_existence_check(DBConnectionManager* manager) {
    int ret;

    print_test("Test 4: Existence Check");

    ret = CharacQuest_Exists(manager, 10001);
    if (ret != 1) {
        print_error("Existing record not found");
        return 0;
    }

    ret = CharacQuest_Exists(manager, 99999);
    if (ret != 0) {
        print_error("Non-existing record found");
        return 0;
    }

    print_success("Existence checks passed");
    return 1;
}

/* Test 5: Update category */
int test_update_category(DBConnectionManager* manager) {
    CharacQuest quest;
    unsigned char test_data[QUEST_DATA_SIZE];
    int ret;
    int i;

    print_test("Test 5: Update Quest Category");

    /* Prepare test data */
    for (i = 0; i < QUEST_DATA_SIZE; i++) {
        test_data[i] = (unsigned char)(255 - i);
    }

    /* Update quest_40 category */
    ret = CharacQuest_UpdateCategory(manager, 10001, "quest_40", test_data);
    if (ret != 0) {
        print_error("Failed to update category");
        return 0;
    }

    /* Verify */
    ret = CharacQuest_GetByCharacNo(manager, 10001, &quest);
    if (ret != 0) {
        print_error("Failed to get quest for verification");
        return 0;
    }

    for (i = 0; i < 10; i++) {
        if (quest.quest_40[i] != test_data[i]) {
            print_error("Category data mismatch");
            return 0;
        }
    }

    print_success("Quest category updated successfully");
    return 1;
}

/* Test 6: Update play slot */
int test_update_play_slot(DBConnectionManager* manager) {
    CharacQuest quest;
    int ret;

    print_test("Test 6: Update Play Slot");

    ret = CharacQuest_UpdatePlaySlot(manager, 10001, 5, 777, 7777);
    if (ret != 0) {
        print_error("Failed to update play slot");
        return 0;
    }

    /* Verify */
    ret = CharacQuest_GetByCharacNo(manager, 10001, &quest);
    if (ret != 0 || quest.play_5 != 777 || quest.play_5_trigger != 7777) {
        print_error("Play slot update verification failed");
        return 0;
    }

    print_success("Play slot updated successfully");
    return 1;
}

/* Test 7: Get total count */
int test_get_total_count(DBConnectionManager* manager) {
    int count;

    print_test("Test 7: Get Total Count");

    count = CharacQuest_GetTotalCount(manager);
    if (count < 0) {
        print_error("Failed to get total count");
        return 0;
    }

    printf("  Total quest records: %d\n", count);
    if (count != 2) {
        print_error("Total count mismatch (expected 2)");
        return 0;
    }

    print_success("Total count query successful");
    return 1;
}

/* Test 8: Clear all quest data */
int test_clear_all(DBConnectionManager* manager) {
    CharacQuest quest;
    int ret;
    int i;
    int all_zero = 1;

    print_test("Test 8: Clear All Quest Data");

    ret = CharacQuest_ClearAll(manager, 10002);
    if (ret != 0) {
        print_error("Failed to clear quest data");
        return 0;
    }

    /* Verify all data is zero */
    ret = CharacQuest_GetByCharacNo(manager, 10002, &quest);
    if (ret != 0) {
        print_error("Failed to get quest after clear");
        return 0;
    }

    /* Check play slots are all zero */
    if (quest.play_1 != 0 || quest.play_1_trigger != 0 ||
        quest.play_2 != 0 || quest.play_2_trigger != 0) {
        all_zero = 0;
    }

    /* Check some binary data is zero */
    for (i = 0; i < QUEST_DATA_SIZE && all_zero; i++) {
        if (quest.quest_10[i] != 0) {
            all_zero = 0;
        }
    }

    if (!all_zero) {
        print_error("Quest data not fully cleared");
        return 0;
    }

    print_success("Quest data cleared successfully");
    return 1;
}

/* Test 9: Get by play value */
int test_get_by_play_value(DBConnectionManager* manager) {
    CharacQuest results[10];
    int ret;

    print_test("Test 9: Get By Play Value");

    ret = CharacQuest_GetByPlayValue(manager, 5, 777, results, 10);
    if (ret < 0) {
        print_error("Failed to get by play value");
        return 0;
    }

    printf("  Found %d records with play_5=777\n", ret);
    if (ret > 0) {
        printf("    CharacNo=%d, play_5=%u, trigger=%d\n",
               results[0].charac_no, results[0].play_5, results[0].play_5_trigger);
    }

    print_success("Get by play value successful");
    return 1;
}

/* Test 10: Delete quest */
int test_delete_quest(DBConnectionManager* manager) {
    int ret;

    print_test("Test 10: Delete Quest");

    ret = CharacQuest_Delete(manager, 10002);
    if (ret != 0) {
        print_error("Failed to delete quest");
        return 0;
    }

    /* Verify deletion */
    ret = CharacQuest_Exists(manager, 10002);
    if (ret != 0) {
        print_error("Quest still exists after deletion");
        return 0;
    }

    print_success("Quest deleted successfully");
    return 1;
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;
    const char* config_file;
    int passed = 0;
    int total = 10;

    printf("\n");
    printf("========================================\n");
    printf("  Character Quest ORM Test Suite\n");
    printf("  Database: test_taiwan_cain\n");
    printf("========================================\n\n");

    if (argc < 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    config_file = argv[1];
    printf("Config file: %s\n\n", config_file);

    /* Clear structure */
    memset(&manager, 0, sizeof(DBConnectionManager));

    /* Initialize connection manager */
    print_info("Initializing database connection...");
    if (DBConnectionManager_Initialize(&manager, config_file) != 0) {
        print_error("Failed to initialize database connection manager");
        return 1;
    }

    /* Connect to database */
    print_info("Connecting to test_taiwan_cain database...");
    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) != 0) {
        print_error("Failed to connect to test database");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    print_info("Database connection established");
    printf("\n");

    /* Clean up old test data */
    {
        DBQueryResult cleanup_result;
        memset(&cleanup_result, 0, sizeof(DBQueryResult));
        DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_CAIN,
                                          "DELETE FROM charac_quest WHERE charac_no IN (10001,10002)", &cleanup_result);
        DBQueryResult_Free(&cleanup_result);
    }

    /* Run tests */
    passed += test_create_quests(&manager);
    passed += test_query_quests(&manager);
    passed += test_update_quest(&manager);
    passed += test_existence_check(&manager);
    passed += test_update_category(&manager);
    passed += test_update_play_slot(&manager);
    passed += test_get_total_count(&manager);
    passed += test_clear_all(&manager);
    passed += test_get_by_play_value(&manager);
    passed += test_delete_quest(&manager);

    /* Cleanup */
    DBConnectionManager_Cleanup(&manager);

    /* Print results */
    printf("\n");
    printf("========================================\n");
    if (passed == total) {
        printf(COLOR_GREEN "  Test Result: %d/%d Passed\n" COLOR_RESET, passed, total);
        printf("========================================\n\n");
        return 0;
    } else {
        printf(COLOR_RED "  Test Result: %d/%d Passed\n" COLOR_RESET, passed, total);
        printf("========================================\n\n");
        return 1;
    }
}
