
CREATE DATABASE IF NOT EXISTS d_gmaster CHARACTER SET utf8 COLLATE utf8_general_ci;
CREATE DATABASE IF NOT EXISTS taiwan_login CHARACTER SET utf8 COLLATE utf8_general_ci;
CREATE DATABASE IF NOT EXISTS taiwan_billing CHARACTER SET utf8 COLLATE utf8_general_ci;
CREATE DATABASE IF NOT EXISTS taiwan_cain_2nd CHARACTER SET utf8 COLLATE utf8_general_ci;

-- 创建用户并授权 - 权限设置（来源已归档）
CREATE USER IF NOT EXISTS 'game'@'localhost' IDENTIFIED BY 'uu5!^%jg';
CREATE USER IF NOT EXISTS 'game'@'%' IDENTIFIED BY 'uu5!^%jg';
GRANT ALL PRIVILEGES ON *.* TO 'game'@'localhost';
GRANT ALL PRIVILEGES ON *.* TO 'game'@'%';
FLUSH PRIVILEGES;

USE d_taiwan;

CREATE TABLE IF NOT EXISTS accounts (
    UID int(11) NOT NULL AUTO_INCREMENT,
    accountname varchar(50) NOT NULL,
    password varchar(255) NOT NULL,
    qq varchar(20) DEFAULT NULL,
    reg_date datetime DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (UID),
    UNIQUE KEY accountname (accountname)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS limit_create_character (
    m_id int(11) NOT NULL,
    count int(11) DEFAULT 0,
    PRIMARY KEY (m_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS member_info (
    m_id int(11) NOT NULL,
    user_id int(11) NOT NULL,
    PRIMARY KEY (m_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS member_join_info (
    m_id int(11) NOT NULL,
    join_date datetime DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (m_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS member_miles (
    m_id int(11) NOT NULL,
    miles int(11) DEFAULT 0,
    PRIMARY KEY (m_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS member_white_account (
    m_id int(11) NOT NULL,
    PRIMARY KEY (m_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS member_punish_info (
    m_id int(11) NOT NULL,
    punish_type int(11) DEFAULT 0,
    punish_date datetime DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (m_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

USE d_gmaster;
CREATE TABLE IF NOT EXISTS login (
    uid int(11) NOT NULL,
    reg_time datetime DEFAULT CURRENT_TIMESTAMP,
    login_time datetime DEFAULT CURRENT_TIMESTAMP,
    login_num int(11) DEFAULT 1,
    mac varchar(50) DEFAULT NULL,
    ip varchar(50) DEFAULT NULL,
    PRIMARY KEY (uid)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

USE taiwan_login;
CREATE TABLE IF NOT EXISTS member_login (
    m_id int(11) NOT NULL,
    PRIMARY KEY (m_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

USE taiwan_billing;
CREATE TABLE IF NOT EXISTS cash_cera (
    account int(11) NOT NULL,
    cera int(11) DEFAULT 10000,
    mod_date datetime DEFAULT CURRENT_TIMESTAMP,
    reg_date datetime DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (account)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS cash_cera_point (
    account int(11) NOT NULL,
    cera_point int(11) DEFAULT 10000,
    reg_date datetime DEFAULT CURRENT_TIMESTAMP,
    mod_date datetime DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (account)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

USE taiwan_cain_2nd;
CREATE TABLE IF NOT EXISTS member_avatar_coin (
    m_id int(11) NOT NULL,
    avatar_coin int(11) DEFAULT 0,
    PRIMARY KEY (m_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

USE d_taiwan;
INSERT IGNORE INTO accounts (UID, accountname, password, qq) VALUES 
(1000, 'admin', 'admin123', '123456789'),
(1001, 'test', 'test123', '987654321');

INSERT IGNORE INTO limit_create_character (m_id) VALUES (1000), (1001);
INSERT IGNORE INTO member_info (m_id, user_id) VALUES (1000, 1000), (1001, 1001);
INSERT IGNORE INTO member_join_info (m_id) VALUES (1000), (1001);
INSERT IGNORE INTO member_miles (m_id) VALUES (1000), (1001);
INSERT IGNORE INTO member_white_account (m_id) VALUES (1000), (1001);

USE taiwan_login;
INSERT IGNORE INTO member_login (m_id) VALUES (1000), (1001);

USE taiwan_billing;
INSERT IGNORE INTO cash_cera (account, cera) VALUES (1000, 50000), (1001, 50000);
INSERT IGNORE INTO cash_cera_point (account, cera_point) VALUES (1000, 50000), (1001, 50000);

USE taiwan_cain_2nd;
INSERT IGNORE INTO member_avatar_coin (m_id) VALUES (1000), (1001);