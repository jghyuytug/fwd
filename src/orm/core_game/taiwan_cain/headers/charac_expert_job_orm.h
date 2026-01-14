#ifndef CHARAC_EXPERT_JOB_ORM_H
#define CHARAC_EXPERT_JOB_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

#define MAX_RECIPE_LEN 4096

typedef struct {
    unsigned int charac_no;                      /* 角色编号 (主键) */
    unsigned char expert_job_giveup_cnt;         /* 专家职业放弃次数 */
    int expert_job_info;                         /* 专家职业信息 */
    int expert_job_info_ex;                      /* 专家职业扩展信息 */
    unsigned char recipe[MAX_RECIPE_LEN];        /* 配方数据 (BLOB) */
    int recipe_len;                              /* 配方数据实际长度 */
} CharacExpertJob;

/*******************************************************************************
 * CRUD 操作
 ******************************************************************************/

/**
 * 添加或更新专家职业记录（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int CharacExpertJob_Add(DBConnectionManager* manager, const CharacExpertJob* record);

/**
 * 根据角色编号获取专家职业记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int CharacExpertJob_Get(DBConnectionManager* manager, unsigned int charac_no,
                        CharacExpertJob* record);

/**
 * 更新专家职业信息
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param expert_job_info 新的职业信息
 * @param expert_job_info_ex 新的扩展信息
 * @return 成功返回0，失败返回-1
 */
int CharacExpertJob_Update(DBConnectionManager* manager, unsigned int charac_no,
                           int expert_job_info, int expert_job_info_ex);

/**
 * 删除专家职业记录
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0，失败返回-1
 */
int CharacExpertJob_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查专家职业记录是否存在
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int CharacExpertJob_Exists(DBConnectionManager* manager, unsigned int charac_no);

/*******************************************************************************
 * 业务操作
 ******************************************************************************/

/**
 * 更新配方数据
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param recipe 新的配方数据
 * @param recipe_len 配方数据长度
 * @return 成功返回0，失败返回-1
 */
int CharacExpertJob_UpdateRecipe(DBConnectionManager* manager, unsigned int charac_no,
                                 const unsigned char* recipe, int recipe_len);

/**
 * 递增放弃次数
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param increment 增加的次数（默认为1）
 * @return 成功返回0，失败返回-1
 */
int CharacExpertJob_IncrementGiveupCnt(DBConnectionManager* manager,
                                       unsigned int charac_no,
                                       unsigned int increment);

/*******************************************************************************
 * 工具函数
 ******************************************************************************/

/**
 * 打印专家职业信息（用于调试）
 * @param record 要打印的记录
 */
void CharacExpertJob_PrintInfo(const CharacExpertJob* record);

#endif /* CHARAC_EXPERT_JOB_ORM_H */
