/**
 * @file etc_parser.c
 * @brief DNF ETC Format Parser Implementation
 *
 * Phase: 6.7 - Configuration Abstraction (Stage 3)
 * Date: 2025-11-25
 */

#include "../../include/config/etc_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ========================================================================
 * INTERNAL HELPERS
 * ======================================================================== */

/**
 * Skip whitespace characters
 */
static const char* skip_whitespace(const char* p) {
    while (*p && (*p == ' ' || *p == '\t')) {
        p++;
    }
    return p;
}

/**
 * Skip to end of line
 */
static const char* skip_to_eol(const char* p) {
    while (*p && *p != '\n' && *p != '\r') {
        p++;
    }
    return p;
}

/**
 * Skip line endings
 */
static const char* skip_eol(const char* p) {
    if (*p == '\r') p++;
    if (*p == '\n') p++;
    return p;
}

/**
 * Check if character is a digit or minus sign
 */
static int is_numeric_start(char c) {
    return isdigit((unsigned char)c) || c == '-' || c == '+';
}

/**
 * Parse a value (integer, float, or string)
 */
static int parse_value(const char* start, const char* end, ETCEntry* entry) {
    char buf[ETC_MAX_VALUE_LENGTH];
    size_t len = end - start;

    if (len >= ETC_MAX_VALUE_LENGTH) {
        len = ETC_MAX_VALUE_LENGTH - 1;
    }

    /* Trim trailing whitespace */
    while (len > 0 && (start[len-1] == ' ' || start[len-1] == '\t' ||
                       start[len-1] == '\r' || start[len-1] == '\n')) {
        len--;
    }

    memcpy(buf, start, len);
    buf[len] = '\0';

    /* Try to parse as number */
    if (is_numeric_start(buf[0])) {
        /* Check if it's a float (contains '.') */
        if (strchr(buf, '.') != NULL) {
            char* endptr;
            float f = strtof(buf, &endptr);
            if (*endptr == '\0' || isspace((unsigned char)*endptr)) {
                entry->type = ETC_VALUE_TYPE_FLOAT;
                entry->value.float_value = f;
                return 0;
            }
        } else {
            char* endptr;
            long l = strtol(buf, &endptr, 10);
            if (*endptr == '\0' || isspace((unsigned char)*endptr)) {
                entry->type = ETC_VALUE_TYPE_INTEGER;
                entry->value.int_value = (int)l;
                return 0;
            }
        }
    }

    /* Treat as string */
    entry->type = ETC_VALUE_TYPE_STRING;
    strncpy(entry->value.str_value, buf, ETC_MAX_VALUE_LENGTH - 1);
    entry->value.str_value[ETC_MAX_VALUE_LENGTH - 1] = '\0';

    return 0;
}

/* ========================================================================
 * PUBLIC API IMPLEMENTATION
 * ======================================================================== */

void ETC_Init(ETCDocument* doc) {
    if (!doc) return;

    memset(doc, 0, sizeof(ETCDocument));
    doc->entry_count = 0;
}

int ETC_Parse(ETCDocument* doc, const char* data, size_t size) {
    if (!doc || !data) {
        return ETC_ERROR_NULL_PARAM;
    }

    ETC_Init(doc);

    const char* p = data;
    const char* end = data + size;

    while (p < end && *p) {
        /* Skip leading whitespace and empty lines */
        p = skip_whitespace(p);

        /* Skip empty lines */
        if (*p == '\n' || *p == '\r') {
            p = skip_eol(p);
            continue;
        }

        /* Skip comments (lines starting with // or #) */
        if (*p == '/' && *(p+1) == '/') {
            p = skip_to_eol(p);
            p = skip_eol(p);
            continue;
        }
        if (*p == '#') {
            p = skip_to_eol(p);
            p = skip_eol(p);
            continue;
        }

        /* Check for backtick-wrapped key: `key_name` */
        if (*p == '`') {
            p++;  /* Skip opening backtick */

            const char* key_start = p;

            /* Find closing backtick */
            while (p < end && *p && *p != '`' && *p != '\n' && *p != '\r') {
                p++;
            }

            if (*p != '`') {
                snprintf(doc->error_message, sizeof(doc->error_message),
                         "Missing closing backtick for key");
                return ETC_ERROR_INVALID_FMT;
            }

            size_t key_len = p - key_start;
            if (key_len >= ETC_MAX_KEY_LENGTH) {
                snprintf(doc->error_message, sizeof(doc->error_message),
                         "Key too long: %zu chars", key_len);
                return ETC_ERROR_KEY_TOO_LONG;
            }

            if (doc->entry_count >= ETC_MAX_ENTRIES) {
                snprintf(doc->error_message, sizeof(doc->error_message),
                         "Too many entries (max %d)", ETC_MAX_ENTRIES);
                return ETC_ERROR_TOO_MANY;
            }

            ETCEntry* entry = &doc->entries[doc->entry_count];
            memcpy(entry->key, key_start, key_len);
            entry->key[key_len] = '\0';

            p++;  /* Skip closing backtick */

            /* Skip whitespace between key and value */
            p = skip_whitespace(p);

            /* Find value (until end of line) */
            const char* val_start = p;
            p = skip_to_eol(p);
            const char* val_end = p;

            /* Parse value */
            if (val_start < val_end) {
                parse_value(val_start, val_end, entry);
            } else {
                /* Empty value - treat as empty string */
                entry->type = ETC_VALUE_TYPE_STRING;
                entry->value.str_value[0] = '\0';
            }

            doc->entry_count++;

            /* Skip line ending */
            p = skip_eol(p);
        } else {
            /* Unknown line format - skip */
            p = skip_to_eol(p);
            p = skip_eol(p);
        }
    }

    return ETC_SUCCESS;
}

int ETC_ParseFile(ETCDocument* doc, const char* filepath) {
    if (!doc || !filepath) {
        return ETC_ERROR_NULL_PARAM;
    }

    FILE* fp = fopen(filepath, "rb");
    if (!fp) {
        snprintf(doc->error_message, sizeof(doc->error_message),
                 "Cannot open file: %s", filepath);
        return ETC_ERROR_FILE_OPEN;
    }

    /* Get file size */
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (file_size <= 0) {
        fclose(fp);
        ETC_Init(doc);
        return ETC_SUCCESS;  /* Empty file is valid */
    }

    /* Read file content */
    char* buffer = (char*)malloc(file_size + 1);
    if (!buffer) {
        fclose(fp);
        return ETC_ERROR_FILE_READ;
    }

    size_t read_size = fread(buffer, 1, file_size, fp);
    fclose(fp);

    buffer[read_size] = '\0';

    /* Parse content */
    int result = ETC_Parse(doc, buffer, read_size);

    free(buffer);

    return result;
}

int ETC_GetInt(const ETCDocument* doc, const char* key, int default_value) {
    if (!doc || !key) {
        return default_value;
    }

    for (int i = 0; i < doc->entry_count; i++) {
        if (strcmp(doc->entries[i].key, key) == 0) {
            if (doc->entries[i].type == ETC_VALUE_TYPE_INTEGER) {
                return doc->entries[i].value.int_value;
            } else if (doc->entries[i].type == ETC_VALUE_TYPE_FLOAT) {
                return (int)doc->entries[i].value.float_value;
            } else if (doc->entries[i].type == ETC_VALUE_TYPE_STRING) {
                return atoi(doc->entries[i].value.str_value);
            }
        }
    }

    return default_value;
}

float ETC_GetFloat(const ETCDocument* doc, const char* key, float default_value) {
    if (!doc || !key) {
        return default_value;
    }

    for (int i = 0; i < doc->entry_count; i++) {
        if (strcmp(doc->entries[i].key, key) == 0) {
            if (doc->entries[i].type == ETC_VALUE_TYPE_FLOAT) {
                return doc->entries[i].value.float_value;
            } else if (doc->entries[i].type == ETC_VALUE_TYPE_INTEGER) {
                return (float)doc->entries[i].value.int_value;
            } else if (doc->entries[i].type == ETC_VALUE_TYPE_STRING) {
                return (float)atof(doc->entries[i].value.str_value);
            }
        }
    }

    return default_value;
}

int ETC_GetString(const ETCDocument* doc, const char* key,
                  char* out_value, size_t out_size) {
    if (!doc || !key || !out_value || out_size == 0) {
        return -1;
    }

    for (int i = 0; i < doc->entry_count; i++) {
        if (strcmp(doc->entries[i].key, key) == 0) {
            if (doc->entries[i].type == ETC_VALUE_TYPE_STRING) {
                strncpy(out_value, doc->entries[i].value.str_value, out_size - 1);
                out_value[out_size - 1] = '\0';
            } else if (doc->entries[i].type == ETC_VALUE_TYPE_INTEGER) {
                snprintf(out_value, out_size, "%d", doc->entries[i].value.int_value);
            } else if (doc->entries[i].type == ETC_VALUE_TYPE_FLOAT) {
                snprintf(out_value, out_size, "%f", doc->entries[i].value.float_value);
            }
            return 0;
        }
    }

    return -1;
}

int ETC_HasKey(const ETCDocument* doc, const char* key) {
    if (!doc || !key) {
        return 0;
    }

    for (int i = 0; i < doc->entry_count; i++) {
        if (strcmp(doc->entries[i].key, key) == 0) {
            return 1;
        }
    }

    return 0;
}

int ETC_GetEntryCount(const ETCDocument* doc) {
    return doc ? doc->entry_count : 0;
}

const ETCEntry* ETC_GetEntry(const ETCDocument* doc, int index) {
    if (!doc || index < 0 || index >= doc->entry_count) {
        return NULL;
    }
    return &doc->entries[index];
}

void ETC_Free(ETCDocument* doc) {
    if (doc) {
        /* Currently no dynamic allocation, but keep for future */
        doc->entry_count = 0;
    }
}
