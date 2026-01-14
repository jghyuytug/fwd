#ifndef CHARAC_VIEW_ACT8_ORM_H
#define CHARAC_VIEW_ACT8_ORM_H

#include "db_connection_manager.h"

/**
 * 角色视图配置 Act8版本 ORM
 *
 * 表: charac_view_act8
 * 主键: m_id
 * 功能: Act8版本的会员角色槽位配置管理
 */

/* 数据结构 */
typedef struct {
    unsigned long long m_id;            /* 会员ID (主键, bigint) */
    unsigned char* info;                /* 信息数据(二进制) */
    int info_len;                       /* info数据长度 */
    unsigned char slot_effect_count;    /* 插槽效果数量 */
    unsigned char charac_slot_limit;    /* 角色槽位限制 */
    char hash_key[33];                  /* 哈希密钥 */
} CharacViewAct8;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新Act8角色视图配置
 */
int CharacViewAct8_Add(DBConnectionManager* manager, const CharacViewAct8* record);

/**
 * 获取Act8角色视图配置
 */
int CharacViewAct8_Get(DBConnectionManager* manager, unsigned long long m_id, CharacViewAct8* record);

/**
 * 更新Act8角色视图配置
 */
int CharacViewAct8_Update(DBConnectionManager* manager, const CharacViewAct8* record);

/**
 * 删除Act8角色视图配置
 */
int CharacViewAct8_Delete(DBConnectionManager* manager, unsigned long long m_id);

/**
 * 检查Act8角色视图配置是否存在
 */
int CharacViewAct8_Exists(DBConnectionManager* manager, unsigned long long m_id);

/* ==================== 专项操作 ==================== */

/**
 * 更新插槽效果数量
 */
int CharacViewAct8_UpdateSlotEffectCount(DBConnectionManager* manager, unsigned long long m_id, unsigned char count);

/**
 * 更新角色槽位限制
 */
int CharacViewAct8_UpdateCharacSlotLimit(DBConnectionManager* manager, unsigned long long m_id, unsigned char limit);

/**
 * 更新哈希密钥
 */
int CharacViewAct8_UpdateHashKey(DBConnectionManager* manager, unsigned long long m_id, const char* hash_key);

/* ==================== 查询操作 ==================== */

/**
 * 获取槽位限制大于指定值的会员
 */
int CharacViewAct8_GetByMinSlotLimit(DBConnectionManager* manager, unsigned char min_limit,
                                     CharacViewAct8* records, int max_result);

/**
 * 获取指定插槽效果数量的会员
 */
int CharacViewAct8_GetBySlotEffectCount(DBConnectionManager* manager, unsigned char count,
                                        CharacViewAct8* records, int max_result);

/* ==================== 工具函数 ==================== */

/**
 * 打印Act8角色视图配置信息
 */
void CharacViewAct8_PrintInfo(const CharacViewAct8* record);

/**
 * 释放CharacViewAct8结构中的动态内存
 */
void CharacViewAct8_Free(CharacViewAct8* record);

#endif /* CHARAC_VIEW_ACT8_ORM_H */
