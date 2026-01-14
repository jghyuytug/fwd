#ifndef DNF_STORY_ORM_H
#define DNF_STORY_ORM_H

#include "db_connection_manager.h"

/**
 * dnf_story - DNF故事/公告系统表
 *
 * 表结构：
 * - no (int, auto_increment): 自增主键
 * - story_type (tinyint): 故事类型
 * - notice_flag (tinyint): 公告标志
 * - m_id (int): 会员ID (索引)
 * - reg_id (varchar 12): 注册ID
 * - title (varchar 50): 标题
 * - url (varchar 250): URL链接
 * - img_name (varchar 30): 图片名称
 * - opt (tinyint unsigned): 选项
 * - open_flag (enum 'y'/'n'): 开放标志
 * - reg_date (datetime): 注册日期 (索引)
 * - hits (int unsigned): 点击数
 * - reserve_time (int unsigned): 保留时间
 * - content (text): 内容
 *
 * 索引：
 * - idx_mid (m_id)
 * - idx_reg (reg_date)
 *
 * 业务说明：
 * - 存储DNF游戏故事、公告、活动等内容
 * - 支持点击统计和时间管理
 * - 支持开放/关闭控制
 */

/* 数据结构定义 */
typedef struct {
    int no;                     /* 自增主键 */
    int story_type;             /* 故事类型 */
    int notice_flag;            /* 公告标志 */
    int m_id;                   /* 会员ID */
    char reg_id[13];            /* 注册ID (12+1) */
    char title[51];             /* 标题 (50+1) */
    char url[251];              /* URL链接 (250+1) */
    char img_name[31];          /* 图片名称 (30+1) */
    unsigned char opt;          /* 选项 */
    char open_flag;             /* 开放标志 ('y' or 'n') */
    char reg_date[20];          /* 注册日期 (YYYY-MM-DD HH:MM:SS) */
    unsigned int hits;          /* 点击数 */
    unsigned int reserve_time;  /* 保留时间 */
    char* content;              /* 内容 (动态分配) */
} DnfStory;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加故事/公告记录
 * @param manager 数据库连接管理器
 * @param record 故事记录
 * @return 新插入的no值（成功）, <0（失败）
 */
int DnfStory_Add(DBConnectionManager* manager, const DnfStory* record);

/**
 * 根据no查询故事记录
 * @param manager 数据库连接管理器
 * @param no 记录编号
 * @param record 输出记录（调用者提供）
 * @return 0=成功, <0=失败
 */
int DnfStory_Get(DBConnectionManager* manager, int no, DnfStory* record);

/**
 * 更新故事记录
 * @param manager 数据库连接管理器
 * @param record 故事记录
 * @return 0=成功, <0=失败
 */
int DnfStory_Update(DBConnectionManager* manager, const DnfStory* record);

/**
 * 删除故事记录
 * @param manager 数据库连接管理器
 * @param no 记录编号
 * @return 0=成功, <0=失败
 */
int DnfStory_Delete(DBConnectionManager* manager, int no);

/**
 * 检查故事记录是否存在
 * @param manager 数据库连接管理器
 * @param no 记录编号
 * @return 1=存在, 0=不存在, <0=错误
 */
int DnfStory_Exists(DBConnectionManager* manager, int no);

/* ==================== 专项操作 ==================== */

/**
 * 查询会员的所有故事记录
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param records 输出记录数组（调用者分配）
 * @param max_count 数组最大容量
 * @return 实际返回的记录数, <0=失败
 */
int DnfStory_GetByMid(DBConnectionManager* manager, int m_id, DnfStory* records, int max_count);

/**
 * 按故事类型查询
 * @param manager 数据库连接管理器
 * @param story_type 故事类型
 * @param records 输出记录数组（调用者分配）
 * @param max_count 数组最大容量
 * @return 实际返回的记录数, <0=失败
 */
int DnfStory_GetByStoryType(DBConnectionManager* manager, int story_type, DnfStory* records, int max_count);

/**
 * 更新点击数
 * @param manager 数据库连接管理器
 * @param no 记录编号
 * @param hits 新的点击数
 * @return 0=成功, <0=失败
 */
int DnfStory_UpdateHits(DBConnectionManager* manager, int no, unsigned int hits);

/**
 * 增加点击数
 * @param manager 数据库连接管理器
 * @param no 记录编号
 * @return 0=成功, <0=失败
 */
int DnfStory_IncrementHits(DBConnectionManager* manager, int no);

/**
 * 更新开放标志
 * @param manager 数据库连接管理器
 * @param no 记录编号
 * @param open_flag 新的开放标志 ('y' or 'n')
 * @return 0=成功, <0=失败
 */
int DnfStory_UpdateOpenFlag(DBConnectionManager* manager, int no, char open_flag);

/* ==================== 查询操作 ==================== */

/**
 * 统计会员的故事数量
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 记录数量, <0=错误
 */
int DnfStory_CountByMid(DBConnectionManager* manager, int m_id);

/**
 * 按故事类型统计数量
 * @param manager 数据库连接管理器
 * @param story_type 故事类型
 * @return 记录数量, <0=错误
 */
int DnfStory_CountByStoryType(DBConnectionManager* manager, int story_type);

/**
 * 统计开放的故事数量
 * @param manager 数据库连接管理器
 * @return 记录数量, <0=错误
 */
int DnfStory_CountOpen(DBConnectionManager* manager);

/**
 * 统计总故事记录数
 * @param manager 数据库连接管理器
 * @return 总记录数, <0=错误
 */
int DnfStory_CountTotal(DBConnectionManager* manager);

/* ==================== 批量操作 ==================== */

/**
 * 批量删除故事记录（按no列表）
 * @param manager 数据库连接管理器
 * @param nos no数组
 * @param count 数组长度
 * @return 0=成功, <0=失败
 */
int DnfStory_BatchDelete(DBConnectionManager* manager, const int* nos, int count);

/* ==================== 工具函数 ==================== */

/**
 * 打印故事记录信息（调试用）
 * @param record 故事记录
 */
void DnfStory_PrintInfo(const DnfStory* record);

/**
 * 释放content内存
 * @param record 故事记录
 */
void DnfStory_FreeContent(DnfStory* record);

#endif /* DNF_STORY_ORM_H */
