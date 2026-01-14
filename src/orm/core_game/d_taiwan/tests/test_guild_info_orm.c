/**
 * DNF游戏服务器 - GuildInfo ORM 测试程序
 *
 * 功能:
 * 1. 测试公会CRUD操作
 * 2. 测试公会信息查询
 * 3. 测试公会管理功能
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db_connection_manager.h"
#include "guild_info_orm.h"

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

// 测试创建公会记录
int test_create_guild(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试1: 创建公会记录%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    GuildInfo guild;
    int i;

    // 创建公会1 - 新手公会
    print_info("创建公会1 - 新手公会");
    GuildInfo_Init(&guild);
    strcpy(guild.guild_name, "TestGuild_Rookies");
    guild.guild_level = 1;
    guild.guild_exp = 0;
    guild.guild_master_charac_no = 1001;
    strcpy(guild.guild_notice, "欢迎加入新手公会!");
    guild.member_count = 1;
    guild.max_member_count = 20;

    int guild_id1 = GuildInfo_Create(manager, &guild);
    if (guild_id1 > 0) {
        print_success("公会创建成功");
        printf("  公会ID: %d\n", guild_id1);
        printf("  公会名称: %s\n", guild.guild_name);
        printf("  等级: %d, 经验: %d\n", guild.guild_level, guild.guild_exp);
    } else {
        print_error("公会创建失败");
        return -1;
    }

    // 创建公会2 - 中级公会
    print_info("创建公会2 - 中级公会");
    GuildInfo_Init(&guild);
    strcpy(guild.guild_name, "TestGuild_Warriors");
    guild.guild_level = 5;
    guild.guild_exp = 5000;
    guild.guild_master_charac_no = 2001;
    strcpy(guild.guild_notice, "战士之家，勇往直前!");
    guild.member_count = 15;
    guild.max_member_count = 50;

    int guild_id2 = GuildInfo_Create(manager, &guild);
    if (guild_id2 > 0) {
        print_success("公会创建成功");
        printf("  公会ID: %d\n", guild_id2);
    } else {
        print_error("公会创建失败");
        return -1;
    }

    // 创建公会3 - 高级公会
    print_info("创建公会3 - 高级公会");
    GuildInfo_Init(&guild);
    strcpy(guild.guild_name, "TestGuild_Legends");
    guild.guild_level = 10;
    guild.guild_exp = 50000;
    guild.guild_master_charac_no = 3001;
    strcpy(guild.guild_notice, "传奇公会，只招精英!");
    guild.member_count = 48;
    guild.max_member_count = 100;

    int guild_id3 = GuildInfo_Create(manager, &guild);
    if (guild_id3 > 0) {
        print_success("公会创建成功");
        printf("  公会ID: %d\n", guild_id3);
    } else {
        print_error("公会创建失败");
        return -1;
    }

    // 批量创建多个公会用于后续测试
    print_info("批量创建测试公会");
    for (i = 1; i <= 5; i++) {
        GuildInfo_Init(&guild);
        char name[51];
        snprintf(name, sizeof(name), "TestGuild_Batch_%d", i);
        strcpy(guild.guild_name, name);
        guild.guild_level = i * 2;
        guild.guild_exp = i * 1000;
        guild.guild_master_charac_no = 8000 + i;
        guild.member_count = i * 5;
        guild.max_member_count = i * 20;
        GuildInfo_Create(manager, &guild);
    }
    print_success("批量创建完成 (5个公会)");

    printf("\n");
    return 0;
}

// 测试查询公会
int test_query_guild(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试2: 查询公会信息%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    GuildInfo guild;

    // 按名称查询
    print_info("按名称查询公会: TestGuild_Warriors");
    if (GuildInfo_GetByName(manager, "TestGuild_Warriors", &guild) == 0) {
        print_success("查询成功");
        printf("  公会ID: %d\n", guild.guild_id);
        printf("  公会名称: %s\n", guild.guild_name);
        printf("  等级: %d, 经验: %d\n", guild.guild_level, guild.guild_exp);
        printf("  会长角色编号: %d\n", guild.guild_master_charac_no);
        printf("  成员数: %d/%d\n", guild.member_count, guild.max_member_count);
        printf("  公告: %s\n", guild.guild_notice);

        // 保存ID用于后续测试
        int saved_guild_id = guild.guild_id;

        // 按ID查询
        print_info("按ID查询同一公会");
        GuildInfo guild_by_id;
        if (GuildInfo_GetByID(manager, saved_guild_id, &guild_by_id) == 0) {
            if (strcmp(guild_by_id.guild_name, guild.guild_name) == 0) {
                print_success("按ID查询结果一致");
            } else {
                print_error("按ID查询结果不一致");
                return -1;
            }
        } else {
            print_error("按ID查询失败");
            return -1;
        }
    } else {
        print_error("按名称查询失败");
        return -1;
    }

    // 查询不存在的公会
    print_info("查询不存在的公会");
    if (GuildInfo_GetByID(manager, 999999, &guild) == -1) {
        print_success("正确返回查询失败");
    } else {
        print_warning("不存在的公会返回了结果");
    }

    printf("\n");
    return 0;
}

// 测试更新公会信息
int test_update_guild(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试3: 更新公会信息%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    GuildInfo guild;

    // 先查询一个公会
    if (GuildInfo_GetByName(manager, "TestGuild_Rookies", &guild) != 0) {
        print_error("无法找到测试公会");
        return -1;
    }

    int guild_id = guild.guild_id;
    printf("  原始信息: 等级=%d, 经验=%d\n", guild.guild_level, guild.guild_exp);

    // 测试更新等级和经验
    print_info("更新公会等级和经验");
    if (GuildInfo_UpdateLevelExp(manager, guild_id, 3, 2500) == 0) {
        print_success("更新成功");

        // 验证更新
        if (GuildInfo_GetByID(manager, guild_id, &guild) == 0) {
            if (guild.guild_level == 3 && guild.guild_exp == 2500) {
                print_success("验证: 等级和经验更新正确");
            } else {
                print_error("验证失败: 数据不匹配");
                return -1;
            }
        }
    } else {
        print_error("更新失败");
        return -1;
    }

    // 测试更新公告
    print_info("更新公会公告");
    const char* new_notice = "这是更新后的公会公告内容";
    if (GuildInfo_UpdateNotice(manager, guild_id, new_notice) == 0) {
        print_success("更新成功");

        // 验证更新
        if (GuildInfo_GetByID(manager, guild_id, &guild) == 0) {
            if (strcmp(guild.guild_notice, new_notice) == 0) {
                print_success("验证: 公告更新正确");
            } else {
                print_error("验证失败: 公告不匹配");
                return -1;
            }
        }
    } else {
        print_error("更新公告失败");
        return -1;
    }

    // 测试更新会长
    print_info("更新公会会长");
    if (GuildInfo_UpdateMaster(manager, guild_id, 9001) == 0) {
        print_success("更新成功");

        // 验证更新
        if (GuildInfo_GetByID(manager, guild_id, &guild) == 0) {
            if (guild.guild_master_charac_no == 9001) {
                print_success("验证: 会长更新正确");
            } else {
                print_error("验证失败: 会长不匹配");
                return -1;
            }
        }
    } else {
        print_error("更新会长失败");
        return -1;
    }

    // 测试更新成员数量
    print_info("更新成员数量");
    if (GuildInfo_UpdateMemberCount(manager, guild_id, 25) == 0) {
        print_success("更新成功");

        // 验证更新
        if (GuildInfo_GetByID(manager, guild_id, &guild) == 0) {
            if (guild.member_count == 25) {
                print_success("验证: 成员数量更新正确");
            } else {
                print_error("验证失败: 成员数量不匹配");
                return -1;
            }
        }
    } else {
        print_error("更新成员数量失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试存在性检查
int test_existence_check(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试4: 公会存在性检查%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    GuildInfo guild;

    // 查找一个存在的公会
    if (GuildInfo_GetByName(manager, "TestGuild_Legends", &guild) != 0) {
        print_error("无法找到测试公会");
        return -1;
    }

    // 测试ID存在性
    print_info("检查公会ID是否存在");
    if (GuildInfo_Exists(manager, guild.guild_id)) {
        print_success("已存在的公会ID返回true");
    } else {
        print_error("已存在的公会ID返回false");
        return -1;
    }

    if (!GuildInfo_Exists(manager, 999999)) {
        print_success("不存在的公会ID返回false");
    } else {
        print_error("不存在的公会ID返回true");
        return -1;
    }

    // 测试名称存在性
    print_info("检查公会名称是否存在");
    if (GuildInfo_NameExists(manager, "TestGuild_Legends")) {
        print_success("已存在的公会名称返回true");
    } else {
        print_error("已存在的公会名称返回false");
        return -1;
    }

    if (!GuildInfo_NameExists(manager, "NonExistent_Guild_XYZ")) {
        print_success("不存在的公会名称返回false");
    } else {
        print_error("不存在的公会名称返回true");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试批量查询
int test_batch_query(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试5: 批量查询公会%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    GuildInfo guilds[100];
    int count;
    int i;

    // 获取所有公会
    print_info("获取所有公会");
    count = GuildInfo_GetAll(manager, guilds, 100);
    if (count > 0) {
        print_success("查询成功");
        printf("  获取到 %d 个公会\n", count);

        // 显示前3个
        int display_count = (count > 3) ? 3 : count;
        for (i = 0; i < display_count; i++) {
            printf("  [%d] ID=%d, 名称=%s, 等级=%d\n",
                   i + 1, guilds[i].guild_id, guilds[i].guild_name, guilds[i].guild_level);
        }
    } else {
        print_error("查询失败或无数据");
        return -1;
    }

    // 按等级范围查询
    print_info("查询等级6-10的公会");
    count = GuildInfo_GetByLevelRange(manager, 6, 10, guilds, 50);
    if (count >= 0) {
        print_success("查询成功");
        printf("  找到 %d 个公会\n", count);

        for (i = 0; i < count && i < 5; i++) {
            printf("  [%d] 名称=%s, 等级=%d\n",
                   i + 1, guilds[i].guild_name, guilds[i].guild_level);
        }
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
    printf("%s测试6: 公会统计功能%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    // 获取公会总数
    print_info("获取公会总数");
    int total_count = GuildInfo_GetCount(manager);
    if (total_count > 0) {
        print_success("查询成功");
        printf("  公会总数: %d\n", total_count);
    } else {
        print_error("查询失败");
        return -1;
    }

    // 获取特定等级的公会数量
    print_info("获取等级10的公会数量");
    int level10_count = GuildInfo_GetCountByLevel(manager, 10);
    if (level10_count >= 0) {
        print_success("查询成功");
        printf("  等级10的公会数量: %d\n", level10_count);
    } else {
        print_error("查询失败");
        return -1;
    }

    printf("\n");
    return 0;
}

// 测试搜索功能
int test_search(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试7: 公会搜索功能%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    GuildInfo guilds[50];
    int count;
    int i;

    // 搜索关键词
    print_info("搜索包含'Batch'的公会");
    count = GuildInfo_Search(manager, "Batch", guilds, 50);
    if (count > 0) {
        print_success("搜索成功");
        printf("  找到 %d 个公会\n", count);

        for (i = 0; i < count && i < 5; i++) {
            printf("  [%d] ID=%d, 名称=%s\n",
                   i + 1, guilds[i].guild_id, guilds[i].guild_name);
        }
    } else if (count == 0) {
        print_warning("未找到匹配的公会");
    } else {
        print_error("搜索失败");
        return -1;
    }

    // 搜索不存在的关键词
    print_info("搜索不存在的关键词");
    count = GuildInfo_Search(manager, "NonExistent_XYZ123", guilds, 50);
    if (count == 0) {
        print_success("正确返回0个结果");
    } else if (count < 0) {
        print_error("搜索失败");
    }

    printf("\n");
    return 0;
}

// 测试删除功能
int test_delete_guild(DBConnectionManager* manager) {
    print_separator();
    printf("%s测试8: 删除公会记录%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    GuildInfo guild;

    // 查找一个公会
    if (GuildInfo_GetByName(manager, "TestGuild_Batch_1", &guild) != 0) {
        print_error("无法找到测试公会");
        return -1;
    }

    int guild_id = guild.guild_id;
    printf("  准备删除: ID=%d, 名称=%s\n", guild_id, guild.guild_name);

    // 删除公会
    print_info("删除公会");
    if (GuildInfo_Delete(manager, guild_id) == 0) {
        print_success("删除成功");

        // 验证删除
        if (!GuildInfo_Exists(manager, guild_id)) {
            print_success("验证: 公会已被删除");
        } else {
            print_error("验证失败: 公会仍然存在");
            return -1;
        }
    } else {
        print_error("删除失败");
        return -1;
    }

    // 尝试删除不存在的公会
    print_info("删除不存在的公会");
    if (GuildInfo_Delete(manager, 999999) == 0) {
        print_success("返回成功(没有行被删除)");
    } else {
        print_warning("返回失败");
    }

    printf("\n");
    return 0;
}

int main(int argc, char* argv[]) {
    printf("\n");
    print_separator();
    printf("%s  DNF游戏服务器 - GuildInfo表ORM测试  %s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();
    printf("\n");

    if (argc < 2) {
        print_error("用法: test_guild_info_orm <配置文件路径>");
        printf("  示例: ./test_guild_info_orm ../config/database.cfg\n");
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

    // 连接到公会数据库 (guild_info表在test_d_guild)
    print_info("连接到公会数据库 (test_d_guild)...");
    if (DBConnectionManager_Connect(&manager, DB_TYPE_GUILD) != 0) {
        print_error("连接失败");
        return 1;
    }
    print_success("连接成功");
    printf("\n");

    // 清理旧测试数据
    print_info("清理旧测试数据...");
    char cleanup_query[256];
    snprintf(cleanup_query, sizeof(cleanup_query), "DELETE FROM guild_info WHERE guild_name LIKE 'TestGuild%%'");
    DBQueryResult cleanup_result;
    memset(&cleanup_result, 0, sizeof(DBQueryResult));
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_GUILD, cleanup_query, &cleanup_result);
    DBQueryResult_Free(&cleanup_result);
    print_success("清理完成");
    printf("\n");

    // 运行测试
    int success_count = 0;
    int total_tests = 8;

    if (test_create_guild(&manager) == 0) success_count++;
    if (test_query_guild(&manager) == 0) success_count++;
    if (test_update_guild(&manager) == 0) success_count++;
    if (test_existence_check(&manager) == 0) success_count++;
    if (test_batch_query(&manager) == 0) success_count++;
    if (test_statistics(&manager) == 0) success_count++;
    if (test_search(&manager) == 0) success_count++;
    if (test_delete_guild(&manager) == 0) success_count++;

    // 清理测试数据
    print_separator();
    print_info("清理测试数据...");
    memset(&cleanup_result, 0, sizeof(DBQueryResult));
    DBConnectionManager_ExecuteQuery(&manager, DB_TYPE_GUILD, cleanup_query, &cleanup_result);
    DBQueryResult_Free(&cleanup_result);
    print_success("清理完成");
    printf("\n");

    // 清理连接
    DBConnectionManager_Cleanup(&manager);

    // 输出测试结果
    print_separator();
    printf("\n");
    printf("%s测试结果汇总:%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();
    printf("  通过: %s%d%s\n", COLOR_GREEN, success_count, COLOR_RESET);
    printf("  失败: %s%d%s\n", COLOR_RED, total_tests - success_count, COLOR_RESET);
    printf("  总计: %d\n", total_tests);
    printf("  成功率: %.1f%%\n", (success_count * 100.0) / total_tests);
    print_separator();
    printf("\n");

    return (success_count == total_tests) ? 0 : 1;
}
