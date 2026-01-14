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

#define DB_HOST "127.0.0.1"
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

    mysql_set_character_set(conn, "utf8");
    printf("[ORM] Connected to database: %s\n", db_name);
    return conn;
}

/*==============================================================================
 * Initialization Functions
 *============================================================================*/

int ORM_Initialize(const char* config_file) {
    (void)config_file;

    if (g_orm_initialized) {
        printf("[ORM] Already initialized\n");
        return 0;
    }

    printf("[ORM] Initializing ORM layer...\n");

    g_orm_db_account = orm_connect_db(DB_HOST, DB_PORT, DB_USER, DB_PASS, DB_ACCOUNT);
    if (!g_orm_db_account) goto error;

    g_orm_db_charac = orm_connect_db(DB_HOST, DB_PORT, DB_USER, DB_PASS, DB_CHARAC);
    if (!g_orm_db_charac) goto error;

    g_orm_db_charac2 = orm_connect_db(DB_HOST, DB_PORT, DB_USER, DB_PASS, DB_CHARAC2);
    if (!g_orm_db_charac2) goto error;

    g_orm_db_guild = orm_connect_db(DB_HOST, DB_PORT, DB_USER, DB_PASS, DB_GUILD);
    if (!g_orm_db_guild) goto error;

    g_orm_db_login = orm_connect_db(DB_HOST, DB_PORT, DB_USER, DB_PASS, DB_LOGIN);

    g_orm_initialized = 1;
    printf("[ORM] ORM layer initialized successfully\n");
    return 0;

error:
    ORM_Cleanup();
    return -1;
}

void ORM_Cleanup(void) {
    printf("[ORM] Cleaning up ORM layer...\n");

    if (g_orm_db_account) { mysql_close(g_orm_db_account); g_orm_db_account = NULL; }
    if (g_orm_db_charac)  { mysql_close(g_orm_db_charac);  g_orm_db_charac = NULL;  }
    if (g_orm_db_charac2) { mysql_close(g_orm_db_charac2); g_orm_db_charac2 = NULL; }
    if (g_orm_db_login)   { mysql_close(g_orm_db_login);   g_orm_db_login = NULL;   }
    if (g_orm_db_guild)   { mysql_close(g_orm_db_guild);   g_orm_db_guild = NULL;   }

    g_orm_initialized = 0;
}

int ORM_IsInitialized(void) {
    return g_orm_initialized;
}

/*==============================================================================
 * Account Functions
 *============================================================================*/

int ORM_Account_GetByUsername(const char* username, ORM_Account* account) {
    if (!g_orm_db_account || !username || !account) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT UID, m_id, m_passwd, m_isuse FROM accounts WHERE m_id='%s' LIMIT 1",
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

    memset(account, 0, sizeof(ORM_Account));
    account->uid = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(account->m_id, row[1], sizeof(account->m_id) - 1);
    if (row[2]) strncpy(account->m_passwd, row[2], sizeof(account->m_passwd) - 1);
    account->m_isuse = row[3] ? atoi(row[3]) : 0;

    mysql_free_result(res);
    return 0;
}

int ORM_Account_VerifyPassword(const char* username, const char* password) {
    if (!username || !password) return -1;

    ORM_Account account;
    if (ORM_Account_GetByUsername(username, &account) < 0) return -1;

    return (strcmp(account.m_passwd, password) == 0) ? 0 : -1;
}

/*==============================================================================
 * Character Functions
 *============================================================================*/

int ORM_CharacInfo_GetByNo(int charac_no, ORM_CharacInfo* info) {
    if (!g_orm_db_charac || !info) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT charac_no, m_id, charac_name, village, sex, job, lev, exp, "
             "grow_type, expert_job, HP, maxHP, maxMP, phy_attack, phy_defense, "
             "mag_attack, mag_defense, move_speed, attack_speed, cast_speed, "
             "fatigue, max_fatigue, guild_id, guild_right "
             "FROM charac_info WHERE charac_no=%d LIMIT 1", charac_no);

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
    info->charac_no = row[0] ? atoi(row[0]) : 0;
    info->m_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(info->charac_name, row[2], sizeof(info->charac_name) - 1);
    info->village = row[3] ? (int8_t)atoi(row[3]) : 0;
    info->sex = row[4] ? (int8_t)atoi(row[4]) : 0;
    info->job = row[5] ? (int8_t)atoi(row[5]) : 0;
    info->lev = row[6] ? (uint8_t)atoi(row[6]) : 1;
    info->exp = row[7] ? atoi(row[7]) : 0;
    info->grow_type = row[8] ? (uint8_t)atoi(row[8]) : 0;
    info->expert_job = row[9] ? (int8_t)atoi(row[9]) : 0;
    info->HP = row[10] ? atoi(row[10]) : 100;
    info->maxHP = row[11] ? atoi(row[11]) : 100;
    info->maxMP = row[12] ? atoi(row[12]) : 100;
    info->phy_attack = row[13] ? atoi(row[13]) : 0;
    info->phy_defense = row[14] ? atoi(row[14]) : 0;
    info->mag_attack = row[15] ? atoi(row[15]) : 0;
    info->mag_defense = row[16] ? atoi(row[16]) : 0;
    info->move_speed = row[17] ? atoi(row[17]) : 100;
    info->attack_speed = row[18] ? atoi(row[18]) : 100;
    info->cast_speed = row[19] ? atoi(row[19]) : 100;
    info->fatigue = row[20] ? atoi(row[20]) : 0;
    info->max_fatigue = row[21] ? atoi(row[21]) : 156;
    info->guild_id = row[22] ? atoi(row[22]) : 0;
    info->guild_right = row[23] ? (int8_t)atoi(row[23]) : 0;

    mysql_free_result(res);
    return 0;
}

int ORM_CharacInfo_GetByMemberId(int m_id, ORM_CharacInfo* results, int max_count) {
    if (!g_orm_db_charac || !results) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT charac_no, m_id, charac_name, village, sex, job, lev, exp, "
             "grow_type, expert_job, HP, maxHP, maxMP, phy_attack, phy_defense, "
             "mag_attack, mag_defense, move_speed, attack_speed, cast_speed, "
             "fatigue, max_fatigue, guild_id, guild_right "
             "FROM charac_info WHERE m_id=%d AND delete_flag=0 ORDER BY charac_no LIMIT %d",
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
        ORM_CharacInfo* c = &results[count];
        memset(c, 0, sizeof(ORM_CharacInfo));

        c->charac_no = row[0] ? atoi(row[0]) : 0;
        c->m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(c->charac_name, row[2], sizeof(c->charac_name) - 1);
        c->village = row[3] ? (int8_t)atoi(row[3]) : 0;
        c->sex = row[4] ? (int8_t)atoi(row[4]) : 0;
        c->job = row[5] ? (int8_t)atoi(row[5]) : 0;
        c->lev = row[6] ? (uint8_t)atoi(row[6]) : 1;
        c->exp = row[7] ? atoi(row[7]) : 0;
        c->grow_type = row[8] ? (uint8_t)atoi(row[8]) : 0;
        c->expert_job = row[9] ? (int8_t)atoi(row[9]) : 0;
        c->HP = row[10] ? atoi(row[10]) : 100;
        c->maxHP = row[11] ? atoi(row[11]) : 100;
        c->maxMP = row[12] ? atoi(row[12]) : 100;
        c->phy_attack = row[13] ? atoi(row[13]) : 0;
        c->phy_defense = row[14] ? atoi(row[14]) : 0;
        c->mag_attack = row[15] ? atoi(row[15]) : 0;
        c->mag_defense = row[16] ? atoi(row[16]) : 0;
        c->move_speed = row[17] ? atoi(row[17]) : 100;
        c->attack_speed = row[18] ? atoi(row[18]) : 100;
        c->cast_speed = row[19] ? atoi(row[19]) : 100;
        c->fatigue = row[20] ? atoi(row[20]) : 0;
        c->max_fatigue = row[21] ? atoi(row[21]) : 156;
        c->guild_id = row[22] ? atoi(row[22]) : 0;
        c->guild_right = row[23] ? (int8_t)atoi(row[23]) : 0;

        count++;
    }

    mysql_free_result(res);
    return count;
}

int ORM_CharacInfo_UpdateLevelExp(int charac_no, uint8_t lev, int exp) {
    if (!g_orm_db_charac) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_info SET lev=%u, exp=%d WHERE charac_no=%d",
             lev, exp, charac_no);

    if (mysql_query(g_orm_db_charac, query)) {
        printf("[ORM] Update level failed: %s\n", mysql_error(g_orm_db_charac));
        return -1;
    }

    return (mysql_affected_rows(g_orm_db_charac) > 0) ? 0 : -1;
}

int ORM_CharacInfo_UpdateCombatStats(int charac_no, int HP, int maxHP, int maxMP) {
    if (!g_orm_db_charac) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_info SET HP=%d, maxHP=%d, maxMP=%d WHERE charac_no=%d",
             HP, maxHP, maxMP, charac_no);

    if (mysql_query(g_orm_db_charac, query)) {
        return -1;
    }

    return 0;
}

int ORM_CharacInfo_UpdateGuild(int charac_no, int guild_id, int8_t guild_right) {
    if (!g_orm_db_charac) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_info SET guild_id=%d, guild_right=%d WHERE charac_no=%d",
             guild_id, guild_right, charac_no);

    if (mysql_query(g_orm_db_charac, query)) {
        return -1;
    }

    return 0;
}

/*==============================================================================
 * Inventory Functions
 *============================================================================*/

int ORM_Inventory_GetItems(int charac_no, ORM_UserItem* items, int max_count) {
    if (!g_orm_db_charac2 || !items) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT ui_id, charac_no, it_id, add_info, slot, stat "
             "FROM user_items WHERE charac_no=%d ORDER BY slot LIMIT %d",
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

        item->ui_id = row[0] ? atoi(row[0]) : 0;
        item->charac_no = row[1] ? atoi(row[1]) : 0;
        item->it_id = row[2] ? atoi(row[2]) : 0;
        item->add_info = row[3] ? atoi(row[3]) : 0;
        item->slot = row[4] ? atoi(row[4]) : 0;
        item->upgrade = row[5] ? atoi(row[5]) : 0;
        item->count = 1;

        count++;
    }

    mysql_free_result(res);
    return count;
}

int ORM_Inventory_AddItem(int charac_no, int it_id, int count, int slot) {
    if (!g_orm_db_charac2) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO user_items (charac_no, slot, it_id, stat, reg_date) "
             "VALUES (%d, %d, %d, 0, NOW())",
             charac_no, slot, it_id);

    if (mysql_query(g_orm_db_charac2, query)) {
        printf("[ORM] Add item failed: %s\n", mysql_error(g_orm_db_charac2));
        return -1;
    }

    (void)count;
    return (int)mysql_insert_id(g_orm_db_charac2);
}

int ORM_Inventory_RemoveItem(int ui_id) {
    if (!g_orm_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query), "DELETE FROM user_items WHERE ui_id=%d", ui_id);

    if (mysql_query(g_orm_db_charac2, query)) {
        return -1;
    }

    return 0;
}

int ORM_Inventory_UpdateCount(int ui_id, int count) {
    if (!g_orm_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE user_items SET add_info=%d WHERE ui_id=%d", count, ui_id);

    if (mysql_query(g_orm_db_charac2, query)) {
        return -1;
    }

    return 0;
}

int ORM_Inventory_MoveItem(int ui_id, int new_slot) {
    if (!g_orm_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE user_items SET slot=%d WHERE ui_id=%d", new_slot, ui_id);

    if (mysql_query(g_orm_db_charac2, query)) {
        return -1;
    }

    return 0;
}

/*==============================================================================
 * Guild Functions
 *============================================================================*/

int ORM_Guild_GetInfo(int guild_id, ORM_GuildInfo* info) {
    if (!g_orm_db_guild || !info) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT guild_id, guild_name, lev, guild_exp, master_id, member_count "
             "FROM guild_info WHERE guild_id=%d LIMIT 1", guild_id);

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
    info->guild_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(info->guild_name, row[1], sizeof(info->guild_name) - 1);
    info->guild_lev = row[2] ? atoi(row[2]) : 1;
    info->guild_exp = row[3] ? atoi(row[3]) : 0;
    info->guild_master = row[4] ? atoi(row[4]) : 0;
    info->member_count = row[5] ? atoi(row[5]) : 0;
    info->max_member = 20 + (info->guild_lev * 2);

    mysql_free_result(res);
    return 0;
}

int ORM_Guild_GetMembers(int guild_id, int* member_ids, int max_count) {
    if (!g_orm_db_charac || !member_ids) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT charac_no FROM charac_info WHERE guild_id=%d AND delete_flag=0 LIMIT %d",
             guild_id, max_count);

    if (mysql_query(g_orm_db_charac, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_orm_db_charac);
    if (!res) return -1;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) && count < max_count) {
        member_ids[count] = row[0] ? atoi(row[0]) : 0;
        count++;
    }

    mysql_free_result(res);
    return count;
}

/*==============================================================================
 * Mail Functions
 *============================================================================*/

int ORM_Mail_GetList(int charac_no, ORM_Mail* mails, int max_count) {
    if (!g_orm_db_charac2 || !mails) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT postal_id, send_charac_no, receive_charac_no, send_charac_name, "
             "message, gold, item_id, item_count, read_flag, receive_flag "
             "FROM postal WHERE receive_charac_no=%d AND delete_flag=0 "
             "ORDER BY postal_id DESC LIMIT %d",
             charac_no, max_count);

    if (mysql_query(g_orm_db_charac2, query)) {
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_orm_db_charac2);
    if (!res) return -1;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) && count < max_count) {
        ORM_Mail* mail = &mails[count];
        memset(mail, 0, sizeof(ORM_Mail));

        mail->postal_id = row[0] ? atoi(row[0]) : 0;
        mail->send_charac_no = row[1] ? atoi(row[1]) : 0;
        mail->receive_charac_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(mail->send_charac_name, row[3], sizeof(mail->send_charac_name) - 1);
        if (row[4]) strncpy(mail->message, row[4], sizeof(mail->message) - 1);
        mail->gold = row[5] ? atoi(row[5]) : 0;
        mail->item_id = row[6] ? atoi(row[6]) : 0;
        mail->item_count = row[7] ? atoi(row[7]) : 0;
        mail->read_flag = row[8] ? atoi(row[8]) : 0;
        mail->receive_flag = row[9] ? atoi(row[9]) : 0;

        count++;
    }

    mysql_free_result(res);
    return count;
}

int ORM_Mail_Send(int from_charac, int to_charac, const char* message,
                  int gold, int item_id, int item_count) {
    if (!g_orm_db_charac2 || !message) return -1;

    char safe_msg[512];
    mysql_real_escape_string(g_orm_db_charac2, safe_msg, message, strlen(message));

    char query[1024];
    snprintf(query, sizeof(query),
             "INSERT INTO postal (send_charac_no, receive_charac_no, message, "
             "gold, item_id, item_count, reg_date) "
             "VALUES (%d, %d, '%s', %d, %d, %d, NOW())",
             from_charac, to_charac, safe_msg, gold, item_id, item_count);

    if (mysql_query(g_orm_db_charac2, query)) {
        printf("[ORM] Send mail failed: %s\n", mysql_error(g_orm_db_charac2));
        return -1;
    }

    return (int)mysql_insert_id(g_orm_db_charac2);
}

int ORM_Mail_MarkRead(int postal_id) {
    if (!g_orm_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE postal SET read_flag=1 WHERE postal_id=%d", postal_id);

    if (mysql_query(g_orm_db_charac2, query)) {
        return -1;
    }

    return 0;
}

int ORM_Mail_Delete(int postal_id) {
    if (!g_orm_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE postal SET delete_flag=1 WHERE postal_id=%d", postal_id);

    if (mysql_query(g_orm_db_charac2, query)) {
        return -1;
    }

    return 0;
}
