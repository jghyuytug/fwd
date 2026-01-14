/**
 * @file db_service.h
 * @brief Database Service Layer for Game Server
 *
 * Provides database access functions for the dispatcher system.
 * Wraps ORM layer calls for accounts, characters, and game data.
 *
 * @version 1.0
 * @date 2025-11-26
 */

#ifndef DB_SERVICE_H
#define DB_SERVICE_H

#include <stdint.h>
#include <stdbool.h>

/*==============================================================================
 * Configuration
 *============================================================================*/

#define DB_MAX_ACCOUNT_NAME     256
#define DB_MAX_CHARAC_NAME      20
#define DB_MAX_CHARACTERS       16
#define DB_MAX_GUILD_NAME       32
#define DB_MAX_ITEMS_PER_SLOT   697

/*==============================================================================
 * Data Structures - Account
 *============================================================================*/

typedef struct {
    int         uid;
    char        account_name[DB_MAX_ACCOUNT_NAME];
    char        password[256];
    char        vip[8];
    int         billing;
} DBAccount;

/*==============================================================================
 * Data Structures - Character
 *============================================================================*/

typedef struct {
    int         charac_no;
    int         m_id;               /* Account UID */
    char        charac_name[DB_MAX_CHARAC_NAME];
    int8_t      village;
    int8_t      sex;
    int8_t      job;
    uint8_t     level;
    int         exp;
    uint8_t     grow_type;
    int8_t      expert_job;
    int         hp;
    int         max_hp;
    int         mp;
    int         max_mp;
    int         phy_attack;
    int         phy_defense;
    int         mag_attack;
    int         mag_defense;
    int         move_speed;
    int         attack_speed;
    int16_t     fatigue;
    int16_t     max_fatigue;
    int         guild_id;
    int8_t      guild_right;
    uint32_t    gold;
    char        create_time[32];
    char        last_play_time[32];
    char        delete_time[32];
    int8_t      deleted;
    uint8_t     slot;
} DBCharacter;

/*==============================================================================
 * Data Structures - Character Stats
 *============================================================================*/

typedef struct {
    int         charac_no;
    int         exp;
    int16_t     fatigue;
    int16_t     used_fatigue;
    int         dungeon_clear_point;
    uint32_t    dungeon_play_count;
    uint32_t    trade_gold_total;
    uint32_t    chaos_point;
    uint32_t    total_play_time;
} DBCharacterStat;

/*==============================================================================
 * Data Structures - Inventory Item
 *============================================================================*/

typedef struct {
    int         ui_id;          /* Unique item ID */
    int         charac_no;
    int         slot;
    int         it_id;          /* Item template ID */
    int         expire_date;
    int8_t      upgrade;
    int8_t      seperate;
    int8_t      ob_type;
    int         lock_key;
    uint16_t    stat_id;
    int8_t      seal_flag;
} DBInventoryItem;

/*==============================================================================
 * Data Structures - Guild
 *============================================================================*/

typedef struct {
    int         guild_id;
    char        guild_name[DB_MAX_GUILD_NAME];
    int         lev;
    int         exp;
    int         master_id;
    char        master_name[DB_MAX_CHARAC_NAME];
    int         member_count;
    int         member_max_count;
    char        create_time[32];
} DBGuild;

/*==============================================================================
 * Database Service API - Initialization
 *============================================================================*/

/**
 * Initialize the database service
 * @param config_file Path to database configuration file
 * @return 0 on success, -1 on failure
 */
int DBService_Initialize(const char* config_file);

/**
 * Cleanup database service and close connections
 */
void DBService_Cleanup(void);

/**
 * Check if database service is initialized and connected
 * @return true if ready, false otherwise
 */
bool DBService_IsReady(void);

/*==============================================================================
 * Database Service API - Account Operations
 *============================================================================*/

/**
 * Verify account credentials
 * @param account_name Account name
 * @param password Password (plaintext or hashed)
 * @param account Output: Account data if successful
 * @return Account UID on success, -1 on failure
 */
int DBService_VerifyAccount(const char* account_name, const char* password, DBAccount* account);

/**
 * Get account by UID
 * @param uid Account UID
 * @param account Output: Account data
 * @return 0 on success, -1 on failure
 */
int DBService_GetAccount(int uid, DBAccount* account);

/**
 * Check if account exists
 * @param account_name Account name
 * @return true if exists, false otherwise
 */
bool DBService_AccountExists(const char* account_name);

/*==============================================================================
 * Database Service API - Character Operations
 *============================================================================*/

/**
 * Get all characters for an account
 * @param account_uid Account UID
 * @param characters Output array for character data
 * @param max_count Maximum characters to retrieve
 * @return Number of characters retrieved, -1 on failure
 */
int DBService_GetCharacters(int account_uid, DBCharacter* characters, int max_count);

/**
 * Get character by ID
 * @param charac_no Character number
 * @param character Output: Character data
 * @return 0 on success, -1 on failure
 */
int DBService_GetCharacter(int charac_no, DBCharacter* character);

/**
 * Create new character
 * @param account_uid Account UID
 * @param character Character data (charac_no will be set on success)
 * @return New character ID on success, -1 on failure
 */
int DBService_CreateCharacter(int account_uid, DBCharacter* character);

/**
 * Delete character (mark as deleted)
 * @param charac_no Character number
 * @return 0 on success, -1 on failure
 */
int DBService_DeleteCharacter(int charac_no);

/**
 * Update character stats after gameplay
 * @param charac_no Character number
 * @param stat Character stats
 * @return 0 on success, -1 on failure
 */
int DBService_UpdateCharacterStats(int charac_no, const DBCharacterStat* stat);

/**
 * Update character level and exp
 * @param charac_no Character number
 * @param level New level
 * @param exp New experience
 * @return 0 on success, -1 on failure
 */
int DBService_UpdateCharacterLevel(int charac_no, uint8_t level, int exp);

/**
 * Update character location
 * @param charac_no Character number
 * @param village Village ID
 * @return 0 on success, -1 on failure
 */
int DBService_UpdateCharacterLocation(int charac_no, int8_t village);

/**
 * Check if character name exists
 * @param charac_name Character name
 * @return true if exists, false otherwise
 */
bool DBService_CharacterNameExists(const char* charac_name);

/**
 * Get character count for account
 * @param account_uid Account UID
 * @return Number of characters, -1 on error
 */
int DBService_GetCharacterCount(int account_uid);

/*==============================================================================
 * Database Service API - Inventory Operations
 *============================================================================*/

/**
 * Get character inventory items
 * @param charac_no Character number
 * @param items Output array for items
 * @param max_count Maximum items to retrieve
 * @return Number of items retrieved, -1 on failure
 */
int DBService_GetInventory(int charac_no, DBInventoryItem* items, int max_count);

/**
 * Add item to inventory
 * @param charac_no Character number
 * @param item Item data
 * @return New item ui_id on success, -1 on failure
 */
int DBService_AddItem(int charac_no, DBInventoryItem* item);

/**
 * Remove item from inventory
 * @param ui_id Item unique ID
 * @return 0 on success, -1 on failure
 */
int DBService_RemoveItem(int ui_id);

/**
 * Update item slot
 * @param ui_id Item unique ID
 * @param new_slot New slot number
 * @return 0 on success, -1 on failure
 */
int DBService_UpdateItemSlot(int ui_id, int new_slot);

/**
 * Update item upgrade level
 * @param ui_id Item unique ID
 * @param upgrade New upgrade level
 * @return 0 on success, -1 on failure
 */
int DBService_UpdateItemUpgrade(int ui_id, int8_t upgrade);

/*==============================================================================
 * Database Service API - Gold Operations
 *============================================================================*/

/**
 * Get character gold
 * @param charac_no Character number
 * @return Gold amount, -1 on error
 */
int64_t DBService_GetGold(int charac_no);

/**
 * Update character gold
 * @param charac_no Character number
 * @param gold New gold amount
 * @return 0 on success, -1 on failure
 */
int DBService_SetGold(int charac_no, uint32_t gold);

/**
 * Add gold to character
 * @param charac_no Character number
 * @param amount Amount to add
 * @return 0 on success, -1 on failure
 */
int DBService_AddGold(int charac_no, uint32_t amount);

/**
 * Deduct gold from character
 * @param charac_no Character number
 * @param amount Amount to deduct
 * @return 0 on success, -1 on failure (including insufficient funds)
 */
int DBService_DeductGold(int charac_no, uint32_t amount);

/*==============================================================================
 * Database Service API - Guild Operations
 *============================================================================*/

/**
 * Get guild information
 * @param guild_id Guild ID
 * @param guild Output: Guild data
 * @return 0 on success, -1 on failure
 */
int DBService_GetGuild(int guild_id, DBGuild* guild);

/**
 * Create new guild
 * @param guild Guild data
 * @return New guild ID on success, -1 on failure
 */
int DBService_CreateGuild(DBGuild* guild);

/**
 * Add member to guild
 * @param guild_id Guild ID
 * @param charac_no Character number
 * @param guild_right Guild member right/rank
 * @return 0 on success, -1 on failure
 */
int DBService_AddGuildMember(int guild_id, int charac_no, int8_t guild_right);

/**
 * Remove member from guild
 * @param guild_id Guild ID
 * @param charac_no Character number
 * @return 0 on success, -1 on failure
 */
int DBService_RemoveGuildMember(int guild_id, int charac_no);

/**
 * Get guild members
 * @param guild_id Guild ID
 * @param characters Output array for member data
 * @param max_count Maximum members to retrieve
 * @return Number of members, -1 on failure
 */
int DBService_GetGuildMembers(int guild_id, DBCharacter* characters, int max_count);

/*==============================================================================
 * Database Service API - Dungeon Operations
 *============================================================================*/

/**
 * Update dungeon clear record
 * @param charac_no Character number
 * @param dungeon_id Dungeon ID
 * @param clear_time Clear time in seconds
 * @return 0 on success, -1 on failure
 */
int DBService_UpdateDungeonClear(int charac_no, int dungeon_id, int clear_time);

/**
 * Increment dungeon play count
 * @param charac_no Character number
 * @return 0 on success, -1 on failure
 */
int DBService_IncrementDungeonPlayCount(int charac_no);

/**
 * Update fatigue after dungeon
 * @param charac_no Character number
 * @param fatigue_used Fatigue consumed
 * @return 0 on success, -1 on failure
 */
int DBService_UseFatigue(int charac_no, int16_t fatigue_used);

/*==============================================================================
 * Database Service API - Skill Operations
 *============================================================================*/

/**
 * Get character skill levels
 * @param charac_no Character number
 * @param skill_ids Output array for skill IDs
 * @param skill_levels Output array for skill levels
 * @param max_count Maximum skills to retrieve
 * @return Number of skills, -1 on failure
 */
int DBService_GetSkills(int charac_no, int* skill_ids, int* skill_levels, int max_count);

/**
 * Update skill level
 * @param charac_no Character number
 * @param skill_id Skill ID
 * @param level New level
 * @return 0 on success, -1 on failure
 */
int DBService_UpdateSkill(int charac_no, int skill_id, int level);

/*==============================================================================
 * Data Structures - Trade (Phase 4)
 *============================================================================*/

typedef struct {
    int         trade_id;
    int         seller_id;          /* Seller character ID */
    int         buyer_id;           /* Buyer character ID (0 if pending) */
    int         item_id;            /* Item template ID */
    int         item_ui_id;         /* Item unique ID */
    uint32_t    price;              /* Gold price */
    int8_t      status;             /* 0=pending, 1=completed, 2=cancelled */
    char        create_time[32];
} DBTrade;

/*==============================================================================
 * Data Structures - Mail (Phase 4)
 *============================================================================*/

#define DB_MAX_MAIL_TITLE       64
#define DB_MAX_MAIL_CONTENT     512

typedef struct {
    int         mail_id;
    int         sender_id;          /* Sender character ID (0 for system) */
    int         receiver_id;        /* Receiver character ID */
    char        sender_name[DB_MAX_CHARAC_NAME];
    char        title[DB_MAX_MAIL_TITLE];
    char        content[DB_MAX_MAIL_CONTENT];
    int         attached_item_id;   /* Attached item ui_id (0 if none) */
    uint32_t    attached_gold;      /* Attached gold amount */
    int8_t      is_read;            /* 0=unread, 1=read */
    int8_t      item_extracted;     /* 0=not extracted, 1=extracted */
    char        send_time[32];
    char        expire_time[32];
} DBMail;

/*==============================================================================
 * Data Structures - Achievement (Phase 4)
 *============================================================================*/

typedef struct {
    int         achievement_id;
    int         charac_no;
    int         progress;           /* Current progress */
    int         target;             /* Target value */
    int8_t      completed;          /* 0=in progress, 1=completed */
    int8_t      reward_claimed;     /* 0=not claimed, 1=claimed */
    char        complete_time[32];
} DBAchievement;

/*==============================================================================
 * Database Service API - Trade Operations (Phase 4)
 *============================================================================*/

/**
 * Create a trade listing
 * @param trade Trade data
 * @return New trade ID on success, -1 on failure
 */
int DBService_CreateTrade(DBTrade* trade);

/**
 * Get active trades for character
 * @param charac_no Character number
 * @param trades Output array
 * @param max_count Maximum trades to retrieve
 * @return Number of trades, -1 on failure
 */
int DBService_GetTrades(int charac_no, DBTrade* trades, int max_count);

/**
 * Complete a trade (buyer purchases)
 * @param trade_id Trade ID
 * @param buyer_id Buyer character ID
 * @return 0 on success, -1 on failure
 */
int DBService_CompleteTrade(int trade_id, int buyer_id);

/**
 * Cancel a trade
 * @param trade_id Trade ID
 * @return 0 on success, -1 on failure
 */
int DBService_CancelTrade(int trade_id);

/**
 * Start direct trade between two characters
 * @param charac1 First character ID
 * @param charac2 Second character ID
 * @return Trade session ID on success, -1 on failure
 */
int DBService_StartDirectTrade(int charac1, int charac2);

/**
 * Confirm direct trade
 * @param trade_session_id Trade session ID
 * @param charac_no Character confirming
 * @return 0 on success, -1 on failure
 */
int DBService_ConfirmDirectTrade(int trade_session_id, int charac_no);

/*==============================================================================
 * Database Service API - Mail Operations (Phase 4)
 *============================================================================*/

/**
 * Send mail
 * @param mail Mail data
 * @return New mail ID on success, -1 on failure
 */
int DBService_SendMail(DBMail* mail);

/**
 * Get mailbox for character
 * @param charac_no Character number
 * @param mails Output array
 * @param max_count Maximum mails to retrieve
 * @return Number of mails, -1 on failure
 */
int DBService_GetMailbox(int charac_no, DBMail* mails, int max_count);

/**
 * Read mail (mark as read)
 * @param mail_id Mail ID
 * @return 0 on success, -1 on failure
 */
int DBService_ReadMail(int mail_id);

/**
 * Extract mail attachment
 * @param mail_id Mail ID
 * @param charac_no Character extracting
 * @return 0 on success, -1 on failure
 */
int DBService_ExtractMailAttachment(int mail_id, int charac_no);

/**
 * Delete mail
 * @param mail_id Mail ID
 * @return 0 on success, -1 on failure
 */
int DBService_DeleteMail(int mail_id);

/**
 * Get unread mail count
 * @param charac_no Character number
 * @return Unread count, -1 on failure
 */
int DBService_GetUnreadMailCount(int charac_no);

/*==============================================================================
 * Database Service API - Enhancement Operations (Phase 4)
 *============================================================================*/

/**
 * Upgrade item (increase upgrade level)
 * @param ui_id Item unique ID
 * @param success_rate Success rate (0-100)
 * @param destroy_on_fail Whether item is destroyed on failure
 * @return New upgrade level on success, -1 on failure, -2 if destroyed
 */
int DBService_UpgradeItem(int ui_id, int success_rate, int destroy_on_fail);

/**
 * Enchant item (add magic property)
 * @param ui_id Item unique ID
 * @param enchant_type Enchant type
 * @param enchant_value Enchant value
 * @return 0 on success, -1 on failure
 */
int DBService_EnchantItem(int ui_id, int enchant_type, int enchant_value);

/**
 * Amplify item (add amplification)
 * @param ui_id Item unique ID
 * @param amplify_value Amplification value
 * @return 0 on success, -1 on failure
 */
int DBService_AmplifyItem(int ui_id, int amplify_value);

/**
 * Get item current upgrade info
 * @param ui_id Item unique ID
 * @param upgrade Output: current upgrade level
 * @param enchant_type Output: current enchant type
 * @param enchant_value Output: current enchant value
 * @return 0 on success, -1 on failure
 */
int DBService_GetItemUpgradeInfo(int ui_id, int8_t* upgrade, int* enchant_type, int* enchant_value);

/*==============================================================================
 * Database Service API - Achievement Operations (Phase 4)
 *============================================================================*/

/**
 * Get character achievements
 * @param charac_no Character number
 * @param achievements Output array
 * @param max_count Maximum achievements to retrieve
 * @return Number of achievements, -1 on failure
 */
int DBService_GetAchievements(int charac_no, DBAchievement* achievements, int max_count);

/**
 * Update achievement progress
 * @param charac_no Character number
 * @param achievement_id Achievement ID
 * @param progress New progress value
 * @return 0 on success, 1 if completed, -1 on failure
 */
int DBService_UpdateAchievementProgress(int charac_no, int achievement_id, int progress);

/**
 * Claim achievement reward
 * @param charac_no Character number
 * @param achievement_id Achievement ID
 * @return 0 on success, -1 on failure
 */
int DBService_ClaimAchievementReward(int charac_no, int achievement_id);

/**
 * Trigger achievement (increment by 1)
 * @param charac_no Character number
 * @param achievement_id Achievement ID
 * @return 0 on success, 1 if completed, -1 on failure
 */
int DBService_TriggerAchievement(int charac_no, int achievement_id);

/*==============================================================================
 * Database Service API - Utility
 *============================================================================*/

/**
 * Begin database transaction
 * @return 0 on success, -1 on failure
 */
int DBService_BeginTransaction(void);

/**
 * Commit database transaction
 * @return 0 on success, -1 on failure
 */
int DBService_CommitTransaction(void);

/**
 * Rollback database transaction
 * @return 0 on success, -1 on failure
 */
int DBService_RollbackTransaction(void);

/**
 * Get last error message
 * @return Error message string
 */
const char* DBService_GetLastError(void);

#endif /* DB_SERVICE_H */
