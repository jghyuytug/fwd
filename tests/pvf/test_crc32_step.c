/**
 * @file test_crc32_step.c
 * @brief CRC32逐步调试 - 打印每个字节的计算过程
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* 直接复制CRC32实现进行调试 */

static uint32_t crc_table[256];
static int initialized = 0;

void init_crc_table(void) {
    int i, j;
    uint32_t c;

    if (initialized) return;

    for (i = 0; i < 256; i++) {
        c = (uint32_t)i;
        for (j = 0; j < 8; j++) {
            if (c & 1) {
                c = 0xEDB88320 ^ (c >> 1);
            } else {
                c = c >> 1;
            }
        }
        crc_table[i] = c;
    }
    initialized = 1;
}

uint32_t calc_crc32_debug(const void* data, size_t length) {
    uint32_t crc;
    const uint8_t* bytes;
    size_t i;
    uint8_t table_index;

    init_crc_table();

    crc = 0xFFFFFFFF;
    bytes = (const uint8_t*)data;

    printf("\n  初始CRC: 0x%08x\n\n", crc);

    for (i = 0; i < length; i++) {
        table_index = (crc ^ bytes[i]) & 0xFF;

        printf("  字节[%lu] = 0x%02x ('%c')\n", (unsigned long)i, bytes[i],
               (bytes[i] >= 32 && bytes[i] < 127) ? bytes[i] : '.');
        printf("    table_index = (0x%08x ^ 0x%02x) & 0xFF = 0x%02x\n",
               crc, bytes[i], table_index);
        printf("    table[0x%02x] = 0x%08x\n", table_index, crc_table[table_index]);

        crc = (crc >> 8) ^ crc_table[table_index];

        printf("    新CRC = (0x%08x >> 8) ^ 0x%08x = 0x%08x\n\n",
               crc << 8, crc_table[table_index], crc);
    }

    printf("  最终CRC (取反前): 0x%08x\n", crc);
    crc = ~crc;
    printf("  最终CRC (取反后): 0x%08x\n\n", crc);

    return crc;
}

int main(void) {
    const char* test_string = "Hello, PVF!";
    uint32_t calculated;
    uint32_t expected = 0xe3ca235d;

    printf("========================================\n");
    printf("CRC32逐步调试\n");
    printf("========================================\n");
    printf("\n输入: \"%s\"\n", test_string);
    printf("长度: %lu 字节\n", (unsigned long)strlen(test_string));

    calculated = calc_crc32_debug(test_string, strlen(test_string));

    printf("========================================\n");
    printf("结果对比:\n");
    printf("  C语言: 0x%08x\n", calculated);
    printf("  Python: 0x%08x\n", expected);
    printf("  匹配: %s\n", (calculated == expected) ? "YES ✓" : "NO ✗");
    printf("========================================\n\n");

    return (calculated == expected) ? 0 : 1;
}
