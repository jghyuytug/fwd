/**
 * @file format_analyzer.c
 * @brief PVF数据格式分析器 - 深度分析文件格式
 *
 * @usage
 *     gcc -m32 -I../../include -o format_analyzer format_analyzer.c \
 *         ../../src/pvf/pvf_crc32.c \
 *         ../../src/pvf/pvf_decrypt.c \
 *         ../../src/pvf/pvf_hash.c \
 *         ../../src/pvf/pvf_parser.c \
 *         ../../src/pvf/pvf_packset.c
 *     ./format_analyzer <pvf_file> <target_file>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pvf/pvf.h"

/* ========================================================================== */
/* 格式分析                                                                   */
/* ========================================================================== */

/**
 * @brief 分析字节模式
 */
static void analyze_byte_patterns(const unsigned char* data, size_t size) {
    size_t i;
    unsigned char byte_freq[256] = {0};

    printf("\n========================================\n");
    printf("字节频率分析\n");
    printf("========================================\n\n");

    /* 统计字节频率 */
    for (i = 0; i < size; i++) {
        byte_freq[data[i]]++;
    }

    /* 显示高频字节 */
    printf("高频字节（出现次数 > 5）:\n\n");
    printf("字节值  十六进制  十进制  ASCII  出现次数\n");
    printf("------------------------------------------------\n");

    for (i = 0; i < 256; i++) {
        if (byte_freq[i] > 5) {
            printf("  0x%02x      %3d      %c      %3u\n",
                   (unsigned int)i,
                   (unsigned int)i,
                   isprint(i) ? (char)i : '.',
                   byte_freq[i]);
        }
    }
}

/**
 * @brief 尝试解析键值对结构
 *
 * 观察到的模式：
 * - 0x05 后面通常跟着键（4字节整数）然后是值
 * - 0x02 后面通常跟着4字节整数值
 * - 0x07 后面可能是另一种类型的值
 */
static void parse_key_value_structure(const unsigned char* data, size_t size) {
    size_t pos = 0;
    int entry_count = 0;

    printf("\n========================================\n");
    printf("键值对结构分析\n");
    printf("========================================\n\n");

    printf("偏移量  类型  键(hex)    值(hex/int)\n");
    printf("------------------------------------------------\n");

    while (pos < size && entry_count < 50) {  /* 只显示前50个条目 */
        unsigned char type = data[pos];

        if (type == 0x05 || type == 0x02 || type == 0x07) {
            printf("0x%04x  0x%02x  ", (unsigned int)pos, type);
            pos++;

            if (pos + 4 <= size) {
                /* 读取4字节键/值 */
                uint32_t key = *(uint32_t*)&data[pos];
                printf("%08x  ", key);
                pos += 4;

                if (type == 0x05 && pos + 4 <= size) {
                    /* 0x05 后面可能还有值 */
                    uint32_t value = *(uint32_t*)&data[pos];
                    printf("%08x (%d)", value, (int)value);
                    pos += 4;
                } else if (type == 0x02) {
                    /* 0x02 的值已经读取 */
                    printf("(int: %d)", (int)key);
                } else if (type == 0x07) {
                    /* 0x07 的值已经读取 */
                    printf("(int: %d)", (int)key);
                }

                printf("\n");
                entry_count++;
            } else {
                printf("数据不足\n");
                break;
            }
        } else {
            pos++;
        }
    }

    if (pos < size) {
        printf("... 还有 %zu 字节未显示\n", size - pos);
    }
}

/**
 * @brief 查找可打印字符串
 */
static void find_strings(const unsigned char* data, size_t size) {
    size_t i;
    char str_buffer[256];
    int str_len = 0;

    printf("\n========================================\n");
    printf("可读字符串搜索\n");
    printf("========================================\n\n");

    printf("偏移量    内容\n");
    printf("------------------------------------------------\n");

    for (i = 0; i < size; i++) {
        if (isprint(data[i]) || data[i] == ' ') {
            str_buffer[str_len++] = data[i];
            if (str_len >= 255) {
                str_buffer[255] = '\0';
                if (str_len >= 4) {  /* 至少4个字符 */
                    printf("0x%04x    %s\n", (unsigned int)(i - str_len + 1), str_buffer);
                }
                str_len = 0;
            }
        } else {
            if (str_len >= 4) {  /* 至少4个字符 */
                str_buffer[str_len] = '\0';
                printf("0x%04x    %s\n", (unsigned int)(i - str_len), str_buffer);
            }
            str_len = 0;
        }
    }
}

/**
 * @brief 十六进制转储
 */
static void hex_dump(const unsigned char* data, size_t size, size_t max_lines) {
    size_t i;
    size_t lines = 0;

    printf("\n========================================\n");
    printf("十六进制转储（前%zu行）\n", max_lines);
    printf("========================================\n\n");

    for (i = 0; i < size && lines < max_lines; i += 16) {
        size_t j;
        printf("%08x  ", (unsigned int)i);

        /* 十六进制 */
        for (j = 0; j < 16 && i + j < size; j++) {
            printf("%02x ", data[i + j]);
            if (j == 7) printf(" ");
        }

        /* 填充 */
        for (; j < 16; j++) {
            printf("   ");
            if (j == 7) printf(" ");
        }

        printf(" |");

        /* ASCII */
        for (j = 0; j < 16 && i + j < size; j++) {
            unsigned char c = data[i + j];
            printf("%c", isprint(c) ? c : '.');
        }

        printf("|\n");
        lines++;
    }

    if (i < size) {
        printf("... 还有 %zu 字节\n", size - i);
    }
}

/* ========================================================================== */
/* 主函数                                                                      */
/* ========================================================================== */

int main(int argc, char* argv[]) {
    PackSetInternal pack;
    PVFIndexEntry* entry;
    unsigned char* buffer;
    int result;
    const char* pvf_path;
    const char* target_file;

    printf("========================================\n");
    printf("PVF 数据格式分析器\n");
    printf("版本: %s\n", PVF_GetVersion());
    printf("========================================\n");

    /* 检查参数 */
    if (argc < 3) {
        printf("\n用法: %s <pvf_file> <target_file>\n", argv[0]);
        printf("示例: %s Script.pvf equipment/character/common/wrist/brac_2choro994.equ\n", argv[0]);
        return 1;
    }

    pvf_path = argv[1];
    target_file = argv[2];

    /* 初始化 */
    PVF_Initialize();

    /* 打开PVF */
    printf("\n打开PVF文件: %s\n", pvf_path);
    result = PackSet_Open(pvf_path, &pack);
    if (result != PVF_SUCCESS) {
        printf("错误: 无法打开PVF文件 (错误码: %d)\n", result);
        return 1;
    }

    printf("成功! 文件数量: %u\n", pack.header.index_count);

    /* 查找目标文件 */
    printf("\n查找文件: %s\n", target_file);
    entry = PackSet_GetFile(&pack, target_file);
    if (entry == NULL) {
        printf("错误: 文件未找到\n");
        PackSet_Close(&pack);
        return 1;
    }

    printf("找到! 文件大小: %u 字节, Hash: 0x%08x\n",
           entry->size, entry->hash_value);

    /* 提取文件 */
    buffer = (unsigned char*)malloc(entry->size);
    if (buffer == NULL) {
        printf("错误: 内存分配失败\n");
        PackSet_Close(&pack);
        return 1;
    }

    result = PackSet_ExtractFile(&pack, entry, buffer, entry->size);
    if (result != PVF_SUCCESS) {
        printf("错误: 提取失败 (错误码: %d)\n", result);
        free(buffer);
        PackSet_Close(&pack);
        return 1;
    }

    printf("提取成功!\n");

    /* 执行分析 */
    hex_dump(buffer, entry->size, 20);
    analyze_byte_patterns(buffer, entry->size);
    parse_key_value_structure(buffer, entry->size);
    find_strings(buffer, entry->size);

    /* 清理 */
    free(buffer);
    PackSet_Close(&pack);

    printf("\n========================================\n");
    printf("分析完成!\n");
    printf("========================================\n\n");

    return 0;
}
