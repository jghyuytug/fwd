#ifndef CHARAC_HOUSING_INFO_ORM_H
#define CHARAC_HOUSING_INFO_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

#define MAX_DECORATION_INVEN_LEN 144

typedef struct {
    unsigned int charac_no;                           /* 角色编号 (主键) */
    unsigned short installed;                         /* 已安装装饰物数量 */
    unsigned char decoration_inven[MAX_DECORATION_INVEN_LEN]; /* 装饰物品栏 */
    int decoration_inven_len;                         /* 装饰物品栏实际长度 */
    unsigned short version;                           /* 版本号 */
} CharacHousingInfo;

/*******************************************************************************
 * CRUD 操作
 ******************************************************************************/

/**
 * 添加或更新角色住房信息（使用 REPLACE INTO 实现幂等性）
 * @param manager 数据库连接管理器
 * @param record 要添加的记录
 * @return 成功返回0，失败返回-1
 */
int CharacHousingInfo_Add(DBConnectionManager* manager, const CharacHousingInfo* record);

/**
 * 根据角色编号获取住房信息
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int CharacHousingInfo_Get(DBConnectionManager* manager, unsigned int charac_no,
                          CharacHousingInfo* record);

/**
 * 更新住房信息
 * @param manager 数据库连接管理器
 * @param record 要更新的记录
 * @return 成功返回0，失败返回-1
 */
int CharacHousingInfo_Update(DBConnectionManager* manager, const CharacHousingInfo* record);

/**
 * 删除住房信息
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 成功返回0，失败返回-1
 */
int CharacHousingInfo_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查住房信息是否存在
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int CharacHousingInfo_Exists(DBConnectionManager* manager, unsigned int charac_no);

/*******************************************************************************
 * 业务操作
 ******************************************************************************/

/**
 * 更新装饰物品栏
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param decoration_inven 装饰物品栏数据
 * @param decoration_inven_len 数据长度
 * @return 成功返回0，失败返回-1
 */
int CharacHousingInfo_UpdateDecorationInven(DBConnectionManager* manager,
                                            unsigned int charac_no,
                                            const unsigned char* decoration_inven,
                                            int decoration_inven_len);

/**
 * 更新已安装数量
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param installed 新的已安装数量
 * @return 成功返回0，失败返回-1
 */
int CharacHousingInfo_UpdateInstalled(DBConnectionManager* manager,
                                      unsigned int charac_no,
                                      unsigned short installed);

/**
 * 更新版本号
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param version 新版本号
 * @return 成功返回0，失败返回-1
 */
int CharacHousingInfo_UpdateVersion(DBConnectionManager* manager,
                                    unsigned int charac_no,
                                    unsigned short version);

/**
 * 递增已安装数量
 * @param manager 数据库连接管理器
 * @param charac_no 角色编号
 * @param increment 增加的数量（默认为1）
 * @return 成功返回0，失败返回-1
 */
int CharacHousingInfo_IncrementInstalled(DBConnectionManager* manager,
                                         unsigned int charac_no,
                                         unsigned int increment);

/*******************************************************************************
 * 工具函数
 ******************************************************************************/

/**
 * 打印住房信息（用于调试）
 * @param record 要打印的记录
 */
void CharacHousingInfo_PrintInfo(const CharacHousingInfo* record);

#endif /* CHARAC_HOUSING_INFO_ORM_H */
