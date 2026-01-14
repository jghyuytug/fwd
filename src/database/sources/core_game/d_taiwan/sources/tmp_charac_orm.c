#include "tmp_charac_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192
#define MAX_FIELDS 64

/* ==================== CRUD 操作 ==================== */

int TmpCharac_Add(DBConnectionManager* manager, const TmpCharac* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    /* 由于表无主键，先删除可能存在的记录 */
    snprintf(query, sizeof(query),
        "DELETE FROM tmp_charac WHERE m_id = %u AND charac_no = %u",
        record->m_id, record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }
    DBQueryResult_Free(&result);

    /* 然后插入新记录 (blob字段暂时插入空字符串，实际使用时需要特殊处理) */
    snprintf(query, sizeof(query),
        "INSERT INTO tmp_charac (m_id, charac_no, charac_name, village, job, lev, exp, grow_type, "
        "HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
        "element_resist, spec_property, inven_weight, hp_regen, mp_regen, "
        "move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, "
        "fatigue, max_fatigue, premium_fatigue, max_premium_fatigue, "
        "create_time, last_play_time, dungeon_clear_point, delete_time, delete_flag, "
        "guild_id, guild_right, member_flag) "
        "VALUES (%u, %u, '%s', %u, %u, %u, %u, %u, "
        "%u, %u, %u, %u, %u, %u, %u, "
        "'', '', %u, %d, %d, "
        "%u, %u, %u, %d, %d, %u, "
        "%d, %d, %d, %d, "
        "'%s', '%s', %u, '%s', %u, "
        "%u, %u, %u)",
        record->m_id, record->charac_no, record->charac_name, record->village, record->job,
        record->lev, record->exp, record->grow_type,
        record->HP, record->maxHP, record->maxMP, record->phy_attack, record->phy_defense,
        record->mag_attack, record->mag_defense,
        record->inven_weight, record->hp_regen, record->mp_regen,
        record->move_speed, record->attack_speed, record->cast_speed, record->hit_recovery,
        record->jump, record->charac_weight,
        record->fatigue, record->max_fatigue, record->premium_fatigue, record->max_premium_fatigue,
        record->create_time, record->last_play_time, record->dungeon_clear_point,
        record->delete_time, record->delete_flag,
        record->guild_id, record->guild_right, record->member_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TmpCharac_Get(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no, TmpCharac* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, charac_name, village, job, lev, exp, grow_type, "
        "HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
        "inven_weight, hp_regen, mp_regen, "
        "move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, "
        "fatigue, max_fatigue, premium_fatigue, max_premium_fatigue, "
        "create_time, last_play_time, dungeon_clear_point, delete_time, delete_flag, "
        "guild_id, guild_right, member_flag "
        "FROM tmp_charac WHERE m_id = %u AND charac_no = %u",
        m_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    if (DBQueryResult_FetchRow(&result, values) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    record->m_id = (unsigned int)atoi(values[0]);
    record->charac_no = (unsigned int)atoi(values[1]);
    strncpy(record->charac_name, values[2], sizeof(record->charac_name) - 1);
    record->charac_name[sizeof(record->charac_name) - 1] = '\0';
    record->village = (unsigned char)atoi(values[3]);
    record->job = (unsigned char)atoi(values[4]);
    record->lev = (unsigned char)atoi(values[5]);
    record->exp = (unsigned int)atoi(values[6]);
    record->grow_type = (unsigned char)atoi(values[7]);
    record->HP = (unsigned char)atoi(values[8]);
    record->maxHP = (unsigned short)atoi(values[9]);
    record->maxMP = (unsigned short)atoi(values[10]);
    record->phy_attack = (unsigned short)atoi(values[11]);
    record->phy_defense = (unsigned short)atoi(values[12]);
    record->mag_attack = (unsigned short)atoi(values[13]);
    record->mag_defense = (unsigned short)atoi(values[14]);
    record->inven_weight = (unsigned int)atoi(values[15]);
    record->hp_regen = (short)atoi(values[16]);
    record->mp_regen = (short)atoi(values[17]);
    record->move_speed = (unsigned short)atoi(values[18]);
    record->attack_speed = (unsigned short)atoi(values[19]);
    record->cast_speed = (unsigned short)atoi(values[20]);
    record->hit_recovery = (short)atoi(values[21]);
    record->jump = (short)atoi(values[22]);
    record->charac_weight = (unsigned int)atoi(values[23]);
    record->fatigue = (short)atoi(values[24]);
    record->max_fatigue = (short)atoi(values[25]);
    record->premium_fatigue = (short)atoi(values[26]);
    record->max_premium_fatigue = (short)atoi(values[27]);
    strncpy(record->create_time, values[28], sizeof(record->create_time) - 1);
    record->create_time[sizeof(record->create_time) - 1] = '\0';
    strncpy(record->last_play_time, values[29], sizeof(record->last_play_time) - 1);
    record->last_play_time[sizeof(record->last_play_time) - 1] = '\0';
    record->dungeon_clear_point = (unsigned int)atoi(values[30]);
    strncpy(record->delete_time, values[31], sizeof(record->delete_time) - 1);
    record->delete_time[sizeof(record->delete_time) - 1] = '\0';
    record->delete_flag = (unsigned char)atoi(values[32]);
    record->guild_id = (unsigned int)atoi(values[33]);
    record->guild_right = (unsigned char)atoi(values[34]);
    record->member_flag = (unsigned char)atoi(values[35]);

    DBQueryResult_Free(&result);
    return 0;
}

int TmpCharac_Update(DBConnectionManager* manager, const TmpCharac* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE tmp_charac SET "
        "charac_name = '%s', village = %u, job = %u, lev = %u, exp = %u, grow_type = %u, "
        "HP = %u, maxHP = %u, maxMP = %u, phy_attack = %u, phy_defense = %u, "
        "mag_attack = %u, mag_defense = %u, inven_weight = %u, hp_regen = %d, mp_regen = %d, "
        "move_speed = %u, attack_speed = %u, cast_speed = %u, hit_recovery = %d, jump = %d, "
        "charac_weight = %u, fatigue = %d, max_fatigue = %d, premium_fatigue = %d, "
        "max_premium_fatigue = %d, create_time = '%s', last_play_time = '%s', "
        "dungeon_clear_point = %u, delete_time = '%s', delete_flag = %u, "
        "guild_id = %u, guild_right = %u, member_flag = %u "
        "WHERE m_id = %u AND charac_no = %u",
        record->charac_name, record->village, record->job, record->lev, record->exp, record->grow_type,
        record->HP, record->maxHP, record->maxMP, record->phy_attack, record->phy_defense,
        record->mag_attack, record->mag_defense, record->inven_weight, record->hp_regen, record->mp_regen,
        record->move_speed, record->attack_speed, record->cast_speed, record->hit_recovery, record->jump,
        record->charac_weight, record->fatigue, record->max_fatigue, record->premium_fatigue,
        record->max_premium_fatigue, record->create_time, record->last_play_time,
        record->dungeon_clear_point, record->delete_time, record->delete_flag,
        record->guild_id, record->guild_right, record->member_flag,
        record->m_id, record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TmpCharac_Delete(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM tmp_charac WHERE m_id = %u AND charac_no = %u",
        m_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TmpCharac_Exists(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) as cnt FROM tmp_charac WHERE m_id = %u AND charac_no = %u",
        m_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return 0;
    }

    char* values[MAX_FIELDS];
    int exists = 0;

    if (DBQueryResult_FetchRow(&result, values) >= 0) {
        exists = (atoi(values[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 专项操作 ==================== */

int TmpCharac_UpdateLevel(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no,
                          unsigned char lev, unsigned int exp) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE tmp_charac SET lev = %u, exp = %u WHERE m_id = %u AND charac_no = %u",
        lev, exp, m_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TmpCharac_UpdateFatigue(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no,
                            short fatigue, short premium_fatigue) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE tmp_charac SET fatigue = %d, premium_fatigue = %d WHERE m_id = %u AND charac_no = %u",
        fatigue, premium_fatigue, m_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TmpCharac_UpdateGuild(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no,
                          unsigned int guild_id, unsigned char guild_right) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE tmp_charac SET guild_id = %u, guild_right = %u WHERE m_id = %u AND charac_no = %u",
        guild_id, guild_right, m_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TmpCharac_MarkDeleted(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE tmp_charac SET delete_flag = 1, delete_time = NOW() WHERE m_id = %u AND charac_no = %u",
        m_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作 ==================== */

int TmpCharac_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) as cnt FROM tmp_charac");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    char* values[MAX_FIELDS];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) >= 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int TmpCharac_GetByMid(DBConnectionManager* manager, unsigned int m_id, TmpCharac** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, charac_name, village, job, lev, exp, grow_type, "
        "HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
        "inven_weight, hp_regen, mp_regen, "
        "move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, "
        "fatigue, max_fatigue, premium_fatigue, max_premium_fatigue, "
        "create_time, last_play_time, dungeon_clear_point, delete_time, delete_flag, "
        "guild_id, guild_right, member_flag "
        "FROM tmp_charac WHERE m_id = %u ORDER BY charac_no",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    *records = (TmpCharac*)malloc(sizeof(TmpCharac) * result.row_count);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    *count = 0;

    while (DBQueryResult_FetchRow(&result, values) >= 0) {
        TmpCharac* rec = &(*records)[*count];
        rec->m_id = (unsigned int)atoi(values[0]);
        rec->charac_no = (unsigned int)atoi(values[1]);
        strncpy(rec->charac_name, values[2], sizeof(rec->charac_name) - 1);
        rec->charac_name[sizeof(rec->charac_name) - 1] = '\0';
        rec->village = (unsigned char)atoi(values[3]);
        rec->job = (unsigned char)atoi(values[4]);
        rec->lev = (unsigned char)atoi(values[5]);
        rec->exp = (unsigned int)atoi(values[6]);
        rec->grow_type = (unsigned char)atoi(values[7]);
        rec->HP = (unsigned char)atoi(values[8]);
        rec->maxHP = (unsigned short)atoi(values[9]);
        rec->maxMP = (unsigned short)atoi(values[10]);
        rec->phy_attack = (unsigned short)atoi(values[11]);
        rec->phy_defense = (unsigned short)atoi(values[12]);
        rec->mag_attack = (unsigned short)atoi(values[13]);
        rec->mag_defense = (unsigned short)atoi(values[14]);
        rec->inven_weight = (unsigned int)atoi(values[15]);
        rec->hp_regen = (short)atoi(values[16]);
        rec->mp_regen = (short)atoi(values[17]);
        rec->move_speed = (unsigned short)atoi(values[18]);
        rec->attack_speed = (unsigned short)atoi(values[19]);
        rec->cast_speed = (unsigned short)atoi(values[20]);
        rec->hit_recovery = (short)atoi(values[21]);
        rec->jump = (short)atoi(values[22]);
        rec->charac_weight = (unsigned int)atoi(values[23]);
        rec->fatigue = (short)atoi(values[24]);
        rec->max_fatigue = (short)atoi(values[25]);
        rec->premium_fatigue = (short)atoi(values[26]);
        rec->max_premium_fatigue = (short)atoi(values[27]);
        strncpy(rec->create_time, values[28], sizeof(rec->create_time) - 1);
        rec->create_time[sizeof(rec->create_time) - 1] = '\0';
        strncpy(rec->last_play_time, values[29], sizeof(rec->last_play_time) - 1);
        rec->last_play_time[sizeof(rec->last_play_time) - 1] = '\0';
        rec->dungeon_clear_point = (unsigned int)atoi(values[30]);
        strncpy(rec->delete_time, values[31], sizeof(rec->delete_time) - 1);
        rec->delete_time[sizeof(rec->delete_time) - 1] = '\0';
        rec->delete_flag = (unsigned char)atoi(values[32]);
        rec->guild_id = (unsigned int)atoi(values[33]);
        rec->guild_right = (unsigned char)atoi(values[34]);
        rec->member_flag = (unsigned char)atoi(values[35]);
        (*count)++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TmpCharac_GetByName(DBConnectionManager* manager, const char* charac_name, TmpCharac** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !charac_name || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, charac_name, village, job, lev, exp, grow_type, "
        "HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
        "inven_weight, hp_regen, mp_regen, "
        "move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, "
        "fatigue, max_fatigue, premium_fatigue, max_premium_fatigue, "
        "create_time, last_play_time, dungeon_clear_point, delete_time, delete_flag, "
        "guild_id, guild_right, member_flag "
        "FROM tmp_charac WHERE charac_name = '%s'",
        charac_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    *records = (TmpCharac*)malloc(sizeof(TmpCharac) * result.row_count);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    *count = 0;

    while (DBQueryResult_FetchRow(&result, values) >= 0) {
        TmpCharac* rec = &(*records)[*count];
        rec->m_id = (unsigned int)atoi(values[0]);
        rec->charac_no = (unsigned int)atoi(values[1]);
        strncpy(rec->charac_name, values[2], sizeof(rec->charac_name) - 1);
        rec->charac_name[sizeof(rec->charac_name) - 1] = '\0';
        rec->village = (unsigned char)atoi(values[3]);
        rec->job = (unsigned char)atoi(values[4]);
        rec->lev = (unsigned char)atoi(values[5]);
        rec->exp = (unsigned int)atoi(values[6]);
        rec->grow_type = (unsigned char)atoi(values[7]);
        rec->HP = (unsigned char)atoi(values[8]);
        rec->maxHP = (unsigned short)atoi(values[9]);
        rec->maxMP = (unsigned short)atoi(values[10]);
        rec->phy_attack = (unsigned short)atoi(values[11]);
        rec->phy_defense = (unsigned short)atoi(values[12]);
        rec->mag_attack = (unsigned short)atoi(values[13]);
        rec->mag_defense = (unsigned short)atoi(values[14]);
        rec->inven_weight = (unsigned int)atoi(values[15]);
        rec->hp_regen = (short)atoi(values[16]);
        rec->mp_regen = (short)atoi(values[17]);
        rec->move_speed = (unsigned short)atoi(values[18]);
        rec->attack_speed = (unsigned short)atoi(values[19]);
        rec->cast_speed = (unsigned short)atoi(values[20]);
        rec->hit_recovery = (short)atoi(values[21]);
        rec->jump = (short)atoi(values[22]);
        rec->charac_weight = (unsigned int)atoi(values[23]);
        rec->fatigue = (short)atoi(values[24]);
        rec->max_fatigue = (short)atoi(values[25]);
        rec->premium_fatigue = (short)atoi(values[26]);
        rec->max_premium_fatigue = (short)atoi(values[27]);
        strncpy(rec->create_time, values[28], sizeof(rec->create_time) - 1);
        rec->create_time[sizeof(rec->create_time) - 1] = '\0';
        strncpy(rec->last_play_time, values[29], sizeof(rec->last_play_time) - 1);
        rec->last_play_time[sizeof(rec->last_play_time) - 1] = '\0';
        rec->dungeon_clear_point = (unsigned int)atoi(values[30]);
        strncpy(rec->delete_time, values[31], sizeof(rec->delete_time) - 1);
        rec->delete_time[sizeof(rec->delete_time) - 1] = '\0';
        rec->delete_flag = (unsigned char)atoi(values[32]);
        rec->guild_id = (unsigned int)atoi(values[33]);
        rec->guild_right = (unsigned char)atoi(values[34]);
        rec->member_flag = (unsigned char)atoi(values[35]);
        (*count)++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TmpCharac_GetDeleted(DBConnectionManager* manager, TmpCharac** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, charac_name, village, job, lev, exp, grow_type, "
        "HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
        "inven_weight, hp_regen, mp_regen, "
        "move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, "
        "fatigue, max_fatigue, premium_fatigue, max_premium_fatigue, "
        "create_time, last_play_time, dungeon_clear_point, delete_time, delete_flag, "
        "guild_id, guild_right, member_flag "
        "FROM tmp_charac WHERE delete_flag = 1 ORDER BY delete_time DESC");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    *records = (TmpCharac*)malloc(sizeof(TmpCharac) * result.row_count);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    *count = 0;

    while (DBQueryResult_FetchRow(&result, values) >= 0) {
        TmpCharac* rec = &(*records)[*count];
        rec->m_id = (unsigned int)atoi(values[0]);
        rec->charac_no = (unsigned int)atoi(values[1]);
        strncpy(rec->charac_name, values[2], sizeof(rec->charac_name) - 1);
        rec->charac_name[sizeof(rec->charac_name) - 1] = '\0';
        rec->village = (unsigned char)atoi(values[3]);
        rec->job = (unsigned char)atoi(values[4]);
        rec->lev = (unsigned char)atoi(values[5]);
        rec->exp = (unsigned int)atoi(values[6]);
        rec->grow_type = (unsigned char)atoi(values[7]);
        rec->HP = (unsigned char)atoi(values[8]);
        rec->maxHP = (unsigned short)atoi(values[9]);
        rec->maxMP = (unsigned short)atoi(values[10]);
        rec->phy_attack = (unsigned short)atoi(values[11]);
        rec->phy_defense = (unsigned short)atoi(values[12]);
        rec->mag_attack = (unsigned short)atoi(values[13]);
        rec->mag_defense = (unsigned short)atoi(values[14]);
        rec->inven_weight = (unsigned int)atoi(values[15]);
        rec->hp_regen = (short)atoi(values[16]);
        rec->mp_regen = (short)atoi(values[17]);
        rec->move_speed = (unsigned short)atoi(values[18]);
        rec->attack_speed = (unsigned short)atoi(values[19]);
        rec->cast_speed = (unsigned short)atoi(values[20]);
        rec->hit_recovery = (short)atoi(values[21]);
        rec->jump = (short)atoi(values[22]);
        rec->charac_weight = (unsigned int)atoi(values[23]);
        rec->fatigue = (short)atoi(values[24]);
        rec->max_fatigue = (short)atoi(values[25]);
        rec->premium_fatigue = (short)atoi(values[26]);
        rec->max_premium_fatigue = (short)atoi(values[27]);
        strncpy(rec->create_time, values[28], sizeof(rec->create_time) - 1);
        rec->create_time[sizeof(rec->create_time) - 1] = '\0';
        strncpy(rec->last_play_time, values[29], sizeof(rec->last_play_time) - 1);
        rec->last_play_time[sizeof(rec->last_play_time) - 1] = '\0';
        rec->dungeon_clear_point = (unsigned int)atoi(values[30]);
        strncpy(rec->delete_time, values[31], sizeof(rec->delete_time) - 1);
        rec->delete_time[sizeof(rec->delete_time) - 1] = '\0';
        rec->delete_flag = (unsigned char)atoi(values[32]);
        rec->guild_id = (unsigned int)atoi(values[33]);
        rec->guild_right = (unsigned char)atoi(values[34]);
        rec->member_flag = (unsigned char)atoi(values[35]);
        (*count)++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 批量操作 ==================== */

int TmpCharac_Clear(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "DELETE FROM tmp_charac");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TmpCharac_DeleteByMid(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM tmp_charac WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 工具函数 ==================== */

void TmpCharac_PrintInfo(const TmpCharac* record) {
    if (!record) {
        printf("TmpCharac record is NULL\n");
        return;
    }

    printf("========================================\n");
    printf("TmpCharac 信息:\n");
    printf("  会员ID: %u\n", record->m_id);
    printf("  角色编号: %u\n", record->charac_no);
    printf("  角色名称: %s\n", record->charac_name);
    printf("  村庄: %u | 职业: %u | 等级: %u | 经验: %u\n",
           record->village, record->job, record->lev, record->exp);
    printf("  成长类型: %u\n", record->grow_type);
    printf("  HP: %u/%u | MP: 0/%u\n", record->HP, record->maxHP, record->maxMP);
    printf("  物理: 攻击=%u 防御=%u\n", record->phy_attack, record->phy_defense);
    printf("  魔法: 攻击=%u 防御=%u\n", record->mag_attack, record->mag_defense);
    printf("  速度: 移动=%u 攻击=%u 施法=%u\n",
           record->move_speed, record->attack_speed, record->cast_speed);
    printf("  恢复: HP=%d MP=%d | 受击恢复=%d | 跳跃=%d\n",
           record->hp_regen, record->mp_regen, record->hit_recovery, record->jump);
    printf("  负重: 背包=%u 角色=%u\n", record->inven_weight, record->charac_weight);
    printf("  疲劳度: %d/%d | 高级: %d/%d\n",
           record->fatigue, record->max_fatigue,
           record->premium_fatigue, record->max_premium_fatigue);
    printf("  副本点数: %u\n", record->dungeon_clear_point);
    printf("  公会: ID=%u 权限=%u 成员标记=%u\n",
           record->guild_id, record->guild_right, record->member_flag);
    printf("  创建时间: %s\n", record->create_time);
    printf("  最后游戏: %s\n", record->last_play_time);
    printf("  删除标记: %u | 删除时间: %s\n", record->delete_flag, record->delete_time);
    printf("========================================\n");
}
