-- ========================================
-- DNF重构版本 - 测试数据库建表SQL
-- ========================================
-- 基于: CREATE_CORE_TABLES.sql
-- 修改: 使用test_前缀避免影响生产环境
-- 生成日期: 2025-11-12
-- ========================================

-- ========================================
-- 数据库1: test_d_taiwan (测试账号库)
-- ========================================

USE `test_d_taiwan`;

-- ----------------------------------------
-- 表: accounts (主账号表)
-- ----------------------------------------
DROP TABLE IF EXISTS `accounts`;
CREATE TABLE `accounts` (
  `UID` int(11) NOT NULL AUTO_INCREMENT COMMENT '用户ID (主键)',
  `accountname` varchar(255) NOT NULL COMMENT '账号名 (登录用户名)',
  `password` varchar(255) NOT NULL COMMENT '密码 (MD5或加密)',
  `qq` varchar(255) DEFAULT NULL COMMENT 'QQ号绑定',
  `dzuid` int(8) DEFAULT NULL COMMENT '第三方平台ID',
  `billing` int(8) DEFAULT '0' COMMENT '计费状态 (0=免费, 1=付费)',
  `VIP` varchar(255) NOT NULL DEFAULT '' COMMENT 'VIP等级或状态',
  PRIMARY KEY (`UID`),
  UNIQUE KEY `idx_accountname` (`accountname`),
  KEY `idx_qq` (`qq`(50))
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 COMMENT='主账号表';

-- ----------------------------------------
-- 表: admin_member (管理员表)
-- ----------------------------------------
DROP TABLE IF EXISTS `admin_member`;
CREATE TABLE `admin_member` (
  `m_id` int(11) NOT NULL DEFAULT '0' COMMENT '账号ID (外键→accounts.UID)',
  `admin_level` tinyint(4) NOT NULL DEFAULT '0' COMMENT '管理员等级 (0-9)',
  `create_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '创建时间',
  `update_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '更新时间',
  PRIMARY KEY (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='管理员权限表';

-- ========================================
-- 数据库2: test_taiwan_cain (测试游戏数据)
-- ========================================

USE `test_taiwan_cain`;

-- ----------------------------------------
-- 表: charac_info (角色信息表) ⭐ 最核心
-- ----------------------------------------
DROP TABLE IF EXISTS `charac_info`;
CREATE TABLE `charac_info` (
  -- 基础信息
  `m_id` int(11) NOT NULL DEFAULT '0' COMMENT '账号ID',
  `charac_no` int(11) NOT NULL AUTO_INCREMENT COMMENT '角色ID (主键)',
  `charac_name` varchar(20) NOT NULL DEFAULT '' COMMENT '角色名 (唯一)',
  `village` tinyint(4) NOT NULL DEFAULT '1' COMMENT '城镇ID',
  `sex` tinyint(4) NOT NULL DEFAULT '1' COMMENT '性别 (0=男, 1=女)',

  -- 职业系统
  `job` tinyint(4) NOT NULL DEFAULT '0' COMMENT '基础职业',
  `grow_type` tinyint(4) NOT NULL DEFAULT '0' COMMENT '成长类型',
  `expert_job` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '进阶职业',
  `skill_tree_index` tinyint(4) NOT NULL DEFAULT '-1' COMMENT '技能树索引',

  -- 等级经验
  `lev` tinyint(4) NOT NULL DEFAULT '1' COMMENT '等级',
  `exp` int(11) NOT NULL DEFAULT '0' COMMENT '经验值',

  -- 生命魔法值
  `HP` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '当前HP百分比',
  `maxHP` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '最大生命值',
  `maxMP` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '最大魔法值',

  -- 攻击防御属性
  `phy_attack` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '物理攻击力',
  `phy_defense` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '物理防御力',
  `mag_attack` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '魔法攻击力',
  `mag_defense` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '魔法防御力',

  -- 特殊属性 (BLOB)
  `element_resist` tinyblob NOT NULL COMMENT '元素抗性',
  `spec_property` tinyblob NOT NULL COMMENT '特殊属性',

  -- 速度相关
  `move_speed` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '移动速度',
  `attack_speed` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '攻击速度',
  `cast_speed` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '施法速度',
  `hit_recovery` smallint(6) NOT NULL DEFAULT '0' COMMENT '受击恢复',
  `jump` smallint(6) NOT NULL DEFAULT '0' COMMENT '跳跃力',

  -- 负重系统
  `inven_weight` int(6) NOT NULL DEFAULT '0' COMMENT '背包当前负重',
  `charac_weight` int(11) NOT NULL DEFAULT '0' COMMENT '角色最大负重',

  -- 疲劳度系统
  `fatigue` smallint(6) NOT NULL DEFAULT '0' COMMENT '当前疲劳度',
  `max_fatigue` smallint(6) NOT NULL DEFAULT '70' COMMENT '最大疲劳度',
  `premium_fatigue` smallint(6) NOT NULL DEFAULT '0' COMMENT '会员额外疲劳度',
  `max_premium_fatigue` smallint(6) NOT NULL DEFAULT '0' COMMENT '会员最大额外疲劳',

  -- 时间戳
  `create_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '角色创建时间',
  `last_play_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '最后登录时间',
  `delete_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '删除时间',
  `delete_flag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '删除标记',

  -- 公会系统
  `guild_id` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '公会ID',
  `guild_right` tinyint(4) NOT NULL DEFAULT '0' COMMENT '公会权限',
  `guild_secede` tinyint(2) NOT NULL DEFAULT '0' COMMENT '公会退出标记',
  `member_flag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '成员标记',

  -- 副本积分
  `dungeon_clear_point` int(11) NOT NULL DEFAULT '0' COMMENT '副本通关积分',

  -- 其他
  `link_charac_no` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '关联角色ID',
  `event_charac_level` tinyint(4) NOT NULL DEFAULT '0' COMMENT '活动等级',
  `VIP` varchar(255) NOT NULL DEFAULT '' COMMENT 'VIP状态',

  -- 竞技场相关
  `start_time` int(11) NOT NULL DEFAULT '0' COMMENT '竞技开始时间',
  `finish_time` int(11) NOT NULL DEFAULT '0' COMMENT '竞技结束时间',
  `competition_area` tinyint(2) NOT NULL DEFAULT '-1' COMMENT '竞技场区域',
  `competition_period` tinyint(2) NOT NULL DEFAULT '-1' COMMENT '竞技场周期',
  `mercenary_start_time` int(11) NOT NULL DEFAULT '0' COMMENT '佣兵开始时间',
  `mercenary_finish_time` int(11) NOT NULL DEFAULT '0' COMMENT '佣兵结束时间',
  `mercenary_area` tinyint(4) NOT NULL DEFAULT '-1' COMMENT '佣兵区域',
  `mercenary_period` tinyint(4) NOT NULL DEFAULT '-1' COMMENT '佣兵周期',

  -- 索引定义
  PRIMARY KEY (`charac_no`),
  UNIQUE KEY `idx_charac_name` USING BTREE (`charac_name`),
  KEY `idx_m_id` USING BTREE (`m_id`),
  KEY `idx_exp` USING BTREE (`exp`),
  KEY `idx_guild_id` USING BTREE (`guild_id`),
  KEY `idx_delete_flag` (`delete_flag`),
  KEY `idx_last_play_time` (`last_play_time`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 COMMENT='角色信息表';

-- ----------------------------------------
-- 表: account_cargo (账号仓库)
-- ----------------------------------------
DROP TABLE IF EXISTS `account_cargo`;
CREATE TABLE `account_cargo` (
  `m_id` int(11) NOT NULL DEFAULT '0' COMMENT '账号ID (主键)',
  `cargo_data` blob COMMENT '仓库数据',
  `update_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '更新时间',
  PRIMARY KEY (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='账号仓库表';

-- ========================================
-- 数据库3: test_taiwan_billing (测试计费)
-- ========================================

USE `test_taiwan_billing`;

-- ----------------------------------------
-- 表: cash_cera (点券余额表)
-- ----------------------------------------
DROP TABLE IF EXISTS `cash_cera`;
CREATE TABLE `cash_cera` (
  `account_id` int(11) NOT NULL COMMENT '账号ID (主键)',
  `cera_point` bigint(20) NOT NULL DEFAULT '0' COMMENT '点券余额',
  `update_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '更新时间',
  PRIMARY KEY (`account_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='点券余额表';

-- ========================================
-- 数据库4: test_taiwan_login (测试登录)
-- ========================================

USE `test_taiwan_login`;

-- ----------------------------------------
-- 表: login_log (登录日志)
-- ----------------------------------------
DROP TABLE IF EXISTS `login_log`;
CREATE TABLE `login_log` (
  `seq` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '流水号 (主键)',
  `account_id` int(11) NOT NULL COMMENT '账号ID',
  `login_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '登录时间',
  `logout_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '登出时间',
  `login_ip` varchar(50) NOT NULL DEFAULT '' COMMENT '登录IP',
  `server_id` tinyint(4) NOT NULL DEFAULT '0' COMMENT '登录服务器ID',
  PRIMARY KEY (`seq`),
  KEY `idx_account_id` (`account_id`),
  KEY `idx_login_time` (`login_time`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 COMMENT='登录日志表';

-- ========================================
-- 数据库5: test_d_guild (测试公会)
-- ========================================

USE `test_d_guild`;

-- ----------------------------------------
-- 表: guild_info (公会信息表)
-- ----------------------------------------
DROP TABLE IF EXISTS `guild_info`;
CREATE TABLE `guild_info` (
  `guild_id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '公会ID (主键)',
  `guild_name` varchar(50) NOT NULL DEFAULT '' COMMENT '公会名称 (唯一)',
  `guild_level` tinyint(4) NOT NULL DEFAULT '1' COMMENT '公会等级',
  `guild_exp` int(11) NOT NULL DEFAULT '0' COMMENT '公会经验',
  `guild_master_charac_no` int(11) NOT NULL COMMENT '会长角色ID',
  `guild_notice` varchar(500) NOT NULL DEFAULT '' COMMENT '公会公告',
  `member_count` smallint(6) NOT NULL DEFAULT '1' COMMENT '成员数量',
  `max_member_count` smallint(6) NOT NULL DEFAULT '20' COMMENT '最大成员数',
  `create_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '创建时间',
  `update_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '更新时间',
  PRIMARY KEY (`guild_id`),
  UNIQUE KEY `idx_guild_name` (`guild_name`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 COMMENT='公会信息表';

-- ========================================
-- 完成
-- ========================================
