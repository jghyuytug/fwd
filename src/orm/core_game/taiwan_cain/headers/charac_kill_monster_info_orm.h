#ifndef CHARAC_KILL_MONSTER_INFO_ORM_H
#define CHARAC_KILL_MONSTER_INFO_ORM_H

#include "db_connection_manager.h"

/**
 * 角色击杀怪物信息表 ORM
 *
 * 表: charac_kill_monster_info
 * 主键: charac_no
 * 功能: 记录角色击杀各类怪物的信息（boss、named怪物、APC boss）
 */

#define MAX_BLOB_SIZE 65535

/* 数据结构 */
typedef struct {
    unsigned int charac_no;          /* 角色编号 (主键) */
    unsigned char* boss_info;        /* Boss击杀信息 (blob) */
    unsigned int boss_info_len;      /* Boss信息长度 */
    unsigned char* named_info;       /* Named怪物击杀信息 (blob) */
    unsigned int named_info_len;     /* Named信息长度 */
    unsigned char* apc_boss_info;    /* APC Boss击杀信息 (blob) */
    unsigned int apc_boss_info_len;  /* APC Boss信息长度 */
} CharacKillMonsterInfo;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新角色击杀怪物信息
 */
int CharacKillMonsterInfo_Add(DBConnectionManager* manager, const CharacKillMonsterInfo* record);

/**
 * 获取角色击杀怪物信息
 */
int CharacKillMonsterInfo_Get(DBConnectionManager* manager, unsigned int charac_no, CharacKillMonsterInfo* record);

/**
 * 更新角色击杀怪物信息
 */
int CharacKillMonsterInfo_Update(DBConnectionManager* manager, const CharacKillMonsterInfo* record);

/**
 * 删除角色击杀怪物信息
 */
int CharacKillMonsterInfo_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查角色击杀怪物信息是否存在
 */
int CharacKillMonsterInfo_Exists(DBConnectionManager* manager, unsigned int charac_no);

/* ==================== 专项更新操作 ==================== */

/**
 * 单独更新Boss击杀信息
 */
int CharacKillMonsterInfo_UpdateBossInfo(DBConnectionManager* manager, unsigned int charac_no,
                                          const unsigned char* boss_info, unsigned int boss_info_len);

/**
 * 单独更新Named怪物击杀信息
 */
int CharacKillMonsterInfo_UpdateNamedInfo(DBConnectionManager* manager, unsigned int charac_no,
                                           const unsigned char* named_info, unsigned int named_info_len);

/**
 * 单独更新APC Boss击杀信息
 */
int CharacKillMonsterInfo_UpdateApcBossInfo(DBConnectionManager* manager, unsigned int charac_no,
                                             const unsigned char* apc_boss_info, unsigned int apc_boss_info_len);

/* ==================== 工具函数 ==================== */

/**
 * 释放CharacKillMonsterInfo结构中的动态内存
 */
void CharacKillMonsterInfo_Free(CharacKillMonsterInfo* record);

/**
 * 打印角色击杀怪物信息（仅显示长度，不显示二进制内容）
 */
void CharacKillMonsterInfo_PrintInfo(const CharacKillMonsterInfo* record);

#endif /* CHARAC_KILL_MONSTER_INFO_ORM_H */
