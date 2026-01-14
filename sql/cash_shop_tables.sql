-- Cash Shop Tables for d_taiwan database
-- 点券商城相关表

-- 点券余额表
CREATE TABLE IF NOT EXISTS cash_balance (
    account_id INT UNSIGNED PRIMARY KEY,
    cash INT UNSIGNED NOT NULL DEFAULT 0,
    bind_cash INT UNSIGNED NOT NULL DEFAULT 0,
    vip_level TINYINT UNSIGNED NOT NULL DEFAULT 0,
    vip_expire DATETIME DEFAULT NULL,
    create_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    update_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 点券消费日志
CREATE TABLE IF NOT EXISTS cash_shop_log (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    account_id INT UNSIGNED NOT NULL,
    type TINYINT NOT NULL COMMENT '1=购买 2=充值 3=赠送 4=退款',
    item_id INT UNSIGNED NOT NULL DEFAULT 0,
    price INT UNSIGNED NOT NULL DEFAULT 0,
    charac_no INT UNSIGNED NOT NULL DEFAULT 0,
    create_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_account (account_id),
    INDEX idx_time (create_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 初始化测试数据 (给账号1添加10000点券)
INSERT INTO cash_balance (account_id, cash, bind_cash, vip_level, vip_expire)
VALUES (1, 10000, 5000, 1, DATE_ADD(NOW(), INTERVAL 30 DAY))
ON DUPLICATE KEY UPDATE cash = 10000, bind_cash = 5000;
