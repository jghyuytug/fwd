/**
 * DNF游戏服务器 - TmeCharac表ORM实现
 */

#include "tme_charac_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ========================================================================
// 内部辅助函数
// ========================================================================

/**
 * 从查询结果行填充TME角色结构
 */
static void FillCharacFromRow(char** row, TmeCharac* charac) {
    charac->m_id = atoi(row[0]);
    charac->charac_no = atoi(row[1]);

    strncpy(charac->charac_name, row[2], sizeof(charac->charac_name) - 1);
    charac->charac_name[sizeof(charac->charac_name) - 1] = '\0';

    charac->village = (signed char)atoi(row[3]);
    charac->job = (signed char)atoi(row[4]);
    charac->lev = (signed char)atoi(row[5]);
    charac->exp = atoi(row[6]);
    charac->grow_type = (signed char)atoi(row[7]);

    charac->HP = (unsigned char)atoi(row[8]);
    charac->maxHP = (unsigned short)atoi(row[9]);
    charac->maxMP = (unsigned short)atoi(row[10]);
    charac->phy_attack = (unsigned short)atoi(row[11]);
    charac->phy_defense = (unsigned short)atoi(row[12]);
    charac->mag_attack = (unsigned short)atoi(row[13]);
    charac->mag_defense = (unsigned short)atoi(row[14]);

    // element_resist (blob) - 简化处理，不使用lengths
    if (charac->element_resist) {
        free(charac->element_resist);
        charac->element_resist = NULL;
    }
    charac->element_resist_len = 0;
    charac->element_resist = NULL;

    // spec_property (blob) - 简化处理，不使用lengths
    if (charac->spec_property) {
        free(charac->spec_property);
        charac->spec_property = NULL;
    }
    charac->spec_property_len = 0;
    charac->spec_property = NULL;

    charac->inven_weight = atoi(row[17]);
    charac->hp_regen = (short)atoi(row[18]);
    charac->mp_regen = (short)atoi(row[19]);
    charac->move_speed = (unsigned short)atoi(row[20]);
    charac->attack_speed = (unsigned short)atoi(row[21]);
    charac->cast_speed = (unsigned short)atoi(row[22]);
    charac->hit_recovery = (short)atoi(row[23]);
    charac->jump = (short)atoi(row[24]);
    charac->charac_weight = atoi(row[25]);

    charac->fatigue = (short)atoi(row[26]);
    charac->max_fatigue = (short)atoi(row[27]);
    charac->premium_fatigue = (short)atoi(row[28]);
    charac->max_premium_fatigue = (short)atoi(row[29]);

    if (row[30]) strncpy(charac->create_time, row[30], sizeof(charac->create_time) - 1);
    if (row[31]) strncpy(charac->last_play_time, row[31], sizeof(charac->last_play_time) - 1);

    charac->dungeon_clear_point = atoi(row[32]);

    if (row[33]) strncpy(charac->delete_time, row[33], sizeof(charac->delete_time) - 1);
    charac->delete_flag = (signed char)atoi(row[34]);

    charac->guild_id = (unsigned int)atoi(row[35]);
    charac->guild_right = (signed char)atoi(row[36]);
    charac->member_flag = (signed char)atoi(row[37]);
}

/**
 * 将二进制数据转换为十六进制字符串
 */
static char* BlobToHex(const unsigned char* blob, size_t blob_len) {
    if (!blob || blob_len == 0) {
        char* empty = (char*)malloc(3);
        strcpy(empty, "''");
        return empty;
    }

    char* hex = (char*)malloc(blob_len * 2 + 3); // 0x + hex + \0
    hex[0] = '0';
    hex[1] = 'x';

    for (size_t i = 0; i < blob_len; i++) {
        sprintf(&hex[2 + i * 2], "%02x", blob[i]);
    }

    return hex;
}

// ========================================================================
// CRUD操作实现
// ========================================================================

int TmeCharac_Add(DBConnectionManager* manager, const TmeCharac* charac) {
    if (!manager || !charac) {
        return -1;
    }

    char query[8192];
    char* element_hex = BlobToHex(charac->element_resist, charac->element_resist_len);
    char* spec_hex = BlobToHex(charac->spec_property, charac->spec_property_len);

    snprintf(query, sizeof(query),
             "INSERT INTO tme_charac "
             "(m_id, charac_no, charac_name, village, job, lev, exp, grow_type, "
             "HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
             "element_resist, spec_property, inven_weight, hp_regen, mp_regen, "
             "move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, "
             "fatigue, max_fatigue, premium_fatigue, max_premium_fatigue, "
             "create_time, last_play_time, dungeon_clear_point, delete_time, delete_flag, "
             "guild_id, guild_right, member_flag) "
             "VALUES (%d, %d, '%s', %d, %d, %d, %d, %d, "
             "%u, %u, %u, %u, %u, %u, %u, "
             "%s, %s, %d, %d, %d, "
             "%u, %u, %u, %d, %d, %d, "
             "%d, %d, %d, %d, "
             "'%s', '%s', %d, '%s', %d, "
             "%u, %d, %d)",
             charac->m_id, charac->charac_no, charac->charac_name,
             charac->village, charac->job, charac->lev, charac->exp, charac->grow_type,
             charac->HP, charac->maxHP, charac->maxMP,
             charac->phy_attack, charac->phy_defense, charac->mag_attack, charac->mag_defense,
             element_hex, spec_hex,
             charac->inven_weight, charac->hp_regen, charac->mp_regen,
             charac->move_speed, charac->attack_speed, charac->cast_speed,
             charac->hit_recovery, charac->jump, charac->charac_weight,
             charac->fatigue, charac->max_fatigue,
             charac->premium_fatigue, charac->max_premium_fatigue,
             charac->create_time, charac->last_play_time,
             charac->dungeon_clear_point, charac->delete_time, charac->delete_flag,
             charac->guild_id, charac->guild_right, charac->member_flag);

    free(element_hex);
    free(spec_hex);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TmeCharac_Get(DBConnectionManager* manager, int m_id, int charac_no, TmeCharac* charac) {
    if (!manager || !charac) {
        return -1;
    }

    char query[1024];
    snprintf(query, sizeof(query),
             "SELECT m_id, charac_no, charac_name, village, job, lev, exp, grow_type, "
             "HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
             "element_resist, spec_property, inven_weight, hp_regen, mp_regen, "
             "move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, "
             "fatigue, max_fatigue, premium_fatigue, max_premium_fatigue, "
             "create_time, last_play_time, dungeon_clear_point, delete_time, delete_flag, "
             "guild_id, guild_right, member_flag "
             "FROM tme_charac WHERE m_id=%d AND charac_no=%d",
             m_id, charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[38];
    int ret = -1;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        FillCharacFromRow(values, charac);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int TmeCharac_Update(DBConnectionManager* manager, const TmeCharac* charac) {
    if (!manager || !charac) {
        return -1;
    }

    char query[8192];
    char* element_hex = BlobToHex(charac->element_resist, charac->element_resist_len);
    char* spec_hex = BlobToHex(charac->spec_property, charac->spec_property_len);

    snprintf(query, sizeof(query),
             "UPDATE tme_charac SET "
             "charac_name='%s', village=%d, job=%d, lev=%d, exp=%d, grow_type=%d, "
             "HP=%u, maxHP=%u, maxMP=%u, phy_attack=%u, phy_defense=%u, "
             "mag_attack=%u, mag_defense=%u, element_resist=%s, spec_property=%s, "
             "inven_weight=%d, hp_regen=%d, mp_regen=%d, move_speed=%u, "
             "attack_speed=%u, cast_speed=%u, hit_recovery=%d, jump=%d, charac_weight=%d, "
             "fatigue=%d, max_fatigue=%d, premium_fatigue=%d, max_premium_fatigue=%d, "
             "create_time='%s', last_play_time='%s', dungeon_clear_point=%d, "
             "delete_time='%s', delete_flag=%d, guild_id=%u, guild_right=%d, member_flag=%d "
             "WHERE m_id=%d AND charac_no=%d",
             charac->charac_name, charac->village, charac->job, charac->lev, charac->exp, charac->grow_type,
             charac->HP, charac->maxHP, charac->maxMP,
             charac->phy_attack, charac->phy_defense, charac->mag_attack, charac->mag_defense,
             element_hex, spec_hex,
             charac->inven_weight, charac->hp_regen, charac->mp_regen,
             charac->move_speed, charac->attack_speed, charac->cast_speed,
             charac->hit_recovery, charac->jump, charac->charac_weight,
             charac->fatigue, charac->max_fatigue, charac->premium_fatigue, charac->max_premium_fatigue,
             charac->create_time, charac->last_play_time, charac->dungeon_clear_point,
             charac->delete_time, charac->delete_flag,
             charac->guild_id, charac->guild_right, charac->member_flag,
             charac->m_id, charac->charac_no);

    free(element_hex);
    free(spec_hex);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TmeCharac_Delete(DBConnectionManager* manager, int m_id, int charac_no) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM tme_charac WHERE m_id=%d AND charac_no=%d",
             m_id, charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

bool TmeCharac_Exists(DBConnectionManager* manager, int m_id, int charac_no) {
    if (!manager) {
        return false;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM tme_charac WHERE m_id=%d AND charac_no=%d",
             m_id, charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return (count > 0);
}

// ========================================================================
// 专项操作实现
// ========================================================================

int TmeCharac_UpdateLevel(DBConnectionManager* manager, int m_id, int charac_no,
                          signed char lev, int exp) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE tme_charac SET lev=%d, exp=%d WHERE m_id=%d AND charac_no=%d",
             lev, exp, m_id, charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TmeCharac_UpdateHP(DBConnectionManager* manager, int m_id, int charac_no,
                       unsigned char HP, unsigned short maxHP, unsigned short maxMP) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE tme_charac SET HP=%u, maxHP=%u, maxMP=%u WHERE m_id=%d AND charac_no=%d",
             HP, maxHP, maxMP, m_id, charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TmeCharac_UpdateFatigue(DBConnectionManager* manager, int m_id, int charac_no,
                            short fatigue) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE tme_charac SET fatigue=%d WHERE m_id=%d AND charac_no=%d",
             fatigue, m_id, charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TmeCharac_UpdateLastPlayTime(DBConnectionManager* manager, int m_id, int charac_no) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE tme_charac SET last_play_time=NOW() WHERE m_id=%d AND charac_no=%d",
             m_id, charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TmeCharac_MarkDeleted(DBConnectionManager* manager, int m_id, int charac_no) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE tme_charac SET delete_flag=1, delete_time=NOW() WHERE m_id=%d AND charac_no=%d",
             m_id, charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TmeCharac_UpdateGuild(DBConnectionManager* manager, int m_id, int charac_no,
                          unsigned int guild_id, signed char guild_right) {
    if (!manager) {
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE tme_charac SET guild_id=%u, guild_right=%d WHERE m_id=%d AND charac_no=%d",
             guild_id, guild_right, m_id, charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

// ========================================================================
// 查询操作实现
// ========================================================================

int TmeCharac_GetByMemberId(DBConnectionManager* manager, int m_id,
                            TmeCharac* characs, int max_count) {
    if (!manager || !characs || max_count <= 0) {
        return -1;
    }

    char query[1024];
    snprintf(query, sizeof(query),
             "SELECT m_id, charac_no, charac_name, village, job, lev, exp, grow_type, "
             "HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
             "element_resist, spec_property, inven_weight, hp_regen, mp_regen, "
             "move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, "
             "fatigue, max_fatigue, premium_fatigue, max_premium_fatigue, "
             "create_time, last_play_time, dungeon_clear_point, delete_time, delete_flag, "
             "guild_id, guild_right, member_flag "
             "FROM tme_charac WHERE m_id=%d ORDER BY charac_no LIMIT %d",
             m_id, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[38];
    int count = 0;

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_count) {
        FillCharacFromRow(values, &characs[count]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int TmeCharac_GetByName(DBConnectionManager* manager, const char* charac_name,
                        TmeCharac* charac) {
    if (!manager || !charac_name || !charac) {
        return -1;
    }

    char query[1024];
    snprintf(query, sizeof(query),
             "SELECT m_id, charac_no, charac_name, village, job, lev, exp, grow_type, "
             "HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
             "element_resist, spec_property, inven_weight, hp_regen, mp_regen, "
             "move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, "
             "fatigue, max_fatigue, premium_fatigue, max_premium_fatigue, "
             "create_time, last_play_time, dungeon_clear_point, delete_time, delete_flag, "
             "guild_id, guild_right, member_flag "
             "FROM tme_charac WHERE charac_name='%s'",
             charac_name);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[38];
    int ret = -1;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        FillCharacFromRow(values, charac);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

bool TmeCharac_NameExists(DBConnectionManager* manager, const char* charac_name) {
    if (!manager || !charac_name) {
        return false;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM tme_charac WHERE charac_name='%s'",
             charac_name);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return (count > 0);
}

int TmeCharac_CountByMemberId(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return -1;
    }

    char query[128];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM tme_charac WHERE m_id=%d",
             m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    int count = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int TmeCharac_GetByGuildId(DBConnectionManager* manager, unsigned int guild_id,
                           TmeCharac* characs, int max_count) {
    if (!manager || !characs || max_count <= 0) {
        return -1;
    }

    char query[1024];
    snprintf(query, sizeof(query),
             "SELECT m_id, charac_no, charac_name, village, job, lev, exp, grow_type, "
             "HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
             "element_resist, spec_property, inven_weight, hp_regen, mp_regen, "
             "move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, "
             "fatigue, max_fatigue, premium_fatigue, max_premium_fatigue, "
             "create_time, last_play_time, dungeon_clear_point, delete_time, delete_flag, "
             "guild_id, guild_right, member_flag "
             "FROM tme_charac WHERE guild_id=%u ORDER BY lev DESC LIMIT %d",
             guild_id, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[38];
    int count = 0;

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_count) {
        FillCharacFromRow(values, &characs[count]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

// ========================================================================
// 工具函数实现
// ========================================================================

void TmeCharac_Init(TmeCharac* charac) {
    if (!charac) {
        return;
    }

    memset(charac, 0, sizeof(TmeCharac));
    charac->village = 1;
    charac->lev = 1;
    charac->max_fatigue = 70;
    strcpy(charac->create_time, "0000-00-00 00:00:00");
    strcpy(charac->last_play_time, "0000-00-00 00:00:00");
    strcpy(charac->delete_time, "0000-00-00 00:00:00");
    charac->element_resist = NULL;
    charac->element_resist_len = 0;
    charac->spec_property = NULL;
    charac->spec_property_len = 0;
}

void TmeCharac_Free(TmeCharac* charac) {
    if (!charac) {
        return;
    }

    if (charac->element_resist) {
        free(charac->element_resist);
        charac->element_resist = NULL;
    }

    if (charac->spec_property) {
        free(charac->spec_property);
        charac->spec_property = NULL;
    }
}

void TmeCharac_PrintInfo(const TmeCharac* charac) {
    if (!charac) {
        printf("  [TmeCharac] NULL charac\n");
        return;
    }

    printf("  ----------------------------------------\n");
    printf("  会员ID: %d\n", charac->m_id);
    printf("  角色编号: %d\n", charac->charac_no);
    printf("  角色名称: %s\n", charac->charac_name);
    printf("  村庄: %d  职业: %d  等级: %d  经验: %d\n",
           charac->village, charac->job, charac->lev, charac->exp);
    printf("  成长类型: %d\n", charac->grow_type);
    printf("  HP: %u/%u  MP: %u\n", charac->HP, charac->maxHP, charac->maxMP);
    printf("  物理攻击: %u  物理防御: %u\n", charac->phy_attack, charac->phy_defense);
    printf("  魔法攻击: %u  魔法防御: %u\n", charac->mag_attack, charac->mag_defense);
    printf("  疲劳值: %d/%d  高级疲劳: %d/%d\n",
           charac->fatigue, charac->max_fatigue,
           charac->premium_fatigue, charac->max_premium_fatigue);
    printf("  创建时间: %s\n", charac->create_time);
    printf("  最后游戏时间: %s\n", charac->last_play_time);
    printf("  公会ID: %u  公会权限: %d\n", charac->guild_id, charac->guild_right);
    printf("  删除标记: %d  删除时间: %s\n", charac->delete_flag, charac->delete_time);
    printf("  ----------------------------------------\n");
}
