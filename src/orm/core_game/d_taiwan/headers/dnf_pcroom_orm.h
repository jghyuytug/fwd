#ifndef DNF_PCROOM_ORM_H
#define DNF_PCROOM_ORM_H

#include "db_connection_manager.h"

/*******************************************************************************
 * 数据结构定义
 ******************************************************************************/

typedef struct {
    int ip_no;                   /* IP编号 (主键, 自增) */
    char district[21];           /* 地区 */
    char firm_name[51];          /* 网吧名称 */
    char telephone[21];          /* 电话 */
    char address[151];           /* 地址 */
    char leader[31];             /* 负责人 */
    char ip[12];                 /* IP地址前缀 (如 "192.168.1") */
    unsigned char start_ip;      /* 起始IP最后一段 (0-255) */
    unsigned char end_ip;        /* 结束IP最后一段 (0-255) */
} DnfPcroom;

/*******************************************************************************
 * CRUD 操作
 ******************************************************************************/

/**
 * 添加网吧信息
 * @param manager 数据库连接管理器
 * @param record 要添加的记录 (ip_no将被忽略，使用自动生成)
 * @return 成功返回新记录的ip_no，失败返回-1
 */
int DnfPcroom_Add(DBConnectionManager* manager, const DnfPcroom* record);

/**
 * 根据IP编号获取网吧信息
 * @param manager 数据库连接管理器
 * @param ip_no IP编号
 * @param record 输出参数，存储查询结果
 * @return 成功返回0，未找到或失败返回-1
 */
int DnfPcroom_Get(DBConnectionManager* manager, int ip_no, DnfPcroom* record);

/**
 * 更新网吧信息
 * @param manager 数据库连接管理器
 * @param record 要更新的记录
 * @return 成功返回0，失败返回-1
 */
int DnfPcroom_Update(DBConnectionManager* manager, const DnfPcroom* record);

/**
 * 删除网吧信息
 * @param manager 数据库连接管理器
 * @param ip_no IP编号
 * @return 成功返回0，失败返回-1
 */
int DnfPcroom_Delete(DBConnectionManager* manager, int ip_no);

/**
 * 检查网吧信息是否存在
 * @param manager 数据库连接管理器
 * @param ip_no IP编号
 * @return 存在返回1，不存在返回0，错误返回-1
 */
int DnfPcroom_Exists(DBConnectionManager* manager, int ip_no);

/*******************************************************************************
 * 业务操作
 ******************************************************************************/

/**
 * 根据地区查询网吧列表
 * @param manager 数据库连接管理器
 * @param district 地区名称
 * @param records 输出参数，存储记录数组（调用者需要提前分配足够空间）
 * @param max_count 最大记录数
 * @return 成功返回实际记录数，失败返回-1
 */
int DnfPcroom_GetByDistrict(DBConnectionManager* manager, const char* district,
                            DnfPcroom* records, int max_count);

/**
 * 根据网吧名称查询（模糊匹配）
 * @param manager 数据库连接管理器
 * @param firm_name 网吧名称（支持部分匹配）
 * @param records 输出参数，存储记录数组
 * @param max_count 最大记录数
 * @return 成功返回实际记录数，失败返回-1
 */
int DnfPcroom_SearchByFirmName(DBConnectionManager* manager, const char* firm_name,
                               DnfPcroom* records, int max_count);

/**
 * 检查指定IP是否在网吧IP范围内
 * @param manager 数据库连接管理器
 * @param full_ip 完整IP地址（如 "192.168.1.100"）
 * @param record 输出参数，如果找到则存储匹配的网吧信息
 * @return 找到返回1，未找到返回0，错误返回-1
 */
int DnfPcroom_CheckIpInRange(DBConnectionManager* manager, const char* full_ip,
                             DnfPcroom* record);

/**
 * 根据负责人查询网吧列表
 * @param manager 数据库连接管理器
 * @param leader 负责人姓名
 * @param records 输出参数，存储记录数组
 * @param max_count 最大记录数
 * @return 成功返回实际记录数，失败返回-1
 */
int DnfPcroom_GetByLeader(DBConnectionManager* manager, const char* leader,
                          DnfPcroom* records, int max_count);

/**
 * 统计网吧总数
 * @param manager 数据库连接管理器
 * @return 成功返回网吧总数，失败返回-1
 */
int DnfPcroom_Count(DBConnectionManager* manager);

/**
 * 统计指定地区的网吧数量
 * @param manager 数据库连接管理器
 * @param district 地区名称
 * @return 成功返回网吧数量，失败返回-1
 */
int DnfPcroom_CountByDistrict(DBConnectionManager* manager, const char* district);

/*******************************************************************************
 * 工具函数
 ******************************************************************************/

/**
 * 打印网吧信息（用于调试）
 * @param record 要打印的记录
 */
void DnfPcroom_PrintInfo(const DnfPcroom* record);

/**
 * 验证IP范围有效性
 * @param start_ip 起始IP
 * @param end_ip 结束IP
 * @return 有效返回1，无效返回0
 */
int DnfPcroom_ValidateIpRange(unsigned char start_ip, unsigned char end_ip);

#endif /* DNF_PCROOM_ORM_H */
