#include "handlers/handler_login.h"

#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <mysql/mysql.h>

#include "db/db_characters.h"
#include "db/db_inventory.h"
#include "db/db_skills.h"
#include "db/db_game.h"
#include "game_packet_utils.h"
#include "game_server_context.h"

/* Job base stats for character creation */
typedef struct {
    uint16_t max_hp;
    uint16_t max_mp;
    uint16_t phy_attack;
    uint16_t mag_attack;
    uint16_t phy_defense;
    uint16_t mag_defense;
    uint16_t move_speed;
    uint16_t attack_speed;
    uint16_t cast_speed;
} JobBaseStats;

static const JobBaseStats g_job_base_stats[] = {
    {500, 200, 50, 30, 35, 25, 100, 100, 90},  /* Slayer */
    {450, 180, 55, 25, 30, 20, 105, 110, 85},  /* Fighter */
    {400, 220, 45, 40, 28, 28, 100, 95, 95},   /* Gunner */
    {350, 350, 25, 60, 20, 35, 95, 85, 110},   /* Mage */
    {420, 300, 35, 45, 32, 38, 98, 90, 100},   /* Priest */
    {380, 250, 48, 35, 25, 22, 115, 115, 95},  /* Thief */
};

int handle_check_connection(ClientSession* s, const uint8_t* data, int len) {
    (void)data;
    (void)len;
    std::printf("[GAME] CHECK_CONNECTION from fd=%d\n", s->fd);

    uint8_t response[8];
    uint32_t result = 0;
    uint32_t server_time = htonl((uint32_t)time(NULL));
    std::memcpy(response, &result, 4);
    std::memcpy(response + 4, &server_time, 4);
    return send_response(s, 0, response, 8);
}

int handle_login(ClientSession* s, const uint8_t* data, int len) {
    std::printf("[GAME] LOGIN from fd=%d, data_len=%d\n", s->fd, len);

    uint32_t account_uid = 1;
    if (len >= 4) {
        std::memcpy(&account_uid, data, 4);
        if (account_uid == 0) {
            account_uid = 1;
        }
    }

    std::printf("[GAME] LOGIN: Looking up account UID=%u\n", account_uid);

    DBAccountInfo account;
    if (db_get_account_by_uid(account_uid, &account) < 0) {
        std::printf("[GAME] LOGIN: Account %u not found, using UID=1\n", account_uid);
        account_uid = 1;
        if (db_get_account_by_uid(account_uid, &account) < 0) {
            std::printf("[GAME] LOGIN: Failed to load account\n");
            uint8_t error[4] = {1, 0, 0, 0};
            return send_response(s, 1, error, 4);
        }
    }

    s->state = STATE_AUTHENTICATED;
    s->account_id = account.uid;
    std::strncpy(s->account_name, account.accountname, sizeof(s->account_name) - 1);

    s->character_count = db_get_characters(s->account_id, s->characters, MAX_CHARACTERS);
    if (s->character_count < 0) {
        s->character_count = 0;
    }

    std::printf("[GAME] LOGIN success: account=%s uid=%u chars=%d\n",
                s->account_name, s->account_id, s->character_count);

    uint8_t response[1024];
    int response_len = build_userinfo_packet(response, sizeof(response), s);
    if (response_len > 0) {
        std::printf("[GAME] Sending USERINFO (%d bytes) with msg_no=2\n", response_len);
        return send_response(s, 2, response, response_len);
    }

    uint8_t error[4] = {1, 0, 0, 0};
    return send_response(s, 2, error, 4);
}

int handle_set_udp(ClientSession* s, const uint8_t* data, int len) {
    (void)data;
    (void)len;
    std::printf("[GAME] SET_UDP_IP_PORT from fd=%d\n", s->fd);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 11, response, 4);
}

int handle_exit(ClientSession* s, const uint8_t* data, int len) {
    (void)data;
    (void)len;
    std::printf("[GAME] EXIT from fd=%d\n", s->fd);

    s->state = STATE_DISCONNECTING;
    uint8_t response[4] = {0, 0, 0, 0};
    send_response(s, 3, response, 4);
    return -1;
}

int handle_select_character(ClientSession* s, const uint8_t* data, int len) {
    std::printf("[GAME] SELECT_CHARACTER from fd=%d\n", s->fd);

    if (s->state < STATE_AUTHENTICATED) {
        std::printf("[GAME] SELECT_CHARACTER: not authenticated\n");
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 4, error, 4);
    }

    if (len < 4) {
        std::printf("[GAME] SELECT_CHARACTER: packet too short\n");
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 4, error, 4);
    }

    uint32_t char_id;
    std::memcpy(&char_id, data, 4);
    std::printf("[GAME] SELECT_CHARACTER: id=%u (chars loaded: %d)\n", char_id, s->character_count);

    DBCharacterInfo* selected = NULL;
    if (char_id < (uint32_t)s->character_count) {
        selected = &s->characters[char_id];
        std::printf("[GAME] SELECT_CHARACTER: found by slot index %u\n", char_id);
    }

    if (!selected) {
        for (int i = 0; i < s->character_count; i++) {
            if (s->characters[i].charac_no == char_id) {
                selected = &s->characters[i];
                std::printf("[GAME] SELECT_CHARACTER: found by charac_no %u\n", char_id);
                break;
            }
        }
    }

    if (!selected && char_id > 0) {
        std::printf("[GAME] SELECT_CHARACTER: trying DB lookup for charac_no=%u\n", char_id);
        DBCharacterInfo ch;
        if (db_get_character(char_id, &ch) == 0 && ch.charac_no > 0) {
            if (s->character_count < MAX_CHARACTERS) {
                s->characters[s->character_count] = ch;
                selected = &s->characters[s->character_count];
                s->character_count++;
            }
        }
    }

    if (!selected) {
        std::printf("[GAME] SELECT_CHARACTER: character not found\n");
        uint8_t error[4] = {4, 0, 0, 0};
        return send_response(s, 4, error, 4);
    }

    s->state = STATE_IN_GAME;
    s->character_id = selected->charac_no;
    std::strncpy(s->character_name, selected->charac_name, sizeof(s->character_name) - 1);

    if (db_get_inventory(selected->charac_no, &s->inventory) < 0) {
        std::printf("[GAME] SELECT_CHARACTER: inventory load failed, using defaults\n");
        s->inventory.charac_no = selected->charac_no;
        s->inventory.money = 0;
        s->inventory.inventory_capacity = 56;
        s->inventory.inventory_loaded = 0;
    }

    if (db_get_skill(selected->charac_no, &s->skill) < 0) {
        std::printf("[GAME] SELECT_CHARACTER: skill load failed, using defaults\n");
        s->skill.charac_no = selected->charac_no;
        s->skill.remain_sp = 0;
        s->skill.skill_loaded = 0;
    }

    if (db_get_equipment(selected->charac_no, &s->equipment) < 0) {
        std::printf("[GAME] SELECT_CHARACTER: equipment load failed, using defaults\n");
        s->equipment.charac_no = selected->charac_no;
        s->equipment.equip_count = 0;
        s->equipment.equip_loaded = 0;
    }

    std::printf("[GAME] SELECT_CHARACTER: selected '%s' charac_no=%u (job=%d, level=%d, gold=%u, sp=%u, equips=%d)\n",
               selected->charac_name, selected->charac_no, selected->job, selected->level,
               s->inventory.money, s->skill.remain_sp, s->equipment.equip_count);

    uint8_t response[128];
    int response_len = build_character_status(response, sizeof(response), selected);

    if (response_len > 0) {
        return send_response(s, 4, response, response_len);
    }

    return 0;
}

int handle_return_select(ClientSession* s, const uint8_t* data, int len) {
    (void)data;
    (void)len;
    std::printf("[GAME] RETURN_SELECT_CHARACTER from fd=%d\n", s->fd);

    s->state = STATE_CHAR_SELECT;
    s->character_id = 0;
    std::memset(s->character_name, 0, sizeof(s->character_name));

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 7, response, 4);
}

int handle_get_userinfo(ClientSession* s, const uint8_t* data, int len) {
    (void)data;
    (void)len;
    std::printf("[GAME] GET_USERINFO from fd=%d\n", s->fd);

    if (s->state < STATE_AUTHENTICATED) {
        uint8_t error[4] = {3, 0, 0, 0};
        return send_response(s, 8, error, 4);
    }

    uint8_t response[1024];
    int response_len = build_userinfo_packet(response, sizeof(response), s);
    if (response_len > 0) {
        return send_response(s, 8, response, response_len);
    }

    return 0;
}

int handle_create_character(ClientSession* s, const uint8_t* data, int len) {
    std::printf("[GAME] CREATE_CHARACTER from fd=%d\n", s->fd);

    if (s->state < STATE_AUTHENTICATED) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 5, error, 4);
    }

    if (len < 24) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 5, error, 4);
    }

    char charac_name[MAX_CHARAC_NAME];
    std::memset(charac_name, 0, sizeof(charac_name));
    std::memcpy(charac_name, data, 16);
    uint8_t job = data[16];
    uint8_t grow_type = data[17];
    uint8_t sex = data[18];

    if (job > 5) {
        job = 0;
    }

    std::printf("[GAME] CREATE_CHARACTER: name=%s job=%d grow=%d sex=%d\n",
               charac_name, job, grow_type, sex);

    const JobBaseStats* stats = &g_job_base_stats[job];

    if (g_db_charac) {
        char check_query[256];
        char escaped_name[MAX_CHARAC_NAME * 2 + 1];
        mysql_real_escape_string(g_db_charac, escaped_name, charac_name, std::strlen(charac_name));

        std::snprintf(check_query, sizeof(check_query),
                      "SELECT charac_no FROM charac_info WHERE charac_name='%s'",
                      escaped_name);

        if (mysql_query(g_db_charac, check_query) == 0) {
            MYSQL_RES* check_res = mysql_store_result(g_db_charac);
            if (check_res) {
                if (mysql_num_rows(check_res) > 0) {
                    mysql_free_result(check_res);
                    std::printf("[GAME] Name already exists: %s\n", charac_name);
                    uint8_t error[4] = {3, 0, 0, 0};
                    return send_response(s, 5, error, 4);
                }
                mysql_free_result(check_res);
            }
        }
    }

    uint32_t charac_no = 0;
    if (g_db_charac) {
        char query[1024];
        char escaped_name[MAX_CHARAC_NAME * 2 + 1];
        mysql_real_escape_string(g_db_charac, escaped_name, charac_name, std::strlen(charac_name));

        std::snprintf(query, sizeof(query),
            "INSERT INTO charac_info (m_id, charac_name, village, job, lev, exp, grow_type, "
            "HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
            "move_speed, attack_speed, cast_speed, max_fatigue, sex) VALUES "
            "(%u, '%s', 1, %d, 1, 0, %d, 100, %d, %d, %d, %d, %d, %d, %d, %d, %d, 70, %d)",
            s->account_id, escaped_name, job, grow_type,
            stats->max_hp, stats->max_mp,
            stats->phy_attack, stats->phy_defense,
            stats->mag_attack, stats->mag_defense,
            stats->move_speed, stats->attack_speed, stats->cast_speed,
            sex);

        if (mysql_query(g_db_charac, query)) {
            std::printf("[DB] Insert character failed: %s\n", mysql_error(g_db_charac));
            uint8_t error[4] = {4, 0, 0, 0};
            return send_response(s, 5, error, 4);
        }

        charac_no = (uint32_t)mysql_insert_id(g_db_charac);
        std::printf("[DB] Created character %s with charac_no=%u\n", charac_name, charac_no);

        std::snprintf(query, sizeof(query),
            "INSERT INTO charac_stat (charac_no, village, exp, HP, fatigue, max_fatigue) "
            "VALUES (%u, 1, 0, 100, 0, 70)", charac_no);
        mysql_query(g_db_charac, query);

        if (g_db_charac2) {
            std::snprintf(query, sizeof(query),
                "INSERT INTO inventory (charac_no, money, slot) VALUES (%u, 10000, 0)",
                charac_no);
            mysql_query(g_db_charac2, query);

            uint32_t weapon_id = 100001 + job * 100;
            std::snprintf(query, sizeof(query),
                "INSERT INTO user_items (charac_no, slot, it_id, stat, reg_date, m_time) "
                "VALUES (%u, 0, %u, 0, NOW(), NOW())",
                charac_no, weapon_id);
            mysql_query(g_db_charac2, query);

            std::printf("[DB] Created starter inventory for charac_no=%u\n", charac_no);
        }
    } else {
        charac_no = 90000 + (s->account_id % 1000);
    }

    uint8_t response[16];
    std::memset(response, 0, sizeof(response));
    uint32_t result = 0;
    std::memcpy(response, &result, 4);
    std::memcpy(response + 4, &charac_no, 4);

    std::printf("[GAME] Character created successfully: charac_no=%u\n", charac_no);
    return send_response(s, 5, response, 16);
}

int handle_delete_character(ClientSession* s, const uint8_t* data, int len) {
    std::printf("[GAME] DELETE_CHARACTER from fd=%d\n", s->fd);

    if (s->state < STATE_AUTHENTICATED || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 6, error, 4);
    }

    uint32_t charac_no;
    std::memcpy(&charac_no, data, 4);
    std::printf("[GAME] DELETE_CHARACTER: charac_no=%u\n", charac_no);

    if (db_soft_delete_character(charac_no, s->account_id) < 0) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 6, error, 4);
    }

    std::printf("[GAME] Character %u deleted successfully\n", charac_no);
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 6, response, 4);
}
