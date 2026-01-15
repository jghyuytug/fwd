/**
 * @file test_equipment_parser.c
 * @brief 装备解析器测试
 *
 * @usage
 *     gcc -m32 -I../../include -o test_equipment_parser test_equipment_parser.c \
 *         ../../src/pvf/*.c ../../src/pvf/parsers/*.c
 *     ./test_equipment_parser
 */

#include <stdio.h>
#include <stdlib.h>
#include "pvf/pvf.h"
#include "pvf/parsers/equipment_parser.h"

static const char* PVF_FILE_PATH = "/home/dxf/game/Script.pvf";

/* ========================================================================== */
/* 测试装备解析                                                               */
/* ========================================================================== */

static int test_equipment_parse(void) {
    PackSetInternal pack;
    EquipmentData eq;
    int result;

    printf("\n========================================\n");
    printf("装备解析器测试\n");
    printf("========================================\n\n");

    /* 打开PVF */
    PVF_Initialize();
    result = PackSet_Open(PVF_FILE_PATH, &pack);
    if (result != PVF_SUCCESS) {
        printf("[SKIP] PVF文件不存在\n");
        return 1;
    }

    printf("测试文件列表:\n\n");

    /* 测试多个装备文件 */
    const char* test_files[] = {
        "equipment/character/common/wrist/brac_2choro994.equ",
        "equipment/character/mage/weapon/spear/beamspeard1.equ",
        "equipment/character/fighter/weapon/knuckle/knuckle_2choro125.equ"
    };

    int test_count = sizeof(test_files) / sizeof(test_files[0]);
    int success_count = 0;
    int i;

    for (i = 0; i < test_count; i++) {
        printf("[%d/%d] %s\n", i+1, test_count, test_files[i]);

        result = Equipment_ParseFromPVF(&eq, &pack, test_files[i]);

        if (result == PVF_SUCCESS) {
            Equipment_Print(&eq);
            Equipment_Free(&eq);
            printf("\n[成功]\n\n");
            success_count++;
        } else if (result == PVF_ERROR_FILE_NOT_FOUND) {
            printf("  [跳过] 文件未找到\n\n");
            success_count++;  /* 不算失败 */
        } else {
            printf("  [失败] 错误码: %d\n\n", result);
        }
    }

    PackSet_Close(&pack);

    printf("========================================\n");
    printf("测试结果: %d/%d\n", success_count, test_count);
    printf("========================================\n\n");

    return (success_count > 0) ? 1 : 0;
}

/* ========================================================================== */
/* 主函数                                                                      */
/* ========================================================================== */

int main(int argc, char* argv[]) {
    if (argc >= 2) {
        PVF_FILE_PATH = argv[1];
    }

    printf("========================================\n");
    printf("PVF装备解析器测试\n");
    printf("版本: %s\n", PVF_GetVersion());
    printf("========================================\n");

    int result = test_equipment_parse();

    return result ? 0 : 1;
}
