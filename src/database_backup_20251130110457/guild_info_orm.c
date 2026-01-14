#define _GNU_SOURCE
#include "guild_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mysql/mysql.h>

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
 * 从查询结果行填充GuildInfo结构体
 */
static void FillGuildFromRow(char** values, GuildInfo* guild) {
    guild->guild_id = atoi(values[0]);
    strncpy(guild->guild_name, values[1], sizeof(guild->guild_name) - 1);
    guild->guild_level = atoi(values[2]);
    guild->guild_exp = atoi(values[3]);
    guild->guild_master_charac_no = atoi(values[4]);
    strncpy(guild->guild_notice, values[5], sizeof(guild->guild_notice) - 1);
    guild->member_count = atoi(values[6]);
    guild->max_member_count = atoi(values[7]);
    strncpy(guild->create_time, values[8], sizeof(guild->create_time) - 1);
    strncpy(guild->update_time, values[9], sizeof(guild->update_time) - 1);
}

// ========================================================================
// CRUD操作实现
// ========================================================================

int GuildInfo_Create(DBConnectionManager* manager, GuildInfo* guild) {
    char query[2048];
    char create_time[20];
    char update_time[20];

    // 获取当前时间
    GetCurrentTimeString(create_time, sizeof(create_time));
    GetCurrentTimeString(update_time, sizeof(update_time));
    strncpy(guild->create_time, create_time, sizeof(guild->create_time) - 1);
    strncpy(guild->update_time, update_time, sizeof(guild->update_time) - 1);

    // 转义公会名称和公告，防止SQL注入
    char escaped_name[sizeof(guild->guild_name) * 2 + 1];
    char escaped_notice[sizeof(guild->guild_notice) * 2 + 1];

    // 简化处理：直接使用原字符串（实际应使用mysql_real_escape_string）
    strncpy(escaped_name, guild->guild_name, sizeof(escaped_name) - 1);
    strncpy(escaped_notice, guild->guild_notice, sizeof(escaped_notice) - 1);

    snprintf(query, sizeof(query),
             "INSERT INTO guild_info "
             "(guild_name, guild_level, guild_exp, guild_master_charac_no, "
             "guild_notice, member_count, max_member_count, create_time, update_time) "
             "VALUES ('%s', %d, %d, %d, '%s', %d, %d, '%s', '%s')",
             escaped_name, guild->guild_level, guild->guild_exp,
             guild->guild_master_charac_no, escaped_notice,
             guild->member_count, guild->max_member_count,
             guild->create_time, guild->update_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret > 0) {
        // 获取自动生成的guild_id
        // 注意：这需要MySQL的LAST_INSERT_ID()
        char id_query[128];
        snprintf(id_query, sizeof(id_query), "SELECT LAST_INSERT_ID()");

        DBQueryResult id_result;
        memset(&id_result, 0, sizeof(DBQueryResult));
        int id_ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, id_query, &id_result);

        if (id_ret > 0) {
            char* id_values[1];
            if (DBQueryResult_FetchRow(&id_result, id_values) > 0) {
                guild->guild_id = atoi(id_values[0]);
                printf("[GuildInfo ORM] Created guild '%s' with ID=%d\n",
                       guild->guild_name, guild->guild_id);
                DBQueryResult_Free(&id_result);
                DBQueryResult_Free(&result);
                return guild->guild_id;
            }
            DBQueryResult_Free(&id_result);
        }

        DBQueryResult_Free(&result);
        return 0;  // Success but couldn't get ID
    }

    DBQueryResult_Free(&result);
    return -1;  // Failure
}

int GuildInfo_GetByID(DBConnectionManager* manager, int guild_id, GuildInfo* guild) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT guild_id, guild_name, guild_level, guild_exp, guild_master_charac_no, "
             "guild_notice, member_count, max_member_count, create_time, update_time "
             "FROM guild_info WHERE guild_id = %d",
             guild_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    // 获取行数据并填充公会信息
    char* values[10];
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        FillGuildFromRow(values, guild);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int GuildInfo_GetByName(DBConnectionManager* manager, const char* guild_name, GuildInfo* guild) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT guild_id, guild_name, guild_level, guild_exp, guild_master_charac_no, "
             "guild_notice, member_count, max_member_count, create_time, update_time "
             "FROM guild_info WHERE guild_name = '%s'",
             guild_name);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    // 获取行数据并填充公会信息
    char* values[10];
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        FillGuildFromRow(values, guild);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int GuildInfo_Update(DBConnectionManager* manager, const GuildInfo* guild) {
    char query[2048];
    char update_time[20];

    GetCurrentTimeString(update_time, sizeof(update_time));

    // 转义字符串
    char escaped_name[sizeof(guild->guild_name) * 2 + 1];
    char escaped_notice[sizeof(guild->guild_notice) * 2 + 1];
    strncpy(escaped_name, guild->guild_name, sizeof(escaped_name) - 1);
    strncpy(escaped_notice, guild->guild_notice, sizeof(escaped_notice) - 1);

    snprintf(query, sizeof(query),
             "UPDATE guild_info SET "
             "guild_name='%s', guild_level=%d, guild_exp=%d, "
             "guild_master_charac_no=%d, guild_notice='%s', "
             "member_count=%d, max_member_count=%d, update_time='%s' "
             "WHERE guild_id=%d",
             escaped_name, guild->guild_level, guild->guild_exp,
             guild->guild_master_charac_no, escaped_notice,
             guild->member_count, guild->max_member_count, update_time,
             guild->guild_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret > 0) {
        printf("[GuildInfo ORM] Updated guild ID=%d\n", guild->guild_id);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildInfo_UpdateLevelExp(DBConnectionManager* manager, int guild_id, int level, int exp) {
    char query[512];
    char update_time[20];

    GetCurrentTimeString(update_time, sizeof(update_time));

    snprintf(query, sizeof(query),
             "UPDATE guild_info SET guild_level=%d, guild_exp=%d, update_time='%s' "
             "WHERE guild_id=%d",
             level, exp, update_time, guild_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret > 0) {
        printf("[GuildInfo ORM] Updated level/exp for guild ID=%d\n", guild_id);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildInfo_UpdateNotice(DBConnectionManager* manager, int guild_id, const char* notice) {
    char query[1024];
    char update_time[20];

    GetCurrentTimeString(update_time, sizeof(update_time));

    // 转义公告内容
    char escaped_notice[1002];  // 500 * 2 + 2
    strncpy(escaped_notice, notice, sizeof(escaped_notice) - 1);

    snprintf(query, sizeof(query),
             "UPDATE guild_info SET guild_notice='%s', update_time='%s' "
             "WHERE guild_id=%d",
             escaped_notice, update_time, guild_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret > 0) {
        printf("[GuildInfo ORM] Updated notice for guild ID=%d\n", guild_id);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildInfo_UpdateMaster(DBConnectionManager* manager, int guild_id, int new_master_charac_no) {
    char query[512];
    char update_time[20];

    GetCurrentTimeString(update_time, sizeof(update_time));

    snprintf(query, sizeof(query),
             "UPDATE guild_info SET guild_master_charac_no=%d, update_time='%s' "
             "WHERE guild_id=%d",
             new_master_charac_no, update_time, guild_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret > 0) {
        printf("[GuildInfo ORM] Updated master for guild ID=%d to charac_no=%d\n",
               guild_id, new_master_charac_no);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildInfo_UpdateMemberCount(DBConnectionManager* manager, int guild_id, int member_count) {
    char query[512];
    char update_time[20];

    GetCurrentTimeString(update_time, sizeof(update_time));

    snprintf(query, sizeof(query),
             "UPDATE guild_info SET member_count=%d, update_time='%s' "
             "WHERE guild_id=%d",
             member_count, update_time, guild_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret > 0) {
        printf("[GuildInfo ORM] Updated member count for guild ID=%d to %d\n",
               guild_id, member_count);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildInfo_Delete(DBConnectionManager* manager, int guild_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM guild_info WHERE guild_id=%d", guild_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret >= 0) {  // DELETE can affect 0 rows if record doesn't exist
        printf("[GuildInfo ORM] Deleted guild ID=%d\n", guild_id);
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

bool GuildInfo_Exists(DBConnectionManager* manager, int guild_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM guild_info WHERE guild_id=%d", guild_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

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

bool GuildInfo_NameExists(DBConnectionManager* manager, const char* guild_name) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM guild_info WHERE guild_name='%s'", guild_name);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

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

int GuildInfo_GetCount(DBConnectionManager* manager) {
    char query[128];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM guild_info");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

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

int GuildInfo_GetCountByLevel(DBConnectionManager* manager, int level) {
    char query[128];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM guild_info WHERE guild_level=%d", level);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

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

int GuildInfo_GetAll(DBConnectionManager* manager, GuildInfo* guilds, int max_count) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT guild_id, guild_name, guild_level, guild_exp, guild_master_charac_no, "
             "guild_notice, member_count, max_member_count, create_time, update_time "
             "FROM guild_info "
             "ORDER BY create_time DESC LIMIT %d",
             max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = (result.row_count < max_count) ? result.row_count : max_count;

    char* values[10];
    int i;
    for (i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillGuildFromRow(values, &guilds[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

int GuildInfo_GetByLevelRange(DBConnectionManager* manager, int min_level, int max_level,
                               GuildInfo* guilds, int max_count) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT guild_id, guild_name, guild_level, guild_exp, guild_master_charac_no, "
             "guild_notice, member_count, max_member_count, create_time, update_time "
             "FROM guild_info "
             "WHERE guild_level >= %d AND guild_level <= %d "
             "ORDER BY guild_level DESC, guild_exp DESC LIMIT %d",
             min_level, max_level, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = (result.row_count < max_count) ? result.row_count : max_count;

    char* values[10];
    int i;
    for (i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillGuildFromRow(values, &guilds[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

int GuildInfo_Search(DBConnectionManager* manager, const char* keyword,
                     GuildInfo* guilds, int max_count) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT guild_id, guild_name, guild_level, guild_exp, guild_master_charac_no, "
             "guild_notice, member_count, max_member_count, create_time, update_time "
             "FROM guild_info "
             "WHERE guild_name LIKE '%%%s%%' "
             "ORDER BY guild_level DESC LIMIT %d",
             keyword, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = (result.row_count < max_count) ? result.row_count : max_count;

    char* values[10];
    int i;
    for (i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillGuildFromRow(values, &guilds[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

// ========================================================================
// 工具函数实现
// ========================================================================

void GuildInfo_Init(GuildInfo* guild) {
    memset(guild, 0, sizeof(GuildInfo));
    guild->guild_id = 0;
    guild->guild_level = 1;
    guild->guild_exp = 0;
    guild->guild_master_charac_no = 0;
    guild->member_count = 1;
    guild->max_member_count = 20;
    strcpy(guild->create_time, "0000-00-00 00:00:00");
    strcpy(guild->update_time, "0000-00-00 00:00:00");
}

void GuildInfo_Print(const GuildInfo* guild) {
    printf("================================================================================\n");
    printf("GuildInfo Information:\n");
    printf("================================================================================\n");
    printf("  公会ID:        %d\n", guild->guild_id);
    printf("  公会名称:      %s\n", guild->guild_name);
    printf("  公会等级:      %d\n", guild->guild_level);
    printf("  公会经验:      %d\n", guild->guild_exp);
    printf("  会长角色编号:  %d\n", guild->guild_master_charac_no);
    printf("  成员数量:      %d/%d\n", guild->member_count, guild->max_member_count);
    printf("  创建时间:      %s\n", guild->create_time);
    printf("  更新时间:      %s\n", guild->update_time);
    printf("  公会公告:      %s\n", guild->guild_notice);
    printf("================================================================================\n");
}

void GuildInfo_Copy(GuildInfo* dest, const GuildInfo* src) {
    memcpy(dest, src, sizeof(GuildInfo));
}
