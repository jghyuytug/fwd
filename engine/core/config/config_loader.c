/**
 * @file config_loader.c
 * @brief Configuration file loader implementation
 */

#include "config_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Trim whitespace from string */
static char* trim(char* str)
{
    char* end;

    if (!str) return NULL;

    /* Trim leading */
    while (isspace((unsigned char)*str)) str++;

    if (*str == '\0') return str;

    /* Trim trailing */
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';

    return str;
}

/* Find entry by section and key */
static ConfigEntry* find_entry(Config* config, const char* section,
                               const char* key)
{
    int i;
    const char* sec = section ? section : "";

    for (i = 0; i < config->entry_count; i++) {
        if (strcmp(config->entries[i].section, sec) == 0 &&
            strcmp(config->entries[i].key, key) == 0) {
            return &config->entries[i];
        }
    }

    return NULL;
}

/* Find entry (const version) */
static const ConfigEntry* find_entry_const(const Config* config,
                                           const char* section,
                                           const char* key)
{
    int i;
    const char* sec = section ? section : "";

    for (i = 0; i < config->entry_count; i++) {
        if (strcmp(config->entries[i].section, sec) == 0 &&
            strcmp(config->entries[i].key, key) == 0) {
            return &config->entries[i];
        }
    }

    return NULL;
}

int config_init(Config* config)
{
    if (!config) {
        return ENGINE_ERROR_PARAM;
    }

    memset(config, 0, sizeof(Config));
    return ENGINE_OK;
}

void config_cleanup(Config* config)
{
    if (config) {
        memset(config, 0, sizeof(Config));
    }
}

int config_load(Config* config, const char* filename)
{
    FILE* fp;
    char line[CONFIG_MAX_LINE_LEN];
    char current_section[CONFIG_MAX_KEY_LEN] = "";
    char* ptr;
    char* key;
    char* value;
    ConfigEntry* entry;

    if (!config || !filename) {
        return ENGINE_ERROR_PARAM;
    }

    fp = fopen(filename, "r");
    if (!fp) {
        return ENGINE_ERROR_IO;
    }

    config->entry_count = 0;
    strncpy(config->filename, filename, sizeof(config->filename) - 1);

    while (fgets(line, sizeof(line), fp)) {
        ptr = trim(line);

        /* Skip empty lines and comments */
        if (*ptr == '\0' || *ptr == '#' || *ptr == ';') {
            continue;
        }

        /* Section header */
        if (*ptr == '[') {
            char* end = strchr(ptr, ']');
            if (end) {
                *end = '\0';
                strncpy(current_section, ptr + 1, sizeof(current_section) - 1);
            }
            continue;
        }

        /* Key=Value */
        key = ptr;
        value = strchr(ptr, '=');
        if (!value) {
            continue;
        }

        *value = '\0';
        value++;

        key = trim(key);
        value = trim(value);

        /* Remove quotes from value */
        if (value[0] == '"' || value[0] == '\'') {
            char quote = value[0];
            char* end;

            value++;
            end = strrchr(value, quote);
            if (end) {
                *end = '\0';
            }
        }

        if (config->entry_count >= CONFIG_MAX_ENTRIES) {
            fprintf(stderr, "[CONFIG] Too many entries, max=%d\n",
                    CONFIG_MAX_ENTRIES);
            break;
        }

        entry = &config->entries[config->entry_count];
        strncpy(entry->section, current_section, sizeof(entry->section) - 1);
        strncpy(entry->key, key, sizeof(entry->key) - 1);
        strncpy(entry->value, value, sizeof(entry->value) - 1);
        config->entry_count++;
    }

    fclose(fp);
    config->modified = 0;

    return ENGINE_OK;
}

int config_save(Config* config, const char* filename)
{
    FILE* fp;
    int i;
    char current_section[CONFIG_MAX_KEY_LEN] = "";
    const char* save_file;

    if (!config) {
        return ENGINE_ERROR_PARAM;
    }

    save_file = filename ? filename : config->filename;
    if (save_file[0] == '\0') {
        return ENGINE_ERROR_PARAM;
    }

    fp = fopen(save_file, "w");
    if (!fp) {
        return ENGINE_ERROR_IO;
    }

    for (i = 0; i < config->entry_count; i++) {
        ConfigEntry* entry = &config->entries[i];

        /* Write section header if changed */
        if (strcmp(current_section, entry->section) != 0) {
            strncpy(current_section, entry->section, sizeof(current_section) - 1);

            if (current_section[0] != '\0') {
                if (i > 0) fprintf(fp, "\n");
                fprintf(fp, "[%s]\n", current_section);
            }
        }

        fprintf(fp, "%s=%s\n", entry->key, entry->value);
    }

    fclose(fp);
    config->modified = 0;

    return ENGINE_OK;
}

int config_reload(Config* config)
{
    if (!config || config->filename[0] == '\0') {
        return ENGINE_ERROR_PARAM;
    }

    return config_load(config, config->filename);
}

const char* config_get_string(const Config* config, const char* section,
                              const char* key, const char* default_val)
{
    const ConfigEntry* entry;

    if (!config || !key) {
        return default_val;
    }

    entry = find_entry_const(config, section, key);
    return entry ? entry->value : default_val;
}

int config_get_int(const Config* config, const char* section,
                   const char* key, int default_val)
{
    const char* value = config_get_string(config, section, key, NULL);
    return value ? atoi(value) : default_val;
}

u32 config_get_uint(const Config* config, const char* section,
                    const char* key, u32 default_val)
{
    const char* value = config_get_string(config, section, key, NULL);
    return value ? (u32)strtoul(value, NULL, 10) : default_val;
}

int config_get_bool(const Config* config, const char* section,
                    const char* key, int default_val)
{
    const char* value = config_get_string(config, section, key, NULL);

    if (!value) {
        return default_val;
    }

    /* Check for true values */
    if (strcmp(value, "1") == 0 ||
        strcasecmp(value, "true") == 0 ||
        strcasecmp(value, "yes") == 0 ||
        strcasecmp(value, "on") == 0) {
        return 1;
    }

    /* Check for false values */
    if (strcmp(value, "0") == 0 ||
        strcasecmp(value, "false") == 0 ||
        strcasecmp(value, "no") == 0 ||
        strcasecmp(value, "off") == 0) {
        return 0;
    }

    return default_val;
}

float config_get_float(const Config* config, const char* section,
                       const char* key, float default_val)
{
    const char* value = config_get_string(config, section, key, NULL);
    return value ? (float)atof(value) : default_val;
}

int config_set_string(Config* config, const char* section,
                      const char* key, const char* value)
{
    ConfigEntry* entry;

    if (!config || !key || !value) {
        return ENGINE_ERROR_PARAM;
    }

    entry = find_entry(config, section, key);

    if (entry) {
        strncpy(entry->value, value, sizeof(entry->value) - 1);
    } else {
        if (config->entry_count >= CONFIG_MAX_ENTRIES) {
            return ENGINE_ERROR_FULL;
        }

        entry = &config->entries[config->entry_count];
        strncpy(entry->section, section ? section : "",
                sizeof(entry->section) - 1);
        strncpy(entry->key, key, sizeof(entry->key) - 1);
        strncpy(entry->value, value, sizeof(entry->value) - 1);
        config->entry_count++;
    }

    config->modified = 1;
    return ENGINE_OK;
}

int config_set_int(Config* config, const char* section,
                   const char* key, int value)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", value);
    return config_set_string(config, section, key, buf);
}

int config_has_key(const Config* config, const char* section, const char* key)
{
    return find_entry_const(config, section, key) != NULL;
}

int config_remove_key(Config* config, const char* section, const char* key)
{
    int i;
    const char* sec = section ? section : "";

    if (!config || !key) {
        return ENGINE_ERROR_PARAM;
    }

    for (i = 0; i < config->entry_count; i++) {
        if (strcmp(config->entries[i].section, sec) == 0 &&
            strcmp(config->entries[i].key, key) == 0) {

            /* Shift remaining entries */
            memmove(&config->entries[i], &config->entries[i + 1],
                    sizeof(ConfigEntry) * (config->entry_count - i - 1));
            config->entry_count--;
            config->modified = 1;

            return ENGINE_OK;
        }
    }

    return ENGINE_ERROR_NOTFOUND;
}

int config_get_count(const Config* config)
{
    return config ? config->entry_count : 0;
}

int config_get_entry(const Config* config, int index,
                     const char** section, const char** key, const char** value)
{
    if (!config || index < 0 || index >= config->entry_count) {
        return ENGINE_ERROR_NOTFOUND;
    }

    if (section) *section = config->entries[index].section;
    if (key) *key = config->entries[index].key;
    if (value) *value = config->entries[index].value;

    return ENGINE_OK;
}
