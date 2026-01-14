-- MySQL dump 10.11
--
-- Host: localhost    Database: taiwan_cain_log
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
-- Table structure for table `channel_lev_status`
--

DROP TABLE IF EXISTS `channel_lev_status`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `channel_lev_status` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `channel_no` tinyint(4) NOT NULL default '0',
  `lev` tinyint(4) NOT NULL default '0',
  `occ_count` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`channel_no`,`lev`),
  KEY `idx_channel_no` USING BTREE (`channel_no`),
  KEY `idx_lev` USING BTREE (`lev`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `client_down`
--

DROP TABLE IF EXISTS `client_down`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `client_down` (
  `occ_date` date NOT NULL default '0000-00-00',
  `occ_count` int(10) unsigned default '0',
  PRIMARY KEY  (`occ_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `concurrent_user_status`
--

DROP TABLE IF EXISTS `concurrent_user_status`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `concurrent_user_status` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `player_status` tinyint(4) NOT NULL default '0',
  `dungeon_index` int(11) NOT NULL default '0',
  `channel_no` int(11) NOT NULL default '0',
  `occ_count` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`player_status`,`dungeon_index`,`channel_no`),
  KEY `idx_player_status` USING BTREE (`player_status`),
  KEY `idx_dungeon_index` USING BTREE (`dungeon_index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `crash_down_by_dungeon`
--

DROP TABLE IF EXISTS `crash_down_by_dungeon`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `crash_down_by_dungeon` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `dungeon_index` int(11) NOT NULL default '0',
  `down_count` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`dungeon_index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `crash_down_by_packet`
--

DROP TABLE IF EXISTS `crash_down_by_packet`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `crash_down_by_packet` (
  `occ_date` date NOT NULL default '0000-00-00',
  `packet_index` smallint(5) NOT NULL default '0',
  `down_count` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`packet_index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `crash_down_by_playstatus`
--

DROP TABLE IF EXISTS `crash_down_by_playstatus`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `crash_down_by_playstatus` (
  `occ_date` date NOT NULL default '0000-00-00',
  `play_status` tinyint(3) unsigned NOT NULL default '0',
  `down_count` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`play_status`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `d_date`
--

DROP TABLE IF EXISTS `d_date`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `d_date` (
  `aa` date NOT NULL default '0000-00-00'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_dbq_log`
--

DROP TABLE IF EXISTS `dnf_dbq_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_dbq_log` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `channel_no` tinyint(3) unsigned NOT NULL default '0',
  `dbq` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`channel_no`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_hack_log`
--

DROP TABLE IF EXISTS `dnf_hack_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_hack_log` (
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `hack_type` smallint(5) unsigned NOT NULL default '0',
  `last_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `etc` int(10) unsigned NOT NULL default '0',
  `cnt` int(10) unsigned NOT NULL default '0',
  `occ_date` date NOT NULL default '0000-00-00',
  PRIMARY KEY  (`m_id`,`hack_type`,`occ_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_hack_log_del`
--

DROP TABLE IF EXISTS `dnf_hack_log_del`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_hack_log_del` (
  `sdate` date NOT NULL default '0000-00-00',
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `hack_type` smallint(5) unsigned NOT NULL default '0',
  `last_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `etc` int(10) unsigned NOT NULL default '0',
  `cnt` int(10) unsigned NOT NULL default '0',
  `occ_date` date NOT NULL default '0000-00-00',
  PRIMARY KEY  (`sdate`,`occ_date`,`m_id`,`hack_type`),
  KEY `idx_occdate` USING BTREE (`occ_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_party_ping_history`
--

DROP TABLE IF EXISTS `dnf_party_ping_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_party_ping_history` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `str_ip` char(16) NOT NULL default '',
  `m_id` int(11) NOT NULL default '0',
  `ip_1` char(16) NOT NULL default '',
  `nat_type_1` tinyint(4) NOT NULL default '0',
  `ping1` int(11) NOT NULL default '0',
  `mtu_1` int(11) NOT NULL default '0',
  `ip_2` char(16) NOT NULL default '',
  `nat_type_2` tinyint(4) NOT NULL default '0',
  `ping2` int(11) NOT NULL default '0',
  `mtu_2` int(11) NOT NULL default '0',
  `ip_3` char(16) NOT NULL default '',
  `nat_type_3` tinyint(4) NOT NULL default '0',
  `ping3` int(11) NOT NULL default '0',
  `mtu_3` int(11) NOT NULL default '0',
  `ip_4` char(16) NOT NULL default '',
  `nat_type_4` tinyint(4) NOT NULL default '0',
  `ping4` int(11) NOT NULL default '0',
  `mtu_4` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `map_id` int(11) NOT NULL default '0',
  `dungeon_id` int(11) NOT NULL default '0',
  `end_reason` char(1) NOT NULL default '',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_party_ping_history_pvp`
--

DROP TABLE IF EXISTS `dnf_party_ping_history_pvp`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_party_ping_history_pvp` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `str_ip` char(16) NOT NULL default '',
  `m_id` int(11) NOT NULL default '0',
  `ip_1` char(16) NOT NULL default '',
  `nat_type_1` tinyint(4) NOT NULL default '0',
  `ping1` int(11) NOT NULL default '0',
  `mtu_1` int(11) NOT NULL default '0',
  `ip_2` char(16) NOT NULL default '',
  `nat_type_2` tinyint(4) NOT NULL default '0',
  `ping2` int(11) NOT NULL default '0',
  `mtu_2` int(11) NOT NULL default '0',
  `ip_3` char(16) NOT NULL default '',
  `nat_type_3` tinyint(4) NOT NULL default '0',
  `ping3` int(11) NOT NULL default '0',
  `mtu_3` int(11) NOT NULL default '0',
  `ip_4` char(16) NOT NULL default '',
  `nat_type_4` tinyint(4) NOT NULL default '0',
  `ping4` int(11) NOT NULL default '0',
  `mtu_4` int(11) NOT NULL default '0',
  `ip_5` char(16) NOT NULL default '',
  `nat_type_5` tinyint(4) NOT NULL default '0',
  `ping5` int(11) NOT NULL default '0',
  `mtu_5` int(11) NOT NULL default '0',
  `ip_6` char(16) NOT NULL default '',
  `nat_type_6` tinyint(4) NOT NULL default '0',
  `ping6` int(11) NOT NULL default '0',
  `mtu_6` int(11) NOT NULL default '0',
  `ip_7` char(16) NOT NULL default '',
  `nat_type_7` tinyint(4) NOT NULL default '0',
  `ping7` int(11) NOT NULL default '0',
  `mtu_7` int(11) NOT NULL default '0',
  `ip_8` char(16) NOT NULL default '',
  `nat_type_8` tinyint(4) NOT NULL default '0',
  `ping8` int(11) NOT NULL default '0',
  `mtu_8` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `map_id` int(11) NOT NULL default '0',
  `dungeon_id` int(11) NOT NULL default '0',
  `end_reason` char(1) NOT NULL default '',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_party_result_history`
--

DROP TABLE IF EXISTS `dnf_party_result_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_party_result_history` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `str_ip` char(16) NOT NULL default '',
  `nat_type` tinyint(4) NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `party_result` tinyint(4) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `map_id` int(11) NOT NULL default '0',
  `dungeon_id` int(11) NOT NULL default '0',
  `peer_ip` char(16) NOT NULL default '',
  `peer_nat_type` tinyint(4) NOT NULL default '0',
  `established_time` int(10) unsigned NOT NULL default '0',
  `IsConnectedToRelaySrv` char(1) NOT NULL default '',
  `ConnectedTimeToRelaySrv` int(10) unsigned NOT NULL default '0',
  `relay_ip` char(16) NOT NULL default '',
  `relay_udp_port` int(10) unsigned NOT NULL default '0',
  `relay_tcp_port` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_party_result_history_pvp`
--

DROP TABLE IF EXISTS `dnf_party_result_history_pvp`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_party_result_history_pvp` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `str_ip` char(16) NOT NULL default '',
  `nat_type` tinyint(4) NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  `party_result` tinyint(4) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `map_id` int(11) NOT NULL default '0',
  `dungeon_id` int(11) NOT NULL default '0',
  `peer_ip` char(16) NOT NULL default '',
  `peer_nat_type` tinyint(4) NOT NULL default '0',
  `established_time` int(10) unsigned NOT NULL default '0',
  `IsConnectedToRelaySrv` char(1) NOT NULL default '',
  `ConnectedTimeToRelaySrv` int(10) unsigned NOT NULL default '0',
  `relay_ip` char(16) NOT NULL default '',
  `relay_udp_port` int(10) unsigned NOT NULL default '0',
  `relay_tcp_port` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_party_stat_history`
--

DROP TABLE IF EXISTS `dnf_party_stat_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_party_stat_history` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `str_ip` char(16) NOT NULL default '',
  `m_id` int(11) NOT NULL default '0',
  `try_num` int(11) NOT NULL default '0',
  `party_result` char(30) NOT NULL default '',
  `party_num` int(11) NOT NULL default '0',
  `ping1` int(11) NOT NULL default '0',
  `ping2` int(11) NOT NULL default '0',
  `ping3` int(11) NOT NULL default '0',
  `ping4` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_proxy_log`
--

DROP TABLE IF EXISTS `dnf_proxy_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_proxy_log` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `m_id` int(11) NOT NULL default '0',
  `nat_ip` varchar(16) NOT NULL default '',
  `peer_ip` varchar(16) NOT NULL default '',
  `nat_tye` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`m_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dnf_zeromoney_log`
--

DROP TABLE IF EXISTS `dnf_zeromoney_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dnf_zeromoney_log` (
  `to_m_id` int(11) NOT NULL default '0',
  `to_nat_ip` varchar(16) NOT NULL default '',
  `to_peer_ip` varchar(16) NOT NULL default '',
  `to_nat_type` tinyint(4) NOT NULL default '0',
  `to_mac_addr` varchar(64) NOT NULL default '',
  `from_m_id` int(11) NOT NULL default '0',
  `from_nat_ip` varchar(16) NOT NULL default '',
  `from_peer_ip` varchar(16) NOT NULL default '',
  `from_nat_type` tinyint(4) NOT NULL default '0',
  `from_mac_addr` varchar(64) NOT NULL default '',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `injury_money` int(10) unsigned NOT NULL default '0',
  KEY `idx_to_m_id` USING BTREE (`to_m_id`),
  KEY `idx_from_m_id` USING BTREE (`from_m_id`),
  KEY `idx_occ_time` USING BTREE (`occ_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dungeon_fail`
--

DROP TABLE IF EXISTS `dungeon_fail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dungeon_fail` (
  `occ_date` date NOT NULL default '0000-00-00',
  `lev` tinyint(4) NOT NULL default '0',
  `occ_count` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`lev`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dungeon_lev_log`
--

DROP TABLE IF EXISTS `dungeon_lev_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dungeon_lev_log` (
  `occ_date` date NOT NULL default '0000-00-00',
  `dungeon_id` int(11) NOT NULL default '0',
  `sum_lev` int(11) NOT NULL default '0',
  `used_coin` int(11) NOT NULL default '0',
  `try_cnt` int(11) NOT NULL default '0',
  `clear_cnt` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`dungeon_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_goldcard_log`
--

DROP TABLE IF EXISTS `event_goldcard_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_goldcard_log` (
  `m_id` int(11) NOT NULL default '0',
  `occ_date` date NOT NULL default '0000-00-00',
  `cnt` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`m_id`,`occ_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_quest_party_member`
--

DROP TABLE IF EXISTS `event_quest_party_member`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_quest_party_member` (
  `charac_no` int(11) unsigned NOT NULL default '0',
  `quest_no` int(11) NOT NULL default '0',
  `member_1` int(11) NOT NULL default '0',
  `member_1_name` varchar(20) NOT NULL default '',
  `member_1_lev` tinyint(3) unsigned NOT NULL default '0',
  `member_2` int(11) NOT NULL default '0',
  `member_2_name` varchar(20) NOT NULL default '',
  `member_2_lev` tinyint(3) unsigned NOT NULL default '0',
  `member_3` int(11) NOT NULL default '0',
  `member_3_name` varchar(20) NOT NULL default '',
  `member_3_lev` tinyint(3) unsigned NOT NULL default '0',
  `m_id` int(11) NOT NULL default '0',
  PRIMARY KEY  (`charac_no`,`quest_no`),
  KEY `idx_m_id` USING BTREE (`m_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_acquire_item`
--

DROP TABLE IF EXISTS `log_acquire_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_acquire_item` (
  `occ_date` date NOT NULL default '0000-00-00',
  `m_id` bigint(20) unsigned default NULL,
  `charac_no` int(11) NOT NULL default '0',
  `item_id` int(11) unsigned NOT NULL default '0',
  `item_type` int(11) NOT NULL default '0',
  `item_count` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`charac_no`,`item_id`),
  KEY `idx_date_mid_itemid` USING BTREE (`occ_date`,`m_id`,`item_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_advance_alter_entrance`
--

DROP TABLE IF EXISTS `log_advance_alter_entrance`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_advance_alter_entrance` (
  `occ_date` date NOT NULL default '0000-00-00',
  `stage_id` int(10) unsigned NOT NULL default '0',
  `entrance_count` int(10) unsigned NOT NULL default '0',
  `success_count` int(10) unsigned NOT NULL default '0',
  `obtain_star` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`stage_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_advance_alter_layer_stat`
--

DROP TABLE IF EXISTS `log_advance_alter_layer_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_advance_alter_layer_stat` (
  `occ_date` date NOT NULL default '0000-00-00',
  `item_type` smallint(10) unsigned NOT NULL default '0',
  `item_id` int(5) unsigned NOT NULL default '0',
  `item_level` smallint(10) unsigned NOT NULL default '0',
  `use_count` smallint(10) unsigned NOT NULL default '0',
  `use_star` smallint(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`item_type`,`item_id`,`item_level`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_arad_event_stat`
--

DROP TABLE IF EXISTS `log_arad_event_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_arad_event_stat` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `item_id` int(11) NOT NULL default '0',
  `item_cnt` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`m_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_assault`
--

DROP TABLE IF EXISTS `log_assault`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_assault` (
  `occ_date` date NOT NULL default '0000-00-00',
  `channel_no` smallint(5) unsigned NOT NULL default '0',
  `assault_count` int(10) unsigned NOT NULL default '0',
  `load_fail_count` int(11) default NULL,
  `p2p_fail_count` int(11) default NULL,
  PRIMARY KEY  (`occ_date`,`channel_no`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_avatar_grind`
--

DROP TABLE IF EXISTS `log_avatar_grind`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_avatar_grind` (
  `cur_date` date NOT NULL default '0000-00-00',
  `grade` enum('normal','high','rare') NOT NULL default 'normal',
  `body_part` enum('headgear','hair','face','jacket','pants','shoes','breast','waist','skin') NOT NULL default 'headgear',
  `avatar_emblem_grind` int(10) unsigned NOT NULL default '0',
  `avatar_bindcube_grind` int(10) unsigned NOT NULL default '0',
  `avatar_rechargestone_grind` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`cur_date`,`grade`,`body_part`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_cerashop_gift`
--

DROP TABLE IF EXISTS `log_cerashop_gift`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_cerashop_gift` (
  `idx` int(10) unsigned NOT NULL auto_increment,
  `occ_time` int(10) unsigned NOT NULL default '0',
  `from_mid` int(10) unsigned NOT NULL default '0',
  `from_charac_no` int(10) unsigned NOT NULL default '0',
  `to_mid` int(10) unsigned NOT NULL default '0',
  `to_charac_no` int(10) unsigned NOT NULL default '0',
  `item_idx` int(10) unsigned NOT NULL default '0',
  `item_cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`idx`)
) ENGINE=MyISAM AUTO_INCREMENT=18237 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_charac_money`
--

DROP TABLE IF EXISTS `log_charac_money`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_charac_money` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `occ_date` date NOT NULL default '0000-00-00',
  `money_plus` int(10) unsigned NOT NULL default '0',
  `money_minus` int(10) unsigned NOT NULL default '0',
  `m_id` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`charac_no`,`occ_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_chest_item_goldweek_2011`
--

DROP TABLE IF EXISTS `log_chest_item_goldweek_2011`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_chest_item_goldweek_2011` (
  `item_id` int(10) unsigned NOT NULL default '0',
  `cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`item_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_chest_item_goldweek_2012`
--

DROP TABLE IF EXISTS `log_chest_item_goldweek_2012`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_chest_item_goldweek_2012` (
  `item_id` int(10) unsigned NOT NULL default '0',
  `cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`item_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_chest_item_goldweek_2013`
--

DROP TABLE IF EXISTS `log_chest_item_goldweek_2013`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_chest_item_goldweek_2013` (
  `item_id` int(10) unsigned NOT NULL default '0',
  `cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`item_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_chest_item_goldweek_2016`
--

DROP TABLE IF EXISTS `log_chest_item_goldweek_2016`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_chest_item_goldweek_2016` (
  `item_id` int(10) unsigned NOT NULL default '0',
  `cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`item_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_client_ting`
--

DROP TABLE IF EXISTS `log_client_ting`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_client_ting` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `channel_no` int(11) NOT NULL default '0',
  `reason` int(11) NOT NULL default '0',
  `cnt` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`channel_no`,`reason`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_client_ting_stat`
--

DROP TABLE IF EXISTS `log_client_ting_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_client_ting_stat` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `channel_no` int(11) NOT NULL default '0',
  `reason` int(11) NOT NULL default '0',
  `cnt` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`channel_no`,`reason`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_cube_stat`
--

DROP TABLE IF EXISTS `log_cube_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_cube_stat` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `channel_no` int(11) NOT NULL default '0',
  `level` int(11) NOT NULL default '0',
  `item_index` int(11) NOT NULL default '0',
  `type` int(11) NOT NULL default '0',
  `item_count` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`channel_no`,`level`,`item_index`,`type`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_deathtower_playdata_job`
--

DROP TABLE IF EXISTS `log_deathtower_playdata_job`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_deathtower_playdata_job` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `type` tinyint(3) unsigned NOT NULL default '0',
  `charac_job` tinyint(3) unsigned NOT NULL default '0',
  `charac_grow` tinyint(3) unsigned NOT NULL default '0',
  `level` tinyint(3) unsigned NOT NULL default '0',
  `playcount` int(10) unsigned NOT NULL default '0',
  `avg_clear_count` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`type`,`charac_job`,`charac_grow`,`level`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_deathtower_playdata_party`
--

DROP TABLE IF EXISTS `log_deathtower_playdata_party`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_deathtower_playdata_party` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `type` tinyint(3) unsigned NOT NULL default '0',
  `party_count` tinyint(3) unsigned NOT NULL default '0',
  `playcount` int(10) unsigned NOT NULL default '0',
  `avg_clear_count` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`type`,`party_count`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_deathtower_value`
--

DROP TABLE IF EXISTS `log_deathtower_value`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_deathtower_value` (
  `occ_date` date NOT NULL default '0000-00-00',
  `type` tinyint(3) unsigned NOT NULL default '0',
  `level` tinyint(3) unsigned NOT NULL default '0',
  `clear_stage` int(10) unsigned NOT NULL default '0',
  `recipeCnt` int(10) unsigned NOT NULL default '0',
  `rareCnt` int(10) unsigned NOT NULL default '0',
  `uniqCnt` int(10) unsigned NOT NULL default '0',
  `card_item_goldprice` int(10) unsigned NOT NULL default '0',
  `card_gold` int(10) unsigned NOT NULL default '0',
  `repair_price` int(10) unsigned NOT NULL default '0',
  `commonCnt` int(10) unsigned NOT NULL default '0',
  `uncommonCnt` int(10) unsigned NOT NULL default '0',
  `try_cnt` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`type`,`level`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_dungeon_charac`
--

DROP TABLE IF EXISTS `log_dungeon_charac`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_dungeon_charac` (
  `channel_no` int(11) NOT NULL default '0' COMMENT '채널번호',
  `dungeon_index` int(11) NOT NULL default '0' COMMENT '던전번호',
  `dungeon_diff` int(11) NOT NULL default '0' COMMENT '던전난이도',
  `charac_job` int(11) NOT NULL default '0' COMMENT '케릭직업',
  `charac_grow` int(11) NOT NULL default '0' COMMENT '케릭성장직업',
  `clear_time` int(11) NOT NULL default '0' COMMENT '클리어시간',
  `die_count` int(11) NOT NULL default '0' COMMENT '죽은 횟수',
  `hp_consume` int(11) NOT NULL default '0' COMMENT 'hp 소비량',
  `mp_consume` int(11) NOT NULL default '0' COMMENT 'mp 소비량',
  `hit_count` int(11) NOT NULL default '0' COMMENT '히트수',
  `hit_per_avg_damage` int(11) NOT NULL default '0' COMMENT '평균 데미지',
  `hp_recovery` int(11) NOT NULL default '0' COMMENT 'hp 회복량',
  `mp_recovery` int(11) NOT NULL default '0' COMMENT 'mp 회복량',
  `success` int(11) NOT NULL default '0' COMMENT '성공구분',
  `last_time` datetime NOT NULL default '0000-00-00 00:00:00' COMMENT '등록일시',
  `update_count` int(11) NOT NULL default '0' COMMENT '업데이트 횟수',
  `level` int(11) NOT NULL default '0' COMMENT '레벨',
  `exp_avg` int(11) NOT NULL default '0' COMMENT '경험치평균',
  `dungeon_standard_level` int(11) NOT NULL default '0' COMMENT '던전표준레벨',
  `fatigue_consume` int(11) NOT NULL default '0' COMMENT '소비된피로도',
  `exp_add` int(11) NOT NULL default '0' COMMENT '경험치합계',
  `party_user_count` tinyint(3) unsigned NOT NULL default '0',
  `rank` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`last_time`,`channel_no`,`dungeon_index`,`dungeon_diff`,`dungeon_standard_level`,`charac_job`,`charac_grow`,`success`,`party_user_count`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='케릭터별 던전 로그 정보 저장';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_dungeon_entrance`
--

DROP TABLE IF EXISTS `log_dungeon_entrance`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_dungeon_entrance` (
  `occ_date` date NOT NULL default '0000-00-00',
  `channel_no` smallint(5) unsigned NOT NULL default '0',
  `dungeon_index` smallint(5) unsigned NOT NULL default '0',
  `enter_count` int(10) unsigned NOT NULL default '0',
  `exercise_count` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`channel_no`,`dungeon_index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_dungeon_entrance_hour`
--

DROP TABLE IF EXISTS `log_dungeon_entrance_hour`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_dungeon_entrance_hour` (
  `occ_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `channel_no` smallint(5) unsigned NOT NULL default '0',
  `dungeon_index` smallint(5) unsigned NOT NULL default '0',
  `hour_enter_count` int(10) unsigned NOT NULL default '0',
  `cnt_enter_count` int(10) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`channel_no`,`dungeon_index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_dungeon_exp`
--

DROP TABLE IF EXISTS `log_dungeon_exp`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_dungeon_exp` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `channel_no` int(11) NOT NULL default '0',
  `lev` int(11) NOT NULL default '0',
  `exp` bigint(20) unsigned NOT NULL default '0',
  `fatigue` bigint(20) unsigned NOT NULL default '0',
  `user_count` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`channel_no`,`lev`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_dungeon_monster`
--

DROP TABLE IF EXISTS `log_dungeon_monster`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_dungeon_monster` (
  `channel_no` int(11) NOT NULL default '0',
  `dungeon_index` int(11) NOT NULL default '0',
  `dungeon_diff` int(11) NOT NULL default '0',
  `monster_index` int(11) NOT NULL default '0',
  `damage_total` int(11) NOT NULL default '0',
  `damage_count` int(11) NOT NULL default '0',
  `last_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`last_time`,`channel_no`,`dungeon_index`,`dungeon_diff`,`monster_index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_dungeon_party`
--

DROP TABLE IF EXISTS `log_dungeon_party`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_dungeon_party` (
  `channel_no` int(11) NOT NULL default '0',
  `dungeon_index` int(11) NOT NULL default '0',
  `dungeon_diff` int(11) NOT NULL default '0',
  `dungeon_standard_level` tinyint(4) NOT NULL default '0',
  `abuse_party` tinyint(4) NOT NULL default '0',
  `balkun_party` tinyint(4) NOT NULL default '0',
  `party_user_count` int(11) NOT NULL default '0',
  `clear_time` int(11) NOT NULL default '0',
  `die_count` int(11) NOT NULL default '0',
  `hp_consume` int(11) NOT NULL default '0',
  `mp_consume` int(11) NOT NULL default '0',
  `hit_count` int(11) NOT NULL default '0',
  `hit_per_avg_damage` int(11) NOT NULL default '0',
  `hp_recovery` int(11) NOT NULL default '0',
  `mp_recovery` int(11) NOT NULL default '0',
  `success` int(11) NOT NULL default '0',
  `last_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `update_count` int(11) NOT NULL default '0',
  `level` int(11) NOT NULL default '0',
  `fatigue_consume` int(11) NOT NULL default '0',
  `exp_add` int(11) NOT NULL default '0',
  PRIMARY KEY  (`last_time`,`channel_no`,`dungeon_index`,`dungeon_diff`,`dungeon_standard_level`,`abuse_party`,`balkun_party`,`party_user_count`,`success`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_dungeon_party_job`
--

DROP TABLE IF EXISTS `log_dungeon_party_job`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_dungeon_party_job` (
  `channel_no` int(11) NOT NULL default '0',
  `dungeon_index` int(11) NOT NULL default '0',
  `dungeon_diff` int(11) NOT NULL default '0',
  `dungeon_standard_level` tinyint(4) NOT NULL default '0',
  `abuse_party` tinyint(4) NOT NULL default '0',
  `balkun_party` tinyint(4) NOT NULL default '0',
  `charac_job` int(11) NOT NULL default '0',
  `charac_grow` int(11) NOT NULL default '0',
  `party_user_count` int(11) NOT NULL default '0',
  `success` int(11) NOT NULL default '0',
  `last_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `job_count` int(11) NOT NULL default '0',
  `rank` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`last_time`,`channel_no`,`dungeon_index`,`dungeon_diff`,`dungeon_standard_level`,`abuse_party`,`balkun_party`,`charac_job`,`charac_grow`,`party_user_count`,`success`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_dungeon_skill`
--

DROP TABLE IF EXISTS `log_dungeon_skill`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_dungeon_skill` (
  `channel_no` int(11) NOT NULL default '0',
  `dungeon_index` int(11) NOT NULL default '0',
  `dungeon_diff` int(11) NOT NULL default '0',
  `charac_job` int(11) NOT NULL default '0',
  `charac_grow` int(11) NOT NULL default '0',
  `skill_index` int(11) NOT NULL default '0',
  `skill_use` int(11) NOT NULL default '0',
  `skill_count` int(11) NOT NULL default '0',
  `skill_cool_time` int(11) NOT NULL default '0',
  `last_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`last_time`,`channel_no`,`dungeon_index`,`dungeon_diff`,`charac_job`,`charac_grow`,`skill_index`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_eco_point`
--

DROP TABLE IF EXISTS `log_eco_point`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_eco_point` (
  `charac_no` int(10) unsigned NOT NULL default '0',
  `save_time` int(10) unsigned NOT NULL default '0',
  `item_id` int(10) unsigned NOT NULL default '0',
  `item_cnt` int(10) unsigned NOT NULL default '0',
  `add_point` int(10) unsigned NOT NULL default '0',
  `use_point` int(10) unsigned NOT NULL default '0',
  `save_type` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`charac_no`,`save_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_emblem_compound`
--

DROP TABLE IF EXISTS `log_emblem_compound`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_emblem_compound` (
  `occ_date` date NOT NULL default '0000-00-00' COMMENT '날짜',
  `channel_no` tinyint(3) unsigned NOT NULL default '0' COMMENT '채널 번호',
  `grade0` int(10) unsigned NOT NULL default '0' COMMENT '등급 별 카운트',
  `grade1` int(10) unsigned NOT NULL default '0' COMMENT '등급 별 카운트',
  `grade2` int(10) unsigned NOT NULL default '0' COMMENT '등급 별 카운트',
  `grade3` int(10) unsigned NOT NULL default '0' COMMENT '등급 별 카운트',
  `grade4` int(10) unsigned NOT NULL default '0' COMMENT '등급 별 카운트',
  `grade5` int(10) unsigned NOT NULL default '0' COMMENT '등급 별 카운트',
  `grade6` int(10) unsigned NOT NULL default '0' COMMENT '등급 별 카운트',
  PRIMARY KEY  (`occ_date`,`channel_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_emblem_create`
--

DROP TABLE IF EXISTS `log_emblem_create`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_emblem_create` (
  `cur_date` date NOT NULL default '0000-00-00' COMMENT '날짜',
  `grade0` int(10) unsigned NOT NULL default '0' COMMENT '등급 별 카운트',
  `grade1` int(10) unsigned NOT NULL default '0' COMMENT '등급 별 카운트',
  `grade2` int(10) unsigned NOT NULL default '0' COMMENT '등급 별 카운트',
  `grade3` int(10) unsigned NOT NULL default '0' COMMENT '등급 별 카운트',
  `grade4` int(10) unsigned NOT NULL default '0' COMMENT '등급 별 카운트',
  `grade5` int(10) unsigned NOT NULL default '0' COMMENT '등급 별 카운트',
  `grade6` int(10) unsigned NOT NULL default '0' COMMENT '등급 별 카운트',
  PRIMARY KEY  (`cur_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_fatigue_daily`
--

DROP TABLE IF EXISTS `log_fatigue_daily`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_fatigue_daily` (
  `occ_date` date NOT NULL default '0000-00-00',
  `fatigue` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_game_channel`
--

DROP TABLE IF EXISTS `log_game_channel`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_game_channel` (
  `gc_up_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `gc_game` tinyint(3) unsigned NOT NULL default '0',
  `gc_no` int(11) unsigned NOT NULL default '0',
  `gc_channel` char(32) NOT NULL default '',
  `gc_ch_group` smallint(5) NOT NULL default '0',
  `gc_ip` char(64) NOT NULL default '',
  `gc_new` smallint(5) unsigned NOT NULL default '0',
  `gc_now` smallint(5) unsigned NOT NULL default '0',
  `gc_out` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`gc_up_time`,`gc_game`,`gc_no`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_gameserver_load_stat`
--

DROP TABLE IF EXISTS `log_gameserver_load_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_gameserver_load_stat` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `server_id` tinyint(3) unsigned NOT NULL default '0',
  `channel_no` int(10) unsigned NOT NULL default '0',
  `dbq_cnt` int(10) unsigned NOT NULL default '0',
  `logdbq_cnt` int(10) unsigned NOT NULL default '0',
  `netq_cnt` int(10) unsigned NOT NULL default '0',
  `packetq_cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`server_id`,`channel_no`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_gameserver_load_stat_del`
--

DROP TABLE IF EXISTS `log_gameserver_load_stat_del`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_gameserver_load_stat_del` (
  `sdate` date NOT NULL default '0000-00-00',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `server_id` tinyint(3) unsigned NOT NULL default '0',
  `channel_no` int(10) unsigned NOT NULL default '0',
  `dbq_cnt` int(10) unsigned NOT NULL default '0',
  `logdbq_cnt` int(10) unsigned NOT NULL default '0',
  `netq_cnt` int(10) unsigned NOT NULL default '0',
  `packetq_cnt` int(10) unsigned NOT NULL default '0',
  KEY `idx1` USING BTREE (`sdate`,`occ_time`,`server_id`,`channel_no`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_gold_stat`
--

DROP TABLE IF EXISTS `log_gold_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_gold_stat` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `channel_no` int(11) NOT NULL default '0',
  `level` int(11) NOT NULL default '0',
  `dungeon_drop` int(10) unsigned NOT NULL default '0',
  `result_card` int(10) unsigned NOT NULL default '0',
  `sell_store` int(10) unsigned NOT NULL default '0',
  `quest_reward` int(10) unsigned NOT NULL default '0',
  `death_tower_reward` int(10) unsigned NOT NULL default '0',
  `illusion_tower_reward` int(10) unsigned NOT NULL default '0',
  `war_area_drop` int(10) unsigned NOT NULL default '0',
  `member_tax` int(10) unsigned NOT NULL default '0',
  `sell_auction` int(10) unsigned NOT NULL default '0',
  `fail_sell_auction` int(10) unsigned NOT NULL default '0',
  `fail_bidding_auction` int(10) unsigned NOT NULL default '0',
  `buy_store` int(10) unsigned NOT NULL default '0',
  `stamina_recovery` int(10) unsigned NOT NULL default '0',
  `repair_item` int(10) unsigned NOT NULL default '0',
  `private_store_commission` int(10) unsigned NOT NULL default '0',
  `gold_card` int(10) unsigned NOT NULL default '0',
  `auction_commission` int(10) unsigned NOT NULL default '0',
  `gold_drop` int(10) unsigned NOT NULL default '0',
  `upgrade` int(10) unsigned NOT NULL default '0',
  `quest_use` int(10) unsigned NOT NULL default '0',
  `mail_commission` int(10) unsigned NOT NULL default '0',
  `punish_user` int(10) unsigned NOT NULL default '0',
  `restrict_trade` int(10) unsigned NOT NULL default '0',
  `guild_level_up` int(10) unsigned NOT NULL default '0',
  `guild_skill` int(10) unsigned NOT NULL default '0',
  `guild_mail` int(10) unsigned NOT NULL default '0',
  `item_compound` int(10) unsigned NOT NULL default '0',
  `auction_bidding` int(10) unsigned NOT NULL default '0',
  `war_area_enter` int(10) unsigned NOT NULL default '0',
  `assault_gold` int(10) unsigned NOT NULL default '0',
  `blood_dungeon_reward` int(10) unsigned NOT NULL default '0',
  `power_dungeon_drop` int(10) unsigned NOT NULL default '0',
  `power_dungeon_result_card` int(10) unsigned NOT NULL default '0',
  `blood_dungeon_enter` int(10) unsigned NOT NULL default '0',
  `blood_dungeon_lotto` int(10) unsigned NOT NULL default '0',
  `buy_cerashop` int(10) unsigned NOT NULL default '0',
  `upgrade_guild_agit` int(10) unsigned NOT NULL default '0',
  `upgrade_guild_cargo` int(10) unsigned NOT NULL default '0',
  `break_away_reward` int(10) unsigned NOT NULL default '0',
  `link_charac_bonus` int(10) unsigned NOT NULL default '0',
  `ultimate_dungeon_reward` int(10) unsigned NOT NULL default '0',
  `guild_fund` int(10) unsigned NOT NULL default '0',
  `guild_fund_dungeon` int(10) unsigned NOT NULL default '0',
  `quest_shop_init_cost` int(10) unsigned NOT NULL default '0',
  `unseal` int(10) unsigned NOT NULL default '0',
  `lottery` int(10) unsigned NOT NULL default '0',
  `amplify` int(10) unsigned NOT NULL default '0',
  `roi_regen` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`channel_no`,`level`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_hardware_ting`
--

DROP TABLE IF EXISTS `log_hardware_ting`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_hardware_ting` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `category1` tinyint(3) unsigned NOT NULL default '0',
  `category2` int(11) NOT NULL default '0',
  `category3` int(11) NOT NULL default '0',
  `total` int(10) unsigned NOT NULL default '0',
  `ting` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`category1`,`category2`,`category3`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_item_develop`
--

DROP TABLE IF EXISTS `log_item_develop`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_item_develop` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `it_no` mediumint(8) unsigned NOT NULL default '0',
  `server_id` tinyint(4) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `charac_name` varchar(20) NOT NULL default '',
  `develop` smallint(6) NOT NULL default '0',
  `sucess_flag` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`it_no`),
  KEY `idx_it_no` USING BTREE (`it_no`),
  KEY `idx_server_id` USING BTREE (`server_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_item_drop`
--

DROP TABLE IF EXISTS `log_item_drop`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_item_drop` (
  `occ_date` date NOT NULL default '0000-00-00',
  `channel_no` smallint(5) unsigned NOT NULL default '0',
  `drop_type` tinyint(3) unsigned NOT NULL default '0',
  `stackable_common` int(10) unsigned NOT NULL default '0',
  `stackable_uncommon` int(10) unsigned NOT NULL default '0',
  `stackable_rare` int(10) unsigned NOT NULL default '0',
  `stackable_unique` int(10) unsigned NOT NULL default '0',
  `stackable_epic` int(10) unsigned NOT NULL default '0',
  `equip_common` int(10) unsigned NOT NULL default '0',
  `equip_uncommon` int(10) unsigned NOT NULL default '0',
  `equip_rare` int(10) unsigned NOT NULL default '0',
  `equip_unique` int(10) unsigned NOT NULL default '0',
  `equip_epic` int(10) unsigned NOT NULL default '0',
  `recipe_common` int(10) unsigned NOT NULL default '0',
  `recipe_uncommon` int(10) unsigned NOT NULL default '0',
  `recipe_rare` int(10) unsigned NOT NULL default '0',
  `recipe_unique` int(10) unsigned NOT NULL default '0',
  `recipe_epic` int(10) unsigned NOT NULL default '0',
  `artifact_common` int(10) unsigned NOT NULL default '0',
  `artifact_uncommon` int(10) unsigned NOT NULL default '0',
  `artifact_rare` int(10) unsigned NOT NULL default '0',
  `artifact_unique` int(10) unsigned NOT NULL default '0',
  `artifact_epic` int(10) unsigned NOT NULL default '0',
  `equip_chronicle` int(10) unsigned NOT NULL default '0',
  `recipe_chronicle` int(10) unsigned NOT NULL default '0',
  `artifact_chronicle` int(10) unsigned NOT NULL default '0',
  `Stackable_chronicle` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`channel_no`,`drop_type`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_item_stat`
--

DROP TABLE IF EXISTS `log_item_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_item_stat` (
  `cur_date` date NOT NULL default '0000-00-00',
  `server_info` tinyint(4) NOT NULL default '0',
  `process_id` tinyint(4) NOT NULL default '0',
  `item_id` int(11) NOT NULL default '0',
  `item_cnt` bigint(20) unsigned NOT NULL default '0',
  PRIMARY KEY  (`cur_date`,`server_info`,`process_id`,`item_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_item_stat_multiproc`
--

DROP TABLE IF EXISTS `log_item_stat_multiproc`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_item_stat_multiproc` (
  `cur_date` date NOT NULL default '0000-00-00',
  `server_info` tinyint(4) NOT NULL default '0',
  `process_id` tinyint(4) NOT NULL default '0',
  `item_id` int(11) NOT NULL default '0',
  `item_cnt` bigint(20) unsigned NOT NULL default '0',
  PRIMARY KEY  (`cur_date`,`server_info`,`process_id`,`item_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_item_stat_multiproc_fix`
--

DROP TABLE IF EXISTS `log_item_stat_multiproc_fix`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_item_stat_multiproc_fix` (
  `cur_date` date NOT NULL default '0000-00-00',
  `server_info` tinyint(4) NOT NULL default '0',
  `process_id` tinyint(4) NOT NULL default '0',
  `item_id` int(11) NOT NULL default '0',
  `item_cnt` bigint(20) unsigned NOT NULL default '0',
  PRIMARY KEY  (`cur_date`,`server_info`,`process_id`,`item_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_item_stat_single`
--

DROP TABLE IF EXISTS `log_item_stat_single`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_item_stat_single` (
  `cur_date` date NOT NULL default '0000-00-00',
  `server_info` tinyint(4) NOT NULL default '0',
  `process_id` tinyint(4) NOT NULL default '0',
  `item_id` int(11) NOT NULL default '0',
  `item_cnt` bigint(20) unsigned NOT NULL default '0',
  PRIMARY KEY  (`cur_date`,`server_info`,`process_id`,`item_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_item_upgrade`
--

DROP TABLE IF EXISTS `log_item_upgrade`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_item_upgrade` (
  `cur_date` date NOT NULL default '0000-00-00',
  `server_info` tinyint(4) NOT NULL default '0',
  `process_id` tinyint(4) NOT NULL default '0',
  `item_id` int(11) NOT NULL default '0',
  `item_cnt` bigint(20) unsigned NOT NULL default '0',
  `upgrade` int(11) NOT NULL default '0',
  `amplify_item_cnt` bigint(20) unsigned NOT NULL default '0',
  `rarity` int(11) NOT NULL default '0',
  `level` int(11) NOT NULL default '0',
  PRIMARY KEY  (`cur_date`,`server_info`,`process_id`,`item_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_item_upgrade_multiproc`
--

DROP TABLE IF EXISTS `log_item_upgrade_multiproc`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_item_upgrade_multiproc` (
  `cur_date` date NOT NULL default '0000-00-00',
  `server_info` tinyint(4) NOT NULL default '0',
  `process_id` tinyint(4) NOT NULL default '0',
  `item_id` int(11) NOT NULL default '0',
  `item_cnt` bigint(20) unsigned NOT NULL default '0',
  `upgrade` int(11) NOT NULL default '0',
  `amplify_item_cnt` bigint(20) unsigned NOT NULL default '0',
  `rarity` int(11) NOT NULL default '0',
  `level` int(11) NOT NULL default '0',
  PRIMARY KEY  (`cur_date`,`server_info`,`process_id`,`item_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_job_gold`
--

DROP TABLE IF EXISTS `log_job_gold`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_job_gold` (
  `cur_date` date NOT NULL default '0000-00-00',
  `server_info` tinyint(4) NOT NULL default '0',
  `job` tinyint(4) NOT NULL default '0',
  `gold_average` int(10) unsigned NOT NULL default '0',
  `gold_max` int(10) unsigned NOT NULL default '0',
  `gold_sum` bigint(20) unsigned NOT NULL default '0',
  PRIMARY KEY  (`cur_date`,`server_info`,`job`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_level_dungeon_play_count`
--

DROP TABLE IF EXISTS `log_level_dungeon_play_count`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_level_dungeon_play_count` (
  `level` smallint(6) NOT NULL default '0',
  `total_charac_count` int(10) unsigned NOT NULL default '0',
  `total_dungeon_play_count` bigint(20) unsigned NOT NULL default '0',
  PRIMARY KEY  (`level`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_login_logout`
--

DROP TABLE IF EXISTS `log_login_logout`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_login_logout` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `channel_no` tinyint(3) unsigned NOT NULL default '0',
  `event_type` tinyint(3) unsigned NOT NULL default '0',
  `count` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`channel_no`,`event_type`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_lucky_ticket_stat`
--

DROP TABLE IF EXISTS `log_lucky_ticket_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_lucky_ticket_stat` (
  `idx` int(10) unsigned NOT NULL auto_increment,
  `occ_time` int(10) unsigned NOT NULL default '0',
  `m_id` int(10) unsigned NOT NULL default '0',
  `charac_no` int(10) unsigned NOT NULL default '0',
  `type_flag` tinyint(4) NOT NULL default '0',
  `ticket_flag` tinyint(4) NOT NULL default '0',
  `item_idx` int(10) unsigned NOT NULL default '0',
  `upgrade` tinyint(3) unsigned NOT NULL default '0',
  `succ_flag` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`idx`),
  KEY `idx_occ_time` USING BTREE (`occ_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_mercenary`
--

DROP TABLE IF EXISTS `log_mercenary`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_mercenary` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `charac_no` int(10) unsigned NOT NULL default '0',
  `start_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `finish_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `type` tinyint(3) unsigned NOT NULL default '0',
  `period` tinyint(3) unsigned NOT NULL default '0',
  `area` tinyint(3) unsigned NOT NULL default '0',
  `gold` int(10) unsigned NOT NULL default '0',
  `item_id` int(10) unsigned NOT NULL default '0',
  `rarity` int(10) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_monstertower_layer_stat`
--

DROP TABLE IF EXISTS `log_monstertower_layer_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_monstertower_layer_stat` (
  `occ_date` date NOT NULL default '0000-00-00',
  `key_id` int(11) unsigned NOT NULL default '0',
  `layer` smallint(5) unsigned NOT NULL default '0',
  `monster_diff` tinyint(4) unsigned NOT NULL default '0',
  `charac_job` tinyint(4) unsigned NOT NULL default '0',
  `charac_grow` tinyint(4) unsigned NOT NULL default '0',
  `level` tinyint(4) unsigned NOT NULL default '0',
  `play_cnt` int(11) unsigned NOT NULL default '0',
  `success_cnt` int(11) unsigned NOT NULL default '0',
  `play_time` bigint(20) unsigned NOT NULL default '0',
  `uniq_cnt` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`key_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_nat_daily`
--

DROP TABLE IF EXISTS `log_nat_daily`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_nat_daily` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `nat_type` tinyint(4) NOT NULL default '0',
  `nat_type_other` tinyint(4) NOT NULL default '0',
  `success_cnt` int(10) unsigned NOT NULL default '0',
  `fail_cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`nat_type`,`nat_type_other`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_nat_stat`
--

DROP TABLE IF EXISTS `log_nat_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_nat_stat` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `nat_type` tinyint(4) NOT NULL default '0',
  `nat_cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`nat_type`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
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
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_ontime_reward`
--

DROP TABLE IF EXISTS `log_ontime_reward`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_ontime_reward` (
  `time` datetime NOT NULL default '0000-00-00 00:00:00',
  `event_no` int(10) unsigned NOT NULL default '0',
  `reward_cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`time`,`event_no`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_otherserver_load_stat`
--

DROP TABLE IF EXISTS `log_otherserver_load_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_otherserver_load_stat` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `server_type` tinyint(3) unsigned NOT NULL default '0',
  `kind` tinyint(4) NOT NULL default '0',
  `q_cnt` int(10) NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`server_type`,`kind`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_packet_dispatcher_error_line`
--

DROP TABLE IF EXISTS `log_packet_dispatcher_error_line`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_packet_dispatcher_error_line` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `channel_no` tinyint(4) NOT NULL default '0',
  `error_line` int(10) unsigned NOT NULL default '0',
  `cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`channel_no`,`error_line`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_party_type`
--

DROP TABLE IF EXISTS `log_party_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_party_type` (
  `channel_no` int(11) NOT NULL default '0',
  `occ_date` date NOT NULL default '0000-00-00',
  `guild_count` int(11) NOT NULL default '0',
  `member_count` int(11) NOT NULL default '0',
  `helped_count` int(11) NOT NULL default '0',
  `normal_count` int(11) NOT NULL default '0',
  `member_helped_count` int(11) NOT NULL default '0',
  PRIMARY KEY  (`channel_no`,`occ_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_playtime`
--

DROP TABLE IF EXISTS `log_playtime`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_playtime` (
  `m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `play_time` int(11) NOT NULL default '0',
  `lobby_intime` datetime NOT NULL default '0000-00-00 00:00:00',
  `lobby_outtime` datetime NOT NULL default '0000-00-00 00:00:00',
  KEY `idx_lobby_outtime` USING BTREE (`lobby_outtime`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_police_chatting`
--

DROP TABLE IF EXISTS `log_police_chatting`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_police_chatting` (
  `no` int(10) unsigned NOT NULL auto_increment,
  `charac_no` int(10) unsigned NOT NULL default '0',
  `msg_type` tinyint(4) NOT NULL default '0',
  `occ_time` int(10) unsigned NOT NULL default '0',
  `message` varchar(255) NOT NULL default '0',
  PRIMARY KEY  (`no`),
  KEY `idx_charac_no` USING BTREE (`charac_no`),
  KEY `idx_occ_time` USING BTREE (`occ_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_pvp_map`
--

DROP TABLE IF EXISTS `log_pvp_map`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_pvp_map` (
  `channel_no` int(11) NOT NULL default '0',
  `occ_date` date NOT NULL default '0000-00-00',
  `map_id` int(11) NOT NULL default '0',
  `play_count` int(11) NOT NULL default '0',
  PRIMARY KEY  (`channel_no`,`map_id`,`occ_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_pvp_player_rate`
--

DROP TABLE IF EXISTS `log_pvp_player_rate`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_pvp_player_rate` (
  `channel_no` int(11) NOT NULL default '0',
  `occ_date` date NOT NULL default '0000-00-00',
  `single_one_to_one` int(11) NOT NULL default '0',
  `single_one_to_one_other` int(11) NOT NULL default '0',
  `team_one_to_one` int(11) NOT NULL default '0',
  `team_one_to_one_other` int(11) NOT NULL default '0',
  `relay_one_to_one` int(11) NOT NULL default '0',
  `relay_one_to_one_other` int(11) NOT NULL default '0',
  PRIMARY KEY  (`channel_no`,`occ_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
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
-- Table structure for table `log_query_stat_del`
--

DROP TABLE IF EXISTS `log_query_stat_del`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_query_stat_del` (
  `sdate` date NOT NULL default '0000-00-00',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `q_id` smallint(5) unsigned NOT NULL default '0',
  `gc_no` int(10) unsigned NOT NULL default '0',
  `total` int(10) unsigned NOT NULL default '0',
  `response_time` int(10) unsigned default NULL,
  PRIMARY KEY  (`sdate`,`occ_time`,`q_id`),
  KEY `idx_q_id` USING BTREE (`q_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_random_option`
--

DROP TABLE IF EXISTS `log_random_option`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_random_option` (
  `level` tinyint(3) unsigned NOT NULL default '0',
  `occ_date` date NOT NULL default '0000-00-00',
  `unseal_cnt` int(10) unsigned NOT NULL default '0',
  `unseal_gold` bigint(20) unsigned NOT NULL default '0',
  `regen_cnt` int(10) unsigned NOT NULL default '0',
  `regen_gold` bigint(20) unsigned NOT NULL default '0',
  `change_cnt` int(10) unsigned NOT NULL default '0',
  `change_gold` bigint(20) unsigned NOT NULL default '0',
  `pick_cnt` int(10) unsigned NOT NULL default '0',
  `reset_cnt` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`level`,`occ_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_response_time_201604`
--

DROP TABLE IF EXISTS `log_response_time_201604`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_response_time_201604` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `channel_no` int(11) NOT NULL default '0',
  `packet_id` int(10) unsigned NOT NULL default '0',
  `packet_count` int(10) unsigned NOT NULL default '0',
  `total_response_time` bigint(20) unsigned NOT NULL default '0',
  `avg_response_time` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_time`,`channel_no`,`packet_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_secret_shop`
--

DROP TABLE IF EXISTS `log_secret_shop`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_secret_shop` (
  `npc_idx` tinyint(4) NOT NULL default '0',
  `occ_date` date NOT NULL default '0000-00-00',
  `dungeon_idx` int(11) NOT NULL default '0',
  `show_count` int(10) unsigned NOT NULL default '0',
  `show_charac_count` int(10) unsigned NOT NULL default '0',
  `buy_count` int(10) unsigned NOT NULL default '0',
  `price` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`npc_idx`,`occ_date`,`dungeon_idx`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_server_optimize_stat`
--

DROP TABLE IF EXISTS `log_server_optimize_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_server_optimize_stat` (
  `channel_no` smallint(5) unsigned NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `total_count` int(10) unsigned NOT NULL default '0',
  `sp_less_5` int(10) unsigned NOT NULL default '0',
  `sp_less_10` int(10) unsigned NOT NULL default '0',
  `sp_less_30` int(10) unsigned NOT NULL default '0',
  `sp_less_50` int(10) unsigned NOT NULL default '0',
  `sp_less_100` int(10) unsigned NOT NULL default '0',
  `sp_more_100` int(10) unsigned NOT NULL default '0',
  `result` float NOT NULL default '0',
  PRIMARY KEY  (`channel_no`,`occ_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_tower_despair_uv_stat`
--

DROP TABLE IF EXISTS `log_tower_despair_uv_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_tower_despair_uv_stat` (
  `occ_date` date NOT NULL default '0000-00-00',
  `server_id` tinyint(3) NOT NULL default '0',
  `uv` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`server_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_upgrade_pot`
--

DROP TABLE IF EXISTS `log_upgrade_pot`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_upgrade_pot` (
  `occ_date` date NOT NULL default '0000-00-00',
  `id_2670203` int(10) unsigned NOT NULL default '0',
  `id_2670204` int(10) unsigned NOT NULL default '0',
  `id_2670205` int(10) unsigned NOT NULL default '0',
  `id_2670206` int(10) unsigned NOT NULL default '0',
  `id_2670241` int(10) unsigned NOT NULL default '0',
  `id_2670242` int(10) unsigned NOT NULL default '0',
  `id_2670243` int(10) unsigned NOT NULL default '0',
  `id_2670244` int(10) unsigned NOT NULL default '0',
  `id_2670245` int(10) unsigned NOT NULL default '0',
  `id_2670246` int(10) unsigned NOT NULL default '0',
  `id_2670247` int(10) unsigned NOT NULL default '0',
  `id_2670248` int(10) unsigned NOT NULL default '0',
  `id_2670249` int(10) unsigned NOT NULL default '0',
  `id_2670250` int(10) unsigned NOT NULL default '0',
  `id_2670251` int(10) unsigned NOT NULL default '0',
  `id_2670252` int(10) unsigned NOT NULL default '0',
  `id_2670253` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_value_stat`
--

DROP TABLE IF EXISTS `log_value_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_value_stat` (
  `channel_no` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `level` int(11) NOT NULL default '0',
  `uv` int(10) unsigned NOT NULL default '0',
  `drop_gold` int(10) unsigned NOT NULL default '0',
  `drop_item` int(10) unsigned NOT NULL default '0',
  `result_card_gold` int(10) unsigned NOT NULL default '0',
  `result_card_item` int(10) unsigned NOT NULL default '0',
  `store_item_buy` int(10) unsigned NOT NULL default '0',
  `jar_item` int(10) unsigned NOT NULL default '0',
  `disjoint_create` int(10) unsigned NOT NULL default '0',
  `upgrade_faild_forced_disjoint` int(10) unsigned NOT NULL default '0',
  `quest_reward` int(10) unsigned NOT NULL default '0',
  `consume_store_item_buy` int(10) unsigned NOT NULL default '0',
  `consume_upgrade_attempt` int(10) unsigned NOT NULL default '0',
  `consume_upgrade_faild` int(10) unsigned NOT NULL default '0',
  `consume_stamina_recovery` int(10) unsigned NOT NULL default '0',
  `consume_quest_consume` int(10) unsigned NOT NULL default '0',
  `consume_auction_commision` int(10) unsigned NOT NULL default '0',
  `consume_item_disjoint` int(10) unsigned NOT NULL default '0',
  `consume_item_repair` int(10) unsigned NOT NULL default '0',
  `consume_item_use` int(10) unsigned NOT NULL default '0',
  `consume_item_drop` int(10) unsigned NOT NULL default '0',
  `consume_gold_drop` int(10) unsigned NOT NULL default '0',
  `gold_card_item` int(10) unsigned NOT NULL default '0',
  `consume_gold_card_price` int(10) unsigned NOT NULL default '0',
  `deathtower_card_gold` int(10) unsigned NOT NULL default '0',
  `deathtower_card_item` int(10) unsigned NOT NULL default '0',
  `consume_qp_init` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`channel_no`,`occ_time`,`level`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_war_stat`
--

DROP TABLE IF EXISTS `log_war_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_war_stat` (
  `occ_date` date NOT NULL default '0000-00-00',
  `channel_no` int(11) NOT NULL default '0',
  `war_zone_cnt` int(11) NOT NULL default '0',
  `peace_zone_cnt` int(11) NOT NULL default '0',
  `rare_item_cnt` int(11) NOT NULL default '0',
  `unique_item_cnt` int(11) NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`channel_no`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `login_account`
--

DROP TABLE IF EXISTS `login_account`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `login_account` (
  `m_id` int(10) unsigned NOT NULL default '0',
  `m_channel_no` int(11) NOT NULL default '0',
  `login_status` tinyint(1) NOT NULL default '0',
  `last_login_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `login_ip` varchar(15) NOT NULL default '',
  PRIMARY KEY  (`m_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `login_view_log`
--

DROP TABLE IF EXISTS `login_view_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `login_view_log` (
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  `m_id` int(11) NOT NULL default '0',
  `charac_m_id` int(11) NOT NULL default '0',
  `charac_no` int(11) NOT NULL default '0',
  `ip` varchar(15) NOT NULL default '',
  PRIMARY KEY  (`occ_time`,`m_id`),
  KEY `idx_charac_m_id` USING BTREE (`charac_m_id`),
  KEY `idx_charac_no` USING BTREE (`charac_no`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mage_grow_log`
--

DROP TABLE IF EXISTS `mage_grow_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mage_grow_log` (
  `grow_type1` int(11) NOT NULL default '0',
  `grow_type2` int(11) NOT NULL default '0',
  `grow_type3` int(11) NOT NULL default '0',
  `grow_type4` int(11) NOT NULL default '0',
  `grow_type5` int(11) NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_environment`
--

DROP TABLE IF EXISTS `member_environment`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_environment` (
  `os_type` int(11) NOT NULL default '0',
  `hard_wholesize` int(10) unsigned NOT NULL default '0',
  `hard_remainsize` int(10) unsigned NOT NULL default '0',
  `memory_wholesize` int(10) unsigned NOT NULL default '0',
  `memory_remainsize` int(10) unsigned NOT NULL default '0',
  `virtualmem_wholesize` int(10) unsigned NOT NULL default '0',
  `virtualmem_remainsize` int(10) unsigned NOT NULL default '0',
  `cpu_speed` int(11) NOT NULL default '0',
  `cpu_count` int(11) NOT NULL default '0',
  `cpu_vendor` varchar(20) NOT NULL default '',
  `graphic_name` varchar(128) NOT NULL default '',
  `graphic_memory` int(10) unsigned NOT NULL default '0',
  `average_fps` int(10) unsigned NOT NULL default '0',
  `reg_date` int(10) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `member_environment_entry`
--

DROP TABLE IF EXISTS `member_environment_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `member_environment_entry` (
  `m_id` int(11) NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`m_id`),
  KEY `idx_occ_time` USING BTREE (`occ_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pvp_status`
--

DROP TABLE IF EXISTS `pvp_status`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pvp_status` (
  `occ_date` date NOT NULL default '0000-00-00',
  `room_number` tinyint(3) unsigned NOT NULL default '0',
  `play_type` tinyint(3) unsigned NOT NULL default '0',
  `no_participants` tinyint(3) unsigned NOT NULL default '0',
  `occ_count` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`room_number`,`play_type`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `re_stamina_shusia`
--

DROP TABLE IF EXISTS `re_stamina_shusia`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `re_stamina_shusia` (
  `occ_date` date NOT NULL default '0000-00-00',
  `lev` tinyint(4) NOT NULL default '0',
  `occ_count` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`lev`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `reliable_member_log`
--

DROP TABLE IF EXISTS `reliable_member_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `reliable_member_log` (
  `occ_date` date NOT NULL default '0000-00-00',
  `m_id` int(11) NOT NULL default '0',
  `ip` varchar(15) NOT NULL default '',
  PRIMARY KEY  (`occ_date`,`m_id`,`ip`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `trade_gold_daily`
--

DROP TABLE IF EXISTS `trade_gold_daily`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `trade_gold_daily` (
  `m_id` int(11) NOT NULL default '0',
  `gold` int(10) unsigned NOT NULL default '0',
  `occ_time` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`m_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `trade_gold_rank`
--

DROP TABLE IF EXISTS `trade_gold_rank`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `trade_gold_rank` (
  `occ_date` date NOT NULL default '0000-00-00',
  `m_id` int(11) NOT NULL default '0',
  `gold` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`occ_date`,`m_id`),
  KEY `idx_gold` USING BTREE (`gold`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
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
