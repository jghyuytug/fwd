/**
 * @file key_mapper.c
 * @brief 键映射助手 - 帮助识别键的真实含义
 *
 * @usage
 *     gcc -m32 -I../../include -o key_mapper key_mapper.c \
 *         ../../src/pvf/*.c
 *     ./key_mapper <pvf_file> <equipment_file>
 */

#include <stdio.h>
#include <stdlib.h>
#include "pvf/pvf.h"
#include "pvf/pvf_data_parser.h"

int main(int argc, char* argv[]) {
    PackSetInternal pack;
    PVFIndexEntry* pvf_entry;
    unsigned char* buffer;
    PVFDataContainer container = {0};
    int result;
    int i;

    if (argc < 3) {
        printf("用法: %s <pvf_file> <equipment_file>\n", argv[0]);
        return 1;
    }

    PVF_Initialize();

    /* 打开PVF */
    result = PackSet_Open(argv[1], &pack);
    if (result != PVF_SUCCESS) {
        printf("错误: 无法打开PVF文件\n");
        return 1;
    }

    /* 提取文件 */
    pvf_entry = PackSet_GetFile(&pack, argv[2]);
    if (pvf_entry == NULL) {
        printf("错误: 文件未找到\n");
        PackSet_Close(&pack);
        return 1;
    }

    buffer = (unsigned char*)malloc(pvf_entry->size);
    result = PackSet_ExtractFile(&pack, pvf_entry, buffer, pvf_entry->size);
    if (result != PVF_SUCCESS) {
        printf("错误: 提取失败\n");
        free(buffer);
        PackSet_Close(&pack);
        return 1;
    }

    /* 解析 */
    result = PVFDataContainer_Parse(&container, buffer, pvf_entry->size);
    if (result != PVF_SUCCESS) {
        printf("错误: 解析失败\n");
        free(buffer);
        PackSet_Close(&pack);
        return 1;
    }

    printf("========================================\n");
    printf("文件: %s\n", argv[2]);
    printf("大小: %u 字节\n", pvf_entry->size);
    printf("键值对: %d 个\n", container.count);
    printf("========================================\n\n");

    printf("所有键值对列表:\n\n");
    printf("序号  键(十六进制)  键(十进制)  类型        值\n");
    printf("----------------------------------------------------------------\n");

    for (i = 0; i < container.count; i++) {
        PVFDataEntry* e = &container.entries[i];

        printf("%3d   0x%08X    %-8u   %-10s  ",
               i + 1, e->key, e->key,
               PVFData_TypeName(e->type));

        switch (e->type) {
            case PVF_VALUE_TYPE_INTEGER:
                printf("%d (0x%X)\n",
                       e->value.int_value,
                       (unsigned int)e->value.int_value);
                break;

            case PVF_VALUE_TYPE_REFERENCE:
                printf("REF:0x%08X (%u)\n",
                       e->value.ref_value,
                       e->value.ref_value);
                break;

            case PVF_VALUE_TYPE_KV:
                printf("KV:{0x%X, 0x%X}\n",
                       e->value.kv.sub_key,
                       e->value.kv.sub_value);
                break;

            default:
                printf("Unknown\n");
        }
    }

    printf("\n========================================\n");
    printf("请将以上数据与游戏内实际装备对比\n");
    printf("来确定每个键的真实含义\n");
    printf("========================================\n");

    /* 清理 */
    PVFDataContainer_Free(&container);
    free(buffer);
    PackSet_Close(&pack);

    return 0;
}
