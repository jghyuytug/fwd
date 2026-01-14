-- ========================================
-- 添加通知表
-- Migration 001
-- 创建日期: 2025-11-11
-- ========================================

USE gm_tools;

-- ========================================
-- GM通知表
-- ========================================

CREATE TABLE IF NOT EXISTS gm_notifications (
    notification_id BIGINT PRIMARY KEY AUTO_INCREMENT COMMENT '通知ID',

    -- 通知内容
    type VARCHAR(16) NOT NULL COMMENT '通知类型：info/warning/error/success',
    title VARCHAR(256) NOT NULL COMMENT '通知标题',
    content TEXT NOT NULL COMMENT '通知内容',

    -- 通知来源（可选）
    source_type VARCHAR(32) COMMENT '来源类型：SYSTEM/LOGIN_FAILURE/OPERATION/etc',
    source_id VARCHAR(64) COMMENT '来源ID（如log_id）',

    -- 已读状态
    is_read BOOLEAN DEFAULT FALSE COMMENT '是否已读',
    read_at TIMESTAMP NULL COMMENT '已读时间',
    read_by_gm_id INT NULL COMMENT '标记已读的GM ID',

    -- 时间信息
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',

    INDEX idx_type (type),
    INDEX idx_is_read (is_read),
    INDEX idx_created_at (created_at),
    INDEX idx_source (source_type, source_id)
) ENGINE=InnoDB COMMENT='GM通知表';

-- 插入一些示例通知数据
INSERT INTO gm_notifications (type, title, content, source_type) VALUES
('warning', '登录失败警告', '检测到多次登录失败尝试，IP: 192.168.1.100', 'SYSTEM'),
('info', '系统更新通知', 'GM工具系统将于今晚22:00进行维护更新', 'SYSTEM'),
('success', '数据同步完成', 'Taiwan区数据同步已完成，耗时2.5秒', 'SYSTEM'),
('error', '数据库连接异常', 'Mirror数据库连接超时，请检查网络连接', 'SYSTEM');

SELECT 'GM Notifications Table Created Successfully!' AS status;
