#include "guild_exp_ref.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void test_crud_operations(DBConnectionManager* manager) {
    printf("Testing CRUD operations...\n");

    // Test Add
    GuildExpRef ref1 = {1, 1000};
    assert(GuildExpRef_Add(manager, &ref1) == 0);
    printf("  Add: PASS\n");

    // Test Exists
    assert(GuildExpRef_Exists(manager, 1) == 1);
    printf("  Exists: PASS\n");

    // Test Get
    GuildExpRef ref2;
    assert(GuildExpRef_Get(manager, 1, &ref2) == 0);
    assert(ref2.grade == 1);
    assert(ref2.exp == 1000);
    printf("  Get: PASS\n");

    // Test Update
    ref2.exp = 2000;
    assert(GuildExpRef_Update(manager, &ref2) == 0);
    assert(GuildExpRef_Get(manager, 1, &ref2) == 0);
    assert(ref2.exp == 2000);
    printf("  Update: PASS\n");

    // Test Delete
    assert(GuildExpRef_Delete(manager, 1) == 0);
    assert(GuildExpRef_Exists(manager, 1) == 0);
    printf("  Delete: PASS\n");
}

void test_business_operations(DBConnectionManager* manager) {
    printf("Testing business operations...\n");

    // Add test data
    GuildExpRef refs[] = {
        {1, 1000},
        {2, 5000},
        {3, 10000},
        {4, 20000},
        {5, 50000}
    };

    for (int i = 0; i < 5; i++) {
        assert(GuildExpRef_Add(manager, &refs[i]) == 0);
    }

    // Test GetAll
    GuildExpRef* all_refs = NULL;
    int count = 0;
    assert(GuildExpRef_GetAll(manager, &all_refs, &count) == 0);
    assert(count == 5);
    assert(all_refs != NULL);
    free(all_refs);
    printf("  GetAll: PASS\n");

    // Test GetByExpRange
    GuildExpRef* range_refs = NULL;
    count = 0;
    assert(GuildExpRef_GetByExpRange(manager, 5000, 20000, &range_refs, &count) == 0);
    assert(count == 3); // grades 2, 3, 4
    assert(range_refs != NULL);
    free(range_refs);
    printf("  GetByExpRange: PASS\n");

    // Test GetMaxGrade
    int max_grade = 0;
    assert(GuildExpRef_GetMaxGrade(manager, &max_grade) == 0);
    assert(max_grade == 5);
    printf("  GetMaxGrade: PASS\n");

    // Cleanup
    for (int i = 0; i < 5; i++) {
        GuildExpRef_Delete(manager, refs[i].grade);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("=== Guild Exp Ref ORM Test ===\n\n");

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
