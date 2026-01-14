-- ========================================
-- DNF GM工具 - 数据库Schema
-- Week 3 基础设施配置
-- 生成日期: 2025-11-09
-- ========================================

-- 创建数据库
CREATE DATABASE IF NOT EXISTS gm_tools
CHARACTER SET utf8mb4
COLLATE utf8mb4_unicode_ci;

USE gm_tools;

-- ========================================
-- 1. GM账号表
-- ========================================

CREATE TABLE IF NOT EXISTS gm_accounts (
    gm_id INT PRIMARY KEY AUTO_INCREMENT COMMENT 'GM账号ID',
    username VARCHAR(64) UNIQUE NOT NULL COMMENT 'GM用户名',
    password_hash VARCHAR(128) NOT NULL COMMENT '密码哈希（bcrypt）',
    real_name VARCHAR(64) NOT NULL COMMENT '真实姓名',
    email VARCHAR(128) COMMENT '邮箱',
    phone VARCHAR(20) COMMENT '手机号',

    -- 权限级别
    gm_level INT NOT NULL DEFAULT 1 COMMENT 'GM等级：1-只读，2-基础操作，3-物品管理，4-账号管理，5-超级管理员',
    department VARCHAR(64) COMMENT '部门',

    -- 双因子认证
    two_factor_enabled BOOLEAN DEFAULT FALSE COMMENT '是否启用2FA',
    two_factor_secret VARCHAR(32) COMMENT '2FA密钥（Base32编码）',
    backup_codes TEXT COMMENT '备用恢复码（JSON数组，加密存储）',

    -- 状态
    is_active BOOLEAN DEFAULT TRUE COMMENT '账号是否激活',
    is_locked BOOLEAN DEFAULT FALSE COMMENT '账号是否锁定',
    locked_until TIMESTAMP NULL COMMENT '锁定至何时（NULL表示永久锁定）',
    failed_login_attempts INT DEFAULT 0 COMMENT '连续登录失败次数',

    -- 审计信息
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    created_by INT COMMENT '创建人GM ID',
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    updated_by INT COMMENT '更新人GM ID',
    last_login_at TIMESTAMP NULL COMMENT '最后登录时间',
    last_login_ip VARCHAR(45) COMMENT '最后登录IP',

    INDEX idx_username (username),
    INDEX idx_email (email),
    INDEX idx_gm_level (gm_level),
    INDEX idx_is_active (is_active),
    INDEX idx_last_login (last_login_at)
) ENGINE=InnoDB COMMENT='GM账号表';

-- 插入默认管理员账号（密码：Admin@123456，生产环境需立即修改）
INSERT INTO gm_accounts (username, password_hash, real_name, gm_level, two_factor_enabled)
VALUES ('admin', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', '系统管理员', 5, FALSE);

-- ========================================
-- 2. GM操作日志表（审计日志）
-- ========================================

CREATE TABLE IF NOT EXISTS gm_operation_logs (
    log_id BIGINT PRIMARY KEY AUTO_INCREMENT COMMENT '日志ID',

    -- 操作人信息
    gm_user VARCHAR(64) NOT NULL COMMENT 'GM用户名',
    gm_id INT NOT NULL COMMENT 'GM账号ID',
    gm_level INT NOT NULL COMMENT '操作时的GM等级',

    -- 操作信息
    operation_type VARCHAR(32) NOT NULL COMMENT '操作类型：PLAYER_QUERY/ITEM_GRANT/ACCOUNT_BAN/etc',
    operation_category VARCHAR(16) NOT NULL COMMENT '操作分类：READ/WRITE/DELETE',

    -- 目标对象
    target_type VARCHAR(32) COMMENT '目标类型：PLAYER/ACCOUNT/ITEM/etc',
    target_id VARCHAR(64) COMMENT '目标ID（角色编号、账号ID等）',
    target_name VARCHAR(64) COMMENT '目标名称（角色名、账号名等）',

    -- 操作详情
    operation_data JSON COMMENT '操作详细数据（JSON格式）',
    reason VARCHAR(512) COMMENT '操作原因',

    -- 操作结果
    result VARCHAR(16) NOT NULL COMMENT '操作结果：SUCCESS/FAILURE',
    error_code VARCHAR(32) COMMENT '错误码（如果失败）',
    error_message TEXT COMMENT '错误信息',

    -- 请求信息
    ip_address VARCHAR(45) NOT NULL COMMENT '操作IP地址',
    user_agent TEXT COMMENT 'User Agent',
    request_id VARCHAR(64) COMMENT '请求ID（用于追踪）',

    -- 时间信息
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '操作时间',
    execution_time_ms INT COMMENT '执行耗时（毫秒）',

    INDEX idx_gm_user_time (gm_user, created_at),
    INDEX idx_operation_type (operation_type),
    INDEX idx_target (target_type, target_id),
    INDEX idx_result (result),
    INDEX idx_created_at (created_at),
    INDEX idx_ip_address (ip_address)
) ENGINE=InnoDB COMMENT='GM操作日志表（审计）';

-- ========================================
-- 3. GM登录日志表
-- ========================================

CREATE TABLE IF NOT EXISTS gm_login_logs (
    log_id BIGINT PRIMARY KEY AUTO_INCREMENT COMMENT '日志ID',

    -- 登录信息
    gm_user VARCHAR(64) NOT NULL COMMENT 'GM用户名',
    gm_id INT COMMENT 'GM账号ID（登录成功时填写）',

    -- 登录结果
    login_result VARCHAR(16) NOT NULL COMMENT '登录结果：SUCCESS/FAILURE',
    failure_reason VARCHAR(64) COMMENT '失败原因：WRONG_PASSWORD/2FA_FAILED/ACCOUNT_LOCKED/etc',

    -- 2FA信息
    two_factor_used BOOLEAN DEFAULT FALSE COMMENT '是否使用了2FA',
    two_factor_result VARCHAR(16) COMMENT '2FA验证结果',

    -- 设备信息
    ip_address VARCHAR(45) NOT NULL COMMENT '登录IP',
    user_agent TEXT COMMENT 'User Agent',
    device_fingerprint VARCHAR(128) COMMENT '设备指纹',

    -- 时间信息
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '登录时间',

    INDEX idx_gm_user (gm_user),
    INDEX idx_login_result (login_result),
    INDEX idx_ip_address (ip_address),
    INDEX idx_created_at (created_at)
) ENGINE=InnoDB COMMENT='GM登录日志表';

-- ========================================
-- 4. 信任设备表（"记住此设备30天"功能）
-- ========================================

CREATE TABLE IF NOT EXISTS gm_trusted_devices (
    device_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '设备ID',

    -- 关联账号
    gm_user VARCHAR(64) NOT NULL COMMENT 'GM用户名',
    gm_id INT NOT NULL COMMENT 'GM账号ID',

    -- 设备信息
    device_fingerprint VARCHAR(128) NOT NULL COMMENT '设备指纹',
    device_token VARCHAR(64) UNIQUE NOT NULL COMMENT '设备Token（写入Cookie）',
    device_name VARCHAR(128) COMMENT '设备名称',

    -- IP信息
    ip_address VARCHAR(45) NOT NULL COMMENT '首次信任时的IP',
    user_agent TEXT COMMENT 'User Agent',

    -- 有效期
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    expires_at DATETIME NOT NULL DEFAULT '2099-12-31 23:59:59' COMMENT '过期时间',
    last_used_at TIMESTAMP NULL COMMENT '最后使用时间',

    -- 状态
    is_active BOOLEAN DEFAULT TRUE COMMENT '是否有效',

    INDEX idx_gm_user (gm_user),
    INDEX idx_device_token (device_token),
    INDEX idx_expires_at (expires_at),
    UNIQUE KEY uk_user_fingerprint (gm_user, device_fingerprint)
) ENGINE=InnoDB COMMENT='信任设备表';

-- ========================================
-- 5. GM权限定义表
-- ========================================

CREATE TABLE IF NOT EXISTS gm_permissions (
    permission_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '权限ID',

    -- 权限标识
    permission_key VARCHAR(64) UNIQUE NOT NULL COMMENT '权限键：player:read, item:grant, account:ban',
    permission_name VARCHAR(128) NOT NULL COMMENT '权限名称',
    permission_desc TEXT COMMENT '权限描述',

    -- 权限分类
    category VARCHAR(32) NOT NULL COMMENT '权限分类：PLAYER/ITEM/ACCOUNT/SYSTEM',
    required_level INT NOT NULL COMMENT '需要的最低GM等级',

    -- 风险等级
    risk_level VARCHAR(16) NOT NULL COMMENT '风险等级：LOW/MEDIUM/HIGH/CRITICAL',

    -- 状态
    is_active BOOLEAN DEFAULT TRUE COMMENT '是否激活',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',

    INDEX idx_category (category),
    INDEX idx_required_level (required_level)
) ENGINE=InnoDB COMMENT='GM权限定义表';

-- 插入默认权限
INSERT INTO gm_permissions (permission_key, permission_name, permission_desc, category, required_level, risk_level) VALUES
-- 玩家查询权限（低风险）
('player:read', '查询玩家信息', '查询角色基础信息、仓库、装备等', 'PLAYER', 1, 'LOW'),
('player:read:sensitive', '查询玩家敏感信息', '查询账号ID、IP地址等敏感信息', 'PLAYER', 2, 'MEDIUM'),

-- 物品管理权限（高风险）
('item:grant', '赠送物品', '向玩家赠送物品', 'ITEM', 3, 'HIGH'),
('item:delete', '删除物品', '删除玩家物品', 'ITEM', 4, 'CRITICAL'),

-- 账号管理权限（高风险）
('account:ban', '封禁账号', '封禁玩家账号', 'ACCOUNT', 4, 'CRITICAL'),
('account:unban', '解封账号', '解封玩家账号', 'ACCOUNT', 4, 'HIGH'),
('account:modify', '修改账号信息', '修改账号基础信息', 'ACCOUNT', 5, 'CRITICAL'),

-- 系统管理权限（最高风险）
('system:gm_manage', 'GM账号管理', '创建、修改、删除GM账号', 'SYSTEM', 5, 'CRITICAL'),
('system:config', '系统配置', '修改系统配置', 'SYSTEM', 5, 'CRITICAL'),
('system:logs', '查看日志', '查看操作日志和登录日志', 'SYSTEM', 2, 'MEDIUM');

-- ========================================
-- 6. GM角色表（可选，用于更细粒度的权限控制）
-- ========================================

CREATE TABLE IF NOT EXISTS gm_roles (
    role_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '角色ID',
    role_name VARCHAR(64) UNIQUE NOT NULL COMMENT '角色名称',
    role_desc TEXT COMMENT '角色描述',

    -- 权限列表（JSON数组）
    permissions JSON COMMENT '权限列表：["player:read", "item:grant"]',

    -- 状态
    is_active BOOLEAN DEFAULT TRUE COMMENT '是否激活',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',

    INDEX idx_role_name (role_name)
) ENGINE=InnoDB COMMENT='GM角色表';

-- 插入默认角色
INSERT INTO gm_roles (role_name, role_desc, permissions) VALUES
('viewer', '只读角色', '["player:read"]'),
('operator', '普通运营', '["player:read", "player:read:sensitive", "system:logs"]'),
('item_manager', '物品管理员', '["player:read", "player:read:sensitive", "item:grant"]'),
('account_manager', '账号管理员', '["player:read", "player:read:sensitive", "account:ban", "account:unban"]'),
('super_admin', '超级管理员', '["player:read", "player:read:sensitive", "item:grant", "item:delete", "account:ban", "account:unban", "account:modify", "system:gm_manage", "system:config", "system:logs"]');

-- ========================================
-- 7. 数据同步状态表
-- ========================================

CREATE TABLE IF NOT EXISTS canal_sync_status (
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT 'ID',

    -- 同步信息
    database_name VARCHAR(64) NOT NULL COMMENT '数据库名',
    table_name VARCHAR(64) NOT NULL COMMENT '表名',

    -- binlog位置
    binlog_file VARCHAR(128) COMMENT 'binlog文件名',
    binlog_position BIGINT COMMENT 'binlog位置',

    -- 同步状态
    sync_status VARCHAR(16) NOT NULL COMMENT '同步状态：RUNNING/STOPPED/ERROR',
    last_sync_time TIMESTAMP NULL COMMENT '最后同步时间',
    sync_delay_seconds INT COMMENT '同步延迟（秒）',

    -- 统计信息
    total_rows_synced BIGINT DEFAULT 0 COMMENT '总同步行数',
    total_sync_errors BIGINT DEFAULT 0 COMMENT '总错误数',

    -- 错误信息
    last_error_message TEXT COMMENT '最后错误信息',
    last_error_time TIMESTAMP NULL COMMENT '最后错误时间',

    -- 时间信息
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',

    UNIQUE KEY uk_db_table (database_name, table_name),
    INDEX idx_sync_status (sync_status),
    INDEX idx_last_sync_time (last_sync_time)
) ENGINE=InnoDB COMMENT='Canal同步状态表';

-- ========================================
-- 8. 系统配置表
-- ========================================

CREATE TABLE IF NOT EXISTS system_config (
    config_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '配置ID',

    -- 配置键值
    config_key VARCHAR(128) UNIQUE NOT NULL COMMENT '配置键',
    config_value TEXT NOT NULL COMMENT '配置值（支持JSON）',
    config_type VARCHAR(16) NOT NULL COMMENT '配置类型：STRING/INT/BOOLEAN/JSON',

    -- 描述
    config_desc TEXT COMMENT '配置描述',
    config_category VARCHAR(32) COMMENT '配置分类：SECURITY/FEATURE/SYSTEM',

    -- 是否敏感
    is_sensitive BOOLEAN DEFAULT FALSE COMMENT '是否为敏感配置（如密钥）',

    -- 审计信息
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    updated_by INT COMMENT '更新人GM ID',

    INDEX idx_category (config_category)
) ENGINE=InnoDB COMMENT='系统配置表';

-- 插入默认配置
INSERT INTO system_config (config_key, config_value, config_type, config_desc, config_category) VALUES
('jwt.expiration', '1800', 'INT', 'JWT过期时间（秒）', 'SECURITY'),
('2fa.required_level', '3', 'INT', '强制启用2FA的最低GM等级', 'SECURITY'),
('login.max_failed_attempts', '5', 'INT', '最大登录失败次数', 'SECURITY'),
('login.lockout_duration', '600', 'INT', '账号锁定时长（秒）', 'SECURITY'),
('audit.log_retention_days', '90', 'INT', '审计日志保留天数', 'SYSTEM'),
('feature.ip_whitelist_enabled', 'false', 'BOOLEAN', '是否启用IP白名单', 'FEATURE');

-- ========================================
-- 数据库初始化完成
-- ========================================

SELECT 'GM Tools Database Schema Created Successfully!' AS status;
