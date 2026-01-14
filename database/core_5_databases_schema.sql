-- MySQL dump 10.11
--
-- Host: localhost    Database: d_taiwan
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
-- Current Database: `taiwan_cain`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `taiwan_cain` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `taiwan_cain`;

--
-- Table structure for table `account_cargo`
--

DROP TABLE IF EXISTS `account_cargo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `account_cargo` (
  `m_id` bigint(20) unsigned NOT NULL default '0',
  `money` int(11) unsigned NOT NULL default '0',
  `capacity` tinyint(3) unsigned NOT NULL default '0',
  `cargo` blob NOT NULL,
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `auction_history`
--

DROP TABLE IF EXISTS `auction_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `auction_history` (
  `auction_id` bigint(20) unsigned NOT NULL default '0',
  `start_time` datetime default NULL,
  `occ_time` datetime default NULL,
  `event_type` tinyint(4) default NULL,
  `owner_id` int(11) default NULL,
  `buyer_id` int(11) default NULL,
  `price` int(11) default NULL,
  `seal_flag` tinyint(4) default NULL,
  `item_id` int(10) unsigned default NULL,
  `add_info` int(11) default NULL,
  `upgrade` tinyint(3) unsigned default NULL,
  `amplify_option` tinyint(3) unsigned NOT NULL default '0',
  `amplify_value` mediumint(8) unsigned NOT NULL default '0',
  `seal_cnt` tinyint(3) unsigned default NULL,
  `endurance` smallint(5) unsigned default NULL,
  `extend_info` int(10) unsigned default NULL,
  `owner_postal_id` int(10) unsigned default NULL,
  `buyer_postal_id` int(10) unsigned default NULL,
  `unit_price` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`auction_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `aura_avatar_option`
--

DROP TABLE IF EXISTS `aura_avatar_option`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `aura_avatar_option` (
  `charac_no` int(11) NOT NULL default '0',
  `option_type` tinyint(4) NOT NULL default '0',
  `value_1` int(11) NOT NULL default '0',
  PRIMARY KEY  (`charac_no`,`option_type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `auto_market_condition_ctrl`
--

DROP TABLE IF EXISTS `auto_market_condition_ctrl`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `auto_market_condition_ctrl` (
  `optimum_gold_supply` bigint(20) unsigned NOT NULL default '0',
  `over_gold` bigint(20) unsigned NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `auto_market_condition_ctrl_change`
--

DROP TABLE IF EXISTS `auto_market_condition_ctrl_change`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `auto_market_condition_ctrl_change` (
  `occ_time` date NOT NULL default '0000-00-00',
  `total_gold_old` bigint(20) unsigned NOT NULL default '0',
  `over_gold_old` bigint(20) unsigned NOT NULL default '0',
  `total_gold_new` bigint(20) unsigned NOT NULL default '0',
  `over_gold_new` bigint(20) unsigned NOT NULL default '0',
  `MNG_user_id` varchar(30) NOT NULL default '',
  `memo` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`occ_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `auto_market_condition_ctrl_daily`
--

DROP TABLE IF EXISTS `auto_market_condition_ctrl_daily`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `auto_market_condition_ctrl_daily` (
  `occ_time` date NOT NULL default '0000-00-00',
  `total_gold` bigint(20) unsigned NOT NULL default '0',
  `auction_gold` bigint(20) unsigned NOT NULL default '0',
  `over_gold` bigint(20) unsigned NOT NULL default '0',
  `optimum_gold_supply` bigint(20) unsigned NOT NULL default '0',
  `gold_phase` int(11) NOT NULL default '0',
  `item_phase` int(11) NOT NULL default '0',
  `durability_phase` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_20130408`
--

DROP TABLE IF EXISTS `bak_20130408`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_20130408` (
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '1'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_50_mid`
--

DROP TABLE IF EXISTS `bak_50_mid`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_50_mid` (
  `m_id` int(10) unsigned NOT NULL default '0',
  KEY `m_id` USING BTREE (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_charac_info_20130219`
--

DROP TABLE IF EXISTS `bak_charac_info_20130219`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_charac_info_20130219` (
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL auto_increment,
  `charac_name` varchar(20) NOT NULL default '',
  `village` tinyint(4) NOT NULL default '1',
  `job` tinyint(4) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '1',
  `exp` int(11) NOT NULL default '0',
  `grow_type` tinyint(4) NOT NULL default '0',
  `HP` tinyint(3) unsigned NOT NULL default '0',
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
  `fatigue` smallint(6) NOT NULL default '0',
  `max_fatigue` smallint(6) NOT NULL default '70',
  `premium_fatigue` smallint(6) NOT NULL default '0',
  `max_premium_fatigue` smallint(6) NOT NULL default '0',
  `create_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `dungeon_clear_point` int(11) NOT NULL default '0',
  `delete_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `delete_flag` tinyint(4) NOT NULL default '0',
  `guild_id` int(10) unsigned NOT NULL default '0',
  `guild_right` tinyint(4) NOT NULL default '0',
  `member_flag` tinyint(4) NOT NULL default '0',
  `sex` tinyint(4) NOT NULL default '1',
  `expert_job` tinyint(3) unsigned NOT NULL default '0',
  `skill_tree_index` tinyint(4) NOT NULL default '-1',
  `link_charac_no` int(10) unsigned NOT NULL default '0',
  `event_charac_level` tinyint(4) NOT NULL default '0',
  `guild_secede` tinyint(2) NOT NULL default '0',
  `start_time` int(11) NOT NULL default '0',
  `finish_time` int(11) NOT NULL default '0',
  `competition_area` tinyint(2) NOT NULL default '-1',
  `competition_period` tinyint(2) NOT NULL default '-1',
  `mercenary_start_time` int(11) NOT NULL default '0',
  `mercenary_finish_time` int(11) NOT NULL default '0',
  `mercenary_area` tinyint(4) NOT NULL default '-1',
  `mercenary_period` tinyint(4) NOT NULL default '-1',
  PRIMARY KEY  (`charac_no`),
  UNIQUE KEY `charac_name` USING BTREE (`charac_name`),
  KEY `charac_info_idx1` USING BTREE (`m_id`),
  KEY `charac_info_idx2` USING BTREE (`exp`),
  KEY `idx_guild_id` USING BTREE (`guild_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_charac_info_20130228`
--

DROP TABLE IF EXISTS `bak_charac_info_20130228`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_charac_info_20130228` (
  `m_id` int(11) NOT NULL default '0',
  `CNT` bigint(21) NOT NULL default '0',
  `user_id` varchar(30) NOT NULL default '',
  KEY `m_id` USING BTREE (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_charac_info_20130228_2`
--

DROP TABLE IF EXISTS `bak_charac_info_20130228_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_charac_info_20130228_2` (
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '1',
  `user_id` varchar(30) NOT NULL default ''
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_charac_info_20130228_3`
--

DROP TABLE IF EXISTS `bak_charac_info_20130228_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_charac_info_20130228_3` (
  `m_id` int(11) NOT NULL default '0',
  `user_id` varchar(30) NOT NULL default '',
  `CNT` bigint(21) NOT NULL default '0',
  KEY `m_id` USING BTREE (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_charac_info_20130326_1_1`
--

DROP TABLE IF EXISTS `bak_charac_info_20130326_1_1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_charac_info_20130326_1_1` (
  `m_id` int(11) NOT NULL default '0',
  `cnt` bigint(21) NOT NULL default '0',
  `user_id` varchar(30) NOT NULL default '',
  KEY `m_id` USING BTREE (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_charac_info_20130326_1_2`
--

DROP TABLE IF EXISTS `bak_charac_info_20130326_1_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_charac_info_20130326_1_2` (
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '1',
  `user_id` varchar(30) NOT NULL default '',
  `create_time` datetime NOT NULL default '0000-00-00 00:00:00'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_charac_no_20130405`
--

DROP TABLE IF EXISTS `bak_charac_no_20130405`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_charac_no_20130405` (
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '1',
  `server_info` tinyint(3) unsigned NOT NULL default '0',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_m_id_20130405`
--

DROP TABLE IF EXISTS `bak_m_id_20130405`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_m_id_20130405` (
  `m_id` int(10) unsigned NOT NULL default '0',
  KEY `m_id` USING BTREE (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_m_id_20130408`
--

DROP TABLE IF EXISTS `bak_m_id_20130408`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_m_id_20130408` (
  `m_id` int(10) unsigned NOT NULL default '0',
  KEY `m_id` USING BTREE (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_m_id_20130424_ccb`
--

DROP TABLE IF EXISTS `bak_m_id_20130424_ccb`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_m_id_20130424_ccb` (
  `m_id` int(10) unsigned NOT NULL default '0',
  KEY `m_id` USING BTREE (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_m_id_cbt`
--

DROP TABLE IF EXISTS `bak_m_id_cbt`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_m_id_cbt` (
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `create_time` datetime NOT NULL default '0000-00-00 00:00:00'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_m_id_cbt_2`
--

DROP TABLE IF EXISTS `bak_m_id_cbt_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_m_id_cbt_2` (
  `m_id` int(11) NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_m_id_cbt_3`
--

DROP TABLE IF EXISTS `bak_m_id_cbt_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_m_id_cbt_3` (
  `m_id` int(11) NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_m_id_charac_no_20130408`
--

DROP TABLE IF EXISTS `bak_m_id_charac_no_20130408`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_m_id_charac_no_20130408` (
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '1',
  `server_info` tinyint(3) unsigned NOT NULL default '0',
  KEY `charac_no` USING BTREE (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_member_info_20130228`
--

DROP TABLE IF EXISTS `bak_member_info_20130228`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_member_info_20130228` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `user_id` varchar(30) NOT NULL default '',
  `user_name` varchar(10) NOT NULL default '',
  KEY `m_id` USING BTREE (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_member_info_20130326`
--

DROP TABLE IF EXISTS `bak_member_info_20130326`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_member_info_20130326` (
  `m_id` int(11) NOT NULL default '0',
  `user_id` varchar(30) default NULL,
  KEY `m_id` USING BTREE (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_mid_20130408`
--

DROP TABLE IF EXISTS `bak_mid_20130408`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_mid_20130408` (
  `m_id` int(10) unsigned NOT NULL default '0',
  KEY `m_id` USING BTREE (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_pvp_20130221`
--

DROP TABLE IF EXISTS `bak_pvp_20130221`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_pvp_20130221` (
  `charac_no` int(11) NOT NULL default '0',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_pvp_20130222`
--

DROP TABLE IF EXISTS `bak_pvp_20130222`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_pvp_20130222` (
  `charac_no` int(11) NOT NULL default '0',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_pvp_20130223`
--

DROP TABLE IF EXISTS `bak_pvp_20130223`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_pvp_20130223` (
  `charac_no` int(11) NOT NULL default '0',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_pvp_20130224`
--

DROP TABLE IF EXISTS `bak_pvp_20130224`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_pvp_20130224` (
  `charac_no` int(11) NOT NULL default '0',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_result_20130408`
--

DROP TABLE IF EXISTS `bak_result_20130408`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_result_20130408` (
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '1',
  `server_info` tinyint(3) unsigned NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_result_50`
--

DROP TABLE IF EXISTS `bak_result_50`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_result_50` (
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '1',
  `server_info` tinyint(3) unsigned NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_sooya_result_20130405`
--

DROP TABLE IF EXISTS `bak_sooya_result_20130405`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_sooya_result_20130405` (
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '1',
  `server_info` tinyint(3) unsigned NOT NULL default '0',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_sooya_result_20130408`
--

DROP TABLE IF EXISTS `bak_sooya_result_20130408`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_sooya_result_20130408` (
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '1',
  `server_info` tinyint(3) unsigned NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_test_20130515`
--

DROP TABLE IF EXISTS `bak_test_20130515`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_test_20130515` (
  `a` int(10) unsigned NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `blood_dungeon_rank_select`
--

DROP TABLE IF EXISTS `blood_dungeon_rank_select`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `blood_dungeon_rank_select` (
  `min_amount` bigint(20) NOT NULL default '0',
  `max_amount` bigint(20) NOT NULL default '0',
  `rank` tinyint(3) unsigned NOT NULL default '0',
  `reward_item_id` int(10) unsigned NOT NULL default '0',
  `reward_gold` int(10) unsigned NOT NULL default '0',
  `winner_count` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`min_amount`,`max_amount`,`rank`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `channel_occ_info`
--

DROP TABLE IF EXISTS `channel_occ_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `channel_occ_info` (
  `gc_no` int(10) unsigned NOT NULL default '0',
  `age` tinyint(3) unsigned NOT NULL default '0',
  `occ_num` smallint(6) NOT NULL default '0',
  PRIMARY KEY  (`gc_no`,`age`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_achievement`
--

DROP TABLE IF EXISTS `charac_achievement`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_achievement` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `achievement` blob NOT NULL,
  `last_update_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_action_point`
--

DROP TABLE IF EXISTS `charac_action_point`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_action_point` (
  `occ_date` date NOT NULL default '0000-00-00',
  `charac_no` int(10) unsigned NOT NULL default '0',
  `ap_sum` int(10) unsigned NOT NULL default '0',
  `is_reward_medal` tinyint(3) unsigned NOT NULL default '0',
  `is_reward_item_1` tinyint(3) unsigned NOT NULL default '0',
  `is_reward_item_2` tinyint(3) unsigned NOT NULL default '0',
  `is_reward_item_3` tinyint(3) unsigned NOT NULL default '0',
  `is_reward_item_4` tinyint(3) unsigned NOT NULL default '0',
  `ap_clear_state` blob NOT NULL,
  PRIMARY KEY  (`charac_no`,`occ_date`),
  KEY `idx_occ_date` USING BTREE (`occ_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_action_point_desc`
--

DROP TABLE IF EXISTS `charac_action_point_desc`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_action_point_desc` (
  `action_group_index` int(11) NOT NULL default '0',
  `action_index` int(11) NOT NULL default '0',
  `action_group_name` varchar(128) default NULL,
  PRIMARY KEY  (`action_group_index`,`action_index`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_best_record`
--

DROP TABLE IF EXISTS `charac_best_record`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_best_record` (
  `charac_no` int(11) NOT NULL default '0',
  `dungeon_no` smallint(6) NOT NULL default '0',
  `difficulty` smallint(6) NOT NULL default '0',
  `style` int(11) NOT NULL default '0',
  `technic` int(11) NOT NULL default '0',
  `attacked` int(11) NOT NULL default '0',
  `rank` int(11) NOT NULL default '0',
  PRIMARY KEY  (`charac_no`,`dungeon_no`,`difficulty`),
  KEY `idx_charac_no` USING BTREE (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_blood_best_record`
--

DROP TABLE IF EXISTS `charac_blood_best_record`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_blood_best_record` (
  `charac_no` int(11) unsigned NOT NULL default '0',
  `dungeon_index` int(11) unsigned NOT NULL default '0',
  `best_round` tinyint(3) unsigned NOT NULL default '0',
  `best_time` int(11) NOT NULL default '0',
  PRIMARY KEY  (`charac_no`,`dungeon_index`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_blood_dungeon_reward`
--

DROP TABLE IF EXISTS `charac_blood_dungeon_reward`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_blood_dungeon_reward` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `week_occ_date` date NOT NULL default '0000-00-00',
  `week_point` int(10) unsigned NOT NULL default '0',
  `week_enter_count` int(10) unsigned NOT NULL default '0',
  `week_use_gold` int(10) unsigned NOT NULL default '0',
  `last_play_date` date NOT NULL default '0000-00-00',
  `enter_count` int(10) unsigned NOT NULL default '0',
  `rank` tinyint(3) unsigned NOT NULL default '0',
  `reward` tinyint(3) unsigned NOT NULL default '0',
  `reward_item_id` int(10) unsigned NOT NULL default '0',
  `reward_gold` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`charac_no`,`week_occ_date`),
  KEY `idx_week_occ_date` USING BTREE (`week_occ_date`),
  KEY `idx_last_play_date` USING BTREE (`last_play_date`),
  KEY `idx_reward` USING BTREE (`reward`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_blood_inout`
--

DROP TABLE IF EXISTS `charac_blood_inout`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_blood_inout` (
  `charac_no` int(11) NOT NULL default '0',
  `dungeon1` tinyint(4) NOT NULL default '0',
  `dungeon2` tinyint(4) NOT NULL default '0',
  `dungeon3` tinyint(4) NOT NULL default '0',
  `dungeon4` tinyint(4) NOT NULL default '0',
  `dungeon5` tinyint(4) NOT NULL default '0',
  `dungeon6` tinyint(4) NOT NULL default '0',
  `dungeon7` tinyint(4) NOT NULL default '0',
  `dungeon8` tinyint(4) NOT NULL default '0',
  `dungeon9` tinyint(4) NOT NULL default '0',
  `dungeon10` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_cerashop_restrict`
--

DROP TABLE IF EXISTS `charac_cerashop_restrict`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_cerashop_restrict` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `ipg_no` int(10) unsigned NOT NULL default '0',
  `count` int(10) unsigned NOT NULL default '0',
  `next_date` int(10) unsigned NOT NULL default '0',
  `end_date` int(10) unsigned NOT NULL default '0',
  `last_access_date` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`charac_no`,`ipg_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_dimension_inout`
--

DROP TABLE IF EXISTS `charac_dimension_inout`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_dimension_inout` (
  `charac_no` int(11) NOT NULL default '0',
  `dungeon1` tinyint(4) NOT NULL default '0',
  `dungeon2` tinyint(4) NOT NULL default '0',
  `dungeon3` tinyint(4) NOT NULL default '0',
  `dungeon4` tinyint(4) NOT NULL default '0',
  `dungeon5` tinyint(4) NOT NULL default '0',
  `dungeon6` tinyint(4) NOT NULL default '0',
  `dungeon7` tinyint(4) NOT NULL default '0',
  `dungeon8` tinyint(4) NOT NULL default '0',
  `dungeon9` tinyint(4) NOT NULL default '0',
  `dungeon10` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_dimension_inout_20130613`
--

DROP TABLE IF EXISTS `charac_dimension_inout_20130613`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_dimension_inout_20130613` (
  `charac_no` int(11) NOT NULL default '0',
  `dungeon1` tinyint(4) NOT NULL default '0',
  `dungeon2` tinyint(4) NOT NULL default '0',
  `dungeon3` tinyint(4) NOT NULL default '0',
  `dungeon4` tinyint(4) NOT NULL default '0',
  `dungeon5` tinyint(4) NOT NULL default '0',
  `dungeon6` tinyint(4) NOT NULL default '0',
  `dungeon7` tinyint(4) NOT NULL default '0',
  `dungeon8` tinyint(4) NOT NULL default '0',
  `dungeon9` tinyint(4) NOT NULL default '0',
  `dungeon10` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_dungeon`
--

DROP TABLE IF EXISTS `charac_dungeon`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_dungeon` (
  `charac_no` int(11) NOT NULL default '0',
  `dungeon` blob NOT NULL,
  `best_clear_time` blob NOT NULL,
  `blue_marble_enter_count` tinyint(3) unsigned NOT NULL default '0',
  `charac_inform_notice` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_dungeon_test`
--

DROP TABLE IF EXISTS `charac_dungeon_test`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_dungeon_test` (
  `charac_no` int(11) NOT NULL default '0',
  `dungeon` blob NOT NULL,
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_expert_job`
--

DROP TABLE IF EXISTS `charac_expert_job`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_expert_job` (
  `charac_no` int(11) NOT NULL default '0',
  `expert_job_giveup_cnt` tinyint(3) unsigned NOT NULL default '0',
  `expert_job_info` int(11) NOT NULL default '0',
  `expert_job_info_ex` int(11) NOT NULL default '0',
  `recipe` blob NOT NULL,
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_friends`
--

DROP TABLE IF EXISTS `charac_friends`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_friends` (
  `charac_no` int(11) NOT NULL default '0',
  `friend_no` int(11) NOT NULL default '0',
  PRIMARY KEY  (`charac_no`,`friend_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_housing_info`
--

DROP TABLE IF EXISTS `charac_housing_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_housing_info` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `installed` smallint(5) unsigned NOT NULL default '0',
  `decoration_inven` binary(144) NOT NULL default '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  `version` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_housing_tree_info`
--

DROP TABLE IF EXISTS `charac_housing_tree_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_housing_tree_info` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `tree_id` int(10) unsigned NOT NULL default '0',
  `expire_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `current_point` smallint(6) NOT NULL default '0',
  `leaf_point` smallint(6) NOT NULL default '0',
  `day_water_count` smallint(6) NOT NULL default '0',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_housing_water_history`
--

DROP TABLE IF EXISTS `charac_housing_water_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_housing_water_history` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `give_time` timestamp NOT NULL default CURRENT_TIMESTAMP,
  `give_charac_name` varchar(20) NOT NULL default '',
  PRIMARY KEY  (`charac_no`,`give_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_info`
--

DROP TABLE IF EXISTS `charac_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_info` (
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL auto_increment,
  `charac_name` varchar(20) NOT NULL default '',
  `village` tinyint(4) NOT NULL default '1',
  `job` tinyint(4) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '1',
  `exp` int(11) NOT NULL default '0',
  `grow_type` tinyint(4) NOT NULL default '0',
  `HP` tinyint(3) unsigned NOT NULL default '0',
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
  `fatigue` smallint(6) NOT NULL default '0',
  `max_fatigue` smallint(6) NOT NULL default '70',
  `premium_fatigue` smallint(6) NOT NULL default '0',
  `max_premium_fatigue` smallint(6) NOT NULL default '0',
  `create_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `dungeon_clear_point` int(11) NOT NULL default '0',
  `delete_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `delete_flag` tinyint(4) NOT NULL default '0',
  `guild_id` int(10) unsigned NOT NULL default '0',
  `guild_right` tinyint(4) NOT NULL default '0',
  `member_flag` tinyint(4) NOT NULL default '0',
  `sex` tinyint(4) NOT NULL default '1',
  `expert_job` tinyint(3) unsigned NOT NULL default '0',
  `skill_tree_index` tinyint(4) NOT NULL default '-1',
  `link_charac_no` int(10) unsigned NOT NULL default '0',
  `event_charac_level` tinyint(4) NOT NULL default '0',
  `guild_secede` tinyint(2) NOT NULL default '0',
  `start_time` int(11) NOT NULL default '0',
  `finish_time` int(11) NOT NULL default '0',
  `competition_area` tinyint(2) NOT NULL default '-1',
  `competition_period` tinyint(2) NOT NULL default '-1',
  `mercenary_start_time` int(11) NOT NULL default '0',
  `mercenary_finish_time` int(11) NOT NULL default '0',
  `mercenary_area` tinyint(4) NOT NULL default '-1',
  `mercenary_period` tinyint(4) NOT NULL default '-1',
  `VIP` varchar(255) NOT NULL,
  PRIMARY KEY  (`charac_no`),
  UNIQUE KEY `charac_name` USING BTREE (`charac_name`),
  KEY `charac_info_idx1` USING BTREE (`m_id`),
  KEY `charac_info_idx2` USING BTREE (`exp`),
  KEY `idx_guild_id` USING BTREE (`guild_id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_item_stat`
--

DROP TABLE IF EXISTS `charac_item_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_item_stat` (
  `charac_no` int(11) NOT NULL default '0',
  `cooltime_item` blob NOT NULL,
  `effect_item` blob NOT NULL,
  `check_flag` blob NOT NULL,
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_kill_monster_info`
--

DROP TABLE IF EXISTS `charac_kill_monster_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_kill_monster_info` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `boss_info` blob NOT NULL,
  `named_info` blob NOT NULL,
  `apc_boss_info` blob NOT NULL,
  PRIMARY KEY  (`charac_no`),
  KEY `pk_charac_no` USING BTREE (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_link_bonus`
--

DROP TABLE IF EXISTS `charac_link_bonus`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_link_bonus` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `exp` int(10) unsigned NOT NULL default '0',
  `gold` int(10) unsigned NOT NULL default '0',
  `mercenary_start_time` int(11) NOT NULL default '0',
  `mercenary_finish_time` int(11) NOT NULL default '0',
  `mercenary_area` tinyint(4) NOT NULL default '-1',
  `mercenary_period` tinyint(4) NOT NULL default '-1',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_link_message`
--

DROP TABLE IF EXISTS `charac_link_message`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_link_message` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `message_flag` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_manage_info`
--

DROP TABLE IF EXISTS `charac_manage_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_manage_info` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `tag_charac_no` int(10) unsigned NOT NULL default '0',
  `striker_skill_index` tinyint(3) unsigned NOT NULL default '0',
  `max_equip_level` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_members`
--

DROP TABLE IF EXISTS `charac_members`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_members` (
  `charac_no` int(11) NOT NULL default '0',
  `master_no` int(11) NOT NULL default '0',
  `exp` int(11) NOT NULL default '0',
  `create_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `delete_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_npc`
--

DROP TABLE IF EXISTS `charac_npc`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_npc` (
  `charac_no` int(11) NOT NULL default '0',
  `npc_cnt` tinyint(3) unsigned NOT NULL default '0',
  `npc_data` blob NOT NULL,
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_option`
--

DROP TABLE IF EXISTS `charac_option`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_option` (
  `charac_no` int(11) NOT NULL default '0',
  `options` blob NOT NULL,
  `best_clear_time` blob NOT NULL,
  `blue_marble_enter_count` tinyint(3) unsigned NOT NULL default '0',
  `charac_inform_notice` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_quest`
--

DROP TABLE IF EXISTS `charac_quest`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_quest` (
  `charac_no` int(11) NOT NULL default '0',
  `quest_10` binary(64) NOT NULL default '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  `quest_15` binary(64) NOT NULL default '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  `quest_20` binary(64) NOT NULL default '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  `quest_30` binary(64) NOT NULL default '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  `quest_40` binary(64) NOT NULL default '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  `quest_40_ext` binary(64) NOT NULL default '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  `quest_50` binary(64) NOT NULL default '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  `quest_60` binary(64) NOT NULL default '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  `quest_70` binary(64) NOT NULL default '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  `quest_etc` binary(64) NOT NULL default '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  `play_1` smallint(5) unsigned NOT NULL default '0',
  `play_1_trigger` int(11) NOT NULL default '0',
  `play_2` smallint(5) unsigned NOT NULL default '0',
  `play_2_trigger` int(11) NOT NULL default '0',
  `play_3` smallint(5) unsigned NOT NULL default '0',
  `play_3_trigger` int(11) NOT NULL default '0',
  `play_4` smallint(5) unsigned NOT NULL default '0',
  `play_4_trigger` int(11) NOT NULL default '0',
  `play_5` smallint(5) unsigned NOT NULL default '0',
  `play_5_trigger` int(11) NOT NULL default '0',
  `play_6` smallint(5) unsigned NOT NULL default '0',
  `play_6_trigger` int(11) NOT NULL default '0',
  `play_7` smallint(5) unsigned NOT NULL default '0',
  `play_7_trigger` int(11) NOT NULL default '0',
  `play_8` smallint(5) unsigned NOT NULL default '0',
  `play_8_trigger` int(11) NOT NULL default '0',
  `play_9` smallint(5) unsigned NOT NULL default '0',
  `play_9_trigger` int(11) NOT NULL default '0',
  `play_10` smallint(5) unsigned NOT NULL default '0',
  `play_10_trigger` int(11) NOT NULL default '0',
  `quest_50_ext` binary(64) NOT NULL default '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  `quest_60_ext` binary(64) NOT NULL default '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  `quest_etc_ext` binary(64) NOT NULL default '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  `quest_60_ext_2nd` binary(64) NOT NULL default '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_quest_ref`
--

DROP TABLE IF EXISTS `charac_quest_ref`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_quest_ref` (
  `origin_idx` int(11) NOT NULL default '0',
  `mapped_idx` int(11) NOT NULL default '0',
  PRIMARY KEY  (`origin_idx`),
  UNIQUE KEY `mapped_idx` USING BTREE (`mapped_idx`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_quest_shop`
--

DROP TABLE IF EXISTS `charac_quest_shop`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_quest_shop` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `qp` int(10) unsigned NOT NULL default '0',
  `max_hp` smallint(5) unsigned NOT NULL default '0',
  `max_mp` smallint(5) unsigned NOT NULL default '0',
  `psy_attack` smallint(5) unsigned NOT NULL default '0',
  `psy_defense` smallint(5) unsigned NOT NULL default '0',
  `mag_attack` smallint(5) unsigned NOT NULL default '0',
  `mag_defence` smallint(5) unsigned NOT NULL default '0',
  `move_speed` smallint(5) unsigned NOT NULL default '0',
  `attack_speed` smallint(5) unsigned NOT NULL default '0',
  `hp_regen` smallint(5) unsigned NOT NULL default '0',
  `mp_regen` smallint(5) unsigned NOT NULL default '0',
  `all_element_resist` smallint(5) unsigned NOT NULL default '0',
  `fire_element_resist` smallint(5) unsigned NOT NULL default '0',
  `water_element_resist` smallint(5) unsigned NOT NULL default '0',
  `light_element_resist` smallint(5) unsigned NOT NULL default '0',
  `dark_element_resist` smallint(5) unsigned NOT NULL default '0',
  `all_element_attack` smallint(5) unsigned NOT NULL default '0',
  `fire_element_attack` smallint(5) unsigned NOT NULL default '0',
  `water_element_attack` smallint(5) unsigned NOT NULL default '0',
  `light_element_attack` smallint(5) unsigned NOT NULL default '0',
  `dark_element_attack` smallint(5) unsigned NOT NULL default '0',
  `psy_critical` smallint(5) unsigned NOT NULL default '0',
  `mag_critical` smallint(5) unsigned NOT NULL default '0',
  `good_hit` smallint(5) unsigned NOT NULL default '0',
  `evasion` smallint(5) unsigned NOT NULL default '0',
  `hit_recovery` smallint(5) unsigned NOT NULL default '0',
  `init_count` smallint(5) unsigned NOT NULL default '0',
  `separate_psy_mag_attack` smallint(5) unsigned NOT NULL default '0',
  `quest_piece` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_ridable_stat`
--

DROP TABLE IF EXISTS `charac_ridable_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_ridable_stat` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `cooltime` blob NOT NULL,
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_stat`
--

DROP TABLE IF EXISTS `charac_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_stat` (
  `charac_no` int(11) NOT NULL default '0',
  `village` tinyint(4) NOT NULL default '1',
  `exp` int(11) NOT NULL default '0',
  `HP` tinyint(4) unsigned NOT NULL default '0',
  `fatigue` smallint(11) NOT NULL default '0',
  `used_fatigue` smallint(11) NOT NULL default '0',
  `premium_fatigue` smallint(11) NOT NULL default '0',
  `dungeon_clear_point` int(11) NOT NULL default '0',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `forbidden_to_play` char(1) NOT NULL default '',
  `forbidden_due_to` datetime NOT NULL default '0000-00-00 00:00:00',
  `tutorial_flag` int(11) NOT NULL default '0',
  `trade_gold_total` int(10) unsigned NOT NULL default '0',
  `trade_gold_total_billion` smallint(5) unsigned NOT NULL default '0',
  `trade_gold_daily` int(10) unsigned NOT NULL default '0',
  `dungeon_map_pass_cnt` int(10) unsigned NOT NULL default '0',
  `dungeon_map_help_pass_cnt` int(10) unsigned NOT NULL default '0',
  `help_abuse_point` int(10) unsigned NOT NULL default '0',
  `chaos_point` int(10) unsigned NOT NULL default '0',
  `chaos_exp` int(10) unsigned NOT NULL default '0',
  `chaos_mode_count` int(10) unsigned NOT NULL default '0',
  `chaos_kill_count` int(10) unsigned NOT NULL default '0',
  `chaos_die_count` int(10) unsigned NOT NULL default '0',
  `chaos_die_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `chaos_kill_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `assault_count` int(10) unsigned NOT NULL default '0',
  `luck_point` int(11) NOT NULL default '5000',
  `dungeon_play_count` int(10) unsigned NOT NULL default '0',
  `help_abuse_ratio` int(10) NOT NULL default '0',
  `help_abuse_exp` int(10) NOT NULL default '0',
  `expert_job_exp` int(11) NOT NULL default '0',
  `fatigue_battery_charging` int(11) NOT NULL default '0',
  `escalade_tutorial_flag` varchar(32) NOT NULL default '',
  `power_war_point` smallint(5) unsigned NOT NULL default '0',
  `power_war_assault_count` int(10) unsigned NOT NULL default '0',
  `power_war_assault_victory_count` int(10) unsigned NOT NULL default '0',
  `fatigue_grownup_buff` int(10) unsigned NOT NULL default '0',
  `village_prev` tinyint(4) NOT NULL default '1',
  `last_play_time_powerwar` datetime NOT NULL default '0000-00-00 00:00:00',
  `emotion` smallint(5) unsigned NOT NULL default '0',
  `add_slot_flag` tinyint(4) unsigned NOT NULL default '0',
  `member_dungeon_flag` tinyint(4) unsigned NOT NULL default '0',
  `open_flag` tinyint(4) NOT NULL default '0',
  `member_bonus_fatigue` tinyint(3) unsigned NOT NULL default '0',
  `birthday_effect_time` datetime default '0000-00-00 00:00:00',
  `visible_flags` tinyint(4) unsigned NOT NULL default '2',
  `add_equipslot_flag` tinyint(4) NOT NULL default '0',
  `channel_equipslot_switch` tinyint(4) NOT NULL default '0',
  `expand_equipslot_switch` tinyint(4) NOT NULL default '0',
  `growth_power_reward` tinyint(4) NOT NULL default '0',
  `chaos_respon_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `last_play_dungeon_index` int(10) unsigned NOT NULL default '0',
  `total_play_time` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`charac_no`),
  KEY `idx_exp` USING BTREE (`exp`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_titlebook`
--

DROP TABLE IF EXISTS `charac_titlebook`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_titlebook` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `specific_section` blob NOT NULL,
  `general_section` blob NOT NULL,
  `despair` blob NOT NULL,
  `event` blob NOT NULL,
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_tower_despair`
--

DROP TABLE IF EXISTS `charac_tower_despair`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_tower_despair` (
  `charac_no` int(11) NOT NULL default '0',
  `first_layer_start_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `today_enter_count` tinyint(4) NOT NULL default '0',
  `last_clear_layer` tinyint(4) NOT NULL default '0',
  `enter_count_by_week` int(11) NOT NULL default '0',
  `m_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `last_clear_date` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`charac_no`),
  KEY `m_date` USING BTREE (`m_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_tower_despair_apc`
--

DROP TABLE IF EXISTS `charac_tower_despair_apc`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_tower_despair_apc` (
  `reg_date` date NOT NULL default '0000-00-00',
  `charac_no` int(11) NOT NULL default '0',
  `seq` int(11) NOT NULL default '0',
  PRIMARY KEY  (`reg_date`,`seq`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_tower_rank`
--

DROP TABLE IF EXISTS `charac_tower_rank`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_tower_rank` (
  `tower_index` tinyint(3) unsigned NOT NULL default '0',
  `part_type` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `member_info` char(128) character set sjis collate sjis_bin NOT NULL default '',
  `rank` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`tower_index`,`part_type`,`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_tower_rank_top5`
--

DROP TABLE IF EXISTS `charac_tower_rank_top5`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_tower_rank_top5` (
  `tower_index` tinyint(3) unsigned NOT NULL default '0',
  `part_type` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `member_info` char(128) character set sjis collate sjis_bin NOT NULL default '',
  `rank` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`tower_index`,`part_type`,`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_tower_record`
--

DROP TABLE IF EXISTS `charac_tower_record`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_tower_record` (
  `charac_no` int(11) NOT NULL default '0',
  `tower_index` tinyint(3) unsigned NOT NULL default '0',
  `member_info_1` char(32) character set sjis collate sjis_bin NOT NULL default '',
  `stage_1` tinyint(3) unsigned NOT NULL default '0',
  `play_time_1` int(10) unsigned NOT NULL default '0',
  `occ_time_1` datetime NOT NULL default '0000-00-00 00:00:00',
  `member_info_2` char(64) character set sjis collate sjis_bin NOT NULL default '',
  `stage_2` tinyint(3) unsigned NOT NULL default '0',
  `play_time_2` int(10) unsigned NOT NULL default '0',
  `occ_time_2` datetime NOT NULL default '0000-00-00 00:00:00',
  `member_info_3` char(96) character set sjis collate sjis_bin NOT NULL default '',
  `stage_3` tinyint(3) unsigned NOT NULL default '0',
  `play_time_3` int(10) unsigned NOT NULL default '0',
  `occ_time_3` datetime NOT NULL default '0000-00-00 00:00:00',
  `member_info_4` char(128) character set sjis collate sjis_bin NOT NULL default '',
  `stage_4` tinyint(3) unsigned NOT NULL default '0',
  `play_time_4` int(10) unsigned NOT NULL default '0',
  `occ_time_4` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_tower_scream`
--

DROP TABLE IF EXISTS `charac_tower_scream`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_tower_scream` (
  `charac_no` int(11) NOT NULL default '0',
  `first_layer_start_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `today_enter_count` tinyint(4) NOT NULL default '0',
  `last_clear_layer` tinyint(4) NOT NULL default '0',
  `enter_count_by_week` int(11) NOT NULL default '0',
  `m_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `last_clear_date` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`charac_no`),
  KEY `m_date` USING BTREE (`m_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_tower_scream_apc`
--

DROP TABLE IF EXISTS `charac_tower_scream_apc`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_tower_scream_apc` (
  `reg_date` date NOT NULL default '0000-00-00',
  `charac_no` int(11) NOT NULL default '0',
  `seq` int(11) NOT NULL default '0',
  PRIMARY KEY  (`reg_date`,`seq`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_trade_limit_info`
--

DROP TABLE IF EXISTS `charac_trade_limit_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_trade_limit_info` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `charac_no` int(10) unsigned NOT NULL default '0',
  `last_trade_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `total_trade_gold` int(10) unsigned NOT NULL default '0',
  `trade_count` smallint(5) unsigned NOT NULL default '0',
  `nexon_user` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`charac_no`),
  KEY `idx_mid` USING BTREE (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_view`
--

DROP TABLE IF EXISTS `charac_view`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_view` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `info` blob NOT NULL,
  `slot_effect_count` tinyint(3) unsigned NOT NULL default '18',
  `charac_slot_limit` tinyint(3) unsigned NOT NULL default '18',
  `hash_key` varchar(32) NOT NULL default '',
  `charac_count` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charac_view_act8`
--

DROP TABLE IF EXISTS `charac_view_act8`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `charac_view_act8` (
  `m_id` bigint(20) unsigned NOT NULL default '0',
  `info` blob NOT NULL,
  `slot_effect_count` tinyint(3) unsigned NOT NULL default '18',
  `charac_slot_limit` tinyint(3) unsigned NOT NULL default '18',
  `hash_key` varchar(32) NOT NULL default '',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cs_table2`
--

DROP TABLE IF EXISTS `cs_table2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cs_table2` (
  `account_id` char(30) NOT NULL,
  `charac_id` char(30) NOT NULL
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
  `district` varchar(20) NOT NULL default '',
  `firm_name` varchar(50) NOT NULL default '',
  `telephone` varchar(20) NOT NULL default '',
  `address` varchar(150) NOT NULL default '',
  `leader` varchar(30) NOT NULL default '',
  `ip` varchar(11) NOT NULL default '',
  `start_ip` tinyint(3) unsigned NOT NULL default '0',
  `end_ip` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ip_no`),
  UNIQUE KEY `start_ip` USING BTREE (`ip`,`start_ip`),
  UNIQUE KEY `end_ip` USING BTREE (`ip`,`end_ip`),
  KEY `idx_district` USING BTREE (`district`),
  KEY `idx_leader` USING BTREE (`leader`),
  KEY `idx_firm_name` USING BTREE (`firm_name`),
  KEY `idx_ip` USING BTREE (`ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `eco_point`
--

DROP TABLE IF EXISTS `eco_point`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `eco_point` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `eco_point` int(10) unsigned NOT NULL default '0',
  `point_500` tinyint(4) NOT NULL default '0',
  `point_300` tinyint(4) NOT NULL default '0',
  `point_100` tinyint(4) NOT NULL default '0',
  `point_50` tinyint(4) NOT NULL default '0',
  `point_20` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_1106_idol_bring_up`
--

DROP TABLE IF EXISTS `event_1106_idol_bring_up`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_1106_idol_bring_up` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `m_id` int(10) unsigned NOT NULL default '0',
  `pot_type` tinyint(4) NOT NULL default '0',
  `water_cnt` tinyint(4) NOT NULL default '0',
  `give_title_flag` tinyint(4) NOT NULL default '0',
  `occ_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `give_title_flag2` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_additional_condition_info`
--

DROP TABLE IF EXISTS `event_additional_condition_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_additional_condition_info` (
  `charac_no` int(11) unsigned NOT NULL default '0',
  `current_step` tinyint(4) unsigned NOT NULL default '0',
  `reward_step` tinyint(4) unsigned NOT NULL default '0',
  `update_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_condition_info`
--

DROP TABLE IF EXISTS `event_condition_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_condition_info` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `current_step` tinyint(3) unsigned NOT NULL default '0',
  `reward_step` tinyint(3) unsigned NOT NULL default '0',
  `update_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_conditionable_info`
--

DROP TABLE IF EXISTS `event_conditionable_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_conditionable_info` (
  `charac_no` int(11) unsigned NOT NULL default '0',
  `current_step` tinyint(4) unsigned NOT NULL default '0',
  `reward_step` tinyint(4) unsigned NOT NULL default '0',
  `update_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_detective_goblin`
--

DROP TABLE IF EXISTS `event_detective_goblin`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_detective_goblin` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `point` int(11) NOT NULL default '0',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_dungeon_clear`
--

DROP TABLE IF EXISTS `event_dungeon_clear`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_dungeon_clear` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `clear_cnt` int(10) unsigned NOT NULL default '0',
  `update_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_levelup_support`
--

DROP TABLE IF EXISTS `event_levelup_support`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_levelup_support` (
  `charac_no` int(11) NOT NULL,
  `level` int(11) NOT NULL,
  `state` int(11) default NULL,
  PRIMARY KEY  (`charac_no`,`level`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_used_fatigue_at_mage`
--

DROP TABLE IF EXISTS `event_used_fatigue_at_mage`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_used_fatigue_at_mage` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `fatigue_quantity` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_visit_room_info`
--

DROP TABLE IF EXISTS `event_visit_room_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_visit_room_info` (
  `charac_no` int(11) unsigned NOT NULL default '0',
  `visit_cnt` tinyint(4) unsigned NOT NULL default '0',
  `visit_charac_no` blob,
  `update_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `exp_level_ref`
--

DROP TABLE IF EXISTS `exp_level_ref`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `exp_level_ref` (
  `exp` int(10) unsigned NOT NULL default '0',
  `lev` int(11) unsigned NOT NULL default '0'
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
  `gc_ip` varchar(64) NOT NULL default '',
  `gc_port` smallint(5) unsigned NOT NULL default '0',
  `gc_max` smallint(5) unsigned NOT NULL default '0',
  `gc_game` tinyint(3) unsigned NOT NULL default '0',
  `gc_channel` varchar(32) NOT NULL default '',
  `gc_ch_group` smallint(5) NOT NULL default '0',
  `gc_channeltype` varchar(25) NOT NULL default '',
  `gc_up_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `gc_swordman_cnt` smallint(5) unsigned NOT NULL default '0',
  `gc_fighter_cnt` smallint(5) unsigned NOT NULL default '0',
  `gc_gunner_cnt` smallint(5) unsigned NOT NULL default '0',
  `gc_mage_cnt` smallint(5) unsigned NOT NULL default '0',
  `gc_priest_cnt` smallint(5) unsigned NOT NULL default '0',
  `gc_at_gunner_cnt` smallint(5) unsigned NOT NULL default '0',
  `gc_thief_cnt` smallint(5) unsigned NOT NULL default '0',
  `gc_hangame` smallint(5) unsigned NOT NULL default '0',
  `gc_nexon` smallint(5) unsigned NOT NULL default '0',
  `gc_type` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`gc_no`),
  KEY `idxGC_GAME` USING BTREE (`gc_game`),
  KEY `idxch_group` USING BTREE (`gc_ch_group`)
) ENGINE=InnoDB AUTO_INCREMENT=3053 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ip_info`
--

DROP TABLE IF EXISTS `ip_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ip_info` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `m_id` int(10) unsigned NOT NULL default '0',
  `ip` varchar(11) NOT NULL default '',
  `start_ip` tinyint(3) unsigned NOT NULL default '0',
  `end_ip` tinyint(3) unsigned NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `ip_check` tinyint(3) unsigned NOT NULL default '0',
  `vendor_no` int(10) unsigned NOT NULL default '0',
  `speed_no` int(10) unsigned NOT NULL default '0',
  `start_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `end_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `charge_flag` tinyint(4) NOT NULL default '0',
  `settle_no` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`no`),
  UNIQUE KEY `ip` USING BTREE (`ip`,`start_ip`,`end_ip`),
  KEY `idx_m_id` USING BTREE (`m_id`),
  KEY `idx_ip` USING BTREE (`ip`),
  KEY `idx_start_ip` USING BTREE (`start_ip`),
  KEY `idx_end_ip` USING BTREE (`end_ip`),
  KEY `idx_occ_time` USING BTREE (`occ_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `item_gen_ref`
--

DROP TABLE IF EXISTS `item_gen_ref`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `item_gen_ref` (
  `item_grade` tinyint(4) NOT NULL default '0',
  `rate_type` tinyint(4) NOT NULL default '0',
  `money_rate` smallint(6) NOT NULL default '0',
  `item_rate` smallint(6) NOT NULL default '0',
  `free_rate` smallint(6) NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `item_making_skill_info`
--

DROP TABLE IF EXISTS `item_making_skill_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `item_making_skill_info` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `weapon` smallint(5) unsigned NOT NULL default '0',
  `cloth` smallint(5) unsigned NOT NULL default '0',
  `leather` smallint(5) unsigned NOT NULL default '0',
  `light_armor` smallint(5) unsigned NOT NULL default '0',
  `heavy_armor` smallint(5) unsigned NOT NULL default '0',
  `plate` smallint(5) unsigned NOT NULL default '0',
  `amulet` smallint(5) unsigned NOT NULL default '0',
  `wrist` smallint(5) unsigned NOT NULL default '0',
  `ring` smallint(5) unsigned NOT NULL default '0',
  `support` smallint(5) unsigned NOT NULL default '0',
  `magic_stone` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `item_select_std`
--

DROP TABLE IF EXISTS `item_select_std`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `item_select_std` (
  `item_grade` int(11) NOT NULL default '0',
  `top` int(11) NOT NULL default '0',
  `bottom` int(11) NOT NULL default '0',
  `weight` int(11) NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `limit_npc_item`
--

DROP TABLE IF EXISTS `limit_npc_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `limit_npc_item` (
  `item_index` int(10) unsigned NOT NULL default '0',
  `max_count` int(10) unsigned NOT NULL default '0',
  `sell_count` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`item_index`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_num_occupations`
--

DROP TABLE IF EXISTS `log_num_occupations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_num_occupations` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `num_occupations_charscreen` mediumint(8) unsigned NOT NULL default '0',
  `num_occupations_seriaroom` mediumint(8) unsigned NOT NULL default '0',
  `num_login_per_min` mediumint(8) unsigned NOT NULL default '0',
  `num_logout_per_min` mediumint(8) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_booster_gage`
--

DROP TABLE IF EXISTS `member_booster_gage`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_booster_gage` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `gage` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_dungeon`
--

DROP TABLE IF EXISTS `member_dungeon`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_dungeon` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `dungeon` text NOT NULL,
  PRIMARY KEY  (`m_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `money_gen_ref`
--

DROP TABLE IF EXISTS `money_gen_ref`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `money_gen_ref` (
  `grade` int(11) NOT NULL default '0',
  `bottom_grade` int(11) NOT NULL default '0',
  `money` int(11) NOT NULL default '0',
  `random_value` int(11) NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `monster_reward_ref`
--

DROP TABLE IF EXISTS `monster_reward_ref`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `monster_reward_ref` (
  `level` smallint(11) NOT NULL default '0',
  `exp` int(11) NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `new_charac_quest`
--

DROP TABLE IF EXISTS `new_charac_quest`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `new_charac_quest` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `clear_quest` blob NOT NULL,
  `quest_notify` blob NOT NULL,
  `play_1` smallint(5) unsigned NOT NULL default '0',
  `play_1_trigger` int(10) unsigned NOT NULL default '0',
  `play_2` smallint(5) unsigned NOT NULL default '0',
  `play_2_trigger` int(10) unsigned NOT NULL default '0',
  `play_3` smallint(5) unsigned NOT NULL default '0',
  `play_3_trigger` int(10) unsigned NOT NULL default '0',
  `play_4` smallint(5) unsigned NOT NULL default '0',
  `play_4_trigger` int(10) unsigned NOT NULL default '0',
  `play_5` smallint(5) unsigned NOT NULL default '0',
  `play_5_trigger` int(10) unsigned NOT NULL default '0',
  `play_6` smallint(5) unsigned NOT NULL default '0',
  `play_6_trigger` int(10) unsigned NOT NULL default '0',
  `play_7` smallint(5) unsigned NOT NULL default '0',
  `play_7_trigger` int(10) unsigned NOT NULL default '0',
  `play_8` smallint(5) unsigned NOT NULL default '0',
  `play_8_trigger` int(10) unsigned NOT NULL default '0',
  `play_9` smallint(5) unsigned NOT NULL default '0',
  `play_9_trigger` int(10) unsigned NOT NULL default '0',
  `play_10` smallint(5) unsigned NOT NULL default '0',
  `play_10_trigger` int(10) unsigned NOT NULL default '0',
  `auto_clear` tinyint(3) unsigned NOT NULL default '0',
  `play_11` smallint(5) unsigned NOT NULL default '0',
  `play_11_trigger` int(10) unsigned NOT NULL default '0',
  `play_12` smallint(5) unsigned NOT NULL default '0',
  `play_12_trigger` int(10) unsigned NOT NULL default '0',
  `play_13` smallint(5) unsigned NOT NULL default '0',
  `play_13_trigger` int(10) unsigned NOT NULL default '0',
  `play_14` smallint(5) unsigned NOT NULL default '0',
  `play_14_trigger` int(10) unsigned NOT NULL default '0',
  `play_15` smallint(5) unsigned NOT NULL default '0',
  `play_15_trigger` int(10) unsigned NOT NULL default '0',
  `play_16` smallint(5) unsigned NOT NULL default '0',
  `play_16_trigger` int(10) unsigned NOT NULL default '0',
  `play_17` smallint(5) unsigned NOT NULL default '0',
  `play_17_trigger` int(10) unsigned NOT NULL default '0',
  `play_18` smallint(5) unsigned NOT NULL default '0',
  `play_18_trigger` int(10) unsigned NOT NULL default '0',
  `play_19` smallint(5) unsigned NOT NULL default '0',
  `play_19_trigger` int(10) unsigned NOT NULL default '0',
  `play_20` smallint(5) unsigned NOT NULL default '0',
  `play_20_trigger` int(10) unsigned NOT NULL default '0',
  `urgentQuestIndex` smallint(6) NOT NULL default '-1',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `party_rank_avg`
--

DROP TABLE IF EXISTS `party_rank_avg`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `party_rank_avg` (
  `dungeon_index` smallint(6) NOT NULL default '0',
  `party_level` smallint(6) NOT NULL default '0',
  `clear_count` bigint(20) NOT NULL default '0',
  `average` int(11) NOT NULL default '0',
  PRIMARY KEY  (`dungeon_index`,`party_level`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pvp_grade_expand`
--

DROP TABLE IF EXISTS `pvp_grade_expand`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pvp_grade_expand` (
  `charac_no` int(11) NOT NULL default '0',
  `pvp_grade` int(11) NOT NULL default '0',
  `pvp_point` int(11) NOT NULL default '0',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`charac_no`),
  KEY `idx_pvp_grade` USING BTREE (`pvp_grade`),
  KEY `idx_pvp_point` USING BTREE (`pvp_point`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pvp_grade_ref`
--

DROP TABLE IF EXISTS `pvp_grade_ref`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pvp_grade_ref` (
  `grade` int(11) NOT NULL default '0',
  `limit_pts` int(11) NOT NULL default '0',
  PRIMARY KEY  (`grade`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pvp_result`
--

DROP TABLE IF EXISTS `pvp_result`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pvp_result` (
  `charac_no` int(11) NOT NULL default '0',
  `win` int(11) NOT NULL default '0',
  `lose` int(11) NOT NULL default '0',
  `pvp_point` int(11) NOT NULL default '0',
  `pvp_grade` int(11) NOT NULL default '0',
  `pvp_grade_ext` tinyint(3) unsigned NOT NULL default '0',
  `avg_kill_count` int(11) NOT NULL default '0',
  `avg_buf_count` int(11) NOT NULL default '0',
  `avg_debuf_count` int(11) NOT NULL default '0',
  `avg_heal_count` int(11) NOT NULL default '0',
  `avg_counter_count` int(11) NOT NULL default '0',
  `avg_back_atk_count` int(11) NOT NULL default '0',
  `avg_union_hit_count` int(11) NOT NULL default '0',
  `avg_overkill_count` int(11) NOT NULL default '0',
  `avg_aerial_count` int(11) NOT NULL default '0',
  `avg_combo_count` int(11) NOT NULL default '0',
  `avg_attacked_count` int(11) NOT NULL default '0',
  `avg_deal_damage` int(11) NOT NULL default '0',
  `avg_technic` int(11) NOT NULL default '0',
  `avg_style` int(11) NOT NULL default '0',
  `avg_hit_penalty` int(11) NOT NULL default '0',
  `pvp_count` int(11) NOT NULL default '0',
  `win_point` int(11) NOT NULL default '0',
  `last_play_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `play_count` int(10) unsigned NOT NULL default '0',
  `play_time` int(10) unsigned NOT NULL default '0',
  `pvp_grade_ext_update_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`charac_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `quest_category`
--

DROP TABLE IF EXISTS `quest_category`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `quest_category` (
  `quest_idx` int(11) NOT NULL default '0',
  `quest_name` varchar(30) NOT NULL default '',
  PRIMARY KEY  (`quest_idx`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `server_state_info`
--

DROP TABLE IF EXISTS `server_state_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `server_state_info` (
  `category` int(11) NOT NULL default '-1',
  `code` int(11) NOT NULL default '-1',
  `state` binary(12) NOT NULL default '\0\0\0\0\0\0\0\0\0\0\0\0',
  `start_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `end_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`category`,`code`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `single_rank_avg`
--

DROP TABLE IF EXISTS `single_rank_avg`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `single_rank_avg` (
  `dungeon_index` smallint(6) NOT NULL default '0',
  `level` smallint(6) NOT NULL default '0',
  `job` smallint(6) NOT NULL default '0',
  `clear_count` bigint(20) NOT NULL default '0',
  `average` int(11) NOT NULL default '0',
  PRIMARY KEY  (`dungeon_index`,`level`,`job`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sp_reward`
--

DROP TABLE IF EXISTS `sp_reward`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sp_reward` (
  `grade` int(11) NOT NULL default '0',
  `sp` int(11) NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `stat_game_channel`
--

DROP TABLE IF EXISTS `stat_game_channel`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `stat_game_channel` (
  `gc_channel` varchar(10) NOT NULL default '',
  `gc_up_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `gc_now` smallint(6) NOT NULL default '0',
  KEY `gc_channel` USING BTREE (`gc_channel`,`gc_up_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `test_sooya`
--

DROP TABLE IF EXISTS `test_sooya`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `test_sooya` (
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '1'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Current Database: `taiwan_billing`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `taiwan_billing` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `taiwan_billing`;

--
-- Table structure for table `bak_cash_cera`
--

DROP TABLE IF EXISTS `bak_cash_cera`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_cera` (
  `account` varchar(30) NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `mod_tran` bigint(20) unsigned NOT NULL,
  `mod_date` datetime NOT NULL,
  `reg_date` datetime NOT NULL,
  `cera_point` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='cash table - cera';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_cash_cera_20130103`
--

DROP TABLE IF EXISTS `bak_cash_cera_20130103`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_cera_20130103` (
  `account` varchar(30) NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `mod_tran` bigint(20) unsigned NOT NULL,
  `mod_date` datetime NOT NULL,
  `reg_date` datetime NOT NULL,
  PRIMARY KEY  (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='cash table - cera';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_cash_cera_20130219`
--

DROP TABLE IF EXISTS `bak_cash_cera_20130219`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_cera_20130219` (
  `account` varchar(30) NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `mod_tran` bigint(20) unsigned NOT NULL,
  `mod_date` datetime NOT NULL,
  `reg_date` datetime NOT NULL,
  PRIMARY KEY  (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_cash_cera_20130221`
--

DROP TABLE IF EXISTS `bak_cash_cera_20130221`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_cera_20130221` (
  `account` varchar(30) NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `mod_tran` bigint(20) unsigned NOT NULL,
  `mod_date` datetime NOT NULL,
  `reg_date` datetime NOT NULL,
  PRIMARY KEY  (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_cash_cera_20130221_2`
--

DROP TABLE IF EXISTS `bak_cash_cera_20130221_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_cera_20130221_2` (
  `account` varchar(30) NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `mod_tran` bigint(20) unsigned NOT NULL,
  `mod_date` datetime NOT NULL,
  `reg_date` datetime NOT NULL,
  PRIMARY KEY  (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_cash_cera_20130221_3`
--

DROP TABLE IF EXISTS `bak_cash_cera_20130221_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_cera_20130221_3` (
  `account` varchar(30) NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `mod_tran` bigint(20) unsigned NOT NULL,
  `mod_date` datetime NOT NULL,
  `reg_date` datetime NOT NULL,
  PRIMARY KEY  (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_cash_cera_20130221_4`
--

DROP TABLE IF EXISTS `bak_cash_cera_20130221_4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_cera_20130221_4` (
  `account` varchar(30) NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `mod_tran` bigint(20) unsigned NOT NULL,
  `mod_date` datetime NOT NULL,
  `reg_date` datetime NOT NULL,
  PRIMARY KEY  (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_cash_cera_point_20130219`
--

DROP TABLE IF EXISTS `bak_cash_cera_point_20130219`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_cera_point_20130219` (
  `account` varchar(30) NOT NULL default '',
  `cera_point` int(10) unsigned NOT NULL,
  `reg_date` datetime NOT NULL,
  `mod_date` datetime NOT NULL,
  PRIMARY KEY  (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_cash_cera_point_20130221`
--

DROP TABLE IF EXISTS `bak_cash_cera_point_20130221`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_cera_point_20130221` (
  `account` varchar(30) NOT NULL default '',
  `cera_point` int(10) unsigned NOT NULL,
  `reg_date` datetime NOT NULL,
  `mod_date` datetime NOT NULL,
  PRIMARY KEY  (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_cash_cera_point_20130221_2`
--

DROP TABLE IF EXISTS `bak_cash_cera_point_20130221_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_cera_point_20130221_2` (
  `account` varchar(30) NOT NULL default '',
  `cera_point` int(10) unsigned NOT NULL,
  `reg_date` datetime NOT NULL,
  `mod_date` datetime NOT NULL,
  PRIMARY KEY  (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_cash_cera_point_20130221_3`
--

DROP TABLE IF EXISTS `bak_cash_cera_point_20130221_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_cera_point_20130221_3` (
  `account` varchar(30) NOT NULL default '',
  `cera_point` int(10) unsigned NOT NULL,
  `reg_date` datetime NOT NULL,
  `mod_date` datetime NOT NULL,
  PRIMARY KEY  (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_cash_cera_point_20130221_4`
--

DROP TABLE IF EXISTS `bak_cash_cera_point_20130221_4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_cera_point_20130221_4` (
  `account` varchar(30) NOT NULL default '',
  `cera_point` int(10) unsigned NOT NULL,
  `reg_date` datetime NOT NULL,
  `mod_date` datetime NOT NULL,
  PRIMARY KEY  (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_cash_sequence_20130103`
--

DROP TABLE IF EXISTS `bak_cash_sequence_20130103`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_sequence_20130103` (
  `sequence_id` bigint(20) NOT NULL auto_increment,
  `dummy` char(1) NOT NULL,
  PRIMARY KEY  (`sequence_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_cash_sequence_20130219`
--

DROP TABLE IF EXISTS `bak_cash_sequence_20130219`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_sequence_20130219` (
  `sequence_id` bigint(20) NOT NULL auto_increment,
  `dummy` char(1) NOT NULL,
  PRIMARY KEY  (`sequence_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_cash_transaction_20130221`
--

DROP TABLE IF EXISTS `bak_cash_transaction_20130221`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_transaction_20130221` (
  `tran_id` bigint(20) NOT NULL auto_increment,
  `dummy` char(1) NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_cash_transaction_20130221_2`
--

DROP TABLE IF EXISTS `bak_cash_transaction_20130221_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_transaction_20130221_2` (
  `tran_id` bigint(20) NOT NULL auto_increment,
  `dummy` char(1) NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_cash_transaction_20130221_3`
--

DROP TABLE IF EXISTS `bak_cash_transaction_20130221_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_transaction_20130221_3` (
  `tran_id` bigint(20) NOT NULL auto_increment,
  `dummy` char(1) NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_cash_transaction_20130221_4`
--

DROP TABLE IF EXISTS `bak_cash_transaction_20130221_4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_cash_transaction_20130221_4` (
  `tran_id` bigint(20) NOT NULL auto_increment,
  `dummy` char(1) NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_cera_point_history_20130219`
--

DROP TABLE IF EXISTS `bak_log_cera_point_history_20130219`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_cera_point_history_20130219` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `account_id` varchar(30) NOT NULL default '',
  `charac_id` varchar(30) NOT NULL default '',
  `cera_point` int(10) unsigned NOT NULL default '0',
  `command` enum('A','U') NOT NULL COMMENT 'A(add), U(use)',
  `charge_type` tinyint(4) NOT NULL default '0',
  `free_charge_type` tinyint(4) NOT NULL default '0',
  `reg_date` datetime NOT NULL,
  PRIMARY KEY  (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_error_history_20130103`
--

DROP TABLE IF EXISTS `bak_log_error_history_20130103`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_error_history_20130103` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `error_id` int(10) NOT NULL,
  `error_msg` varchar(255) NOT NULL,
  `error_query` varchar(512) NOT NULL,
  `proc_name` varchar(45) NOT NULL,
  `proc_line` int(10) NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_error_history_20130219`
--

DROP TABLE IF EXISTS `bak_log_error_history_20130219`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_error_history_20130219` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `error_id` int(10) NOT NULL,
  `error_msg` varchar(255) NOT NULL,
  `error_query` varchar(512) NOT NULL,
  `proc_name` varchar(45) NOT NULL,
  `proc_line` int(10) NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_error_history_20130221`
--

DROP TABLE IF EXISTS `bak_log_error_history_20130221`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_error_history_20130221` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `error_id` int(10) NOT NULL,
  `error_msg` varchar(255) NOT NULL,
  `error_query` varchar(512) NOT NULL,
  `proc_name` varchar(45) NOT NULL,
  `proc_line` int(10) NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_error_history_20130221_2`
--

DROP TABLE IF EXISTS `bak_log_error_history_20130221_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_error_history_20130221_2` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `error_id` int(10) NOT NULL,
  `error_msg` varchar(255) NOT NULL,
  `error_query` varchar(512) NOT NULL,
  `proc_name` varchar(45) NOT NULL,
  `proc_line` int(10) NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_error_history_20130221_3`
--

DROP TABLE IF EXISTS `bak_log_error_history_20130221_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_error_history_20130221_3` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `error_id` int(10) NOT NULL,
  `error_msg` varchar(255) NOT NULL,
  `error_query` varchar(512) NOT NULL,
  `proc_name` varchar(45) NOT NULL,
  `proc_line` int(10) NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_error_history_20130221_4`
--

DROP TABLE IF EXISTS `bak_log_error_history_20130221_4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_error_history_20130221_4` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `error_id` int(10) NOT NULL,
  `error_msg` varchar(255) NOT NULL,
  `error_query` varchar(512) NOT NULL,
  `proc_name` varchar(45) NOT NULL,
  `proc_line` int(10) NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_gift_history_20130103`
--

DROP TABLE IF EXISTS `bak_log_gift_history_20130103`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_gift_history_20130103` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `send_account_id` varchar(30) NOT NULL,
  `send_charac_id` varchar(30) NOT NULL,
  `recv_account_id` varchar(30) NOT NULL,
  `item_id` int(10) unsigned NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `send_befor_cera` int(10) unsigned NOT NULL,
  `send_after_cera` int(10) unsigned NOT NULL,
  `recv_befor_cera` int(10) unsigned NOT NULL,
  `recv_after_cera` int(10) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='gift history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_gift_history_20130219`
--

DROP TABLE IF EXISTS `bak_log_gift_history_20130219`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_gift_history_20130219` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `send_account_id` varchar(30) NOT NULL,
  `send_charac_id` varchar(30) NOT NULL,
  `recv_account_id` varchar(30) NOT NULL,
  `item_id` int(10) unsigned NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `send_befor_cera` int(10) unsigned NOT NULL,
  `send_after_cera` int(10) unsigned NOT NULL,
  `recv_befor_cera` int(10) unsigned NOT NULL,
  `recv_after_cera` int(10) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='gift history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_gift_history_20130221`
--

DROP TABLE IF EXISTS `bak_log_gift_history_20130221`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_gift_history_20130221` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `send_account_id` varchar(30) NOT NULL,
  `send_charac_id` varchar(30) NOT NULL,
  `recv_account_id` varchar(30) NOT NULL,
  `item_id` int(10) unsigned NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `send_befor_cera` int(10) unsigned NOT NULL,
  `send_after_cera` int(10) unsigned NOT NULL,
  `recv_befor_cera` int(10) unsigned NOT NULL,
  `recv_after_cera` int(10) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='gift history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_gift_history_20130221_2`
--

DROP TABLE IF EXISTS `bak_log_gift_history_20130221_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_gift_history_20130221_2` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `send_account_id` varchar(30) NOT NULL,
  `send_charac_id` varchar(30) NOT NULL,
  `recv_account_id` varchar(30) NOT NULL,
  `item_id` int(10) unsigned NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `send_befor_cera` int(10) unsigned NOT NULL,
  `send_after_cera` int(10) unsigned NOT NULL,
  `recv_befor_cera` int(10) unsigned NOT NULL,
  `recv_after_cera` int(10) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='gift history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_gift_history_20130221_3`
--

DROP TABLE IF EXISTS `bak_log_gift_history_20130221_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_gift_history_20130221_3` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `send_account_id` varchar(30) NOT NULL,
  `send_charac_id` varchar(30) NOT NULL,
  `recv_account_id` varchar(30) NOT NULL,
  `item_id` int(10) unsigned NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `send_befor_cera` int(10) unsigned NOT NULL,
  `send_after_cera` int(10) unsigned NOT NULL,
  `recv_befor_cera` int(10) unsigned NOT NULL,
  `recv_after_cera` int(10) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='gift history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_gift_history_20130221_4`
--

DROP TABLE IF EXISTS `bak_log_gift_history_20130221_4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_gift_history_20130221_4` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `send_account_id` varchar(30) NOT NULL,
  `send_charac_id` varchar(30) NOT NULL,
  `recv_account_id` varchar(30) NOT NULL,
  `item_id` int(10) unsigned NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `send_befor_cera` int(10) unsigned NOT NULL,
  `send_after_cera` int(10) unsigned NOT NULL,
  `recv_befor_cera` int(10) unsigned NOT NULL,
  `recv_after_cera` int(10) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='gift history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_issue_tran_history_20130103`
--

DROP TABLE IF EXISTS `bak_log_issue_tran_history_20130103`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_issue_tran_history_20130103` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_type` tinyint(3) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='issue transaction history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_issue_tran_history_20130219`
--

DROP TABLE IF EXISTS `bak_log_issue_tran_history_20130219`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_issue_tran_history_20130219` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_type` tinyint(3) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='issue transaction history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_item_refund_history_20130221`
--

DROP TABLE IF EXISTS `bak_log_item_refund_history_20130221`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_item_refund_history_20130221` (
  `pf_rel_id` int(10) unsigned NOT NULL auto_increment,
  `purchase_tran_id` bigint(20) unsigned NOT NULL,
  `recharge_tran_id` bigint(20) unsigned NOT NULL,
  `account_id` char(30) NOT NULL,
  `occ_date` datetime NOT NULL,
  `reason` varchar(255) NOT NULL default '',
  `admin_id` varchar(30) NOT NULL default '',
  PRIMARY KEY  (`pf_rel_id`),
  KEY `log_item_refund_history_idx001` (`account_id`),
  KEY `log_item_refund_history_ibfk_1` (`purchase_tran_id`),
  KEY `log_item_refund_history_ibfk_2` (`recharge_tran_id`),
  CONSTRAINT `BAK_log_item_refund_history_20130221_ibfk_1` FOREIGN KEY (`purchase_tran_id`) REFERENCES `bak_log_purchase_history_20130221` (`tran_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `BAK_log_item_refund_history_20130221_ibfk_2` FOREIGN KEY (`recharge_tran_id`) REFERENCES `bak_log_recharge_history_20130221` (`tran_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_item_refund_history_20130221_2`
--

DROP TABLE IF EXISTS `bak_log_item_refund_history_20130221_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_item_refund_history_20130221_2` (
  `pf_rel_id` int(10) unsigned NOT NULL auto_increment,
  `purchase_tran_id` bigint(20) unsigned NOT NULL,
  `recharge_tran_id` bigint(20) unsigned NOT NULL,
  `account_id` char(30) NOT NULL,
  `occ_date` datetime NOT NULL,
  `reason` varchar(255) NOT NULL default '',
  `admin_id` varchar(30) NOT NULL default '',
  PRIMARY KEY  (`pf_rel_id`),
  KEY `log_item_refund_history_idx001` (`account_id`),
  KEY `log_item_refund_history_ibfk_1` (`purchase_tran_id`),
  KEY `log_item_refund_history_ibfk_2` (`recharge_tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_item_refund_history_20130221_3`
--

DROP TABLE IF EXISTS `bak_log_item_refund_history_20130221_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_item_refund_history_20130221_3` (
  `pf_rel_id` int(10) unsigned NOT NULL auto_increment,
  `purchase_tran_id` bigint(20) unsigned NOT NULL,
  `recharge_tran_id` bigint(20) unsigned NOT NULL,
  `account_id` char(30) NOT NULL,
  `occ_date` datetime NOT NULL,
  `reason` varchar(255) NOT NULL default '',
  `admin_id` varchar(30) NOT NULL default '',
  PRIMARY KEY  (`pf_rel_id`),
  KEY `log_item_refund_history_idx001` (`account_id`),
  KEY `log_item_refund_history_ibfk_1` (`purchase_tran_id`),
  KEY `log_item_refund_history_ibfk_2` (`recharge_tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_item_refund_history_20130221_4`
--

DROP TABLE IF EXISTS `bak_log_item_refund_history_20130221_4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_item_refund_history_20130221_4` (
  `pf_rel_id` int(10) unsigned NOT NULL auto_increment,
  `purchase_tran_id` bigint(20) unsigned NOT NULL,
  `recharge_tran_id` bigint(20) unsigned NOT NULL,
  `account_id` char(30) NOT NULL,
  `occ_date` datetime NOT NULL,
  `reason` varchar(255) NOT NULL default '',
  `admin_id` varchar(30) NOT NULL default '',
  PRIMARY KEY  (`pf_rel_id`),
  KEY `log_item_refund_history_idx001` (`account_id`),
  KEY `log_item_refund_history_ibfk_1` (`purchase_tran_id`),
  KEY `log_item_refund_history_ibfk_2` (`recharge_tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_point_history_20130221`
--

DROP TABLE IF EXISTS `bak_log_point_history_20130221`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_point_history_20130221` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `account_id` varchar(30) NOT NULL default '',
  `charac_id` varchar(30) NOT NULL default '',
  `cera_point` int(10) unsigned NOT NULL default '0',
  `command` enum('A','U') NOT NULL COMMENT 'A(add), U(use)',
  `charge_type` tinyint(4) NOT NULL default '0',
  `free_charge_type` tinyint(4) NOT NULL default '0',
  `reg_date` datetime NOT NULL,
  PRIMARY KEY  (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_point_history_20130221_2`
--

DROP TABLE IF EXISTS `bak_log_point_history_20130221_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_point_history_20130221_2` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `account_id` varchar(30) NOT NULL default '',
  `charac_id` varchar(30) NOT NULL default '',
  `cera_point` int(10) unsigned NOT NULL default '0',
  `command` enum('A','U') NOT NULL COMMENT 'A(add), U(use)',
  `charge_type` tinyint(4) NOT NULL default '0',
  `free_charge_type` tinyint(4) NOT NULL default '0',
  `reg_date` datetime NOT NULL,
  PRIMARY KEY  (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_point_history_20130221_3`
--

DROP TABLE IF EXISTS `bak_log_point_history_20130221_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_point_history_20130221_3` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `account_id` varchar(30) NOT NULL default '',
  `charac_id` varchar(30) NOT NULL default '',
  `cera_point` int(10) unsigned NOT NULL default '0',
  `command` enum('A','U') NOT NULL COMMENT 'A(add), U(use)',
  `charge_type` tinyint(4) NOT NULL default '0',
  `free_charge_type` tinyint(4) NOT NULL default '0',
  `reg_date` datetime NOT NULL,
  PRIMARY KEY  (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_point_history_20130221_4`
--

DROP TABLE IF EXISTS `bak_log_point_history_20130221_4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_point_history_20130221_4` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `account_id` varchar(30) NOT NULL default '',
  `charac_id` varchar(30) NOT NULL default '',
  `cera_point` int(10) unsigned NOT NULL default '0',
  `command` enum('A','U') NOT NULL COMMENT 'A(add), U(use)',
  `charge_type` tinyint(4) NOT NULL default '0',
  `free_charge_type` tinyint(4) NOT NULL default '0',
  `reg_date` datetime NOT NULL,
  PRIMARY KEY  (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_purchase_history_20130103`
--

DROP TABLE IF EXISTS `bak_log_purchase_history_20130103`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_purchase_history_20130103` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `account_id` char(30) NOT NULL,
  `charac_id` char(30) NOT NULL,
  `item_id` int(10) unsigned NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `befor_cera` int(10) unsigned NOT NULL,
  `after_cera` int(10) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='purchase history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_purchase_history_20130219`
--

DROP TABLE IF EXISTS `bak_log_purchase_history_20130219`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_purchase_history_20130219` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `account_id` char(30) NOT NULL,
  `charac_id` char(30) NOT NULL,
  `item_id` int(10) unsigned NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `befor_cera` int(10) unsigned NOT NULL,
  `after_cera` int(10) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='purchase history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_purchase_history_20130221`
--

DROP TABLE IF EXISTS `bak_log_purchase_history_20130221`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_purchase_history_20130221` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `account_id` char(30) NOT NULL,
  `charac_id` char(30) NOT NULL,
  `item_id` int(10) unsigned NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `befor_cera` int(10) unsigned NOT NULL,
  `after_cera` int(10) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='purchase history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_purchase_history_20130221_2`
--

DROP TABLE IF EXISTS `bak_log_purchase_history_20130221_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_purchase_history_20130221_2` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `account_id` char(30) NOT NULL,
  `charac_id` char(30) NOT NULL,
  `item_id` int(10) unsigned NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `befor_cera` int(10) unsigned NOT NULL,
  `after_cera` int(10) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='purchase history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_purchase_history_20130221_3`
--

DROP TABLE IF EXISTS `bak_log_purchase_history_20130221_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_purchase_history_20130221_3` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `account_id` char(30) NOT NULL,
  `charac_id` char(30) NOT NULL,
  `item_id` int(10) unsigned NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `befor_cera` int(10) unsigned NOT NULL,
  `after_cera` int(10) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='purchase history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_purchase_history_20130221_4`
--

DROP TABLE IF EXISTS `bak_log_purchase_history_20130221_4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_purchase_history_20130221_4` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `account_id` char(30) NOT NULL,
  `charac_id` char(30) NOT NULL,
  `item_id` int(10) unsigned NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `befor_cera` int(10) unsigned NOT NULL,
  `after_cera` int(10) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='purchase history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_recharge_history_20130103`
--

DROP TABLE IF EXISTS `bak_log_recharge_history_20130103`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_recharge_history_20130103` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `account_id` varchar(30) NOT NULL,
  `charac_id` varchar(30) NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `befor_cera` int(10) unsigned NOT NULL,
  `after_cera` int(10) unsigned NOT NULL,
  `charge_type` tinyint(3) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='recharge history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_recharge_history_20130219`
--

DROP TABLE IF EXISTS `bak_log_recharge_history_20130219`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_recharge_history_20130219` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `account_id` varchar(30) NOT NULL,
  `charac_id` varchar(30) NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `befor_cera` int(10) unsigned NOT NULL,
  `after_cera` int(10) unsigned NOT NULL,
  `charge_type` tinyint(3) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='recharge history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_recharge_history_20130221`
--

DROP TABLE IF EXISTS `bak_log_recharge_history_20130221`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_recharge_history_20130221` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `order_tran_id` varchar(35) NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `account_id` varchar(30) NOT NULL,
  `charac_id` varchar(30) NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `befor_cera` int(10) unsigned NOT NULL,
  `after_cera` int(10) unsigned NOT NULL,
  `charge_type` tinyint(3) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`),
  KEY `log_recharge_history_idx01` (`account_id`),
  KEY `log_recharge_history_idx02` (`charac_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='recharge history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_recharge_history_20130221_2`
--

DROP TABLE IF EXISTS `bak_log_recharge_history_20130221_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_recharge_history_20130221_2` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `order_tran_id` varchar(35) NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `account_id` varchar(30) NOT NULL,
  `charac_id` varchar(30) NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `befor_cera` int(10) unsigned NOT NULL,
  `after_cera` int(10) unsigned NOT NULL,
  `charge_type` tinyint(3) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`),
  KEY `log_recharge_history_idx01` (`account_id`),
  KEY `log_recharge_history_idx02` (`charac_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='recharge history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_recharge_history_20130221_3`
--

DROP TABLE IF EXISTS `bak_log_recharge_history_20130221_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_recharge_history_20130221_3` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `order_tran_id` varchar(35) NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `account_id` varchar(30) NOT NULL,
  `charac_id` varchar(30) NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `befor_cera` int(10) unsigned NOT NULL,
  `after_cera` int(10) unsigned NOT NULL,
  `charge_type` tinyint(3) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`),
  KEY `log_recharge_history_idx01` (`account_id`),
  KEY `log_recharge_history_idx02` (`charac_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='recharge history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_recharge_history_20130221_4`
--

DROP TABLE IF EXISTS `bak_log_recharge_history_20130221_4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_recharge_history_20130221_4` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `order_tran_id` varchar(35) NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `account_id` varchar(30) NOT NULL,
  `charac_id` varchar(30) NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `befor_cera` int(10) unsigned NOT NULL,
  `after_cera` int(10) unsigned NOT NULL,
  `charge_type` tinyint(3) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`),
  KEY `log_recharge_history_idx01` (`account_id`),
  KEY `log_recharge_history_idx02` (`charac_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='recharge history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_refund_history_20130221`
--

DROP TABLE IF EXISTS `bak_log_refund_history_20130221`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_refund_history_20130221` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `account_id` varchar(30) NOT NULL,
  `order_tran_id` varchar(35) NOT NULL,
  `amount` int(10) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`,`account_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_refund_history_20130221_2`
--

DROP TABLE IF EXISTS `bak_log_refund_history_20130221_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_refund_history_20130221_2` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `account_id` varchar(30) NOT NULL,
  `order_tran_id` varchar(35) NOT NULL,
  `amount` int(10) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`,`account_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_refund_history_20130221_3`
--

DROP TABLE IF EXISTS `bak_log_refund_history_20130221_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_refund_history_20130221_3` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `account_id` varchar(30) NOT NULL,
  `order_tran_id` varchar(35) NOT NULL,
  `amount` int(10) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`,`account_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_refund_history_20130221_4`
--

DROP TABLE IF EXISTS `bak_log_refund_history_20130221_4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_refund_history_20130221_4` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `account_id` varchar(30) NOT NULL,
  `order_tran_id` varchar(35) NOT NULL,
  `amount` int(10) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`,`account_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_transaction_history_20130221`
--

DROP TABLE IF EXISTS `bak_log_transaction_history_20130221`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_transaction_history_20130221` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_type` tinyint(3) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_transaction_history_20130221_2`
--

DROP TABLE IF EXISTS `bak_log_transaction_history_20130221_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_transaction_history_20130221_2` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_type` tinyint(3) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_transaction_history_20130221_3`
--

DROP TABLE IF EXISTS `bak_log_transaction_history_20130221_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_transaction_history_20130221_3` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_type` tinyint(3) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bak_log_transaction_history_20130221_4`
--

DROP TABLE IF EXISTS `bak_log_transaction_history_20130221_4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bak_log_transaction_history_20130221_4` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_type` tinyint(3) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cash_cera`
--

DROP TABLE IF EXISTS `cash_cera`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cash_cera` (
  `account` varchar(30) NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `mod_tran` bigint(20) unsigned NOT NULL,
  `mod_date` datetime NOT NULL,
  `reg_date` datetime NOT NULL,
  PRIMARY KEY  (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cash_cera_point`
--

DROP TABLE IF EXISTS `cash_cera_point`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cash_cera_point` (
  `account` varchar(30) NOT NULL default '',
  `cera_point` int(10) unsigned NOT NULL,
  `reg_date` datetime NOT NULL,
  `mod_date` datetime NOT NULL,
  PRIMARY KEY  (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cash_transaction`
--

DROP TABLE IF EXISTS `cash_transaction`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cash_transaction` (
  `tran_id` bigint(20) NOT NULL auto_increment,
  `dummy` char(1) NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cs_table2`
--

DROP TABLE IF EXISTS `cs_table2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cs_table2` (
  `account_id` char(30) NOT NULL,
  `charac_id` char(30) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_error_history`
--

DROP TABLE IF EXISTS `log_error_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_error_history` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `error_id` int(10) NOT NULL,
  `error_msg` varchar(255) NOT NULL,
  `error_query` varchar(512) NOT NULL,
  `proc_name` varchar(45) NOT NULL,
  `proc_line` int(10) NOT NULL,
  `query_user` varchar(45) NOT NULL default 'None',
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_gift_history`
--

DROP TABLE IF EXISTS `log_gift_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_gift_history` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `send_account_id` varchar(30) NOT NULL,
  `send_charac_id` varchar(30) NOT NULL,
  `recv_account_id` varchar(30) NOT NULL,
  `item_id` int(10) unsigned NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `send_befor_cera` int(10) unsigned NOT NULL,
  `send_after_cera` int(10) unsigned NOT NULL,
  `recv_befor_cera` int(10) unsigned NOT NULL,
  `recv_after_cera` int(10) unsigned NOT NULL,
  `query_user` varchar(45) NOT NULL default 'None',
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='gift history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_item_refund_history`
--

DROP TABLE IF EXISTS `log_item_refund_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_item_refund_history` (
  `pf_rel_id` int(10) unsigned NOT NULL auto_increment,
  `purchase_tran_id` bigint(20) unsigned NOT NULL,
  `recharge_tran_id` bigint(20) unsigned NOT NULL,
  `account_id` char(30) NOT NULL,
  `occ_date` datetime NOT NULL,
  `reason` varchar(255) NOT NULL default '',
  `admin_id` varchar(30) NOT NULL default '',
  `query_user` varchar(45) NOT NULL default 'None',
  PRIMARY KEY  (`pf_rel_id`),
  KEY `log_item_refund_history_idx001` (`account_id`),
  KEY `log_item_refund_history_ibfk_1` (`purchase_tran_id`),
  KEY `log_item_refund_history_ibfk_2` (`recharge_tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_point_history`
--

DROP TABLE IF EXISTS `log_point_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_point_history` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `account_id` varchar(30) NOT NULL default '',
  `charac_id` varchar(30) NOT NULL default '',
  `cera_point` int(10) unsigned NOT NULL default '0',
  `command` enum('A','U') NOT NULL COMMENT 'A(add), U(use)',
  `charge_type` tinyint(4) NOT NULL default '0',
  `free_charge_type` tinyint(4) NOT NULL default '0',
  `item_id` int(10) unsigned NOT NULL default '0',
  `query_user` varchar(45) NOT NULL default 'None',
  `reg_date` datetime NOT NULL,
  PRIMARY KEY  (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_purchase_history`
--

DROP TABLE IF EXISTS `log_purchase_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_purchase_history` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `account_id` char(30) NOT NULL,
  `charac_id` char(30) NOT NULL,
  `item_id` int(10) unsigned NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `befor_cera` int(10) unsigned NOT NULL,
  `after_cera` int(10) unsigned NOT NULL,
  `query_user` varchar(45) NOT NULL default 'None',
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='purchase history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_recharge_history`
--

DROP TABLE IF EXISTS `log_recharge_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_recharge_history` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `order_tran_id` varchar(35) NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `account_id` varchar(30) NOT NULL,
  `charac_id` varchar(30) NOT NULL,
  `cera` int(10) unsigned NOT NULL,
  `befor_cera` int(10) unsigned NOT NULL,
  `after_cera` int(10) unsigned NOT NULL,
  `charge_type` tinyint(3) unsigned NOT NULL,
  `query_user` varchar(45) NOT NULL default 'None',
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`),
  KEY `log_recharge_history_idx01` (`account_id`),
  KEY `log_recharge_history_idx02` (`charac_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='recharge history';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_refund_history`
--

DROP TABLE IF EXISTS `log_refund_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_refund_history` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `account_id` varchar(30) NOT NULL,
  `order_tran_id` varchar(35) NOT NULL,
  `amount` int(10) unsigned NOT NULL,
  `tran_state` tinyint(3) unsigned NOT NULL,
  `query_user` varchar(45) NOT NULL default 'None',
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`,`account_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_transaction_history`
--

DROP TABLE IF EXISTS `log_transaction_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_transaction_history` (
  `tran_id` bigint(20) unsigned NOT NULL,
  `tran_type` tinyint(3) unsigned NOT NULL,
  `occ_date` datetime NOT NULL,
  PRIMARY KEY  (`tran_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Current Database: `taiwan_login`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `taiwan_login` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `taiwan_login`;

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
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2025-11-12  1:40:51
