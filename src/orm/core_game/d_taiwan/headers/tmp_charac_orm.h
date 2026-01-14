#ifndef TMP_CHARAC_ORM_H
#define TMP_CHARAC_ORM_H

#include "db_connection_manager.h"

/* 临时角色数据结构 */
typedef struct {
    unsigned int m_id;                  /* 会员ID */
    unsigned int charac_no;             /* 角色编号 */
    char charac_name[11];               /* 角色名称 (最大10字符+\0) */
    unsigned char village;              /* 村庄/城镇 */
    unsigned char job;                  /* 职业 */
    unsigned char lev;                  /* 等级 */
    unsigned int exp;                   /* 经验值 */
    unsigned char grow_type;            /* 成长类型 */
    unsigned char HP;                   /* 当前HP */
    unsigned short maxHP;               /* 最大HP */
    unsigned short maxMP;               /* 最大MP */
    unsigned short phy_attack;          /* 物理攻击 */
    unsigned short phy_defense;         /* 物理防御 */
    unsigned short mag_attack;          /* 魔法攻击 */
    unsigned short mag_defense;         /* 魔法防御 */
    char element_resist[256];           /* 元素抗性 (blob数据) */
    char spec_property[256];            /* 特殊属性 (blob数据) */
    unsigned int inven_weight;          /* 背包负重 */
    short hp_regen;                     /* HP恢复速度 */
    short mp_regen;                     /* MP恢复速度 */
    unsigned short move_speed;          /* 移动速度 */
    unsigned short attack_speed;        /* 攻击速度 */
    unsigned short cast_speed;          /* 施法速度 */
    short hit_recovery;                 /* 受击恢复 */
    short jump;                         /* 跳跃力 */
    unsigned int charac_weight;         /* 角色负重 */
    short fatigue;                      /* 当前疲劳度 */
    short max_fatigue;                  /* 最大疲劳度 */
    short premium_fatigue;              /* 高级疲劳度 */
    short max_premium_fatigue;          /* 最大高级疲劳度 */
    char create_time[20];               /* 创建时间 */
    char last_play_time[20];            /* 最后游戏时间 */
    unsigned int dungeon_clear_point;   /* 副本通关点数 */
    char delete_time[20];               /* 删除时间 */
    unsigned char delete_flag;          /* 删除标记 */
    unsigned int guild_id;              /* 公会ID */
    unsigned char guild_right;          /* 公会权限 */
    unsigned char member_flag;          /* 成员标记 */
} TmpCharac;

/* CRUD 操作 */
int TmpCharac_Add(DBConnectionManager* manager, const TmpCharac* record);
int TmpCharac_Get(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no, TmpCharac* record);
int TmpCharac_Update(DBConnectionManager* manager, const TmpCharac* record);
int TmpCharac_Delete(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no);
int TmpCharac_Exists(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no);

/* 专项操作 */
int TmpCharac_UpdateLevel(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no,
                          unsigned char lev, unsigned int exp);
int TmpCharac_UpdateFatigue(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no,
                            short fatigue, short premium_fatigue);
int TmpCharac_UpdateGuild(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no,
                          unsigned int guild_id, unsigned char guild_right);
int TmpCharac_MarkDeleted(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no);

/* 查询操作 */
int TmpCharac_Count(DBConnectionManager* manager);
int TmpCharac_GetByMid(DBConnectionManager* manager, unsigned int m_id, TmpCharac** records, int* count);
int TmpCharac_GetByName(DBConnectionManager* manager, const char* charac_name, TmpCharac** records, int* count);
int TmpCharac_GetDeleted(DBConnectionManager* manager, TmpCharac** records, int* count);

/* 批量操作 */
int TmpCharac_Clear(DBConnectionManager* manager);
int TmpCharac_DeleteByMid(DBConnectionManager* manager, unsigned int m_id);

/* 工具函数 */
void TmpCharac_PrintInfo(const TmpCharac* record);

#endif /* TMP_CHARAC_ORM_H */
