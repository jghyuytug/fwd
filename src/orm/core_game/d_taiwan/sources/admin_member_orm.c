/**
 * DNF游戏服务器 - AdminMember表ORM实现
 */

#include "admin_member_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========================================================================
// 内部辅助函数
// ========================================================================

/**
 * 从查询结果行填充管理员结构
 */
static void FillAdminFromRow(char** row, AdminMember* admin) {
    admin->no = atoi(row[0]);
    strncpy(admin->user_id, row[1], sizeof(admin->user_id) - 1);
    admin->user_id[sizeof(admin->user_id) - 1] = '\0';
    strncpy(admin->password, row[2], sizeof(admin->password) - 1);
    admin->password[sizeof(admin->password) - 1] = '\0';
    strncpy(admin->name, row[3], sizeof(admin->name) - 1);
    admin->name[sizeof(admin->name) - 1] = '\0';

    // email (可选)
    if (row[4] && strlen(row[4]) > 0) {
        strncpy(admin->email, row[4], sizeof(admin->email) - 1);
        admin->email[sizeof(admin->email) - 1] = '\0';
    } else {
        admin->email[0] = '\0';
    }

    // phone (可选)
    if (row[5] && strlen(row[5]) > 0) {
        strncpy(admin->phone, row[5], sizeof(admin->phone) - 1);
        admin->phone[sizeof(admin->phone) - 1] = '\0';
    } else {
        admin->phone[0] = '\0';
    }

    // msn (可选)
    if (row[6] && strlen(row[6]) > 0) {
        strncpy(admin->msn, row[6], sizeof(admin->msn) - 1);
        admin->msn[sizeof(admin->msn) - 1] = '\0';
    } else {
        admin->msn[0] = '\0';
    }

    // comment (text字段，动态分配)
    if (admin->comment) {
        free(admin->comment);
        admin->comment = NULL;
    }
    if (row[7] && strlen(row[7]) > 0) {
        admin->comment = strdup(row[7]);
    }

    // reg_date (可选)
    if (row[8] && strlen(row[8]) > 0) {
        admin->reg_date = atoll(row[8]);
    } else {
        admin->reg_date = 0;
    }

    // confirm (可选，默认'0')
    if (row[9] && strlen(row[9]) > 0) {
        admin->confirm = row[9][0];
    } else {
        admin->confirm = '0';
    }

    // level
    strncpy(admin->level, row[10], sizeof(admin->level) - 1);
    admin->level[sizeof(admin->level) - 1] = '\0';

    // level_group1-6
    strncpy(admin->level_group1, row[11], sizeof(admin->level_group1) - 1);
    admin->level_group1[sizeof(admin->level_group1) - 1] = '\0';
    strncpy(admin->level_group2, row[12], sizeof(admin->level_group2) - 1);
    admin->level_group2[sizeof(admin->level_group2) - 1] = '\0';
    strncpy(admin->level_group3, row[13], sizeof(admin->level_group3) - 1);
    admin->level_group3[sizeof(admin->level_group3) - 1] = '\0';
    strncpy(admin->level_group4, row[14], sizeof(admin->level_group4) - 1);
    admin->level_group4[sizeof(admin->level_group4) - 1] = '\0';
    strncpy(admin->level_group5, row[15], sizeof(admin->level_group5) - 1);
    admin->level_group5[sizeof(admin->level_group5) - 1] = '\0';
    strncpy(admin->level_group6, row[16], sizeof(admin->level_group6) - 1);
    admin->level_group6[sizeof(admin->level_group6) - 1] = '\0';
}

// ========================================================================
// CRUD操作实现
// ========================================================================

int AdminMember_Add(DBConnectionManager* manager, AdminMember* admin) {
    if (!manager || !admin) {
        return -1;
    }

    char query[4096];  // 大查询（level字段最大2000字符）
    char* escaped_comment = NULL;

    // 转义comment字段
    if (admin->comment && strlen(admin->comment) > 0) {
        size_t len = strlen(admin->comment);
        escaped_comment = (char*)malloc(len * 2 + 1);
        // 简单转义：替换单引号为两个单引号
        char* src = admin->comment;
        char* dst = escaped_comment;
        while (*src) {
            if (*src == '\'') {
                *dst++ = '\'';
                *dst++ = '\'';
            } else {
                *dst++ = *src;
            }
            src++;
        }
        *dst = '\0';
    }

    snprintf(query, sizeof(query),
             "INSERT INTO admin_member "
             "(user_id, password, name, email, phone, msn, comment, reg_date, confirm, "
             "level, level_group1, level_group2, level_group3, level_group4, level_group5, level_group6) "
             "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', %lld, '%c', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
             admin->user_id, admin->password, admin->name,
             admin->email, admin->phone, admin->msn,
             escaped_comment ? escaped_comment : "",
             admin->reg_date, admin->confirm,
             admin->level,
             admin->level_group1, admin->level_group2, admin->level_group3,
             admin->level_group4, admin->level_group5, admin->level_group6);

    if (escaped_comment) {
        free(escaped_comment);
    }

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // 获取插入的auto_increment ID
    snprintf(query, sizeof(query), "SELECT LAST_INSERT_ID()");
    DBQueryResult_Free(&result);
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    int new_no = -1;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        new_no = atoi(values[0]);
        admin->no = new_no;
    }

    DBQueryResult_Free(&result);
    return new_no;
}

int AdminMember_Get(DBConnectionManager* manager, int no, AdminMember* admin) {
    if (!manager || !admin) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT no, user_id, password, name, email, phone, msn, comment, reg_date, confirm, "
             "level, level_group1, level_group2, level_group3, level_group4, level_group5, level_group6 "
             "FROM admin_member WHERE no = %d",
             no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[17];
    int ret = -1;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        FillAdminFromRow(values, admin);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int AdminMember_GetByUserId(DBConnectionManager* manager, const char* user_id, AdminMember* admin) {
    if (!manager || !user_id || !admin) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT no, user_id, password, name, email, phone, msn, comment, reg_date, confirm, "
             "level, level_group1, level_group2, level_group3, level_group4, level_group5, level_group6 "
             "FROM admin_member WHERE user_id = '%s'",
             user_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[17];
    int ret = -1;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        FillAdminFromRow(values, admin);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int AdminMember_Update(DBConnectionManager* manager, const AdminMember* admin) {
    if (!manager || !admin) {
        return -1;
    }

    char query[4096];
    char* escaped_comment = NULL;

    // 转义comment字段
    if (admin->comment && strlen(admin->comment) > 0) {
        size_t len = strlen(admin->comment);
        escaped_comment = (char*)malloc(len * 2 + 1);
        char* src = admin->comment;
        char* dst = escaped_comment;
        while (*src) {
            if (*src == '\'') {
                *dst++ = '\'';
                *dst++ = '\'';
            } else {
                *dst++ = *src;
            }
            src++;
        }
        *dst = '\0';
    }

    snprintf(query, sizeof(query),
             "UPDATE admin_member SET "
             "user_id='%s', password='%s', name='%s', email='%s', phone='%s', msn='%s', "
             "comment='%s', reg_date=%lld, confirm='%c', level='%s', "
             "level_group1='%s', level_group2='%s', level_group3='%s', "
             "level_group4='%s', level_group5='%s', level_group6='%s' "
             "WHERE no=%d",
             admin->user_id, admin->password, admin->name,
             admin->email, admin->phone, admin->msn,
             escaped_comment ? escaped_comment : "",
             admin->reg_date, admin->confirm, admin->level,
             admin->level_group1, admin->level_group2, admin->level_group3,
             admin->level_group4, admin->level_group5, admin->level_group6,
             admin->no);

    if (escaped_comment) {
        free(escaped_comment);
    }

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AdminMember_Delete(DBConnectionManager* manager, int no) {
    if (!manager) {
        return -1;
    }

    char query[128];
    snprintf(query, sizeof(query),
             "DELETE FROM admin_member WHERE no=%d", no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

// ========================================================================
// 专项操作实现
// ========================================================================

int AdminMember_Verify(DBConnectionManager* manager, const char* user_id, const char* password) {
    if (!manager || !user_id || !password) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT no FROM admin_member WHERE user_id='%s' AND password='%s'",
             user_id, password);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    int no = -1;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        no = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return no;
}

int AdminMember_UpdatePassword(DBConnectionManager* manager, int no, const char* new_password) {
    if (!manager || !new_password) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE admin_member SET password='%s' WHERE no=%d",
             new_password, no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AdminMember_UpdateConfirm(DBConnectionManager* manager, int no, char confirm) {
    if (!manager) {
        return -1;
    }

    char query[128];
    snprintf(query, sizeof(query),
             "UPDATE admin_member SET confirm='%c' WHERE no=%d",
             confirm, no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AdminMember_UpdateContact(DBConnectionManager* manager, int no, const char* email, const char* phone) {
    if (!manager || (!email && !phone)) {
        return -1;
    }

    char query[512];
    char* pos = query;
    int remaining = sizeof(query);

    int written = snprintf(pos, remaining, "UPDATE admin_member SET ");
    pos += written;
    remaining -= written;

    int first = 1;
    if (email) {
        written = snprintf(pos, remaining, "%semail='%s'", first ? "" : ", ", email);
        pos += written;
        remaining -= written;
        first = 0;
    }

    if (phone) {
        written = snprintf(pos, remaining, "%sphone='%s'", first ? "" : ", ", phone);
        pos += written;
        remaining -= written;
    }

    snprintf(pos, remaining, " WHERE no=%d", no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AdminMember_UpdateLevel(DBConnectionManager* manager, int no, const char* level) {
    if (!manager || !level) {
        return -1;
    }

    char query[2512];
    snprintf(query, sizeof(query),
             "UPDATE admin_member SET level='%s' WHERE no=%d",
             level, no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

bool AdminMember_UserIdExists(DBConnectionManager* manager, const char* user_id) {
    if (!manager || !user_id) {
        return false;
    }

    char query[128];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM admin_member WHERE user_id='%s'", user_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return (count > 0);
}

// ========================================================================
// 查询操作实现
// ========================================================================

bool AdminMember_Exists(DBConnectionManager* manager, int no) {
    if (!manager) {
        return false;
    }

    char query[128];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM admin_member WHERE no=%d", no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return (count > 0);
}

int AdminMember_GetAll(DBConnectionManager* manager, AdminMember* admins, int max_count) {
    if (!manager || !admins || max_count <= 0) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT no, user_id, password, name, email, phone, msn, comment, reg_date, confirm, "
             "level, level_group1, level_group2, level_group3, level_group4, level_group5, level_group6 "
             "FROM admin_member ORDER BY no LIMIT %d",
             max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[17];
    int count = 0;

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_count) {
        FillAdminFromRow(values, &admins[count]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int AdminMember_GetConfirmed(DBConnectionManager* manager, AdminMember* admins, int max_count) {
    if (!manager || !admins || max_count <= 0) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT no, user_id, password, name, email, phone, msn, comment, reg_date, confirm, "
             "level, level_group1, level_group2, level_group3, level_group4, level_group5, level_group6 "
             "FROM admin_member WHERE confirm='1' ORDER BY no LIMIT %d",
             max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[17];
    int count = 0;

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_count) {
        FillAdminFromRow(values, &admins[count]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int AdminMember_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[64];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM admin_member");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int AdminMember_Search(DBConnectionManager* manager, const char* keyword, AdminMember* admins, int max_count) {
    if (!manager || !keyword || !admins || max_count <= 0) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT no, user_id, password, name, email, phone, msn, comment, reg_date, confirm, "
             "level, level_group1, level_group2, level_group3, level_group4, level_group5, level_group6 "
             "FROM admin_member WHERE user_id LIKE '%%%s%%' OR name LIKE '%%%s%%' "
             "ORDER BY no LIMIT %d",
             keyword, keyword, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[17];
    int count = 0;

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_count) {
        FillAdminFromRow(values, &admins[count]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

// ========================================================================
// 工具函数实现
// ========================================================================

void AdminMember_Init(AdminMember* admin) {
    if (!admin) {
        return;
    }

    memset(admin, 0, sizeof(AdminMember));
    admin->no = 0;
    admin->user_id[0] = '\0';
    admin->password[0] = '\0';
    admin->name[0] = '\0';
    admin->email[0] = '\0';
    admin->phone[0] = '\0';
    admin->msn[0] = '\0';
    admin->comment = NULL;
    admin->reg_date = 0;
    admin->confirm = '0';
    admin->level[0] = '\0';
    strcpy(admin->level_group1, "_");
    strcpy(admin->level_group2, "_");
    strcpy(admin->level_group3, "_");
    strcpy(admin->level_group4, "_");
    strcpy(admin->level_group5, "_");
    strcpy(admin->level_group6, "_");
}

void AdminMember_Free(AdminMember* admin) {
    if (!admin) {
        return;
    }

    if (admin->comment) {
        free(admin->comment);
        admin->comment = NULL;
    }
}

void AdminMember_PrintInfo(const AdminMember* admin) {
    if (!admin) {
        printf("  [AdminMember] NULL admin\n");
        return;
    }

    printf("  ----------------------------------------\n");
    printf("  管理员编号: %d\n", admin->no);
    printf("  用户ID: %s\n", admin->user_id);
    printf("  密码: %s\n", admin->password);
    printf("  姓名: %s\n", admin->name);
    printf("  邮箱: %s\n", admin->email[0] ? admin->email : "(未设置)");
    printf("  电话: %s\n", admin->phone[0] ? admin->phone : "(未设置)");
    printf("  MSN: %s\n", admin->msn[0] ? admin->msn : "(未设置)");
    printf("  备注: %s\n", admin->comment ? admin->comment : "(无)");
    printf("  注册日期: %lld\n", admin->reg_date);
    printf("  确认状态: %c (%s)\n", admin->confirm, admin->confirm == '1' ? "已确认" : "未确认");
    printf("  权限级别: %s\n", admin->level[0] ? admin->level : "(无)");
    printf("  权限组: [%s] [%s] [%s] [%s] [%s] [%s]\n",
           admin->level_group1, admin->level_group2, admin->level_group3,
           admin->level_group4, admin->level_group5, admin->level_group6);
    printf("  ----------------------------------------\n");
}
