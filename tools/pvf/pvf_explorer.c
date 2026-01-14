/**
 * @file pvf_explorer.c
 * @brief PVF文件探索工具 - 分析游戏数据格式
 *
 * @usage
 *     gcc -m32 -I../../include -o pvf_explorer pvf_explorer.c \
 *         ../../src/pvf/pvf_crc32.c \
 *         ../../src/pvf/pvf_decrypt.c \
 *         ../../src/pvf/pvf_hash.c \
 *         ../../src/pvf/pvf_parser.c \
 *         ../../src/pvf/pvf_packset.c
 *     ./pvf_explorer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pvf/pvf.h"

/* 默认PVF文件路径 */
static const char* DEFAULT_PVF_PATH = "/home/dxf/game/Script.pvf";

/* 输出目录 */
static const char* OUTPUT_DIR = "pvf_samples/";

/* ========================================================================== */
/* 辅助函数                                                                   */
/* ========================================================================== */

/**
 * @brief 统计文件扩展名
 */
typedef struct {
    char ext[16];
    uint32_t count;
} ExtensionStat;

static ExtensionStat ext_stats[100];
static int ext_count = 0;

static void add_extension(const char* filename) {
    const char* dot = strrchr(filename, '.');
    if (dot == NULL) return;

    const char* ext = dot + 1;
    int i;

    /* 查找已存在的扩展名 */
    for (i = 0; i < ext_count; i++) {
        if (strcmp(ext_stats[i].ext, ext) == 0) {
            ext_stats[i].count++;
            return;
        }
    }

    /* 添加新扩展名 */
    if (ext_count < 100) {
        strncpy(ext_stats[ext_count].ext, ext, 15);
        ext_stats[ext_count].ext[15] = '\0';
        ext_stats[ext_count].count = 1;
        ext_count++;
    }
}

/**
 * @brief 比较函数（用于排序）
 */
static int compare_ext_stats(const void* a, const void* b) {
    const ExtensionStat* ea = (const ExtensionStat*)a;
    const ExtensionStat* eb = (const ExtensionStat*)b;
    return eb->count - ea->count;  /* 降序 */
}

/**
 * @brief 检查文件是否为文本
 */
static int is_text_file(const char* data, size_t size) {
    size_t i;
    size_t text_chars = 0;
    size_t check_size = (size < 512) ? size : 512;

    for (i = 0; i < check_size; i++) {
        unsigned char c = (unsigned char)data[i];
        if (isprint(c) || isspace(c)) {
            text_chars++;
        }
    }

    return (text_chars * 100 / check_size) > 80;  /* 80%以上是可打印字符 */
}

/**
 * @brief 提取文件样本
 */
static int extract_sample(PackSetInternal* pack, const char* filepath) {
    PVFIndexEntry* entry;
    char* buffer;
    char output_path[512];
    FILE* fp;
    int result;

    entry = PackSet_GetFile(pack, filepath);
    if (entry == NULL) {
        printf("  [跳过] 文件未找到: %s\n", filepath);
        return 0;
    }

    /* 限制样本大小 */
    size_t extract_size = (entry->size < 65536) ? entry->size : 65536;

    buffer = (char*)malloc(extract_size);
    if (buffer == NULL) {
        printf("  [错误] 内存分配失败\n");
        return 0;
    }

    result = PackSet_ExtractFile(pack, entry, buffer, extract_size);
    if (result != PVF_SUCCESS) {
        printf("  [错误] 提取失败: %s (错误码: %d)\n", filepath, result);
        free(buffer);
        return 0;
    }

    /* 构造输出路径 */
    snprintf(output_path, sizeof(output_path), "%s%s",
             OUTPUT_DIR, strrchr(filepath, '/') ? strrchr(filepath, '/') + 1 : filepath);

    /* 写入文件 */
    fp = fopen(output_path, "wb");
    if (fp == NULL) {
        printf("  [错误] 无法创建文件: %s\n", output_path);
        free(buffer);
        return 0;
    }

    fwrite(buffer, 1, extract_size, fp);
    fclose(fp);

    printf("  [成功] %s -> %s (%u 字节)\n", filepath, output_path, extract_size);

    /* 检查是否为文本文件 */
    if (is_text_file(buffer, extract_size)) {
        printf("         类型: 文本文件\n");
        printf("         预览: ");
        int i;
        for (i = 0; i < 60 && i < (int)extract_size; i++) {
            if (buffer[i] == '\n') break;
            if (isprint((unsigned char)buffer[i])) {
                putchar(buffer[i]);
            }
        }
        printf("...\n");
    } else {
        printf("         类型: 二进制文件\n");
        printf("         前16字节: ");
        int i;
        for (i = 0; i < 16 && i < (int)extract_size; i++) {
            printf("%02x ", (unsigned char)buffer[i]);
        }
        printf("\n");
    }

    free(buffer);
    return 1;
}

/* ========================================================================== */
/* 主要功能                                                                   */
/* ========================================================================== */

/**
 * @brief 分析文件类型分布
 */
static void analyze_file_types(PackSetInternal* pack) {
    uint32_t i;

    printf("\n========================================\n");
    printf("文件类型分析\n");
    printf("========================================\n\n");

    printf("正在扫描 %u 个文件...\n", pack->header.index_count);

    /* 统计扩展名 */
    for (i = 0; i < pack->header.index_count; i++) {
        add_extension(pack->indexes[i].name);
    }

    /* 排序 */
    qsort(ext_stats, ext_count, sizeof(ExtensionStat), compare_ext_stats);

    /* 显示前20种 */
    printf("\n文件扩展名分布（前20种）:\n\n");
    printf("%-10s %10s %8s\n", "扩展名", "数量", "占比");
    printf("----------------------------------------\n");

    int display_count = (ext_count < 20) ? ext_count : 20;
    for (i = 0; i < (uint32_t)display_count; i++) {
        float percentage = (ext_stats[i].count * 100.0) / pack->header.index_count;
        printf("%-10s %10u %7.2f%%\n",
               ext_stats[i].ext,
               ext_stats[i].count,
               percentage);
    }

    if (ext_count > 20) {
        printf("... 还有 %d 种扩展名\n", ext_count - 20);
    }
}

/**
 * @brief 分析目录结构
 */
static void analyze_directory_structure(PackSetInternal* pack) {
    uint32_t i;
    char top_dirs[100][64];
    uint32_t top_dir_counts[100];
    int top_dir_count = 0;

    printf("\n========================================\n");
    printf("目录结构分析\n");
    printf("========================================\n\n");

    /* 统计顶级目录 */
    for (i = 0; i < pack->header.index_count; i++) {
        char* name = pack->indexes[i].name;
        char* slash = strchr(name, '/');

        if (slash != NULL) {
            int len = slash - name;
            char top_dir[64];
            strncpy(top_dir, name, len);
            top_dir[len] = '\0';

            /* 查找已存在的目录 */
            int j;
            int found = 0;
            for (j = 0; j < top_dir_count; j++) {
                if (strcmp(top_dirs[j], top_dir) == 0) {
                    top_dir_counts[j]++;
                    found = 1;
                    break;
                }
            }

            /* 添加新目录 */
            if (!found && top_dir_count < 100) {
                strcpy(top_dirs[top_dir_count], top_dir);
                top_dir_counts[top_dir_count] = 1;
                top_dir_count++;
            }
        }
    }

    printf("顶级目录列表:\n\n");
    printf("%-30s %10s\n", "目录名", "文件数");
    printf("----------------------------------------\n");

    for (i = 0; i < (uint32_t)top_dir_count; i++) {
        printf("%-30s %10u\n", top_dirs[i], top_dir_counts[i]);
    }
}

/**
 * @brief 提取样本文件
 */
static void extract_samples(PackSetInternal* pack) {
    printf("\n========================================\n");
    printf("提取样本文件\n");
    printf("========================================\n\n");

    /* 创建输出目录 */
    #ifdef _WIN32
    system("mkdir pvf_samples 2>nul");
    #else
    system("mkdir -p pvf_samples");
    #endif

    /* 样本文件列表 */
    const char* samples[] = {
        /* 装备文件 */
        "equipment/character/common/wrist/brac_2choro994.equ",
        "equipment/character/mage/weapon/spear/beamspeard1.equ",

        /* 动画文件 */
        "creature/2wu_gunner/prime/animation/run/02_prime_run_01_dodge.ani",
        "passiveobject/character/gunner/animation/stingerex/subdodgenotarget.ani",

        /* 动作文件 */
        "monster/newmonsters/shonan/kaizen/action/teleport.act",

        /* 其他 */
        "stackable/cash/cash_cn.stk",
        "etc/itemscript.txt"
    };

    int sample_count = sizeof(samples) / sizeof(samples[0]);
    int i;

    for (i = 0; i < sample_count; i++) {
        extract_sample(pack, samples[i]);
    }
}

/* ========================================================================== */
/* 主函数                                                                      */
/* ========================================================================== */

int main(int argc, char* argv[]) {
    PackSetInternal pack;
    int result;
    const char* pvf_path;

    printf("========================================\n");
    printf("PVF 文件探索工具\n");
    printf("版本: %s\n", PVF_GetVersion());
    printf("========================================\n");

    /* 获取PVF文件路径 */
    if (argc >= 2) {
        pvf_path = argv[1];
    } else {
        pvf_path = DEFAULT_PVF_PATH;
    }

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

    /* 执行分析 */
    analyze_file_types(&pack);
    analyze_directory_structure(&pack);
    extract_samples(&pack);

    /* 关闭 */
    PackSet_Close(&pack);

    printf("\n========================================\n");
    printf("分析完成!\n");
    printf("样本文件已保存到: %s\n", OUTPUT_DIR);
    printf("========================================\n\n");

    return 0;
}
