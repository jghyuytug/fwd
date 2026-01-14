-- MySQL dump 10.11
--
-- Host: localhost    Database: 
-- ------------------------------------------------------
-- Server version	5.0.95

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES latin1 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Current Database: `d_channel`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `d_channel` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `d_channel`;

--
-- Table structure for table `ch_dungeon_data`
--

DROP TABLE IF EXISTS `ch_dungeon_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ch_dungeon_data` (
  `kind_name` varchar(20) NOT NULL default '',
  `dungeon_name` varchar(30) NOT NULL default ''
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ch_dungeon_data_old`
--

DROP TABLE IF EXISTS `ch_dungeon_data_old`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ch_dungeon_data_old` (
  `kind_name` varchar(20) character set sjis NOT NULL default '',
  `dungeon_name` varchar(30) character set sjis NOT NULL default '',
  PRIMARY KEY  (`kind_name`,`dungeon_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ch_dungeon_list`
--

DROP TABLE IF EXISTS `ch_dungeon_list`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ch_dungeon_list` (
  `kind_name` varchar(20) NOT NULL default '',
  `dungeon_id` smallint(5) unsigned NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ch_dungeon_list_old`
--

DROP TABLE IF EXISTS `ch_dungeon_list_old`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ch_dungeon_list_old` (
  `kind_name` varchar(20) character set sjis NOT NULL default '',
  `dungeon_id` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`dungeon_id`),
  KEY `idx_kind_name` USING BTREE (`kind_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ch_gc_info`
--

DROP TABLE IF EXISTS `ch_gc_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ch_gc_info` (
  `group_name` varchar(20) NOT NULL default '',
  `group_gc_no` varchar(10) NOT NULL default ''
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ch_script_version`
--

DROP TABLE IF EXISTS `ch_script_version`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ch_script_version` (
  `script_version` varchar(10) character set sjis NOT NULL default ''
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ch_server_data`
--

DROP TABLE IF EXISTS `ch_server_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ch_server_data` (
  `id` int(11) NOT NULL auto_increment,
  `server_id` tinyint(3) unsigned NOT NULL default '0',
  `is_use` tinyint(4) NOT NULL default '0',
  `channel_number` smallint(5) unsigned NOT NULL default '0',
  `channel_name` varchar(30) NOT NULL default '',
  `channel_kind` tinyint(3) unsigned NOT NULL default '0',
  `kind_name` varchar(20) NOT NULL default '',
  `exp_rate` float NOT NULL default '0',
  `exp_1` float NOT NULL default '0',
  `exp_2` float NOT NULL default '0',
  `exp_3` float NOT NULL default '0',
  `exp_4` float NOT NULL default '0',
  `exp_5` float NOT NULL default '0',
  `exp_6` float NOT NULL default '0',
  `exp_7` float NOT NULL default '0',
  `exp_8` float NOT NULL default '0',
  `exp_9` float NOT NULL default '0',
  `exp_10` float NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `server_id` USING BTREE (`server_id`,`channel_number`),
  KEY `idx_channel_number` USING BTREE (`channel_number`),
  KEY `idx_is_use` USING BTREE (`is_use`)
) ENGINE=InnoDB AUTO_INCREMENT=102 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Current Database: `d_guild`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `d_guild` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `d_guild`;

--
-- Table structure for table `guild_bbs`
--

DROP TABLE IF EXISTS `guild_bbs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_bbs` (
  `gno` int(11) NOT NULL auto_increment,
  `bd_id` tinyint(4) NOT NULL default '0',
  `empyn` tinyint(4) NOT NULL default '0',
  `mgno` int(11) NOT NULL default '0',
  `open` tinyint(1) NOT NULL default '1',
  `main` tinyint(4) NOT NULL default '0',
  `reg_date` int(11) NOT NULL default '0',
  `mod_date` int(11) NOT NULL default '0',
  `hits` mediumint(8) unsigned NOT NULL default '0',
  `body_type` char(1) NOT NULL default '',
  `m_id` int(11) NOT NULL default '0',
  `reg_id` varchar(20) NOT NULL default '',
  `subject` varchar(50) NOT NULL default '',
  PRIMARY KEY  (`gno`),
  UNIQUE KEY `uk_bdid_empyn_gno` USING BTREE (`bd_id`,`empyn`,`gno`),
  UNIQUE KEY `uk_bdid_empyn_mgno` USING BTREE (`bd_id`,`empyn`,`mgno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_body`
--

DROP TABLE IF EXISTS `guild_body`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_body` (
  `gno` int(11) NOT NULL default '0',
  `body` text NOT NULL,
  PRIMARY KEY  (`gno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_event`
--

DROP TABLE IF EXISTS `guild_event`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_event` (
  `gno` int(11) NOT NULL default '0',
  `stt_date` date NOT NULL default '0000-00-00',
  `end_date` date NOT NULL default '0000-00-00',
  `ann_date` date NOT NULL default '0000-00-00',
  `page_url` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`gno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_exp_ref`
--

DROP TABLE IF EXISTS `guild_exp_ref`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_exp_ref` (
  `grade` int(11) NOT NULL default '0',
  `exp` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`grade`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_files`
--

DROP TABLE IF EXISTS `guild_files`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_files` (
  `gno` int(11) NOT NULL default '0',
  `gf_no` tinyint(4) NOT NULL auto_increment,
  `file_server` varchar(50) NOT NULL default '',
  `file_location` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`gno`,`gf_no`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_grade_log`
--

DROP TABLE IF EXISTS `guild_grade_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_grade_log` (
  `id` int(11) NOT NULL auto_increment,
  `guild_id` int(11) NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '1',
  `charac_no` int(11) NOT NULL default '0',
  `charac_name` varchar(20) NOT NULL default '',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `grade_prev` tinyint(4) NOT NULL default '0',
  `grade_next` tinyint(4) NOT NULL default '0',
  `reason` varchar(40) NOT NULL default '',
  `admin_no` int(11) default NULL,
  `admin_name` varchar(20) NOT NULL default '',
  PRIMARY KEY  (`id`),
  KEY `idx1` USING BTREE (`guild_id`),
  KEY `idx2` USING BTREE (`server_id`,`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_halloffame`
--

DROP TABLE IF EXISTS `guild_halloffame`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_halloffame` (
  `fame_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `guild_id` int(11) NOT NULL default '0',
  `guild_name` varchar(40) NOT NULL default '',
  `file_url` varchar(128) NOT NULL default '',
  `open_flag` tinyint(4) NOT NULL default '0',
  `main_flag` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`fame_id`,`server_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_halloffame_html`
--

DROP TABLE IF EXISTS `guild_halloffame_html`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_halloffame_html` (
  `fame_id` int(11) NOT NULL default '0',
  `title` varchar(100) NOT NULL default '',
  `html` text NOT NULL,
  PRIMARY KEY  (`fame_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_info`
--

DROP TABLE IF EXISTS `guild_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_info` (
  `guild_id` int(11) NOT NULL auto_increment,
  `server_id` tinyint(4) NOT NULL default '0',
  `guild_name` varchar(40) NOT NULL default '',
  `master_id` int(11) NOT NULL default '0',
  `master_no` int(11) NOT NULL default '0',
  `master_name` varchar(20) NOT NULL default '',
  `guild_url` varchar(40) NOT NULL default '',
  `guild_icon` tinyint(3) unsigned NOT NULL default '0',
  `create_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `lev` int(11) NOT NULL default '0',
  `ability` tinyint(4) NOT NULL default '0',
  `expire_flag` tinyint(4) NOT NULL default '0',
  `expire_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `member_secede_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `member_count` int(11) NOT NULL default '0',
  `recommend_flag` tinyint(4) NOT NULL default '0',
  `recommend_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `guild_point` int(10) unsigned NOT NULL default '0',
  `guild_point_acc` int(10) unsigned NOT NULL default '0',
  `guild_point_prev` int(10) unsigned NOT NULL default '0',
  `guild_rank` int(10) unsigned NOT NULL default '0',
  `guild_war_point` int(10) unsigned NOT NULL default '0',
  `final_entry` smallint(5) unsigned NOT NULL default '0',
  `final_win` smallint(5) unsigned NOT NULL default '0',
  `guild_icon_auth` tinyint(4) NOT NULL default '0',
  `guild_exp` int(10) unsigned NOT NULL default '0',
  `power_side` tinyint(4) NOT NULL default '0',
  `guild_agit_flag` tinyint(4) NOT NULL default '0',
  `lev_up_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `power_secede_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `power_war_point` int(10) unsigned NOT NULL default '0',
  `power_join_count` tinyint(3) unsigned NOT NULL default '0',
  `guild_fund` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`guild_id`),
  KEY `idx_server_id` USING BTREE (`server_id`),
  KEY `idx_guild_name` USING BTREE (`guild_name`),
  KEY `idx_master_no` USING BTREE (`master_no`),
  KEY `idx_master_name` USING BTREE (`master_name`),
  KEY `idx_guild_point_prev` USING BTREE (`guild_point_prev`),
  KEY `idx_guild_point_acc` USING BTREE (`guild_point_acc`),
  KEY `idx_member_count` USING BTREE (`member_count`),
  KEY `idx_expire_flag` USING BTREE (`expire_flag`),
  KEY `idx_guild_point` USING BTREE (`guild_point`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_introduce`
--

DROP TABLE IF EXISTS `guild_introduce`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_introduce` (
  `guild_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `introduce` varchar(200) NOT NULL default '',
  PRIMARY KEY  (`guild_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_join_list`
--

DROP TABLE IF EXISTS `guild_join_list`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_join_list` (
  `guild_id` int(10) unsigned NOT NULL default '0',
  `charac_no` int(10) unsigned NOT NULL default '0',
  `server_group` tinyint(4) NOT NULL default '0',
  `m_id` int(10) unsigned NOT NULL default '0',
  `born_year` varchar(2) NOT NULL default '',
  `memo` varchar(25) NOT NULL default '',
  `occ_time` datetime default '0000-00-00 00:00:00',
  PRIMARY KEY  (`guild_id`,`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_member`
--

DROP TABLE IF EXISTS `guild_member`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_member` (
  `guild_id` int(11) NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `charac_name` varchar(20) NOT NULL default '',
  `memo` varchar(30) NOT NULL default '',
  `grade` tinyint(4) NOT NULL default '0',
  `job` tinyint(4) NOT NULL default '0',
  `grow_type` tinyint(4) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '0',
  `age` tinyint(4) NOT NULL default '0',
  `born_year` varchar(2) NOT NULL default '',
  `sex` char(1) NOT NULL default '',
  `apply_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `member_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `member_flag` tinyint(4) NOT NULL default '0',
  `bbs_cnt` smallint(5) unsigned NOT NULL default '0',
  `last_visit_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `secede_type` tinyint(4) NOT NULL default '0',
  `secede_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `member_point` int(10) unsigned NOT NULL default '0',
  `member_point_prev` int(10) unsigned NOT NULL default '0',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `nick_name` varchar(12) NOT NULL default '',
  PRIMARY KEY  (`guild_id`,`charac_no`),
  KEY `idx_guild_id` USING BTREE (`guild_id`),
  KEY `idx_charac_no` USING BTREE (`charac_no`),
  KEY `idx_last_visit_time` USING BTREE (`last_visit_time`),
  KEY `idx_apply_time` USING BTREE (`apply_time`),
  KEY `idx_secede_type` USING BTREE (`secede_type`),
  KEY `idx_secede_time` USING BTREE (`secede_time`),
  KEY `idx_member_flag` USING BTREE (`member_flag`),
  KEY `idx_m_id` USING BTREE (`m_id`),
  KEY `idx_member_time` USING BTREE (`member_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_member_introduce`
--

DROP TABLE IF EXISTS `guild_member_introduce`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_member_introduce` (
  `guild_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `introduce` varchar(200) NOT NULL default '',
  PRIMARY KEY  (`guild_id`,`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_notice`
--

DROP TABLE IF EXISTS `guild_notice`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_notice` (
  `guild_id` int(10) unsigned NOT NULL default '0',
  `notice` varchar(200) NOT NULL default '',
  `acc_date` int(11) NOT NULL default '0',
  PRIMARY KEY  (`guild_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_rank`
--

DROP TABLE IF EXISTS `guild_rank`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_rank` (
  `server_id` tinyint(4) NOT NULL default '0',
  `guild_id` int(11) NOT NULL default '0',
  `guild_name` varchar(40) NOT NULL default '',
  `guild_Rank` smallint(5) unsigned NOT NULL default '0',
  `guild_point` int(10) unsigned NOT NULL default '0',
  `guild_acc_point` int(10) unsigned NOT NULL default '0',
  `guild_visit` int(10) unsigned NOT NULL default '0',
  `guild_acc_visit` int(10) unsigned NOT NULL default '0',
  `guild_member` smallint(5) unsigned NOT NULL default '0',
  `guild_acc_member` smallint(5) unsigned NOT NULL default '0',
  `guild_avg_lev` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`server_id`,`guild_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_recommend`
--

DROP TABLE IF EXISTS `guild_recommend`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_recommend` (
  `no` int(11) NOT NULL default '0',
  `guild_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `charac_name` varchar(20) NOT NULL default '',
  `comment` varchar(100) NOT NULL default '',
  `recommend_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`no`),
  KEY `idx_guild_id` USING BTREE (`guild_id`),
  KEY `idx_charac_no` USING BTREE (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_search`
--

DROP TABLE IF EXISTS `guild_search`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_search` (
  `guild_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `guild_name` varchar(40) NOT NULL default '',
  `master_name` varchar(20) NOT NULL default '',
  `create_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `lev` int(11) NOT NULL default '0',
  `member_count` int(11) NOT NULL default '0',
  `guild_point_acc` int(10) unsigned NOT NULL default '0',
  `guild_exp` int(10) unsigned NOT NULL default '0',
  `guild_url` varchar(40) NOT NULL default '',
  PRIMARY KEY  (`guild_id`),
  KEY `idx_server_id` USING BTREE (`server_id`),
  KEY `idx_guild_name` USING BTREE (`guild_name`),
  KEY `idx_master_name` USING BTREE (`master_name`),
  KEY `idx_guild_url` USING BTREE (`guild_url`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_skill`
--

DROP TABLE IF EXISTS `guild_skill`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_skill` (
  `guild_id` int(11) NOT NULL default '0',
  `remain_sp` int(11) unsigned NOT NULL default '0',
  `skill_slot` blob NOT NULL,
  `used_sp` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`guild_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_stat`
--

DROP TABLE IF EXISTS `guild_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_stat` (
  `occ_date` date NOT NULL default '0000-00-00',
  `lev` tinyint(3) unsigned NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `create_no` int(11) NOT NULL default '0',
  `acc_create_no` int(11) NOT NULL default '0',
  `member_no` int(11) NOT NULL default '0',
  `acc_member_no` int(11) NOT NULL default '0',
  `avg_lev` float default NULL,
  `avg_master_lev` float default NULL,
  `expire_no` int(11) NOT NULL default '0',
  `new_account_no` int(11) NOT NULL default '0',
  `new_member_no` int(11) NOT NULL default '0',
  `acc_account_no` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`lev`,`server_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_stat_month`
--

DROP TABLE IF EXISTS `guild_stat_month`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_stat_month` (
  `occ_date` date NOT NULL default '0000-00-00',
  `lev` tinyint(3) unsigned NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `avg_guild_point` int(11) NOT NULL default '0',
  `avg_guild_point_acc` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`lev`,`server_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_stat_uv`
--

DROP TABLE IF EXISTS `guild_stat_uv`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_stat_uv` (
  `occ_date` date NOT NULL default '0000-00-00',
  `pv` int(10) unsigned default '0',
  `new_bbs` int(10) unsigned NOT NULL default '0',
  `total_read_bbs` int(10) unsigned NOT NULL default '0',
  `member_uv` int(10) unsigned NOT NULL default '0',
  `member_uv_week` int(10) unsigned NOT NULL default '0',
  `master_uv` int(10) unsigned NOT NULL default '0',
  `master_uv_week` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_visit`
--

DROP TABLE IF EXISTS `guild_visit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_visit` (
  `guild_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `total_visit` int(11) NOT NULL default '0',
  `today_visit` int(11) NOT NULL default '0',
  PRIMARY KEY  (`guild_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `power_war`
--

DROP TABLE IF EXISTS `power_war`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `power_war` (
  `server_id` tinyint(4) NOT NULL default '0',
  `a_side_point` int(10) unsigned NOT NULL default '0',
  `b_side_point` int(10) unsigned NOT NULL default '0',
  `winner_side` tinyint(4) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`server_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `power_war_guild_rank`
--

DROP TABLE IF EXISTS `power_war_guild_rank`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `power_war_guild_rank` (
  `server_id` tinyint(3) unsigned NOT NULL default '0',
  `rank` smallint(5) unsigned NOT NULL default '0',
  `guild_id` int(10) unsigned NOT NULL default '0',
  `power_war_point` int(10) unsigned NOT NULL default '0',
  `power_side` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`guild_id`),
  KEY `idx_server_id` USING BTREE (`server_id`),
  KEY `idx_power_side` USING BTREE (`power_side`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `power_war_statue_ranker`
--

DROP TABLE IF EXISTS `power_war_statue_ranker`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `power_war_statue_ranker` (
  `server_id` tinyint(3) unsigned NOT NULL default '0',
  `first_ranker` int(10) unsigned NOT NULL default '0',
  `second_ranker` int(10) unsigned NOT NULL default '0',
  `third_ranker` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`server_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `power_war_user_rank`
--

DROP TABLE IF EXISTS `power_war_user_rank`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `power_war_user_rank` (
  `server_id` tinyint(3) unsigned NOT NULL default '0',
  `rank` smallint(5) unsigned NOT NULL default '0',
  `charac_no` int(10) unsigned NOT NULL default '0',
  `power_war_point` int(10) unsigned NOT NULL default '0',
  `power_side` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`charac_no`,`server_id`),
  KEY `idx_server_id` USING BTREE (`server_id`),
  KEY `idx_power_side` USING BTREE (`power_side`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Current Database: `d_taiwan`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `d_taiwan` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `d_taiwan`;

--
-- Table structure for table `account_cerashop_restrict`
--

DROP TABLE IF EXISTS `account_cerashop_restrict`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `account_cerashop_restrict` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `ipg_no` int(10) unsigned NOT NULL default '0',
  `count` int(10) unsigned NOT NULL default '0',
  `next_date` int(10) unsigned NOT NULL default '0',
  `end_date` int(10) unsigned NOT NULL default '0',
  `last_access_date` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`,`ipg_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `accounts`
--

DROP TABLE IF EXISTS `accounts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `accounts` (
  `UID` int(11) NOT NULL auto_increment,
  `accountname` varchar(255) NOT NULL,
  `password` varchar(255) NOT NULL,
  `qq` varchar(255) default NULL,
  `dzuid` int(8) default NULL,
  `billing` int(8) default '0',
  `VIP` varchar(255) NOT NULL,
  PRIMARY KEY  (`UID`)
) ENGINE=MyISAM AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `accountss`
--

DROP TABLE IF EXISTS `accountss`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `accountss` (
  `uid` int(11) NOT NULL auto_increment,
  `accountname` varchar(255) NOT NULL,
  `password` varchar(255) NOT NULL,
  PRIMARY KEY  (`uid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `admin_member`
--

DROP TABLE IF EXISTS `admin_member`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `admin_member` (
  `no` int(20) NOT NULL auto_increment,
  `user_id` varchar(20) NOT NULL default '',
  `password` varchar(20) NOT NULL default '',
  `name` varchar(20) NOT NULL default '',
  `email` varchar(255) default NULL,
  `phone` varchar(255) default NULL,
  `msn` varchar(50) default NULL,
  `comment` text,
  `reg_date` int(13) default NULL,
  `confirm` char(1) default '0',
  `level` varchar(2000) NOT NULL default '',
  `level_group1` varchar(2) NOT NULL default '_',
  `level_group2` varchar(2) NOT NULL default '_',
  `level_group3` varchar(2) NOT NULL default '_',
  `level_group4` varchar(2) NOT NULL default '_',
  `level_group5` varchar(2) NOT NULL default '_',
  `level_group6` varchar(2) NOT NULL default '_',
  PRIMARY KEY  (`no`),
  KEY `user_id` (`user_id`),
  KEY `password` (`password`),
  KEY `name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=51 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bad_user`
--

DROP TABLE IF EXISTS `bad_user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bad_user` (
  `no` int(11) NOT NULL auto_increment,
  `m_id` int(11) NOT NULL default '0',
  `bad_code` int(11) NOT NULL default '0',
  `create_day` int(11) NOT NULL default '0',
  `exit_day` int(11) NOT NULL default '0',
  `admin_n` int(11) NOT NULL default '0',
  PRIMARY KEY  (`no`),
  KEY `idx_mid` (`m_id`),
  KEY `idx_code` (`bad_code`),
  KEY `idx_eday` (`exit_day`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_dnf_item_info`
--

DROP TABLE IF EXISTS `bak_dnf_item_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_dnf_item_info` (
  `it_no` mediumint(8) unsigned NOT NULL default '0',
  `it_name` varchar(25) NOT NULL default '',
  `it_eng_name` varchar(50) NOT NULL default '',
  `it_explain` varchar(60) NOT NULL default '',
  `master_type` tinyint(3) unsigned NOT NULL default '0',
  `sub_type` smallint(5) unsigned NOT NULL default '0',
  `job` varchar(12) NOT NULL default '',
  `class` tinyint(3) unsigned NOT NULL default '0',
  `revert` varchar(5) NOT NULL default '',
  `level` tinyint(3) unsigned NOT NULL default '0',
  `skill` smallint(5) unsigned NOT NULL default '0',
  `create_ratio` float NOT NULL default '0',
  `rarity` tinyint(3) unsigned NOT NULL default '0',
  `weight` smallint(6) NOT NULL default '0',
  `price` smallint(5) unsigned NOT NULL default '0',
  `cash` smallint(5) unsigned NOT NULL default '0',
  `medal` smallint(5) unsigned NOT NULL default '0',
  `durability` smallint(6) NOT NULL default '0',
  `cooltime` smallint(6) NOT NULL default '0',
  `hp_max` smallint(6) NOT NULL default '0',
  `mp_max` smallint(6) NOT NULL default '0',
  `phy_att` smallint(6) NOT NULL default '0',
  `phy_def` smallint(6) NOT NULL default '0',
  `mag_att` smallint(6) NOT NULL default '0',
  `mag_def` smallint(6) NOT NULL default '0',
  `equip_phy_att` smallint(6) NOT NULL default '0',
  `equip_phy_def` smallint(6) NOT NULL default '0',
  `equip_mag_att` smallint(6) NOT NULL default '0',
  `equip_mag_def` smallint(6) NOT NULL default '0',
  `ref_fire` tinyint(4) NOT NULL default '0',
  `ref_water` tinyint(4) NOT NULL default '0',
  `ref_dark` tinyint(4) NOT NULL default '0',
  `ref_light` tinyint(4) NOT NULL default '0',
  `ref_all` tinyint(4) NOT NULL default '0',
  `ref_slow` tinyint(4) NOT NULL default '0',
  `ref_freeze` tinyint(4) NOT NULL default '0',
  `ref_poison` tinyint(4) NOT NULL default '0',
  `ref_stun` tinyint(4) NOT NULL default '0',
  `ref_cus` tinyint(4) NOT NULL default '0',
  `ref_blind` tinyint(4) NOT NULL default '0',
  `ref_lite` tinyint(4) NOT NULL default '0',
  `ref_ston` tinyint(4) NOT NULL default '0',
  `ref_sleep` tinyint(4) NOT NULL default '0',
  `ref_deekement` tinyint(4) NOT NULL default '0',
  `ref_deadlystrike` tinyint(4) NOT NULL default '0',
  `ref_bleeding` tinyint(4) NOT NULL default '0',
  `ref_confuse` tinyint(4) NOT NULL default '0',
  `ref_hold` tinyint(4) NOT NULL default '0',
  `ref_all_stat` tinyint(4) NOT NULL default '0',
  `ref_pierce` smallint(6) NOT NULL default '0',
  `ref_stuck` smallint(6) NOT NULL default '0',
  `inven_max` smallint(6) NOT NULL default '0',
  `hp_regenrate` smallint(6) NOT NULL default '0',
  `mp_regenrate` smallint(6) NOT NULL default '0',
  `mov_speed` smallint(6) NOT NULL default '0',
  `att_speed` smallint(6) NOT NULL default '0',
  `quest` smallint(6) NOT NULL default '0',
  `hit_recovery` smallint(6) NOT NULL default '0',
  `jump` smallint(6) NOT NULL default '0',
  `att_element` enum('Void','Fire','Water','Dark','Light') NOT NULL default 'Void',
  `att_active_status` smallint(6) NOT NULL default '0',
  `att_active_status_ratio` float NOT NULL default '0',
  `att_active_status_pow` smallint(6) NOT NULL default '0',
  `att_backforce` smallint(6) NOT NULL default '0',
  `att_upforce` smallint(6) NOT NULL default '0',
  `att_hp_drain` tinyint(4) NOT NULL default '0',
  `att_mp_drain` tinyint(4) NOT NULL default '0',
  `criticalhit_rate` float NOT NULL default '0',
  `stuck_rate` float NOT NULL default '0',
  `att_defenseIgnore` tinyint(4) NOT NULL default '0',
  `skill_levelup` varchar(25) NOT NULL default '',
  `set_type` enum('n','y') NOT NULL default 'n',
  `url` varchar(64) NOT NULL default '',
  `jewel_type` varchar(5) NOT NULL default '',
  PRIMARY KEY  (`it_no`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_m_id_20130426_2`
--

DROP TABLE IF EXISTS `bak_m_id_20130426_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_m_id_20130426_2` (
  `m_id` int(11) NOT NULL default '0',
  KEY `m_id` (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_m_id_20130426_3`
--

DROP TABLE IF EXISTS `bak_m_id_20130426_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_m_id_20130426_3` (
  `user_id` varchar(30) default NULL,
  `reg_date` datetime default NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ch_status`
--

DROP TABLE IF EXISTS `ch_status`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ch_status` (
  `gc_group` tinyint(3) unsigned NOT NULL default '1',
  `gc_status` tinyint(3) unsigned NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `check_pick_up_random_option_item`
--

DROP TABLE IF EXISTS `check_pick_up_random_option_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `check_pick_up_random_option_item` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `check_count` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `db_connect`
--

DROP TABLE IF EXISTS `db_connect`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `db_connect` (
  `no` int(10) unsigned NOT NULL default '0',
  `host_name` varchar(50) default NULL,
  `db_server_group` tinyint(3) unsigned default NULL,
  `db_type` int(10) unsigned NOT NULL,
  `db_name` varchar(50) NOT NULL,
  `db_ip` varchar(16) NOT NULL,
  `db_port` int(10) unsigned NOT NULL,
  `db_userid` varchar(20) NOT NULL,
  `db_passwd` varchar(50) NOT NULL,
  `comments` varchar(255) default NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dblab_db_connect_130516`
--

DROP TABLE IF EXISTS `dblab_db_connect_130516`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dblab_db_connect_130516` (
  `no` int(10) unsigned NOT NULL default '0',
  `host_name` varchar(50) default NULL,
  `db_server_group` tinyint(3) unsigned default NULL,
  `db_type` int(10) unsigned NOT NULL,
  `db_name` varchar(50) NOT NULL,
  `db_ip` varchar(16) NOT NULL,
  `db_port` int(10) unsigned NOT NULL,
  `db_userid` varchar(20) NOT NULL,
  `db_passwd` varchar(50) NOT NULL,
  `comments` varchar(255) default NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_charac_mov`
--

DROP TABLE IF EXISTS `dnf_charac_mov`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_charac_mov` (
  `id` int(11) NOT NULL auto_increment,
  `m_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `move_server_id` tinyint(4) NOT NULL default '0',
  `move_charac_no` int(11) NOT NULL default '0',
  `move_check` int(11) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `m_id` (`m_id`,`server_id`,`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_event_address`
--

DROP TABLE IF EXISTS `dnf_event_address`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_event_address` (
  `event_id` int(11) NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `occ_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `zipcode` varchar(7) NOT NULL default '',
  `address` varchar(150) NOT NULL default '',
  `phone_no` varchar(15) NOT NULL default '',
  PRIMARY KEY  (`event_id`,`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_event_entry_notuse`
--

DROP TABLE IF EXISTS `dnf_event_entry_notuse`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_event_entry_notuse` (
  `event_id` int(11) NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `occ_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `obtain_date` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`event_id`,`m_id`),
  KEY `idx_m_id` (`m_id`),
  KEY `idx_charac_no` (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_event_info`
--

DROP TABLE IF EXISTS `dnf_event_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_event_info` (
  `event_id` int(11) NOT NULL default '0',
  `event_name` varchar(30) NOT NULL default '',
  `event_explain` varchar(100) NOT NULL default '',
  `apply_type` tinyint(4) NOT NULL default '0',
  `start_date` date NOT NULL default '0000-00-00',
  `end_date` date NOT NULL default '0000-00-00',
  PRIMARY KEY  (`event_id`),
  UNIQUE KEY `event_name` (`event_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_event_log`
--

DROP TABLE IF EXISTS `dnf_event_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_event_log` (
  `log_id` int(10) unsigned NOT NULL auto_increment,
  `occ_time` int(11) NOT NULL default '0',
  `event_type` tinyint(3) unsigned NOT NULL default '0',
  `parameter1` int(10) unsigned NOT NULL default '0',
  `parameter2` int(10) unsigned NOT NULL default '0',
  `server_id` tinyint(3) unsigned NOT NULL default '0',
  `event_flag` tinyint(3) default '0',
  `start_time` int(11) NOT NULL default '0',
  `end_time` int(11) NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `expl` varchar(200) NOT NULL default '',
  `etc` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`log_id`),
  KEY `idx_occ_time` (`occ_time`),
  KEY `idx_sever_id` (`server_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_event_prize`
--

DROP TABLE IF EXISTS `dnf_event_prize`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_event_prize` (
  `prize_id` int(11) NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `check_time` int(11) NOT NULL default '0',
  PRIMARY KEY  (`prize_id`,`m_id`),
  KEY `idx_m_id` (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_game_message`
--

DROP TABLE IF EXISTS `dnf_game_message`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_game_message` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `message` varchar(255) default NULL,
  `display_type` tinyint(4) NOT NULL default '1',
  `start_h` tinyint(3) unsigned NOT NULL default '0',
  `end_h` tinyint(3) unsigned NOT NULL default '0',
  `occ_date` date NOT NULL default '0000-00-00',
  PRIMARY KEY  (`no`),
  KEY `display_type` (`display_type`,`occ_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_gamein_notice`
--

DROP TABLE IF EXISTS `dnf_gamein_notice`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_gamein_notice` (
  `no` int(11) NOT NULL auto_increment,
  `img_name` varchar(250) NOT NULL default '',
  `server_id` tinyint(4) NOT NULL default '0',
  `reg_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `open_flag` enum('y','n') default 'n',
  PRIMARY KEY  (`no`),
  KEY `idx_server_id` (`server_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_master_charac`
--

DROP TABLE IF EXISTS `dnf_master_charac`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_master_charac` (
  `m_id` int(11) NOT NULL default '0',
  `global_type` tinyint(4) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `charac_name` varchar(20) NOT NULL default '',
  `job` tinyint(4) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`m_id`,`global_type`,`server_id`),
  KEY `server_id` (`server_id`,`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_pcroom`
--

DROP TABLE IF EXISTS `dnf_pcroom`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_pcroom` (
  `ip_no` int(11) NOT NULL auto_increment,
  `district` varchar(10) NOT NULL default '',
  `firm_name` varchar(25) NOT NULL default '',
  `telephone` varchar(10) NOT NULL default '',
  `address` varchar(75) NOT NULL default '',
  `leader` varchar(15) NOT NULL default '',
  `start_ip` varchar(7) NOT NULL default '',
  `end_ip` varchar(7) NOT NULL default '',
  PRIMARY KEY  (`ip_no`),
  UNIQUE KEY `start_ip` (`start_ip`),
  UNIQUE KEY `end_ip` (`end_ip`),
  KEY `idx_district` (`district`),
  KEY `idx_leader` (`leader`),
  KEY `idx_firm_name` (`firm_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_restrict_info`
--

DROP TABLE IF EXISTS `dnf_restrict_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_restrict_info` (
  `category` int(11) NOT NULL,
  `restrict_code` int(11) NOT NULL,
  `restrict_str` varchar(45) NOT NULL,
  `reg_date` datetime NOT NULL,
  PRIMARY KEY  (`category`,`restrict_code`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='서비스 제재 정보 문자열';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_restrict_state`
--

DROP TABLE IF EXISTS `dnf_restrict_state`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_restrict_state` (
  `server_group` int(11) NOT NULL,
  `category` int(11) NOT NULL,
  `restrict_code` int(11) NOT NULL,
  `restrict_value` char(1) NOT NULL,
  `mod_date` datetime NOT NULL,
  `reg_date` datetime NOT NULL,
  PRIMARY KEY  (`server_group`,`category`,`restrict_code`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_story`
--

DROP TABLE IF EXISTS `dnf_story`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_story` (
  `no` int(11) NOT NULL auto_increment,
  `story_type` tinyint(4) NOT NULL default '0',
  `notice_flag` tinyint(4) NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `reg_id` varchar(12) NOT NULL default '',
  `title` varchar(50) NOT NULL default '',
  `url` varchar(250) NOT NULL default '',
  `img_name` varchar(30) NOT NULL default '',
  `opt` tinyint(3) unsigned NOT NULL default '0',
  `open_flag` enum('y','n') NOT NULL default 'n',
  `reg_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `hits` int(10) unsigned NOT NULL default '0',
  `reserve_time` int(10) unsigned NOT NULL default '0',
  `content` text,
  PRIMARY KEY  (`no`),
  KEY `idx_mid` (`m_id`),
  KEY `idx_reg` (`reg_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_testr_m_id`
--

DROP TABLE IF EXISTS `dnf_testr_m_id`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_testr_m_id` (
  `m_id` int(11) NOT NULL default '0',
  `sex` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_arad_birthday_6th`
--

DROP TABLE IF EXISTS `event_arad_birthday_6th`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_arad_birthday_6th` (
  `server` int(10) unsigned NOT NULL default '0',
  `point` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`server`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_aradlotto_0809_entry`
--

DROP TABLE IF EXISTS `event_aradlotto_0809_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_aradlotto_0809_entry` (
  `m_id` int(11) NOT NULL default '0',
  `occ_date` int(11) NOT NULL default '0',
  `lotto_num` char(7) NOT NULL default '',
  PRIMARY KEY  (`m_id`),
  KEY `idx1` (`lotto_num`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_charac_mov_1th`
--

DROP TABLE IF EXISTS `event_charac_mov_1th`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_charac_mov_1th` (
  `id` int(11) NOT NULL auto_increment,
  `m_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `move_server_id` tinyint(4) NOT NULL default '0',
  `move_charac_no` int(11) NOT NULL default '0',
  `move_check` int(11) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `m_id` (`m_id`,`server_id`,`charac_no`),
  KEY `idx_move_charac_no` (`move_charac_no`),
  KEY `idx_m_id` (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_charac_mov_1th_entry`
--

DROP TABLE IF EXISTS `event_charac_mov_1th_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_charac_mov_1th_entry` (
  `m_id` int(11) NOT NULL default '0',
  `occ_time` int(11) NOT NULL default '0',
  `it_no` int(11) NOT NULL default '0',
  `item_check` int(11) NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_goldcard_cnt`
--

DROP TABLE IF EXISTS `event_goldcard_cnt`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_goldcard_cnt` (
  `item_no` int(10) NOT NULL default '0',
  `occ_date` date NOT NULL default '0000-00-00',
  `cnt` int(11) NOT NULL default '0',
  PRIMARY KEY  (`item_no`,`occ_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_goldcard_entry1`
--

DROP TABLE IF EXISTS `event_goldcard_entry1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_goldcard_entry1` (
  `occ_date` int(11) NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `item_no` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`m_id`),
  KEY `idx_m_id` (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_goldcard_entry2`
--

DROP TABLE IF EXISTS `event_goldcard_entry2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_goldcard_entry2` (
  `occ_date` int(11) NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `item_no` int(11) unsigned NOT NULL default '0',
  `item_check` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`m_id`),
  KEY `idx_m_id` (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_goldcard_info`
--

DROP TABLE IF EXISTS `event_goldcard_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_goldcard_info` (
  `m_id` int(11) NOT NULL default '0',
  `coupon` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_hinamatsuri_cnt`
--

DROP TABLE IF EXISTS `event_hinamatsuri_cnt`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_hinamatsuri_cnt` (
  `cnt` int(11) NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_mage_2years`
--

DROP TABLE IF EXISTS `event_mage_2years`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_mage_2years` (
  `m_id` int(11) NOT NULL default '0',
  `server_info` tinyint(3) unsigned NOT NULL default '0',
  `charac_no` int(11) NOT NULL auto_increment,
  `charac_name` varchar(100) NOT NULL default '',
  `create_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `delete_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `delete_flag` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`m_id`,`server_info`,`charac_no`),
  KEY `charac_no` (`charac_no`),
  KEY `idx_create_time` (`create_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_newmember0709_entry`
--

DROP TABLE IF EXISTS `event_newmember0709_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_newmember0709_entry` (
  `m_id` int(11) NOT NULL default '0',
  `occ_date` int(11) unsigned NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `item1_no` int(11) unsigned NOT NULL default '0',
  `item1_check` int(11) unsigned NOT NULL default '0',
  `item2_no` int(11) unsigned NOT NULL default '0',
  `item2_check` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`),
  KEY `idx_occ_date` (`occ_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_pandora_entry_200905`
--

DROP TABLE IF EXISTS `event_pandora_entry_200905`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_pandora_entry_200905` (
  `m_id` int(11) unsigned NOT NULL default '0',
  `occ_date` date NOT NULL default '0000-00-00',
  `server_id` tinyint(4) unsigned NOT NULL default '0',
  `charac_no` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`,`occ_date`,`server_id`),
  KEY `idx_date` (`occ_date`),
  KEY `idx_charac` (`server_id`,`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_quest_party_member_web`
--

DROP TABLE IF EXISTS `event_quest_party_member_web`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_quest_party_member_web` (
  `m_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `quest_no` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `send_charac_no` int(11) NOT NULL default '0',
  PRIMARY KEY  (`m_id`,`server_id`,`charac_no`,`quest_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_quizquiz_stamp`
--

DROP TABLE IF EXISTS `event_quizquiz_stamp`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_quizquiz_stamp` (
  `m_id` int(11) NOT NULL default '0',
  `degree` tinyint(4) NOT NULL default '0',
  `stamp` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`m_id`,`degree`),
  KEY `idx_m_id` (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_skill2025_entry`
--

DROP TABLE IF EXISTS `event_skill2025_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_skill2025_entry` (
  `m_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_tower_entry`
--

DROP TABLE IF EXISTS `event_tower_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_tower_entry` (
  `m_id` int(11) NOT NULL default '0',
  `occ_date` int(11) unsigned NOT NULL default '0',
  `occ_check` int(11) unsigned NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `item1_no` int(11) unsigned NOT NULL default '0',
  `item1_check` int(11) unsigned NOT NULL default '0',
  `item2_no` int(11) unsigned NOT NULL default '0',
  `item2_check` int(11) unsigned NOT NULL default '0',
  `item3_no` int(11) unsigned NOT NULL default '0',
  `item3_check` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`),
  KEY `idx_occ_date` (`occ_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_webmoneystamp_entry`
--

DROP TABLE IF EXISTS `event_webmoneystamp_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_webmoneystamp_entry` (
  `m_id` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `attend_point` smallint(5) unsigned NOT NULL default '0',
  `last_attend_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `return_flag` tinyint(4) NOT NULL default '0',
  `entry_item` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_webmoneystamp_item`
--

DROP TABLE IF EXISTS `event_webmoneystamp_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_webmoneystamp_item` (
  `m_id` int(11) NOT NULL default '0',
  `occ_time` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` tinyint(4) NOT NULL default '0',
  `item_no` int(10) unsigned NOT NULL default '0',
  `item_check` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`,`occ_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `game_channel`
--

DROP TABLE IF EXISTS `game_channel`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `game_channel` (
  `gc_no` int(11) unsigned NOT NULL auto_increment,
  `gc_now` smallint(5) unsigned NOT NULL default '0',
  `gc_ip` char(32) NOT NULL default '',
  `gc_port` smallint(5) unsigned NOT NULL default '0',
  `gc_max` smallint(5) unsigned NOT NULL default '0',
  `gc_game` tinyint(3) unsigned NOT NULL default '0',
  `gc_channel` char(16) NOT NULL default '',
  `gc_ch_group` smallint(5) NOT NULL default '0',
  `gc_channeltype` char(0) NOT NULL default '',
  PRIMARY KEY  (`gc_no`),
  KEY `idxGC_GAME` (`gc_game`),
  KEY `idxch_group` (`gc_ch_group`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `geo_allow`
--

DROP TABLE IF EXISTS `geo_allow`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `geo_allow` (
  `allow_ip` varchar(20) NOT NULL default '',
  `allow_c_code` varchar(4) NOT NULL default '',
  `allow_date` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  PRIMARY KEY  (`allow_ip`),
  KEY `idx_c_code` (`allow_c_code`),
  KEY `idx_date` (`allow_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `geo_allow_country`
--

DROP TABLE IF EXISTS `geo_allow_country`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `geo_allow_country` (
  `server_group` tinyint(4) NOT NULL,
  `country_code` varchar(10) NOT NULL,
  `reg_date` datetime NOT NULL,
  PRIMARY KEY  (`country_code`,`server_group`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `geo_country_code`
--

DROP TABLE IF EXISTS `geo_country_code`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `geo_country_code` (
  `code_no` int(11) NOT NULL,
  `country_code_a2` varchar(10) NOT NULL,
  `country_code_a3` varchar(10) NOT NULL,
  `country` varchar(255) NOT NULL,
  PRIMARY KEY  (`code_no`),
  UNIQUE KEY `geo_country_code_unq001` (`country_code_a2`),
  UNIQUE KEY `geo_country_code_unq002` (`country_code_a3`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `geo_reject`
--

DROP TABLE IF EXISTS `geo_reject`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `geo_reject` (
  `rej_ip` varchar(20) NOT NULL default '',
  `rej_c_code` varchar(4) NOT NULL default '',
  `rej_ip_count` int(11) unsigned NOT NULL default '0',
  `rej_last_date` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `rej_chk` char(1) NOT NULL default 'N',
  `rej_src` enum('w','g') NOT NULL default 'w',
  PRIMARY KEY  (`rej_ip`),
  KEY `idx_c_code` (`rej_c_code`),
  KEY `idx_date` (`rej_last_date`),
  KEY `idx_chk` (`rej_chk`),
  KEY `rej_src` (`rej_src`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gift_ticket_entry`
--

DROP TABLE IF EXISTS `gift_ticket_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `gift_ticket_entry` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `gift_no` smallint(5) unsigned NOT NULL default '0',
  `buyer_id` int(10) unsigned NOT NULL default '0',
  `buyer_date` int(10) unsigned NOT NULL default '0',
  `buyer_code` varchar(21) NOT NULL default '',
  `buyer_check` int(10) unsigned NOT NULL default '0',
  `other_id` int(10) unsigned NOT NULL default '0',
  `other_date` int(10) unsigned NOT NULL default '0',
  `other_code` varchar(21) NOT NULL default '',
  `other_check` int(10) unsigned NOT NULL default '0',
  `message` varchar(200) NOT NULL default '',
  PRIMARY KEY  (`id`),
  KEY `idx_buyer_id` (`buyer_id`),
  KEY `idx_other_id` (`other_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gm_manifest_notuse`
--

DROP TABLE IF EXISTS `gm_manifest_notuse`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `gm_manifest_notuse` (
  `m_id` int(11) NOT NULL default '0',
  `level` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_bbs`
--

DROP TABLE IF EXISTS `guild_bbs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_bbs` (
  `gno` int(11) NOT NULL auto_increment,
  `bd_id` tinyint(4) NOT NULL default '0',
  `empyn` tinyint(4) NOT NULL default '0',
  `mgno` int(11) NOT NULL default '0',
  `open` tinyint(1) NOT NULL default '1',
  `main` tinyint(4) NOT NULL default '0',
  `reg_date` int(11) NOT NULL default '0',
  `mod_date` int(11) NOT NULL default '0',
  `hits` mediumint(8) unsigned NOT NULL default '0',
  `body_type` char(1) NOT NULL default '',
  `m_id` int(11) NOT NULL default '0',
  `reg_id` varchar(20) NOT NULL default '',
  `subject` varchar(50) NOT NULL default '',
  PRIMARY KEY  (`gno`),
  UNIQUE KEY `uk_bdid_empyn_gno` (`bd_id`,`empyn`,`gno`),
  UNIQUE KEY `uk_bdid_empyn_mgno` (`bd_id`,`empyn`,`mgno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_body`
--

DROP TABLE IF EXISTS `guild_body`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_body` (
  `gno` int(11) NOT NULL default '0',
  `body` text NOT NULL,
  PRIMARY KEY  (`gno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_event`
--

DROP TABLE IF EXISTS `guild_event`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_event` (
  `gno` int(11) NOT NULL default '0',
  `stt_date` date NOT NULL default '0000-00-00',
  `end_date` date NOT NULL default '0000-00-00',
  `ann_date` date NOT NULL default '0000-00-00',
  `page_url` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`gno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_files`
--

DROP TABLE IF EXISTS `guild_files`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_files` (
  `gno` int(11) NOT NULL default '0',
  `gf_no` tinyint(4) NOT NULL auto_increment,
  `file_server` varchar(50) NOT NULL default '',
  `file_location` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`gno`,`gf_no`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_halloffame`
--

DROP TABLE IF EXISTS `guild_halloffame`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_halloffame` (
  `fame_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `guild_id` int(11) NOT NULL default '0',
  `guild_name` varchar(40) NOT NULL default '',
  `file_url` varchar(128) NOT NULL default '0',
  `open_flag` tinyint(4) NOT NULL default '0',
  `main_flag` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`fame_id`,`server_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_halloffame_html`
--

DROP TABLE IF EXISTS `guild_halloffame_html`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_halloffame_html` (
  `fame_id` int(11) NOT NULL default '0',
  `title` varchar(100) NOT NULL default '',
  `html` text NOT NULL,
  PRIMARY KEY  (`fame_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_info`
--

DROP TABLE IF EXISTS `guild_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_info` (
  `guild_id` int(11) NOT NULL auto_increment,
  `server_id` tinyint(4) NOT NULL default '0',
  `guild_name` varchar(40) NOT NULL default '',
  `master_id` int(11) NOT NULL default '0',
  `master_no` int(11) NOT NULL default '0',
  `master_name` varchar(20) NOT NULL default '',
  `guild_url` varchar(40) NOT NULL default '',
  `guild_icon` tinyint(3) unsigned NOT NULL default '0',
  `create_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `lev` int(11) NOT NULL default '0',
  `ability` tinyint(4) NOT NULL default '0',
  `expire_flag` tinyint(4) NOT NULL default '0',
  `expire_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `member_secede_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `member_count` int(11) NOT NULL default '0',
  `recommend_flag` tinyint(4) NOT NULL default '0',
  `recommend_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `guild_point` int(10) unsigned NOT NULL default '0',
  `guild_point_acc` int(10) unsigned NOT NULL default '0',
  `guild_point_prev` int(10) unsigned NOT NULL default '0',
  `guild_rank` int(10) unsigned NOT NULL default '0',
  `guild_war_point` int(10) unsigned NOT NULL default '0',
  `final_entry` smallint(5) unsigned NOT NULL default '0',
  `final_win` smallint(5) unsigned NOT NULL default '0',
  `guild_icon_auth` tinyint(4) NOT NULL default '0',
  `guild_exp` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`guild_id`),
  KEY `idx_server_id` (`server_id`),
  KEY `idx_guild_name` (`guild_name`),
  KEY `idx_master_no` (`master_no`),
  KEY `idx_master_name` (`master_name`),
  KEY `idx_guild_rank` (`guild_rank`),
  KEY `idx_guild_point_prev` (`guild_point_prev`),
  KEY `idx_guild_point_acc` (`guild_point_acc`),
  KEY `idx_member_count` (`member_count`),
  KEY `idx_expire_flag` (`expire_flag`),
  KEY `idx_guild_point` (`guild_point`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_introduce`
--

DROP TABLE IF EXISTS `guild_introduce`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_introduce` (
  `guild_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `introduce` varchar(200) NOT NULL default '',
  PRIMARY KEY  (`guild_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_member`
--

DROP TABLE IF EXISTS `guild_member`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_member` (
  `guild_id` int(11) NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `charac_name` varchar(20) NOT NULL default '',
  `nick_name` varchar(12) NOT NULL default '',
  `grade` tinyint(4) NOT NULL default '0',
  `job` tinyint(4) NOT NULL default '0',
  `grow_type` tinyint(4) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '0',
  `age` tinyint(4) NOT NULL default '0',
  `born_year` varchar(2) NOT NULL default '',
  `sex` char(1) NOT NULL default '',
  `apply_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `member_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `member_flag` tinyint(4) NOT NULL default '0',
  `bbs_cnt` smallint(5) unsigned NOT NULL default '0',
  `last_visit_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `secede_type` tinyint(4) NOT NULL default '0',
  `secede_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `member_point` int(10) unsigned NOT NULL default '0',
  `member_point_prev` int(10) unsigned NOT NULL default '0',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`guild_id`,`charac_no`),
  KEY `idx_guild_id` (`guild_id`),
  KEY `idx_charac_no` (`charac_no`),
  KEY `idx_last_visit_time` (`last_visit_time`),
  KEY `idx_apply_time` (`apply_time`),
  KEY `idx_secede_type` (`secede_type`),
  KEY `idx_secede_time` (`secede_time`),
  KEY `idx_member_flag` (`member_flag`),
  KEY `idx_m_id` (`m_id`),
  KEY `idx_member_time` (`member_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_member_introduce`
--

DROP TABLE IF EXISTS `guild_member_introduce`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_member_introduce` (
  `guild_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `introduce` varchar(200) NOT NULL default '',
  PRIMARY KEY  (`guild_id`,`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_rank`
--

DROP TABLE IF EXISTS `guild_rank`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_rank` (
  `server_id` tinyint(4) NOT NULL default '0',
  `guild_id` int(11) NOT NULL default '0',
  `guild_name` varchar(40) NOT NULL default '0',
  `guild_Rank` smallint(5) unsigned NOT NULL default '0',
  `guild_point` int(10) unsigned NOT NULL default '0',
  `guild_acc_point` int(10) unsigned NOT NULL default '0',
  `guild_visit` int(10) unsigned NOT NULL default '0',
  `guild_acc_visit` int(10) unsigned NOT NULL default '0',
  `guild_member` smallint(5) unsigned NOT NULL default '0',
  `guild_acc_member` smallint(5) unsigned NOT NULL default '0',
  `guild_avg_lev` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`server_id`,`guild_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_recommend`
--

DROP TABLE IF EXISTS `guild_recommend`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_recommend` (
  `no` int(11) NOT NULL default '0',
  `guild_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `charac_name` varchar(20) NOT NULL default '',
  `comment` varchar(100) NOT NULL default '',
  `recommend_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`no`),
  KEY `idx_guild_id` (`guild_id`),
  KEY `idx_charac_no` (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_stat`
--

DROP TABLE IF EXISTS `guild_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_stat` (
  `occ_date` date NOT NULL default '0000-00-00',
  `lev` tinyint(3) unsigned NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `create_no` int(11) NOT NULL default '0',
  `acc_create_no` int(11) NOT NULL default '0',
  `member_no` int(11) NOT NULL default '0',
  `acc_member_no` int(11) NOT NULL default '0',
  `avg_lev` float NOT NULL default '0',
  `avg_master_lev` float NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`lev`,`server_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_stat_month`
--

DROP TABLE IF EXISTS `guild_stat_month`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_stat_month` (
  `occ_date` date NOT NULL default '0000-00-00',
  `lev` tinyint(3) unsigned NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `avg_guild_point` int(11) NOT NULL default '0',
  `avg_guild_point_acc` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`lev`,`server_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_visit`
--

DROP TABLE IF EXISTS `guild_visit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild_visit` (
  `guild_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `total_visit` int(11) NOT NULL default '0',
  `today_visit` int(11) NOT NULL default '0',
  PRIMARY KEY  (`guild_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `limit_create_character`
--

DROP TABLE IF EXISTS `limit_create_character`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `limit_create_character` (
  `m_id` int(11) unsigned NOT NULL default '0',
  `count` int(11) unsigned NOT NULL default '0',
  `last_access_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`m_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `limit_create_character_ip`
--

DROP TABLE IF EXISTS `limit_create_character_ip`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `limit_create_character_ip` (
  `ip` int(11) unsigned NOT NULL default '0',
  `ip_str` char(16) NOT NULL default '',
  `last_access_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `count` int(11) unsigned NOT NULL default '0',
  `last_access_mid` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ip`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `limited_shop_manager`
--

DROP TABLE IF EXISTS `limited_shop_manager`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `limited_shop_manager` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `occ_time` int(10) unsigned NOT NULL default '0',
  `server_id` tinyint(3) unsigned NOT NULL default '0',
  `ipg_no` int(10) unsigned NOT NULL default '0',
  `item_no` int(10) unsigned NOT NULL default '0',
  `item_cnt` int(10) unsigned NOT NULL default '0',
  `cera_price` int(10) unsigned NOT NULL default '0',
  `gold_price` int(10) unsigned NOT NULL default '0',
  `avatar_period_type` tinyint(4) NOT NULL default '-1',
  `total_cnt` int(11) NOT NULL default '0',
  `sell_cnt` int(10) unsigned NOT NULL default '0',
  `restrict_no` int(10) unsigned NOT NULL default '0',
  `start_time` int(10) unsigned NOT NULL default '0',
  `end_time` int(10) unsigned NOT NULL default '0',
  `real_end_time` int(10) unsigned NOT NULL default '0',
  `npc_idx` int(10) unsigned NOT NULL default '0',
  `cond_charac_job` tinyint(3) unsigned NOT NULL default '0',
  `cond_lev_begin` tinyint(3) unsigned NOT NULL default '0',
  `cond_lev_end` tinyint(3) unsigned NOT NULL default '0',
  `cond_acc_create_time_begin` int(10) unsigned NOT NULL default '0',
  `cond_acc_create_time_end` int(10) unsigned NOT NULL default '0',
  `cond_cha_create_time_begin` int(10) unsigned NOT NULL default '0',
  `cond_cha_create_time_end` int(10) unsigned NOT NULL default '0',
  `status_flag` tinyint(3) unsigned NOT NULL default '0',
  `title` varchar(50) NOT NULL default '',
  `range_section` tinyint(3) unsigned NOT NULL default '0',
  `reason_etc` varchar(200) NOT NULL default '',
  `reason_stop` varchar(200) NOT NULL default '',
  `pos_flag` char(1) NOT NULL default '0',
  PRIMARY KEY  (`no`,`server_id`,`ipg_no`),
  KEY `idx_occ_time` (`occ_time`),
  KEY `idx_server_id` (`server_id`),
  KEY `idx_restrict_no` (`restrict_no`),
  KEY `idx_status_flag` (`status_flag`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_query_stat`
--

DROP TABLE IF EXISTS `log_query_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_query_stat` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `q_id` smallint(5) unsigned NOT NULL default '0',
  `gc_no` int(10) unsigned NOT NULL default '0',
  `total` int(10) unsigned NOT NULL default '0',
  `response_time` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`q_id`,`gc_no`),
  KEY `idx_q_id` USING BTREE (`q_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `m_withdraw`
--

DROP TABLE IF EXISTS `m_withdraw`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `m_withdraw` (
  `m_id` int(11) NOT NULL auto_increment,
  `user_id` varchar(6) NOT NULL default '',
  `user_name` varchar(5) NOT NULL default '',
  `first_ssn` varchar(3) NOT NULL default '',
  `second_ssn` varchar(3) NOT NULL default '',
  `passwd` varchar(8) NOT NULL default '',
  `mobile_no` varchar(7) NOT NULL default '',
  `reg_date` int(11) NOT NULL default '0',
  `email` varchar(25) NOT NULL default '',
  `q_no` tinyint(4) NOT NULL default '0',
  `q_answer` varchar(15) NOT NULL default '',
  `updt_date` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `state` tinyint(4) NOT NULL default '0',
  `w_type` smallint(6) NOT NULL default '0',
  `w_cause` varchar(100) NOT NULL default '',
  `w_date` int(11) NOT NULL default '0',
  `nickname` varchar(8) NOT NULL default '',
  PRIMARY KEY  (`m_id`),
  KEY `idx_reg_date` (`reg_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `max_count`
--

DROP TABLE IF EXISTS `max_count`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `max_count` (
  `server_info` tinyint(3) unsigned NOT NULL default '0',
  `mc_max` int(11) unsigned NOT NULL default '0',
  `mc_date` datetime NOT NULL default '0000-00-00 00:00:00',
  KEY `idx_mc_date` (`mc_date`),
  KEY `idx_server_info` (`server_info`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `max_count_channel`
--

DROP TABLE IF EXISTS `max_count_channel`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `max_count_channel` (
  `server_info` tinyint(3) unsigned NOT NULL default '0',
  `gc_channeltype` varchar(25) NOT NULL default '',
  `mc_max` int(11) unsigned NOT NULL default '0',
  `mc_date` datetime NOT NULL default '0000-00-00 00:00:00',
  KEY `idx_mc_date` (`mc_date`),
  KEY `idx_server_info` (`server_info`,`gc_channeltype`),
  KEY `idx_gc_channeltype` (`gc_channeltype`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `max_count_pvp`
--

DROP TABLE IF EXISTS `max_count_pvp`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `max_count_pvp` (
  `server_info` tinyint(3) unsigned NOT NULL default '0',
  `mc_max` int(11) unsigned NOT NULL default '0',
  `mc_date` datetime NOT NULL default '0000-00-00 00:00:00',
  KEY `idx_mc_date` (`mc_date`),
  KEY `idx_server_info` (`server_info`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `max_count_v2`
--

DROP TABLE IF EXISTS `max_count_v2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `max_count_v2` (
  `server_info` tinyint(3) unsigned NOT NULL default '0',
  `num_occupations_charscreen` int(10) unsigned NOT NULL default '0',
  `num_occupations_seriaroom` int(10) unsigned NOT NULL default '0',
  `num_login_per_min` int(10) unsigned NOT NULL default '0',
  `num_logout_per_min` int(10) unsigned NOT NULL default '0',
  `mc_date` datetime NOT NULL default '0000-00-00 00:00:00',
  KEY `mc_date` (`mc_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_abnomal`
--

DROP TABLE IF EXISTS `member_abnomal`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_abnomal` (
  `user_id` varchar(12) NOT NULL default '',
  `overlab_count` smallint(6) NOT NULL default '0',
  PRIMARY KEY  (`user_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_broadcast`
--

DROP TABLE IF EXISTS `member_broadcast`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_broadcast` (
  `event_id` int(11) NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `charac_name` varchar(20) NOT NULL default '',
  `start_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `end_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`event_id`,`m_id`,`server_id`,`charac_no`,`start_time`),
  UNIQUE KEY `charac_name` (`charac_name`),
  KEY `idx_m_id` (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_captcha_info`
--

DROP TABLE IF EXISTS `member_captcha_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_captcha_info` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `cert_time` int(10) unsigned NOT NULL default '0',
  `fail_count` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_handicap`
--

DROP TABLE IF EXISTS `member_handicap`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_handicap` (
  `event_id` int(11) NOT NULL default '0',
  `cap_type` tinyint(3) unsigned NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `start_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `end_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `handicap_value` int(11) NOT NULL default '0',
  PRIMARY KEY  (`event_id`,`cap_type`,`server_id`,`m_id`,`start_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_info`
--

DROP TABLE IF EXISTS `member_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_info` (
  `m_id` int(11) NOT NULL auto_increment,
  `user_id` varchar(30) default NULL,
  `user_name` varchar(10) NOT NULL default '',
  `first_ssn` varchar(6) NOT NULL default '',
  `second_ssn` varchar(7) NOT NULL default '',
  `passwd` varchar(32) NOT NULL default '',
  `mobile_no` varchar(15) NOT NULL default '',
  `reg_date` int(11) NOT NULL default '0',
  `email` varchar(50) NOT NULL default '',
  `q_no` tinyint(4) NOT NULL default '0',
  `q_answer` varchar(30) NOT NULL default '',
  `updt_date` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `state` tinyint(4) NOT NULL default '1',
  `nickname` varchar(16) NOT NULL default '',
  `email_yn` enum('y','n') NOT NULL default 'y',
  `ssn_check` tinyint(3) unsigned NOT NULL default '0',
  `slot` int(10) unsigned NOT NULL default '8',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `hangame_flag` tinyint(4) NOT NULL default '0',
  `hanmon_flag` tinyint(4) NOT NULL default '0',
  `m_type` tinyint(3) NOT NULL default '0',
  PRIMARY KEY  (`m_id`),
  KEY `idx_reg_date` (`reg_date`),
  KEY `idx_ssn` (`first_ssn`,`second_ssn`),
  KEY `idx_nick` (`nickname`),
  KEY `idx_userid` (`user_id`),
  KEY `idx_user_name` (`user_name`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_info_detail`
--

DROP TABLE IF EXISTS `member_info_detail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_info_detail` (
  `m_id` int(11) NOT NULL default '0',
  `zipcode` varchar(7) NOT NULL default '',
  `address` varchar(80) NOT NULL default '',
  `address_detail` varchar(70) NOT NULL default '',
  `occ_date` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_info_euckr`
--

DROP TABLE IF EXISTS `member_info_euckr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_info_euckr` (
  `m_id` int(11) NOT NULL auto_increment,
  `user_id` varchar(30) default NULL,
  `user_name` varchar(10) NOT NULL default '',
  `first_ssn` varchar(6) NOT NULL default '',
  `second_ssn` varchar(7) NOT NULL default '',
  `passwd` varchar(32) NOT NULL default '',
  `mobile_no` varchar(15) NOT NULL default '',
  `reg_date` int(11) NOT NULL default '0',
  `email` varchar(50) NOT NULL default '',
  `q_no` tinyint(4) NOT NULL default '0',
  `q_answer` varchar(30) NOT NULL default '',
  `updt_date` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `state` tinyint(4) NOT NULL default '1',
  `nickname` varchar(16) NOT NULL default '',
  `email_yn` enum('y','n') NOT NULL default 'y',
  `ssn_check` tinyint(3) unsigned NOT NULL default '0',
  `slot` int(10) unsigned NOT NULL default '8',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `hangame_flag` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`m_id`),
  KEY `idx_reg_date` (`reg_date`),
  KEY `idx_ssn` (`first_ssn`,`second_ssn`),
  KEY `idx_nick` (`nickname`),
  KEY `idx_userid` (`user_id`),
  KEY `idx_user_name` (`user_name`)
) ENGINE=InnoDB DEFAULT CHARSET=euckr;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_info_mileage`
--

DROP TABLE IF EXISTS `member_info_mileage`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_info_mileage` (
  `m_id` int(11) NOT NULL auto_increment,
  `user_id` varchar(30) default NULL,
  `user_name` varchar(10) NOT NULL default '',
  `first_ssn` varchar(6) NOT NULL default '',
  `second_ssn` varchar(7) NOT NULL default '',
  `passwd` varchar(32) NOT NULL default '',
  `mobile_no` varchar(15) NOT NULL default '',
  `reg_date` int(11) NOT NULL default '0',
  `email` varchar(50) NOT NULL default '',
  `q_no` tinyint(4) NOT NULL default '0',
  `q_answer` varchar(30) NOT NULL default '',
  `updt_date` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `state` tinyint(4) NOT NULL default '1',
  `nickname` varchar(16) NOT NULL default '',
  `email_yn` enum('y','n') NOT NULL default 'y',
  `ssn_check` tinyint(3) unsigned NOT NULL default '0',
  `slot` int(10) unsigned NOT NULL default '8',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `hangame_flag` tinyint(4) NOT NULL default '0',
  `hanmon_flag` tinyint(4) NOT NULL default '0',
  `mileage` int(11) default '0',
  PRIMARY KEY  (`m_id`),
  KEY `idx_reg_date` (`reg_date`),
  KEY `idx_ssn` (`first_ssn`,`second_ssn`),
  KEY `idx_nick` (`nickname`),
  KEY `idx_userid` (`user_id`),
  KEY `idx_user_name` (`user_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_info_old`
--

DROP TABLE IF EXISTS `member_info_old`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_info_old` (
  `m_id` int(11) NOT NULL auto_increment,
  `user_id` varchar(15) default NULL,
  `user_name` varchar(5) NOT NULL default '',
  `first_ssn` varchar(10) default NULL,
  `second_ssn` varchar(10) default NULL,
  `passwd` varchar(8) NOT NULL default '',
  `mobile_no` varchar(8) NOT NULL default '',
  `reg_date` int(11) NOT NULL default '0',
  `email` varchar(25) NOT NULL default '',
  `q_no` tinyint(4) NOT NULL default '0',
  `q_answer` varchar(15) NOT NULL default '',
  `updt_date` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `state` tinyint(4) NOT NULL default '1',
  `nickname` varchar(8) NOT NULL default '',
  `email_yn` enum('y','n') NOT NULL default 'y',
  `ssn_check` tinyint(3) unsigned NOT NULL default '0',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`m_id`),
  KEY `idx_reg_date` (`reg_date`),
  KEY `idx_ssn` (`first_ssn`,`second_ssn`),
  KEY `idx_nick` (`nickname`),
  KEY `idx_userid` (`user_id`),
  KEY `idx_user_name` (`user_name`),
  KEY `first_ssn` (`first_ssn`),
  KEY `second_ssn` (`second_ssn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_info_utf8`
--

DROP TABLE IF EXISTS `member_info_utf8`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_info_utf8` (
  `m_id` int(11) NOT NULL default '0',
  `user_id` varchar(30) default NULL,
  `user_name` varchar(10) NOT NULL default '',
  `first_ssn` varchar(6) NOT NULL default '',
  `second_ssn` varchar(7) NOT NULL default '',
  `passwd` varchar(32) NOT NULL default '',
  `mobile_no` varchar(15) NOT NULL default '',
  `reg_date` int(11) NOT NULL default '0',
  `email` varchar(50) NOT NULL default '',
  `q_no` tinyint(4) NOT NULL default '0',
  `q_answer` varchar(30) NOT NULL default '',
  `updt_date` timestamp NOT NULL default CURRENT_TIMESTAMP,
  `state` tinyint(4) NOT NULL default '1',
  `nickname` varchar(16) NOT NULL default '',
  `email_yn` enum('y','n') NOT NULL default 'y',
  `ssn_check` tinyint(3) unsigned NOT NULL default '0',
  `slot` int(10) unsigned NOT NULL default '8',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `hangame_flag` tinyint(4) NOT NULL default '0',
  `hanmon_flag` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`m_id`),
  KEY `idx_reg_date` (`reg_date`),
  KEY `idx_ssn` (`first_ssn`,`second_ssn`),
  KEY `idx_nick` (`nickname`),
  KEY `idx_userid` (`user_id`),
  KEY `idx_user_name` (`user_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_join_info`
--

DROP TABLE IF EXISTS `member_join_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_join_info` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `reg_date` int(11) NOT NULL default '0',
  `ip` varchar(15) NOT NULL default '',
  `contry_code` tinyint(3) unsigned NOT NULL default '0',
  `login_time` int(11) NOT NULL default '0',
  `error_type` tinyint(3) unsigned NOT NULL default '0',
  `login_ip` varchar(15) NOT NULL default '',
  `game_use_history` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`),
  KEY `idx_ip` (`ip`),
  KEY `idx_reg_date` (`reg_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_lioness`
--

DROP TABLE IF EXISTS `member_lioness`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_lioness` (
  `user_id` varchar(30) NOT NULL default ''
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_miles`
--

DROP TABLE IF EXISTS `member_miles`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_miles` (
  `m_id` int(11) NOT NULL default '0',
  `miles` int(11) NOT NULL default '0',
  `daily_miles` smallint(6) NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_mouse_sms`
--

DROP TABLE IF EXISTS `member_mouse_sms`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_mouse_sms` (
  `m_id` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `cnt` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_passwd_mod`
--

DROP TABLE IF EXISTS `member_passwd_mod`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_passwd_mod` (
  `m_id` int(11) NOT NULL default '0',
  `first_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `last_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `cnt` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_premium_history`
--

DROP TABLE IF EXISTS `member_premium_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_premium_history` (
  `event_id` int(11) NOT NULL default '0',
  `pre_type` tinyint(3) unsigned NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `service_start` datetime NOT NULL default '0000-00-00 00:00:00',
  `service_end` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`event_id`,`pre_type`,`m_id`,`service_start`),
  KEY `idx_m_id` (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_premium_notuse`
--

DROP TABLE IF EXISTS `member_premium_notuse`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_premium_notuse` (
  `event_id` int(11) NOT NULL default '0',
  `pre_type` tinyint(3) unsigned NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `service_start` datetime NOT NULL default '0000-00-00 00:00:00',
  `service_end` datetime NOT NULL default '0000-00-00 00:00:00',
  `server_id` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`event_id`,`pre_type`,`server_id`,`m_id`,`service_start`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_punish_hack`
--

DROP TABLE IF EXISTS `member_punish_hack`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_punish_hack` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `occ_time` int(10) unsigned NOT NULL default '0',
  `period` int(10) unsigned NOT NULL default '0',
  `now_flag` tinyint(4) NOT NULL default '0',
  `auto_flag` tinyint(4) NOT NULL default '0',
  `reason` varchar(250) NOT NULL default '',
  `hack_ip` varchar(15) NOT NULL default '',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_punish_hack_history`
--

DROP TABLE IF EXISTS `member_punish_hack_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_punish_hack_history` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `occ_time` int(10) unsigned NOT NULL default '0',
  `period` int(10) unsigned NOT NULL default '0',
  `now_flag` tinyint(4) NOT NULL default '0',
  `auto_flag` tinyint(4) NOT NULL default '0',
  `reason` varchar(250) NOT NULL default '',
  KEY `idx_m_id` (`m_id`),
  KEY `idx_occ_time` (`occ_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_punish_info`
--

DROP TABLE IF EXISTS `member_punish_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_punish_info` (
  `m_id` int(11) NOT NULL default '0',
  `punish_type` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `punish_value` int(11) NOT NULL default '0',
  `apply_flag` tinyint(4) NOT NULL default '0',
  `start_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `end_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `admin_id` varchar(255) NOT NULL,
  `reason` varchar(255) default NULL,
  `IP` varchar(15) default NULL,
  `Mac` varchar(100) default NULL,
  PRIMARY KEY  (`m_id`,`punish_type`),
  KEY `idx1` (`occ_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_punish_info_history`
--

DROP TABLE IF EXISTS `member_punish_info_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_punish_info_history` (
  `no` int(11) NOT NULL auto_increment,
  `m_id` int(11) NOT NULL default '0',
  `punish_type` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `punish_value` int(11) NOT NULL default '0',
  `apply_flag` tinyint(4) NOT NULL default '0',
  `start_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `end_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `admin_id` varchar(25) default NULL,
  `reason` varchar(100) default NULL,
  `is_kicked` tinyint(4) default NULL,
  `first_ssn` varchar(32) default NULL,
  `second_ssn` varchar(32) default NULL,
  PRIMARY KEY  (`no`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_punish_info_history_2012`
--

DROP TABLE IF EXISTS `member_punish_info_history_2012`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_punish_info_history_2012` (
  `no` int(11) NOT NULL auto_increment,
  `m_id` int(11) NOT NULL default '0',
  `punish_type` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `punish_value` int(11) NOT NULL default '0',
  `apply_flag` tinyint(4) NOT NULL default '0',
  `start_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `end_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `admin_id` varchar(25) default NULL,
  `reason` varchar(100) default NULL,
  `is_kicked` tinyint(4) default NULL,
  `first_ssn` varchar(32) default NULL,
  `second_ssn` varchar(32) default NULL,
  PRIMARY KEY  (`no`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_punish_info_history_2013`
--

DROP TABLE IF EXISTS `member_punish_info_history_2013`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_punish_info_history_2013` (
  `no` int(11) NOT NULL auto_increment,
  `m_id` int(11) NOT NULL default '0',
  `punish_type` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `punish_value` int(11) NOT NULL default '0',
  `apply_flag` tinyint(4) NOT NULL default '0',
  `start_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `end_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `admin_id` varchar(25) default NULL,
  `reason` varchar(100) default NULL,
  `is_kicked` tinyint(4) default NULL,
  `first_ssn` varchar(32) default NULL,
  `second_ssn` varchar(32) default NULL,
  PRIMARY KEY  (`no`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_safe_ensure`
--

DROP TABLE IF EXISTS `member_safe_ensure`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_safe_ensure` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `mobile_no` varchar(15) NOT NULL default '',
  `service_flag` tinyint(4) NOT NULL default '0',
  `type1_flag` tinyint(4) NOT NULL default '0',
  `type2_flag` tinyint(4) NOT NULL default '0',
  `expire_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `settle_id` varchar(18) NOT NULL default '',
  KEY `idx_m_id` (`m_id`),
  KEY `idx_mobile_no` (`mobile_no`),
  KEY `idx_occ_time` (`occ_time`),
  KEY `idx_expire_time` (`expire_time`),
  KEY `idx_settle_id` (`settle_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_safe_ensure_history`
--

DROP TABLE IF EXISTS `member_safe_ensure_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_safe_ensure_history` (
  `mod_flag` tinyint(4) NOT NULL default '0',
  `mod_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `m_id` int(10) unsigned NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `mobile_no` varchar(15) NOT NULL default '',
  `service_flag` tinyint(4) NOT NULL default '0',
  `type1_flag` tinyint(4) NOT NULL default '0',
  `type2_flag` tinyint(4) NOT NULL default '0',
  `expire_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `settle_id` varchar(18) NOT NULL default '',
  KEY `idx_m_id` (`m_id`),
  KEY `idx_mobile_no` (`mobile_no`),
  KEY `idx_occ_time` (`occ_time`),
  KEY `idx_expire_time` (`expire_time`),
  KEY `idx_mod_time` (`mod_time`),
  KEY `idx_settle_id` (`settle_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_security_grade`
--

DROP TABLE IF EXISTS `member_security_grade`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_security_grade` (
  `m_id` int(11) NOT NULL default '0',
  `last_visit_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `pass_fail_cnt` int(11) NOT NULL default '0',
  `last_vaccine_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `last_window_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `goblin_pass_mod` datetime NOT NULL default '0000-00-00 00:00:00',
  `goblin_fail_cnt` int(11) NOT NULL default '0',
  `security_card_reg` datetime NOT NULL default '0000-00-00 00:00:00',
  `security_card_fail_cnt` int(11) NOT NULL default '0',
  `m_opt_reg` datetime NOT NULL default '0000-00-00 00:00:00',
  `pc_opt_reg` datetime NOT NULL default '0000-00-00 00:00:00',
  `black_ip_try_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `linear_pass_fail_cnt` int(11) NOT NULL default '0',
  `last_pass_fail_time` int(10) unsigned NOT NULL default '0',
  `last_check_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `pass_modify_check` datetime NOT NULL default '0000-00-00 00:00:00',
  `member_pc_reg` datetime NOT NULL default '0000-00-00 00:00:00',
  `gatekeeper_otp_reg` datetime NOT NULL default '0000-00-00 00:00:00',
  `goblin_validity_time` int(11) NOT NULL default '0',
  `security_card_validity_time` int(11) NOT NULL default '0',
  `validity_ip` varchar(15) NOT NULL default '',
  `cargopad_status` tinyint(4) NOT NULL default '0',
  `cargopad_mod` datetime NOT NULL default '0000-00-00 00:00:00',
  `cargopad_validity_time` int(11) NOT NULL default '0',
  PRIMARY KEY  (`m_id`),
  KEY `idx_pass_check` (`last_pass_fail_time`,`linear_pass_fail_cnt`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_white_account`
--

DROP TABLE IF EXISTS `member_white_account`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_white_account` (
  `m_id` int(10) unsigned NOT NULL,
  `reg_date` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `news_bbs`
--

DROP TABLE IF EXISTS `news_bbs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `news_bbs` (
  `bbs_code` tinyint(4) NOT NULL default '0',
  `emph_yn` tinyint(1) NOT NULL default '0',
  `no` int(11) NOT NULL auto_increment,
  `user_id` varchar(12) NOT NULL default '',
  `reg_date` int(11) NOT NULL default '0',
  `html_yn` tinyint(1) default '0',
  `subject` varchar(50) NOT NULL default '',
  `body` text NOT NULL,
  `hits` smallint(6) NOT NULL default '0',
  `prev_no` int(11) NOT NULL default '0',
  `next_no` int(11) NOT NULL default '0',
  `updt_date` int(11) default NULL,
  `use_yn` tinyint(1) NOT NULL default '1',
  `file_name` varchar(50) default NULL,
  PRIMARY KEY  (`bbs_code`,`emph_yn`,`no`),
  UNIQUE KEY `uk_no` (`no`),
  KEY `idx_prev` (`prev_no`),
  KEY `idx_next` (`next_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `notice`
--

DROP TABLE IF EXISTS `notice`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `notice` (
  `bbs_name` varchar(10) NOT NULL default '',
  `no` mediumint(8) unsigned NOT NULL auto_increment,
  `category` tinyint(3) unsigned NOT NULL default '0',
  `m_nickname` varchar(12) NOT NULL default '',
  `m_id` int(10) unsigned NOT NULL default '0',
  `m_user_id` varchar(16) NOT NULL default '',
  `m_sex` enum('m','f') NOT NULL default 'm',
  `title` varchar(120) NOT NULL default '',
  `create_day` int(10) unsigned NOT NULL default '0',
  `comment` smallint(5) unsigned NOT NULL default '0',
  `view` mediumint(8) unsigned NOT NULL default '0',
  `recom` mediumint(8) unsigned NOT NULL default '0',
  `adorn` tinyint(3) unsigned NOT NULL default '0',
  `adorn_color1` tinyint(3) unsigned NOT NULL default '0',
  `adorn_color2` tinyint(3) unsigned NOT NULL default '0',
  `depth` tinyint(3) unsigned NOT NULL default '0',
  `sequence` double unsigned NOT NULL default '0',
  `content` text NOT NULL,
  `content_type` enum('br','text','all') NOT NULL default 'br',
  `ip` varchar(15) NOT NULL default '',
  `ring` smallint(5) unsigned NOT NULL default '0',
  `sms` enum('y','n') NOT NULL default 'n',
  KEY `idx1` (`bbs_name`),
  KEY `idx2` (`no`),
  KEY `idx3` (`sequence`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `passwd_mod_entry`
--

DROP TABLE IF EXISTS `passwd_mod_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `passwd_mod_entry` (
  `m_id` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `ip` varchar(15) NOT NULL default '',
  `pre_passwd` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`m_id`,`occ_time`),
  KEY `idx_occ_time` (`occ_time`),
  KEY `idx_ip` (`ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pswd_qstion`
--

DROP TABLE IF EXISTS `pswd_qstion`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pswd_qstion` (
  `q_no` tinyint(4) NOT NULL default '0',
  `q_text` varchar(20) NOT NULL default '',
  PRIMARY KEY  (`q_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pswd_qstion_direct`
--

DROP TABLE IF EXISTS `pswd_qstion_direct`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pswd_qstion_direct` (
  `m_id` int(11) NOT NULL default '0',
  `q_text` varchar(20) NOT NULL default '',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pu_user_list`
--

DROP TABLE IF EXISTS `pu_user_list`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pu_user_list` (
  `m_id` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `slang_list`
--

DROP TABLE IF EXISTS `slang_list`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `slang_list` (
  `slang` varchar(153) character set utf8 collate utf8_bin NOT NULL default '',
  PRIMARY KEY  (`slang`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `slang_list_name`
--

DROP TABLE IF EXISTS `slang_list_name`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `slang_list_name` (
  `slang` varchar(153) character set utf8 collate utf8_bin NOT NULL default '',
  PRIMARY KEY  (`slang`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `test`
--

DROP TABLE IF EXISTS `test`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `test` (
  `a` int(11) default NULL,
  `b` datetime default NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `test2`
--

DROP TABLE IF EXISTS `test2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `test2` (
  `a` int(10) unsigned NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `tme_charac`
--

DROP TABLE IF EXISTS `tme_charac`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tme_charac` (
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `charac_name` varchar(10) NOT NULL default '',
  `village` tinyint(4) NOT NULL default '1',
  `job` tinyint(4) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '1',
  `exp` int(11) NOT NULL default '0',
  `grow_type` tinyint(4) NOT NULL default '0',
  `HP` tinyint(4) unsigned NOT NULL default '0',
  `maxHP` smallint(6) unsigned NOT NULL default '0',
  `maxMP` smallint(6) unsigned NOT NULL default '0',
  `phy_attack` smallint(6) unsigned NOT NULL default '0',
  `phy_defense` smallint(6) unsigned NOT NULL default '0',
  `mag_attack` smallint(6) unsigned NOT NULL default '0',
  `mag_defense` smallint(6) unsigned NOT NULL default '0',
  `element_resist` tinyblob NOT NULL,
  `spec_property` tinyblob NOT NULL,
  `inven_weight` int(6) NOT NULL default '0',
  `hp_regen` smallint(6) NOT NULL default '0',
  `mp_regen` smallint(6) NOT NULL default '0',
  `move_speed` smallint(6) unsigned NOT NULL default '0',
  `attack_speed` smallint(6) unsigned NOT NULL default '0',
  `cast_speed` smallint(6) unsigned NOT NULL default '0',
  `hit_recovery` smallint(6) NOT NULL default '0',
  `jump` smallint(6) NOT NULL default '0',
  `charac_weight` int(11) NOT NULL default '0',
  `fatigue` smallint(11) NOT NULL default '0',
  `max_fatigue` smallint(6) NOT NULL default '70',
  `premium_fatigue` smallint(11) NOT NULL default '0',
  `max_premium_fatigue` smallint(6) NOT NULL default '0',
  `create_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `dungeon_clear_point` int(11) NOT NULL default '0',
  `delete_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `delete_flag` tinyint(4) NOT NULL default '0',
  `guild_id` int(10) unsigned NOT NULL default '0',
  `guild_right` tinyint(4) NOT NULL default '0',
  `member_flag` tinyint(4) NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `tmp_charac`
--

DROP TABLE IF EXISTS `tmp_charac`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tmp_charac` (
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `charac_name` varchar(10) NOT NULL default '',
  `village` tinyint(4) NOT NULL default '1',
  `job` tinyint(4) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '1',
  `exp` int(11) NOT NULL default '0',
  `grow_type` tinyint(4) NOT NULL default '0',
  `HP` tinyint(4) unsigned NOT NULL default '0',
  `maxHP` smallint(6) unsigned NOT NULL default '0',
  `maxMP` smallint(6) unsigned NOT NULL default '0',
  `phy_attack` smallint(6) unsigned NOT NULL default '0',
  `phy_defense` smallint(6) unsigned NOT NULL default '0',
  `mag_attack` smallint(6) unsigned NOT NULL default '0',
  `mag_defense` smallint(6) unsigned NOT NULL default '0',
  `element_resist` tinyblob NOT NULL,
  `spec_property` tinyblob NOT NULL,
  `inven_weight` int(6) NOT NULL default '0',
  `hp_regen` smallint(6) NOT NULL default '0',
  `mp_regen` smallint(6) NOT NULL default '0',
  `move_speed` smallint(6) unsigned NOT NULL default '0',
  `attack_speed` smallint(6) unsigned NOT NULL default '0',
  `cast_speed` smallint(6) unsigned NOT NULL default '0',
  `hit_recovery` smallint(6) NOT NULL default '0',
  `jump` smallint(6) NOT NULL default '0',
  `charac_weight` int(11) NOT NULL default '0',
  `fatigue` smallint(11) NOT NULL default '0',
  `max_fatigue` smallint(6) NOT NULL default '70',
  `premium_fatigue` smallint(11) NOT NULL default '0',
  `max_premium_fatigue` smallint(6) NOT NULL default '0',
  `create_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `dungeon_clear_point` int(11) NOT NULL default '0',
  `delete_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `delete_flag` tinyint(4) NOT NULL default '0',
  `guild_id` int(10) unsigned NOT NULL default '0',
  `guild_right` tinyint(4) NOT NULL default '0',
  `member_flag` tinyint(4) NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `under_age_consent`
--

DROP TABLE IF EXISTS `under_age_consent`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `under_age_consent` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `consent_type` tinyint(3) unsigned NOT NULL default '0',
  `limit_money` mediumint(8) unsigned NOT NULL default '0',
  `parent_name` varchar(4) NOT NULL default '',
  `parent_jumin` bigint(20) unsigned NOT NULL default '0',
  `parent_phone1` tinyint(3) unsigned NOT NULL default '0',
  `parent_phone2` smallint(5) unsigned NOT NULL default '0',
  `parent_phone3` smallint(5) unsigned NOT NULL default '0',
  `parent_email` varchar(25) NOT NULL default '',
  `parent_consent_type` tinyint(3) unsigned NOT NULL default '0',
  `notice_type` tinyint(3) unsigned NOT NULL default '0',
  `notice_addr` varchar(50) NOT NULL default '',
  `create_date` int(10) unsigned NOT NULL default '0',
  `consent_date` int(10) unsigned NOT NULL default '0',
  `consent_yn` tinyint(3) unsigned NOT NULL default '0',
  `history_yn` tinyint(3) unsigned NOT NULL default '0',
  KEY `idxid` (`m_id`,`create_date`),
  KEY `idx_parent_name` (`parent_name`),
  KEY `idx_parent_email` (`parent_email`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `under_billing_confirm`
--

DROP TABLE IF EXISTS `under_billing_confirm`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `under_billing_confirm` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `parent_name` varchar(4) NOT NULL default '',
  `parent_jumin` bigint(20) unsigned NOT NULL default '0',
  `parent_phone1` tinyint(3) unsigned NOT NULL default '0',
  `parent_phone2` smallint(5) unsigned NOT NULL default '0',
  `parent_phone3` smallint(5) unsigned NOT NULL default '0',
  `parent_email` varchar(25) NOT NULL default '',
  `parent_consent_type` tinyint(3) unsigned NOT NULL default '0',
  `create_date` int(10) unsigned NOT NULL default '0',
  `consent_date` int(10) unsigned NOT NULL default '0',
  `consent_yn` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `user_ban`
--

DROP TABLE IF EXISTS `user_ban`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `user_ban` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `category` tinyint(4) NOT NULL default '1',
  `m_id` int(10) unsigned NOT NULL default '0',
  `ban_term` smallint(5) unsigned NOT NULL default '0',
  `ban_reason` tinyint(3) unsigned NOT NULL default '0',
  `detail_reason` text NOT NULL,
  `ban_date` int(10) unsigned NOT NULL default '0',
  `cancel_reason` text NOT NULL,
  `cancel_date` int(10) unsigned NOT NULL default '0',
  `admin_id` int(10) unsigned NOT NULL default '0',
  `status` tinyint(3) unsigned NOT NULL default '0',
  `first_ssn` varchar(6) NOT NULL default '',
  `second_ssn` varchar(7) NOT NULL default '',
  PRIMARY KEY  (`no`),
  KEY `ie_m_id` (`m_id`,`status`),
  KEY `idx_first_ssn` (`first_ssn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Current Database: `d_taiwan_secu`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `d_taiwan_secu` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `d_taiwan_secu`;

--
-- Table structure for table `member_env_log`
--

DROP TABLE IF EXISTS `member_env_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_env_log` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `mac_addr` varchar(64) NOT NULL default '',
  `log` text NOT NULL,
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  KEY `mac_addr` USING BTREE (`mac_addr`(7))
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_mac_info`
--

DROP TABLE IF EXISTS `member_mac_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_mac_info` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `mac_addr` varchar(64) NOT NULL default '',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`no`),
  UNIQUE KEY `mac_addr` USING BTREE (`mac_addr`),
  KEY `mac_addr_2` USING BTREE (`mac_addr`(7)),
  KEY `add_date` USING BTREE (`occ_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_punish_info`
--

DROP TABLE IF EXISTS `member_punish_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_punish_info` (
  `m_id` int(11) NOT NULL default '0',
  `punish_type` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `punish_value` int(11) NOT NULL default '0',
  `apply_flag` tinyint(4) NOT NULL default '0',
  `start_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `end_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `admin_id` varchar(25) NOT NULL default '',
  `reason` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`m_id`,`punish_type`),
  KEY `idx1` USING BTREE (`occ_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_punish_info_ars_mail`
--

DROP TABLE IF EXISTS `member_punish_info_ars_mail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_punish_info_ars_mail` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `m_id` int(10) unsigned NOT NULL default '0',
  `punish_type` int(10) unsigned NOT NULL default '0',
  `apply_flag` tinyint(3) unsigned NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `send_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`no`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_punish_info_history_2012`
--

DROP TABLE IF EXISTS `member_punish_info_history_2012`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_punish_info_history_2012` (
  `no` int(11) NOT NULL auto_increment,
  `m_id` int(11) NOT NULL default '0',
  `punish_type` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `punish_value` int(11) NOT NULL default '0',
  `apply_flag` tinyint(4) NOT NULL default '0',
  `start_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `end_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `admin_id` varchar(25) NOT NULL default '',
  `reason` text NOT NULL,
  `is_kicked` tinyint(4) NOT NULL default '0',
  `first_ssn` varchar(32) NOT NULL default '',
  `second_ssn` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`no`),
  KEY `idx2` USING BTREE (`m_id`,`punish_type`),
  KEY `idx1` USING BTREE (`occ_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_punish_info_history_2013`
--

DROP TABLE IF EXISTS `member_punish_info_history_2013`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_punish_info_history_2013` (
  `no` int(11) NOT NULL auto_increment,
  `m_id` int(11) NOT NULL default '0',
  `punish_type` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `punish_value` int(11) NOT NULL default '0',
  `apply_flag` tinyint(4) NOT NULL default '0',
  `start_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `end_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `admin_id` varchar(25) NOT NULL default '',
  `reason` text NOT NULL,
  `is_kicked` tinyint(4) NOT NULL default '0',
  `first_ssn` varchar(32) NOT NULL default '',
  `second_ssn` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`no`),
  KEY `idx2` USING BTREE (`m_id`,`punish_type`),
  KEY `idx1` USING BTREE (`occ_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_security_grade`
--

DROP TABLE IF EXISTS `member_security_grade`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_security_grade` (
  `m_id` int(11) NOT NULL default '0',
  `last_visit_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `pass_fail_cnt` int(11) NOT NULL default '0',
  `last_vaccine_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `last_window_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `goblin_pass_mod` datetime NOT NULL default '0000-00-00 00:00:00',
  `goblin_fail_cnt` int(11) NOT NULL default '0',
  `security_card_reg` datetime NOT NULL default '0000-00-00 00:00:00',
  `security_card_fail_cnt` int(11) NOT NULL default '0',
  `m_opt_reg` datetime NOT NULL default '0000-00-00 00:00:00',
  `pc_opt_reg` datetime NOT NULL default '0000-00-00 00:00:00',
  `black_ip_try_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `linear_pass_fail_cnt` int(11) NOT NULL default '0',
  `last_pass_fail_time` int(10) unsigned NOT NULL default '0',
  `last_check_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `pass_modify_check` datetime NOT NULL default '0000-00-00 00:00:00',
  `member_pc_reg` datetime NOT NULL default '0000-00-00 00:00:00',
  `gatekeeper_otp_reg` datetime NOT NULL default '0000-00-00 00:00:00',
  `goblin_validity_time` int(11) NOT NULL default '0',
  `security_card_validity_time` int(11) NOT NULL default '0',
  `validity_ip` varchar(15) NOT NULL default '',
  `cargopad_status` tinyint(4) NOT NULL default '0',
  `cargopad_mod` datetime NOT NULL default '0000-00-00 00:00:00',
  `cargopad_validity_time` int(11) NOT NULL default '0',
  PRIMARY KEY  (`m_id`),
  KEY `idx_pass_check` USING BTREE (`last_pass_fail_time`,`linear_pass_fail_cnt`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `monitoring_logout`
--

DROP TABLE IF EXISTS `monitoring_logout`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `monitoring_logout` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `m_id` int(10) unsigned NOT NULL default '0',
  `logout_time` int(10) unsigned NOT NULL default '0',
  `logout_ip` int(10) unsigned NOT NULL default '0',
  `otp_del_type` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`no`),
  KEY `idx_time` USING BTREE (`logout_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Current Database: `d_technical_report`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `d_technical_report` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `d_technical_report`;

--
-- Table structure for table `accessibility_stat`
--

DROP TABLE IF EXISTS `accessibility_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `accessibility_stat` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `main_type` tinyint(3) unsigned NOT NULL default '0',
  `sub_type` smallint(5) unsigned NOT NULL default '0',
  `val` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`main_type`,`sub_type`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `assert_manager`
--

DROP TABLE IF EXISTS `assert_manager`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `assert_manager` (
  `file_name` varchar(255) NOT NULL default '',
  `file_line` smallint(5) unsigned NOT NULL default '0',
  `reason` varchar(255) NOT NULL default '',
  `cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`file_name`,`file_line`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `captcha_info`
--

DROP TABLE IF EXISTS `captcha_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `captcha_info` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `total_try_count` int(10) unsigned NOT NULL default '0',
  `success_count` int(10) unsigned NOT NULL default '0',
  `fail_count` int(10) unsigned NOT NULL default '0',
  `block_count` int(10) unsigned NOT NULL default '0',
  `incomplete_request_count` int(10) unsigned NOT NULL default '0',
  `invalid_request_count` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `idx_occ_time` USING BTREE (`occ_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `captcha_invalid_request`
--

DROP TABLE IF EXISTS `captcha_invalid_request`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `captcha_invalid_request` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `m_id` int(10) unsigned NOT NULL default '0',
  `request_type` smallint(5) unsigned NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`id`),
  KEY `idx_m_id` USING BTREE (`m_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `captcha_invalid_request_type`
--

DROP TABLE IF EXISTS `captcha_invalid_request_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `captcha_invalid_request_type` (
  `type` smallint(5) unsigned NOT NULL auto_increment,
  `type_desc` varchar(20) NOT NULL default '',
  PRIMARY KEY  (`type`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `challenge_lag_index`
--

DROP TABLE IF EXISTS `challenge_lag_index`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `challenge_lag_index` (
  `spec_id` int(10) unsigned NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `server_group` tinyint(4) NOT NULL default '0',
  `min_fps` smallint(6) NOT NULL default '0',
  `avg_fps` smallint(6) NOT NULL default '0',
  `max_fps` smallint(6) NOT NULL default '0',
  `win_fps` smallint(6) NOT NULL default '0',
  `full_fps` smallint(6) NOT NULL default '0',
  `full_win_fps` smallint(6) NOT NULL default '0',
  `full_win_nosync_fps` smallint(6) NOT NULL default '0',
  `frame1` int(11) NOT NULL default '0',
  `time1` float(7,3) NOT NULL default '0.000',
  `frame2` int(11) NOT NULL default '0',
  `time2` float(7,3) NOT NULL default '0.000',
  `frame3` int(11) NOT NULL default '0',
  `time3` float(7,3) NOT NULL default '0.000',
  `frame4` int(11) NOT NULL default '0',
  `time4` float(7,3) NOT NULL default '0.000',
  `frame5` int(11) NOT NULL default '0',
  `time5` float(7,3) NOT NULL default '0.000',
  `frame6` int(11) NOT NULL default '0',
  `time6` float(7,3) NOT NULL default '0.000',
  `share_rate` int(10) unsigned NOT NULL default '0',
  KEY `idx1` USING BTREE (`spec_id`,`occ_time`,`server_group`),
  KEY `idx2` USING BTREE (`occ_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `challenge_lag_index_daily`
--

DROP TABLE IF EXISTS `challenge_lag_index_daily`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `challenge_lag_index_daily` (
  `occ_date` date NOT NULL default '0000-00-00',
  `spec_id` int(10) unsigned NOT NULL default '0',
  `win_fps` int(10) unsigned NOT NULL default '0',
  `win_fps_cnt` int(11) NOT NULL default '0',
  `full_fps` int(10) unsigned NOT NULL default '0',
  `full_fps_cnt` int(11) NOT NULL default '0',
  `full_win_fps` int(10) unsigned NOT NULL default '0',
  `full_win_fps_cnt` int(11) NOT NULL default '0',
  `full_win_nosync_fps` int(10) unsigned NOT NULL default '0',
  `full_win_nosync_fps_cnt` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`spec_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `collect_interval`
--

DROP TABLE IF EXISTS `collect_interval`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `collect_interval` (
  `start_time` datetime default NULL,
  `end_time` datetime default NULL,
  `value` smallint(5) unsigned default '60'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `common_index`
--

DROP TABLE IF EXISTS `common_index`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `common_index` (
  `spec_id` int(10) unsigned NOT NULL default '0',
  `occ_time` datetime default NULL,
  `server_group` tinyint(4) NOT NULL default '0',
  `share_rate` int(10) unsigned default NULL,
  `crash_count` smallint(5) unsigned default NULL,
  `village_to_dungeon_lag` smallint(6) NOT NULL default '0',
  `dungeon_to_village_lag` smallint(6) NOT NULL default '0',
  `crash_village` smallint(5) unsigned NOT NULL default '0',
  `crash_dungeon` smallint(5) unsigned NOT NULL default '0',
  `crash_challenge` smallint(5) unsigned NOT NULL default '0',
  `crash_wararea` smallint(5) unsigned NOT NULL default '0',
  `crash_fight_village` smallint(5) unsigned NOT NULL default '0',
  `crash_dead_tower` smallint(5) unsigned NOT NULL default '0',
  `crash_channel` smallint(5) unsigned NOT NULL default '0',
  `crash_chaos` smallint(5) unsigned NOT NULL default '0',
  `crash_load` smallint(5) unsigned NOT NULL default '0',
  KEY `idx1` USING BTREE (`spec_id`,`occ_time`,`server_group`),
  KEY `idx2` USING BTREE (`occ_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `common_index_daily`
--

DROP TABLE IF EXISTS `common_index_daily`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `common_index_daily` (
  `occ_date` date NOT NULL default '0000-00-00',
  `server_group` tinyint(4) NOT NULL default '0',
  `spec_id` int(10) unsigned NOT NULL default '0',
  `share_rate` int(10) unsigned NOT NULL default '0',
  `crash_village` int(11) NOT NULL default '0',
  `crash_dungeon` int(11) NOT NULL default '0',
  `crash_challenge` int(11) NOT NULL default '0',
  `crash_wararea` int(11) NOT NULL default '0',
  `crash_fight_village` int(11) NOT NULL default '0',
  `crash_dead_tower` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`server_group`,`spec_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dead_tower_lag_index`
--

DROP TABLE IF EXISTS `dead_tower_lag_index`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dead_tower_lag_index` (
  `spec_id` int(10) unsigned NOT NULL default '0',
  `occ_time` datetime default NULL,
  `server_group` tinyint(4) NOT NULL default '0',
  `min_fps` smallint(6) NOT NULL default '0',
  `avg_fps` smallint(6) NOT NULL default '0',
  `max_fps` smallint(6) NOT NULL default '0',
  `win_fps` smallint(6) NOT NULL default '0',
  `full_fps` smallint(6) NOT NULL default '0',
  `full_win_fps` smallint(6) default '0',
  `full_win_nosync_fps` smallint(6) NOT NULL default '0',
  `frame1` int(11) NOT NULL default '0',
  `time1` float(7,3) NOT NULL default '0.000',
  `frame2` int(11) NOT NULL default '0',
  `time2` float(7,3) NOT NULL default '0.000',
  `frame3` int(11) NOT NULL default '0',
  `time3` float(7,3) NOT NULL default '0.000',
  `frame4` int(11) NOT NULL default '0',
  `time4` float(7,3) NOT NULL default '0.000',
  `frame5` int(11) NOT NULL default '0',
  `time5` float(7,3) NOT NULL default '0.000',
  `frame6` int(11) NOT NULL default '0',
  `time6` float(7,3) NOT NULL default '0.000',
  `share_rate` int(10) unsigned NOT NULL default '0',
  KEY `idx1` USING BTREE (`spec_id`,`occ_time`,`server_group`),
  KEY `idx2` USING BTREE (`occ_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dead_tower_lag_index_daily`
--

DROP TABLE IF EXISTS `dead_tower_lag_index_daily`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dead_tower_lag_index_daily` (
  `occ_date` date NOT NULL default '0000-00-00',
  `spec_id` int(10) unsigned NOT NULL default '0',
  `win_fps` int(10) unsigned NOT NULL default '0',
  `win_fps_cnt` int(11) NOT NULL default '0',
  `full_fps` int(10) unsigned NOT NULL default '0',
  `full_fps_cnt` int(11) NOT NULL default '0',
  `full_win_fps` int(10) unsigned NOT NULL default '0',
  `full_win_fps_cnt` int(11) NOT NULL default '0',
  `full_win_nosync_fps` int(10) unsigned NOT NULL default '0',
  `full_win_nosync_fps_cnt` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`spec_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `deathtower_ting_log`
--

DROP TABLE IF EXISTS `deathtower_ting_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `deathtower_ting_log` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `level` tinyint(3) unsigned NOT NULL default '0',
  `ting_cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`level`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `deathtower_ting_log_daily`
--

DROP TABLE IF EXISTS `deathtower_ting_log_daily`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `deathtower_ting_log_daily` (
  `occ_date` date NOT NULL default '0000-00-00',
  `level` tinyint(3) unsigned NOT NULL default '0',
  `ting_cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`level`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `directx_version`
--

DROP TABLE IF EXISTS `directx_version`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `directx_version` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `server_group` tinyint(4) NOT NULL default '0',
  `ver_etc` int(10) unsigned NOT NULL default '0',
  `ver_8_x` int(10) unsigned NOT NULL default '0',
  `ver_9_0` int(10) unsigned NOT NULL default '0',
  `ver_9_0_a` int(10) unsigned NOT NULL default '0',
  `ver_9_0_b` int(10) unsigned NOT NULL default '0',
  `ver_9_0_c` int(10) unsigned NOT NULL default '0',
  `ver_10_x` int(10) unsigned NOT NULL default '0',
  `ver_11_x` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`server_group`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dungeon_lag_index`
--

DROP TABLE IF EXISTS `dungeon_lag_index`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dungeon_lag_index` (
  `spec_id` int(10) unsigned NOT NULL default '0',
  `occ_time` datetime default NULL,
  `server_group` tinyint(4) NOT NULL default '0',
  `min_fps` smallint(6) NOT NULL default '0',
  `avg_fps` smallint(6) NOT NULL default '0',
  `max_fps` smallint(6) NOT NULL default '0',
  `win_fps` smallint(6) NOT NULL default '0',
  `full_fps` smallint(6) NOT NULL default '0',
  `full_win_fps` smallint(6) default '0',
  `full_win_nosync_fps` smallint(6) NOT NULL default '0',
  `frame1` int(11) NOT NULL default '0',
  `time1` float(7,3) NOT NULL default '0.000',
  `frame2` int(11) NOT NULL default '0',
  `time2` float(7,3) NOT NULL default '0.000',
  `frame3` int(11) NOT NULL default '0',
  `time3` float(7,3) NOT NULL default '0.000',
  `frame4` int(11) NOT NULL default '0',
  `time4` float(7,3) NOT NULL default '0.000',
  `frame5` int(11) NOT NULL default '0',
  `time5` float(7,3) NOT NULL default '0.000',
  `frame6` int(11) NOT NULL default '0',
  `time6` float(7,3) NOT NULL default '0.000',
  `share_rate` int(10) unsigned NOT NULL default '0',
  KEY `idx1` USING BTREE (`spec_id`,`occ_time`,`server_group`),
  KEY `idx2` USING BTREE (`occ_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dungeon_lag_index_daily`
--

DROP TABLE IF EXISTS `dungeon_lag_index_daily`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dungeon_lag_index_daily` (
  `occ_date` date NOT NULL default '0000-00-00',
  `spec_id` int(10) unsigned NOT NULL default '0',
  `win_fps` int(10) unsigned NOT NULL default '0',
  `win_fps_cnt` int(11) NOT NULL default '0',
  `full_fps` int(10) unsigned NOT NULL default '0',
  `full_fps_cnt` int(11) NOT NULL default '0',
  `full_win_fps` int(10) unsigned NOT NULL default '0',
  `full_win_fps_cnt` int(11) NOT NULL default '0',
  `full_win_nosync_fps` int(10) unsigned NOT NULL default '0',
  `full_win_nosync_fps_cnt` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`spec_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `fight_village_lag_index`
--

DROP TABLE IF EXISTS `fight_village_lag_index`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `fight_village_lag_index` (
  `spec_id` int(10) unsigned NOT NULL default '0',
  `occ_time` datetime default NULL,
  `server_group` tinyint(4) NOT NULL default '0',
  `min_fps` smallint(6) NOT NULL default '0',
  `avg_fps` smallint(6) NOT NULL default '0',
  `max_fps` smallint(6) NOT NULL default '0',
  `win_fps` smallint(6) NOT NULL default '0',
  `full_fps` smallint(6) NOT NULL default '0',
  `full_win_fps` smallint(6) default '0',
  `full_win_nosync_fps` smallint(6) NOT NULL default '0',
  `frame1` int(11) NOT NULL default '0',
  `time1` float(7,3) NOT NULL default '0.000',
  `frame2` int(11) NOT NULL default '0',
  `time2` float(7,3) NOT NULL default '0.000',
  `frame3` int(11) NOT NULL default '0',
  `time3` float(7,3) NOT NULL default '0.000',
  `frame4` int(11) NOT NULL default '0',
  `time4` float(7,3) NOT NULL default '0.000',
  `frame5` int(11) NOT NULL default '0',
  `time5` float(7,3) NOT NULL default '0.000',
  `frame6` int(11) NOT NULL default '0',
  `time6` float(7,3) NOT NULL default '0.000',
  `share_rate` int(10) unsigned NOT NULL default '0',
  KEY `idx1` USING BTREE (`spec_id`,`occ_time`,`server_group`),
  KEY `idx2` USING BTREE (`occ_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `fight_village_lag_index_daily`
--

DROP TABLE IF EXISTS `fight_village_lag_index_daily`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `fight_village_lag_index_daily` (
  `occ_date` date NOT NULL default '0000-00-00',
  `spec_id` int(10) unsigned NOT NULL default '0',
  `win_fps` int(10) unsigned NOT NULL default '0',
  `win_fps_cnt` int(11) NOT NULL default '0',
  `full_fps` int(10) unsigned NOT NULL default '0',
  `full_fps_cnt` int(11) NOT NULL default '0',
  `full_win_fps` int(10) unsigned NOT NULL default '0',
  `full_win_fps_cnt` int(11) NOT NULL default '0',
  `full_win_nosync_fps` int(10) unsigned NOT NULL default '0',
  `full_win_nosync_fps_cnt` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`spec_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `lag_stat_dungeon`
--

DROP TABLE IF EXISTS `lag_stat_dungeon`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `lag_stat_dungeon` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `server_id` tinyint(4) NOT NULL default '0',
  `dungeon_idx` int(11) NOT NULL default '0',
  `first_average` int(10) unsigned NOT NULL default '0',
  `first_deviation` int(10) unsigned NOT NULL default '0',
  `first_count` int(11) NOT NULL default '0',
  `boss_average` int(10) unsigned NOT NULL default '0',
  `boss_deviation` int(10) unsigned NOT NULL default '0',
  `boss_count` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`server_id`,`dungeon_idx`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `lag_stat_module`
--

DROP TABLE IF EXISTS `lag_stat_module`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `lag_stat_module` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `server_id` tinyint(4) NOT NULL default '0',
  `module` tinyint(4) NOT NULL default '0',
  `average` int(10) unsigned NOT NULL default '0',
  `deviation` int(10) unsigned NOT NULL default '0',
  `count` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`server_id`,`module`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `loading_time`
--

DROP TABLE IF EXISTS `loading_time`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `loading_time` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `server_id` tinyint(3) unsigned NOT NULL default '0',
  `type` tinyint(3) unsigned NOT NULL default '0',
  `load_sec` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`server_id`,`type`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_launcher_stat`
--

DROP TABLE IF EXISTS `log_launcher_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_launcher_stat` (
  `occ_date` date NOT NULL default '0000-00-00',
  `execute` int(11) NOT NULL default '0',
  `cancel` int(11) NOT NULL default '0',
  `success` int(11) NOT NULL default '0',
  `first_success` int(11) NOT NULL default '0',
  `p2p` double NOT NULL default '0',
  `all_time` bigint(20) NOT NULL default '0',
  `p2p_count` int(11) NOT NULL default '0',
  `all_time_count` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `monitoring_spec`
--

DROP TABLE IF EXISTS `monitoring_spec`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `monitoring_spec` (
  `unique_id` int(10) unsigned NOT NULL auto_increment,
  `modify_time` datetime default NULL,
  `spec_id` int(10) unsigned default NULL,
  `cpu_vendor` tinyint(4) NOT NULL default '0',
  `cpu_processor_num` tinyint(4) NOT NULL default '0',
  `above_cpu_clock` int(11) NOT NULL default '0',
  `below_cpu_clock` int(11) NOT NULL default '0',
  `ram` smallint(6) NOT NULL default '0',
  `videocard_vendor` int(11) NOT NULL default '0',
  `videocard_device` int(11) NOT NULL default '0',
  `videocard_texture_mem` smallint(6) NOT NULL default '0',
  `os_version` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`unique_id`),
  KEY `idx1` USING BTREE (`spec_id`)
) ENGINE=MyISAM AUTO_INCREMENT=301 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `p2p_connect_success_rate`
--

DROP TABLE IF EXISTS `p2p_connect_success_rate`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `p2p_connect_success_rate` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `server_group` tinyint(3) unsigned NOT NULL,
  `connected_type` tinyint(4) NOT NULL,
  `required_time` int(10) unsigned NOT NULL,
  `check_time` int(10) unsigned NOT NULL default '0',
  `nation_code` varchar(15) NOT NULL,
  `peer_address` varchar(15) NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`no`)
) ENGINE=InnoDB AUTO_INCREMENT=122 DEFAULT CHARSET=utf8 COMMENT='대만 P2P 홀펀칭 성공&실패 여결 타입과 IP까지 함께남기는 작업';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `p2p_statistics`
--

DROP TABLE IF EXISTS `p2p_statistics`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `p2p_statistics` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `server_group` tinyint(4) NOT NULL default '0',
  `p2p_user` int(10) unsigned default '0',
  `p2p_min_ping` int(10) unsigned default '0',
  `p2p_max_ping` int(10) unsigned default '0',
  `p2p_avg_ping` int(10) unsigned default '0',
  `p2p_over_ping_100` int(10) unsigned default '0',
  `p2p_over_ping_200` int(10) unsigned default '0',
  `p2p_over_ping_300` int(10) unsigned default '0',
  `p2p_over_ping_400` int(10) unsigned default '0',
  `relay_user` int(10) unsigned default '0',
  `relay_min_ping` int(10) unsigned default '0',
  `relay_max_ping` int(10) unsigned default '0',
  `relay_avg_ping` int(10) unsigned default '0',
  `relay_over_ping_100` int(10) unsigned default '0',
  `relay_over_ping_200` int(10) unsigned default '0',
  `relay_over_ping_300` int(10) unsigned default '0',
  `relay_over_ping_400` int(10) unsigned default '0',
  PRIMARY KEY  (`occ_time`,`server_group`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `p2pnetwork_statistic`
--

DROP TABLE IF EXISTS `p2pnetwork_statistic`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `p2pnetwork_statistic` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `server_group` tinyint(4) NOT NULL default '0',
  `success_party_try` int(11) NOT NULL default '0',
  `total_party_try` int(11) NOT NULL default '0',
  `dungeon_bad_ping` int(11) NOT NULL default '0',
  `dungeon_total` int(11) NOT NULL default '0',
  `pvp_bad_ping` int(11) NOT NULL default '0',
  `pvp_total` int(11) NOT NULL default '0',
  `success_dungeon_clear` int(11) NOT NULL default '0',
  `total_dungeon_clear` int(11) NOT NULL default '0',
  `fair_pvp_total` int(11) default NULL,
  `fair_pvp_bad_ping` int(11) default NULL,
  PRIMARY KEY  (`id`),
  KEY `idx1` USING BTREE (`occ_time`)
) ENGINE=MyISAM AUTO_INCREMENT=17672 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `p2pnetwork_statistic_daily`
--

DROP TABLE IF EXISTS `p2pnetwork_statistic_daily`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `p2pnetwork_statistic_daily` (
  `cur_date` date NOT NULL default '0000-00-00',
  `success_party` float(3,2) NOT NULL default '0.00',
  `dungeon_bad` float(3,2) NOT NULL default '0.00',
  `pvp_bad` float(3,2) NOT NULL default '0.00',
  `success_dungeon_clear` float(3,2) NOT NULL default '0.00',
  `fair_pvp_bad` float(3,2) NOT NULL default '0.00',
  PRIMARY KEY  (`cur_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `packet_overflow`
--

DROP TABLE IF EXISTS `packet_overflow`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `packet_overflow` (
  `packet_type` smallint(5) unsigned NOT NULL default '0',
  `packet_kind` varchar(255) NOT NULL default '',
  `cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`packet_type`,`packet_kind`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `powerwar_lag`
--

DROP TABLE IF EXISTS `powerwar_lag`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `powerwar_lag` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `round` smallint(5) unsigned NOT NULL default '0',
  `player` tinyint(3) unsigned NOT NULL default '0',
  `lag_avg` float unsigned NOT NULL default '0',
  `lag_cnt` float unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`,`occ_time`,`round`),
  KEY `round_idx` USING BTREE (`occ_time`,`round`),
  KEY `player_idx` USING BTREE (`occ_time`,`player`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `powerwar_loading`
--

DROP TABLE IF EXISTS `powerwar_loading`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `powerwar_loading` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `round` smallint(5) unsigned NOT NULL default '0',
  `player` tinyint(3) unsigned NOT NULL default '0',
  `my_loading` smallint(5) unsigned NOT NULL default '0',
  `other_loading` smallint(5) unsigned NOT NULL default '0',
  `vs_loading` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`,`occ_time`,`round`),
  KEY `round_idx` USING BTREE (`occ_time`,`round`),
  KEY `player_idx` USING BTREE (`occ_time`,`player`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `powerwar_ting_type`
--

DROP TABLE IF EXISTS `powerwar_ting_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `powerwar_ting_type` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `server_id` tinyint(3) unsigned NOT NULL default '0',
  `ting_type` tinyint(3) unsigned NOT NULL default '0',
  `ting_cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`server_id`,`ting_type`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spec_info`
--

DROP TABLE IF EXISTS `spec_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `spec_info` (
  `uid` int(10) unsigned NOT NULL auto_increment,
  `vendor_id` int(10) unsigned NOT NULL default '0',
  `device_id` int(10) unsigned NOT NULL default '0',
  `vendor_name` varchar(50) NOT NULL default '',
  `device_name` varchar(120) NOT NULL default '',
  PRIMARY KEY  (`uid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `technical_category`
--

DROP TABLE IF EXISTS `technical_category`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `technical_category` (
  `cateno` int(11) unsigned NOT NULL default '0',
  `pcateno` int(11) unsigned NOT NULL default '0',
  `name` varchar(100) NOT NULL default '',
  `step` int(11) NOT NULL default '0',
  KEY `idx1` USING BTREE (`cateno`,`pcateno`),
  KEY `idx2` USING BTREE (`pcateno`,`cateno`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ting_user_account`
--

DROP TABLE IF EXISTS `ting_user_account`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ting_user_account` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `m_id` int(10) unsigned NOT NULL default '0',
  `minute` tinyint(3) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ting_user_spec`
--

DROP TABLE IF EXISTS `ting_user_spec`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ting_user_spec` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `reg_datetime` datetime NOT NULL default '0000-00-00 00:00:00',
  `cpu_vendor` char(1) NOT NULL default '0',
  `cpu_num` char(1) NOT NULL default '0',
  `cpu_clock` int(10) unsigned NOT NULL default '0',
  `ram` smallint(5) unsigned NOT NULL default '0',
  `video_vendor` smallint(5) unsigned NOT NULL default '0',
  `video_device` smallint(5) unsigned NOT NULL default '0',
  `video_ram` smallint(5) unsigned NOT NULL default '0',
  `os` char(1) NOT NULL default '0',
  `os_bit` char(1) NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `used_memory`
--

DROP TABLE IF EXISTS `used_memory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `used_memory` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `minute_type` char(1) NOT NULL default '0',
  `module` char(1) NOT NULL default '0',
  `memory` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`minute_type`,`module`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `user_ting_timecheck`
--

DROP TABLE IF EXISTS `user_ting_timecheck`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `user_ting_timecheck` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `minute` int(11) NOT NULL default '0',
  `cnt` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`minute`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `village_lag_index`
--

DROP TABLE IF EXISTS `village_lag_index`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `village_lag_index` (
  `spec_id` int(10) unsigned NOT NULL default '0',
  `occ_time` datetime default NULL,
  `server_group` tinyint(4) NOT NULL default '0',
  `min_fps` smallint(6) NOT NULL default '0',
  `avg_fps` smallint(6) NOT NULL default '0',
  `max_fps` smallint(6) NOT NULL default '0',
  `win_fps` smallint(6) NOT NULL default '0',
  `full_fps` smallint(6) NOT NULL default '0',
  `full_win_fps` smallint(6) default '0',
  `full_win_nosync_fps` smallint(6) NOT NULL default '0',
  `frame1` int(11) NOT NULL default '0',
  `time1` float(7,3) NOT NULL default '0.000',
  `frame2` int(11) NOT NULL default '0',
  `time2` float(7,3) NOT NULL default '0.000',
  `frame3` int(11) NOT NULL default '0',
  `time3` float(7,3) NOT NULL default '0.000',
  `frame4` int(11) NOT NULL default '0',
  `time4` float(7,3) NOT NULL default '0.000',
  `frame5` int(11) NOT NULL default '0',
  `time5` float(7,3) NOT NULL default '0.000',
  `frame6` int(11) NOT NULL default '0',
  `time6` float(7,3) NOT NULL default '0.000',
  `share_rate` int(10) unsigned NOT NULL default '0',
  KEY `idx1` USING BTREE (`spec_id`,`occ_time`,`server_group`),
  KEY `idx2` USING BTREE (`occ_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `village_lag_index_daily`
--

DROP TABLE IF EXISTS `village_lag_index_daily`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `village_lag_index_daily` (
  `occ_date` date NOT NULL default '0000-00-00',
  `spec_id` int(10) unsigned NOT NULL default '0',
  `win_fps` int(10) unsigned NOT NULL default '0',
  `win_fps_cnt` int(11) NOT NULL default '0',
  `full_fps` int(10) unsigned NOT NULL default '0',
  `full_fps_cnt` int(11) NOT NULL default '0',
  `full_win_fps` int(10) unsigned NOT NULL default '0',
  `full_win_fps_cnt` int(11) NOT NULL default '0',
  `full_win_nosync_fps` int(10) unsigned NOT NULL default '0',
  `full_win_nosync_fps_cnt` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`spec_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `wararea_lag_index`
--

DROP TABLE IF EXISTS `wararea_lag_index`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `wararea_lag_index` (
  `spec_id` int(10) unsigned NOT NULL default '0',
  `occ_time` datetime default NULL,
  `server_group` tinyint(4) NOT NULL default '0',
  `min_fps` smallint(6) NOT NULL default '0',
  `avg_fps` smallint(6) NOT NULL default '0',
  `max_fps` smallint(6) NOT NULL default '0',
  `win_fps` smallint(6) NOT NULL default '0',
  `full_fps` smallint(6) NOT NULL default '0',
  `full_win_fps` smallint(6) default '0',
  `full_win_nosync_fps` smallint(6) NOT NULL default '0',
  `frame1` int(11) NOT NULL default '0',
  `time1` float(7,3) NOT NULL default '0.000',
  `frame2` int(11) NOT NULL default '0',
  `time2` float(7,3) NOT NULL default '0.000',
  `frame3` int(11) NOT NULL default '0',
  `time3` float(7,3) NOT NULL default '0.000',
  `frame4` int(11) NOT NULL default '0',
  `time4` float(7,3) NOT NULL default '0.000',
  `frame5` int(11) NOT NULL default '0',
  `time5` float(7,3) NOT NULL default '0.000',
  `frame6` int(11) NOT NULL default '0',
  `time6` float(7,3) NOT NULL default '0.000',
  `share_rate` int(10) unsigned NOT NULL default '0',
  KEY `idx1` USING BTREE (`spec_id`,`occ_time`,`server_group`),
  KEY `idx2` USING BTREE (`occ_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `wararea_lag_index_daily`
--

DROP TABLE IF EXISTS `wararea_lag_index_daily`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `wararea_lag_index_daily` (
  `occ_date` date NOT NULL default '0000-00-00',
  `spec_id` int(10) unsigned NOT NULL default '0',
  `win_fps` int(10) unsigned NOT NULL default '0',
  `win_fps_cnt` int(11) NOT NULL default '0',
  `full_fps` int(10) unsigned NOT NULL default '0',
  `full_fps_cnt` int(11) NOT NULL default '0',
  `full_win_fps` int(10) unsigned NOT NULL default '0',
  `full_win_fps_cnt` int(11) NOT NULL default '0',
  `full_win_nosync_fps` int(10) unsigned NOT NULL default '0',
  `full_win_nosync_fps_cnt` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`spec_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Current Database: `gm_gui`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `gm_gui` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `gm_gui`;

--
-- Table structure for table `Pvf_item_other`
--

DROP TABLE IF EXISTS `Pvf_item_other`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Pvf_item_other` (
  `Id` int(11) NOT NULL auto_increment,
  `ItemCode` int(11) NOT NULL,
  `ItemName` varchar(255) default NULL,
  `ShopId` int(11) NOT NULL,
  `Type` tinyint(2) NOT NULL,
  PRIMARY KEY  (`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=2161 DEFAULT CHARSET=utf8 COMMENT='å…¶ä»–ä»£ç è¡¨';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `TownTreeInfoTable`
--

DROP TABLE IF EXISTS `TownTreeInfoTable`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `TownTreeInfoTable` (
  `Id` int(11) NOT NULL auto_increment,
  `Items` longtext NOT NULL,
  PRIMARY KEY  (`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `avatar_fixed_hiddenoption_list`
--

DROP TABLE IF EXISTS `avatar_fixed_hiddenoption_list`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `avatar_fixed_hiddenoption_list` (
  `Id` smallint(6) NOT NULL,
  `Type` tinyint(2) NOT NULL,
  `Name` varchar(255) NOT NULL,
  `MinValue` float NOT NULL,
  `MaxValue` float NOT NULL,
  PRIMARY KEY  (`Id`,`Type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `options`
--

DROP TABLE IF EXISTS `options`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `options` (
  `Id` int(11) NOT NULL auto_increment,
  `RechargeOption_Ratio` int(11) NOT NULL,
  `PostalTitle` varchar(255) default NULL,
  `PostalText` varchar(255) default NULL,
  `CharacUpgradeLevelExpTable` text NOT NULL,
  `AdminId` varchar(255) default NULL,
  PRIMARY KEY  (`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `postal_item_base`
--

DROP TABLE IF EXISTS `postal_item_base`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `postal_item_base` (
  `Id` int(11) NOT NULL auto_increment,
  `Target` tinyint(3) NOT NULL,
  `ItemName` varchar(255) NOT NULL,
  `ItemCode` int(11) NOT NULL,
  `ItemCount` int(11) NOT NULL,
  `MaxCount` int(11) NOT NULL,
  `Upgrade` int(11) NOT NULL,
  `Best` tinyint(1) NOT NULL,
  `AmplifyOptionType` tinyint(3) NOT NULL,
  `AmplifyValue` tinyint(3) NOT NULL,
  `Gold` int(11) NOT NULL,
  `SealFlag` tinyint(1) NOT NULL,
  `SeperateUpgrade` int(11) NOT NULL,
  `Endurance` int(11) NOT NULL,
  `PostalType` tinyint(3) NOT NULL,
  `IsEQU` tinyint(1) default NULL,
  `CreatedAt` datetime NOT NULL,
  `Probability` bigint(20) NOT NULL,
  PRIMARY KEY  (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pvf_item`
--

DROP TABLE IF EXISTS `pvf_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pvf_item` (
  `Id` int(11) NOT NULL auto_increment,
  `ItemCode` int(11) NOT NULL,
  `ItemName` varchar(255) default NULL,
  `IconId` int(11) NOT NULL,
  `IconIndex` int(11) NOT NULL,
  `Rarity` tinyint(2) NOT NULL,
  `ItemCodeType` tinyint(2) NOT NULL,
  `StackLimit` int(11) NOT NULL,
  PRIMARY KEY  (`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=76421 DEFAULT CHARSET=utf8 COMMENT='ä»£ç è¡¨';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Current Database: `mysql`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `mysql` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `mysql`;

--
-- Table structure for table `columns_priv`
--

DROP TABLE IF EXISTS `columns_priv`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `columns_priv` (
  `Host` char(60) collate utf8_bin NOT NULL default '',
  `Db` char(64) collate utf8_bin NOT NULL default '',
  `User` char(16) collate utf8_bin NOT NULL default '',
  `Table_name` char(64) collate utf8_bin NOT NULL default '',
  `Column_name` char(64) collate utf8_bin NOT NULL default '',
  `Timestamp` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `Column_priv` set('Select','Insert','Update','References') character set utf8 NOT NULL default '',
  PRIMARY KEY  (`Host`,`Db`,`User`,`Table_name`,`Column_name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='Column privileges';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `db`
--

DROP TABLE IF EXISTS `db`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `db` (
  `Host` char(60) collate utf8_bin NOT NULL default '',
  `Db` char(64) collate utf8_bin NOT NULL default '',
  `User` char(16) collate utf8_bin NOT NULL default '',
  `Select_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `Insert_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `Update_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `Delete_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `Create_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `Drop_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `Grant_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `References_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `Index_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `Alter_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `Create_tmp_table_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `Lock_tables_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `Create_view_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `Show_view_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `Create_routine_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `Alter_routine_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `Execute_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `Event_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  `Trigger_priv` enum('N','Y') character set utf8 NOT NULL default 'N',
  PRIMARY KEY  (`Host`,`Db`,`User`),
  KEY `User` (`User`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='Database privileges';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event`
--

DROP TABLE IF EXISTS `event`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event` (
  `db` char(64) character set utf8 collate utf8_bin NOT NULL default '',
  `name` char(64) NOT NULL default '',
  `body` longblob NOT NULL,
  `definer` char(77) character set utf8 collate utf8_bin NOT NULL default '',
  `execute_at` datetime default NULL,
  `interval_value` int(11) default NULL,
  `interval_field` enum('YEAR','QUARTER','MONTH','DAY','HOUR','MINUTE','WEEK','SECOND','MICROSECOND','YEAR_MONTH','DAY_HOUR','DAY_MINUTE','DAY_SECOND','HOUR_MINUTE','HOUR_SECOND','MINUTE_SECOND','DAY_MICROSECOND','HOUR_MICROSECOND','MINUTE_MICROSECOND','SECOND_MICROSECOND') default NULL,
  `created` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `modified` timestamp NOT NULL default '0000-00-00 00:00:00',
  `last_executed` datetime default NULL,
  `starts` datetime default NULL,
  `ends` datetime default NULL,
  `status` enum('ENABLED','DISABLED','SLAVESIDE_DISABLED') NOT NULL default 'ENABLED',
  `on_completion` enum('DROP','PRESERVE') NOT NULL default 'DROP',
  `sql_mode` set('REAL_AS_FLOAT','PIPES_AS_CONCAT','ANSI_QUOTES','IGNORE_SPACE','NOT_USED','ONLY_FULL_GROUP_BY','NO_UNSIGNED_SUBTRACTION','NO_DIR_IN_CREATE','POSTGRESQL','ORACLE','MSSQL','DB2','MAXDB','NO_KEY_OPTIONS','NO_TABLE_OPTIONS','NO_FIELD_OPTIONS','MYSQL323','MYSQL40','ANSI','NO_AUTO_VALUE_ON_ZERO','NO_BACKSLASH_ESCAPES','STRICT_TRANS_TABLES','STRICT_ALL_TABLES','NO_ZERO_IN_DATE','NO_ZERO_DATE','INVALID_DATES','ERROR_FOR_DIVISION_BY_ZERO','TRADITIONAL','NO_AUTO_CREATE_USER','HIGH_NOT_PRECEDENCE','NO_ENGINE_SUBSTITUTION','PAD_CHAR_TO_FULL_LENGTH') NOT NULL default '',
  `comment` char(64) character set utf8 collate utf8_bin NOT NULL default '',
  `originator` int(10) unsigned NOT NULL,
  `time_zone` char(64) character set latin1 NOT NULL default 'SYSTEM',
  `character_set_client` char(32) character set utf8 collate utf8_bin default NULL,
  `collation_connection` char(32) character set utf8 collate utf8_bin default NULL,
  `db_collation` char(32) character set utf8 collate utf8_bin default NULL,
  `body_utf8` longblob,
  PRIMARY KEY  (`db`,`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Events';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `func`
--

DROP TABLE IF EXISTS `func`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `func` (
  `name` char(6Error: Couldn't read status information for table general_log ()
mysqldump: Couldn't execute 'show create table `general_log`': Table 'mysql.general_log' doesn't exist (1146)
4) collate utf8_bin NOT NULL default '',
  `ret` tinyint(1) NOT NULL default '0',
  `dl` char(128) collate utf8_bin NOT NULL default '',
  `type` enum('function','aggregate') character set utf8 NOT NULL,
  PRIMARY KEY  (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='User defined functions';
/*!40101 SET character_set_client = @saved_cs_client */;
