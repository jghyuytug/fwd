-- MySQL dump 10.11
--
-- Host: localhost    Database: taiwan_billing
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
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
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
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2025-10-20 11:32:26
