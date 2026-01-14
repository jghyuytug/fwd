/*
 * orm_adapter.h - ORM Layer Adapter for Game Server Integration
 *
 * This adapter provides a simplified interface for using the ORM layer
 * in the game server. It wraps the DBConnectionManager and ORM functions
 * to provide a more convenient API.
 *
 * Compatible with GCC 4.1.2 / CentOS 5.8 (VM-131)
 */

#ifndef ORM_ADAPTER_H
#define ORM_ADAPTER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
 * Database Type Constants
 *============================================================================*/

#define ORM_DB_TAIWAN   1   /* Account database (d_taiwan) */
#define ORM_DB_CAIN     2   /* Game data (taiwan_cain) */
#define ORM_DB_CAIN2    3   /* Extended data (taiwan_cain_2nd) - mapped to BILLING */
#define ORM_DB_LOGIN    4   /* Login logs (taiwan_login) */
#define ORM_DB_GUILD    5   /* Guild data (d_guild) */

/*==============================================================================
 * ORM Result Structure - Simplified character info
 *============================================================================*/

typedef struct {
    uint32_t charac_no;
    uint32_t m_id;
    char     charac_name[20];
    int8_t   village;
    int8_t   sex;
    int8_t   job;
    uint8_t  lev;
    uint32_t exp;
    uint8_t  grow_type;
    int8_t   expert_job;
    uint32_t HP;
    uint32_t maxHP;
    uint32_t maxMP;
    uint32_t phy_attack;
    uint32_t phy_defense;
    uint32_t mag_attack;
    uint32_t mag_defense;
    int32_t  move_speed;
    int32_t  attack_speed;
    int32_t  cast_speed;
    int32_t  fatigue;
    int32_t  max_fatigue;
    uint32_t guild_id;
    int8_t   guild_right;
    char     create_time[32];
    char     last_play_time[32];
    char     delete_time[32];
} ORM_CharacInfo;

typedef struct {
    uint32_t ui_id;
    uint32_t charac_no;
    int32_t  slot;
    uint32_t it_id;
    char     expire_date[20];
    int8_t   stat;
    int8_t   item_lock_key;
    int16_t  hidden_option;
} ORM_UserItem;

typedef struct {
    uint32_t guild_id;
    int8_t   server_id;
    char     guild_name[41];
    uint32_t master_id;
    uint32_t master_no;
    char     master_name[21];
    int32_t  lev;
    int32_t  member_count;
    uint32_t guild_point;
    uint32_t guild_exp;
} ORM_GuildInfo;

typedef struct {
    uint32_t charac_no;
    uint32_t pvp_point;
    uint32_t pvp_win;
    uint32_t pvp_lose;
} ORM_CharacStat;

typedef struct {
    uint32_t skill_id;
    uint32_t charac_no;
    int8_t   lev;
    int8_t   slot_index;
} ORM_Skill;

typedef struct {
    uint32_t postal_no;
    uint32_t charac_no;
    char     send_charac_name[20];
    uint32_t gold;
    uint32_t it_id;
    uint8_t  opened;
    uint8_t  deleted;
    char     reg_date[20];
} ORM_Postal;

/*==============================================================================
 * ORM Adapter Functions
 *============================================================================*/

/* Initialize ORM layer with config file */
int orm_init(const char* config_file);

/* Cleanup ORM layer */
void orm_cleanup(void);

/* Check if ORM is initialized */
int orm_is_initialized(void);

/*------------------------------------------------------------------------------
 * Account Functions (d_taiwan)
 *----------------------------------------------------------------------------*/

/* Get account by m_id (username) */
int orm_get_account_by_username(const char* username, uint32_t* uid, char* password_hash, int max_pwd_len);

/* Get account by UID */
int orm_get_account_by_uid(uint32_t uid, char* username, int max_name_len);

/*------------------------------------------------------------------------------
 * Character Functions (taiwan_cain)
 *----------------------------------------------------------------------------*/

/* Get characters for account */
int orm_get_characters(uint32_t m_id, ORM_CharacInfo* chars, int max_count);

/* Get character by charac_no */
int orm_get_character(uint32_t charac_no, ORM_CharacInfo* info);

/* Get character by name */
int orm_get_character_by_name(const char* charac_name, ORM_CharacInfo* info);

/* Create character */
int orm_create_character(const ORM_CharacInfo* info);

/* Update character level and exp */
int orm_update_character_level(uint32_t charac_no, uint8_t lev, uint32_t exp);

/* Update character HP/MP */
int orm_update_character_hp_mp(uint32_t charac_no, uint32_t HP, uint32_t maxHP, uint32_t maxMP);

/* Update character guild */
int orm_update_character_guild(uint32_t charac_no, uint32_t guild_id, int8_t guild_right);

/* Soft delete character */
int orm_delete_character(uint32_t charac_no);

/* Get character count for account */
int orm_get_character_count(uint32_t m_id);

/*------------------------------------------------------------------------------
 * Character Stats Functions (taiwan_cain)
 *----------------------------------------------------------------------------*/

/* Get character PVP stats */
int orm_get_charac_stat(uint32_t charac_no, ORM_CharacStat* stat);

/* Update character PVP stats */
int orm_update_charac_stat(uint32_t charac_no, uint32_t pvp_point, uint32_t pvp_win, uint32_t pvp_lose);

/*------------------------------------------------------------------------------
 * Inventory Functions (taiwan_cain_2nd)
 *----------------------------------------------------------------------------*/

/* Get inventory gold */
int orm_get_inventory_gold(uint32_t charac_no, uint32_t* gold);

/* Update inventory gold */
int orm_update_inventory_gold(uint32_t charac_no, uint32_t gold);

/* Get user items */
int orm_get_user_items(uint32_t charac_no, ORM_UserItem* items, int max_count);

/* Get item at slot */
int orm_get_item_at_slot(uint32_t charac_no, int slot, ORM_UserItem* item);

/* Add item */
int orm_add_item(uint32_t charac_no, uint32_t it_id, int slot, int8_t stat);

/* Delete item */
int orm_delete_item(uint32_t ui_id);

/* Move item to new slot */
int orm_move_item(uint32_t ui_id, int new_slot);

/* Find free slot */
int orm_find_free_slot(uint32_t charac_no, int start_slot);

/*------------------------------------------------------------------------------
 * Skill Functions (taiwan_cain_2nd)
 *----------------------------------------------------------------------------*/

/* Get skills for character */
int orm_get_skills(uint32_t charac_no, ORM_Skill* skills, int max_count);

/* Update skill level */
int orm_update_skill(uint32_t charac_no, uint32_t skill_id, int8_t lev);

/*------------------------------------------------------------------------------
 * Mail Functions (taiwan_cain_2nd)
 *----------------------------------------------------------------------------*/

/* Get mail list */
int orm_get_mail_list(uint32_t charac_no, ORM_Postal* mails, int max_count);

/* Send mail */
int orm_send_mail(uint32_t from_charac, uint32_t to_charac, const char* sender_name,
                  uint32_t gold, uint32_t it_id);

/* Delete mail */
int orm_delete_mail(uint32_t postal_no);

/* Mark mail as read */
int orm_mark_mail_read(uint32_t postal_no);

/*------------------------------------------------------------------------------
 * Guild Functions (d_guild)
 *----------------------------------------------------------------------------*/

/* Get guild info */
int orm_get_guild(uint32_t guild_id, ORM_GuildInfo* info);

/* Get guild by name */
int orm_get_guild_by_name(const char* guild_name, ORM_GuildInfo* info);

/* Create guild */
int orm_create_guild(const ORM_GuildInfo* info);

/* Update guild info */
int orm_update_guild(const ORM_GuildInfo* info);

/* Delete guild */
int orm_delete_guild(uint32_t guild_id);

/* Get guild member count */
int orm_get_guild_member_count(uint32_t guild_id);

/*------------------------------------------------------------------------------
 * Transaction Support
 *----------------------------------------------------------------------------*/

/* Begin transaction */
int orm_begin_transaction(int db_type);

/* Commit transaction */
int orm_commit_transaction(int db_type);

/* Rollback transaction */
int orm_rollback_transaction(int db_type);

#ifdef __cplusplus
}
#endif

#endif /* ORM_ADAPTER_H */
