/**
 * @file npk_parser.c
 * @brief DNF NPK Archive Parser Implementation
 *
 * Based on reverse engineering from:
 * - pvfUtility/NpkOperation/NpkPraser.cs
 * - Community documentation (CSDN, GitHub)
 *
 * Key discovery:
 * - Decrypt key: "puchikon@neople dungeon and fighter DNF" + "DNF" padding
 * - File entries: 264 bytes each (offset 4B + size 4B + encrypted_name 256B)
 * - Magic header: "NeoplePack_Bill" (16 bytes)
 *
 * @date 2026-01-04
 */

#include "resource/npk_parser.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if defined(_MSC_VER)
#define strcasecmp _stricmp
#endif

/* ========================================================================== */
/* Static Data                                                                 */
/* ========================================================================== */

/**
 * NPK filename decrypt key (256 bytes)
 *
 * Key composition:
 * - Base string: "puchikon@neople dungeon and fighter DNF" (41 bytes)
 * - Padding: "DNF" repeated to fill 256 bytes
 * - Last byte: '\0'
 *
 * Formula: key[i] = base_string[i] for i < 41
 *          key[i] = "DNF"[(i-41) % 3] for 41 <= i < 255
 *          key[255] = '\0'
 */
static uint8_t g_npk_decrypt_key[NPK_NAME_LENGTH];
static bool g_npk_initialized = false;

/* ========================================================================== */
/* Initialization                                                              */
/* ========================================================================== */

void NPK_Initialize(void)
{
    if (g_npk_initialized) {
        return;
    }

    /* Base key string */
    const char* base_key = "puchikon@neople dungeon and fighter DNF";
    size_t base_len = strlen(base_key);  /* 41 bytes */

    /* Padding pattern */
    const char* padding = "DNF";
    size_t pad_len = 3;

    /* Build the 256-byte key */
    size_t i;

    /* Copy base string */
    for (i = 0; i < base_len && i < NPK_NAME_LENGTH - 1; i++) {
        g_npk_decrypt_key[i] = (uint8_t)base_key[i];
    }

    /* Fill with "DNF" padding */
    size_t pad_idx = 0;
    for (; i < NPK_NAME_LENGTH - 1; i++) {
        g_npk_decrypt_key[i] = (uint8_t)padding[pad_idx];
        pad_idx = (pad_idx + 1) % pad_len;
    }

    /* Last byte is null terminator */
    g_npk_decrypt_key[NPK_NAME_LENGTH - 1] = '\0';

    g_npk_initialized = true;
}

const uint8_t* NPK_GetDecryptKey(void)
{
    if (!g_npk_initialized) {
        NPK_Initialize();
    }
    return g_npk_decrypt_key;
}

/* ========================================================================== */
/* Filename Operations                                                         */
/* ========================================================================== */

void NPK_DecryptFilename(const uint8_t* encrypted, char* decrypted)
{
    if (!g_npk_initialized) {
        NPK_Initialize();
    }

    /* XOR decrypt: decrypted[i] = encrypted[i] ^ key[i] */
    for (int i = 0; i < NPK_NAME_LENGTH; i++) {
        decrypted[i] = (char)(encrypted[i] ^ g_npk_decrypt_key[i]);
    }

    /* Clean up: remove trailing slashes and nulls */
    int end = NPK_NAME_LENGTH - 1;
    while (end >= 0 && (decrypted[end] == '/' || decrypted[end] == '\\' ||
                        decrypted[end] == '\0')) {
        decrypted[end] = '\0';
        end--;
    }
}

uint32_t NPK_HashFilename(const char* filename)
{
    /* DJB2 hash variant (same as PVF library) */
    uint32_t hash = 5381;
    const unsigned char* p = (const unsigned char*)filename;

    while (*p) {
        /* Case-insensitive hash */
        unsigned char c = (unsigned char)tolower(*p);
        /* Normalize path separators */
        if (c == '\\') c = '/';
        hash = ((hash << 5) + hash) + c;  /* hash * 33 + c */
        p++;
    }

    return hash;
}

/* ========================================================================== */
/* Internal Hash Table Operations                                              */
/* ========================================================================== */

static int npk_build_hash_table(NPKArchive* archive)
{
    /* Allocate hash table */
    archive->hash_table_size = NPK_HASH_TABLE_SIZE;
    archive->hash_table = (NPKHashNode**)calloc(NPK_HASH_TABLE_SIZE,
                                                 sizeof(NPKHashNode*));
    if (archive->hash_table == NULL) {
        return NPK_ERROR_MEMORY;
    }

    /* Insert all entries */
    for (uint32_t i = 0; i < archive->file_count; i++) {
        NPKFileEntry* entry = &archive->entries[i];

        /* Compute hash and bucket */
        entry->name_hash = NPK_HashFilename(entry->name);
        int bucket = entry->name_hash % NPK_HASH_TABLE_SIZE;

        /* Create node */
        NPKHashNode* node = (NPKHashNode*)malloc(sizeof(NPKHashNode));
        if (node == NULL) {
            return NPK_ERROR_MEMORY;
        }
        node->index = i;
        node->next = archive->hash_table[bucket];
        archive->hash_table[bucket] = node;
    }

    return NPK_SUCCESS;
}

static void npk_free_hash_table(NPKArchive* archive)
{
    if (archive->hash_table == NULL) {
        return;
    }

    for (int i = 0; i < archive->hash_table_size; i++) {
        NPKHashNode* node = archive->hash_table[i];
        while (node != NULL) {
            NPKHashNode* next = node->next;
            free(node);
            node = next;
        }
    }

    free(archive->hash_table);
    archive->hash_table = NULL;
}

/* ========================================================================== */
/* Core API Implementation                                                     */
/* ========================================================================== */

int NPK_Open(const char* filepath, NPKArchive* archive)
{
    if (filepath == NULL || archive == NULL) {
        return NPK_ERROR_INVALID_PARAM;
    }

    /* Initialize if needed */
    if (!g_npk_initialized) {
        NPK_Initialize();
    }

    /* Clear archive structure */
    memset(archive, 0, sizeof(NPKArchive));
    strncpy(archive->path, filepath, sizeof(archive->path) - 1);

    /* Open file */
    archive->fp = fopen(filepath, "rb");
    if (archive->fp == NULL) {
        return NPK_ERROR_FILE_NOT_FOUND;
    }

    /* Read and verify magic header */
    char magic[NPK_MAGIC_LENGTH + 1];
    if (fread(magic, 1, NPK_MAGIC_LENGTH, archive->fp) != NPK_MAGIC_LENGTH) {
        fclose(archive->fp);
        archive->fp = NULL;
        return NPK_ERROR_READ_FAILED;
    }
    magic[NPK_MAGIC_LENGTH] = '\0';

    /* Check magic - "NeoplePack_Bill" or similar */
    if (strncmp(magic, NPK_MAGIC, 14) != 0) {  /* Check first 14 chars */
        fclose(archive->fp);
        archive->fp = NULL;
        return NPK_ERROR_INVALID_MAGIC;
    }

    /* Check if encrypted (last bytes may differ) */
    archive->is_encrypted = (magic[14] != 'l' || magic[15] != 'l');

    /* Read file count */
    if (fread(&archive->file_count, sizeof(uint32_t), 1, archive->fp) != 1) {
        fclose(archive->fp);
        archive->fp = NULL;
        return NPK_ERROR_READ_FAILED;
    }

    /* Sanity check */
    if (archive->file_count == 0 || archive->file_count > 100000) {
        fclose(archive->fp);
        archive->fp = NULL;
        return NPK_ERROR_INVALID_MAGIC;
    }

    /* Allocate entry array */
    archive->entries = (NPKFileEntry*)calloc(archive->file_count,
                                              sizeof(NPKFileEntry));
    if (archive->entries == NULL) {
        fclose(archive->fp);
        archive->fp = NULL;
        return NPK_ERROR_MEMORY;
    }

    /* Read file entries */
    uint32_t total_data = 0;
    for (uint32_t i = 0; i < archive->file_count; i++) {
        NPKFileEntry* entry = &archive->entries[i];

        /* Read offset and size */
        if (fread(&entry->offset, sizeof(uint32_t), 1, archive->fp) != 1 ||
            fread(&entry->size, sizeof(uint32_t), 1, archive->fp) != 1) {
            NPK_Close(archive);
            return NPK_ERROR_READ_FAILED;
        }

        /* Read encrypted filename */
        uint8_t encrypted_name[NPK_NAME_LENGTH];
        if (fread(encrypted_name, 1, NPK_NAME_LENGTH, archive->fp) != NPK_NAME_LENGTH) {
            NPK_Close(archive);
            return NPK_ERROR_READ_FAILED;
        }

        /* Decrypt filename */
        NPK_DecryptFilename(encrypted_name, entry->name);

        total_data += entry->size;
    }

    archive->total_data_size = total_data;

    /* Build hash table for O(1) lookup */
    int result = npk_build_hash_table(archive);
    if (result != NPK_SUCCESS) {
        NPK_Close(archive);
        return result;
    }

    return NPK_SUCCESS;
}

void NPK_Close(NPKArchive* archive)
{
    if (archive == NULL) {
        return;
    }

    /* Free hash table */
    npk_free_hash_table(archive);

    /* Free entries */
    if (archive->entries != NULL) {
        free(archive->entries);
        archive->entries = NULL;
    }

    /* Close file */
    if (archive->fp != NULL) {
        fclose(archive->fp);
        archive->fp = NULL;
    }

    /* Clear structure */
    memset(archive, 0, sizeof(NPKArchive));
}

NPKFileEntry* NPK_FindFile(NPKArchive* archive, const char* filename)
{
    if (archive == NULL || archive->hash_table == NULL || filename == NULL) {
        return NULL;
    }

    /* Compute hash */
    uint32_t hash = NPK_HashFilename(filename);
    int bucket = hash % archive->hash_table_size;

    /* Search bucket */
    NPKHashNode* node = archive->hash_table[bucket];
    while (node != NULL) {
        NPKFileEntry* entry = &archive->entries[node->index];
        if (entry->name_hash == hash) {
            /* Compare names (case-insensitive) */
            if (strcasecmp(entry->name, filename) == 0) {
                return entry;
            }
            /* Also try with normalized path */
            char normalized[NPK_NAME_LENGTH];
            strncpy(normalized, filename, NPK_NAME_LENGTH - 1);
            normalized[NPK_NAME_LENGTH - 1] = '\0';
            for (int i = 0; normalized[i]; i++) {
                if (normalized[i] == '\\') normalized[i] = '/';
            }
            if (strcasecmp(entry->name, normalized) == 0) {
                return entry;
            }
        }
        node = node->next;
    }

    return NULL;
}

int NPK_ReadFile(NPKArchive* archive, NPKFileEntry* entry,
                 void* buffer, size_t buffer_size)
{
    if (archive == NULL || entry == NULL || buffer == NULL) {
        return NPK_ERROR_INVALID_PARAM;
    }

    if (archive->fp == NULL) {
        return NPK_ERROR_FILE_NOT_FOUND;
    }

    if (buffer_size < entry->size) {
        return NPK_ERROR_INVALID_PARAM;
    }

    /* The offset stored in entry is already an ABSOLUTE offset in the file */
    /* (Verified via testing with Taiwan DNF NPK files) */
    uint32_t absolute_offset = entry->offset;

    /* Seek to data */
    if (fseek(archive->fp, absolute_offset, SEEK_SET) != 0) {
        return NPK_ERROR_READ_FAILED;
    }

    /* Read data */
    if (fread(buffer, 1, entry->size, archive->fp) != entry->size) {
        return NPK_ERROR_READ_FAILED;
    }

    return NPK_SUCCESS;
}

int NPK_ReadFileByPath(NPKArchive* archive, const char* filename,
                       void* buffer, size_t buffer_size, uint32_t* actual_size)
{
    NPKFileEntry* entry = NPK_FindFile(archive, filename);
    if (entry == NULL) {
        return NPK_ERROR_NOT_FOUND;
    }

    int result = NPK_ReadFile(archive, entry, buffer, buffer_size);
    if (result == NPK_SUCCESS && actual_size != NULL) {
        *actual_size = entry->size;
    }

    return result;
}

/* ========================================================================== */
/* Utility Functions                                                           */
/* ========================================================================== */

uint32_t NPK_GetFileCount(NPKArchive* archive)
{
    if (archive == NULL) {
        return 0;
    }
    return archive->file_count;
}

NPKFileEntry* NPK_GetEntry(NPKArchive* archive, uint32_t index)
{
    if (archive == NULL || index >= archive->file_count) {
        return NULL;
    }
    return &archive->entries[index];
}

void NPK_ListFiles(NPKArchive* archive,
                   void (*callback)(const char* filename, uint32_t size, void* userdata),
                   void* userdata)
{
    if (archive == NULL || callback == NULL) {
        return;
    }

    for (uint32_t i = 0; i < archive->file_count; i++) {
        NPKFileEntry* entry = &archive->entries[i];
        callback(entry->name, entry->size, userdata);
    }
}

bool NPK_IsEncrypted(NPKArchive* archive)
{
    if (archive == NULL) {
        return false;
    }
    return archive->is_encrypted;
}
