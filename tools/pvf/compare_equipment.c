/**
 * @file compare_equipment.c
 * @brief 对比多个装备文件的键值
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pvf/pvf.h"
#include "pvf/pvf_data_parser.h"

typedef struct {
    uint32_t key;
    int count;
    int min_val;
    int max_val;
} KeyStat;

int main(int argc, char* argv[]) {
    PackSetInternal pack;
    KeyStat stats[256] = {0};
    int stat_count = 0;
    int i, j;

    if (argc < 2) {
        printf("用法: %s <pvf_file>\n", argv[0]);
        return 1;
    }

    PVF_Initialize();

    if (PackSet_Open(argv[1], &pack) != PVF_SUCCESS) {
        printf("错误: 无法打开PVF文件\n");
        return 1;
    }

    printf("扫描装备文件...\n\n");

    /* 扫描前100个.equ文件 */
    int processed = 0;
    for (i = 0; i < (int)pack.header.index_count && processed < 100; i++) {
        if (strstr(pack.indexes[i].name, ".equ") == NULL) {
            continue;
        }

        PVFIndexEntry* pvf_entry = &pack.indexes[i];
        unsigned char* buffer = (unsigned char*)malloc(pvf_entry->size);

        if (PackSet_ExtractFile(&pack, pvf_entry, buffer, pvf_entry->size) == PVF_SUCCESS) {
            PVFDataContainer container = {0};

            if (PVFDataContainer_Parse(&container, buffer, pvf_entry->size) == PVF_SUCCESS) {
                /* 统计每个键 */
                for (j = 0; j < container.count; j++) {
                    PVFDataEntry* e = &container.entries[j];

                    if (e->key == 0) continue;  /* 跳过键=0 */

                    /* 查找或添加统计 */
                    int found = 0;
                    int k;
                    for (k = 0; k < stat_count; k++) {
                        if (stats[k].key == e->key) {
                            stats[k].count++;

                            int val = (e->type == PVF_VALUE_TYPE_INTEGER)
                                ? e->value.int_value
                                : (int)e->value.ref_value;

                            if (val < stats[k].min_val) stats[k].min_val = val;
                            if (val > stats[k].max_val) stats[k].max_val = val;

                            found = 1;
                            break;
                        }
                    }

                    if (!found && stat_count < 256) {
                        stats[stat_count].key = e->key;
                        stats[stat_count].count = 1;

                        int val = (e->type == PVF_VALUE_TYPE_INTEGER)
                            ? e->value.int_value
                            : (int)e->value.ref_value;

                        stats[stat_count].min_val = val;
                        stats[stat_count].max_val = val;
                        stat_count++;
                    }
                }

                PVFDataContainer_Free(&container);
            }
        }

        free(buffer);
        processed++;
    }

    printf("已扫描 %d 个装备文件\n\n", processed);
    printf("键统计（按出现频率排序）:\n\n");
    printf("键(十六进制)  出现次数  最小值      最大值      可能含义\n");
    printf("----------------------------------------------------------------\n");

    /* 简单排序 */
    int k;
    for (i = 0; i < stat_count - 1; i++) {
        for (j = i + 1; j < stat_count; j++) {
            if (stats[j].count > stats[i].count) {
                KeyStat temp = stats[i];
                stats[i] = stats[j];
                stats[j] = temp;
            }
        }
    }

    /* 显示前30个 */
    for (k = 0; k < 30 && k < stat_count; k++) {
        const char* guess = "";

        /* 基于范围猜测 */
        if (stats[k].max_val <= 10) {
            guess = "稀有度?";
        } else if (stats[k].max_val <= 100) {
            guess = "等级/职业?";
        } else if (stats[k].max_val <= 10000) {
            guess = "属性值?";
        }

        printf("0x%08X    %4d      %-10d  %-10d  %s\n",
               stats[k].key,
               stats[k].count,
               stats[k].min_val,
               stats[k].max_val,
               guess);
    }

    PackSet_Close(&pack);
    return 0;
}
