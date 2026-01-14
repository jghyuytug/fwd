#ifndef LIMIT_CREATE_CHARACTER_IP_ORM_H
#define LIMIT_CREATE_CHARACTER_IP_ORM_H

#include "db_connection_manager.h"

/* IP创建角色限制数据结构 */
typedef struct {
    unsigned int ip;                /* IP地址数值形式 (主键) */
    char ip_str[17];                /* IP地址字符串形式 */
    char last_access_time[32];      /* 最后访问时间 */
    unsigned int count;             /* 创建角色次数 */
    unsigned int last_access_mid;   /* 最后访问的会员ID */
} LimitCreateCharacterIP;

/* CRUD 操作 */
int LimitCreateCharacterIP_Add(DBConnectionManager* manager, const LimitCreateCharacterIP* record);
int LimitCreateCharacterIP_Get(DBConnectionManager* manager, unsigned int ip, LimitCreateCharacterIP* record);
int LimitCreateCharacterIP_GetByIPStr(DBConnectionManager* manager, const char* ip_str, LimitCreateCharacterIP* record);
int LimitCreateCharacterIP_Update(DBConnectionManager* manager, const LimitCreateCharacterIP* record);
int LimitCreateCharacterIP_Delete(DBConnectionManager* manager, unsigned int ip);
int LimitCreateCharacterIP_Exists(DBConnectionManager* manager, unsigned int ip);

/* 专项操作 */
int LimitCreateCharacterIP_IncrementCount(DBConnectionManager* manager, unsigned int ip, unsigned int m_id);
int LimitCreateCharacterIP_ResetCount(DBConnectionManager* manager, unsigned int ip);
int LimitCreateCharacterIP_UpdateAccessTime(DBConnectionManager* manager, unsigned int ip);
int LimitCreateCharacterIP_UpdateAccessMid(DBConnectionManager* manager, unsigned int ip, unsigned int m_id);

/* 查询操作 */
int LimitCreateCharacterIP_Count(DBConnectionManager* manager);
int LimitCreateCharacterIP_GetAll(DBConnectionManager* manager, LimitCreateCharacterIP** records, int* count);
int LimitCreateCharacterIP_GetByCountRange(DBConnectionManager* manager, unsigned int min_count, unsigned int max_count,
                                           LimitCreateCharacterIP** records, int* count);
int LimitCreateCharacterIP_GetRecentAccess(DBConnectionManager* manager, int days,
                                           LimitCreateCharacterIP** records, int* count);
int LimitCreateCharacterIP_GetByMid(DBConnectionManager* manager, unsigned int m_id,
                                    LimitCreateCharacterIP** records, int* count);

/* 批量操作 */
int LimitCreateCharacterIP_Clear(DBConnectionManager* manager);
int LimitCreateCharacterIP_BatchDelete(DBConnectionManager* manager, const unsigned int* ips, int count);
int LimitCreateCharacterIP_BatchResetCount(DBConnectionManager* manager, const unsigned int* ips, int count);

/* 工具函数 */
void LimitCreateCharacterIP_PrintInfo(const LimitCreateCharacterIP* record);
unsigned int LimitCreateCharacterIP_StrToInt(const char* ip_str);
void LimitCreateCharacterIP_IntToStr(unsigned int ip, char* ip_str, size_t len);

#endif /* LIMIT_CREATE_CHARACTER_IP_ORM_H */
