/**
 * @file pvf_data_parser.c
 * @brief PVF binary key/value parser implementation
 */

#include "pvf/pvf_data_parser.h"
#include "pvf/pvf_types.h"

#include <stdlib.h>
#include <string.h>

/* ----------------------------------------------------------------------------
 * Low-level readers (little-endian)
 * ------------------------------------------------------------------------- */

static inline uint8_t read_u8(const unsigned char* data, size_t* pos) {
    return data[(*pos)++];
}

static inline uint16_t read_u16(const unsigned char* data, size_t* pos) {
    uint16_t value;
    memcpy(&value, data + *pos, sizeof(value));
    *pos += 2;
    return value;
}

static inline uint32_t read_u32(const unsigned char* data, size_t* pos) {
    uint32_t value;
    memcpy(&value, data + *pos, sizeof(value));
    *pos += 4;
    return value;
}

static inline int32_t read_i32(const unsigned char* data, size_t* pos) {
    int32_t value;
    memcpy(&value, data + *pos, sizeof(value));
    *pos += 4;
    return value;
}

static inline float read_f32(const unsigned char* data, size_t* pos) {
    float value;
    memcpy(&value, data + *pos, sizeof(value));
    *pos += 4;
    return value;
}

static int parse_string(const unsigned char* data, size_t size, size_t* pos, char** out, uint32_t* out_len) {
    if (pos == NULL || out == NULL || out_len == NULL) {
        return PVF_ERROR_INVALID_PARAMETER;
    }
    if (*pos + 5 > size) {
        return PVF_ERROR_PARSE_FAILED;
    }

    uint32_t len = read_u32(data, pos);
    uint8_t marker = read_u8(data, pos);
    if (marker != 0x0A) {
        return PVF_ERROR_PARSE_FAILED;
    }
    if (*pos + (size_t)len > size) {
        return PVF_ERROR_PARSE_FAILED;
    }

    char* s = (char*)malloc((size_t)len + 1);
    if (s == NULL) {
        return PVF_ERROR_MEMORY;
    }

    memcpy(s, data + *pos, len);
    s[len] = '\0';
    *pos += len;

    *out = s;
    *out_len = len;
    return PVF_SUCCESS;
}

/* ----------------------------------------------------------------------------
 * Iterator
 * ------------------------------------------------------------------------- */

int PVFDataIterator_Init(PVFDataIterator* iter, const void* data, size_t size) {
    if (iter == NULL || data == NULL || size < 2) {
        return PVF_ERROR_INVALID_PARAMETER;
    }

    iter->data = (const unsigned char*)data;
    iter->size = size;
    iter->position = 0;
    iter->error = 0;

    iter->has_magic = PVFData_HasMagic(data, size);
    if (iter->has_magic) {
        iter->position = 2;
    }

    return PVF_SUCCESS;
}

int PVFDataIterator_Next(PVFDataIterator* iter, PVFDataEntry* entry) {
    if (iter == NULL || entry == NULL) {
        return PVF_ERROR_INVALID_PARAMETER;
    }
    if (!PVFDataIterator_HasNext(iter)) {
        return PVF_ERROR_EOF;
    }

    memset(entry, 0, sizeof(*entry));
    entry->type = PVF_VALUE_TYPE_UNKNOWN;

    size_t start_pos = iter->position;
    uint8_t type_byte = read_u8(iter->data, &iter->position);

    switch (type_byte) {
        case PVF_TYPE_NULL:
            entry->key = 0;
            entry->type = PVF_VALUE_TYPE_NULL;
            return PVF_SUCCESS;

        case PVF_TYPE_INTEGER:
            if (iter->position + 4 > iter->size) goto parse_failed;
            entry->key = 0;
            entry->type = PVF_VALUE_TYPE_INTEGER;
            entry->value.int_value = read_i32(iter->data, &iter->position);
            return PVF_SUCCESS;

        case PVF_TYPE_FLOAT:
            if (iter->position + 4 > iter->size) goto parse_failed;
            entry->key = 0;
            entry->type = PVF_VALUE_TYPE_FLOAT;
            entry->value.float_value = read_f32(iter->data, &iter->position);
            return PVF_SUCCESS;

        case PVF_TYPE_REFERENCE:
            if (iter->position + 2 > iter->size) goto parse_failed;
            entry->key = 0;
            entry->type = PVF_VALUE_TYPE_REFERENCE;
            entry->value.ref_value = (uint32_t)read_u16(iter->data, &iter->position);
            return PVF_SUCCESS;

        case PVF_TYPE_STRING: {
            char* s = NULL;
            uint32_t len = 0;
            int ret = parse_string(iter->data, iter->size, &iter->position, &s, &len);
            if (ret != PVF_SUCCESS) {
                if (s) free(s);
                if (ret == PVF_ERROR_MEMORY) return ret;
                goto parse_failed;
            }
            entry->key = 0;
            entry->type = PVF_VALUE_TYPE_STRING;
            entry->value.str.str = s;
            entry->value.str.length = len;
            return PVF_SUCCESS;
        }

        case PVF_TYPE_KEY_VALUE_PAIR: {
            if (iter->position + 5 > iter->size) goto parse_failed;
            entry->key = (uint32_t)read_u16(iter->data, &iter->position);
            (void)read_u16(iter->data, &iter->position); /* extra */

            uint8_t inner_type = read_u8(iter->data, &iter->position);
            switch (inner_type) {
                case PVF_TYPE_NULL:
                    entry->type = PVF_VALUE_TYPE_NULL;
                    return PVF_SUCCESS;

                case PVF_TYPE_INTEGER:
                    if (iter->position + 4 > iter->size) goto parse_failed;
                    entry->type = PVF_VALUE_TYPE_INTEGER;
                    entry->value.int_value = read_i32(iter->data, &iter->position);
                    return PVF_SUCCESS;

                case PVF_TYPE_FLOAT:
                    if (iter->position + 4 > iter->size) goto parse_failed;
                    entry->type = PVF_VALUE_TYPE_FLOAT;
                    entry->value.float_value = read_f32(iter->data, &iter->position);
                    return PVF_SUCCESS;

                case PVF_TYPE_REFERENCE:
                    if (iter->position + 2 > iter->size) goto parse_failed;
                    entry->type = PVF_VALUE_TYPE_REFERENCE;
                    entry->value.ref_value = (uint32_t)read_u16(iter->data, &iter->position);
                    return PVF_SUCCESS;

                case PVF_TYPE_KEY_VALUE_PAIR:
                    if (iter->position + 2 > iter->size) goto parse_failed;
                    entry->type = PVF_VALUE_TYPE_KV;
                    entry->value.kv.nested_key = (uint32_t)read_u16(iter->data, &iter->position);
                    return PVF_SUCCESS;

                case PVF_TYPE_STRING: {
                    char* s = NULL;
                    uint32_t len = 0;
                    int ret = parse_string(iter->data, iter->size, &iter->position, &s, &len);
                    if (ret != PVF_SUCCESS) {
                        if (s) free(s);
                        if (ret == PVF_ERROR_MEMORY) return ret;
                        goto parse_failed;
                    }
                    entry->type = PVF_VALUE_TYPE_STRING;
                    entry->value.str.str = s;
                    entry->value.str.length = len;
                    return PVF_SUCCESS;
                }

                default:
                    goto parse_failed;
            }
        }

        default:
            goto parse_failed;
    }

parse_failed:
    iter->position = start_pos + 1;
    iter->error = 1;
    return PVF_ERROR_PARSE_FAILED;
}

int PVFDataIterator_HasNext(const PVFDataIterator* iter) {
    if (iter == NULL) return 0;
    return iter->position < iter->size;
}

void PVFDataIterator_Reset(PVFDataIterator* iter) {
    if (iter == NULL) return;
    iter->position = iter->has_magic ? 2 : 0;
    iter->error = 0;
}

size_t PVFDataIterator_GetPosition(const PVFDataIterator* iter) {
    return iter ? iter->position : 0;
}

/* ----------------------------------------------------------------------------
 * Container
 * ------------------------------------------------------------------------- */

int PVFDataContainer_Init(PVFDataContainer* container, int initial_capacity) {
    if (container == NULL || initial_capacity <= 0) {
        return PVF_ERROR_INVALID_PARAMETER;
    }

    container->entries = (PVFDataEntry*)malloc(sizeof(PVFDataEntry) * (size_t)initial_capacity);
    if (container->entries == NULL) {
        return PVF_ERROR_MEMORY;
    }

    container->count = 0;
    container->capacity = initial_capacity;
    return PVF_SUCCESS;
}

int PVFDataContainer_Add(PVFDataContainer* container, const PVFDataEntry* entry) {
    if (container == NULL || entry == NULL) {
        return PVF_ERROR_INVALID_PARAMETER;
    }

    if (container->count >= container->capacity) {
        int new_capacity = (container->capacity > 0) ? (container->capacity * 2) : 64;
        PVFDataEntry* new_entries = (PVFDataEntry*)realloc(container->entries, sizeof(PVFDataEntry) * (size_t)new_capacity);
        if (new_entries == NULL) {
            return PVF_ERROR_MEMORY;
        }
        container->entries = new_entries;
        container->capacity = new_capacity;
    }

    container->entries[container->count++] = *entry;
    return PVF_SUCCESS;
}

PVFDataEntry* PVFDataContainer_Find(const PVFDataContainer* container, uint32_t key) {
    if (container == NULL) {
        return NULL;
    }
    for (int i = 0; i < container->count; i++) {
        if (container->entries[i].key == key) {
            return &container->entries[i];
        }
    }
    return NULL;
}

int32_t PVFDataContainer_GetInt(const PVFDataContainer* container, uint32_t key, int32_t default_value) {
    PVFDataEntry* entry = PVFDataContainer_Find(container, key);
    if (entry != NULL && entry->type == PVF_VALUE_TYPE_INTEGER) {
        return entry->value.int_value;
    }
    return default_value;
}

uint32_t PVFDataContainer_GetRef(const PVFDataContainer* container, uint32_t key, uint32_t default_value) {
    PVFDataEntry* entry = PVFDataContainer_Find(container, key);
    if (entry != NULL && entry->type == PVF_VALUE_TYPE_REFERENCE) {
        return entry->value.ref_value;
    }
    return default_value;
}

float PVFDataContainer_GetFloat(const PVFDataContainer* container, uint32_t key, float default_value) {
    PVFDataEntry* entry = PVFDataContainer_Find(container, key);
    if (entry != NULL && entry->type == PVF_VALUE_TYPE_FLOAT) {
        return entry->value.float_value;
    }
    return default_value;
}

const char* PVFDataContainer_GetString(const PVFDataContainer* container, uint32_t key, const char* default_value) {
    PVFDataEntry* entry = PVFDataContainer_Find(container, key);
    if (entry != NULL && entry->type == PVF_VALUE_TYPE_STRING && entry->value.str.str != NULL) {
        return entry->value.str.str;
    }
    return default_value;
}

void PVFDataContainer_Free(PVFDataContainer* container) {
    if (container == NULL) return;

    if (container->entries != NULL) {
        for (int i = 0; i < container->count; i++) {
            PVFDataEntry* e = &container->entries[i];
            if (e->type == PVF_VALUE_TYPE_STRING && e->value.str.str != NULL) {
                free(e->value.str.str);
                e->value.str.str = NULL;
                e->value.str.length = 0;
            }
        }

        free(container->entries);
        container->entries = NULL;
    }

    container->count = 0;
    container->capacity = 0;
}

int PVFDataContainer_Parse(PVFDataContainer* container, const void* data, size_t size) {
    if (container == NULL || data == NULL) {
        return PVF_ERROR_INVALID_PARAMETER;
    }

    int estimated_count = PVFData_EstimateCount(data, size);
    if (estimated_count <= 0) estimated_count = 64;

    if (container->entries == NULL) {
        int ret = PVFDataContainer_Init(container, estimated_count);
        if (ret != PVF_SUCCESS) {
            return ret;
        }
    }

    PVFDataIterator iter;
    int ret = PVFDataIterator_Init(&iter, data, size);
    if (ret != PVF_SUCCESS) {
        return ret;
    }

    while (PVFDataIterator_HasNext(&iter)) {
        PVFDataEntry e;
        ret = PVFDataIterator_Next(&iter, &e);
        if (ret == PVF_SUCCESS) {
            ret = PVFDataContainer_Add(container, &e);
            if (ret != PVF_SUCCESS) {
                if (e.type == PVF_VALUE_TYPE_STRING && e.value.str.str != NULL) {
                    free(e.value.str.str);
                }
                return ret;
            }
        } else if (ret == PVF_ERROR_EOF) {
            break;
        } else if (ret == PVF_ERROR_MEMORY) {
            return ret;
        } else {
            continue;
        }
    }

    return PVF_SUCCESS;
}

/* ----------------------------------------------------------------------------
 * Utilities
 * ------------------------------------------------------------------------- */

int PVFDataRaw_Validate(const void* data, size_t size) {
    if (data == NULL || size < 2) {
        return 0;
    }

    if (PVFData_HasMagic(data, size)) {
        return 1;
    }

    PVFDataIterator iter;
    PVFDataEntry entry;
    if (PVFDataIterator_Init(&iter, data, size) != PVF_SUCCESS) {
        return 0;
    }
    if (PVFDataIterator_HasNext(&iter)) {
        return (PVFDataIterator_Next(&iter, &entry) == PVF_SUCCESS);
    }
    return 0;
}

int PVFData_HasMagic(const void* data, size_t size) {
    if (data == NULL || size < 2) {
        return 0;
    }
    const unsigned char* bytes = (const unsigned char*)data;
    return (bytes[0] == 0xB0 && bytes[1] == 0xD0);
}

int PVFData_EstimateCount(const void* data, size_t size) {
    if (data == NULL || size < 2) {
        return 0;
    }

    size_t effective_size = size;
    if (PVFData_HasMagic(data, size)) {
        effective_size -= 2;
    }

    return (int)(effective_size / 10) + 10;
}

const char* PVFData_TypeName(PVFValueType type) {
    switch (type) {
        case PVF_VALUE_TYPE_NULL:      return "Null";
        case PVF_VALUE_TYPE_INTEGER:   return "Integer";
        case PVF_VALUE_TYPE_FLOAT:     return "Float";
        case PVF_VALUE_TYPE_KV:        return "KeyValue";
        case PVF_VALUE_TYPE_REFERENCE: return "Reference";
        case PVF_VALUE_TYPE_STRING:    return "String";
        case PVF_VALUE_TYPE_UNKNOWN:   return "Unknown";
        default:                       return "Invalid";
    }
}

