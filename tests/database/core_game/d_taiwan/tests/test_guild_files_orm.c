#include "guild_files_orm.h"
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

    printf("=== Guild Files ORM Test Suite ===\n\n");

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
        "DELETE FROM guild_files WHERE gno IN (100001, 200001)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    GuildFiles file, retrieved;
    int ret;

    printf("--- Test 1: Add Guild File ---\n");
    memset(&file, 0, sizeof(GuildFiles));
    file.gno = 100001;
    /* gf_no is auto-increment, don't set it */
    strcpy(file.file_server, "fileserver01.example.com");
    strcpy(file.file_location, "/guilds/100001/logo.png");

    ret = GuildFiles_Add(manager, &file);
    if (ret == 0) {
        printf("PASS: Added guild file successfully\n");
    } else {
        printf("FAIL: Failed to add guild file (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 2: Retrieve Guild File (by guild) ---\n");
    GuildFiles files[10];
    int count = 0;
    ret = GuildFiles_GetByGuild(manager, 100001, files, 10, &count);
    if (ret == 0 && count > 0) {
        printf("PASS: Retrieved %d file(s) for guild 100001\n", count);
        printf("First file - gf_no: %u, location: %s\n",
               files[0].gf_no, files[0].file_location);
        /* Save gf_no for later tests */
        file.gf_no = files[0].gf_no;
    } else {
        printf("FAIL: Failed to retrieve files (ret=%d, count=%d)\n", ret, count);
        return;
    }

    printf("\n--- Test 3: Get Guild File by Primary Key ---\n");
    memset(&retrieved, 0, sizeof(GuildFiles));
    ret = GuildFiles_Get(manager, 100001, file.gf_no, &retrieved);
    if (ret == 0) {
        printf("PASS: Retrieved guild file (gno=%d, gf_no=%u)\n",
               retrieved.gno, retrieved.gf_no);
        GuildFiles_PrintInfo(&retrieved);
    } else {
        printf("FAIL: Failed to retrieve guild file (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 4: Update Guild File ---\n");
    strcpy(retrieved.file_location, "/guilds/100001/logo_v2.png");
    strcpy(retrieved.file_server, "fileserver02.example.com");
    ret = GuildFiles_Update(manager, &retrieved);
    if (ret == 0) {
        printf("PASS: Updated guild file successfully\n");
    } else {
        printf("FAIL: Failed to update guild file (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 5: Verify Update ---\n");
    memset(&retrieved, 0, sizeof(GuildFiles));
    ret = GuildFiles_Get(manager, 100001, file.gf_no, &retrieved);
    if (ret == 0 && strcmp(retrieved.file_location, "/guilds/100001/logo_v2.png") == 0) {
        printf("PASS: Update verified (location=%s, server=%s)\n",
               retrieved.file_location, retrieved.file_server);
    } else {
        printf("FAIL: Update verification failed\n");
    }

    printf("\n--- Test 6: Check Exists ---\n");
    ret = GuildFiles_Exists(manager, 100001, file.gf_no);
    if (ret == 1) {
        printf("PASS: Guild file exists (gno=%d, gf_no=%u)\n", 100001, file.gf_no);
    } else {
        printf("FAIL: Exists check failed (ret=%d)\n", ret);
    }

    printf("\n--- Test 7: Delete Guild File ---\n");
    ret = GuildFiles_Delete(manager, 100001, file.gf_no);
    if (ret == 0) {
        printf("PASS: Deleted guild file successfully\n");
    } else {
        printf("FAIL: Failed to delete guild file (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 8: Verify Deletion ---\n");
    ret = GuildFiles_Exists(manager, 100001, file.gf_no);
    if (ret == 0) {
        printf("PASS: Guild file no longer exists\n");
    } else {
        printf("FAIL: Deletion verification failed (ret=%d)\n", ret);
    }
}

void test_business_operations(DBConnectionManager* manager) {
    GuildFiles files[10];
    int count = 0;
    int ret;

    printf("\n--- Test 9: Add Multiple Files for Same Guild ---\n");
    for (int i = 0; i < 3; i++) {
        GuildFiles file;
        memset(&file, 0, sizeof(GuildFiles));
        file.gno = 200001;
        strcpy(file.file_server, "fileserver03.example.com");
        snprintf(file.file_location, sizeof(file.file_location),
                 "/guilds/200001/file%d.dat", i + 1);

        ret = GuildFiles_Add(manager, &file);
        if (ret != 0) {
            printf("FAIL: Failed to add file %d (ret=%d)\n", i + 1, ret);
            return;
        }
    }
    printf("PASS: Added 3 files for guild 200001\n");

    printf("\n--- Test 10: Get All Files for Guild ---\n");
    ret = GuildFiles_GetByGuild(manager, 200001, files, 10, &count);
    if (ret == 0 && count == 3) {
        printf("PASS: Retrieved %d files for guild 200001\n", count);
        for (int i = 0; i < count; i++) {
            printf("  File %d: gf_no=%u, location=%s\n",
                   i + 1, files[i].gf_no, files[i].file_location);
        }
    } else {
        printf("FAIL: Failed to retrieve files (ret=%d, count=%d, expected=3)\n",
               ret, count);
    }

    printf("\n--- Test 11: Count Files for Guild ---\n");
    ret = GuildFiles_CountByGuild(manager, 200001, &count);
    if (ret == 0 && count == 3) {
        printf("PASS: Guild 200001 has %d files\n", count);
    } else {
        printf("FAIL: Count failed (ret=%d, count=%d, expected=3)\n", ret, count);
    }

    printf("\n--- Test 12: Update File Location ---\n");
    if (count > 0) {
        /* Get the first file */
        GuildFiles first_file;
        GuildFiles_GetByGuild(manager, 200001, &first_file, 1, &count);
        unsigned char gf_no = first_file.gf_no;

        ret = GuildFiles_UpdateLocation(manager, 200001, gf_no,
                                         "/guilds/200001/relocated.dat");
        if (ret == 0) {
            GuildFiles updated;
            GuildFiles_Get(manager, 200001, gf_no, &updated);
            if (strcmp(updated.file_location, "/guilds/200001/relocated.dat") == 0) {
                printf("PASS: File location updated successfully\n");
            } else {
                printf("FAIL: Location not updated correctly\n");
            }
        } else {
            printf("FAIL: Failed to update location (ret=%d)\n", ret);
        }
    }

    printf("\n--- Test 13: Delete All Files for Guild ---\n");
    ret = GuildFiles_DeleteByGuild(manager, 200001);
    if (ret == 0) {
        printf("PASS: Deleted all files for guild 200001\n");
    } else {
        printf("FAIL: Failed to delete files (ret=%d)\n", ret);
        return;
    }

    printf("\n--- Test 14: Verify All Files Deleted ---\n");
    ret = GuildFiles_CountByGuild(manager, 200001, &count);
    if (ret == 0 && count == 0) {
        printf("PASS: All files deleted (count=0)\n");
    } else {
        printf("FAIL: Files still exist (ret=%d, count=%d)\n", ret, count);
    }
}
