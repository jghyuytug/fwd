#ifndef LIMIT_CREATE_CHARACTER_ORM_H
#define LIMIT_CREATE_CHARACTER_ORM_H

#include "db_connection_manager.h"

/* 创建角色限制数据结构 */
typedef struct {
    unsigned int m_id;              /* 会员ID (主键) */
    unsigned int count;             /* 创建角色次数 */
    char last_access_time[32];      /* 最后访问时间 */
} LimitCreateCharacter;

/* CRUD 操作 */
int LimitCreateCharacter_Add(DBConnectionManager* manager, const LimitCreateCharacter* record);
int LimitCreateCharacter_Get(DBConnectionManager* manager, unsigned int m_id, LimitCreateCharacter* record);
int LimitCreateCharacter_Update(DBConnectionManager* manager, const LimitCreateCharacter* record);
int LimitCreateCharacter_Delete(DBConnectionManager* manager, unsigned int m_id);
int LimitCreateCharacter_Exists(DBConnectionManager* manager, unsigned int m_id);

/* 专项操作 */
int LimitCreateCharacter_IncrementCount(DBConnectionManager* manager, unsigned int m_id);
int LimitCreateCharacter_ResetCount(DBConnectionManager* manager, unsigned int m_id);
int LimitCreateCharacter_UpdateAccessTime(DBConnectionManager* manager, unsigned int m_id);

/* 查询操作 */
int LimitCreateCharacter_Count(DBConnectionManager* manager);
int LimitCreateCharacter_GetAll(DBConnectionManager* manager, LimitCreateCharacter** records, int* count);
int LimitCreateCharacter_GetByCountRange(DBConnectionManager* manager, unsigned int min_count, unsigned int max_count,
                                         LimitCreateCharacter** records, int* count);
int LimitCreateCharacter_GetRecentAccess(DBConnectionManager* manager, int days,
                                         LimitCreateCharacter** records, int* count);

/* 批量操作 */
int LimitCreateCharacter_Clear(DBConnectionManager* manager);
int LimitCreateCharacter_BatchDelete(DBConnectionManager* manager, const unsigned int* m_ids, int count);
int LimitCreateCharacter_BatchResetCount(DBConnectionManager* manager, const unsigned int* m_ids, int count);

/* 工具函数 */
void LimitCreateCharacter_PrintInfo(const LimitCreateCharacter* record);

#endif /* LIMIT_CREATE_CHARACTER_ORM_H */
