/*
 * orm_adapter.c - ORM Layer Adapter Implementation
 *
 * This adapter wraps the generated ORM modules to provide a unified
 * interface for the game server.
 *
 * Compatible with GCC 4.1.2 / CentOS 5.8 (VM-131)
 */

#include "orm_adapter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

/*==============================================================================
 * Database Configuration
 *============================================================================*/

#define DB_HOST "192.168.200.131"
#define DB_PORT 3306
#define DB_USER "root"
#define DB_PASS "88888888"

#define DB_ACCOUNT  "d_taiwan"
#define DB_CHARAC   "taiwan_cain"
#define DB_CHARAC2  "taiwan_cain_2nd"
#define DB_LOGIN    "taiwan_login"
#define DB_GUILD    "d_guild"

/*==============================================================================
 * Global State
 *============================================================================*/

static MYSQL* g_orm_db_account = NULL;
static MYSQL* g_orm_db_charac = NULL;
static MYSQL* g_orm_db_charac2 = NULL;
static MYSQL* g_orm_db_login = NULL;
static MYSQL* g_orm_db_guild = NULL;
static int g_orm_initialized = 0;

/*==============================================================================
 * Helper Functions
 *============================================================================*/

static MYSQL* orm_get_db(int db_type) {
    switch (db_type) {
        case ORM_DB_TAIWAN: return g_orm_db_account;
        case ORM_DB_CAIN:   return g_orm_db_charac;
        case ORM_DB_CAIN2:  return g_orm_db_charac2;
        case ORM_DB_LOGIN:  return g_orm_db_login;
        case ORM_DB_GUILD:  return g_orm_db_guild;
        default: return NULL;
    }
}

static MYSQL* orm_connect_db(const char* host, int port, const char* user,
                             const char* pass, const char* db_name) {
    MYSQL* conn = mysql_init(NULL);
    if (!conn) {
        printf("[ORM] Failed to init MySQL handle for %s\n", db_name);
        return NULL;
    }

    unsigned int timeout = 5;
    mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&timeout);
    mysql_options(conn, MYSQL_OPT_READ_TIMEOUT, (const char*)&timeout);
    mysql_options(conn, MYSQL_OPT_WRITE_TIMEOUT, (const char*)&timeout);

    if (!mysql_real_connect(conn, host, user, pass, db_name, port, NULL, 0)) {
        printf("[ORM] Failed to connect to %s: %s\n", db_name, mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    /* Set charset to UTF-8 */
    mysql_set_character_set(conn, "utf8");

    printf("[ORM] Connected to database: %s\n", db_name);
    return conn;
}

/*==============================================================================
 * Initialization Functions
 *============================================================================*/

int orm_init(const char* config_file) {
    (void)config_file; /* TODO: Read config from file */

    if (g_orm_initialized) {
        printf("[ORM] Already initialized\n");
        return 0;
    }

    printf("[ORM] Initializing ORM layer...\n");

    /* Connect to all databases */
    g_orm_db_account = orm_connect_db(DB_HOST, DB_PORT, DB_USER, DB_PASS, DB_ACCOUNT);
    if (!g_orm_db_account) goto error;

    g_orm_db_charac = orm_connect_db(DB_HOST, DB_PORT, DB_USER, DB_PASS, DB_CHARAC);
    if (!g_orm_db_charac) goto error;

    g_orm_db_charac2 = orm_connect_db(DB_HOST, DB_PORT, DB_USER, DB_PASS, DB_CHARAC2);
    if (!g_orm_db_charac2) goto error;

    g_orm_db_guild = orm_connect_db(DB_HOST, DB_PORT, DB_USER, DB_PASS, DB_GUILD);
    if (!g_orm_db_guild) goto error;

    /* Login DB is optional */
    g_orm_db_login = orm_connect_db(DB_HOST, DB_PORT, DB_USER, DB_PASS, DB_LOGIN);

    g_orm_initialized = 1;
    printf("[ORM] ORM layer initialized successfully\n");
    return 0;

error:
    orm_cleanup();
    return -1;
}

void orm_cleanup(void) {
    printf("[ORM] Cleaning up ORM layer...\n");

    if (g_orm_db_account) { mysql_close(g_orm_db_account); g_orm_db_account = NULL; }
    if (g_orm_db_charac)  { mysql_close(g_orm_db_charac);  g_orm_db_charac = NULL;  }
    if (g_orm_db_charac2) { mysql_close(g_orm_db_charac2); g_orm_db_charac2 = NULL; }
    if (g_orm_db_login)   { mysql_close(g_orm_db_login);   g_orm_db_login = NULL;   }
    if (g_orm_db_guild)   { mysql_close(g_orm_db_guild);   g_orm_db_guild = NULL;   }

    g_orm_initialized = 0;
}

int orm_is_initialized(void) {
    return g_orm_initialized;
}

/*==============================================================================
 * Account Functions
 *============================================================================*/

int orm_get_account_by_username(const char* username, uint32_t* uid, char* password_hash, int max_pwd_len) {
    if (!g_orm_db_account || !username || !uid || !password_hash) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT UID, m_id, m_passwd FROM accounts WHERE m_id='%s' LIMIT 1",
             username);

    if (mysql_query(g_orm_db_account, query)) {
        printf("[ORM] Account query failed: %s\n", mysql_error(g_orm_db_account));
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_orm_db_account);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        return -1;
    }

    *uid = row[0] ? (uint32_t)atoi(row[0]) : 0;
    if (row[2] && max_pwd_len > 0) {
        strncpy(password_hash, row[2], max_pwd_len - 1);
        password_hash[max_pwd_len - 1] = '\0';
    }

    mysql_free_result(res);
    return 0;
}

int orm_get_account_by_uid(uint32_t uid, char* username, int max_name_len) {
    if (!g_orm_db_account || !username) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT m_id FROM accounts WHERE UID=%u LIMIT 1", uid);

    if (mysql_query(g_orm_db_account, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_orm_db_account);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row || !row[0]) {
        mysql_free_result(res);
        return -1;
    }

    strncpy(username, row[0], max_name_len - 1);
    username[max_name_len - 1] = '\0';

    mysql_free_result(res);
    return 0;
}

/*==============================================================================
 * Character Functions
 *============================================================================*/

int orm_get_characters(uint32_t m_id, ORM_CharacInfo* chars, int max_count) {
    if (!g_orm_db_charac || !chars) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT charac_no, m_id, charac_name, village, sex, job, lev, exp, "
             "grow_type, expert_job, HP, maxHP, maxMP, phy_attack, phy_defense, "
             "mag_attack, mag_defense, move_speed, attack_speed, cast_speed, "
             "fatigue, max_fatigue, guild_id, guild_right, create_time, last_play_time, delete_time "
             "FROM charac_info WHERE m_id=%u AND delete_flag=0 ORDER BY charac_no LIMIT %d",
             m_id, max_count);

    if (mysql_query(g_orm_db_charac, query)) {
        printf("[ORM] Character query failed: %s\n", mysql_error(g_orm_db_charac));
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_orm_db_charac);
    if (!res) return -1;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) && count < max_count) {
        ORM_CharacInfo* c = &chars[count];
        memset(c, 0, sizeof(ORM_CharacInfo));

        c->charac_no = row[0] ? (uint32_t)atoi(row[0]) : 0;
        c->m_id = row[1] ? (uint32_t)atoi(row[1]) : 0;
        if (row[2]) strncpy(c->charac_name, row[2], sizeof(c->charac_name) - 1);
        c->village = row[3] ? (int8_t)atoi(row[3]) : 0;
        c->sex = row[4] ? (int8_t)atoi(row[4]) : 0;
        c->job = row[5] ? (int8_t)atoi(row[5]) : 0;
        c->lev = row[6] ? (uint8_t)atoi(row[6]) : 1;
        c->exp = row[7] ? (uint32_t)atoi(row[7]) : 0;
        c->grow_type = row[8] ? (uint8_t)atoi(row[8]) : 0;
        c->expert_job = row[9] ? (int8_t)atoi(row[9]) : 0;
        c->HP = row[10] ? (uint32_t)atoi(row[10]) : 100;
        c->maxHP = row[11] ? (uint32_t)atoi(row[11]) : 100;
        c->maxMP = row[12] ? (uint32_t)atoi(row[12]) : 100;
        c->phy_attack = row[13] ? (uint32_t)atoi(row[13]) : 0;
        c->phy_defense = row[14] ? (uint32_t)atoi(row[14]) : 0;
        c->mag_attack = row[15] ? (uint32_t)atoi(row[15]) : 0;
        c->mag_defense = row[16] ? (uint32_t)atoi(row[16]) : 0;
        c->move_speed = row[17] ? (int32_t)atoi(row[17]) : 100;
        c->attack_speed = row[18] ? (int32_t)atoi(row[18]) : 100;
        c->cast_speed = row[19] ? (int32_t)atoi(row[19]) : 100;
        c->fatigue = row[20] ? (int32_t)atoi(row[20]) : 0;
        c->max_fatigue = row[21] ? (int32_t)atoi(row[21]) : 156;
        c->guild_id = row[22] ? (uint32_t)atoi(row[22]) : 0;
        c->guild_right = row[23] ? (int8_t)atoi(row[23]) : 0;
        if (row[24]) strncpy(c->create_time, row[24], sizeof(c->create_time) - 1);
        if (row[25]) strncpy(c->last_play_time, row[25], sizeof(c->last_play_time) - 1);
        if (row[26]) strncpy(c->delete_time, row[26], sizeof(c->delete_time) - 1);

        count++;
    }

    mysql_free_result(res);
    return count;
}

int orm_get_character(uint32_t charac_no, ORM_CharacInfo* info) {
    if (!g_orm_db_charac || !info) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT charac_no, m_id, charac_name, village, sex, job, lev, exp, "
             "grow_type, expert_job, HP, maxHP, maxMP, phy_attack, phy_defense, "
             "mag_attack, mag_defense, move_speed, attack_speed, cast_speed, "
             "fatigue, max_fatigue, guild_id, guild_right, create_time, last_play_time, delete_time "
             "FROM charac_info WHERE charac_no=%u LIMIT 1", charac_no);

    if (mysql_query(g_orm_db_charac, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_orm_db_charac);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        return -1;
    }

    memset(info, 0, sizeof(ORM_CharacInfo));
    info->charac_no = row[0] ? (uint32_t)atoi(row[0]) : 0;
    info->m_id = row[1] ? (uint32_t)atoi(row[1]) : 0;
    if (row[2]) strncpy(info->charac_name, row[2], sizeof(info->charac_name) - 1);
    info->village = row[3] ? (int8_t)atoi(row[3]) : 0;
    info->sex = row[4] ? (int8_t)atoi(row[4]) : 0;
    info->job = row[5] ? (int8_t)atoi(row[5]) : 0;
    info->lev = row[6] ? (uint8_t)atoi(row[6]) : 1;
    info->exp = row[7] ? (uint32_t)atoi(row[7]) : 0;
    info->grow_type = row[8] ? (uint8_t)atoi(row[8]) : 0;
    info->expert_job = row[9] ? (int8_t)atoi(row[9]) : 0;
    info->HP = row[10] ? (uint32_t)atoi(row[10]) : 100;
    info->maxHP = row[11] ? (uint32_t)atoi(row[11]) : 100;
    info->maxMP = row[12] ? (uint32_t)atoi(row[12]) : 100;
    info->phy_attack = row[13] ? (uint32_t)atoi(row[13]) : 0;
    info->phy_defense = row[14] ? (uint32_t)atoi(row[14]) : 0;
    info->mag_attack = row[15] ? (uint32_t)atoi(row[15]) : 0;
    info->mag_defense = row[16] ? (uint32_t)atoi(row[16]) : 0;
    info->move_speed = row[17] ? (int32_t)atoi(row[17]) : 100;
    info->attack_speed = row[18] ? (int32_t)atoi(row[18]) : 100;
    info->cast_speed = row[19] ? (int32_t)atoi(row[19]) : 100;
    info->fatigue = row[20] ? (int32_t)atoi(row[20]) : 0;
    info->max_fatigue = row[21] ? (int32_t)atoi(row[21]) : 156;
    info->guild_id = row[22] ? (uint32_t)atoi(row[22]) : 0;
    info->guild_right = row[23] ? (int8_t)atoi(row[23]) : 0;
    if (row[24]) strncpy(info->create_time, row[24], sizeof(info->create_time) - 1);
    if (row[25]) strncpy(info->last_play_time, row[25], sizeof(info->last_play_time) - 1);
    if (row[26]) strncpy(info->delete_time, row[26], sizeof(info->delete_time) - 1);

    mysql_free_result(res);
    return 0;
}

int orm_get_character_by_name(const char* charac_name, ORM_CharacInfo* info) {
    if (!g_orm_db_charac || !charac_name || !info) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT charac_no FROM charac_info WHERE charac_name='%s' AND delete_flag=0 LIMIT 1",
             charac_name);

    if (mysql_query(g_orm_db_charac, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_orm_db_charac);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row || !row[0]) {
        mysql_free_result(res);
        return -1;
    }

    uint32_t charac_no = (uint32_t)atoi(row[0]);
    mysql_free_result(res);

    return orm_get_character(charac_no, info);
}

int orm_update_character_level(uint32_t charac_no, uint8_t lev, uint32_t exp) {
    if (!g_orm_db_charac) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_info SET lev=%u, exp=%u WHERE charac_no=%u",
             lev, exp, charac_no);

    if (mysql_query(g_orm_db_charac, query)) {
        printf("[ORM] Update level failed: %s\n", mysql_error(g_orm_db_charac));
        return -1;
    }

    return (mysql_affected_rows(g_orm_db_charac) > 0) ? 0 : -1;
}

int orm_update_character_hp_mp(uint32_t charac_no, uint32_t HP, uint32_t maxHP, uint32_t maxMP) {
    if (!g_orm_db_charac) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_info SET HP=%u, maxHP=%u, maxMP=%u WHERE charac_no=%u",
             HP, maxHP, maxMP, charac_no);

    if (mysql_query(g_orm_db_charac, query)) {
        return -1;
    }

    return 0;
}

int orm_update_character_guild(uint32_t charac_no, uint32_t guild_id, int8_t guild_right) {
    if (!g_orm_db_charac) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_info SET guild_id=%u, guild_right=%d WHERE charac_no=%u",
             guild_id, guild_right, charac_no);

    if (mysql_query(g_orm_db_charac, query)) {
        return -1;
    }

    return 0;
}

int orm_delete_character(uint32_t charac_no) {
    if (!g_orm_db_charac) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_info SET delete_flag=1, delete_time=NOW() WHERE charac_no=%u",
             charac_no);

    if (mysql_query(g_orm_db_charac, query)) {
        return -1;
    }

    return 0;
}

int orm_get_character_count(uint32_t m_id) {
    if (!g_orm_db_charac) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM charac_info WHERE m_id=%u AND delete_flag=0", m_id);

    if (mysql_query(g_orm_db_charac, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_orm_db_charac);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    int count = (row && row[0]) ? atoi(row[0]) : 0;

    mysql_free_result(res);
    return count;
}

/*==============================================================================
 * Character Stats Functions
 *============================================================================*/

int orm_get_charac_stat(uint32_t charac_no, ORM_CharacStat* stat) {
    if (!g_orm_db_charac || !stat) return -1;

    memset(stat, 0, sizeof(ORM_CharacStat));
    stat->charac_no = charac_no;
    stat->pvp_point = 1000;  /* Default */

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT pvp_point, pvp_win, pvp_lose FROM charac_stat WHERE charac_no=%u LIMIT 1",
             charac_no);

    if (mysql_query(g_orm_db_charac, query)) {
        return 0;  /* Return defaults if table doesn't exist */
    }

    MYSQL_RES* res = mysql_store_result(g_orm_db_charac);
    if (!res) return 0;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        stat->pvp_point = row[0] ? (uint32_t)atoi(row[0]) : 1000;
        stat->pvp_win = row[1] ? (uint32_t)atoi(row[1]) : 0;
        stat->pvp_lose = row[2] ? (uint32_t)atoi(row[2]) : 0;
    }

    mysql_free_result(res);
    return 0;
}

int orm_update_charac_stat(uint32_t charac_no, uint32_t pvp_point, uint32_t pvp_win, uint32_t pvp_lose) {
    if (!g_orm_db_charac) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO charac_stat (charac_no, pvp_point, pvp_win, pvp_lose) "
             "VALUES (%u, %u, %u, %u) "
             "ON DUPLICATE KEY UPDATE pvp_point=%u, pvp_win=%u, pvp_lose=%u",
             charac_no, pvp_point, pvp_win, pvp_lose,
             pvp_point, pvp_win, pvp_lose);

    if (mysql_query(g_orm_db_charac, query)) {
        printf("[ORM] Update charac_stat failed: %s\n", mysql_error(g_orm_db_charac));
        return -1;
    }

    return 0;
}

/*==============================================================================
 * Inventory Functions
 *============================================================================*/

int orm_get_inventory_gold(uint32_t charac_no, uint32_t* gold) {
    if (!g_orm_db_charac2 || !gold) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT money FROM inventory WHERE charac_no=%u LIMIT 1", charac_no);

    if (mysql_query(g_orm_db_charac2, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_orm_db_charac2);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    *gold = (row && row[0]) ? (uint32_t)atoi(row[0]) : 0;

    mysql_free_result(res);
    return 0;
}

int orm_update_inventory_gold(uint32_t charac_no, uint32_t gold) {
    if (!g_orm_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE inventory SET money=%u WHERE charac_no=%u", gold, charac_no);

    if (mysql_query(g_orm_db_charac2, query)) {
        return -1;
    }

    return 0;
}

int orm_get_user_items(uint32_t charac_no, ORM_UserItem* items, int max_count) {
    if (!g_orm_db_charac2 || !items) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT ui_id, charac_no, slot, it_id, expire_date, stat, item_lock_key, hidden_option "
             "FROM user_items WHERE charac_no=%u ORDER BY slot LIMIT %d",
             charac_no, max_count);

    if (mysql_query(g_orm_db_charac2, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_orm_db_charac2);
    if (!res) return -1;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) && count < max_count) {
        ORM_UserItem* item = &items[count];
        memset(item, 0, sizeof(ORM_UserItem));

        item->ui_id = row[0] ? (uint32_t)atoi(row[0]) : 0;
        item->charac_no = row[1] ? (uint32_t)atoi(row[1]) : 0;
        item->slot = row[2] ? (int32_t)atoi(row[2]) : 0;
        item->it_id = row[3] ? (uint32_t)atoi(row[3]) : 0;
        if (row[4]) strncpy(item->expire_date, row[4], sizeof(item->expire_date) - 1);
        item->stat = row[5] ? (int8_t)atoi(row[5]) : 0;
        item->item_lock_key = row[6] ? (int8_t)atoi(row[6]) : 0;
        item->hidden_option = row[7] ? (int16_t)atoi(row[7]) : 0;

        count++;
    }

    mysql_free_result(res);
    return count;
}

int orm_get_item_at_slot(uint32_t charac_no, int slot, ORM_UserItem* item) {
    if (!g_orm_db_charac2 || !item) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT ui_id, charac_no, slot, it_id, expire_date, stat, item_lock_key, hidden_option "
             "FROM user_items WHERE charac_no=%u AND slot=%d LIMIT 1",
             charac_no, slot);

    if (mysql_query(g_orm_db_charac2, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_orm_db_charac2);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        return -1;
    }

    memset(item, 0, sizeof(ORM_UserItem));
    item->ui_id = row[0] ? (uint32_t)atoi(row[0]) : 0;
    item->charac_no = row[1] ? (uint32_t)atoi(row[1]) : 0;
    item->slot = row[2] ? (int32_t)atoi(row[2]) : 0;
    item->it_id = row[3] ? (uint32_t)atoi(row[3]) : 0;
    if (row[4]) strncpy(item->expire_date, row[4], sizeof(item->expire_date) - 1);
    item->stat = row[5] ? (int8_t)atoi(row[5]) : 0;
    item->item_lock_key = row[6] ? (int8_t)atoi(row[6]) : 0;
    item->hidden_option = row[7] ? (int16_t)atoi(row[7]) : 0;

    mysql_free_result(res);
    return 0;
}

int orm_add_item(uint32_t charac_no, uint32_t it_id, int slot, int8_t stat) {
    if (!g_orm_db_charac2) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO user_items (charac_no, slot, it_id, stat, reg_date) "
             "VALUES (%u, %d, %u, %d, NOW())",
             charac_no, slot, it_id, stat);

    if (mysql_query(g_orm_db_charac2, query)) {
        printf("[ORM] Add item failed: %s\n", mysql_error(g_orm_db_charac2));
        return -1;
    }

    return (int)mysql_insert_id(g_orm_db_charac2);
}

int orm_delete_item(uint32_t ui_id) {
    if (!g_orm_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query), "DELETE FROM user_items WHERE ui_id=%u", ui_id);

    if (mysql_query(g_orm_db_charac2, query)) {
        return -1;
    }

    return 0;
}

int orm_move_item(uint32_t ui_id, int new_slot) {
    if (!g_orm_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE user_items SET slot=%d WHERE ui_id=%u", new_slot, ui_id);

    if (mysql_query(g_orm_db_charac2, query)) {
        return -1;
    }

    return 0;
}

int orm_find_free_slot(uint32_t charac_no, int start_slot) {
    if (!g_orm_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT slot FROM user_items WHERE charac_no=%u AND slot >= %d ORDER BY slot",
             charac_no, start_slot);

    if (mysql_query(g_orm_db_charac2, query)) {
        return start_slot;  /* Assume free on error */
    }

    MYSQL_RES* res = mysql_store_result(g_orm_db_charac2);
    if (!res) return start_slot;

    int expected_slot = start_slot;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        int slot = atoi(row[0]);
        if (slot != expected_slot) {
            mysql_free_result(res);
            return expected_slot;
        }
        expected_slot++;
    }

    mysql_free_result(res);
    return expected_slot;
}

/*==============================================================================
 * Guild Functions
 *============================================================================*/

int orm_get_guild(uint32_t guild_id, ORM_GuildInfo* info) {
    if (!g_orm_db_guild || !info) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT guild_id, server_id, guild_name, master_id, master_no, master_name, "
             "lev, member_count, guild_point, guild_exp "
             "FROM guild_info WHERE guild_id=%u LIMIT 1", guild_id);

    if (mysql_query(g_orm_db_guild, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_orm_db_guild);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        return -1;
    }

    memset(info, 0, sizeof(ORM_GuildInfo));
    info->guild_id = row[0] ? (uint32_t)atoi(row[0]) : 0;
    info->server_id = row[1] ? (int8_t)atoi(row[1]) : 0;
    if (row[2]) strncpy(info->guild_name, row[2], sizeof(info->guild_name) - 1);
    info->master_id = row[3] ? (uint32_t)atoi(row[3]) : 0;
    info->master_no = row[4] ? (uint32_t)atoi(row[4]) : 0;
    if (row[5]) strncpy(info->master_name, row[5], sizeof(info->master_name) - 1);
    info->lev = row[6] ? (int32_t)atoi(row[6]) : 1;
    info->member_count = row[7] ? (int32_t)atoi(row[7]) : 0;
    info->guild_point = row[8] ? (uint32_t)atoi(row[8]) : 0;
    info->guild_exp = row[9] ? (uint32_t)atoi(row[9]) : 0;

    mysql_free_result(res);
    return 0;
}

int orm_get_guild_by_name(const char* guild_name, ORM_GuildInfo* info) {
    if (!g_orm_db_guild || !guild_name || !info) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT guild_id FROM guild_info WHERE guild_name='%s' LIMIT 1", guild_name);

    if (mysql_query(g_orm_db_guild, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_orm_db_guild);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row || !row[0]) {
        mysql_free_result(res);
        return -1;
    }

    uint32_t guild_id = (uint32_t)atoi(row[0]);
    mysql_free_result(res);

    return orm_get_guild(guild_id, info);
}

int orm_delete_guild(uint32_t guild_id) {
    if (!g_orm_db_guild) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE guild_info SET expire_flag=1 WHERE guild_id=%u", guild_id);

    if (mysql_query(g_orm_db_guild, query)) {
        return -1;
    }

    return 0;
}

int orm_get_guild_member_count(uint32_t guild_id) {
    if (!g_orm_db_guild) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT member_count FROM guild_info WHERE guild_id=%u LIMIT 1", guild_id);

    if (mysql_query(g_orm_db_guild, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_orm_db_guild);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    int count = (row && row[0]) ? atoi(row[0]) : 0;

    mysql_free_result(res);
    return count;
}

/*==============================================================================
 * Transaction Support
 *============================================================================*/

int orm_begin_transaction(int db_type) {
    MYSQL* db = orm_get_db(db_type);
    if (!db) return -1;

    return mysql_query(db, "START TRANSACTION");
}

int orm_commit_transaction(int db_type) {
    MYSQL* db = orm_get_db(db_type);
    if (!db) return -1;

    return mysql_query(db, "COMMIT");
}

int orm_rollback_transaction(int db_type) {
    MYSQL* db = orm_get_db(db_type);
    if (!db) return -1;

    return mysql_query(db, "ROLLBACK");
}
