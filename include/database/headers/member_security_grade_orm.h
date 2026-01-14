#ifndef MEMBER_SECURITY_GRADE_ORM_H
#define MEMBER_SECURITY_GRADE_ORM_H

#include "db_connection_manager.h"

/**
 * member_security_grade - 会员安全等级表
 *
 * 表结构:
 *   m_id                         INT(11)              主键 - 会员ID
 *   last_visit_time              DATETIME             最后访问时间
 *   pass_fail_cnt                INT(11)              密码失败总次数
 *   last_vaccine_date            DATETIME             最后安全扫描日期
 *   last_window_date             DATETIME             最后窗口日期
 *   goblin_pass_mod              DATETIME             Goblin密码修改时间
 *   goblin_fail_cnt              INT(11)              Goblin失败次数
 *   security_card_reg            DATETIME             安全卡注册时间
 *   security_card_fail_cnt       INT(11)              安全卡失败次数
 *   m_opt_reg                    DATETIME             移动OTP注册时间
 *   pc_opt_reg                   DATETIME             PC OTP注册时间
 *   black_ip_try_time            DATETIME             黑名单IP尝试时间
 *   linear_pass_fail_cnt         INT(11)              连续密码失败次数
 *   last_pass_fail_time          INT(10) UNSIGNED     最后密码失败时间（时间戳）
 *   last_check_time              DATETIME             最后检查时间
 *   pass_modify_check            DATETIME             密码修改检查时间
 *   member_pc_reg                DATETIME             会员PC注册时间
 *   gatekeeper_otp_reg           DATETIME             门禁OTP注册时间
 *   goblin_validity_time         INT(11)              Goblin有效期（秒）
 *   security_card_validity_time  INT(11)              安全卡有效期（秒）
 *   validity_ip                  VARCHAR(15)          有效IP地址
 *   cargopad_status              TINYINT(4)           Cargopad状态
 *   cargopad_mod                 DATETIME             Cargopad修改时间
 *   cargopad_validity_time       INT(11)              Cargopad有效期（秒）
 *
 * 用途: 跟踪会员账号的安全状态和各种认证方式（OTP、安全卡、Goblin等）
 * 主键: m_id
 * 索引: idx_pass_check (last_pass_fail_time, linear_pass_fail_cnt)
 */

/* 结构体定义 */
typedef struct {
    int m_id;                          /* 会员ID (主键) */
    char last_visit_time[20];          /* 最后访问时间 (YYYY-MM-DD HH:MM:SS) */
    int pass_fail_cnt;                 /* 密码失败总次数 */
    char last_vaccine_date[20];        /* 最后安全扫描日期 (YYYY-MM-DD HH:MM:SS) */
    char last_window_date[20];         /* 最后窗口日期 (YYYY-MM-DD HH:MM:SS) */
    char goblin_pass_mod[20];          /* Goblin密码修改时间 (YYYY-MM-DD HH:MM:SS) */
    int goblin_fail_cnt;               /* Goblin失败次数 */
    char security_card_reg[20];        /* 安全卡注册时间 (YYYY-MM-DD HH:MM:SS) */
    int security_card_fail_cnt;        /* 安全卡失败次数 */
    char m_opt_reg[20];                /* 移动OTP注册时间 (YYYY-MM-DD HH:MM:SS) */
    char pc_opt_reg[20];               /* PC OTP注册时间 (YYYY-MM-DD HH:MM:SS) */
    char black_ip_try_time[20];        /* 黑名单IP尝试时间 (YYYY-MM-DD HH:MM:SS) */
    int linear_pass_fail_cnt;          /* 连续密码失败次数 */
    unsigned int last_pass_fail_time;  /* 最后密码失败时间（UNIX时间戳） */
    char last_check_time[20];          /* 最后检查时间 (YYYY-MM-DD HH:MM:SS) */
    char pass_modify_check[20];        /* 密码修改检查时间 (YYYY-MM-DD HH:MM:SS) */
    char member_pc_reg[20];            /* 会员PC注册时间 (YYYY-MM-DD HH:MM:SS) */
    char gatekeeper_otp_reg[20];       /* 门禁OTP注册时间 (YYYY-MM-DD HH:MM:SS) */
    int goblin_validity_time;          /* Goblin有效期（秒） */
    int security_card_validity_time;   /* 安全卡有效期（秒） */
    char validity_ip[16];              /* 有效IP地址 */
    char cargopad_status;              /* Cargopad状态 */
    char cargopad_mod[20];             /* Cargopad修改时间 (YYYY-MM-DD HH:MM:SS) */
    int cargopad_validity_time;        /* Cargopad有效期（秒） */
} MemberSecurityGrade;

/* ========================================
 * CRUD 操作
 * ======================================== */

/**
 * 添加/更新会员安全等级记录（幂等操作）
 * 使用 REPLACE INTO 确保幂等性
 *
 * @param manager 数据库连接管理器
 * @param record 会员安全等级记录
 * @return 0=成功, -1=失败
 */
int MemberSecurityGrade_Add(DBConnectionManager* manager, const MemberSecurityGrade* record);

/**
 * 获取会员安全等级记录
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @param record 输出记录
 * @return 0=成功, -1=失败
 */
int MemberSecurityGrade_Get(DBConnectionManager* manager, int m_id, MemberSecurityGrade* record);

/**
 * 更新会员安全等级记录
 *
 * @param manager 数据库连接管理器
 * @param record 会员安全等级记录
 * @return 0=成功, -1=失败
 */
int MemberSecurityGrade_Update(DBConnectionManager* manager, const MemberSecurityGrade* record);

/**
 * 删除会员安全等级记录（幂等操作）
 * 无论记录是否存在都返回0
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 0=成功（包括记录不存在）, -1=SQL执行失败
 */
int MemberSecurityGrade_Delete(DBConnectionManager* manager, int m_id);

/**
 * 检查会员安全等级记录是否存在
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 1=存在, 0=不存在
 */
int MemberSecurityGrade_Exists(DBConnectionManager* manager, int m_id);

/* ========================================
 * 业务查询
 * ======================================== */

/**
 * 原子递增密码失败次数
 * 同时更新 last_pass_fail_time 为当前时间戳
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 0=成功, -1=失败
 */
int MemberSecurityGrade_IncrementPassFailCnt(DBConnectionManager* manager, int m_id);

/**
 * 重置密码失败次数
 * 将 pass_fail_cnt 和 linear_pass_fail_cnt 都重置为0
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 0=成功, -1=失败
 */
int MemberSecurityGrade_ResetPassFailCnt(DBConnectionManager* manager, int m_id);

/**
 * 原子递增连续密码失败次数
 * 同时更新 last_pass_fail_time 为当前时间戳
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 0=成功, -1=失败
 */
int MemberSecurityGrade_IncrementLinearPassFailCnt(DBConnectionManager* manager, int m_id);

/**
 * 按密码失败次数范围查询
 *
 * @param manager 数据库连接管理器
 * @param min_count 最小失败次数
 * @param max_count 最大失败次数
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSecurityGrade_GetByPassFailCntRange(DBConnectionManager* manager, int min_count, int max_count,
                                                 MemberSecurityGrade* records, int max_records);

/**
 * 按连续密码失败次数查询
 * 用于找出当前处于连续失败状态的账号
 *
 * @param manager 数据库连接管理器
 * @param min_linear_cnt 最小连续失败次数
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSecurityGrade_GetByLinearPassFailCnt(DBConnectionManager* manager, int min_linear_cnt,
                                                  MemberSecurityGrade* records, int max_records);

/**
 * 按有效IP查询
 *
 * @param manager 数据库连接管理器
 * @param validity_ip 有效IP地址（支持LIKE模式，如"192.168.%"）
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSecurityGrade_GetByValidityIP(DBConnectionManager* manager, const char* validity_ip,
                                          MemberSecurityGrade* records, int max_records);

/**
 * 按最后访问时间范围查询
 *
 * @param manager 数据库连接管理器
 * @param start_time 开始时间 (YYYY-MM-DD HH:MM:SS)
 * @param end_time 结束时间 (YYYY-MM-DD HH:MM:SS)
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSecurityGrade_GetByLastVisitTimeRange(DBConnectionManager* manager, const char* start_time,
                                                   const char* end_time, MemberSecurityGrade* records, int max_records);

/**
 * 获取高风险用户
 * 定义：pass_fail_cnt >= threshold 或 linear_pass_fail_cnt >= linear_threshold
 *
 * @param manager 数据库连接管理器
 * @param fail_threshold 密码失败次数阈值
 * @param linear_threshold 连续失败次数阈值
 * @param records 输出数组
 * @param max_records 数组最大容量
 * @return 实际查询到的记录数, -1=失败
 */
int MemberSecurityGrade_GetHighRiskUsers(DBConnectionManager* manager, int fail_threshold, int linear_threshold,
                                           MemberSecurityGrade* records, int max_records);

/**
 * 更新最后访问时间
 * 将 last_visit_time 更新为当前时间
 *
 * @param manager 数据库连接管理器
 * @param m_id 会员ID
 * @return 0=成功, -1=失败
 */
int MemberSecurityGrade_UpdateLastVisitTime(DBConnectionManager* manager, int m_id);

/* ========================================
 * 批量操作
 * ======================================== */

/**
 * 批量添加安全等级记录
 *
 * @param manager 数据库连接管理器
 * @param records 记录数组
 * @param count 数组长度
 * @return 成功添加的记录数, -1=失败
 */
int MemberSecurityGrade_BulkAdd(DBConnectionManager* manager, const MemberSecurityGrade* records, int count);

/**
 * 批量删除安全等级记录
 *
 * @param manager 数据库连接管理器
 * @param m_ids 会员ID数组
 * @param count 数组长度
 * @return 0=成功, -1=失败
 */
int MemberSecurityGrade_BulkDelete(DBConnectionManager* manager, const int* m_ids, int count);

/* ========================================
 * 工具函数
 * ======================================== */

/**
 * 打印安全等级记录信息（用于调试）
 *
 * @param record 安全等级记录
 */
void MemberSecurityGrade_PrintInfo(const MemberSecurityGrade* record);

#endif /* MEMBER_SECURITY_GRADE_ORM_H */
