/**
 * @file auto_map_keys.c
 * @brief 自动发现文本标签与二进制键的映射关系
 *
 * @description
 * 通过对比sample文件（文本格式）中的标签值和实际装备文件（二进制格式）
 * 中的键值，自动建立映射关系。
 *
 * @algorithm
 * 1. 从命令行接收"标签 => 值"对（来自sample文件）
 * 2. 扫描多个二进制装备文件
 * 3. 查找包含该值的INTEGER键
 * 4. 统计每个键出现该值的次数
 * 5. 输出最可能的映射关系
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pvf/pvf.h"
#include "pvf/pvf_data_parser.h"

#define MAX_CANDIDATES 20

typedef struct {
    uint32_t key;
    int match_count;
    int total_count;
    float confidence;  /* 匹配率 */
} KeyCandidate;

/**
 * @brief 在装备文件中查找包含指定值的键
 */
int find_keys_with_value(PackSetInternal* pack,
                          const char* filepath,
                          int32_t target_value,
                          KeyCandidate* candidates,
                          int* candidate_count) {
    PVFIndexEntry* entry;
    unsigned char* buffer;
    PVFDataContainer container = {0};
    int result;
    int i, j;

    entry = PackSet_GetFile(pack, filepath);
    if (entry == NULL) return -1;

    buffer = (unsigned char*)malloc(entry->size);
    result = PackSet_ExtractFile(pack, entry, buffer, entry->size);
    if (result != PVF_SUCCESS) {
        free(buffer);
        return -1;
    }

    result = PVFDataContainer_Parse(&container, buffer, entry->size);
    if (result != PVF_SUCCESS) {
        free(buffer);
        return -1;
    }

    /* 查找包含目标值的INTEGER键 */
    for (i = 0; i < container.count; i++) {
        PVFDataEntry* e = &container.entries[i];

        if (e->key == 0 || e->type != PVF_VALUE_TYPE_INTEGER)
            continue;

        /* 检查这个键是否已在候选列表中 */
        int found = -1;
        for (j = 0; j < *candidate_count; j++) {
            if (candidates[j].key == e->key) {
                found = j;
                break;
            }
        }

        if (found >= 0) {
            /* 已存在，更新统计 */
            candidates[found].total_count++;
            if (e->value.int_value == target_value) {
                candidates[found].match_count++;
            }
        } else if (*candidate_count < MAX_CANDIDATES) {
            /* 新键，添加到列表 */
            candidates[*candidate_count].key = e->key;
            candidates[*candidate_count].total_count = 1;
            candidates[*candidate_count].match_count =
                (e->value.int_value == target_value) ? 1 : 0;
            (*candidate_count)++;
        }
    }

    PVFDataContainer_Free(&container);
    free(buffer);
    return 0;
}

int main(int argc, char* argv[]) {
    PackSetInternal pack;
    KeyCandidate candidates[MAX_CANDIDATES] = {0};
    int candidate_count = 0;
    int i, j;
    int scanned = 0;

    if (argc < 4) {
        printf("用法: %s <pvf_file> <label> <value> [max_files]\n", argv[0]);
        printf("示例: %s Script.pvf \"[physical attack]\" 44 100\n", argv[0]);
        printf("\n");
        printf("说明:\n");
        printf("  <pvf_file>  - PVF文件路径\n");
        printf("  <label>     - sample文件中的标签\n");
        printf("  <value>     - 该标签对应的值\n");
        printf("  [max_files] - 最多扫描文件数（默认200）\n");
        return 1;
    }

    const char* label = argv[2];
    int32_t target_value = atoi(argv[3]);
    int max_files = (argc >= 5) ? atoi(argv[4]) : 200;

    PVF_Initialize();

    if (PackSet_Open(argv[1], &pack) != PVF_SUCCESS) {
        printf("错误: 无法打开PVF\n");
        return 1;
    }

    printf("========================================\n");
    printf("自动键映射发现工具\n");
    printf("========================================\n");
    printf("标签:        %s\n", label);
    printf("目标值:      %d\n", target_value);
    printf("扫描限制:    %d 个文件\n", max_files);
    printf("========================================\n\n");

    /* 扫描装备文件 */
    for (i = 0; i < (int)pack.header.index_count && scanned < max_files; i++) {
        if (strstr(pack.indexes[i].name, ".equ") == NULL ||
            strstr(pack.indexes[i].name, "avatar") != NULL ||
            strstr(pack.indexes[i].name, "creature") != NULL) {
            continue;
        }

        find_keys_with_value(&pack, pack.indexes[i].name,
                              target_value, candidates, &candidate_count);
        scanned++;

        if (scanned % 50 == 0) {
            printf("已扫描 %d 个文件...\n", scanned);
        }
    }

    printf("\n已扫描 %d 个装备文件\n\n", scanned);

    /* 计算置信度并排序 */
    for (i = 0; i < candidate_count; i++) {
        if (candidates[i].total_count > 0) {
            candidates[i].confidence =
                (float)candidates[i].match_count / candidates[i].total_count * 100.0f;
        }
    }

    /* 按置信度排序（冒泡排序） */
    for (i = 0; i < candidate_count - 1; i++) {
        for (j = i + 1; j < candidate_count; j++) {
            if (candidates[j].confidence > candidates[i].confidence) {
                KeyCandidate temp = candidates[i];
                candidates[i] = candidates[j];
                candidates[j] = temp;
            }
        }
    }

    /* 输出结果 */
    printf("候选键（按置信度排序）:\n\n");
    printf("%-12s  %-8s  %-8s  %-8s  建议\n",
           "键(十六进制)", "匹配次数", "总出现", "置信度");
    printf("----------------------------------------------------------------\n");

    int found_high_confidence = 0;
    for (i = 0; i < candidate_count && i < 10; i++) {
        const char* suggestion = "";

        if (candidates[i].confidence >= 80.0f) {
            suggestion = "✓ 强烈推荐";
            found_high_confidence = 1;
        } else if (candidates[i].confidence >= 50.0f) {
            suggestion = "可能";
        } else {
            suggestion = "不太可能";
        }

        printf("0x%08X    %-8d  %-8d  %5.1f%%   %s\n",
               candidates[i].key,
               candidates[i].match_count,
               candidates[i].total_count,
               candidates[i].confidence,
               suggestion);
    }

    printf("\n========================================\n");

    if (found_high_confidence) {
        printf("发现高置信度映射（>=80%%）！\n");
        printf("建议使用标记为\"✓ 强烈推荐\"的键。\n");
    } else {
        printf("未找到高置信度映射。\n");
        printf("可能原因:\n");
        printf("1. 该值在装备中不常见\n");
        printf("2. 需要更多样本文件\n");
        printf("3. 该标签可能不对应单一键\n");
    }

    printf("========================================\n");

    PackSet_Close(&pack);
    return 0;
}
