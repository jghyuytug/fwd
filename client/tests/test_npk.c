/**
 * @file test_npk.c
 * @brief NPK Parser Test Program
 *
 * Tests NPK parsing using real files from Taiwan DNF client.
 *
 * Usage: test_npk <npk_file>
 *
 * @date 2026-01-04
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/resource/npk_parser.h"

/* Test configuration */
#define DEFAULT_NPK_PATH "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2/sprite_character.NPK"

/* Callback for listing files */
static void list_file_callback(const char* filename, uint32_t size, void* userdata)
{
    int* count = (int*)userdata;
    (*count)++;

    /* Print first 20 files as sample */
    if (*count <= 20) {
        printf("  [%3d] %-60s %8u bytes\n", *count, filename, size);
    } else if (*count == 21) {
        printf("  ... (more files)\n");
    }
}

/* Test NPK opening and listing */
static int test_npk_open(const char* filepath)
{
    NPKArchive archive;
    int result;
    int file_count = 0;

    printf("\n=== Test 1: NPK Open ===\n");
    printf("File: %s\n", filepath);

    result = NPK_Open(filepath, &archive);
    if (result != NPK_SUCCESS) {
        printf("FAILED: NPK_Open returned %d\n", result);
        return -1;
    }

    printf("SUCCESS: NPK opened\n");
    printf("  File count: %u\n", archive.file_count);
    printf("  Total data size: %u bytes (%.2f MB)\n",
           archive.total_data_size,
           archive.total_data_size / (1024.0 * 1024.0));
    printf("  Encrypted: %s\n", archive.is_encrypted ? "Yes" : "No");

    /* List files */
    printf("\n=== Test 2: File Listing ===\n");
    NPK_ListFiles(&archive, list_file_callback, &file_count);
    printf("Total files listed: %d\n", file_count);

    /* Test file lookup */
    printf("\n=== Test 3: File Lookup ===\n");
    if (archive.file_count > 0) {
        NPKFileEntry* first = NPK_GetEntry(&archive, 0);
        if (first) {
            printf("First entry name: %s\n", first->name);
            printf("First entry size: %u bytes\n", first->size);
            printf("First entry offset: 0x%08X\n", first->offset);

            /* Try to find it by name */
            NPKFileEntry* found = NPK_FindFile(&archive, first->name);
            if (found) {
                printf("Lookup by name: SUCCESS (size=%u)\n", found->size);
            } else {
                printf("Lookup by name: FAILED\n");
            }
        }
    }

    /* Test file reading */
    printf("\n=== Test 4: File Reading ===\n");
    if (archive.file_count > 0) {
        NPKFileEntry* entry = NPK_GetEntry(&archive, 0);
        if (entry && entry->size > 0 && entry->size < 10 * 1024 * 1024) {
            void* buffer = malloc(entry->size);
            if (buffer) {
                result = NPK_ReadFile(&archive, entry, buffer, entry->size);
                if (result == NPK_SUCCESS) {
                    /* Print first 32 bytes as hex */
                    printf("Read SUCCESS: %u bytes\n", entry->size);
                    printf("First 32 bytes: ");
                    uint8_t* data = (uint8_t*)buffer;
                    int show = entry->size < 32 ? entry->size : 32;
                    for (int i = 0; i < show; i++) {
                        printf("%02X ", data[i]);
                    }
                    printf("\n");

                    /* Check for IMG magic */
                    if (entry->size >= 18) {
                        if (strncmp((char*)data, "Neople Img File", 15) == 0 ||
                            strncmp((char*)data, "Neople Image File", 17) == 0) {
                            printf("Detected: IMG file format!\n");
                        }
                    }
                } else {
                    printf("Read FAILED: error %d\n", result);
                }
                free(buffer);
            }
        }
    }

    /* Cleanup */
    NPK_Close(&archive);
    printf("\n=== Test Complete ===\n");

    return 0;
}

/* Test decrypt key */
static void test_decrypt_key(void)
{
    printf("\n=== Test 0: Decrypt Key ===\n");

    NPK_Initialize();
    const uint8_t* key = NPK_GetDecryptKey();

    printf("Key (first 64 bytes):\n  ");
    for (int i = 0; i < 64; i++) {
        if (key[i] >= 32 && key[i] < 127) {
            printf("%c", key[i]);
        } else {
            printf(".");
        }
    }
    printf("\n");

    printf("Key hex (first 48 bytes):\n  ");
    for (int i = 0; i < 48; i++) {
        printf("%02X ", key[i]);
        if ((i + 1) % 16 == 0) printf("\n  ");
    }
    printf("\n");
}

/* Test multiple NPK files */
static void test_multiple_npks(void)
{
    const char* test_files[] = {
        "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2/sprite_character.NPK",
        "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2/sprite.NPK",
        "C:/Users/waw/Desktop/地下城与勇士台服/imagepacks2/sprite_character_common.NPK",
        NULL
    };

    printf("\n=== Multi-NPK Test ===\n");

    for (int i = 0; test_files[i] != NULL; i++) {
        NPKArchive archive;
        int result = NPK_Open(test_files[i], &archive);

        const char* basename = strrchr(test_files[i], '/');
        basename = basename ? basename + 1 : test_files[i];

        if (result == NPK_SUCCESS) {
            printf("[OK] %-50s %4u files, %8u bytes\n",
                   basename, archive.file_count, archive.total_data_size);
            NPK_Close(&archive);
        } else {
            printf("[FAIL] %-50s error=%d\n", basename, result);
        }
    }
}

int main(int argc, char* argv[])
{
    const char* npk_path = DEFAULT_NPK_PATH;

    printf("========================================\n");
    printf("  NPK Parser Test Program\n");
    printf("========================================\n");

    if (argc > 1) {
        npk_path = argv[1];
    }

    /* Test decrypt key first */
    test_decrypt_key();

    /* Test main NPK file */
    int result = test_npk_open(npk_path);

    /* Test multiple files */
    if (result == 0) {
        test_multiple_npks();
    }

    printf("\n");
    return result;
}
