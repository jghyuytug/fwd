/**
 * @file test_pvf_manager.c
 * @brief PVF Manager集成测试
 *
 * 测试PVF管理器是否能正确加载Script.pvf和stringtable.bin
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pvf_manager.h"
#include "pvf/parsers/skill_parser.h"

int main(int argc, char* argv[])
{
    int ret;
    int i;
    const char* pvf_path = "../../Script.pvf";
    const char* stringtable_path = "../../stringtable.bin";
    const char* skill_file = NULL;

    /* 使用命令行参数覆盖默认路径 */
    if (argc >= 2) {
        pvf_path = argv[1];
    }
    if (argc >= 3) {
        stringtable_path = argv[2];
    }
    if (argc >= 4) {
        skill_file = argv[3];
    }

    printf("=== PVF Manager Integration Test ===\n\n");
    printf("PVF Path:         %s\n", pvf_path);
    printf("StringTable Path: %s\n\n", stringtable_path);

    /* 初始化 */
    printf("[1] Initializing PVF Manager...\n");
    ret = PVF_Manager_Initialize(pvf_path, stringtable_path);
    if (ret != PVF_MGR_SUCCESS) {
        fprintf(stderr, "[FAIL] Initialization failed: %d\n", ret);
        return 1;
    }
    printf("[PASS] Initialization successful\n\n");

    /* 打印状态 */
    printf("[2] PVF Status:\n");
    PVF_Manager_PrintStatus();
    printf("\n");

    /* 测试文件存在性检查 */
    printf("[3] Testing file existence...\n");
    const char* test_files[] = {
        "n_quest/skill/gunner.txt",
        "etc/itemshop.etc",
        "equipment/armor/cloth/arm.equ",
        "nonexistent/file.txt"
    };

    for (i = 0; i < 4; i++) {
        int exists = PVF_Manager_FileExists(test_files[i]);
        printf("    %s: %s\n", test_files[i], exists ? "EXISTS" : "NOT FOUND");
    }
    printf("\n");

    /* 测试文件大小获取 */
    printf("[4] Testing file size retrieval...\n");
    for (i = 0; i < 3; i++) {
        int32_t size = PVF_Manager_GetFileSize(test_files[i]);
        if (size >= 0) {
            printf("    %s: %d bytes\n", test_files[i], size);
        } else {
            printf("    %s: ERROR\n", test_files[i]);
        }
    }
    printf("\n");

    /* 测试文件读取 */
    printf("[5] Testing file read...\n");
    unsigned char* data = NULL;
    size_t size = 0;

    ret = PVF_Manager_ReadFile("n_quest/skill/gunner.txt", &data, &size);
    if (ret == PVF_MGR_SUCCESS) {
        printf("    Read %zu bytes from n_quest/skill/gunner.txt\n", size);
        printf("    First 100 chars: %.100s...\n", (char*)data);
        free(data);
    } else {
        printf("    [FAIL] Read failed: %d\n", ret);
    }
    printf("\n");

    /* 测试StringTable */
    printf("[6] Testing StringTable...\n");
    const char* str = PVF_Manager_GetString(0x00004817);  /* [grade] */
    if (str) {
        printf("    Index 0x4817: %s\n", str);
    } else {
        printf("    StringTable not loaded or index not found\n");
    }

    char tag[128];
    ret = PVF_Manager_GetTag(0x00002ca5, tag, sizeof(tag));  /* [name] */
    if (ret > 0) {
        printf("    Tag 0x2ca5: %s\n", tag);
    }
    printf("\n");

    /* 测试SKL解析 */
    printf("[6b] Testing .skl parsing...\n");
    if (skill_file == NULL) {
        skill_file = "skill/gunner/basichpmaxup.skl";
    }
    printf("    Target skill file: %s\n", skill_file);

    data = NULL;
    size = 0;
    ret = PVF_Manager_ReadFile(skill_file, &data, &size);
    if (ret == PVF_MGR_SUCCESS) {
        SkillData skl;
        ret = Skill_Parse(&skl, data, size);
        if (ret == PVF_SUCCESS) {
            Skill_Print(&skl);
            Skill_Free(&skl);
        } else {
            printf("    [FAIL] Skill_Parse failed: %d\n", ret);
        }
        free(data);
    } else {
        printf("    [FAIL] PVF_Manager_ReadFile failed: %d\n", ret);
    }
    printf("\n");

    /* 清理 */
    printf("[7] Cleaning up...\n");
    PVF_Manager_Cleanup();
    printf("[PASS] Cleanup successful\n\n");

    printf("=== All tests completed ===\n");
    return 0;
}
