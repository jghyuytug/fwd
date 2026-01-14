#include "db/db_characters.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "database/headers/db_connection_manager.h"
#include "game_server_context.h"
#include "charac_info_orm.h"
#include "accounts_orm.h"

static int execute_query(const char* query, DBQueryResult* result) {
    if (DBConnectionManager_ExecuteQuery(&g_db_manager, DB_TYPE_CAIN, query, result) < 0) {
        return -1;
    }
    return 0;
}

static int execute_non_query(const char* query) {
    DBQueryResult result;
    std::memset(&result, 0, sizeof(result));
    if (execute_query(query, &result) < 0) {
        return -1;
    }
    int affected = result.row_count;
    DBQueryResult_Free(&result);
    return affected;
}


static void load_equipped_items(DBCharacterInfo* ch) {
    if (!ch) return;

    std::memset(ch->equip_item_ids, 0, sizeof(ch->equip_item_ids));

    char query[256];
    std::snprintf(query, sizeof(query),
                  "SELECT slot, it_id FROM user_items WHERE charac_no=%u AND slot < %d",
                  ch->charac_no, MAX_EQUIP_SLOTS);

    DBQueryResult result;
    std::memset(&result, 0, sizeof(result));

    if (DBConnectionManager_ExecuteQuery(&g_db_manager, DB_TYPE_CAIN, query, &result) < 0) {
        std::printf("[DB][CHAR] Equip query failed for charac_no=%u\n", ch->charac_no);
        return;
    }

    char* values[2] = {0};
    while (DBQueryResult_FetchRow(&result, values) > 0) {
        int slot = values[0] ? std::atoi(values[0]) : -1;
        uint32_t it_id = values[1] ? static_cast<uint32_t>(std::strtoul(values[1], NULL, 10)) : 0;
        if (slot >= 0 && slot < MAX_EQUIP_SLOTS) {
            ch->equip_item_ids[slot] = it_id;
        }
    }

    DBQueryResult_Free(&result);
}

static void map_charac_info(const CharacInfo& src, DBCharacterInfo* dst) {
    if (!dst) return;

    dst->charac_no = static_cast<uint32_t>(src.charac_no);
    std::strncpy(dst->charac_name, src.charac_name, sizeof(dst->charac_name) - 1);
    dst->job = static_cast<uint8_t>(src.job);
    dst->grow_type = static_cast<uint8_t>(src.grow_type);
    dst->level = src.lev ? static_cast<uint8_t>(src.lev) : 1;
    dst->exp = static_cast<uint32_t>(src.exp);
    dst->hp = src.HP > 0 ? static_cast<uint16_t>(src.HP) : 1000;
    dst->max_hp = src.maxHP > 0 ? static_cast<uint16_t>(src.maxHP) : 1000;
    dst->max_mp = src.maxMP > 0 ? static_cast<uint16_t>(src.maxMP) : 500;
    dst->village = src.village;
    dst->fatigue = src.fatigue > 0 ? static_cast<int16_t>(src.fatigue) : 156;
    dst->sex = static_cast<uint8_t>(src.sex);
    dst->deleted = 0;
    dst->guild_id = static_cast<uint32_t>(src.guild_id);
    dst->dungeon_clear_point = 0;
    dst->create_time = 0;
    dst->expert_job = static_cast<uint8_t>(src.expert_job);
    dst->skill_tree_index = -1;
    dst->phy_attack = src.phy_attack > 0 ? static_cast<uint16_t>(src.phy_attack) : 100;
    dst->phy_defense = src.phy_defense > 0 ? static_cast<uint16_t>(src.phy_defense) : 50;
    dst->mag_attack = src.mag_attack > 0 ? static_cast<uint16_t>(src.mag_attack) : 100;
    dst->mag_defense = src.mag_defense > 0 ? static_cast<uint16_t>(src.mag_defense) : 50;
    dst->move_speed = src.move_speed > 0 ? static_cast<uint16_t>(src.move_speed) : 100;
    dst->attack_speed = src.attack_speed > 0 ? static_cast<uint16_t>(src.attack_speed) : 100;
    dst->cast_speed = src.cast_speed > 0 ? static_cast<uint16_t>(src.cast_speed) : 100;
    dst->hit_recovery = 0;
    dst->jump = 100;
    dst->charac_weight = 0;
    dst->max_fatigue = src.max_fatigue > 0 ? static_cast<int16_t>(src.max_fatigue) : 156;
    dst->guild_right = static_cast<uint8_t>(src.guild_right);
    dst->member_flag = 0;
    dst->gold = 0;

    if (dst->max_hp == 0) {
        dst->max_hp = 35200;
    }
    if (dst->hp == 0) {
        dst->hp = dst->max_hp;
    }
    if (dst->fatigue <= 0) {
        dst->fatigue = 156;
    }
    if (dst->max_fatigue <= 0) {
        dst->max_fatigue = 156;
    }
}

int db_get_characters(uint32_t m_id, DBCharacterInfo* chars, int max_count) {
    if (!chars || max_count <= 0) {
        return -1;
    }

    std::vector<CharacInfo> records(max_count);
    int fetched = CharacInfo_GetByMemberId(&g_db_manager, static_cast<int>(m_id),
                                           records.data(), max_count);
    if (fetched < 0) {
        std::printf("[DB][CHAR] CharacInfo_GetByMemberId failed for m_id=%u\n", m_id);
        return -1;
    }

    for (int i = 0; i < fetched; ++i) {
        std::memset(&chars[i], 0, sizeof(DBCharacterInfo));
        map_charac_info(records[i], &chars[i]);
        load_equipped_items(&chars[i]);
    }

    std::printf("[DB] Loaded %d characters for m_id=%u (with equipment)\n", fetched, m_id);
    return fetched;
}

int db_get_account_by_uid(uint32_t uid, DBAccountInfo* account) {
    if (!account) return -1;

    Account orm_account;
    Account_Init(&orm_account);

    if (Account_GetByUID(&g_db_manager, static_cast<int>(uid), &orm_account) < 0) {
        std::printf("[DB][ACCOUNT] Account_GetByUID failed for uid=%u\n", uid);
        return -1;
    }

    std::memset(account, 0, sizeof(DBAccountInfo));
    account->uid = static_cast<uint32_t>(orm_account.UID);
    std::strncpy(account->accountname, orm_account.accountname, sizeof(account->accountname) - 1);
    std::strncpy(account->password, orm_account.password, sizeof(account->password) - 1);
    std::strncpy(account->vip, orm_account.VIP, sizeof(account->vip) - 1);
    return 0;
}

int db_get_character(uint32_t charac_no, DBCharacterInfo* ch) {
    if (!ch) return -1;

    CharacInfo record;
    std::memset(&record, 0, sizeof(record));

    if (CharacInfo_GetByCharacNo(&g_db_manager, static_cast<int>(charac_no), &record) < 0) {
        std::printf("[DB][CHAR] CharacInfo_GetByCharacNo failed for charac_no=%u\n", charac_no);
        return -1;
    }

    std::memset(ch, 0, sizeof(DBCharacterInfo));
    map_charac_info(record, ch);
    load_equipped_items(ch);
    return 0;
}

int db_get_equipment(uint32_t charac_no, DBEquipmentInfo* equip) {
    if (!equip) return -1;

    std::memset(equip, 0, sizeof(DBEquipmentInfo));
    equip->charac_no = charac_no;

    char query[256];
    std::snprintf(query, sizeof(query),
                  "SELECT ui_id, slot, it_id, stat, hidden_option FROM user_items "
                  "WHERE charac_no=%u AND slot < %d ORDER BY slot",
                  charac_no, MAX_EQUIP_SLOTS);

    DBQueryResult result;
    std::memset(&result, 0, sizeof(result));
    if (execute_query(query, &result) < 0) {
        std::printf("[DB][CHAR] Equipment query failed for charac_no=%u\n", charac_no);
        return -1;
    }

    char* values[5] = {0};
    int count = 0;
    while (DBQueryResult_FetchRow(&result, values) == 5 && count < MAX_EQUIP_SLOTS) {
        int slot = values[1] ? std::atoi(values[1]) : 0;
        if (slot >= 0 && slot < MAX_EQUIP_SLOTS) {
            DBItemInfo& info = equip->equipment[slot];
            info.ui_id = values[0] ? static_cast<uint32_t>(strtoul(values[0], NULL, 10)) : 0;
            info.slot = static_cast<uint32_t>(slot);
            info.it_id = values[2] ? static_cast<uint32_t>(strtoul(values[2], NULL, 10)) : 0;
            info.stat = values[3] ? static_cast<uint8_t>(std::atoi(values[3])) : 0;
            info.hidden_option = values[4] ? static_cast<uint16_t>(std::atoi(values[4])) : 0;
            info.endurance = 100;
            count++;
        }
    }

    DBQueryResult_Free(&result);
    equip->equip_count = count;
    equip->equip_loaded = 1;

    std::printf("[DB] Loaded %d equipment items for charac_no=%u\n", count, charac_no);
    return 0;
}

int db_soft_delete_character(uint32_t charac_no, uint32_t m_id) {
    CharacInfo info;
    std::memset(&info, 0, sizeof(info));

    if (CharacInfo_GetByCharacNo(&g_db_manager, static_cast<int>(charac_no), &info) < 0) {
        std::printf("[DB][CHAR] Failed to fetch charac_no=%u for delete\n", charac_no);
        return -1;
    }

    if (info.m_id != static_cast<int>(m_id)) {
        std::printf("[DB][CHAR] Delete rejected: charac_no=%u not owned by m_id=%u\n",
                    charac_no, m_id);
        return -1;
    }

    char query[256];
    std::snprintf(query, sizeof(query),
                  "UPDATE charac_info SET delete_flag=1, delete_time=NOW() "
                  "WHERE charac_no=%u AND m_id=%u",
                  charac_no, m_id);

    int affected = execute_non_query(query);
    return affected > 0 ? 0 : -1;
}

int db_update_character_hp_mp(uint32_t charac_no, uint16_t hp, uint16_t mp) {
    if (CharacInfo_UpdateCombatStats(&g_db_manager, static_cast<int>(charac_no),
                                     static_cast<int>(hp), static_cast<int>(hp),
                                     static_cast<int>(mp)) < 0) {
        std::printf("[DB][CHAR] Update combat stats failed for charac_no=%u\n", charac_no);
        return -1;
    }
    return 0;
}
