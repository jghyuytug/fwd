/**
 * @file pvf_stringtable.c
 * @brief PVF StringTable 加载器实现
 *
 * @date 2025-11-25
 */

#include "pvf/pvf_stringtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========================================================================== */
/* 初始化与释放                                                                */
/* ========================================================================== */

void StringTable_Init(PVFStringTable* st) {
    if (st == NULL) {
        return;
    }

    memset(st, 0, sizeof(PVFStringTable));
    st->loaded = 0;
}

void StringTable_Free(PVFStringTable* st) {
    if (st == NULL) {
        return;
    }

    if (st->indices != NULL) {
        free(st->indices);
        st->indices = NULL;
    }

    if (st->string_data != NULL) {
        free(st->string_data);
        st->string_data = NULL;
    }

    st->count = 0;
    st->data_size = 0;
    st->loaded = 0;
}

/* ========================================================================== */
/* 加载函数                                                                    */
/* ========================================================================== */

int StringTable_LoadFromFile(PVFStringTable* st, const char* filepath) {
    FILE* fp;
    uint32_t count;
    size_t header_size;
    size_t file_size;
    size_t data_size;
    size_t read_size;

    if (st == NULL || filepath == NULL) {
        return STRINGTABLE_ERROR_NULL_PTR;
    }

    /* 释放之前的数据 */
    StringTable_Free(st);

    /* 打开文件 */
    fp = fopen(filepath, "rb");
    if (fp == NULL) {
        return STRINGTABLE_ERROR_FILE_OPEN;
    }

    /* 获取文件大小 */
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    /* 读取字符串数量 */
    if (fread(&count, sizeof(uint32_t), 1, fp) != 1) {
        fclose(fp);
        return STRINGTABLE_ERROR_FILE_READ;
    }

    /* 验证数据 */
    header_size = sizeof(uint32_t) + count * sizeof(uint32_t);
    if (header_size > file_size) {
        fclose(fp);
        return STRINGTABLE_ERROR_INVALID;
    }

    st->count = count;

    /* 分配索引表内存 */
    st->indices = (uint32_t*)malloc(count * sizeof(uint32_t));
    if (st->indices == NULL) {
        fclose(fp);
        StringTable_Free(st);
        return STRINGTABLE_ERROR_OUT_OF_MEM;
    }

    /* 读取索引表 */
    read_size = fread(st->indices, sizeof(uint32_t), count, fp);
    if (read_size != count) {
        fclose(fp);
        StringTable_Free(st);
        return STRINGTABLE_ERROR_FILE_READ;
    }

    /* 计算字符串数据区大小 */
    data_size = file_size - header_size;
    st->data_size = data_size;

    /* 分配字符串数据区内存 */
    st->string_data = (char*)malloc(data_size);
    if (st->string_data == NULL) {
        fclose(fp);
        StringTable_Free(st);
        return STRINGTABLE_ERROR_OUT_OF_MEM;
    }

    /* 读取字符串数据区 */
    read_size = fread(st->string_data, 1, data_size, fp);
    if (read_size != data_size) {
        fclose(fp);
        StringTable_Free(st);
        return STRINGTABLE_ERROR_FILE_READ;
    }

    fclose(fp);
    st->loaded = 1;

    return STRINGTABLE_SUCCESS;
}

int StringTable_LoadFromBuffer(PVFStringTable* st, const void* buffer, size_t size) {
    const uint8_t* data;
    uint32_t count;
    size_t header_size;
    size_t data_size;

    if (st == NULL || buffer == NULL || size == 0) {
        return STRINGTABLE_ERROR_NULL_PTR;
    }

    /* 释放之前的数据 */
    StringTable_Free(st);

    data = (const uint8_t*)buffer;

    /* 读取字符串数量 */
    if (size < sizeof(uint32_t)) {
        return STRINGTABLE_ERROR_INVALID;
    }
    memcpy(&count, data, sizeof(uint32_t));

    /* 验证数据 */
    header_size = sizeof(uint32_t) + count * sizeof(uint32_t);
    if (header_size > size) {
        return STRINGTABLE_ERROR_INVALID;
    }

    st->count = count;

    /* 分配索引表内存 */
    st->indices = (uint32_t*)malloc(count * sizeof(uint32_t));
    if (st->indices == NULL) {
        StringTable_Free(st);
        return STRINGTABLE_ERROR_OUT_OF_MEM;
    }

    /* 复制索引表 */
    memcpy(st->indices, data + sizeof(uint32_t), count * sizeof(uint32_t));

    /* 计算字符串数据区大小 */
    data_size = size - header_size;
    st->data_size = data_size;

    /* 分配字符串数据区内存 */
    st->string_data = (char*)malloc(data_size);
    if (st->string_data == NULL) {
        StringTable_Free(st);
        return STRINGTABLE_ERROR_OUT_OF_MEM;
    }

    /* 复制字符串数据区 */
    memcpy(st->string_data, data + header_size, data_size);

    st->loaded = 1;

    return STRINGTABLE_SUCCESS;
}

/* ========================================================================== */
/* 查询函数                                                                    */
/* ========================================================================== */

int StringTable_IsValidIndex(const PVFStringTable* st, uint32_t index) {
    if (st == NULL || !st->loaded) {
        return 0;
    }

    return (index < st->count);
}

const char* StringTable_GetString(const PVFStringTable* st, uint32_t index) {
    uint32_t offset;
    size_t header_size;
    size_t relative_offset;

    if (st == NULL || !st->loaded) {
        return NULL;
    }

    if (index >= st->count) {
        return NULL;
    }

    /* 获取字符串偏移 */
    offset = st->indices[index];

    /* 计算相对于数据区的偏移 */
    header_size = sizeof(uint32_t) + st->count * sizeof(uint32_t);

    if (offset < header_size) {
        return NULL;
    }

    relative_offset = offset - header_size;

    if (relative_offset >= st->data_size) {
        return NULL;
    }

    return st->string_data + relative_offset;
}

int StringTable_GetTag(const PVFStringTable* st, uint32_t index,
                       char* tag_buf, size_t buf_size) {
    const char* str;
    const char* tag_start;
    const char* tag_end;
    size_t tag_len;

    if (st == NULL || tag_buf == NULL || buf_size == 0) {
        return STRINGTABLE_ERROR_NULL_PTR;
    }

    if (!st->loaded) {
        return STRINGTABLE_ERROR_NOT_LOADED;
    }

    /* 清空缓冲区 */
    tag_buf[0] = '\0';

    /* 获取原始字符串 */
    str = StringTable_GetString(st, index);
    if (str == NULL) {
        return STRINGTABLE_ERROR_INVALID;
    }

    /* 跳过前4个字节，查找 '[' */
    if (strlen(str) < STRINGTABLE_TAG_OFFSET) {
        return STRINGTABLE_ERROR_INVALID;
    }

    tag_start = str + STRINGTABLE_TAG_OFFSET;

    /* 查找 '[' */
    if (*tag_start != '[') {
        /* 没有标签，返回空 */
        return 0;
    }

    /* 查找 ']' */
    tag_end = strchr(tag_start, ']');
    if (tag_end == NULL) {
        return STRINGTABLE_ERROR_INVALID;
    }

    /* 计算标签长度（包括 '[' 和 ']'） */
    tag_len = tag_end - tag_start + 1;

    if (tag_len >= buf_size) {
        tag_len = buf_size - 1;
    }

    /* 复制标签 */
    strncpy(tag_buf, tag_start, tag_len);
    tag_buf[tag_len] = '\0';

    return (int)tag_len;
}

/* ========================================================================== */
/* 全局实例（可选）                                                            */
/* ========================================================================== */

static PVFStringTable g_stringtable = {0};
static int g_stringtable_initialized = 0;

/**
 * @brief 获取全局 StringTable 实例
 */
PVFStringTable* StringTable_GetGlobal(void) {
    if (!g_stringtable_initialized) {
        StringTable_Init(&g_stringtable);
        g_stringtable_initialized = 1;
    }
    return &g_stringtable;
}

/**
 * @brief 加载全局 StringTable
 */
int StringTable_LoadGlobal(const char* filepath) {
    PVFStringTable* st = StringTable_GetGlobal();
    return StringTable_LoadFromFile(st, filepath);
}

/**
 * @brief 释放全局 StringTable
 */
void StringTable_FreeGlobal(void) {
    if (g_stringtable_initialized) {
        StringTable_Free(&g_stringtable);
    }
}

/**
 * @brief 使用全局实例查询标签
 */
int StringTable_GetTagGlobal(uint32_t index, char* tag_buf, size_t buf_size) {
    return StringTable_GetTag(&g_stringtable, index, tag_buf, buf_size);
}
