-- ========================================
-- DNF GM镜像数据库 - Schema
-- Week 3 基础设施配置
-- 生成日期: 2025-11-09
-- ========================================
-- 说明：此数据库用于存储从生产数据库同步过来的只读数据
-- 数据通过Canal实时同步，GM工具仅查询不修改
-- ========================================

-- ========================================
-- 创建镜像数据库
-- ========================================

CREATE DATABASE IF NOT EXISTS taiwan_cain_mirror
CHARACTER SET utf8mb4
COLLATE utf8mb4_unicode_ci;

CREATE DATABASE IF NOT EXISTS taiwan_login_mirror
CHARACTER SET utf8mb4
COLLATE utf8mb4_unicode_ci;

-- ========================================
-- taiwan_cain_mirror - 角色数据库镜像
-- ========================================

USE taiwan_cain_mirror;

-- 角色基础信息表
CREATE TABLE IF NOT EXISTS charac_info (
    char_no INT PRIMARY KEY COMMENT '角色编号',
    m_id VARCHAR(64) NOT NULL COMMENT '账号ID',
    char_name VARCHAR(64) NOT NULL COMMENT '角色名',

    -- 角色属性
    job INT NOT NULL COMMENT '职业ID',
    lev INT NOT NULL DEFAULT 1 COMMENT '等级',
    grow_type INT COMMENT '成长类型',
    exp BIGINT DEFAULT 0 COMMENT '经验值',

    -- 状态
    delete_flag TINYINT DEFAULT 0 COMMENT '删除标记：0-正常，1-已删除',
    login_time TIMESTAMP NULL COMMENT '最后登录时间',
    logout_time TIMESTAMP NULL COMMENT '最后登出时间',
    create_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',

    -- 游戏数据
    dungeon_clear_point INT DEFAULT 0 COMMENT '地下城清除点数',
    hp INT COMMENT '生命值',
    mp INT COMMENT '魔法值',

    INDEX idx_m_id (m_id),
    INDEX idx_char_name (char_name),
    INDEX idx_delete_flag (delete_flag),
    INDEX idx_job (job),
    INDEX idx_lev (lev)
) ENGINE=InnoDB COMMENT='角色基础信息表（镜像）';

-- 角色仓库表
CREATE TABLE IF NOT EXISTS charac_warehouse (
    char_no INT NOT NULL COMMENT '角色编号',
    slot_no INT NOT NULL COMMENT '槽位编号',

    -- 物品信息
    item_id INT COMMENT '物品ID',
    amount INT DEFAULT 1 COMMENT '数量',
    upgrade_level INT DEFAULT 0 COMMENT '强化等级',
    enchant_id INT DEFAULT 0 COMMENT '附魔ID',

    -- 物品属性
    is_bound TINYINT DEFAULT 0 COMMENT '是否绑定',
    expire_time TIMESTAMP NULL COMMENT '过期时间',

    -- 时间信息
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '获得时间',
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',

    PRIMARY KEY (char_no, slot_no),
    INDEX idx_item_id (item_id),
    INDEX idx_char_no (char_no)
) ENGINE=InnoDB COMMENT='角色仓库表（镜像）';

-- 角色装备表
CREATE TABLE IF NOT EXISTS charac_equip (
    char_no INT NOT NULL COMMENT '角色编号',
    equip_slot INT NOT NULL COMMENT '装备槽位',

    -- 装备信息
    item_id INT COMMENT '物品ID',
    upgrade_level INT DEFAULT 0 COMMENT '强化等级',
    enchant_id INT DEFAULT 0 COMMENT '附魔ID',
    amplify_option INT DEFAULT 0 COMMENT '增幅选项',

    -- 装备属性
    strength_bonus INT DEFAULT 0 COMMENT '力量加成',
    intelligence_bonus INT DEFAULT 0 COMMENT '智力加成',
    vitality_bonus INT DEFAULT 0 COMMENT '体力加成',
    spirit_bonus INT DEFAULT 0 COMMENT '精神加成',

    -- 时间信息
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '装备时间',
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',

    PRIMARY KEY (char_no, equip_slot),
    INDEX idx_item_id (item_id)
) ENGINE=InnoDB COMMENT='角色装备表（镜像）';

-- 角色任务表
CREATE TABLE IF NOT EXISTS charac_quest (
    char_no INT NOT NULL COMMENT '角色编号',
    quest_id INT NOT NULL COMMENT '任务ID',

    -- 任务状态
    quest_status TINYINT NOT NULL COMMENT '任务状态：0-未接受，1-进行中，2-已完成',
    quest_progress INT DEFAULT 0 COMMENT '任务进度',

    -- 时间信息
    accepted_at TIMESTAMP NULL COMMENT '接受时间',
    completed_at TIMESTAMP NULL COMMENT '完成时间',

    PRIMARY KEY (char_no, quest_id),
    INDEX idx_quest_status (quest_status)
) ENGINE=InnoDB COMMENT='角色任务表（镜像）';

-- 账号信息表
CREATE TABLE IF NOT EXISTS member_info (
    m_id VARCHAR(64) PRIMARY KEY COMMENT '账号ID',

    -- 账号基础信息
    account_name VARCHAR(64) COMMENT '账号名',
    account_type TINYINT DEFAULT 0 COMMENT '账号类型：0-普通，1-VIP',

    -- 货币
    cera_point INT DEFAULT 0 COMMENT '点券',
    mileage_point INT DEFAULT 0 COMMENT '里程',

    -- 状态
    is_banned TINYINT DEFAULT 0 COMMENT '是否封禁',
    ban_expire_time TIMESTAMP NULL COMMENT '封禁过期时间',

    -- 时间信息
    register_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '注册时间',
    last_login_time TIMESTAMP NULL COMMENT '最后登录时间',

    INDEX idx_account_name (account_name),
    INDEX idx_is_banned (is_banned)
) ENGINE=InnoDB COMMENT='账号信息表（镜像）';

-- 账号封禁记录表
CREATE TABLE IF NOT EXISTS member_punish_info (
    punish_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '处罚ID',
    m_id VARCHAR(64) NOT NULL COMMENT '账号ID',

    -- 处罚信息
    punish_type TINYINT NOT NULL COMMENT '处罚类型：1-封禁，2-禁言，3-警告',
    punish_reason VARCHAR(512) COMMENT '处罚原因',
    punish_duration INT COMMENT '处罚时长（秒）',

    -- 操作人
    operator VARCHAR(64) COMMENT '操作人',

    -- 时间信息
    punish_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '处罚时间',
    expire_time TIMESTAMP NULL COMMENT '过期时间',

    INDEX idx_m_id (m_id),
    INDEX idx_punish_type (punish_type),
    INDEX idx_punish_time (punish_time)
) ENGINE=InnoDB COMMENT='账号封禁记录表（镜像）';

-- ========================================
-- taiwan_login_mirror - 登录数据库镜像
-- ========================================

USE taiwan_login_mirror;

-- 登录日志表
CREATE TABLE IF NOT EXISTS member_login_log (
    log_id BIGINT PRIMARY KEY AUTO_INCREMENT COMMENT '日志ID',
    m_id VARCHAR(64) NOT NULL COMMENT '账号ID',

    -- 登录信息
    login_ip VARCHAR(45) COMMENT '登录IP',
    login_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '登录时间',
    logout_time TIMESTAMP NULL COMMENT '登出时间',

    -- 在线时长
    online_duration INT COMMENT '在线时长（秒）',

    INDEX idx_m_id (m_id),
    INDEX idx_login_time (login_time),
    INDEX idx_login_ip (login_ip)
) ENGINE=InnoDB COMMENT='登录日志表（镜像）';

-- ========================================
-- 创建只读用户（用于GM工具访问）
-- ========================================

-- 注意：此用户仅用于GM工具读取镜像数据库，无写权限
CREATE USER IF NOT EXISTS 'gm_tools_reader'@'%' IDENTIFIED BY 'CHANGE_ME_READER_PASSWORD';

-- 授予只读权限
GRANT SELECT ON taiwan_cain_mirror.* TO 'gm_tools_reader'@'%';
GRANT SELECT ON taiwan_login_mirror.* TO 'gm_tools_reader'@'%';

FLUSH PRIVILEGES;

-- ========================================
-- 数据同步监控视图
-- ========================================

USE taiwan_cain_mirror;

-- 角色统计视图
CREATE OR REPLACE VIEW v_charac_stats AS
SELECT
    COUNT(*) AS total_characters,
    COUNT(DISTINCT m_id) AS total_accounts,
    AVG(lev) AS avg_level,
    MAX(lev) AS max_level,
    SUM(CASE WHEN delete_flag = 0 THEN 1 ELSE 0 END) AS active_characters,
    SUM(CASE WHEN delete_flag = 1 THEN 1 ELSE 0 END) AS deleted_characters
FROM charac_info;

-- 职业分布视图
CREATE OR REPLACE VIEW v_job_distribution AS
SELECT
    job,
    COUNT(*) AS character_count,
    AVG(lev) AS avg_level
FROM charac_info
WHERE delete_flag = 0
GROUP BY job
ORDER BY character_count DESC;

-- ========================================
-- 镜像数据库初始化完成
-- ========================================

SELECT 'GM Mirror Database Schema Created Successfully!' AS status;
SELECT 'Waiting for Canal to sync data from production...' AS next_step;
