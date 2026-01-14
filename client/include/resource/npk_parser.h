/**
 * @file npk_parser.h
 * @brief DNF NPK Archive Parser
 *
 * NPK (Neople Pack) is DNF's image archive format.
 * Each NPK contains multiple IMG files (sprite data).
 *
 * File format:
 * - Header: "NeoplePack_Bill" (16 bytes)
 * - File count: uint32_t
 * - File entries: 264 bytes each (offset, size, encrypted_name)
 * - IMG data blocks
 *
 * @date 2026-01-04
 */

#ifndef NPK_PARSER_H
#define NPK_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

/* ========================================================================== */
/* Constants                                                                   */
/* ========================================================================== */

#define NPK_MAGIC           "NeoplePack_Bill"
#define NPK_MAGIC_LENGTH    16
#define NPK_ENTRY_SIZE      264     /* 4 + 4 + 256 bytes per entry */
#define NPK_NAME_LENGTH     256     /* encrypted filename length */
#define NPK_HASH_TABLE_SIZE 256     /* hash table bucket count */

/* Return codes */
#define NPK_SUCCESS              0
#define NPK_ERROR_FILE_NOT_FOUND -1
#define NPK_ERROR_INVALID_MAGIC  -2
#define NPK_ERROR_READ_FAILED    -3
#define NPK_ERROR_MEMORY         -4
#define NPK_ERROR_INVALID_PARAM  -5
#define NPK_ERROR_NOT_FOUND      -6

/* ========================================================================== */
/* Data Structures                                                             */
/* ========================================================================== */

/**
 * @brief NPK file entry (264 bytes in file)
 */
typedef struct NPKFileEntry {
    uint32_t    offset;             /* data offset in NPK */
    uint32_t    size;               /* data size */
    char        name[NPK_NAME_LENGTH]; /* decrypted filename */
    uint32_t    name_hash;          /* filename hash for quick lookup */
} NPKFileEntry;

/**
 * @brief Hash table node for O(1) file lookup
 */
typedef struct NPKHashNode {
    int                 index;      /* entry index in entries array */
    struct NPKHashNode* next;       /* collision chain */
} NPKHashNode;

/**
 * @brief NPK Archive handle
 */
typedef struct NPKArchive {
    char            path[512];      /* archive file path */
    FILE*           fp;             /* file handle */
    uint32_t        file_count;     /* number of IMG files */
    NPKFileEntry*   entries;        /* file entry array */

    /* Hash table for O(1) lookup */
    NPKHashNode**   hash_table;
    int             hash_table_size;

    /* Statistics */
    uint32_t        total_data_size;
    bool            is_encrypted;   /* ImagePacks4 = encrypted */
} NPKArchive;

/* ========================================================================== */
/* Core API                                                                    */
/* ========================================================================== */

/**
 * @brief Initialize NPK subsystem
 *
 * Generates the decrypt key table. Must be called once before using NPK functions.
 */
void NPK_Initialize(void);

/**
 * @brief Open an NPK archive
 *
 * @param filepath  Path to the .npk file
 * @param archive   Output: archive handle
 *
 * @return NPK_SUCCESS on success, error code otherwise
 */
int NPK_Open(const char* filepath, NPKArchive* archive);

/**
 * @brief Close an NPK archive
 *
 * @param archive   Archive handle to close
 */
void NPK_Close(NPKArchive* archive);

/**
 * @brief Find a file in the archive
 *
 * @param archive   Archive handle
 * @param filename  IMG filename to find (e.g., "sprite_character.img")
 *
 * @return Pointer to entry, or NULL if not found
 */
NPKFileEntry* NPK_FindFile(NPKArchive* archive, const char* filename);

/**
 * @brief Read file data from archive
 *
 * @param archive     Archive handle
 * @param entry       File entry (from NPK_FindFile)
 * @param buffer      Output buffer (caller allocates)
 * @param buffer_size Buffer size (must >= entry->size)
 *
 * @return NPK_SUCCESS on success, error code otherwise
 */
int NPK_ReadFile(NPKArchive* archive, NPKFileEntry* entry,
                 void* buffer, size_t buffer_size);

/**
 * @brief Read file by path (convenience function)
 *
 * @param archive     Archive handle
 * @param filename    IMG filename to read
 * @param buffer      Output buffer (caller allocates)
 * @param buffer_size Buffer size
 * @param actual_size Output: actual bytes read
 *
 * @return NPK_SUCCESS on success, error code otherwise
 */
int NPK_ReadFileByPath(NPKArchive* archive, const char* filename,
                       void* buffer, size_t buffer_size, uint32_t* actual_size);

/* ========================================================================== */
/* Utility Functions                                                           */
/* ========================================================================== */

/**
 * @brief Get number of files in archive
 *
 * @param archive   Archive handle
 * @return File count, or 0 if invalid
 */
uint32_t NPK_GetFileCount(NPKArchive* archive);

/**
 * @brief Get file entry by index
 *
 * @param archive   Archive handle
 * @param index     Entry index (0-based)
 *
 * @return Pointer to entry, or NULL if out of range
 */
NPKFileEntry* NPK_GetEntry(NPKArchive* archive, uint32_t index);

/**
 * @brief List all files (for debugging)
 *
 * @param archive   Archive handle
 * @param callback  Function called for each file (filename, size, userdata)
 * @param userdata  User data passed to callback
 */
void NPK_ListFiles(NPKArchive* archive,
                   void (*callback)(const char* filename, uint32_t size, void* userdata),
                   void* userdata);

/**
 * @brief Check if archive is encrypted (ImagePacks4)
 *
 * @param archive   Archive handle
 * @return true if encrypted, false otherwise
 */
bool NPK_IsEncrypted(NPKArchive* archive);

/* ========================================================================== */
/* Internal Functions (exposed for testing)                                    */
/* ========================================================================== */

/**
 * @brief Decrypt filename using XOR key
 *
 * @param encrypted  Encrypted filename (256 bytes)
 * @param decrypted  Output: decrypted filename (256 bytes)
 */
void NPK_DecryptFilename(const uint8_t* encrypted, char* decrypted);

/**
 * @brief Compute filename hash (DJB2 variant)
 *
 * @param filename  Filename to hash
 * @return Hash value
 */
uint32_t NPK_HashFilename(const char* filename);

/**
 * @brief Get the decrypt key table (256 bytes)
 *
 * @return Pointer to static key table
 */
const uint8_t* NPK_GetDecryptKey(void);

#ifdef __cplusplus
}
#endif

#endif /* NPK_PARSER_H */
