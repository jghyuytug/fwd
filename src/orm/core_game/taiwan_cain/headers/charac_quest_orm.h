/**
 * @file charac_quest_orm.h
 * @brief Character Quest ORM Module - Player Quest System Management
 *
 * This module provides Object-Relational Mapping for the charac_quest table,
 * which stores player quest progress and play event data.
 *
 * Database: taiwan_cain (DB_TYPE_CAIN = 2)
 * Table: charac_quest
 * Primary Key: charac_no
 *
 * Features:
 * - Quest progress tracking (10 quest categories)
 * - Play event management (10 play slots with triggers)
 * - Extended quest data storage
 * - Binary data handling for quest states
 *
 * @version 1.0
 * @date 2025-01-13
 */

#ifndef CHARAC_QUEST_ORM_H
#define CHARAC_QUEST_ORM_H

#include "db_connection_manager.h"

#define QUEST_DATA_SIZE 64
#define QUEST_CATEGORY_COUNT 10
#define PLAY_SLOT_COUNT 10

/**
 * @struct CharacQuest
 * @brief Character quest data structure
 *
 * This structure contains quest progress data including:
 * - Quest category binary data (64 bytes each)
 * - Play event slots with triggers
 * - Extended quest data
 */
typedef struct {
    int charac_no;                              /**< Character number (Primary Key) */

    /* Quest Category Data (10 categories, 64 bytes each) */
    unsigned char quest_10[QUEST_DATA_SIZE];    /**< Quest level 10 data */
    unsigned char quest_15[QUEST_DATA_SIZE];    /**< Quest level 15 data */
    unsigned char quest_20[QUEST_DATA_SIZE];    /**< Quest level 20 data */
    unsigned char quest_30[QUEST_DATA_SIZE];    /**< Quest level 30 data */
    unsigned char quest_40[QUEST_DATA_SIZE];    /**< Quest level 40 data */
    unsigned char quest_40_ext[QUEST_DATA_SIZE];/**< Quest level 40 extended data */
    unsigned char quest_50[QUEST_DATA_SIZE];    /**< Quest level 50 data */
    unsigned char quest_60[QUEST_DATA_SIZE];    /**< Quest level 60 data */
    unsigned char quest_70[QUEST_DATA_SIZE];    /**< Quest level 70 data */
    unsigned char quest_etc[QUEST_DATA_SIZE];   /**< Other quest data */

    /* Play Event Slots (10 slots) */
    unsigned short play_1;                      /**< Play slot 1 value */
    int play_1_trigger;                         /**< Play slot 1 trigger */
    unsigned short play_2;                      /**< Play slot 2 value */
    int play_2_trigger;                         /**< Play slot 2 trigger */
    unsigned short play_3;                      /**< Play slot 3 value */
    int play_3_trigger;                         /**< Play slot 3 trigger */
    unsigned short play_4;                      /**< Play slot 4 value */
    int play_4_trigger;                         /**< Play slot 4 trigger */
    unsigned short play_5;                      /**< Play slot 5 value */
    int play_5_trigger;                         /**< Play slot 5 trigger */
    unsigned short play_6;                      /**< Play slot 6 value */
    int play_6_trigger;                         /**< Play slot 6 trigger */
    unsigned short play_7;                      /**< Play slot 7 value */
    int play_7_trigger;                         /**< Play slot 7 trigger */
    unsigned short play_8;                      /**< Play slot 8 value */
    int play_8_trigger;                         /**< Play slot 8 trigger */
    unsigned short play_9;                      /**< Play slot 9 value */
    int play_9_trigger;                         /**< Play slot 9 trigger */
    unsigned short play_10;                     /**< Play slot 10 value */
    int play_10_trigger;                        /**< Play slot 10 trigger */

    /* Extended Quest Data */
    unsigned char quest_50_ext[QUEST_DATA_SIZE];    /**< Quest level 50 extended data */
    unsigned char quest_60_ext[QUEST_DATA_SIZE];    /**< Quest level 60 extended data */
    unsigned char quest_etc_ext[QUEST_DATA_SIZE];   /**< Other quest extended data */
    unsigned char quest_60_ext_2nd[QUEST_DATA_SIZE];/**< Quest level 60 2nd extended data */
} CharacQuest;

/**
 * @brief Create a new character quest record
 *
 * @param manager Database connection manager
 * @param quest Quest data to insert
 * @return 0 on success, -1 on failure
 */
int CharacQuest_Create(DBConnectionManager* manager, CharacQuest* quest);

/**
 * @brief Get character quest by character number
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @param quest Output buffer for quest data
 * @return 0 on success, -1 on failure
 */
int CharacQuest_GetByCharacNo(DBConnectionManager* manager, int charac_no, CharacQuest* quest);

/**
 * @brief Update existing character quest record
 *
 * @param manager Database connection manager
 * @param quest Updated quest data
 * @return 0 on success, -1 on failure
 */
int CharacQuest_Update(DBConnectionManager* manager, CharacQuest* quest);

/**
 * @brief Delete character quest record
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @return 0 on success, -1 on failure
 */
int CharacQuest_Delete(DBConnectionManager* manager, int charac_no);

/**
 * @brief Check if quest record exists for character
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @return 1 if exists, 0 if not exists, -1 on error
 */
int CharacQuest_Exists(DBConnectionManager* manager, int charac_no);

/**
 * @brief Update specific quest category data
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @param category Quest category name (e.g., "quest_10", "quest_50")
 * @param data Binary data (64 bytes)
 * @return 0 on success, -1 on failure
 */
int CharacQuest_UpdateCategory(DBConnectionManager* manager, int charac_no,
                                const char* category, const unsigned char* data);

/**
 * @brief Update specific play slot
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @param slot_num Play slot number (1-10)
 * @param value Play value
 * @param trigger Trigger value
 * @return 0 on success, -1 on failure
 */
int CharacQuest_UpdatePlaySlot(DBConnectionManager* manager, int charac_no,
                                int slot_num, unsigned short value, int trigger);

/**
 * @brief Get total quest records count
 *
 * @param manager Database connection manager
 * @return Total count, or -1 on error
 */
int CharacQuest_GetTotalCount(DBConnectionManager* manager);

/**
 * @brief Clear all quest data for character (reset to zero)
 *
 * @param manager Database connection manager
 * @param charac_no Character number
 * @return 0 on success, -1 on failure
 */
int CharacQuest_ClearAll(DBConnectionManager* manager, int charac_no);

/**
 * @brief Get characters with specific play value
 *
 * @param manager Database connection manager
 * @param slot_num Play slot number (1-10)
 * @param value Play value to search
 * @param results Output array for results
 * @param max_count Maximum number of results
 * @return Number of results found, or -1 on error
 */
int CharacQuest_GetByPlayValue(DBConnectionManager* manager, int slot_num,
                                unsigned short value, CharacQuest* results, int max_count);

#endif /* CHARAC_QUEST_ORM_H */
