/**
 * @file config_loader.h
 * @brief Configuration file loader (INI/CFG format)
 *
 * Supports key=value format with sections.
 */

#ifndef ENGINE_CORE_CONFIG_CONFIG_LOADER_H
#define ENGINE_CORE_CONFIG_CONFIG_LOADER_H

#include "../types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_MAX_SECTIONS   64
#define CONFIG_MAX_ENTRIES    256
#define CONFIG_MAX_KEY_LEN    64
#define CONFIG_MAX_VALUE_LEN  256
#define CONFIG_MAX_LINE_LEN   512

/* Config entry */
typedef struct {
    char key[CONFIG_MAX_KEY_LEN];
    char value[CONFIG_MAX_VALUE_LEN];
    char section[CONFIG_MAX_KEY_LEN];
} ConfigEntry;

/* Config structure */
typedef struct {
    ConfigEntry entries[CONFIG_MAX_ENTRIES];
    int         entry_count;
    char        filename[256];
    u8          modified;
} Config;

/**
 * Initialize config
 * @param config Config instance
 * @return ENGINE_OK on success
 */
int config_init(Config* config);

/**
 * Cleanup config
 * @param config Config instance
 */
void config_cleanup(Config* config);

/**
 * Load config from file
 * @param config   Config instance
 * @param filename File path
 * @return ENGINE_OK on success
 */
int config_load(Config* config, const char* filename);

/**
 * Save config to file
 * @param config   Config instance
 * @param filename File path (NULL = use original)
 * @return ENGINE_OK on success
 */
int config_save(Config* config, const char* filename);

/**
 * Reload config from file
 * @param config Config instance
 * @return ENGINE_OK on success
 */
int config_reload(Config* config);

/**
 * Get string value
 * @param config  Config instance
 * @param section Section name (NULL for global)
 * @param key     Key name
 * @param default_val Default if not found
 * @return Value string or default
 */
const char* config_get_string(const Config* config, const char* section,
                              const char* key, const char* default_val);

/**
 * Get integer value
 * @param config  Config instance
 * @param section Section name (NULL for global)
 * @param key     Key name
 * @param default_val Default if not found
 * @return Integer value or default
 */
int config_get_int(const Config* config, const char* section,
                   const char* key, int default_val);

/**
 * Get unsigned integer value
 * @param config  Config instance
 * @param section Section name (NULL for global)
 * @param key     Key name
 * @param default_val Default if not found
 * @return Unsigned integer value or default
 */
u32 config_get_uint(const Config* config, const char* section,
                    const char* key, u32 default_val);

/**
 * Get boolean value
 * @param config  Config instance
 * @param section Section name (NULL for global)
 * @param key     Key name
 * @param default_val Default if not found
 * @return 1 for true, 0 for false
 */
int config_get_bool(const Config* config, const char* section,
                    const char* key, int default_val);

/**
 * Get float value
 * @param config  Config instance
 * @param section Section name (NULL for global)
 * @param key     Key name
 * @param default_val Default if not found
 * @return Float value or default
 */
float config_get_float(const Config* config, const char* section,
                       const char* key, float default_val);

/**
 * Set string value
 * @param config  Config instance
 * @param section Section name (NULL for global)
 * @param key     Key name
 * @param value   Value to set
 * @return ENGINE_OK on success
 */
int config_set_string(Config* config, const char* section,
                      const char* key, const char* value);

/**
 * Set integer value
 * @param config  Config instance
 * @param section Section name (NULL for global)
 * @param key     Key name
 * @param value   Value to set
 * @return ENGINE_OK on success
 */
int config_set_int(Config* config, const char* section,
                   const char* key, int value);

/**
 * Check if key exists
 * @param config  Config instance
 * @param section Section name (NULL for global)
 * @param key     Key name
 * @return 1 if exists, 0 otherwise
 */
int config_has_key(const Config* config, const char* section, const char* key);

/**
 * Remove key
 * @param config  Config instance
 * @param section Section name (NULL for global)
 * @param key     Key name
 * @return ENGINE_OK on success
 */
int config_remove_key(Config* config, const char* section, const char* key);

/**
 * Get entry count
 * @param config Config instance
 * @return Number of entries
 */
int config_get_count(const Config* config);

/**
 * Iterate entries
 * @param config Config instance
 * @param index  Entry index
 * @param section Output: section name
 * @param key    Output: key name
 * @param value  Output: value
 * @return ENGINE_OK on success, ENGINE_ERROR_NOTFOUND if index out of range
 */
int config_get_entry(const Config* config, int index,
                     const char** section, const char** key, const char** value);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_CORE_CONFIG_CONFIG_LOADER_H */
