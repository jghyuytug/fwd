/**
 * DNF游戏服务器 - MemberPunishInfo表ORM实现
 * 处罚信息系统 - 用于管理玩家处罚记录
 */

#include "member_punish_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ========================================================================
 * 内部辅助函数
 * ======================================================================== */

/**
 * SQL字符串转义函数
 * 将单引号 ' 转义为 ''
 */
static void EscapeString(const char* src, char* dest, size_t dest_size) {
    size_t i, j;
    i = 0;
    j = 0;
    while (src[i] && j < dest_size - 2) {
        if (src[i] == '\'') {
            if (j < dest_size - 3) {
                dest[j++] = '\'';
                dest[j++] = '\'';
            }
        } else {
            dest[j++] = src[i];
        }
        i++;
    }
    dest[j] = '\0';
}

/* ========================================================================
 * 核心CRUD操作实现
 * ======================================================================== */

/**
 * 添加处罚记录
 * 使用 INSERT IGNORE 实现幂等性
 * @return 0=成功, -1=失败
 */
int MemberPunishInfo_Add(DBConnectionManager* manager, MemberPunishInfo* punish) {
    char query[2048];
    char escaped_admin_id[512];
    char escaped_reason[512];
    char escaped_ip[32];
    char escaped_mac[202];

    EscapeString(punish->admin_id, escaped_admin_id, sizeof(escaped_admin_id));
    EscapeString(punish->reason, escaped_reason, sizeof(escaped_reason));
    EscapeString(punish->ip, escaped_ip, sizeof(escaped_ip));
    EscapeString(punish->mac, escaped_mac, sizeof(escaped_mac));

    snprintf(query, sizeof(query),
             "INSERT IGNORE INTO member_punish_info "
             "(m_id, punish_type, occ_time, punish_value, apply_flag, start_time, end_time, "
             "admin_id, reason, IP, Mac) "
             "VALUES (%d, %d, '%s', %d, %d, '%s', '%s', '%s', '%s', '%s', '%s')",
             punish->m_id, punish->punish_type, punish->occ_time, punish->punish_value,
             punish->apply_flag, punish->start_time, punish->end_time,
             escaped_admin_id, escaped_reason, escaped_ip, escaped_mac);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (ret > 0) {
        printf("[MemberPunishInfo ORM] Added punish: m_id=%d, type=%d\n",
               punish->m_id, punish->punish_type);
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 获取指定会员的特定类型处罚记录
 * @return 0=成功找到, -1=未找到或失败
 */
int MemberPunishInfo_Get(DBConnectionManager* manager, int m_id, int punish_type,
                         MemberPunishInfo* punish) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT m_id, punish_type, occ_time, punish_value, apply_flag, start_time, end_time, "
             "admin_id, reason, IP, Mac FROM member_punish_info WHERE m_id=%d AND punish_type=%d",
             m_id, punish_type);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[11];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    MemberPunishInfo_Init(punish);
    punish->m_id = atoi(values[0]);
    punish->punish_type = atoi(values[1]);
    strncpy(punish->occ_time, values[2] ? values[2] : "", sizeof(punish->occ_time) - 1);
    punish->punish_value = atoi(values[3]);
    punish->apply_flag = atoi(values[4]);
    strncpy(punish->start_time, values[5] ? values[5] : "", sizeof(punish->start_time) - 1);
    strncpy(punish->end_time, values[6] ? values[6] : "", sizeof(punish->end_time) - 1);
    strncpy(punish->admin_id, values[7] ? values[7] : "", sizeof(punish->admin_id) - 1);
    strncpy(punish->reason, values[8] ? values[8] : "", sizeof(punish->reason) - 1);
    strncpy(punish->ip, values[9] ? values[9] : "", sizeof(punish->ip) - 1);
    strncpy(punish->mac, values[10] ? values[10] : "", sizeof(punish->mac) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 获取指定会员的处罚记录
 * @return 记录数, -1=失败
 */
int MemberPunishInfo_GetByMember(DBConnectionManager* manager, int m_id,
                                  MemberPunishInfo* punishes, int max_count) {
    char query[512];
    int i;
    snprintf(query, sizeof(query),
             "SELECT m_id, punish_type, occ_time, punish_value, apply_flag, start_time, end_time, "
             "admin_id, reason, IP, Mac FROM member_punish_info WHERE m_id=%d ORDER BY occ_time DESC",
             m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return (ret < 0) ? -1 : 0;
    }

    int count = 0;
    for (i = 0; i < max_count; i++) {
        char* values[11];
        if (DBQueryResult_FetchRow(&result, values) <= 0) {
            break;
        }

        MemberPunishInfo_Init(&punishes[count]);
        punishes[count].m_id = atoi(values[0]);
        punishes[count].punish_type = atoi(values[1]);
        strncpy(punishes[count].occ_time, values[2] ? values[2] : "", sizeof(punishes[count].occ_time) - 1);
        punishes[count].punish_value = atoi(values[3]);
        punishes[count].apply_flag = atoi(values[4]);
        strncpy(punishes[count].start_time, values[5] ? values[5] : "", sizeof(punishes[count].start_time) - 1);
        strncpy(punishes[count].end_time, values[6] ? values[6] : "", sizeof(punishes[count].end_time) - 1);
        strncpy(punishes[count].admin_id, values[7] ? values[7] : "", sizeof(punishes[count].admin_id) - 1);
        strncpy(punishes[count].reason, values[8] ? values[8] : "", sizeof(punishes[count].reason) - 1);
        strncpy(punishes[count].ip, values[9] ? values[9] : "", sizeof(punishes[count].ip) - 1);
        strncpy(punishes[count].mac, values[10] ? values[10] : "", sizeof(punishes[count].mac) - 1);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 更新处罚记录
 * @return 0=成功, -1=失败
 */
int MemberPunishInfo_Update(DBConnectionManager* manager, MemberPunishInfo* punish) {
    char query[2048];
    char escaped_admin_id[512];
    char escaped_reason[512];
    char escaped_ip[32];
    char escaped_mac[202];

    EscapeString(punish->admin_id, escaped_admin_id, sizeof(escaped_admin_id));
    EscapeString(punish->reason, escaped_reason, sizeof(escaped_reason));
    EscapeString(punish->ip, escaped_ip, sizeof(escaped_ip));
    EscapeString(punish->mac, escaped_mac, sizeof(escaped_mac));

    snprintf(query, sizeof(query),
             "UPDATE member_punish_info SET occ_time='%s', punish_value=%d, apply_flag=%d, "
             "start_time='%s', end_time='%s', admin_id='%s', reason='%s', IP='%s', Mac='%s' "
             "WHERE m_id=%d AND punish_type=%d",
             punish->occ_time, punish->punish_value, punish->apply_flag,
             punish->start_time, punish->end_time,
             escaped_admin_id, escaped_reason, escaped_ip, escaped_mac,
             punish->m_id, punish->punish_type);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    printf("[MemberPunishInfo ORM] Updated punish: m_id=%d, type=%d\n",
           punish->m_id, punish->punish_type);

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 删除处罚记录
 * @return 0=成功, -1=失败
 */
int MemberPunishInfo_Delete(DBConnectionManager* manager, int m_id, int punish_type) {
    char query[512];
    snprintf(query, sizeof(query),
             "DELETE FROM member_punish_info WHERE m_id=%d AND punish_type=%d",
             m_id, punish_type);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    DBQueryResult_Free(&result);

    if (ret < 0) {
        return -1;
    }

    printf("[MemberPunishInfo ORM] Deleted punish: m_id=%d, type=%d\n",
           m_id, punish_type);

    return 0;
}

/**
 * 删除指定会员的所有处罚记录
 * @return 0=成功, -1=失败
 */
int MemberPunishInfo_DeleteAll(DBConnectionManager* manager, int m_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM member_punish_info WHERE m_id=%d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    DBQueryResult_Free(&result);

    if (ret < 0) {
        return -1;
    }

    printf("[MemberPunishInfo ORM] Deleted all punishes for m_id=%d\n", m_id);

    return 0;
}

/* ========================================================================
 * 业务逻辑查询函数实现
 * ======================================================================== */

/**
 * 检查会员是否有指定类型的生效处罚
 * @return true=有生效处罚, false=无或已过期
 */
bool MemberPunishInfo_HasActivePunish(DBConnectionManager* manager, int m_id, int punish_type) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM member_punish_info "
             "WHERE m_id=%d AND punish_type=%d AND apply_flag=%d AND end_time > NOW()",
             m_id, punish_type, PUNISH_STATUS_ACTIVE);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
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

/**
 * 检查会员是否有任何生效处罚
 * @return true=有生效处罚, false=无或已过期
 */
bool MemberPunishInfo_HasAnyActivePunish(DBConnectionManager* manager, int m_id) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM member_punish_info "
             "WHERE m_id=%d AND apply_flag=%d AND end_time > NOW()",
             m_id, PUNISH_STATUS_ACTIVE);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
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

/**
 * 获取会员的生效处罚列表
 * @return 记录数, -1=失败
 */
int MemberPunishInfo_GetActive(DBConnectionManager* manager, int m_id,
                                MemberPunishInfo* punishes, int max_count) {
    char query[512];
    int i;
    snprintf(query, sizeof(query),
             "SELECT m_id, punish_type, occ_time, punish_value, apply_flag, start_time, end_time, "
             "admin_id, reason, IP, Mac FROM member_punish_info "
             "WHERE m_id=%d AND apply_flag=%d AND end_time > NOW() ORDER BY occ_time DESC",
             m_id, PUNISH_STATUS_ACTIVE);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return (ret < 0) ? -1 : 0;
    }

    int count = 0;
    for (i = 0; i < max_count; i++) {
        char* values[11];
        if (DBQueryResult_FetchRow(&result, values) <= 0) {
            break;
        }

        MemberPunishInfo_Init(&punishes[count]);
        punishes[count].m_id = atoi(values[0]);
        punishes[count].punish_type = atoi(values[1]);
        strncpy(punishes[count].occ_time, values[2] ? values[2] : "", sizeof(punishes[count].occ_time) - 1);
        punishes[count].punish_value = atoi(values[3]);
        punishes[count].apply_flag = atoi(values[4]);
        strncpy(punishes[count].start_time, values[5] ? values[5] : "", sizeof(punishes[count].start_time) - 1);
        strncpy(punishes[count].end_time, values[6] ? values[6] : "", sizeof(punishes[count].end_time) - 1);
        strncpy(punishes[count].admin_id, values[7] ? values[7] : "", sizeof(punishes[count].admin_id) - 1);
        strncpy(punishes[count].reason, values[8] ? values[8] : "", sizeof(punishes[count].reason) - 1);
        strncpy(punishes[count].ip, values[9] ? values[9] : "", sizeof(punishes[count].ip) - 1);
        strncpy(punishes[count].mac, values[10] ? values[10] : "", sizeof(punishes[count].mac) - 1);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 更新处罚状态
 * @return 0=成功, -1=失败
 */
int MemberPunishInfo_UpdateStatus(DBConnectionManager* manager, int m_id, int punish_type, int status) {
    char query[512];
    snprintf(query, sizeof(query),
             "UPDATE member_punish_info SET apply_flag=%d WHERE m_id=%d AND punish_type=%d",
             status, m_id, punish_type);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    printf("[MemberPunishInfo ORM] Updated status: m_id=%d, type=%d, status=%d\n",
           m_id, punish_type, status);

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 解除处罚（将状态设为LIFTED）
 * @return 0=成功, -1=失败
 */
int MemberPunishInfo_Lift(DBConnectionManager* manager, int m_id, int punish_type) {
    return MemberPunishInfo_UpdateStatus(manager, m_id, punish_type, PUNISH_STATUS_LIFTED);
}

/**
 * 获取会员处罚记录总数
 * @return 记录数, -1=失败
 */
int MemberPunishInfo_GetCount(DBConnectionManager* manager, int m_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM member_punish_info WHERE m_id=%d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
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

/**
 * 检查处罚是否已过期（end_time < 当前时间）
 * @return true=已过期, false=未过期或失败
 */
bool MemberPunishInfo_IsExpired(DBConnectionManager* manager, int m_id, int punish_type) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM member_punish_info "
             "WHERE m_id=%d AND punish_type=%d AND end_time < NOW()",
             m_id, punish_type);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
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

/**
 * 按管理员ID查询处罚记录
 * @return 记录数, -1=失败
 */
int MemberPunishInfo_GetByAdmin(DBConnectionManager* manager, const char* admin_id,
                                 MemberPunishInfo* punishes, int max_count) {
    char query[768];
    char escaped_admin_id[512];
    int i;

    EscapeString(admin_id, escaped_admin_id, sizeof(escaped_admin_id));

    snprintf(query, sizeof(query),
             "SELECT m_id, punish_type, occ_time, punish_value, apply_flag, start_time, end_time, "
             "admin_id, reason, IP, Mac FROM member_punish_info "
             "WHERE admin_id='%s' ORDER BY occ_time DESC",
             escaped_admin_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return (ret < 0) ? -1 : 0;
    }

    int count = 0;
    for (i = 0; i < max_count; i++) {
        char* values[11];
        if (DBQueryResult_FetchRow(&result, values) <= 0) {
            break;
        }

        MemberPunishInfo_Init(&punishes[count]);
        punishes[count].m_id = atoi(values[0]);
        punishes[count].punish_type = atoi(values[1]);
        strncpy(punishes[count].occ_time, values[2] ? values[2] : "", sizeof(punishes[count].occ_time) - 1);
        punishes[count].punish_value = atoi(values[3]);
        punishes[count].apply_flag = atoi(values[4]);
        strncpy(punishes[count].start_time, values[5] ? values[5] : "", sizeof(punishes[count].start_time) - 1);
        strncpy(punishes[count].end_time, values[6] ? values[6] : "", sizeof(punishes[count].end_time) - 1);
        strncpy(punishes[count].admin_id, values[7] ? values[7] : "", sizeof(punishes[count].admin_id) - 1);
        strncpy(punishes[count].reason, values[8] ? values[8] : "", sizeof(punishes[count].reason) - 1);
        strncpy(punishes[count].ip, values[9] ? values[9] : "", sizeof(punishes[count].ip) - 1);
        strncpy(punishes[count].mac, values[10] ? values[10] : "", sizeof(punishes[count].mac) - 1);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ========================================================================
 * 批量操作实现
 * ======================================================================== */

/**
 * 批量添加处罚记录
 * @return 成功添加的记录数
 */
int MemberPunishInfo_AddBatch(DBConnectionManager* manager, MemberPunishInfo* punishes, int count) {
    int i;
    int success_count = 0;
    for (i = 0; i < count; i++) {
        if (MemberPunishInfo_Add(manager, &punishes[i]) == 0) {
            success_count++;
        }
    }
    return success_count;
}

/**
 * 批量更新过期处罚状态
 * 将所有 end_time < NOW() 且状态为ACTIVE的记录更新为EXPIRED
 * @return 更新的记录数, -1=失败
 */
int MemberPunishInfo_UpdateExpired(DBConnectionManager* manager) {
    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE member_punish_info SET apply_flag=%d "
             "WHERE apply_flag=%d AND end_time < NOW()",
             PUNISH_STATUS_EXPIRED, PUNISH_STATUS_ACTIVE);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    DBQueryResult_Free(&result);

    if (ret < 0) {
        return -1;
    }

    printf("[MemberPunishInfo ORM] Updated %d expired punishes\n", ret);

    return ret;
}

/* ========================================================================
 * 工具函数实现
 * ======================================================================== */

/**
 * 初始化处罚信息结构
 */
void MemberPunishInfo_Init(MemberPunishInfo* punish) {
    memset(punish, 0, sizeof(MemberPunishInfo));
}

/**
 * 打印处罚信息
 */
void MemberPunishInfo_Print(const MemberPunishInfo* punish) {
    printf("=== Member Punish Info ===\n");
    printf("会员ID (m_id): %d\n", punish->m_id);
    printf("处罚类型 (punish_type): %d [%s]\n", punish->punish_type,
           MemberPunishInfo_GetTypeName(punish->punish_type));
    printf("发生时间 (occ_time): %s\n", punish->occ_time);
    printf("处罚值 (punish_value): %d\n", punish->punish_value);
    printf("状态标记 (apply_flag): %d [%s]\n", punish->apply_flag,
           MemberPunishInfo_GetStatusName(punish->apply_flag));
    printf("开始时间 (start_time): %s\n", punish->start_time);
    printf("结束时间 (end_time): %s\n", punish->end_time);
    printf("管理员ID (admin_id): %s\n", punish->admin_id);
    printf("处罚原因 (reason): %s\n", punish->reason);
    printf("IP地址 (ip): %s\n", punish->ip);
    printf("MAC地址 (mac): %s\n", punish->mac);
    printf("==========================\n");
}

/**
 * 获取当前时间字符串 (YYYY-MM-DD HH:MM:SS)
 */
void MemberPunishInfo_GetTimeString(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

/**
 * 计算过期时间 (当前时间 + duration_seconds)
 */
void MemberPunishInfo_GetExpireTime(char* buffer, size_t size, int duration_seconds) {
    time_t now = time(NULL);
    time_t expire = now + duration_seconds;
    struct tm* tm_info = localtime(&expire);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

/**
 * 解析处罚类型名称
 */
const char* MemberPunishInfo_GetTypeName(int punish_type) {
    switch (punish_type) {
        case PUNISH_TYPE_CHAT_BAN: return "禁言";
        case PUNISH_TYPE_TRADE_BAN: return "禁止交易";
        case PUNISH_TYPE_ACCOUNT_BAN: return "封号";
        case PUNISH_TYPE_IP_BAN: return "IP封禁";
        case PUNISH_TYPE_MAC_BAN: return "MAC封禁";
        case PUNISH_TYPE_TEMP_BAN: return "临时封禁";
        case PUNISH_TYPE_PERM_BAN: return "永久封禁";
        case PUNISH_TYPE_WARNING: return "警告";
        default: return "未知类型";
    }
}

/**
 * 解析处罚状态名称
 */
const char* MemberPunishInfo_GetStatusName(int apply_flag) {
    switch (apply_flag) {
        case PUNISH_STATUS_PENDING: return "待生效";
        case PUNISH_STATUS_ACTIVE: return "生效中";
        case PUNISH_STATUS_EXPIRED: return "已过期";
        case PUNISH_STATUS_LIFTED: return "已解除";
        default: return "未知状态";
    }
}
