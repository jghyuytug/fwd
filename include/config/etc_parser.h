/**
 * @file etc_parser.h
 * @brief DNF ETC Format Parser - Parse DNF-style configuration files
 *
 * DNF ETC format uses backtick-wrapped keys:
 *   `key_name` value
 *   `another_key` 123
 *
 * Phase: 6.7 - Configuration Abstraction (Stage 3)
 * Date: 2025-11-25
 */

#ifndef INCLUDE_CONFIG_ETC_PARSER_H_
#define INCLUDE_CONFIG_ETC_PARSER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* ========================================================================
 * CONSTANTS
 * ======================================================================== */

#define ETC_MAX_KEY_LENGTH      128
#define ETC_MAX_VALUE_LENGTH    1024
#define ETC_MAX_ENTRIES         256

/* ========================================================================
 * DATA STRUCTURES
 * ======================================================================== */

/**
 * Value type enumeration
 */
typedef enum {
    ETC_VALUE_TYPE_INTEGER = 0,
    ETC_VALUE_TYPE_FLOAT = 1,
    ETC_VALUE_TYPE_STRING = 2
} ETCValueType;

/**
 * Single key-value entry
 */
typedef struct {
    char key[ETC_MAX_KEY_LENGTH];
    ETCValueType type;
    union {
        int int_value;
        float float_value;
        char str_value[ETC_MAX_VALUE_LENGTH];
    } value;
} ETCEntry;

/**
 * Parsed ETC document
 */
typedef struct {
    ETCEntry entries[ETC_MAX_ENTRIES];
    int entry_count;
    char error_message[256];
} ETCDocument;

/* ========================================================================
 * PUBLIC API
 * ======================================================================== */

/**
 * Initialize ETC document
 *
 * Parameters:
 *   doc: Document to initialize
 */
void ETC_Init(ETCDocument* doc);

/**
 * Parse ETC format data from buffer
 *
 * Parameters:
 *   doc: Output document
 *   data: Input data buffer
 *   size: Size of input data
 *
 * Returns: 0 on success, negative error code on failure
 */
int ETC_Parse(ETCDocument* doc, const char* data, size_t size);

/**
 * Parse ETC format data from file
 *
 * Parameters:
 *   doc: Output document
 *   filepath: Path to ETC file
 *
 * Returns: 0 on success, negative error code on failure
 */
int ETC_ParseFile(ETCDocument* doc, const char* filepath);

/**
 * Get integer value by key
 *
 * Parameters:
 *   doc: Parsed document
 *   key: Key name (without backticks)
 *   default_value: Value to return if key not found
 *
 * Returns: Integer value or default_value
 */
int ETC_GetInt(const ETCDocument* doc, const char* key, int default_value);

/**
 * Get float value by key
 *
 * Parameters:
 *   doc: Parsed document
 *   key: Key name (without backticks)
 *   default_value: Value to return if key not found
 *
 * Returns: Float value or default_value
 */
float ETC_GetFloat(const ETCDocument* doc, const char* key, float default_value);

/**
 * Get string value by key
 *
 * Parameters:
 *   doc: Parsed document
 *   key: Key name (without backticks)
 *   out_value: Output buffer
 *   out_size: Size of output buffer
 *
 * Returns: 0 on success, -1 if key not found
 */
int ETC_GetString(const ETCDocument* doc, const char* key,
                  char* out_value, size_t out_size);

/**
 * Check if key exists
 *
 * Parameters:
 *   doc: Parsed document
 *   key: Key name (without backticks)
 *
 * Returns: 1 if exists, 0 if not
 */
int ETC_HasKey(const ETCDocument* doc, const char* key);

/**
 * Get entry count
 */
int ETC_GetEntryCount(const ETCDocument* doc);

/**
 * Get entry by index
 */
const ETCEntry* ETC_GetEntry(const ETCDocument* doc, int index);

/**
 * Free document resources (if any dynamic allocation)
 */
void ETC_Free(ETCDocument* doc);

/* ========================================================================
 * ERROR CODES
 * ======================================================================== */

#define ETC_SUCCESS             0
#define ETC_ERROR_NULL_PARAM   -1
#define ETC_ERROR_INVALID_FMT  -2
#define ETC_ERROR_TOO_MANY     -3
#define ETC_ERROR_KEY_TOO_LONG -4
#define ETC_ERROR_VAL_TOO_LONG -5
#define ETC_ERROR_FILE_OPEN    -6
#define ETC_ERROR_FILE_READ    -7

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_CONFIG_ETC_PARSER_H_ */
