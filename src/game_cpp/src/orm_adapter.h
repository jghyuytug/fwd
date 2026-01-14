/**
 * @file orm_adapter.h
 * @brief ORM Adapter for df_game_db
 *
 * Bridges the ORM layer (C) with main_db.cpp (C++)
 * Provides simplified interface for common database operations
 */

#ifndef ORM_ADAPTER_H
#define ORM_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ========================================================================== */
/* Initialization                                                              */
/* ========================================================================== */

/**
 * @brief Initialize ORM adapter with database connections
 * @param config_file Path to database configuration file
 * @return 0 on success, -1 on error
 */
int ORM_Initialize(const char* config_file);

/**
 * @brief Cleanup ORM adapter and close connections
 */
void ORM_Cleanup(void);

/**
 * @brief Check if ORM is initialized
 * @return 1 if initialized, 0 otherwise
 */
int ORM_IsInitialized(void);

/* ========================================================================== */
/* Character Operations                                                        */
/* ========================================================================== */

typedef struct {
    int charac_no;
    int m_id;
    char charac_name[20];
    int8_t village;
    int8_t sex;
    int8_t job;
    uint8_t lev;
    int exp;
    uint8_t grow_type;
    int8_t expert_job;
    int HP;
    int maxHP;
    int maxMP;
    int phy_attack;
    int phy_defense;
    int mag_attack;
    int mag_defense;
    int move_speed;
    int attack_speed;
    int cast_speed;
    int fatigue;
    int max_fatigue;
    int guild_id;
    int8_t guild_right;
} ORM_CharacInfo;

/**
 * @brief Get character info by character ID
 */
int ORM_CharacInfo_GetByNo(int charac_no, ORM_CharacInfo* info);

/**
 * @brief Get characters by member ID
 */
int ORM_CharacInfo_GetByMemberId(int m_id, ORM_CharacInfo* results, int max_count);

/**
 * @brief Update character level and exp
 */
int ORM_CharacInfo_UpdateLevelExp(int charac_no, uint8_t lev, int exp);

/**
 * @brief Update character HP/MP
 */
int ORM_CharacInfo_UpdateCombatStats(int charac_no, int HP, int maxHP, int maxMP);

/**
 * @brief Update character guild info
 */
int ORM_CharacInfo_UpdateGuild(int charac_no, int guild_id, int8_t guild_right);

/* ========================================================================== */
/* Account Operations                                                          */
/* ========================================================================== */

typedef struct {
    int uid;
    char m_id[32];
    char m_passwd[128];
    int m_isuse;
    int login_count;
} ORM_Account;

/**
 * @brief Get account by username
 */
int ORM_Account_GetByUsername(const char* username, ORM_Account* account);

/**
 * @brief Verify account password
 */
int ORM_Account_VerifyPassword(const char* username, const char* password);

/* ========================================================================== */
/* Inventory Operations                                                        */
/* ========================================================================== */

typedef struct {
    int ui_id;
    int charac_no;
    int it_id;
    int add_info;
    int slot;
    int count;
    int expire_time;
    int seal_cnt;
    int upgrade;
    int durability;
} ORM_UserItem;

/**
 * @brief Get inventory items for character
 */
int ORM_Inventory_GetItems(int charac_no, ORM_UserItem* items, int max_count);

/**
 * @brief Add item to inventory
 */
int ORM_Inventory_AddItem(int charac_no, int it_id, int count, int slot);

/**
 * @brief Remove item from inventory
 */
int ORM_Inventory_RemoveItem(int ui_id);

/**
 * @brief Update item count
 */
int ORM_Inventory_UpdateCount(int ui_id, int count);

/**
 * @brief Move item to new slot
 */
int ORM_Inventory_MoveItem(int ui_id, int new_slot);

/* ========================================================================== */
/* Guild Operations                                                            */
/* ========================================================================== */

typedef struct {
    int guild_id;
    char guild_name[32];
    int guild_lev;
    int guild_exp;
    int guild_master;
    int member_count;
    int max_member;
} ORM_GuildInfo;

/**
 * @brief Get guild info
 */
int ORM_Guild_GetInfo(int guild_id, ORM_GuildInfo* info);

/**
 * @brief Get guild members
 */
int ORM_Guild_GetMembers(int guild_id, int* member_ids, int max_count);

/* ========================================================================== */
/* Mail Operations                                                             */
/* ========================================================================== */

typedef struct {
    int postal_id;
    int send_charac_no;
    int receive_charac_no;
    char send_charac_name[20];
    char message[256];
    int gold;
    int item_id;
    int item_count;
    int read_flag;
    int receive_flag;
} ORM_Mail;

/**
 * @brief Get mails for character
 */
int ORM_Mail_GetList(int charac_no, ORM_Mail* mails, int max_count);

/**
 * @brief Send mail
 */
int ORM_Mail_Send(int from_charac, int to_charac, const char* message, int gold, int item_id, int item_count);

/**
 * @brief Mark mail as read
 */
int ORM_Mail_MarkRead(int postal_id);

/**
 * @brief Delete mail
 */
int ORM_Mail_Delete(int postal_id);

#ifdef __cplusplus
}
#endif

#endif /* ORM_ADAPTER_H */
