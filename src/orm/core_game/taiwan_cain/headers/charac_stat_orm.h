/**
 * @file charac_stat_orm.h
 * @brief Character Statistics ORM Module - Player Stats Management
 *
 * This module provides Object-Relational Mapping for the charac_stat table,
 * which stores player statistics, progress, and game play data.
 *
 * Database: taiwan_cain (DB_TYPE_CAIN = 2)
 * Table: charac_stat
 * Primary Key: charac_no
 *
 * Features:
 * - Character experience and level tracking
 * - Fatigue system management
 * - Dungeon progress and statistics
 * - Trade and economy tracking
 * - Chaos mode statistics
 * - PvP and Power War tracking
 * - Tutorial and flag management
 *
 * @version 1.0
 * @date 2025-01-13
 */

#ifndef CHARAC_STAT_ORM_H
#define CHARAC_STAT_ORM_H

#include "db_connection_manager.h"

/**
 * @struct CharacStat
 * @brief Character statistics data structure (53 fields)
 */
typedef struct {
    int charac_no;                          /**< Character number (Primary Key) */

    /* Basic Stats */
    signed char village;                    /**< Current village ID */
    int exp;                                /**< Experience points */
    unsigned char HP;                       /**< HP value */
    short fatigue;                          /**< Current fatigue */
    short used_fatigue;                     /**< Used fatigue */
    short premium_fatigue;                  /**< Premium fatigue */

    /* Dungeon & Play */
    int dungeon_clear_point;                /**< Dungeon clear points */
    char last_play_time[32];                /**< Last play time */
    char forbidden_to_play[2];              /**< Play forbidden flag */
    char forbidden_due_to[32];              /**< Forbidden until time */
    int tutorial_flag;                      /**< Tutorial completion flags */

    /* Economy */
    unsigned int trade_gold_total;          /**< Total trade gold */
    unsigned short trade_gold_total_billion;/**< Billions of trade gold */
    unsigned int trade_gold_daily;          /**< Daily trade gold */

    /* Dungeon Statistics */
    unsigned int dungeon_map_pass_cnt;      /**< Dungeon pass count */
    unsigned int dungeon_map_help_pass_cnt; /**< Dungeon help pass count */
    unsigned int help_abuse_point;          /**< Help abuse points */

    /* Chaos Mode */
    unsigned int chaos_point;               /**< Chaos points */
    unsigned int chaos_exp;                 /**< Chaos experience */
    unsigned int chaos_mode_count;          /**< Chaos mode count */
    unsigned int chaos_kill_count;          /**< Chaos kills */
    unsigned int chaos_die_count;           /**< Chaos deaths */
    char chaos_die_time[32];                /**< Last chaos death time */
    char chaos_kill_time[32];               /**< Last chaos kill time */

    /* Combat */
    unsigned int assault_count;             /**< Assault count */
    int luck_point;                         /**< Luck points */
    unsigned int dungeon_play_count;        /**< Total dungeon plays */

    /* Help System */
    int help_abuse_ratio;                   /**< Help abuse ratio */
    int help_abuse_exp;                     /**< Help abuse experience */

    /* Expert Job */
    int expert_job_exp;                     /**< Expert job experience */
    int fatigue_battery_charging;           /**< Fatigue battery charging */
    char escalade_tutorial_flag[33];        /**< Escalade tutorial flags */

    /* Power War */
    unsigned short power_war_point;         /**< Power war points */
    unsigned int power_war_assault_count;   /**< Power war assaults */
    unsigned int power_war_assault_victory_count; /**< Power war victories */
    unsigned int fatigue_grownup_buff;      /**< Fatigue grownup buff */

    /* Village & Settings */
    signed char village_prev;               /**< Previous village */
    char last_play_time_powerwar[32];       /**< Last power war play time */
    unsigned short emotion;                 /**< Emotion value */
    unsigned char add_slot_flag;            /**< Additional slot flag */
    unsigned char member_dungeon_flag;      /**< Member dungeon flag */
    signed char open_flag;                  /**< Open flag */
    unsigned char member_bonus_fatigue;     /**< Member bonus fatigue */
    char birthday_effect_time[32];          /**< Birthday effect time */
    unsigned char visible_flags;            /**< Visibility flags */

    /* Equipment & Slots */
    signed char add_equipslot_flag;         /**< Additional equip slot flag */
    signed char channel_equipslot_switch;   /**< Channel equip slot switch */
    signed char expand_equipslot_switch;    /**< Expand equip slot switch */
    signed char growth_power_reward;        /**< Growth power reward */

    /* Miscellaneous */
    char chaos_respon_time[32];             /**< Chaos respawn time */
    unsigned int last_play_dungeon_index;   /**< Last played dungeon index */
    unsigned int total_play_time;           /**< Total play time (seconds) */
} CharacStat;

/**
 * @brief Create a new character stat record
 *
 * @param manager Database connection manager
 * @param stat Character stat data to insert
 * @return 0 on success, -1 on failure
 */
int CharacStat_Create(DBConnectionManager* manager, CharacStat* stat);

/**
 * @brief Get character stat by character number
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @param stat Output buffer for stat data
 * @return 0 on success, -1 on failure
 */
int CharacStat_GetByCharacNo(DBConnectionManager* manager, int charac_no, CharacStat* stat);

/**
 * @brief Update existing character stat record
 *
 * @param manager Database connection manager
 * @param stat Updated stat data
 * @return 0 on success, -1 on failure
 */
int CharacStat_Update(DBConnectionManager* manager, CharacStat* stat);

/**
 * @brief Delete character stat record
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @return 0 on success, -1 on failure
 */
int CharacStat_Delete(DBConnectionManager* manager, int charac_no);

/**
 * @brief Check if stat record exists for character
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @return 1 if exists, 0 if not exists, -1 on error
 */
int CharacStat_Exists(DBConnectionManager* manager, int charac_no);

/**
 * @brief Update character experience
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @param exp New experience value
 * @return 0 on success, -1 on failure
 */
int CharacStat_UpdateExp(DBConnectionManager* manager, int charac_no, int exp);

/**
 * @brief Update fatigue values
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @param fatigue Current fatigue
 * @param used_fatigue Used fatigue
 * @param premium_fatigue Premium fatigue
 * @return 0 on success, -1 on failure
 */
int CharacStat_UpdateFatigue(DBConnectionManager* manager, int charac_no,
                              short fatigue, short used_fatigue, short premium_fatigue);

/**
 * @brief Update last play time
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @return 0 on success, -1 on failure
 */
int CharacStat_UpdateLastPlayTime(DBConnectionManager* manager, int charac_no);

/**
 * @brief Increment dungeon play count
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @return 0 on success, -1 on failure
 */
int CharacStat_IncrementDungeonPlayCount(DBConnectionManager* manager, int charac_no);

/**
 * @brief Update chaos mode statistics
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @param chaos_point Chaos points
 * @param chaos_exp Chaos experience
 * @param chaos_kill_count Chaos kills
 * @param chaos_die_count Chaos deaths
 * @return 0 on success, -1 on failure
 */
int CharacStat_UpdateChaosStats(DBConnectionManager* manager, int charac_no,
                                 unsigned int chaos_point, unsigned int chaos_exp,
                                 unsigned int chaos_kill_count, unsigned int chaos_die_count);

/**
 * @brief Get characters by experience range
 *
 * @param manager Database connection manager
 * @param min_exp Minimum experience
 * @param max_exp Maximum experience
 * @param results Output array for results
 * @param max_count Maximum number of results
 * @return Number of results found, or -1 on error
 */
int CharacStat_GetByExpRange(DBConnectionManager* manager, int min_exp, int max_exp,
                              CharacStat* results, int max_count);

/**
 * @brief Get total character count
 *
 * @param manager Database connection manager
 * @return Total count, or -1 on error
 */
int CharacStat_GetTotalCount(DBConnectionManager* manager);

#endif /* CHARAC_STAT_ORM_H */
