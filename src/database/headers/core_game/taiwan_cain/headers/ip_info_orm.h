#ifndef IP_INFO_ORM_H
#define IP_INFO_ORM_H

#include "db_connection_manager.h"

/**
 * IP信息管理系统 ORM
 *
 * 功能：
 * - IP地址范围管理（ip前缀 + start_ip ~ end_ip）
 * - 网吧计费系统（vendor_no, charge_flag, settle_no）
 * - IP安全检查（ip_check标志）
 * - 网速等级管理（speed_no）
 * - 时间范围控制（start_time, end_time）
 * - 用户IP关联（m_id会员ID）
 *
 * 索引优化：
 * - UNIQUE KEY (ip, start_ip, end_ip) - IP范围唯一性
 * - INDEX m_id, ip, start_ip, end_ip, occ_time
 */

/* IP信息记录结构 */
typedef struct {
    unsigned int no;              /* 自增主键 */
    unsigned int m_id;            /* 会员ID */
    char ip[12];                  /* IP地址前缀（varchar(11) + \0） */
    unsigned char start_ip;       /* IP范围起始（最后一段） */
    unsigned char end_ip;         /* IP范围结束（最后一段） */
    char occ_time[20];            /* 发生时间（YYYY-MM-DD HH:MM:SS） */
    unsigned char ip_check;       /* IP检查标志（0=未检查，1=已检查，2=异常） */
    unsigned int vendor_no;       /* 供应商编号（网吧标识） */
    unsigned int speed_no;        /* 网速等级编号（1=慢速，2=中速，3=高速） */
    char start_time[20];          /* 开始时间（YYYY-MM-DD HH:MM:SS） */
    char end_time[20];            /* 结束时间（YYYY-MM-DD HH:MM:SS） */
    char charge_flag;             /* 计费标志（0=免费，1=计费） */
    unsigned int settle_no;       /* 结算编号 */
} IpInfo;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加IP信息记录（使用 INSERT）
 *
 * @param manager 数据库连接管理器
 * @param record IP信息记录（no自增，传入时忽略）
 * @return >=0 插入的自增ID, <0 失败
 */
int IpInfo_Add(DBConnectionManager* manager, const IpInfo* record);

/**
 * 获取IP信息记录（按主键no）
 *
 * @param manager 数据库连接管理器
 * @param no 主键编号
 * @param record 输出参数，接收记录数据
 * @return 0 成功, <0 失败或记录不存在
 */
int IpInfo_Get(DBConnectionManager* manager, unsigned int no, IpInfo* record);

/**
 * 更新IP信息记录（按主键no）
 *
 * @param manager 数据库连接管理器
 * @param record IP信息记录（必须包含有效的no）
 * @return 0 成功, <0 失败
 */
int IpInfo_Update(DBConnectionManager* manager, const IpInfo* record);

/**
 * 删除IP信息记录（按主键no）
 *
 * @param manager 数据库连接管理器
 * @param no 主键编号
 * @return 0 成功, <0 失败
 */
int IpInfo_Delete(DBConnectionManager* manager, unsigned int no);

/**
 * 检查IP信息记录是否存在（按主键no）
 *
 * @param manager 数据库连接管理器
 * @param no 主键编号
 * @return 1 存在, 0 不存在, <0 错误
 */
int IpInfo_Exists(DBConnectionManager* manager, unsigned int no);

/* ==================== 专项操作 ==================== */

/**
 * 检查IP是否在记录范围内（ip前缀 + 最后一段匹配）
 *
 * @param manager 数据库连接管理器
 * @param ip_prefix IP前缀（如 "192.168.1."）
 * @param last_segment IP最后一段（如 100）
 * @param record 输出参数，接收匹配的记录
 * @return 0 匹配成功, <0 无匹配或错误
 */
int IpInfo_CheckIpInRange(DBConnectionManager* manager, const char* ip_prefix, unsigned char last_segment, IpInfo* record);

/**
 * 更新IP检查状态
 *
 * @param manager 数据库连接管理器
 * @param no 主键编号
 * @param ip_check 新的检查状态（0=未检查，1=已检查，2=异常）
 * @return 0 成功, <0 失败
 */
int IpInfo_UpdateCheckStatus(DBConnectionManager* manager, unsigned int no, unsigned char ip_check);

/**
 * 更新计费状态
 *
 * @param manager 数据库连接管理器
 * @param no 主键编号
 * @param charge_flag 计费标志（0=免费，1=计费）
 * @param settle_no 结算编号
 * @return 0 成功, <0 失败
 */
int IpInfo_UpdateChargeStatus(DBConnectionManager* manager, unsigned int no, char charge_flag, unsigned int settle_no);

/**
 * 延长结束时间（用于网吧续费）
 *
 * @param manager 数据库连接管理器
 * @param no 主键编号
 * @param hours 延长小时数
 * @return 0 成功, <0 失败
 */
int IpInfo_ExtendEndTime(DBConnectionManager* manager, unsigned int no, int hours);

/**
 * 批量删除过期记录（end_time < 当前时间）
 *
 * @param manager 数据库连接管理器
 * @return >=0 删除的记录数, <0 失败
 */
int IpInfo_DeleteExpired(DBConnectionManager* manager);

/* ==================== 查询操作 ==================== */

/**
 * 按会员ID查询IP记录数量
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return >=0 记录数量, <0 失败
 */
int IpInfo_CountByMemberId(DBConnectionManager* manager, unsigned int m_id);

/**
 * 按供应商编号查询记录数量
 *
 * @param manager 数据库连接管理器
 * @param vendor_no 供应商编号
 * @return >=0 记录数量, <0 失败
 */
int IpInfo_CountByVendor(DBConnectionManager* manager, unsigned int vendor_no);

/**
 * 按IP前缀查询记录数量
 *
 * @param manager 数据库连接管理器
 * @param ip_prefix IP前缀（如 "192.168.1."）
 * @return >=0 记录数量, <0 失败
 */
int IpInfo_CountByIpPrefix(DBConnectionManager* manager, const char* ip_prefix);

/**
 * 统计有效记录数（end_time >= 当前时间）
 *
 * @param manager 数据库连接管理器
 * @return >=0 有效记录数, <0 失败
 */
int IpInfo_CountActive(DBConnectionManager* manager);

/**
 * 按检查状态统计
 *
 * @param manager 数据库连接管理器
 * @param ip_check 检查状态
 * @return >=0 记录数量, <0 失败
 */
int IpInfo_CountByCheckStatus(DBConnectionManager* manager, unsigned char ip_check);

/* ==================== 工具函数 ==================== */

/**
 * 打印IP信息记录（调试用）
 *
 * @param record IP信息记录
 */
void IpInfo_PrintInfo(const IpInfo* record);

#endif /* IP_INFO_ORM_H */
