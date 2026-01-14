-- ========================================
-- DNF重构版本 - 核心数据库建表SQL
-- ========================================
-- 生成日期: 2025-11-11
-- 来源: 生产服务器 192.168.200.131
-- MySQL版本: 5.0.95+
-- 字符集: utf8
-- 引擎: InnoDB (推荐) / MyISAM
-- ========================================

-- ========================================
-- 数据库1: d_taiwan (主账号库)
-- ========================================

CREATE DATABASE IF NOT EXISTS `d_taiwan` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

USE `d_taiwan`;

-- ----------------------------------------
-- 表: accounts (主账号表)
-- 用途: 存储用户登录账号信息
-- 关键字段: UID (主键), accountname (登录名), password (密码)
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
-- 用途: GM账号权限管理
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
-- 数据库2: taiwan_cain (核心游戏数据)
-- ========================================

CREATE DATABASE IF NOT EXISTS `taiwan_cain` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

USE `taiwan_cain`;

-- ----------------------------------------
-- 表: charac_info (角色信息表) ⭐ 最核心
-- 用途: 存储所有角色的基础信息和属性
-- 关键字段: charac_no (主键), m_id (外键→accounts.UID), charac_name (唯一)
-- ----------------------------------------
DROP TABLE IF EXISTS `charac_info`;
CREATE TABLE `charac_info` (
  -- 基础信息
  `m_id` int(11) NOT NULL DEFAULT '0' COMMENT '账号ID (外键→d_taiwan.accounts.UID)',
  `charac_no` int(11) NOT NULL AUTO_INCREMENT COMMENT '角色ID (主键, 全局唯一)',
  `charac_name` varchar(20) NOT NULL DEFAULT '' COMMENT '角色名 (唯一, 2-20字符)',
  `village` tinyint(4) NOT NULL DEFAULT '1' COMMENT '城镇ID (出生地/当前城镇)',
  `sex` tinyint(4) NOT NULL DEFAULT '1' COMMENT '性别 (0=男, 1=女)',

  -- 职业系统
  `job` tinyint(4) NOT NULL DEFAULT '0' COMMENT '基础职业 (0-15)',
  `grow_type` tinyint(4) NOT NULL DEFAULT '0' COMMENT '成长类型 (转职方向)',
  `expert_job` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '进阶职业 (二次觉醒等)',
  `skill_tree_index` tinyint(4) NOT NULL DEFAULT '-1' COMMENT '技能树索引 (-1=默认)',

  -- 等级经验
  `lev` tinyint(4) NOT NULL DEFAULT '1' COMMENT '等级 (1-MAX_LEVEL)',
  `exp` int(11) NOT NULL DEFAULT '0' COMMENT '经验值',

  -- 生命魔法值
  `HP` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '当前HP百分比 (0-100)',
  `maxHP` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '最大生命值',
  `maxMP` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '最大魔法值',

  -- 攻击防御属性
  `phy_attack` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '物理攻击力',
  `phy_defense` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '物理防御力',
  `mag_attack` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '魔法攻击力',
  `mag_defense` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '魔法防御力',

  -- 特殊属性 (BLOB存储复杂数据结构)
  `element_resist` tinyblob NOT NULL COMMENT '元素抗性 (火/冰/光/暗等, 二进制数据)',
  `spec_property` tinyblob NOT NULL COMMENT '特殊属性 (buff/状态等, 二进制数据)',

  -- 速度相关
  `move_speed` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '移动速度',
  `attack_speed` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '攻击速度',
  `cast_speed` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '施法速度',
  `hit_recovery` smallint(6) NOT NULL DEFAULT '0' COMMENT '受击恢复/硬直',
  `jump` smallint(6) NOT NULL DEFAULT '0' COMMENT '跳跃力',

  -- 负重系统
  `inven_weight` int(6) NOT NULL DEFAULT '0' COMMENT '背包当前负重',
  `charac_weight` int(11) NOT NULL DEFAULT '0' COMMENT '角色最大负重',

  -- 疲劳度系统
  `fatigue` smallint(6) NOT NULL DEFAULT '0' COMMENT '当前疲劳度',
  `max_fatigue` smallint(6) NOT NULL DEFAULT '70' COMMENT '最大疲劳度 (默认70)',
  `premium_fatigue` smallint(6) NOT NULL DEFAULT '0' COMMENT '会员额外疲劳度',
  `max_premium_fatigue` smallint(6) NOT NULL DEFAULT '0' COMMENT '会员最大额外疲劳',

  -- 时间戳
  `create_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '角色创建时间',
  `last_play_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '最后登录时间',
  `delete_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '删除时间 (软删除)',
  `delete_flag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '删除标记 (0=正常, 1=已删除)',

  -- 公会系统
  `guild_id` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '公会ID (0=无公会)',
  `guild_right` tinyint(4) NOT NULL DEFAULT '0' COMMENT '公会权限 (0=成员, 1=管理, 2=会长)',
  `guild_secede` tinyint(2) NOT NULL DEFAULT '0' COMMENT '公会退出标记',
  `member_flag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '成员标记',

  -- 副本积分
  `dungeon_clear_point` int(11) NOT NULL DEFAULT '0' COMMENT '副本通关积分',

  -- 其他
  `link_charac_no` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '关联角色ID (转职/继承)',
  `event_charac_level` tinyint(4) NOT NULL DEFAULT '0' COMMENT '活动等级',
  `VIP` varchar(255) NOT NULL DEFAULT '' COMMENT 'VIP状态',

  -- 竞技场相关
  `start_time` int(11) NOT NULL DEFAULT '0' COMMENT '竞技开始时间 (UNIX时间戳)',
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
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 COMMENT='角色信息表 (核心)';

-- ----------------------------------------
-- 表: account_cargo (账号仓库)
-- 用途: 存储账号级别的共享仓库
-- ----------------------------------------
DROP TABLE IF EXISTS `account_cargo`;
CREATE TABLE `account_cargo` (
  `m_id` int(11) NOT NULL DEFAULT '0' COMMENT '账号ID (主键)',
  `cargo_data` blob COMMENT '仓库数据 (二进制, 存储物品列表)',
  `update_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '更新时间',
  PRIMARY KEY (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='账号仓库表';

-- ----------------------------------------
-- 表: charac_stat (角色属性扩展表)
-- 用途: 存储角色的详细属性数据
-- ----------------------------------------
DROP TABLE IF EXISTS `charac_stat`;
CREATE TABLE `charac_stat` (
  `charac_no` int(11) NOT NULL COMMENT '角色ID (主键, 外键→charac_info.charac_no)',
  `stat_data` blob COMMENT '属性数据 (二进制, 包含力量/智力/体力/精神等)',
  `update_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '更新时间',
  PRIMARY KEY (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='角色详细属性表';

-- ----------------------------------------
-- 表: charac_quest (角色任务)
-- 用途: 存储角色的任务进度
-- ----------------------------------------
DROP TABLE IF EXISTS `charac_quest`;
CREATE TABLE `charac_quest` (
  `charac_no` int(11) NOT NULL COMMENT '角色ID',
  `quest_id` int(11) NOT NULL COMMENT '任务ID',
  `quest_status` tinyint(4) NOT NULL DEFAULT '0' COMMENT '任务状态 (0=未接, 1=进行中, 2=已完成)',
  `quest_data` blob COMMENT '任务进度数据 (二进制)',
  `start_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '接取时间',
  `finish_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '完成时间',
  PRIMARY KEY (`charac_no`, `quest_id`),
  KEY `idx_quest_status` (`quest_status`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='角色任务表';

-- ----------------------------------------
-- 表: charac_dungeon (角色副本进度)
-- 用途: 存储角色的副本通关记录
-- ----------------------------------------
DROP TABLE IF EXISTS `charac_dungeon`;
CREATE TABLE `charac_dungeon` (
  `charac_no` int(11) NOT NULL COMMENT '角色ID',
  `dungeon_id` int(11) NOT NULL COMMENT '副本ID',
  `clear_count` int(11) NOT NULL DEFAULT '0' COMMENT '通关次数',
  `best_time` int(11) NOT NULL DEFAULT '0' COMMENT '最佳通关时间 (秒)',
  `best_rank` tinyint(4) NOT NULL DEFAULT '0' COMMENT '最佳评级 (S/A/B/C)',
  `last_clear_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '最后通关时间',
  PRIMARY KEY (`charac_no`, `dungeon_id`),
  KEY `idx_dungeon_id` (`dungeon_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='角色副本进度表';

-- ----------------------------------------
-- 表: charac_friends (角色好友列表)
-- 用途: 存储角色的好友关系
-- ----------------------------------------
DROP TABLE IF EXISTS `charac_friends`;
CREATE TABLE `charac_friends` (
  `charac_no` int(11) NOT NULL COMMENT '角色ID',
  `friend_charac_no` int(11) NOT NULL COMMENT '好友角色ID',
  `friend_type` tinyint(4) NOT NULL DEFAULT '0' COMMENT '好友类型 (0=普通, 1=亲密, 2=黑名单)',
  `add_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '添加时间',
  PRIMARY KEY (`charac_no`, `friend_charac_no`),
  KEY `idx_friend_charac_no` (`friend_charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='角色好友表';

-- ========================================
-- 数据库3: taiwan_billing (计费系统)
-- ========================================

CREATE DATABASE IF NOT EXISTS `taiwan_billing` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

USE `taiwan_billing`;

-- ----------------------------------------
-- 表: cash_cera (点券余额表)
-- 用途: 存储账号的点券余额
-- ----------------------------------------
DROP TABLE IF EXISTS `cash_cera`;
CREATE TABLE `cash_cera` (
  `account_id` int(11) NOT NULL COMMENT '账号ID (主键)',
  `cera_point` bigint(20) NOT NULL DEFAULT '0' COMMENT '点券余额',
  `update_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '更新时间',
  PRIMARY KEY (`account_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='点券余额表';

-- ----------------------------------------
-- 表: cash_cera_point_log (点券消费日志)
-- 用途: 记录点券的所有交易
-- ----------------------------------------
DROP TABLE IF EXISTS `cash_cera_point_log`;
CREATE TABLE `cash_cera_point_log` (
  `seq` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '流水号 (主键)',
  `account_id` int(11) NOT NULL COMMENT '账号ID',
  `cera_change` int(11) NOT NULL COMMENT '点券变化 (正=充值, 负=消费)',
  `cera_balance` bigint(20) NOT NULL COMMENT '变化后余额',
  `reason` varchar(100) NOT NULL DEFAULT '' COMMENT '交易原因',
  `create_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '交易时间',
  PRIMARY KEY (`seq`),
  KEY `idx_account_id` (`account_id`),
  KEY `idx_create_time` (`create_time`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 COMMENT='点券消费日志';

-- ========================================
-- 数据库4: taiwan_login (登录系统)
-- ========================================

CREATE DATABASE IF NOT EXISTS `taiwan_login` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

USE `taiwan_login`;

-- ----------------------------------------
-- 表: login_log (登录日志)
-- 用途: 记录所有登录行为
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
-- 数据库5: d_guild (公会系统)
-- ========================================

CREATE DATABASE IF NOT EXISTS `d_guild` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

USE `d_guild`;

-- ----------------------------------------
-- 表: guild_info (公会信息表)
-- 用途: 存储公会的基本信息
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

-- 总计:
-- 5个数据库
-- 14张核心表
--
-- 注意事项:
-- 1. AUTO_INCREMENT从1开始，生产环境请根据实际调整
-- 2. BLOB字段用于存储复杂的二进制数据，需要在代码层序列化/反序列化
-- 3. 所有datetime字段默认值为 '0000-00-00 00:00:00'，建议在代码中使用CURRENT_TIMESTAMP
-- 4. 索引设计已优化，但请根据实际查询模式调整
-- 5. 外键约束未添加，建议在应用层保证数据一致性
--
-- 下一步:
-- 1. 执行本SQL文件创建数据库和表
-- 2. 在src/database/中实现ORM封装
-- 3. 在src/user/中实现用户管理业务逻辑
-- 4. 测试数据库连接和基本CRUD操作
