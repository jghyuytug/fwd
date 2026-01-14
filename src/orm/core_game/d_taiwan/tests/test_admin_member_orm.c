/**
 * DNF游戏服务器 - AdminMember表ORM测试程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "db_connection_manager.h"
#include "admin_member_orm.h"

/* 测试统计 */
static int test_count = 0;
static int test_passed = 0;

#define TEST_START(name) \
    do { \
        printf("\n[TEST %d] %s\n", ++test_count, name); \
    } while(0)

#define ASSERT_TRUE(expr, message) \
    do { \
        if (expr) { \
            printf("  ✓ %s\n", message); \
        } else { \
            printf("  ✗ %s (FAILED)\n", message); \
            return -1; \
        } \
    } while(0)

#define ASSERT_EQ(a, b, message) \
    do { \
        if ((a) == (b)) { \
            printf("  ✓ %s\n", message); \
        } else { \
            printf("  ✗ %s (FAILED: %d != %d)\n", message, (int)(a), (int)(b)); \
            return -1; \
        } \
    } while(0)

#define ASSERT_STR_EQ(a, b, message) \
    do { \
        if (strcmp((a), (b)) == 0) { \
            printf("  ✓ %s\n", message); \
        } else { \
            printf("  ✗ %s (FAILED: '%s' != '%s')\n", message, (a), (b)); \
            return -1; \
        } \
    } while(0)

/* 测试用例 */

int test_add_admin(DBConnectionManager* manager) {
    TEST_START("添加管理员记录");

    AdminMember admin;
    AdminMember_Init(&admin);
    strcpy(admin.user_id, "testadmin01");
    strcpy(admin.password, "pass123");
    strcpy(admin.name, "测试管理员1");
    strcpy(admin.email, "test@example.com");
    strcpy(admin.phone, "13800138000");
    admin.comment = strdup("测试管理员账号");
    admin.reg_date = (long long)time(NULL);
    admin.confirm = '1';
    strcpy(admin.level, "superadmin");
    strcpy(admin.level_group1, "01");

    int no = AdminMember_Add(manager, &admin);
    ASSERT_TRUE(no > 0, "添加成功，获得编号");
    printf("    管理员编号: %d\n", no);

    AdminMember_Free(&admin);
    return 0;
}

int test_get_admin(DBConnectionManager* manager) {
    TEST_START("获取管理员记录");

    AdminMember admin;
    AdminMember_Init(&admin);

    ASSERT_EQ(AdminMember_GetByUserId(manager, "testadmin01", &admin), 0, "按user_id查询成功");
    ASSERT_STR_EQ(admin.user_id, "testadmin01", "用户ID匹配");
    ASSERT_STR_EQ(admin.password, "pass123", "密码匹配");
    ASSERT_STR_EQ(admin.name, "测试管理员1", "姓名匹配");
    ASSERT_TRUE(admin.confirm == '1', "确认状态正确");

    printf("    管理员编号: %d\n", admin.no);

    int test_no = admin.no;
    AdminMember_Free(&admin);

    AdminMember_Init(&admin);
    ASSERT_EQ(AdminMember_Get(manager, test_no, &admin), 0, "按no查询成功");
    ASSERT_STR_EQ(admin.user_id, "testadmin01", "再次查询user_id匹配");

    AdminMember_Free(&admin);
    return 0;
}

int test_update_admin(DBConnectionManager* manager) {
    TEST_START("更新管理员记录");

    AdminMember admin;
    AdminMember_Init(&admin);

    ASSERT_EQ(AdminMember_GetByUserId(manager, "testadmin01", &admin), 0, "获取记录成功");

    strcpy(admin.email, "newemail@test.com");
    strcpy(admin.phone, "13900139000");
    admin.confirm = '0';

    ASSERT_EQ(AdminMember_Update(manager, &admin), 0, "更新成功");

    AdminMember_Free(&admin);

    AdminMember_Init(&admin);
    ASSERT_EQ(AdminMember_GetByUserId(manager, "testadmin01", &admin), 0, "再次获取记录");
    ASSERT_STR_EQ(admin.email, "newemail@test.com", "邮箱已更新");
    ASSERT_STR_EQ(admin.phone, "13900139000", "电话已更新");
    ASSERT_TRUE(admin.confirm == '0', "确认状态已更新");

    AdminMember_Free(&admin);
    return 0;
}

int test_verify(DBConnectionManager* manager) {
    TEST_START("验证管理员登录");

    int no = AdminMember_Verify(manager, "testadmin01", "pass123");
    ASSERT_TRUE(no > 0, "验证成功");
    printf("    验证通过，管理员编号: %d\n", no);

    no = AdminMember_Verify(manager, "testadmin01", "wrongpass");
    ASSERT_TRUE(no < 0, "错误密码验证失败");

    no = AdminMember_Verify(manager, "nonexistent", "pass123");
    ASSERT_TRUE(no < 0, "不存在的用户验证失败");

    return 0;
}

int test_update_functions(DBConnectionManager* manager) {
    TEST_START("专项更新函数");

    AdminMember admin;
    AdminMember_Init(&admin);
    ASSERT_EQ(AdminMember_GetByUserId(manager, "testadmin01", &admin), 0, "获取记录");
    int test_no = admin.no;
    AdminMember_Free(&admin);

    ASSERT_EQ(AdminMember_UpdatePassword(manager, test_no, "newpass456"), 0, "更新密码成功");

    int verify_no = AdminMember_Verify(manager, "testadmin01", "newpass456");
    ASSERT_TRUE(verify_no > 0, "新密码验证通过");

    ASSERT_EQ(AdminMember_UpdateConfirm(manager, test_no, '1'), 0, "更新确认状态成功");
    ASSERT_EQ(AdminMember_UpdateContact(manager, test_no, "contact@example.com", "18888888888"), 0, "更新联系信息成功");
    ASSERT_EQ(AdminMember_UpdateLevel(manager, test_no, "admin_level_99"), 0, "更新权限级别成功");

    AdminMember_Init(&admin);
    ASSERT_EQ(AdminMember_Get(manager, test_no, &admin), 0, "验证获取");
    ASSERT_STR_EQ(admin.email, "contact@example.com", "联系邮箱已更新");
    ASSERT_STR_EQ(admin.phone, "18888888888", "联系电话已更新");
    ASSERT_STR_EQ(admin.level, "admin_level_99", "权限级别已更新");
    ASSERT_TRUE(admin.confirm == '1', "确认状态已更新");
    AdminMember_Free(&admin);

    return 0;
}

int test_add_more_admins(DBConnectionManager* manager) {
    TEST_START("添加更多管理员");

    for (int i = 2; i <= 5; i++) {
        AdminMember admin;
        AdminMember_Init(&admin);

        sprintf(admin.user_id, "testadmin%02d", i);
        sprintf(admin.password, "pass%d", i);
        sprintf(admin.name, "测试管理员%d", i);
        admin.reg_date = (long long)time(NULL);
        admin.confirm = (i % 2 == 0) ? '1' : '0';
        sprintf(admin.level, "level_%d", i);

        int no = AdminMember_Add(manager, &admin);
        ASSERT_TRUE(no > 0, "添加管理员成功");
        AdminMember_Free(&admin);
    }

    return 0;
}

int test_exists_and_count(DBConnectionManager* manager) {
    TEST_START("存在性检查和计数");

    ASSERT_TRUE(AdminMember_UserIdExists(manager, "testadmin01"), "testadmin01存在");
    ASSERT_TRUE(!AdminMember_UserIdExists(manager, "nonexistent"), "nonexistent不存在");

    AdminMember admin;
    AdminMember_Init(&admin);
    ASSERT_EQ(AdminMember_GetByUserId(manager, "testadmin01", &admin), 0, "获取记录");
    ASSERT_TRUE(AdminMember_Exists(manager, admin.no), "按no存在");
    AdminMember_Free(&admin);

    int count = AdminMember_GetCount(manager);
    ASSERT_TRUE(count >= 5, "管理员总数 >= 5");
    printf("    管理员总数: %d\n", count);

    return 0;
}

int test_get_all_and_confirmed(DBConnectionManager* manager) {
    TEST_START("获取所有和已确认管理员");

    AdminMember admins[100];
    for (int i = 0; i < 100; i++) {
        AdminMember_Init(&admins[i]);
    }

    int count = AdminMember_GetAll(manager, admins, 100);
    ASSERT_TRUE(count >= 5, "获取所有管理员成功");
    printf("    获取到 %d 个管理员\n", count);

    for (int i = 0; i < count; i++) {
        AdminMember_Free(&admins[i]);
        AdminMember_Init(&admins[i]);
    }

    int confirmed_count = AdminMember_GetConfirmed(manager, admins, 100);
    ASSERT_TRUE(confirmed_count >= 1, "获取已确认管理员成功");
    printf("    获取到 %d 个已确认管理员\n", confirmed_count);

    for (int i = 0; i < confirmed_count; i++) {
        ASSERT_TRUE(admins[i].confirm == '1', "确认状态为'1'");
        AdminMember_Free(&admins[i]);
    }

    return 0;
}

int test_search(DBConnectionManager* manager) {
    TEST_START("搜索管理员");

    AdminMember admins[100];
    for (int i = 0; i < 100; i++) {
        AdminMember_Init(&admins[i]);
    }

    int count = AdminMember_Search(manager, "testadmin", admins, 100);
    ASSERT_TRUE(count >= 5, "搜索到管理员");
    printf("    搜索到 %d 个管理员\n", count);

    for (int i = 0; i < count; i++) {
        AdminMember_Free(&admins[i]);
    }

    return 0;
}

int test_delete(DBConnectionManager* manager) {
    TEST_START("删除管理员");

    AdminMember admin;
    AdminMember_Init(&admin);
    ASSERT_EQ(AdminMember_GetByUserId(manager, "testadmin05", &admin), 0, "获取要删除的记录");
    int delete_no = admin.no;
    AdminMember_Free(&admin);

    ASSERT_EQ(AdminMember_Delete(manager, delete_no), 0, "删除成功");
    ASSERT_TRUE(!AdminMember_Exists(manager, delete_no), "记录已不存在");

    return 0;
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc < 2) {
        fprintf(stderr, "用法: %s <config_file>\n", argv[0]);
        return 1;
    }

    printf("========================================\n");
    printf("管理员表 ORM 测试\n");
    printf("========================================\n");

    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        fprintf(stderr, "初始化数据库连接管理器失败\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        fprintf(stderr, "连接到数据库失败\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    /* 运行测试用例 */
    if (test_add_admin(&manager) == 0) test_passed++;
    if (test_get_admin(&manager) == 0) test_passed++;
    if (test_update_admin(&manager) == 0) test_passed++;
    if (test_verify(&manager) == 0) test_passed++;
    if (test_update_functions(&manager) == 0) test_passed++;
    if (test_add_more_admins(&manager) == 0) test_passed++;
    if (test_exists_and_count(&manager) == 0) test_passed++;
    if (test_get_all_and_confirmed(&manager) == 0) test_passed++;
    if (test_search(&manager) == 0) test_passed++;
    if (test_delete(&manager) == 0) test_passed++;

    DBConnectionManager_Cleanup(&manager);

    printf("\n========================================\n");
    printf("测试用例: %d/%d 通过\n", test_passed, test_count);
    printf("========================================\n");

    return (test_passed == test_count) ? 0 : 1;
}
