/**
 * @file pvf_data_parser.c
 * @brief PVF数据解析器实现
 */

#include "pvf/pvf_data_parser.h"
#include "pvf/pvf_types.h"
#include <stdlib.h>
#include <string.h>

/* ========================================================================== */
/* 内部辅助函数                                                               */
/* ========================================================================== */

/**
 * @brief 从缓冲区读取uint8
 */
static inline uint8_t read_uint8(const unsigned char* data, size_t* pos) {
    return data[(*pos)++];
}

/**
 * @brief 从缓冲区读取uint32（小端序）
 */
static inline uint32_t read_uint32(const unsigned char* data, size_t* pos) {
    uint32_t value = *(const uint32_t*)(data + *pos);
    *pos += 4;
    return value;
}

/**
 * @brief 从缓冲区读取int32（小端序）
 */
static inline int32_t read_int32(const unsigned char* data, size_t* pos) {
    int32_t value = *(const int32_t*)(data + *pos);
    *pos += 4;
    return value;
}

/* ========================================================================== */
/* 迭代器实现                                                                 */
/* ========================================================================== */

int PVFDataIterator_Init(PVFDataIterator* iter, const void* data, size_t size) {
    if (iter == NULL || data == NULL || size < 2) {
        return PVF_ERROR_INVALID_PARAMETER;
    }

    iter->data = (const unsigned char*)data;
    iter->size = size;
    iter->position = 0;
    iter->error = 0;

    /* 检查魔数 */
    iter->has_magic = PVFData_HasMagic(data, size);
    if (iter->has_magic) {
        iter->position = 2;  /* 跳过魔数 */
    }

    return PVF_SUCCESS;
}

int PVFDataIterator_Next(PVFDataIterator* iter, PVFDataEntry* entry) {
    if (iter == NULL || entry == NULL) {
        return PVF_ERROR_INVALID_PARAMETER;
    }

    if (iter->error) {
        return PVF_ERROR_PARSE_FAILED;
    }

    if (!PVFDataIterator_HasNext(iter)) {
        return PVF_ERROR_EOF;
    }

    /* 读取类型标记 */
    uint8_t type_byte = read_uint8(iter->data, &iter->position);

    switch (type_byte) {
        case PVF_TYPE_KEY_VALUE_PAIR: {
            /* 0x05: [键][类型][值] */
            if (iter->position + 8 > iter->size) {
                iter->error = 1;
                return PVF_ERROR_PARSE_FAILED;
            }

            entry->key = read_uint32(iter->data, &iter->position);

            /* 读取内部类型 */
            uint8_t inner_type = read_uint8(iter->data, &iter->position);
            uint32_t inner_value = read_uint32(iter->data, &iter->position);

            if (inner_type == PVF_TYPE_INTEGER) {
                entry->type = PVF_VALUE_TYPE_INTEGER;
                entry->value.int_value = (int32_t)inner_value;
            } else if (inner_type == PVF_TYPE_REFERENCE) {
                entry->type = PVF_VALUE_TYPE_REFERENCE;
                entry->value.ref_value = inner_value;
            } else if (inner_type == PVF_TYPE_KEY_VALUE_PAIR) {
                /* 嵌套键值对 */
                entry->type = PVF_VALUE_TYPE_KV;
                entry->value.kv.sub_key = inner_type;
                entry->value.kv.sub_value = inner_value;
            } else {
                /* 未知类型，作为引用处理 */
                entry->type = PVF_VALUE_TYPE_REFERENCE;
                entry->value.ref_value = inner_value;
            }
            break;
        }

        case PVF_TYPE_INTEGER: {
            /* 0x02: [值] */
            if (iter->position + 4 > iter->size) {
                iter->error = 1;
                return PVF_ERROR_PARSE_FAILED;
            }

            entry->key = 0;  /* 无键 */
            entry->type = PVF_VALUE_TYPE_INTEGER;
            entry->value.int_value = read_int32(iter->data, &iter->position);
            break;
        }

        case PVF_TYPE_REFERENCE: {
            /* 0x07: [值] */
            if (iter->position + 4 > iter->size) {
                iter->error = 1;
                return PVF_ERROR_PARSE_FAILED;
            }

            entry->key = 0;  /* 无键 */
            entry->type = PVF_VALUE_TYPE_REFERENCE;
            entry->value.ref_value = read_uint32(iter->data, &iter->position);
            break;
        }

        default:
            /* 未知类型，回退一字节并尝试继续 */
            iter->position--;
            iter->error = 1;
            return PVF_ERROR_PARSE_FAILED;
    }

    return PVF_SUCCESS;
}

int PVFDataIterator_HasNext(const PVFDataIterator* iter) {
    if (iter == NULL || iter->error) {
        return 0;
    }
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

/* ========================================================================== */
/* 容器实现                                                                   */
/* ========================================================================== */

int PVFDataContainer_Init(PVFDataContainer* container, int initial_capacity) {
    if (container == NULL || initial_capacity <= 0) {
        return PVF_ERROR_INVALID_PARAMETER;
    }

    container->entries = (PVFDataEntry*)malloc(
        sizeof(PVFDataEntry) * initial_capacity
    );

    if (container->entries == NULL) {
        return PVF_ERROR_MEMORY;
    }

    container->count = 0;
    container->capacity = initial_capacity;

    return PVF_SUCCESS;
}

int PVFDataContainer_Parse(PVFDataContainer* container,
                            const void* data, size_t size) {
    PVFDataIterator iter;
    PVFDataEntry entry;
    int result;

    if (container == NULL || data == NULL) {
        return PVF_ERROR_INVALID_PARAMETER;
    }

    /* 估算容量 */
    int estimated_count = PVFData_EstimateCount(data, size);
    if (container->entries == NULL) {
        result = PVFDataContainer_Init(container, estimated_count);
        if (result != PVF_SUCCESS) {
            return result;
        }
    }

    /* 初始化迭代器 */
    result = PVFDataIterator_Init(&iter, data, size);
    if (result != PVF_SUCCESS) {
        return result;
    }

    /* 迭代解析 */
    while (PVFDataIterator_HasNext(&iter)) {
        result = PVFDataIterator_Next(&iter, &entry);
        if (result == PVF_SUCCESS) {
            result = PVFDataContainer_Add(container, &entry);
            if (result != PVF_SUCCESS) {
                return result;
            }
        } else if (result == PVF_ERROR_EOF) {
            break;
        } else {
            /* 解析错误，但继续尝试 */
            continue;
        }
    }

    return PVF_SUCCESS;
}

int PVFDataContainer_Add(PVFDataContainer* container,
                         const PVFDataEntry* entry) {
    if (container == NULL || entry == NULL) {
        return PVF_ERROR_INVALID_PARAMETER;
    }

    /* 扩容检查 */
    if (container->count >= container->capacity) {
        int new_capacity = container->capacity * 2;
        PVFDataEntry* new_entries = (PVFDataEntry*)realloc(
            container->entries,
            sizeof(PVFDataEntry) * new_capacity
        );

        if (new_entries == NULL) {
            return PVF_ERROR_MEMORY;
        }

        container->entries = new_entries;
        container->capacity = new_capacity;
    }

    /* 添加条目 */
    container->entries[container->count++] = *entry;

    return PVF_SUCCESS;
}

PVFDataEntry* PVFDataContainer_Find(const PVFDataContainer* container,
                                     uint32_t key) {
    int i;

    if (container == NULL) {
        return NULL;
    }

    /* 线性查找 */
    for (i = 0; i < container->count; i++) {
        if (container->entries[i].key == key) {
            return &container->entries[i];
        }
    }

    return NULL;
}

int32_t PVFDataContainer_GetInt(const PVFDataContainer* container,
                                 uint32_t key, int32_t default_value) {
    PVFDataEntry* entry = PVFDataContainer_Find(container, key);

    if (entry != NULL && entry->type == PVF_VALUE_TYPE_INTEGER) {
        return entry->value.int_value;
    }

    return default_value;
}

uint32_t PVFDataContainer_GetRef(const PVFDataContainer* container,
                                  uint32_t key, uint32_t default_value) {
    PVFDataEntry* entry = PVFDataContainer_Find(container, key);

    if (entry != NULL && entry->type == PVF_VALUE_TYPE_REFERENCE) {
        return entry->value.ref_value;
    }

    return default_value;
}

void PVFDataContainer_Free(PVFDataContainer* container) {
    if (container == NULL) return;

    if (container->entries != NULL) {
        free(container->entries);
        container->entries = NULL;
    }

    container->count = 0;
    container->capacity = 0;
}

/* ========================================================================== */
/* 实用函数实现                                                               */
/* ========================================================================== */

int PVFDataRaw_Validate(const void* data, size_t size) {
    if (data == NULL || size < 2) {
        return 0;
    }

    /* 基本验证：检查魔数或至少能解析一个条目 */
    if (PVFData_HasMagic(data, size)) {
        return 1;  /* 有魔数就认为有效 */
    }

    /* 尝试解析 */
    if (size < 5) {
        return 0;  /* 太小，无法包含完整条目 */
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

    /* 平均每个条目约10字节 */
    size_t effective_size = size;

    if (PVFData_HasMagic(data, size)) {
        effective_size -= 2;
    }

    return (int)(effective_size / 10) + 10;  /* +10 预留 */
}

const char* PVFData_TypeName(PVFValueType type) {
    switch (type) {
        case PVF_VALUE_TYPE_KV:        return "KeyValue";
        case PVF_VALUE_TYPE_INTEGER:   return "Integer";
        case PVF_VALUE_TYPE_REFERENCE: return "Reference";
        case PVF_VALUE_TYPE_UNKNOWN:   return "Unknown";
        default:                        return "Invalid";
    }
}
