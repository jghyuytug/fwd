/**
 * DNF游戏服务器 - AccountCargo表ORM测试程序
 *
 * 功能:
 * 1. 测试账号仓库CRUD操作
 * 2. 测试BLOB数据处理
 * 3. 测试仓库数据管理功能
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db_connection_manager.h"
#include "account_cargo_orm.h"

// ANSI颜色代码
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RED     "\033[31m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"

void print_separator() {
    printf("%s================================================================================\n%s",
           COLOR_CYAN, COLOR_RESET);
}

void print_success(const char* msg) {
    printf("%s✓ %s%s\n", COLOR_GREEN, msg, COLOR_RESET);
}

void print_error(const char* msg) {
    printf("%s✗ %s%s\n", COLOR_RED, msg, COLOR_RESET);
}

void print_info(const char* msg) {
    printf("%s● %s%s\n", COLOR_BLUE, msg, COLOR_RESET);
}

void print_warning(const char* msg) {
    printf("%s⚠ %s%s\n", COLOR_YELLOW, msg, COLOR_RESET);
}

// 创建测试用的仓库数据
void create_test_cargo_data(unsigned char* buffer, int size) {
    for (int i = 0; i < size; i++) {
        buffer[i] = (unsigned char)(i % 256);
    }
}

// 测试创建仓库记录
int test_create_cargo(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试1: 创建账号仓库记录%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    AccountCargo cargo;

    // 创建仓库记录1 - 小数据量
    print_info("创建仓库记录 - 账号1 (小数据量: 100字节)");
    AccountCargo_Init(&cargo);
    cargo.m_id = 1;

    if (AccountCargo_AllocateData(&cargo, 100) == 0) {
        create_test_cargo_data(cargo.cargo_data, 100);

        if (AccountCargo_Create(manager, &cargo) == 0) {
            print_success("仓库记录创建成功");
            printf("  账号ID: %d\n", cargo.m_id);
            printf("  数据大小: %d 字节\n", cargo.cargo_data_len);
            printf("  更新时间: %s\n", cargo.update_time);
        } else {
            print_error("仓库记录创建失败");
            AccountCargo_Free(&cargo);
            return -1;
        }
    } else {
        print_error("分配数据缓冲区失败");
        return -1;
    }
    AccountCargo_Free(&cargo);

    // 创建仓库记录2 - 中等数据量
    print_info("创建仓库记录 - 账号2 (中等数据量: 1024字节)");
    AccountCargo_Init(&cargo);
    cargo.m_id = 2;

    if (AccountCargo_AllocateData(&cargo, 1024) == 0) {
        create_test_cargo_data(cargo.cargo_data, 1024);

        if (AccountCargo_Create(manager, &cargo) == 0) {
            print_success("仓库记录创建成功");
            printf("  账号ID: %d\n", cargo.m_id);
            printf("  数据大小: %d 字节\n", cargo.cargo_data_len);
        } else {
            print_error("仓库记录创建失败");
            AccountCargo_Free(&cargo);
            return -1;
        }
    } else {
        print_error("分配数据缓冲区失败");
        return -1;
    }
    AccountCargo_Free(&cargo);

    // 创建仓库记录3 - 大数据量
    print_info("创建仓库记录 - 账号3 (大数据量: 4096字节)");
    AccountCargo_Init(&cargo);
    cargo.m_id = 3;

    if (AccountCargo_AllocateData(&cargo, 4096) == 0) {
        create_test_cargo_data(cargo.cargo_data, 4096);

        if (AccountCargo_Create(manager, &cargo) == 0) {
            print_success("仓库记录创建成功");
            printf("  账号ID: %d\n", cargo.m_id);
            printf("  数据大小: %d 字节\n", cargo.cargo_data_len);
        } else {
            print_error("仓库记录创建失败");
            AccountCargo_Free(&cargo);
            return -1;
        }
    } else {
        print_error("分配数据缓冲区失败");
        return -1;
    }
    AccountCargo_Free(&cargo);

    printf("\n");
    return 0;
}

// 测试查询仓库记录
int test_query_cargo(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试2: 查询账号仓库记录%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    AccountCargo cargo;

    // 查询账号1的仓库
    print_info("查询账号1的仓库数据");
    AccountCargo_Init(&cargo);

    if (AccountCargo_GetByID(manager, 1, &cargo) == 0) {
        print_success("查询成功");
        AccountCargo_Print(&cargo);

        // 验证数据完整性
        bool data_valid = true;
        for (int i = 0; i < cargo.cargo_data_len && i < 100; i++) {
            if (cargo.cargo_data[i] != (unsigned char)(i % 256)) {
                data_valid = false;
                break;
            }
        }

        if (data_valid) {
            print_success("数据完整性验证通过");
        } else {
            print_error("数据完整性验证失败");
            AccountCargo_Free(&cargo);
            return -1;
        }

        AccountCargo_Free(&cargo);
    } else {
        print_error("查询失败");
        return -1;
    }

    // 查询账号2的仓库
    print_info("查询账号2的仓库数据");
    AccountCargo_Init(&cargo);

    if (AccountCargo_GetByID(manager, 2, &cargo) == 0) {
        print_success("查询成功");
        printf("  账号ID: %d\n", cargo.m_id);
        printf("  数据大小: %d 字节\n", cargo.cargo_data_len);
        printf("  更新时间: %s\n", cargo.update_time);
        AccountCargo_Free(&cargo);
    } else {
        print_error("查询失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试更新仓库数据
int test_update_cargo(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试3: 更新账号仓库数据%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    AccountCargo cargo;

    // 获取原仓库数据
    print_info("获取账号1的原仓库数据");
    AccountCargo_Init(&cargo);

    if (AccountCargo_GetByID(manager, 1, &cargo) != 0) {
        print_error("获取原数据失败");
        return -1;
    }

    int old_size = cargo.cargo_data_len;
    printf("  原数据大小: %d 字节\n", old_size);
    AccountCargo_Free(&cargo);

    // 更新为更大的数据
    print_info("更新为更大的数据 (500字节)");
    AccountCargo_Init(&cargo);
    cargo.m_id = 1;

    if (AccountCargo_AllocateData(&cargo, 500) == 0) {
        create_test_cargo_data(cargo.cargo_data, 500);

        if (AccountCargo_Update(manager, &cargo) == 0) {
            print_success("更新成功");
            printf("  新数据大小: %d 字节\n", cargo.cargo_data_len);
        } else {
            print_error("更新失败");
            AccountCargo_Free(&cargo);
            return -1;
        }
        AccountCargo_Free(&cargo);
    } else {
        print_error("分配数据缓冲区失败");
        return -1;
    }

    // 验证更新
    print_info("验证更新结果");
    AccountCargo_Init(&cargo);

    if (AccountCargo_GetByID(manager, 1, &cargo) == 0) {
        if (cargo.cargo_data_len == 500) {
            print_success("更新验证通过");
            printf("  新数据大小: %d 字节\n", cargo.cargo_data_len);
        } else {
            print_error("更新验证失败");
            AccountCargo_Free(&cargo);
            return -1;
        }
        AccountCargo_Free(&cargo);
    } else {
        print_error("验证查询失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试存在性检查
int test_existence_check(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试4: 存在性检查%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    // 检查存在的仓库
    print_info("检查已存在的仓库 (账号1)");
    if (AccountCargo_Exists(manager, 1)) {
        print_success("正确识别出仓库存在");
    } else {
        print_error("错误地判断仓库不存在");
        return -1;
    }

    // 检查不存在的仓库
    print_info("检查不存在的仓库 (账号999)");
    if (!AccountCargo_Exists(manager, 999)) {
        print_success("正确识别出仓库不存在");
    } else {
        print_error("错误地判断仓库存在");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试数据大小查询
int test_data_size(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试5: 查询数据大小%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    print_info("查询账号1的仓库数据大小");
    int size = AccountCargo_GetDataSize(manager, 1);
    if (size >= 0) {
        print_success("查询成功");
        printf("  数据大小: %d 字节\n", size);
    } else {
        print_error("查询失败");
        return -1;
    }

    print_info("查询账号2的仓库数据大小");
    size = AccountCargo_GetDataSize(manager, 2);
    if (size >= 0) {
        print_success("查询成功");
        printf("  数据大小: %d 字节\n", size);
    } else {
        print_error("查询失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试统计功能
int test_statistics(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试6: 统计功能%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    // 获取仓库总数
    print_info("获取仓库记录总数");
    int count = AccountCargo_GetCount(manager);
    if (count >= 0) {
        print_success("查询成功");
        printf("  仓库记录总数: %d\n", count);
    } else {
        print_error("查询失败");
        return -1;
    }

    // 获取所有账号ID列表
    print_info("获取所有有仓库的账号列表");
    int m_ids[100];
    int num = AccountCargo_GetAllAccountIDs(manager, m_ids, 100);
    if (num >= 0) {
        print_success("查询成功");
        printf("  账号数量: %d\n", num);
        printf("  账号列表: ");
        for (int i = 0; i < num && i < 10; i++) {
            printf("%d ", m_ids[i]);
        }
        if (num > 10) {
            printf("...");
        }
        printf("\n");
    } else {
        print_error("查询失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试复制功能
int test_copy(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试7: 仓库数据复制%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    AccountCargo src, dest;

    // 获取源仓库数据
    print_info("获取源仓库数据 (账号1)");
    AccountCargo_Init(&src);
    if (AccountCargo_GetByID(manager, 1, &src) != 0) {
        print_error("获取源数据失败");
        return -1;
    }
    print_success("源数据获取成功");
    printf("  源数据大小: %d 字节\n", src.cargo_data_len);

    // 复制仓库数据
    print_info("复制仓库数据");
    AccountCargo_Init(&dest);
    if (AccountCargo_Copy(&dest, &src) == 0) {
        print_success("复制成功");
        printf("  目标数据大小: %d 字节\n", dest.cargo_data_len);

        // 验证数据一致性
        if (dest.cargo_data_len == src.cargo_data_len &&
            memcmp(dest.cargo_data, src.cargo_data, src.cargo_data_len) == 0) {
            print_success("数据一致性验证通过");
        } else {
            print_error("数据一致性验证失败");
            AccountCargo_Free(&src);
            AccountCargo_Free(&dest);
            return -1;
        }
    } else {
        print_error("复制失败");
        AccountCargo_Free(&src);
        return -1;
    }

    AccountCargo_Free(&src);
    AccountCargo_Free(&dest);

    printf("\n");
    return 0;
}

// 测试删除功能
int test_delete_cargo(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试8: 删除仓库记录%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    // 删除账号3的仓库
    print_info("删除账号3的仓库记录");
    if (AccountCargo_Delete(manager, 3) == 0) {
        print_success("删除成功");

        // 验证删除
        if (!AccountCargo_Exists(manager, 3)) {
            print_success("确认仓库已被删除");
        } else {
            print_error("仓库仍然存在");
            return -1;
        }
    } else {
        print_error("删除失败");
        return -1;
    }

    printf("\n");
    return 0;
}

int main(int argc, char* argv[]) {
    printf("\n");
    print_separator();
    printf("%s  DNF游戏服务器 - AccountCargo表ORM测试  %s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();
    printf("\n");

    if (argc < 2) {
        print_error("用法: test_account_cargo_orm <配置文件路径>");
        printf("  示例: ./test_account_cargo_orm ../config/database.cfg\n");
        return 1;
    }

    const char* config_file = argv[1];
    printf("配置文件: %s\n\n", config_file);

    // 初始化数据库连接
    DBConnectionManager manager;
    memset(&manager, 0, sizeof(DBConnectionManager));

    print_info("初始化数据库连接...");
    if (DBConnectionManager_Initialize(&manager, config_file) != 0) {
        print_error("初始化失败");
        return 1;
    }
    print_success("初始化成功");

    // 连接到角色数据库 (account_cargo表在test_taiwan_cain)
    print_info("连接到角色数据库 (test_taiwan_cain)...");
    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) != 0) {
        print_error("连接失败");
        return 1;
    }
    print_success("连接成功");
    printf("\n");

    // 运行测试
    int success_count = 0;
    int total_tests = 8;

    if (test_create_cargo(&manager) == 0) success_count++;
    if (test_query_cargo(&manager) == 0) success_count++;
    if (test_update_cargo(&manager) == 0) success_count++;
    if (test_existence_check(&manager) == 0) success_count++;
    if (test_data_size(&manager) == 0) success_count++;
    if (test_statistics(&manager) == 0) success_count++;
    if (test_copy(&manager) == 0) success_count++;
    if (test_delete_cargo(&manager) == 0) success_count++;

    // 清理
    print_info("清理数据库连接...");
    DBConnectionManager_Cleanup(&manager);
    print_success("清理完成");

    printf("\n");
    print_separator();
    printf("%s测试结果: %d/%d 通过%s\n",
           success_count == total_tests ? COLOR_GREEN : COLOR_YELLOW,
           success_count, total_tests, COLOR_RESET);
    print_separator();
    printf("\n");

    return (success_count == total_tests) ? 0 : 1;
}
