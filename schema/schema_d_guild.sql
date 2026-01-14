-- MySQL dump 10.11
--
-- Host: localhost    Database: d_guild
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

-- Dump completed on 2025-10-20 11:32:26
