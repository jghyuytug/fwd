#ifndef CHARAC_QUEST_SHOP_ORM_H
#define CHARAC_QUEST_SHOP_ORM_H

#include "db_connection_manager.h"

/**
 * 角色任务商店 ORM
 *
 * 表: charac_quest_shop
 * 主键: charac_no
 * 功能: 任务商店系统，使用任务点数（QP）购买角色属性提升
 */

/* 数据结构 */
typedef struct {
    unsigned int charac_no;              /* 角色编号 (主键) */
    unsigned int qp;                     /* 任务点数 */

    /* 基础属性 */
    unsigned short max_hp;               /* 最大HP */
    unsigned short max_mp;               /* 最大MP */

    /* 物理/魔法战斗属性 */
    unsigned short psy_attack;           /* 物理攻击 */
    unsigned short psy_defense;          /* 物理防御 */
    unsigned short mag_attack;           /* 魔法攻击 */
    unsigned short mag_defence;          /* 魔法防御 */

    /* 速度属性 */
    unsigned short move_speed;           /* 移动速度 */
    unsigned short attack_speed;         /* 攻击速度 */

    /* 恢复属性 */
    unsigned short hp_regen;             /* HP恢复 */
    unsigned short mp_regen;             /* MP恢复 */

    /* 元素抗性 */
    unsigned short all_element_resist;   /* 全元素抗性 */
    unsigned short fire_element_resist;  /* 火元素抗性 */
    unsigned short water_element_resist; /* 水元素抗性 */
    unsigned short light_element_resist; /* 光元素抗性 */
    unsigned short dark_element_resist;  /* 暗元素抗性 */

    /* 元素攻击 */
    unsigned short all_element_attack;   /* 全元素攻击 */
    unsigned short fire_element_attack;  /* 火元素攻击 */
    unsigned short water_element_attack; /* 水元素攻击 */
    unsigned short light_element_attack; /* 光元素攻击 */
    unsigned short dark_element_attack;  /* 暗元素攻击 */

    /* 暴击属性 */
    unsigned short psy_critical;         /* 物理暴击 */
    unsigned short mag_critical;         /* 魔法暴击 */

    /* 命中/闪避属性 */
    unsigned short good_hit;             /* 命中率 */
    unsigned short evasion;              /* 闪避率 */
    unsigned short hit_recovery;         /* 受击恢复 */

    /* 特殊属性 */
    unsigned short init_count;           /* 初始化计数 */
    unsigned short separate_psy_mag_attack; /* 物理魔法分离攻击 */
    unsigned short quest_piece;          /* 任务碎片 */
} CharacQuestShop;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新角色任务商店数据
 */
int CharacQuestShop_Add(DBConnectionManager* manager, const CharacQuestShop* record);

/**
 * 获取角色任务商店数据
 */
int CharacQuestShop_Get(DBConnectionManager* manager, unsigned int charac_no, CharacQuestShop* record);

/**
 * 更新角色任务商店数据
 */
int CharacQuestShop_Update(DBConnectionManager* manager, const CharacQuestShop* record);

/**
 * 删除角色任务商店数据
 */
int CharacQuestShop_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查角色任务商店数据是否存在
 */
int CharacQuestShop_Exists(DBConnectionManager* manager, unsigned int charac_no);

/* ==================== 专项操作 ==================== */

/**
 * 增加任务点数
 */
int CharacQuestShop_AddQuestPoints(DBConnectionManager* manager, unsigned int charac_no, unsigned int qp_amount);

/**
 * 消费任务点数（减少QP）
 * @return 0 成功, -1 失败（QP不足或其他错误）
 */
int CharacQuestShop_SpendQuestPoints(DBConnectionManager* manager, unsigned int charac_no, unsigned int qp_cost);

/**
 * 更新单个属性值（通用函数）
 * @param stat_name 属性名称（如 "max_hp", "psy_attack" 等）
 * @param value 新值
 */
int CharacQuestShop_UpdateStat(DBConnectionManager* manager, unsigned int charac_no,
                                const char* stat_name, unsigned short value);

/**
 * 增加单个属性值（通用函数）
 * @param stat_name 属性名称
 * @param increment 增加量
 */
int CharacQuestShop_IncrementStat(DBConnectionManager* manager, unsigned int charac_no,
                                   const char* stat_name, unsigned short increment);

/**
 * 重置所有属性为0（保留QP）
 */
int CharacQuestShop_ResetAllStats(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 批量更新元素抗性
 */
int CharacQuestShop_UpdateElementResists(DBConnectionManager* manager, unsigned int charac_no,
                                          unsigned short all, unsigned short fire,
                                          unsigned short water, unsigned short light, unsigned short dark);

/**
 * 批量更新元素攻击
 */
int CharacQuestShop_UpdateElementAttacks(DBConnectionManager* manager, unsigned int charac_no,
                                          unsigned short all, unsigned short fire,
                                          unsigned short water, unsigned short light, unsigned short dark);

/* ==================== 查询操作 ==================== */

/**
 * 获取总记录数
 */
int CharacQuestShop_GetTotalCount(DBConnectionManager* manager);

/**
 * 获取指定QP范围的角色列表
 */
int CharacQuestShop_GetByQPRange(DBConnectionManager* manager, unsigned int min_qp, unsigned int max_qp,
                                  unsigned int* charac_nos, int max_result);

/**
 * 获取QP排名前N的角色
 */
int CharacQuestShop_GetTopByQP(DBConnectionManager* manager, int limit, unsigned int* charac_nos);

/**
 * 获取所有角色的总任务点数
 */
long long CharacQuestShop_GetTotalQP(DBConnectionManager* manager);

/* ==================== 工具函数 ==================== */

/**
 * 打印角色任务商店信息
 */
void CharacQuestShop_PrintInfo(const CharacQuestShop* record);

#endif /* CHARAC_QUEST_SHOP_ORM_H */
