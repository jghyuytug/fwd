/**
 * @file find_equipment.c
 * @brief 根据属性查找装备
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pvf/pvf.h"
#include "pvf/pvf_data_parser.h"

int main(int argc, char* argv[]) {
    PackSetInternal pack;
    int i;
    int found_count = 0;

    if (argc < 2) {
        printf("用法: %s <pvf_file> [search_value]\n", argv[0]);
        printf("示例: %s Script.pvf 70  (查找可能等级=70的装备)\n", argv[0]);
        return 1;
    }

    PVF_Initialize();

    if (PackSet_Open(argv[1], &pack) != PVF_SUCCESS) {
        printf("错误: 无法打开PVF\n");
        return 1;
    }

    int search_value = (argc >= 3) ? atoi(argv[2]) : 70;
    printf("搜索包含值 %d 的装备...\n\n", search_value);

    /* 扫描腰带目录 */
    for (i = 0; i < (int)pack.header.index_count && found_count < 20; i++) {
        char* name = pack.indexes[i].name;

        /* 只看腰带 */
        if (strstr(name, "belt/") == NULL || strstr(name, ".equ") == NULL) {
            continue;
        }

        PVFIndexEntry* entry = &pack.indexes[i];
        unsigned char* buffer = (unsigned char*)malloc(entry->size);

        if (PackSet_ExtractFile(&pack, entry, buffer, entry->size) == PVF_SUCCESS) {
            PVFDataContainer container = {0};

            if (PVFDataContainer_Parse(&container, buffer, entry->size) == PVF_SUCCESS) {
                /* 查找包含目标值的条目 */
                int matched = 0;
                int j;

                for (j = 0; j < container.count; j++) {
                    PVFDataEntry* e = &container.entries[j];

                    int val = 0;
                    if (e->type == PVF_VALUE_TYPE_INTEGER) {
                        val = e->value.int_value;
                    } else if (e->type == PVF_VALUE_TYPE_REFERENCE) {
                        val = (int)e->value.ref_value;
                    }

                    if (val == search_value || val == search_value * 100) {
                        matched = 1;
                        break;
                    }
                }

                if (matched) {
                    printf("[%d] %s\n", found_count + 1, name);

                    /* 显示所有INTEGER类型的值 */
                    printf("    Integer值: ");
                    int count = 0;
                    for (j = 0; j < container.count && count < 10; j++) {
                        PVFDataEntry* e = &container.entries[j];
                        if (e->type == PVF_VALUE_TYPE_INTEGER && e->key != 0) {
                            printf("%d ", e->value.int_value);
                            count++;
                        }
                    }
                    printf("\n\n");

                    found_count++;
                }

                PVFDataContainer_Free(&container);
            }
        }

        free(buffer);
    }

    printf("共找到 %d 个匹配的装备\n", found_count);

    PackSet_Close(&pack);
    return 0;
}
