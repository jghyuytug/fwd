/**
 * @file db_service.c
 * @brief Database Service Layer Implementation
 *
 * Provides database access for the game server dispatcher.
 * Can operate in two modes:
 * 1. Mock mode: Uses in-memory mock data for testing
 * 2. ORM mode: Uses real database via ORM layer
 *
 * @version 1.0
 * @date 2025-11-26
 */

#include "db_service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*==============================================================================
 * Configuration
 *============================================================================*/

/* Set to 1 to use mock data, 0 to use real ORM */
#define DB_USE_MOCK_DATA    1

#if !DB_USE_MOCK_DATA
#include "database/headers/db_connection_manager.h"
#include "orm/core_game/d_taiwan/headers/accounts_orm.h"
#include "orm/core_game/taiwan_cain/headers/charac_info_orm.h"
#include "orm/core_game/taiwan_cain/headers/charac_stat_orm.h"
#endif

/*==============================================================================
 * Module State
 *============================================================================*/

static struct {
    int initialized;
    char last_error[256];
#if !DB_USE_MOCK_DATA
    DBConnectionManager db_manager;
#endif
} g_db = {0};

/*==============================================================================
 * Mock Data Storage
 *============================================================================*/

#if DB_USE_MOCK_DATA

#define MOCK_MAX_ACCOUNTS       100
#define MOCK_MAX_CHARACTERS     500
#define MOCK_MAX_ITEMS          5000

/* Mock account storage */
static DBAccount g_mock_accounts[MOCK_MAX_ACCOUNTS];
static int g_mock_account_count = 0;

/* Mock character storage */
static DBCharacter g_mock_characters[MOCK_MAX_CHARACTERS];
static int g_mock_character_count = 0;
static int g_mock_next_charac_no = 100001;

/* Mock item storage */
static DBInventoryItem g_mock_items[MOCK_MAX_ITEMS];
static int g_mock_item_count = 0;
static int g_mock_next_item_id = 1;

/**
 * Initialize mock data with test accounts
 */
static void InitMockData(void)
{
    /* Create test account 1 */
    DBAccount* acc1 = &g_mock_accounts[0];
    acc1->uid = 10001;
    strncpy(acc1->account_name, "test_user", sizeof(acc1->account_name) - 1);
    strncpy(acc1->password, "test123", sizeof(acc1->password) - 1);
    strncpy(acc1->vip, "0", sizeof(acc1->vip) - 1);
    acc1->billing = 0;
    g_mock_account_count = 1;

    /* Create test characters for account 1 */
    DBCharacter* ch1 = &g_mock_characters[0];
    ch1->charac_no = 100001;
    ch1->m_id = 10001;
    strncpy(ch1->charac_name, "TestSlayer", sizeof(ch1->charac_name) - 1);
    ch1->village = 1;
    ch1->sex = 0;
    ch1->job = 0;  /* Slayer */
    ch1->level = 85;
    ch1->exp = 1234567;
    ch1->grow_type = 1;
    ch1->hp = 50000;
    ch1->max_hp = 50000;
    ch1->mp = 30000;
    ch1->max_mp = 30000;
    ch1->phy_attack = 5000;
    ch1->phy_defense = 3000;
    ch1->mag_attack = 2000;
    ch1->mag_defense = 2500;
    ch1->fatigue = 156;
    ch1->max_fatigue = 156;
    ch1->guild_id = 0;
    ch1->gold = 999999999;
    strncpy(ch1->create_time, "2024-01-01 00:00:00", sizeof(ch1->create_time) - 1);
    strncpy(ch1->last_play_time, "2025-11-26 10:00:00", sizeof(ch1->last_play_time) - 1);
    ch1->deleted = 0;
    ch1->slot = 0;

    DBCharacter* ch2 = &g_mock_characters[1];
    ch2->charac_no = 100002;
    ch2->m_id = 10001;
    strncpy(ch2->charac_name, "TestMage", sizeof(ch2->charac_name) - 1);
    ch2->village = 2;
    ch2->sex = 1;
    ch2->job = 3;  /* Mage */
    ch2->level = 70;
    ch2->exp = 567890;
    ch2->grow_type = 2;
    ch2->hp = 35000;
    ch2->max_hp = 35000;
    ch2->mp = 30000;
    ch2->max_mp = 30000;
    ch2->phy_attack = 1500;
    ch2->phy_defense = 1500;
    ch2->mag_attack = 6000;
    ch2->mag_defense = 3500;
    ch2->fatigue = 100;
    ch2->max_fatigue = 156;
    ch2->guild_id = 0;
    ch2->gold = 50000000;
    strncpy(ch2->create_time, "2024-06-01 00:00:00", sizeof(ch2->create_time) - 1);
    strncpy(ch2->last_play_time, "2025-11-25 18:00:00", sizeof(ch2->last_play_time) - 1);
    ch2->deleted = 0;
    ch2->slot = 1;

    g_mock_character_count = 2;
    g_mock_next_charac_no = 100003;

    printf("[DBService] Mock data initialized: %d accounts, %d characters\n",
           g_mock_account_count, g_mock_character_count);
}

#endif /* DB_USE_MOCK_DATA */

/*==============================================================================
 * Initialization
 *============================================================================*/

int DBService_Initialize(const char* config_file)
{
    if (g_db.initialized) {
        return 0;  /* Already initialized */
    }

    memset(&g_db, 0, sizeof(g_db));

#if DB_USE_MOCK_DATA
    (void)config_file;  /* Unused in mock mode */
    InitMockData();
    g_db.initialized = 1;
    printf("[DBService] Initialized in MOCK mode\n");
    return 0;
#else
    if (DBConnectionManager_Initialize(&g_db.db_manager, config_file) != 0) {
        snprintf(g_db.last_error, sizeof(g_db.last_error),
                 "Failed to initialize database manager");
        return -1;
    }

    /* Connect to required databases */
    if (DBConnectionManager_Connect(&g_db.db_manager, DB_TYPE_TAIWAN) != 0 ||
        DBConnectionManager_Connect(&g_db.db_manager, DB_TYPE_CAIN) != 0) {
        snprintf(g_db.last_error, sizeof(g_db.last_error),
                 "Failed to connect to databases");
        DBConnectionManager_Cleanup(&g_db.db_manager);
        return -1;
    }

    g_db.initialized = 1;
    printf("[DBService] Initialized in ORM mode\n");
    return 0;
#endif
}

void DBService_Cleanup(void)
{
    if (!g_db.initialized) return;

#if !DB_USE_MOCK_DATA
    DBConnectionManager_Cleanup(&g_db.db_manager);
#endif

    memset(&g_db, 0, sizeof(g_db));
    printf("[DBService] Cleanup complete\n");
}

bool DBService_IsReady(void)
{
    return g_db.initialized != 0;
}

/*==============================================================================
 * Account Operations
 *============================================================================*/

int DBService_VerifyAccount(const char* account_name, const char* password, DBAccount* account)
{
    if (!g_db.initialized || !account_name || !password || !account) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_account_count; i++) {
        if (strcmp(g_mock_accounts[i].account_name, account_name) == 0) {
            if (strcmp(g_mock_accounts[i].password, password) == 0) {
                memcpy(account, &g_mock_accounts[i], sizeof(DBAccount));
                printf("[DBService] Account verified: %s (UID=%d)\n", account_name, account->uid);
                return account->uid;
            }
            snprintf(g_db.last_error, sizeof(g_db.last_error), "Invalid password");
            return -1;
        }
    }
    snprintf(g_db.last_error, sizeof(g_db.last_error), "Account not found");
    return -1;
#else
    int uid = Account_Verify(&g_db.db_manager, account_name, password);
    if (uid > 0) {
        Account acc;
        if (Account_GetByUID(&g_db.db_manager, uid, &acc) == 0) {
            account->uid = acc.UID;
            strncpy(account->account_name, acc.accountname, sizeof(account->account_name) - 1);
            strncpy(account->password, "", sizeof(account->password) - 1);  /* Don't expose password */
            strncpy(account->vip, acc.VIP, sizeof(account->vip) - 1);
            account->billing = acc.billing;
            return uid;
        }
    }
    snprintf(g_db.last_error, sizeof(g_db.last_error), "Account verification failed");
    return -1;
#endif
}

int DBService_GetAccount(int uid, DBAccount* account)
{
    if (!g_db.initialized || !account) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_account_count; i++) {
        if (g_mock_accounts[i].uid == uid) {
            memcpy(account, &g_mock_accounts[i], sizeof(DBAccount));
            return 0;
        }
    }
    return -1;
#else
    Account acc;
    if (Account_GetByUID(&g_db.db_manager, uid, &acc) == 0) {
        account->uid = acc.UID;
        strncpy(account->account_name, acc.accountname, sizeof(account->account_name) - 1);
        strncpy(account->vip, acc.VIP, sizeof(account->vip) - 1);
        account->billing = acc.billing;
        return 0;
    }
    return -1;
#endif
}

bool DBService_AccountExists(const char* account_name)
{
    if (!g_db.initialized || !account_name) {
        return false;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_account_count; i++) {
        if (strcmp(g_mock_accounts[i].account_name, account_name) == 0) {
            return true;
        }
    }
    return false;
#else
    return Account_NameExists(&g_db.db_manager, account_name);
#endif
}

/*==============================================================================
 * Character Operations
 *============================================================================*/

int DBService_GetCharacters(int account_uid, DBCharacter* characters, int max_count)
{
    if (!g_db.initialized || !characters || max_count <= 0) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    int count = 0;
    for (int i = 0; i < g_mock_character_count && count < max_count; i++) {
        if (g_mock_characters[i].m_id == account_uid && !g_mock_characters[i].deleted) {
            memcpy(&characters[count], &g_mock_characters[i], sizeof(DBCharacter));
            count++;
        }
    }
    printf("[DBService] GetCharacters: account=%d, count=%d\n", account_uid, count);
    return count;
#else
    CharacInfo infos[DB_MAX_CHARACTERS];
    int count = CharacInfo_GetByMemberId(&g_db.db_manager, account_uid, infos, max_count);
    if (count <= 0) return count;

    for (int i = 0; i < count; i++) {
        DBCharacter* ch = &characters[i];
        CharacInfo* info = &infos[i];

        ch->charac_no = info->charac_no;
        ch->m_id = info->m_id;
        strncpy(ch->charac_name, info->charac_name, sizeof(ch->charac_name) - 1);
        ch->village = info->village;
        ch->sex = info->sex;
        ch->job = info->job;
        ch->level = info->lev;
        ch->exp = info->exp;
        ch->grow_type = info->grow_type;
        ch->hp = info->HP;
        ch->max_hp = info->maxHP;
        ch->max_mp = info->maxMP;
        ch->phy_attack = info->phy_attack;
        ch->phy_defense = info->phy_defense;
        ch->mag_attack = info->mag_attack;
        ch->mag_defense = info->mag_defense;
        ch->fatigue = info->fatigue;
        ch->max_fatigue = info->max_fatigue;
        ch->guild_id = info->guild_id;
        strncpy(ch->create_time, info->create_time, sizeof(ch->create_time) - 1);
        strncpy(ch->last_play_time, info->last_play_time, sizeof(ch->last_play_time) - 1);
        strncpy(ch->delete_time, info->delete_time, sizeof(ch->delete_time) - 1);
        ch->deleted = (info->delete_time[0] != '\0') ? 1 : 0;
    }
    return count;
#endif
}

int DBService_GetCharacter(int charac_no, DBCharacter* character)
{
    if (!g_db.initialized || !character) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_character_count; i++) {
        if (g_mock_characters[i].charac_no == charac_no) {
            memcpy(character, &g_mock_characters[i], sizeof(DBCharacter));
            return 0;
        }
    }
    return -1;
#else
    CharacInfo info;
    if (CharacInfo_GetByCharacNo(&g_db.db_manager, charac_no, &info) != 0) {
        return -1;
    }

    character->charac_no = info.charac_no;
    character->m_id = info.m_id;
    strncpy(character->charac_name, info.charac_name, sizeof(character->charac_name) - 1);
    character->village = info.village;
    character->sex = info.sex;
    character->job = info.job;
    character->level = info.lev;
    character->exp = info.exp;
    character->grow_type = info.grow_type;
    character->hp = info.HP;
    character->max_hp = info.maxHP;
    character->max_mp = info.maxMP;
    character->fatigue = info.fatigue;
    character->max_fatigue = info.max_fatigue;
    character->guild_id = info.guild_id;
    return 0;
#endif
}

int DBService_CreateCharacter(int account_uid, DBCharacter* character)
{
    if (!g_db.initialized || !character) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    if (g_mock_character_count >= MOCK_MAX_CHARACTERS) {
        snprintf(g_db.last_error, sizeof(g_db.last_error), "Max characters reached");
        return -1;
    }

    /* Check character count for account */
    int count = DBService_GetCharacterCount(account_uid);
    if (count >= DB_MAX_CHARACTERS) {
        snprintf(g_db.last_error, sizeof(g_db.last_error), "Account character limit reached");
        return -1;
    }

    /* Check name exists */
    if (DBService_CharacterNameExists(character->charac_name)) {
        snprintf(g_db.last_error, sizeof(g_db.last_error), "Character name already exists");
        return -1;
    }

    /* Create new character */
    int new_charac_no = g_mock_next_charac_no++;
    character->charac_no = new_charac_no;
    character->m_id = account_uid;

    /* Set default values */
    character->level = 1;
    character->exp = 0;
    character->hp = 1000;
    character->max_hp = 1000;
    character->mp = 500;
    character->max_mp = 500;
    character->phy_attack = 100;
    character->phy_defense = 50;
    character->mag_attack = 100;
    character->mag_defense = 50;
    character->fatigue = 156;
    character->max_fatigue = 156;
    character->gold = 10000;
    character->deleted = 0;

    /* Set timestamps */
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(character->create_time, sizeof(character->create_time), "%Y-%m-%d %H:%M:%S", tm_info);
    strncpy(character->last_play_time, character->create_time, sizeof(character->last_play_time) - 1);
    character->delete_time[0] = '\0';

    /* Store in mock data */
    memcpy(&g_mock_characters[g_mock_character_count], character, sizeof(DBCharacter));
    g_mock_character_count++;

    printf("[DBService] Created character: %s (charac_no=%d) for account=%d\n",
           character->charac_name, new_charac_no, account_uid);
    return new_charac_no;
#else
    CharacInfo info;
    memset(&info, 0, sizeof(CharacInfo));

    info.m_id = account_uid;
    strncpy(info.charac_name, character->charac_name, sizeof(info.charac_name) - 1);
    info.village = character->village;
    info.sex = character->sex;
    info.job = character->job;
    info.lev = 1;
    info.exp = 0;
    info.grow_type = character->grow_type;
    info.HP = 1000;
    info.maxHP = 1000;
    info.maxMP = 500;
    info.fatigue = 156;
    info.max_fatigue = 156;

    if (CharacInfo_Create(&g_db.db_manager, &info) != 0) {
        return -1;
    }

    /* Get the created character to get charac_no */
    if (CharacInfo_GetByName(&g_db.db_manager, character->charac_name, &info) != 0) {
        return -1;
    }

    character->charac_no = info.charac_no;
    return info.charac_no;
#endif
}

int DBService_DeleteCharacter(int charac_no)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_character_count; i++) {
        if (g_mock_characters[i].charac_no == charac_no) {
            g_mock_characters[i].deleted = 1;
            time_t now = time(NULL);
            struct tm* tm_info = localtime(&now);
            strftime(g_mock_characters[i].delete_time, sizeof(g_mock_characters[i].delete_time),
                     "%Y-%m-%d %H:%M:%S", tm_info);
            printf("[DBService] Deleted character: charac_no=%d\n", charac_no);
            return 0;
        }
    }
    return -1;
#else
    /* Mark character as deleted by setting delete_time */
    return CharacInfo_Delete(&g_db.db_manager, charac_no);
#endif
}

int DBService_UpdateCharacterStats(int charac_no, const DBCharacterStat* stat)
{
    if (!g_db.initialized || !stat) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_character_count; i++) {
        if (g_mock_characters[i].charac_no == charac_no) {
            g_mock_characters[i].exp = stat->exp;
            g_mock_characters[i].fatigue = stat->fatigue;
            /* Update last play time */
            time_t now = time(NULL);
            struct tm* tm_info = localtime(&now);
            strftime(g_mock_characters[i].last_play_time, sizeof(g_mock_characters[i].last_play_time),
                     "%Y-%m-%d %H:%M:%S", tm_info);
            return 0;
        }
    }
    return -1;
#else
    CharacStat db_stat;
    db_stat.charac_no = charac_no;
    db_stat.exp = stat->exp;
    db_stat.fatigue = stat->fatigue;
    db_stat.used_fatigue = stat->used_fatigue;
    db_stat.dungeon_clear_point = stat->dungeon_clear_point;
    db_stat.dungeon_play_count = stat->dungeon_play_count;
    return CharacStat_Update(&g_db.db_manager, &db_stat);
#endif
}

int DBService_UpdateCharacterLevel(int charac_no, uint8_t level, int exp)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_character_count; i++) {
        if (g_mock_characters[i].charac_no == charac_no) {
            g_mock_characters[i].level = level;
            g_mock_characters[i].exp = exp;
            return 0;
        }
    }
    return -1;
#else
    return CharacInfo_UpdateLevelExp(&g_db.db_manager, charac_no, level, exp);
#endif
}

int DBService_UpdateCharacterLocation(int charac_no, int8_t village)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_character_count; i++) {
        if (g_mock_characters[i].charac_no == charac_no) {
            g_mock_characters[i].village = village;
            return 0;
        }
    }
    return -1;
#else
    DBCharacter ch;
    if (DBService_GetCharacter(charac_no, &ch) != 0) {
        return -1;
    }
    CharacInfo info;
    if (CharacInfo_GetByCharacNo(&g_db.db_manager, charac_no, &info) != 0) {
        return -1;
    }
    info.village = village;
    return CharacInfo_Update(&g_db.db_manager, &info);
#endif
}

bool DBService_CharacterNameExists(const char* charac_name)
{
    if (!g_db.initialized || !charac_name) {
        return false;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_character_count; i++) {
        if (strcmp(g_mock_characters[i].charac_name, charac_name) == 0 &&
            !g_mock_characters[i].deleted) {
            return true;
        }
    }
    return false;
#else
    CharacInfo info;
    return CharacInfo_GetByName(&g_db.db_manager, charac_name, &info) == 0;
#endif
}

int DBService_GetCharacterCount(int account_uid)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    int count = 0;
    for (int i = 0; i < g_mock_character_count; i++) {
        if (g_mock_characters[i].m_id == account_uid && !g_mock_characters[i].deleted) {
            count++;
        }
    }
    return count;
#else
    return CharacInfo_GetCountByMemberId(&g_db.db_manager, account_uid);
#endif
}

/*==============================================================================
 * Inventory Operations
 *============================================================================*/

int DBService_GetInventory(int charac_no, DBInventoryItem* items, int max_count)
{
    if (!g_db.initialized || !items || max_count <= 0) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    int count = 0;
    for (int i = 0; i < g_mock_item_count && count < max_count; i++) {
        if (g_mock_items[i].charac_no == charac_no) {
            memcpy(&items[count], &g_mock_items[i], sizeof(DBInventoryItem));
            count++;
        }
    }
    return count;
#else
    /* TODO: Implement with inventory ORM */
    (void)charac_no;
    return 0;
#endif
}

int DBService_AddItem(int charac_no, DBInventoryItem* item)
{
    if (!g_db.initialized || !item) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    if (g_mock_item_count >= MOCK_MAX_ITEMS) {
        snprintf(g_db.last_error, sizeof(g_db.last_error), "Max items reached");
        return -1;
    }

    item->ui_id = g_mock_next_item_id++;
    item->charac_no = charac_no;

    memcpy(&g_mock_items[g_mock_item_count], item, sizeof(DBInventoryItem));
    g_mock_item_count++;

    return item->ui_id;
#else
    /* TODO: Implement with inventory ORM */
    return -1;
#endif
}

int DBService_RemoveItem(int ui_id)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_item_count; i++) {
        if (g_mock_items[i].ui_id == ui_id) {
            /* Remove by shifting remaining items */
            memmove(&g_mock_items[i], &g_mock_items[i + 1],
                    (g_mock_item_count - i - 1) * sizeof(DBInventoryItem));
            g_mock_item_count--;
            return 0;
        }
    }
    return -1;
#else
    /* TODO: Implement with inventory ORM */
    return -1;
#endif
}

int DBService_UpdateItemSlot(int ui_id, int new_slot)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_item_count; i++) {
        if (g_mock_items[i].ui_id == ui_id) {
            g_mock_items[i].slot = new_slot;
            return 0;
        }
    }
    return -1;
#else
    /* TODO: Implement with inventory ORM */
    (void)new_slot;
    return -1;
#endif
}

int DBService_UpdateItemUpgrade(int ui_id, int8_t upgrade)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_item_count; i++) {
        if (g_mock_items[i].ui_id == ui_id) {
            g_mock_items[i].upgrade = upgrade;
            return 0;
        }
    }
    return -1;
#else
    /* TODO: Implement with inventory ORM */
    (void)upgrade;
    return -1;
#endif
}

/*==============================================================================
 * Gold Operations
 *============================================================================*/

int64_t DBService_GetGold(int charac_no)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_character_count; i++) {
        if (g_mock_characters[i].charac_no == charac_no) {
            return g_mock_characters[i].gold;
        }
    }
    return -1;
#else
    DBCharacter ch;
    if (DBService_GetCharacter(charac_no, &ch) != 0) {
        return -1;
    }
    return ch.gold;
#endif
}

int DBService_SetGold(int charac_no, uint32_t gold)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_character_count; i++) {
        if (g_mock_characters[i].charac_no == charac_no) {
            g_mock_characters[i].gold = gold;
            return 0;
        }
    }
    return -1;
#else
    /* TODO: Implement with proper gold update */
    (void)gold;
    return -1;
#endif
}

int DBService_AddGold(int charac_no, uint32_t amount)
{
    int64_t current = DBService_GetGold(charac_no);
    if (current < 0) return -1;

    uint32_t new_gold = (uint32_t)(current + amount);
    if (new_gold < current) {
        /* Overflow - cap at max */
        new_gold = 0xFFFFFFFF;
    }
    return DBService_SetGold(charac_no, new_gold);
}

int DBService_DeductGold(int charac_no, uint32_t amount)
{
    int64_t current = DBService_GetGold(charac_no);
    if (current < 0) return -1;

    if ((uint32_t)current < amount) {
        snprintf(g_db.last_error, sizeof(g_db.last_error), "Insufficient gold");
        return -1;
    }

    return DBService_SetGold(charac_no, (uint32_t)(current - amount));
}

/*==============================================================================
 * Guild Operations
 *============================================================================*/

int DBService_GetGuild(int guild_id, DBGuild* guild)
{
    if (!g_db.initialized || !guild) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    /* No mock guilds by default */
    (void)guild_id;
    return -1;
#else
    /* TODO: Implement with guild ORM */
    (void)guild_id;
    return -1;
#endif
}

int DBService_CreateGuild(DBGuild* guild)
{
    if (!g_db.initialized || !guild) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    /* TODO: Implement mock guild creation */
    return -1;
#else
    /* TODO: Implement with guild ORM */
    return -1;
#endif
}

int DBService_AddGuildMember(int guild_id, int charac_no, int8_t guild_right)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_character_count; i++) {
        if (g_mock_characters[i].charac_no == charac_no) {
            g_mock_characters[i].guild_id = guild_id;
            g_mock_characters[i].guild_right = guild_right;
            return 0;
        }
    }
    return -1;
#else
    return CharacInfo_UpdateGuild(&g_db.db_manager, charac_no, guild_id, guild_right);
#endif
}

int DBService_RemoveGuildMember(int guild_id, int charac_no)
{
    (void)guild_id;
    return DBService_AddGuildMember(0, charac_no, 0);
}

int DBService_GetGuildMembers(int guild_id, DBCharacter* characters, int max_count)
{
    if (!g_db.initialized || !characters || max_count <= 0) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    int count = 0;
    for (int i = 0; i < g_mock_character_count && count < max_count; i++) {
        if (g_mock_characters[i].guild_id == guild_id && !g_mock_characters[i].deleted) {
            memcpy(&characters[count], &g_mock_characters[i], sizeof(DBCharacter));
            count++;
        }
    }
    return count;
#else
    /* TODO: Implement with guild member ORM */
    (void)guild_id;
    return 0;
#endif
}

/*==============================================================================
 * Dungeon Operations
 *============================================================================*/

int DBService_UpdateDungeonClear(int charac_no, int dungeon_id, int clear_time)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    /* Mock: just update last play time */
    (void)dungeon_id;
    (void)clear_time;
    for (int i = 0; i < g_mock_character_count; i++) {
        if (g_mock_characters[i].charac_no == charac_no) {
            time_t now = time(NULL);
            struct tm* tm_info = localtime(&now);
            strftime(g_mock_characters[i].last_play_time, sizeof(g_mock_characters[i].last_play_time),
                     "%Y-%m-%d %H:%M:%S", tm_info);
            return 0;
        }
    }
    return -1;
#else
    /* TODO: Implement with dungeon ORM */
    (void)dungeon_id;
    (void)clear_time;
    return -1;
#endif
}

int DBService_IncrementDungeonPlayCount(int charac_no)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    return 0;  /* Mock: no-op */
#else
    return CharacStat_IncrementDungeonPlayCount(&g_db.db_manager, charac_no);
#endif
}

int DBService_UseFatigue(int charac_no, int16_t fatigue_used)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_character_count; i++) {
        if (g_mock_characters[i].charac_no == charac_no) {
            if (g_mock_characters[i].fatigue < fatigue_used) {
                snprintf(g_db.last_error, sizeof(g_db.last_error), "Insufficient fatigue");
                return -1;
            }
            g_mock_characters[i].fatigue -= fatigue_used;
            return 0;
        }
    }
    return -1;
#else
    /* Get current fatigue */
    CharacStat stat;
    if (CharacStat_GetByCharacNo(&g_db.db_manager, charac_no, &stat) != 0) {
        return -1;
    }
    if (stat.fatigue < fatigue_used) {
        snprintf(g_db.last_error, sizeof(g_db.last_error), "Insufficient fatigue");
        return -1;
    }
    return CharacStat_UpdateFatigue(&g_db.db_manager, charac_no,
                                     stat.fatigue - fatigue_used,
                                     stat.used_fatigue + fatigue_used,
                                     stat.premium_fatigue);
#endif
}

/*==============================================================================
 * Skill Operations
 *============================================================================*/

int DBService_GetSkills(int charac_no, int* skill_ids, int* skill_levels, int max_count)
{
    if (!g_db.initialized || !skill_ids || !skill_levels || max_count <= 0) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    /* Mock: return some default skills based on job */
    (void)charac_no;
    return 0;  /* No skills stored in mock */
#else
    /* TODO: Implement with skill ORM */
    (void)charac_no;
    return 0;
#endif
}

int DBService_UpdateSkill(int charac_no, int skill_id, int level)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    /* Mock: no-op */
    (void)charac_no;
    (void)skill_id;
    (void)level;
    return 0;
#else
    /* TODO: Implement with skill ORM */
    (void)charac_no;
    (void)skill_id;
    (void)level;
    return -1;
#endif
}

/*==============================================================================
 * Transaction Operations
 *============================================================================*/

int DBService_BeginTransaction(void)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    return 0;  /* Mock: no-op */
#else
    return DBConnectionManager_BeginTransaction(&g_db.db_manager, DB_TYPE_CAIN);
#endif
}

int DBService_CommitTransaction(void)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    return 0;  /* Mock: no-op */
#else
    return DBConnectionManager_CommitTransaction(&g_db.db_manager, DB_TYPE_CAIN);
#endif
}

int DBService_RollbackTransaction(void)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    return 0;  /* Mock: no-op */
#else
    return DBConnectionManager_RollbackTransaction(&g_db.db_manager, DB_TYPE_CAIN);
#endif
}

const char* DBService_GetLastError(void)
{
    return g_db.last_error;
}

/*==============================================================================
 * Phase 4: Trade Operations (Mock Implementation)
 *============================================================================*/

#if DB_USE_MOCK_DATA
#define MOCK_MAX_TRADES 100
#define MOCK_MAX_MAILS 100
#define MOCK_MAX_ACHIEVEMENTS 50

static DBTrade g_mock_trades[MOCK_MAX_TRADES];
static int g_mock_trade_count = 0;
static int g_mock_next_trade_id = 1;

static DBMail g_mock_mails[MOCK_MAX_MAILS];
static int g_mock_mail_count = 0;
static int g_mock_next_mail_id = 1;

static DBAchievement g_mock_achievements[MOCK_MAX_ACHIEVEMENTS];
static int g_mock_achievement_count = 0;

/* Direct trade sessions */
typedef struct {
    int session_id;
    int charac1;
    int charac2;
    int confirmed1;
    int confirmed2;
} DirectTradeSession;
static DirectTradeSession g_mock_trade_sessions[10];
static int g_mock_trade_session_count = 0;
static int g_mock_next_session_id = 1;
#endif

int DBService_CreateTrade(DBTrade* trade)
{
    if (!g_db.initialized || !trade) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    if (g_mock_trade_count >= MOCK_MAX_TRADES) {
        snprintf(g_db.last_error, sizeof(g_db.last_error), "Trade storage full");
        return -1;
    }
    trade->trade_id = g_mock_next_trade_id++;
    trade->status = 0;  /* pending */
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(trade->create_time, sizeof(trade->create_time), "%Y-%m-%d %H:%M:%S", tm_info);
    g_mock_trades[g_mock_trade_count++] = *trade;
    return trade->trade_id;
#else
    (void)trade;
    return -1;
#endif
}

int DBService_GetTrades(int charac_no, DBTrade* trades, int max_count)
{
    if (!g_db.initialized || !trades || max_count <= 0) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    int count = 0;
    for (int i = 0; i < g_mock_trade_count && count < max_count; i++) {
        if (g_mock_trades[i].seller_id == charac_no && g_mock_trades[i].status == 0) {
            trades[count++] = g_mock_trades[i];
        }
    }
    return count;
#else
    (void)charac_no;
    return 0;
#endif
}

int DBService_CompleteTrade(int trade_id, int buyer_id)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_trade_count; i++) {
        if (g_mock_trades[i].trade_id == trade_id && g_mock_trades[i].status == 0) {
            g_mock_trades[i].buyer_id = buyer_id;
            g_mock_trades[i].status = 1;  /* completed */
            return 0;
        }
    }
    return -1;
#else
    (void)trade_id;
    (void)buyer_id;
    return -1;
#endif
}

int DBService_CancelTrade(int trade_id)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_trade_count; i++) {
        if (g_mock_trades[i].trade_id == trade_id && g_mock_trades[i].status == 0) {
            g_mock_trades[i].status = 2;  /* cancelled */
            return 0;
        }
    }
    return -1;
#else
    (void)trade_id;
    return -1;
#endif
}

int DBService_StartDirectTrade(int charac1, int charac2)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    if (g_mock_trade_session_count >= 10) {
        return -1;
    }
    DirectTradeSession* session = &g_mock_trade_sessions[g_mock_trade_session_count++];
    session->session_id = g_mock_next_session_id++;
    session->charac1 = charac1;
    session->charac2 = charac2;
    session->confirmed1 = 0;
    session->confirmed2 = 0;
    return session->session_id;
#else
    (void)charac1;
    (void)charac2;
    return -1;
#endif
}

int DBService_ConfirmDirectTrade(int trade_session_id, int charac_no)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_trade_session_count; i++) {
        DirectTradeSession* s = &g_mock_trade_sessions[i];
        if (s->session_id == trade_session_id) {
            if (s->charac1 == charac_no) s->confirmed1 = 1;
            if (s->charac2 == charac_no) s->confirmed2 = 1;
            return (s->confirmed1 && s->confirmed2) ? 1 : 0;  /* 1 = both confirmed */
        }
    }
    return -1;
#else
    (void)trade_session_id;
    (void)charac_no;
    return -1;
#endif
}

/*==============================================================================
 * Phase 4: Mail Operations (Mock Implementation)
 *============================================================================*/

int DBService_SendMail(DBMail* mail)
{
    if (!g_db.initialized || !mail) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    if (g_mock_mail_count >= MOCK_MAX_MAILS) {
        snprintf(g_db.last_error, sizeof(g_db.last_error), "Mailbox storage full");
        return -1;
    }
    mail->mail_id = g_mock_next_mail_id++;
    mail->is_read = 0;
    mail->item_extracted = 0;
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(mail->send_time, sizeof(mail->send_time), "%Y-%m-%d %H:%M:%S", tm_info);
    /* Set expiry to 30 days */
    time_t expire = now + (30 * 24 * 60 * 60);
    tm_info = localtime(&expire);
    strftime(mail->expire_time, sizeof(mail->expire_time), "%Y-%m-%d %H:%M:%S", tm_info);
    g_mock_mails[g_mock_mail_count++] = *mail;
    return mail->mail_id;
#else
    (void)mail;
    return -1;
#endif
}

int DBService_GetMailbox(int charac_no, DBMail* mails, int max_count)
{
    if (!g_db.initialized || !mails || max_count <= 0) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    int count = 0;
    for (int i = 0; i < g_mock_mail_count && count < max_count; i++) {
        if (g_mock_mails[i].receiver_id == charac_no) {
            mails[count++] = g_mock_mails[i];
        }
    }
    return count;
#else
    (void)charac_no;
    return 0;
#endif
}

int DBService_ReadMail(int mail_id)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_mail_count; i++) {
        if (g_mock_mails[i].mail_id == mail_id) {
            g_mock_mails[i].is_read = 1;
            return 0;
        }
    }
    return -1;
#else
    (void)mail_id;
    return -1;
#endif
}

int DBService_ExtractMailAttachment(int mail_id, int charac_no)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_mail_count; i++) {
        if (g_mock_mails[i].mail_id == mail_id &&
            g_mock_mails[i].receiver_id == charac_no &&
            !g_mock_mails[i].item_extracted) {
            g_mock_mails[i].item_extracted = 1;
            /* In real implementation, add item/gold to character */
            return 0;
        }
    }
    return -1;
#else
    (void)mail_id;
    (void)charac_no;
    return -1;
#endif
}

int DBService_DeleteMail(int mail_id)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_mail_count; i++) {
        if (g_mock_mails[i].mail_id == mail_id) {
            /* Shift remaining mails */
            for (int j = i; j < g_mock_mail_count - 1; j++) {
                g_mock_mails[j] = g_mock_mails[j + 1];
            }
            g_mock_mail_count--;
            return 0;
        }
    }
    return -1;
#else
    (void)mail_id;
    return -1;
#endif
}

int DBService_GetUnreadMailCount(int charac_no)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    int count = 0;
    for (int i = 0; i < g_mock_mail_count; i++) {
        if (g_mock_mails[i].receiver_id == charac_no && !g_mock_mails[i].is_read) {
            count++;
        }
    }
    return count;
#else
    (void)charac_no;
    return 0;
#endif
}

/*==============================================================================
 * Phase 4: Enhancement Operations (Mock Implementation)
 *============================================================================*/

int DBService_UpgradeItem(int ui_id, int success_rate, int destroy_on_fail)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    /* Find item in mock items array */
    for (int i = 0; i < g_mock_item_count; i++) {
        if (g_mock_items[i].ui_id == ui_id) {
            /* Roll for success */
            int roll = rand() % 100;
            if (roll < success_rate) {
                /* Success - increase upgrade level */
                if (g_mock_items[i].upgrade < 15) {
                    g_mock_items[i].upgrade++;
                }
                return g_mock_items[i].upgrade;
            } else {
                /* Failure */
                if (destroy_on_fail) {
                    /* Remove item by shifting */
                    for (int j = i; j < g_mock_item_count - 1; j++) {
                        g_mock_items[j] = g_mock_items[j + 1];
                    }
                    g_mock_item_count--;
                    return -2;  /* destroyed */
                }
                return g_mock_items[i].upgrade;  /* unchanged */
            }
        }
    }
    return -1;
#else
    (void)ui_id;
    (void)success_rate;
    (void)destroy_on_fail;
    return -1;
#endif
}

int DBService_EnchantItem(int ui_id, int enchant_type, int enchant_value)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    /* Find item and set enchant */
    for (int i = 0; i < g_mock_item_count; i++) {
        if (g_mock_items[i].ui_id == ui_id) {
            /* Store enchant in stat_id field for mock */
            g_mock_items[i].stat_id = (uint16_t)((enchant_type << 8) | (enchant_value & 0xFF));
            return 0;
        }
    }
    return -1;
#else
    (void)ui_id;
    (void)enchant_type;
    (void)enchant_value;
    return -1;
#endif
}

int DBService_AmplifyItem(int ui_id, int amplify_value)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    /* Find item and set amplify */
    for (int i = 0; i < g_mock_item_count; i++) {
        if (g_mock_items[i].ui_id == ui_id) {
            /* Store amplify in ob_type field for mock */
            g_mock_items[i].ob_type = (int8_t)amplify_value;
            return 0;
        }
    }
    return -1;
#else
    (void)ui_id;
    (void)amplify_value;
    return -1;
#endif
}

int DBService_GetItemUpgradeInfo(int ui_id, int8_t* upgrade, int* enchant_type, int* enchant_value)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_item_count; i++) {
        if (g_mock_items[i].ui_id == ui_id) {
            if (upgrade) *upgrade = g_mock_items[i].upgrade;
            if (enchant_type) *enchant_type = g_mock_items[i].stat_id >> 8;
            if (enchant_value) *enchant_value = g_mock_items[i].stat_id & 0xFF;
            return 0;
        }
    }
    return -1;
#else
    (void)ui_id;
    (void)upgrade;
    (void)enchant_type;
    (void)enchant_value;
    return -1;
#endif
}

/*==============================================================================
 * Phase 4: Achievement Operations (Mock Implementation)
 *============================================================================*/

int DBService_GetAchievements(int charac_no, DBAchievement* achievements, int max_count)
{
    if (!g_db.initialized || !achievements || max_count <= 0) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    int count = 0;
    for (int i = 0; i < g_mock_achievement_count && count < max_count; i++) {
        if (g_mock_achievements[i].charac_no == charac_no) {
            achievements[count++] = g_mock_achievements[i];
        }
    }
    return count;
#else
    (void)charac_no;
    return 0;
#endif
}

int DBService_UpdateAchievementProgress(int charac_no, int achievement_id, int progress)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    /* Find or create achievement entry */
    for (int i = 0; i < g_mock_achievement_count; i++) {
        if (g_mock_achievements[i].charac_no == charac_no &&
            g_mock_achievements[i].achievement_id == achievement_id) {
            g_mock_achievements[i].progress = progress;
            if (progress >= g_mock_achievements[i].target && !g_mock_achievements[i].completed) {
                g_mock_achievements[i].completed = 1;
                time_t now = time(NULL);
                struct tm* tm_info = localtime(&now);
                strftime(g_mock_achievements[i].complete_time,
                         sizeof(g_mock_achievements[i].complete_time),
                         "%Y-%m-%d %H:%M:%S", tm_info);
                return 1;  /* completed */
            }
            return 0;
        }
    }
    /* Create new achievement entry */
    if (g_mock_achievement_count < MOCK_MAX_ACHIEVEMENTS) {
        DBAchievement* a = &g_mock_achievements[g_mock_achievement_count++];
        a->achievement_id = achievement_id;
        a->charac_no = charac_no;
        a->progress = progress;
        a->target = 10;  /* default target */
        a->completed = 0;
        a->reward_claimed = 0;
        return 0;
    }
    return -1;
#else
    (void)charac_no;
    (void)achievement_id;
    (void)progress;
    return -1;
#endif
}

int DBService_ClaimAchievementReward(int charac_no, int achievement_id)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    for (int i = 0; i < g_mock_achievement_count; i++) {
        if (g_mock_achievements[i].charac_no == charac_no &&
            g_mock_achievements[i].achievement_id == achievement_id &&
            g_mock_achievements[i].completed &&
            !g_mock_achievements[i].reward_claimed) {
            g_mock_achievements[i].reward_claimed = 1;
            return 0;
        }
    }
    return -1;
#else
    (void)charac_no;
    (void)achievement_id;
    return -1;
#endif
}

int DBService_TriggerAchievement(int charac_no, int achievement_id)
{
    if (!g_db.initialized) {
        return -1;
    }

#if DB_USE_MOCK_DATA
    /* Find achievement and increment */
    for (int i = 0; i < g_mock_achievement_count; i++) {
        if (g_mock_achievements[i].charac_no == charac_no &&
            g_mock_achievements[i].achievement_id == achievement_id) {
            return DBService_UpdateAchievementProgress(charac_no, achievement_id,
                                                        g_mock_achievements[i].progress + 1);
        }
    }
    /* Create new and set progress to 1 */
    return DBService_UpdateAchievementProgress(charac_no, achievement_id, 1);
#else
    (void)charac_no;
    (void)achievement_id;
    return -1;
#endif
}
