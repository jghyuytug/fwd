/**
 * @file test_extended_parser.c
 * @brief 测试扩展后的装备解析器
 */

#include <stdio.h>
#include "pvf/pvf.h"
#include "pvf/parsers/equipment_parser.h"

int main(int argc, char* argv[]) {
    PackSetInternal pack;
    EquipmentData eq;
    int result;

    if (argc < 3) {
        printf("用法: %s <pvf_file> <equipment_file>\n", argv[0]);
        return 1;
    }

    PVF_Initialize();

    /* 打开PVF */
    result = PackSet_Open(argv[1], &pack);
    if (result != PVF_SUCCESS) {
        printf("错误: 无法打开PVF\n");
        return 1;
    }

    /* 解析装备 */
    result = Equipment_ParseFromPVF(&eq, &pack, argv[2]);
    if (result != PVF_SUCCESS) {
        printf("错误: 解析失败 (%d)\n", result);
        PackSet_Close(&pack);
        return 1;
    }

    /* 打印装备信息 */
    Equipment_Print(&eq);

    /* 清理 */
    Equipment_Free(&eq);
    PackSet_Close(&pack);

    return 0;
}
