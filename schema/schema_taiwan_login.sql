-- MySQL dump 10.11
--
-- Host: localhost    Database: taiwan_login
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
-- Table structure for table `allow_proxy_user`
--

DROP TABLE IF EXISTS `allow_proxy_user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `allow_proxy_user` (
  `m_id` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `auto_punish_blackip_info`
--

DROP TABLE IF EXISTS `auto_punish_blackip_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `auto_punish_blackip_info` (
  `ip` varchar(11) NOT NULL default '',
  `start_ip` tinyint(3) unsigned NOT NULL default '0',
  `end_ip` tinyint(3) unsigned NOT NULL default '0',
  `reg_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `apply_flag` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`ip`,`start_ip`,`end_ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `auto_punish_first_user`
--

DROP TABLE IF EXISTS `auto_punish_first_user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `auto_punish_first_user` (
  `m_id` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `ip` varchar(15) NOT NULL default '',
  `hack_type` smallint(5) unsigned NOT NULL default '0',
  `cnt` int(10) unsigned NOT NULL default '0',
  `punish_flag` tinyint(4) NOT NULL default '0',
  `hack_sub_type` smallint(5) unsigned NOT NULL default '0',
  `hack_sub_cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`,`hack_type`,`hack_sub_type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `auto_punish_hack_full_ip`
--

DROP TABLE IF EXISTS `auto_punish_hack_full_ip`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `auto_punish_hack_full_ip` (
  `occ_date` date NOT NULL default '0000-00-00',
  `hack_type` smallint(5) unsigned NOT NULL default '0',
  `hack_sub_type` smallint(5) unsigned NOT NULL default '0',
  `full_ip` varchar(15) NOT NULL default '',
  `cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`hack_type`,`hack_sub_type`,`full_ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `auto_punish_hack_info`
--

DROP TABLE IF EXISTS `auto_punish_hack_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `auto_punish_hack_info` (
  `hack_type` smallint(5) unsigned NOT NULL default '0',
  `cnt` int(10) unsigned NOT NULL default '0',
  `etc` bigint(20) unsigned NOT NULL default '0',
  `reg_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `apply_flag` tinyint(4) NOT NULL default '0',
  `hack_sub_type` smallint(5) unsigned NOT NULL default '0',
  `hack_sub_cnt` int(10) unsigned NOT NULL default '0',
  `ip_cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`hack_type`,`apply_flag`,`hack_sub_type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `auto_punish_hack_ip`
--

DROP TABLE IF EXISTS `auto_punish_hack_ip`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `auto_punish_hack_ip` (
  `occ_date` date NOT NULL default '0000-00-00',
  `hack_type` smallint(5) unsigned NOT NULL default '0',
  `hack_sub_type` smallint(5) unsigned NOT NULL default '0',
  `c_class_ip` varchar(12) NOT NULL default '',
  `cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`hack_type`,`hack_sub_type`,`c_class_ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `auto_punish_second_log`
--

DROP TABLE IF EXISTS `auto_punish_second_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `auto_punish_second_log` (
  `m_id` int(11) NOT NULL default '0',
  `hack_m_id` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `trade_cnt` int(10) unsigned NOT NULL default '0',
  `trade_gold` bigint(20) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`,`hack_m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `auto_punish_second_user`
--

DROP TABLE IF EXISTS `auto_punish_second_user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `auto_punish_second_user` (
  `m_id` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `total_trade_cnt` int(10) unsigned NOT NULL default '0',
  `trade_cnt` int(10) unsigned NOT NULL default '0',
  `total_trade_gold` bigint(20) unsigned NOT NULL default '0',
  `trade_gold` bigint(20) unsigned NOT NULL default '0',
  `punish_flag` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`m_id`),
  KEY `idx_punish_flag` USING BTREE (`punish_flag`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `churn_member_info`
--

DROP TABLE IF EXISTS `churn_member_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `churn_member_info` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `accrue_cera` int(10) unsigned NOT NULL default '0',
  `play_info` char(30) NOT NULL default '',
  `first_reward_time` int(10) unsigned NOT NULL default '0',
  `last_reward_time` int(10) unsigned NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(10) unsigned NOT NULL default '0',
  `item_id` int(10) unsigned NOT NULL default '0',
  `add_info` tinyint(4) NOT NULL default '0',
  `luck_point` int(10) unsigned NOT NULL default '0',
  `last_update_time` int(10) unsigned NOT NULL default '0',
  `second_reward_time` int(10) unsigned NOT NULL default '0',
  `quest_time` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `churn_reward_history_201506`
--

DROP TABLE IF EXISTS `churn_reward_history_201506`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `churn_reward_history_201506` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(10) unsigned NOT NULL default '0',
  `item_id` int(10) unsigned NOT NULL default '0',
  `add_info` int(10) unsigned NOT NULL default '0',
  `luck_point` int(10) unsigned NOT NULL default '0',
  `reward_order` int(10) unsigned NOT NULL default '0',
  `cera` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`,`occ_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `churn_reward_history_201507`
--

DROP TABLE IF EXISTS `churn_reward_history_201507`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `churn_reward_history_201507` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(10) unsigned NOT NULL default '0',
  `item_id` int(10) unsigned NOT NULL default '0',
  `add_info` int(10) unsigned NOT NULL default '0',
  `luck_point` int(10) unsigned NOT NULL default '0',
  `reward_order` int(10) unsigned NOT NULL default '0',
  `cera` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`,`occ_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `churn_reward_manager`
--

DROP TABLE IF EXISTS `churn_reward_manager`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `churn_reward_manager` (
  `min_day` tinyint(4) NOT NULL default '0',
  `max_day` tinyint(4) NOT NULL default '0',
  `min_val` int(10) unsigned NOT NULL default '0',
  `max_val` int(10) unsigned NOT NULL default '0',
  `item_id` int(10) unsigned NOT NULL default '0',
  `add_info` int(10) unsigned NOT NULL default '0',
  `luck_point` int(10) unsigned NOT NULL default '0',
  `quest_id` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`min_day`,`max_day`,`min_val`,`max_val`,`quest_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `churn_system_manager`
--

DROP TABLE IF EXISTS `churn_system_manager`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `churn_system_manager` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `weekday_var_a` int(11) NOT NULL default '0',
  `weekday_var_b` int(11) NOT NULL default '0',
  `weekday_var_c` int(11) NOT NULL default '0',
  `weekend_var_x` int(11) NOT NULL default '0',
  `weekend_var_y` int(11) NOT NULL default '0',
  `weekend_var_z` int(11) NOT NULL default '0',
  `next_reward_day` int(11) NOT NULL default '0',
  `admin_id` int(10) unsigned NOT NULL default '0',
  `reg_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `state_flag` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_event_entry`
--

DROP TABLE IF EXISTS `dnf_event_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_event_entry` (
  `event_id` int(11) NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `occ_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `obtain_date` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`event_id`,`m_id`),
  KEY `idx_m_id` USING BTREE (`m_id`),
  KEY `idx_charac_no` USING BTREE (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_server_message`
--

DROP TABLE IF EXISTS `event_server_message`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_server_message` (
  `server_info` tinyint(4) unsigned NOT NULL default '0',
  `channel_no` tinyint(4) unsigned NOT NULL default '0',
  `kind` char(1) NOT NULL default '',
  `message_index` char(1) NOT NULL default '',
  `charac_name` char(64) NOT NULL default '',
  `message` char(128) NOT NULL default '',
  `update_time` int(10) unsigned NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gm_manifest`
--

DROP TABLE IF EXISTS `gm_manifest`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `gm_manifest` (
  `m_id` int(11) NOT NULL default '0',
  `level` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `hack_cleanpad_ratio_info`
--

DROP TABLE IF EXISTS `hack_cleanpad_ratio_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `hack_cleanpad_ratio_info` (
  `hack_type` smallint(5) unsigned NOT NULL default '0',
  `value` int(10) unsigned NOT NULL default '0',
  `reg_date` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`hack_type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `human_certify_try_count`
--

DROP TABLE IF EXISTS `human_certify_try_count`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `human_certify_try_count` (
  `m_id` int(11) NOT NULL default '0',
  `count` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ip_monitor_punish`
--

DROP TABLE IF EXISTS `ip_monitor_punish`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ip_monitor_punish` (
  `ip` varchar(15) NOT NULL default '',
  `type` tinyint(4) NOT NULL default '0',
  `m_id_cnt` smallint(5) unsigned NOT NULL default '0',
  `start_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `end_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`ip`,`type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_growth`
--

DROP TABLE IF EXISTS `log_growth`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_growth` (
  `m_id` int(11) NOT NULL default '0',
  `server_info` tinyint(3) unsigned NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `charac_name` varchar(25) NOT NULL default '',
  `job` tinyint(3) unsigned NOT NULL default '0',
  `grow_type` tinyint(3) unsigned NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`m_id`,`server_info`,`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_query_dbmw_ref`
--

DROP TABLE IF EXISTS `log_query_dbmw_ref`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_query_dbmw_ref` (
  `query_hash` varchar(16) NOT NULL default '',
  `q_id` smallint(5) unsigned NOT NULL auto_increment,
  `query` text NOT NULL,
  PRIMARY KEY  (`q_id`),
  UNIQUE KEY `query_hash` USING BTREE (`query_hash`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_query_ref`
--

DROP TABLE IF EXISTS `log_query_ref`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_query_ref` (
  `query_hash` varchar(16) NOT NULL default '',
  `q_id` smallint(5) unsigned NOT NULL auto_increment,
  `query` text NOT NULL,
  PRIMARY KEY  (`q_id`),
  UNIQUE KEY `query_hash` (`query_hash`)
) ENGINE=InnoDB AUTO_INCREMENT=65536 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_query_ref@007e`
--

DROP TABLE IF EXISTS `log_query_ref@007e`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_query_ref@007e` (
  `query_hash` varchar(16) NOT NULL default '',
  `q_id` smallint(5) unsigned NOT NULL auto_increment,
  `query` text NOT NULL,
  PRIMARY KEY  (`q_id`),
  UNIQUE KEY `query_hash` USING BTREE (`query_hash`)
) ENGINE=InnoDB AUTO_INCREMENT=65536 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `login_account_1`
--

DROP TABLE IF EXISTS `login_account_1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `login_account_1` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `m_channel_no` int(11) NOT NULL default '0',
  `login_status` tinyint(1) NOT NULL default '0',
  `last_login_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `login_ip` varchar(15) NOT NULL default '',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `login_account_2`
--

DROP TABLE IF EXISTS `login_account_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `login_account_2` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `m_channel_no` int(11) NOT NULL default '0',
  `login_status` tinyint(1) NOT NULL default '0',
  `last_login_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `login_ip` varchar(15) NOT NULL default '',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `login_account_3`
--

DROP TABLE IF EXISTS `login_account_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `login_account_3` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `m_channel_no` int(11) NOT NULL default '0',
  `login_status` tinyint(1) NOT NULL default '0',
  `last_login_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `login_ip` varchar(15) NOT NULL default '',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `login_history`
--

DROP TABLE IF EXISTS `login_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `login_history` (
  `m_id` int(11) NOT NULL default '0',
  `occ_time` int(11) NOT NULL default '0',
  `trigger` tinyint(4) NOT NULL default '0'
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
-- Table structure for table `member_doubt_trade`
--

DROP TABLE IF EXISTS `member_doubt_trade`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_doubt_trade` (
  `m_id` int(11) NOT NULL default '0',
  `last_update_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `over_count` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_game_option`
--

DROP TABLE IF EXISTS `member_game_option`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_game_option` (
  `m_id` int(11) NOT NULL default '0',
  `option_1` blob NOT NULL,
  `option_2` blob NOT NULL,
  `option_3` blob NOT NULL,
  `shortcut_emoticon` blob NOT NULL,
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_game_option_1`
--

DROP TABLE IF EXISTS `member_game_option_1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_game_option_1` (
  `m_id` int(11) NOT NULL default '0',
  `option_1` blob NOT NULL,
  `option_2` blob NOT NULL,
  `option_3` blob NOT NULL,
  PRIMARY KEY  (`m_id`)
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
-- Table structure for table `member_key_option`
--

DROP TABLE IF EXISTS `member_key_option`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_key_option` (
  `m_id` bigint(20) unsigned NOT NULL default '0',
  `key_type` tinyint(4) NOT NULL default '0',
  `key_option` blob NOT NULL,
  PRIMARY KEY  (`m_id`,`key_type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_login`
--

DROP TABLE IF EXISTS `member_login`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_login` (
  `m_id` int(11) NOT NULL default '0',
  `login_time` int(10) unsigned NOT NULL default '0',
  `expire_time` int(10) unsigned NOT NULL default '0',
  `last_play_time` int(10) unsigned NOT NULL default '0',
  `total_account_fail` int(10) unsigned NOT NULL default '0',
  `account_fail` tinyint(4) NOT NULL default '0',
  `report_cnt` int(11) NOT NULL default '0',
  `reliable_flag` tinyint(4) NOT NULL default '0',
  `trade_gold_daily` int(10) unsigned NOT NULL default '0',
  `last_gift_time` int(10) unsigned NOT NULL default '0',
  `gift_cnt` smallint(5) unsigned NOT NULL default '0',
  `login_ip` varchar(15) NOT NULL default '',
  `security_flag` tinyint(4) NOT NULL default '0',
  `power_side` tinyint(4) NOT NULL default '0',
  `dungeon_gain_gold` int(10) unsigned NOT NULL default '0',
  `school_id` int(11) NOT NULL default '0',
  `rating` float NOT NULL default '0',
  `cleanpad_point` int(10) unsigned NOT NULL default '0',
  `tutorial_skipable` char(1) NOT NULL default '0',
  `event_charac_flag` tinyint(3) unsigned NOT NULL default '0',
  `garena_token_key` bigint(20) NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_login_backup`
--

DROP TABLE IF EXISTS `member_login_backup`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_login_backup` (
  `m_id` int(11) NOT NULL default '0',
  `login_time` int(10) unsigned NOT NULL default '0',
  `expire_time` int(10) unsigned NOT NULL default '0',
  `last_play_time` int(10) unsigned NOT NULL default '0',
  `total_account_fail` int(10) unsigned NOT NULL default '0',
  `account_fail` tinyint(4) NOT NULL default '0',
  `report_cnt` int(11) NOT NULL default '0',
  `reliable_flag` tinyint(4) NOT NULL default '0',
  `trade_gold_daily` int(10) unsigned NOT NULL default '0',
  `last_gift_time` int(10) unsigned NOT NULL default '0',
  `gift_cnt` smallint(5) unsigned NOT NULL default '0',
  `login_ip` varchar(15) NOT NULL default '',
  `security_flag` tinyint(4) NOT NULL default '0',
  `power_side` tinyint(4) NOT NULL default '0',
  `dungeon_gain_gold` int(10) unsigned NOT NULL default '0',
  `school_id` int(11) NOT NULL default '0',
  `rating` float NOT NULL default '0',
  `cleanpad_point` int(10) unsigned NOT NULL default '0',
  `tutorial_skipable` char(1) NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_mousepass`
--

DROP TABLE IF EXISTS `member_mousepass`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_mousepass` (
  `m_id` int(11) NOT NULL default '0',
  `mousepass` varchar(32) NOT NULL default '',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `fail_cnt` tinyint(4) NOT NULL default '0',
  `cancel_cnt` smallint(5) unsigned NOT NULL default '0',
  `version_info` char(1) NOT NULL default '1',
  `validity_time` int(11) NOT NULL default '0',
  `reward_time` int(11) NOT NULL default '0',
  `enable_flag` char(1) NOT NULL default ''
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_mousepass_history`
--

DROP TABLE IF EXISTS `member_mousepass_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_mousepass_history` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `m_id` int(11) NOT NULL default '0',
  `pre_mousepass` varchar(32) NOT NULL default '',
  `modify_type` tinyint(4) NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_mousepass_history_201506`
--

DROP TABLE IF EXISTS `member_mousepass_history_201506`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_mousepass_history_201506` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `m_id` int(11) NOT NULL default '0',
  `pre_mousepass` varchar(32) NOT NULL default '',
  `modify_type` tinyint(4) NOT NULL default '0',
  `ip_info` varchar(15) NOT NULL default '',
  `port_info` varchar(5) NOT NULL default '',
  PRIMARY KEY  (`occ_time`,`m_id`),
  KEY `idx_m_id` USING BTREE (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_mousepass_history_201604`
--

DROP TABLE IF EXISTS `member_mousepass_history_201604`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_mousepass_history_201604` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `m_id` int(11) NOT NULL default '0',
  `pre_mousepass` varchar(32) NOT NULL default '',
  `modify_type` tinyint(4) NOT NULL default '0',
  `ip_info` varchar(15) NOT NULL default '',
  `port_info` varchar(5) NOT NULL default '',
  PRIMARY KEY  (`occ_time`,`m_id`),
  KEY `idx_m_id` USING BTREE (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_play_info`
--

DROP TABLE IF EXISTS `member_play_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_play_info` (
  `occ_date` date NOT NULL default '0000-00-00',
  `m_id` int(11) NOT NULL default '0',
  `play_time` int(10) unsigned NOT NULL default '0',
  `play_count` int(10) unsigned NOT NULL default '0',
  `trade_cnt` int(11) NOT NULL default '0',
  `exp` int(10) unsigned NOT NULL default '0',
  `used_fatigue` smallint(5) unsigned NOT NULL default '0',
  `ip` varchar(15) NOT NULL default '',
  `last_play_time` int(10) unsigned NOT NULL default '0',
  `pcbang_flag` tinyint(4) NOT NULL default '0',
  `end_ip` varchar(3) NOT NULL default '',
  `ting_count` smallint(5) unsigned NOT NULL default '0',
  `mac_addr` varchar(64) NOT NULL default '',
  `server_id` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`m_id`),
  KEY `idx_m_id` USING BTREE (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_play_info_20130502`
--

DROP TABLE IF EXISTS `member_play_info_20130502`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_play_info_20130502` (
  `occ_date` date NOT NULL default '0000-00-00',
  `m_id` int(11) NOT NULL default '0',
  `play_time` int(10) unsigned NOT NULL default '0',
  `play_count` int(10) unsigned NOT NULL default '0',
  `trade_cnt` int(11) NOT NULL default '0',
  `exp` int(10) unsigned NOT NULL default '0',
  `used_fatigue` smallint(5) unsigned NOT NULL default '0',
  `ip` varchar(15) NOT NULL default '',
  `last_play_time` int(10) unsigned NOT NULL default '0',
  `pcbang_flag` tinyint(4) NOT NULL default '0',
  `end_ip` varchar(3) NOT NULL default '',
  `ting_count` smallint(5) unsigned NOT NULL default '0',
  `mac_addr` varchar(64) NOT NULL default '',
  `server_id` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`m_id`),
  KEY `idx_m_id` USING BTREE (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_play_info_del`
--

DROP TABLE IF EXISTS `member_play_info_del`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_play_info_del` (
  `sdate` date NOT NULL default '0000-00-00',
  `occ_date` date NOT NULL default '0000-00-00',
  `m_id` int(11) NOT NULL default '0',
  `play_time` int(10) unsigned NOT NULL default '0',
  `play_count` int(10) unsigned NOT NULL default '0',
  `trade_cnt` int(11) NOT NULL default '0',
  `exp` int(10) unsigned NOT NULL default '0',
  `used_fatigue` smallint(5) unsigned NOT NULL default '0',
  `ip` varchar(15) NOT NULL default '',
  `last_play_time` int(10) unsigned NOT NULL default '0',
  `pcbang_flag` tinyint(4) NOT NULL default '0',
  `end_ip` varchar(3) NOT NULL default '',
  `ting_count` smallint(5) unsigned NOT NULL default '0',
  `mac_addr` varchar(64) NOT NULL default '',
  `server_id` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`sdate`,`occ_date`,`m_id`),
  KEY `idx_m_id` USING BTREE (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_premium`
--

DROP TABLE IF EXISTS `member_premium`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_premium` (
  `event_id` int(11) NOT NULL default '0',
  `pre_type` tinyint(3) unsigned NOT NULL default '0',
  `server_id` tinyint(3) unsigned NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `service_start` datetime NOT NULL default '0000-00-00 00:00:00',
  `service_end` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`event_id`,`pre_type`,`server_id`,`m_id`,`service_start`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_premium_old`
--

DROP TABLE IF EXISTS `member_premium_old`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_premium_old` (
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
-- Table structure for table `member_security_card`
--

DROP TABLE IF EXISTS `member_security_card`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_security_card` (
  `m_id` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `phone` varchar(11) NOT NULL default '',
  `cert_key` varchar(12) NOT NULL default '',
  `server_key` varchar(32) NOT NULL default '',
  `card` varchar(255) NOT NULL default '',
  `fail_cnt` tinyint(4) NOT NULL default '0',
  `re_issue_cnt` tinyint(4) NOT NULL default '0',
  `last_issue_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `validity_time` int(11) NOT NULL default '0',
  `apply_flag` tinyint(4) NOT NULL default '0',
  `cancel_cnt` smallint(5) unsigned NOT NULL default '0',
  `web_flag` tinyint(4) NOT NULL default '0',
  `cert_flag` char(1) NOT NULL default '0',
  PRIMARY KEY  (`m_id`),
  KEY `idx_phone` USING BTREE (`phone`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_security_card_history`
--

DROP TABLE IF EXISTS `member_security_card_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_security_card_history` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `m_id` int(11) NOT NULL default '0',
  `modify_type` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `nexon_none_memner_restriction`
--

DROP TABLE IF EXISTS `nexon_none_memner_restriction`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `nexon_none_memner_restriction` (
  `m_id` int(11) unsigned NOT NULL default '0',
  `charac_id` int(11) unsigned NOT NULL default '0',
  `last_trade_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `total_trade_gold` int(12) unsigned NOT NULL default '0',
  `trade_count` smallint(6) unsigned NOT NULL default '0',
  `nexon_user` tinyint(4) NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `test`
--

DROP TABLE IF EXISTS `test`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `test` (
  `id` int(111) NOT NULL auto_increment,
  `hashs` varchar(16) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `village_attacked_charac_point_rank`
--

DROP TABLE IF EXISTS `village_attacked_charac_point_rank`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `village_attacked_charac_point_rank` (
  `server_info` tinyint(3) unsigned NOT NULL default '0',
  `occ_date` date NOT NULL default '0000-00-00',
  `charac_no` int(10) unsigned NOT NULL default '0',
  `hunting_point` int(10) unsigned NOT NULL default '0',
  `rank` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`server_info`,`occ_date`,`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `village_attacked_server_point_rank`
--

DROP TABLE IF EXISTS `village_attacked_server_point_rank`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `village_attacked_server_point_rank` (
  `server_info` tinyint(3) unsigned NOT NULL default '0',
  `occ_date` date NOT NULL default '0000-00-00',
  `hunting_point` int(10) unsigned NOT NULL default '0',
  `rank` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`server_info`,`occ_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `village_attacked_server_time_rank`
--

DROP TABLE IF EXISTS `village_attacked_server_time_rank`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `village_attacked_server_time_rank` (
  `server_info` tinyint(3) unsigned NOT NULL default '0',
  `occ_date` date NOT NULL default '0000-00-00',
  `clear_time` int(10) unsigned NOT NULL default '0',
  `rank` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`server_info`,`occ_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2025-10-20 11:32:26
