-- MySQL dump 10.11
--
-- Host: localhost    Database: taiwan_cain
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
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;
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
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2025-10-20 11:32:25
