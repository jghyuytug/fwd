#ifndef CHARAC_VIEW_ORM_H
#define CHARAC_VIEW_ORM_H

#include "db_connection_manager.h"

/**
 * 角色视图配置 ORM
 *
 * 表: charac_view
 * 主键: m_id
 * 功能: 会员角色槽位配置、角色数量管理、效果计数
 */

/* 数据结构 */
typedef struct {
    unsigned int m_id;                  /* 会员ID (主键) */
    unsigned char* info;                /* 信息数据(二进制) */
    int info_len;                       /* info数据长度 */
    unsigned char slot_effect_count;    /* 插槽效果数量 */
    unsigned char charac_slot_limit;    /* 角色槽位限制 */
    char hash_key[33];                  /* 哈希密钥 */
    unsigned char charac_count;         /* 角色数量 */
} CharacView;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新角色视图配置
 */
int CharacView_Add(DBConnectionManager* manager, const CharacView* record);

/**
 * 获取角色视图配置
 */
int CharacView_Get(DBConnectionManager* manager, unsigned int m_id, CharacView* record);

/**
 * 更新角色视图配置
 */
int CharacView_Update(DBConnectionManager* manager, const CharacView* record);

/**
 * 删除角色视图配置
 */
int CharacView_Delete(DBConnectionManager* manager, unsigned int m_id);

/**
 * 检查角色视图配置是否存在
 */
int CharacView_Exists(DBConnectionManager* manager, unsigned int m_id);

/* ==================== 专项操作 ==================== */

/**
 * 更新插槽效果数量
 */
int CharacView_UpdateSlotEffectCount(DBConnectionManager* manager, unsigned int m_id, unsigned char count);

/**
 * 更新角色槽位限制
 */
int CharacView_UpdateCharacSlotLimit(DBConnectionManager* manager, unsigned int m_id, unsigned char limit);

/**
 * 更新哈希密钥
 */
int CharacView_UpdateHashKey(DBConnectionManager* manager, unsigned int m_id, const char* hash_key);

/**
 * 更新角色数量
 */
int CharacView_UpdateCharacCount(DBConnectionManager* manager, unsigned int m_id, unsigned char count);

/**
 * 增加角色数量
 */
int CharacView_IncrementCharacCount(DBConnectionManager* manager, unsigned int m_id);

/**
 * 减少角色数量
 */
int CharacView_DecrementCharacCount(DBConnectionManager* manager, unsigned int m_id);

/* ==================== 查询操作 ==================== */

/**
 * 获取角色数量大于指定值的会员
 */
int CharacView_GetByMinCharacCount(DBConnectionManager* manager, unsigned char min_count,
                                   CharacView* records, int max_result);

/**
 * 获取角色数量等于指定值的会员
 */
int CharacView_GetByCharacCount(DBConnectionManager* manager, unsigned char count,
                                CharacView* records, int max_result);

/**
 * 获取槽位限制大于指定值的会员
 */
int CharacView_GetByMinSlotLimit(DBConnectionManager* manager, unsigned char min_limit,
                                 CharacView* records, int max_result);

/* ==================== 工具函数 ==================== */

/**
 * 打印角色视图配置信息
 */
void CharacView_PrintInfo(const CharacView* record);

/**
 * 释放CharacView结构中的动态内存
 */
void CharacView_Free(CharacView* record);

#endif /* CHARAC_VIEW_ORM_H */
