/**
 * DNF游戏服务器 - AdminMember表ORM实现
 */

#define _GNU_SOURCE
#include "admin_member_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ========================================================================
// 内部辅助函数
// ========================================================================

/**
 * 获取当前时间字符串 (YYYY-MM-DD HH:MM:SS)
 */
static void GetCurrentTimeString(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    snprintf(buffer, size, "%04d-%02d-%02d %02d:%02d:%02d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
}

/**
 * 从查询结果行填充管理员结构
 */
static void FillAdminFromRow(char** row, AdminMember* admin) {
    admin->m_id = atoi(row[0]);
    admin->admin_level = atoi(row[1]);
    strncpy(admin->create_time, row[2], sizeof(admin->create_time) - 1);
    strncpy(admin->update_time, row[3], sizeof(admin->update_time) - 1);
}

// ========================================================================
// CRUD操作实现
// ========================================================================

int AdminMember_Create(DBConnectionManager* manager, AdminMember* admin) {
    char query[512];
    char create_time[20];

    // 获取当前时间
    GetCurrentTimeString(create_time, sizeof(create_time));
    strncpy(admin->create_time, create_time, sizeof(admin->create_time) - 1);
    strncpy(admin->update_time, create_time, sizeof(admin->update_time) - 1);

    // 构建INSERT查询
    snprintf(query, sizeof(query),
             "INSERT INTO admin_member (m_id, admin_level, create_time, update_time) "
             "VALUES (%d, %d, '%s', '%s')",
             admin->m_id, admin->admin_level,
             admin->create_time, admin->update_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret > 0) {
        printf("[AdminMember ORM] Created admin member for account m_id=%d, level=%d\n",
               admin->m_id, admin->admin_level);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

int AdminMember_GetByID(DBConnectionManager* manager, int m_id, AdminMember* admin) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT m_id, admin_level, create_time, update_time "
             "FROM admin_member WHERE m_id = %d",
             m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    // 获取行数据并填充管理员信息
    char* values[4];
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        FillAdminFromRow(values, admin);
    } else {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AdminMember_Update(DBConnectionManager* manager, AdminMember* admin) {
    char query[512];
    char update_time[20];

    // 更新时间
    GetCurrentTimeString(update_time, sizeof(update_time));
    strncpy(admin->update_time, update_time, sizeof(admin->update_time) - 1);

    snprintf(query, sizeof(query),
             "UPDATE admin_member SET admin_level=%d, update_time='%s' "
             "WHERE m_id=%d",
             admin->admin_level, admin->update_time, admin->m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret > 0) {
        printf("[AdminMember ORM] Updated admin member m_id=%d\n", admin->m_id);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

int AdminMember_UpdateLevel(DBConnectionManager* manager, int m_id, int new_level) {
    char query[512];
    char update_time[20];

    GetCurrentTimeString(update_time, sizeof(update_time));

    snprintf(query, sizeof(query),
             "UPDATE admin_member SET admin_level=%d, update_time='%s' "
             "WHERE m_id=%d",
             new_level, update_time, m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret > 0) {
        printf("[AdminMember ORM] Updated admin level for m_id=%d to level %d\n",
               m_id, new_level);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

int AdminMember_Delete(DBConnectionManager* manager, int m_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM admin_member WHERE m_id=%d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret >= 0) {  // DELETE can affect 0 rows if record doesn't exist
        printf("[AdminMember ORM] Deleted admin member m_id=%d\n", m_id);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

// ========================================================================
// 辅助查询函数实现
// ========================================================================

bool AdminMember_Exists(DBConnectionManager* manager, int m_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM admin_member WHERE m_id=%d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return false;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);

    return (count > 0);
}

bool AdminMember_HasLevel(DBConnectionManager* manager, int m_id, int required_level) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT admin_level FROM admin_member WHERE m_id=%d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return false;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    int level = atoi(values[0]);
    DBQueryResult_Free(&result);

    return (level >= required_level);
}

int AdminMember_GetByLevel(DBConnectionManager* manager, int level,
                           AdminMember* admins, int max_count) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT m_id, admin_level, create_time, update_time "
             "FROM admin_member WHERE admin_level=%d "
             "ORDER BY create_time LIMIT %d",
             level, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    /* Query execution error */
    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* No records found - return 0, not an error */
    if (ret == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int count = (result.row_count < max_count) ? result.row_count : max_count;

    char* values[4];
    for (int i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillAdminFromRow(values, &admins[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

int AdminMember_GetAll(DBConnectionManager* manager, AdminMember* admins, int max_count) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT m_id, admin_level, create_time, update_time "
             "FROM admin_member "
             "ORDER BY admin_level DESC, create_time LIMIT %d",
             max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = (result.row_count < max_count) ? result.row_count : max_count;

    char* values[4];
    for (int i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillAdminFromRow(values, &admins[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

int AdminMember_GetCount(DBConnectionManager* manager) {
    char query[128];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM admin_member");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);

    return count;
}

int AdminMember_GetCountByLevel(DBConnectionManager* manager, int level) {
    char query[128];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM admin_member WHERE admin_level=%d", level);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);

    return count;
}

// ========================================================================
// 工具函数实现
// ========================================================================

void AdminMember_Init(AdminMember* admin) {
    memset(admin, 0, sizeof(AdminMember));
    admin->m_id = 0;
    admin->admin_level = ADMIN_LEVEL_NONE;
    strcpy(admin->create_time, "0000-00-00 00:00:00");
    strcpy(admin->update_time, "0000-00-00 00:00:00");
}

void AdminMember_Print(const AdminMember* admin) {
    printf("================================================================================\n");
    printf("AdminMember Information:\n");
    printf("================================================================================\n");
    printf("  账号ID:        %d\n", admin->m_id);
    printf("  管理员等级:    %d (%s)\n", admin->admin_level,
           AdminMember_GetLevelName(admin->admin_level));
    printf("  创建时间:      %s\n", admin->create_time);
    printf("  更新时间:      %s\n", admin->update_time);
    printf("================================================================================\n");
}

const char* AdminMember_GetLevelName(int level) {
    switch (level) {
        case ADMIN_LEVEL_NONE:        return "普通玩家";
        case ADMIN_LEVEL_MODERATOR:   return "版主";
        case ADMIN_LEVEL_GM:          return "GM";
        case ADMIN_LEVEL_SENIOR_GM:   return "高级GM";
        case ADMIN_LEVEL_ADMIN:       return "管理员";
        case ADMIN_LEVEL_SUPER_ADMIN: return "超级管理员";
        case ADMIN_LEVEL_SYSTEM:      return "系统管理员";
        default:                      return "未知等级";
    }
}
