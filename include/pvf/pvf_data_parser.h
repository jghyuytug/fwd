#ifndef PVF_DATA_PARSER_H
#define PVF_DATA_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file pvf_data_parser.h
 * @brief PVF binary tag/value parser (generic)
 *
 * This module parses the low-level binary key/value stream found in PVF data
 * files (.equ/.skl/.mob/.stk/...).
 */

#include <stddef.h>
#include <stdint.h>

/* Magic number (little-endian): bytes B0 D0 */
#define PVF_DATA_MAGIC_NUMBER 0xD0B0

/* Type bytes (outer type or KV inner type) */
#define PVF_TYPE_NULL           0x00
#define PVF_TYPE_INTEGER        0x02
#define PVF_TYPE_FLOAT          0x04
#define PVF_TYPE_KEY_VALUE_PAIR 0x05
#define PVF_TYPE_REFERENCE      0x07
#define PVF_TYPE_STRING         0x09 /* u32 length + 0x0A marker + bytes */

typedef enum {
    PVF_VALUE_TYPE_NULL      = PVF_TYPE_NULL,
    PVF_VALUE_TYPE_INTEGER   = PVF_TYPE_INTEGER,
    PVF_VALUE_TYPE_FLOAT     = PVF_TYPE_FLOAT,
    PVF_VALUE_TYPE_KV        = PVF_TYPE_KEY_VALUE_PAIR, /* nested marker */
    PVF_VALUE_TYPE_REFERENCE = PVF_TYPE_REFERENCE,
    PVF_VALUE_TYPE_STRING    = PVF_TYPE_STRING,
    PVF_VALUE_TYPE_UNKNOWN   = 0xFF
} PVFValueType;

typedef struct {
    uint32_t key;
    PVFValueType type;

    union {
        int32_t int_value;
        float float_value;
        uint32_t ref_value;
        struct {
            char* str;       /* owned by PVFDataContainer */
            uint32_t length; /* excluding trailing '\\0' */
        } str;
        struct {
            uint32_t nested_key;
        } kv;
    } value;
} PVFDataEntry;

typedef struct {
    const unsigned char* data;
    size_t size;
    size_t position;
    int has_magic;
    int error;
} PVFDataIterator;

typedef struct {
    PVFDataEntry* entries;
    int count;
    int capacity;
} PVFDataContainer;

/* Iterator API */
int PVFDataIterator_Init(PVFDataIterator* iter, const void* data, size_t size);
int PVFDataIterator_Next(PVFDataIterator* iter, PVFDataEntry* entry);
int PVFDataIterator_HasNext(const PVFDataIterator* iter);
void PVFDataIterator_Reset(PVFDataIterator* iter);
size_t PVFDataIterator_GetPosition(const PVFDataIterator* iter);

/* Container API */
int PVFDataContainer_Init(PVFDataContainer* container, int initial_capacity);
int PVFDataContainer_Parse(PVFDataContainer* container, const void* data, size_t size);
int PVFDataContainer_Add(PVFDataContainer* container, const PVFDataEntry* entry);
PVFDataEntry* PVFDataContainer_Find(const PVFDataContainer* container, uint32_t key);

int32_t PVFDataContainer_GetInt(const PVFDataContainer* container, uint32_t key, int32_t default_value);
uint32_t PVFDataContainer_GetRef(const PVFDataContainer* container, uint32_t key, uint32_t default_value);
float PVFDataContainer_GetFloat(const PVFDataContainer* container, uint32_t key, float default_value);
const char* PVFDataContainer_GetString(const PVFDataContainer* container, uint32_t key, const char* default_value);

void PVFDataContainer_Free(PVFDataContainer* container);

/* Utilities */
int PVFDataRaw_Validate(const void* data, size_t size);
int PVFData_HasMagic(const void* data, size_t size);
int PVFData_EstimateCount(const void* data, size_t size);
const char* PVFData_TypeName(PVFValueType type);

#ifdef __cplusplus
}
#endif

#endif /* PVF_DATA_PARSER_H */
