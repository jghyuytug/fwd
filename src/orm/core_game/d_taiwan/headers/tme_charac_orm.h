/**
 * DNF游戏服务器 - TmeCharac表ORM层
 *
 * 功能: TME角色信息管理系统
 * 数据库: d_taiwan
 * 表: tme_charac
 */

#ifndef TME_CHARAC_ORM_H
#define TME_CHARAC_ORM_H

#include "db_connection_manager.h"
#include <stdbool.h>

// ========================================================================
// TME角色结构定义
// ========================================================================

/**
 * TME角色信息结构
 */
typedef struct {
    int m_id;                       // 会员ID
    int charac_no;                  // 角色编号
    char charac_name[31];           // 角色名称 (10字符*3字节UTF8 + \0)
    signed char village;            // 村庄/阵营 (默认1)
    signed char job;                // 职业 (默认0)
    signed char lev;                // 等级 (默认1)
    int exp;                        // 经验值
    signed char grow_type;          // 成长类型

    // 属性数据
    unsigned char HP;               // 当前HP
    unsigned short maxHP;           // 最大HP
    unsigned short maxMP;           // 最大MP
    unsigned short phy_attack;      // 物理攻击
    unsigned short phy_defense;     // 物理防御
    unsigned short mag_attack;      // 魔法攻击
    unsigned short mag_defense;     // 魔法防御

    // 二进制数据
    unsigned char* element_resist;  // 元素抗性 (tinyblob)
    size_t element_resist_len;      // 元素抗性数据长度
    unsigned char* spec_property;   // 特殊属性 (tinyblob)
    size_t spec_property_len;       // 特殊属性数据长度

    // 背包和移动属性
    int inven_weight;               // 背包负重
    short hp_regen;                 // HP恢复速度
    short mp_regen;                 // MP恢复速度
    unsigned short move_speed;      // 移动速度
    unsigned short attack_speed;    // 攻击速度
    unsigned short cast_speed;      // 施法速度
    short hit_recovery;             // 受击恢复
    short jump;                     // 跳跃力
    int charac_weight;              // 角色负重

    // 疲劳值系统
    short fatigue;                  // 当前疲劳值
    short max_fatigue;              // 最大疲劳值 (默认70)
    short premium_fatigue;          // 高级疲劳值
    short max_premium_fatigue;      // 最大高级疲劳值

    // 时间和状态
    char create_time[20];           // 创建时间 (YYYY-MM-DD HH:MM:SS)
    char last_play_time[20];        // 最后游戏时间
    int dungeon_clear_point;        // 副本清除点数
    char delete_time[20];           // 删除时间
    signed char delete_flag;        // 删除标记 (0=正常, 1=已删除)

    // 公会信息
    unsigned int guild_id;          // 公会ID
    signed char guild_right;        // 公会权限
    signed char member_flag;        // 成员标记
} TmeCharac;

// ========================================================================
// CRUD操作函数
// ========================================================================

/**
 * 添加TME角色记录
 *
 * @param manager 数据库连接管理器
 * @param charac 角色信息结构
 * @return 0=成功, -1=失败
 */
int TmeCharac_Add(DBConnectionManager* manager, const TmeCharac* charac);

/**
 * 根据m_id和charac_no获取TME角色信息
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param charac_no 角色编号
 * @param charac 输出参数，角色信息
 * @return 0=成功, -1=失败
 */
int TmeCharac_Get(DBConnectionManager* manager, int m_id, int charac_no, TmeCharac* charac);

/**
 * 更新TME角色信息
 *
 * @param manager 数据库连接管理器
 * @param charac 角色信息结构
 * @return 0=成功, -1=失败
 */
int TmeCharac_Update(DBConnectionManager* manager, const TmeCharac* charac);

/**
 * 删除TME角色记录
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param charac_no 角色编号
 * @return 0=成功, -1=失败
 */
int TmeCharac_Delete(DBConnectionManager* manager, int m_id, int charac_no);

/**
 * 检查TME角色是否存在
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param charac_no 角色编号
 * @return true=存在, false=不存在
 */
bool TmeCharac_Exists(DBConnectionManager* manager, int m_id, int charac_no);

// ========================================================================
// 专项操作函数
// ========================================================================

/**
 * 更新角色等级和经验值
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param charac_no 角色编号
 * @param lev 新等级
 * @param exp 新经验值
 * @return 0=成功, -1=失败
 */
int TmeCharac_UpdateLevel(DBConnectionManager* manager, int m_id, int charac_no,
                          signed char lev, int exp);

/**
 * 更新角色HP/MP
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param charac_no 角色编号
 * @param HP 当前HP
 * @param maxHP 最大HP
 * @param maxMP 最大MP
 * @return 0=成功, -1=失败
 */
int TmeCharac_UpdateHP(DBConnectionManager* manager, int m_id, int charac_no,
                       unsigned char HP, unsigned short maxHP, unsigned short maxMP);

/**
 * 更新疲劳值
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param charac_no 角色编号
 * @param fatigue 当前疲劳值
 * @return 0=成功, -1=失败
 */
int TmeCharac_UpdateFatigue(DBConnectionManager* manager, int m_id, int charac_no,
                            short fatigue);

/**
 * 更新最后游戏时间
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param charac_no 角色编号
 * @return 0=成功, -1=失败
 */
int TmeCharac_UpdateLastPlayTime(DBConnectionManager* manager, int m_id, int charac_no);

/**
 * 标记角色为删除
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param charac_no 角色编号
 * @return 0=成功, -1=失败
 */
int TmeCharac_MarkDeleted(DBConnectionManager* manager, int m_id, int charac_no);

/**
 * 更新公会信息
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param charac_no 角色编号
 * @param guild_id 公会ID
 * @param guild_right 公会权限
 * @return 0=成功, -1=失败
 */
int TmeCharac_UpdateGuild(DBConnectionManager* manager, int m_id, int charac_no,
                          unsigned int guild_id, signed char guild_right);

// ========================================================================
// 查询操作函数
// ========================================================================

/**
 * 获取会员的所有角色
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param characs 输出数组
 * @param max_count 数组最大容量
 * @return 实际角色数量, -1=失败
 */
int TmeCharac_GetByMemberId(DBConnectionManager* manager, int m_id,
                            TmeCharac* characs, int max_count);

/**
 * 根据角色名查询
 *
 * @param manager 数据库连接管理器
 * @param charac_name 角色名称
 * @param charac 输出参数，角色信息
 * @return 0=成功, -1=失败
 */
int TmeCharac_GetByName(DBConnectionManager* manager, const char* charac_name,
                        TmeCharac* charac);

/**
 * 检查角色名是否已存在
 *
 * @param manager 数据库连接管理器
 * @param charac_name 角色名称
 * @return true=已存在, false=不存在
 */
bool TmeCharac_NameExists(DBConnectionManager* manager, const char* charac_name);

/**
 * 统计会员的角色数量
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 角色数量, -1=失败
 */
int TmeCharac_CountByMemberId(DBConnectionManager* manager, int m_id);

/**
 * 获取公会成员角色列表
 *
 * @param manager 数据库连接管理器
 * @param guild_id 公会ID
 * @param characs 输出数组
 * @param max_count 数组最大容量
 * @return 实际角色数量, -1=失败
 */
int TmeCharac_GetByGuildId(DBConnectionManager* manager, unsigned int guild_id,
                           TmeCharac* characs, int max_count);

// ========================================================================
// 工具函数
// ========================================================================

/**
 * 初始化TME角色结构（设置默认值）
 *
 * @param charac 角色结构指针
 */
void TmeCharac_Init(TmeCharac* charac);

/**
 * 释放TME角色结构中的动态内存
 *
 * @param charac 角色结构指针
 */
void TmeCharac_Free(TmeCharac* charac);

/**
 * 打印TME角色信息（用于调试）
 *
 * @param charac 角色结构指针
 */
void TmeCharac_PrintInfo(const TmeCharac* charac);

#endif // TME_CHARAC_ORM_H
