#ifndef DNF_RESTRICT_INFO_ORM_H
#define DNF_RESTRICT_INFO_ORM_H

#include "db_connection_manager.h"

/* dnf_restrict_info 表数据结构
 * 服务限制信息字符串
 * 主键: (category, restrict_code)
 */
typedef struct {
    int category;              // 分类
    int restrict_code;         // 限制代码
    char restrict_str[46];     // 限制字符串 (varchar(45) + '\0')
    char reg_date[20];         // 注册日期 (YYYY-MM-DD HH:MM:SS)
} DnfRestrictInfo;

/* CRUD 操作 */

/**
 * 添加或更新限制信息
 * @param manager 数据库连接管理器
 * @param record 限制信息记录
 * @return 0成功, <0失败
 */
int DnfRestrictInfo_Add(DBConnectionManager* manager, const DnfRestrictInfo* record);

/**
 * 获取限制信息
 * @param manager 数据库连接管理器
 * @param category 分类
 * @param restrict_code 限制代码
 * @param record 输出参数，限制信息记录
 * @return 0成功, <0失败
 */
int DnfRestrictInfo_Get(DBConnectionManager* manager, int category, int restrict_code, DnfRestrictInfo* record);

/**
 * 更新限制信息
 * @param manager 数据库连接管理器
 * @param record 限制信息记录
 * @return 0成功, <0失败
 */
int DnfRestrictInfo_Update(DBConnectionManager* manager, const DnfRestrictInfo* record);

/**
 * 删除限制信息
 * @param manager 数据库连接管理器
 * @param category 分类
 * @param restrict_code 限制代码
 * @return 0成功, <0失败
 */
int DnfRestrictInfo_Delete(DBConnectionManager* manager, int category, int restrict_code);

/**
 * 检查限制信息是否存在
 * @param manager 数据库连接管理器
 * @param category 分类
 * @param restrict_code 限制代码
 * @return 1存在, 0不存在, <0失败
 */
int DnfRestrictInfo_Exists(DBConnectionManager* manager, int category, int restrict_code);

/* 业务操作 */

/**
 * 根据分类获取所有限制信息
 * @param manager 数据库连接管理器
 * @param category 分类
 * @param records 输出参数，限制信息数组
 * @param count 输出参数，记录数量
 * @return 0成功, <0失败
 */
int DnfRestrictInfo_GetByCategory(DBConnectionManager* manager, int category, DnfRestrictInfo** records, int* count);

/**
 * 获取所有限制信息
 * @param manager 数据库连接管理器
 * @param records 输出参数，限制信息数组
 * @param count 输出参数，记录数量
 * @return 0成功, <0失败
 */
int DnfRestrictInfo_GetAll(DBConnectionManager* manager, DnfRestrictInfo** records, int* count);

/**
 * 根据限制字符串模糊查询
 * @param manager 数据库连接管理器
 * @param pattern 模糊匹配模式
 * @param records 输出参数，限制信息数组
 * @param count 输出参数，记录数量
 * @return 0成功, <0失败
 */
int DnfRestrictInfo_SearchByString(DBConnectionManager* manager, const char* pattern, DnfRestrictInfo** records, int* count);

/* 工具函数 */

/**
 * 打印限制信息
 * @param record 限制信息记录
 */
void DnfRestrictInfo_PrintInfo(const DnfRestrictInfo* record);

/**
 * 释放记录数组内存
 * @param records 限制信息数组
 */
void DnfRestrictInfo_FreeRecords(DnfRestrictInfo* records);

#endif /* DNF_RESTRICT_INFO_ORM_H */
