/**
 * @file test_crc32_debug.c
 * @brief CRC32调试程序 - 对比Python结果
 */

#include <stdio.h>
#include <string.h>
#include "pvf/pvf_crc32.h"

int main(void) {
    const char* test_string = "Hello, PVF!";
    uint32_t calculated;
    uint32_t expected = 0xe3ca235d;  /* Python zlib.crc32() 结果 */

    printf("========================================\n");
    printf("CRC32调试测试\n");
    printf("========================================\n\n");

    /* 初始化CRC32表 */
    PVF_InitCRC32Table();
    printf("[1] CRC32表初始化完成\n");
    printf("    前4个条目:\n");
    printf("    [0] = 0x%08x\n", g_crc32_table[0]);
    printf("    [1] = 0x%08x\n", g_crc32_table[1]);
    printf("    [2] = 0x%08x\n", g_crc32_table[2]);
    printf("    [3] = 0x%08x\n", g_crc32_table[3]);

    /* 计算CRC32 */
    printf("\n[2] 计算CRC32\n");
    printf("    输入字符串: \"%s\"\n", test_string);
    printf("    长度: %lu 字节\n", (unsigned long)strlen(test_string));

    calculated = PVF_CalcCRC32(test_string, strlen(test_string));

    printf("\n[3] 结果对比\n");
    printf("    C语言计算: 0x%08x\n", calculated);
    printf("    Python结果: 0x%08x\n", expected);

    if (calculated == expected) {
        printf("\n[SUCCESS] CRC32计算正确! ✓\n\n");
        return 0;
    } else {
        printf("\n[FAILED] CRC32计算错误\n");
        printf("    差值: 0x%08x\n\n", calculated ^ expected);
        return 1;
    }
}
