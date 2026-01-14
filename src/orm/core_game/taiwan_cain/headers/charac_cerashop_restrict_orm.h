#ifndef CHARAC_CERASHOP_RESTRICT_ORM_H
#define CHARAC_CERASHOP_RESTRICT_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * Charac Cerashop Restrict ORM - 角色Cera商店限购系统
 *
 * 表: charac_cerashop_restrict (taiwan_cain 数据库)
 * 描述: 角色在Cera商店的限购记录表
 * 主键: (charac_no, ipg_no) (复合主键)
 *
 * 字段说明:
 * - charac_no: 角色编号 (INT UNSIGNED, PRIMARY KEY 1)
 * - ipg_no: 商品编号 (INT UNSIGNED, PRIMARY KEY 2)
 * - count: 已购买数量 (INT UNSIGNED)
 * - next_date: 下次可购买日期 (INT UNSIGNED, Unix时间戳)
 * - end_date: 限购结束日期 (INT UNSIGNED, Unix时间戳)
 * - last_access_date: 最后访问日期 (INT UNSIGNED, Unix时间戳)
 *
 * 业务用途:
 * - 记录角色在Cera商店的限购物品购买情况
 * - 控制限购数量和时间限制
 * - 支持按角色查询所有限购记录
 *
 * API设计: 11个函数
 * - CRUD操作: 5个
 * - 查询操作: 4个
 * - 业务操作: 1个
 * - 工具函数: 1个
 *
 * 创建日期: 2025-11-15
 * ORM模块编号: #63/421
 ******************************************************************************/

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

/* 角色Cera商店限购记录结构 */
typedef struct {
    unsigned int charac_no;         /* 角色编号 (复合主键1) */
    unsigned int ipg_no;            /* 商品编号 (复合主键2) */
    unsigned int count;             /* 已购买数量 */
    unsigned int next_date;         /* 下次可购买日期 (Unix时间戳) */
    unsigned int end_date;          /* 限购结束日期 (Unix时间戳) */
    unsigned int last_access_date;  /* 最后访问日期 (Unix时间戳) */
} CharacCerashopRestrict;

/*******************************************************************************
 * CRUD 操作 (5个函数)
 ******************************************************************************/

/**
 * 添加或更新Cera商店限购记录（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int CharacCerashopRestrict_Add(DBConnectionManager* manager, const CharacCerashopRestrict* record);

/**
 * 获取指定角色的特定商品限购记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param ipg_no 商品编号
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int CharacCerashopRestrict_Get(DBConnectionManager* manager, unsigned int charac_no, unsigned int ipg_no, CharacCerashopRestrict* record);

/**
 * 更新Cera商店限购记录
 * @param manager 数据库连接管理器
 * @param record 包含更新数据的记录
 * @return 成功返回0，失败返回-1
 */
int CharacCerashopRestrict_Update(DBConnectionManager* manager, const CharacCerashopRestrict* record);

/**
 * 删除Cera商店限购记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param ipg_no 商品编号
 * @return 成功返回0，失败返回-1
 */
int CharacCerashopRestrict_Delete(DBConnectionManager* manager, unsigned int charac_no, unsigned int ipg_no);

/**
 * 检查Cera商店限购记录是否存在
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param ipg_no 商品编号
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int CharacCerashopRestrict_Exists(DBConnectionManager* manager, unsigned int charac_no, unsigned int ipg_no);

/*******************************************************************************
 * 查询操作 (4个函数)
 ******************************************************************************/

/**
 * 获取限购记录总数
 * @param manager 数据库连接管理器
 * @return >=0记录数，<0失败
 */
int CharacCerashopRestrict_Count(DBConnectionManager* manager);

/**
 * 获取所有限购记录
 * @param manager 数据库连接管理器
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int CharacCerashopRestrict_GetAll(DBConnectionManager* manager, CharacCerashopRestrict* records, int max_records);

/**
 * 获取指定角色的所有限购记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际返回的记录数，失败返回-1
 */
int CharacCerashopRestrict_GetByCharacter(DBConnectionManager* manager, unsigned int charac_no, CharacCerashopRestrict* records, int max_records);

/**
 * 删除指定角色的所有限购记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回删除的记录数，失败返回-1
 */
int CharacCerashopRestrict_DeleteByCharacter(DBConnectionManager* manager, unsigned int charac_no);

/*******************************************************************************
 * 业务操作 (1个函数)
 ******************************************************************************/

/**
 * 增加购买数量（原子操作）
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param ipg_no 商品编号
 * @param amount 增加的数量
 * @return 成功返回0，失败返回-1
 */
int CharacCerashopRestrict_IncreaseCount(DBConnectionManager* manager, unsigned int charac_no, unsigned int ipg_no, unsigned int amount);

/*******************************************************************************
 * 工具函数 (1个函数)
 ******************************************************************************/

/**
 * 打印Cera商店限购记录信息（用于调试）
 * @param record Cera商店限购记录指针
 */
void CharacCerashopRestrict_PrintInfo(const CharacCerashopRestrict* record);

#endif /* CHARAC_CERASHOP_RESTRICT_ORM_H */
