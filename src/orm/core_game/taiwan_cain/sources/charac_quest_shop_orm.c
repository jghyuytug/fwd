#include "charac_quest_shop_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

/* ==================== CRUD 操作实现 ==================== */

int CharacQuestShop_Add(DBConnectionManager* manager, const CharacQuestShop* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_quest_shop "
        "(charac_no, qp, max_hp, max_mp, psy_attack, psy_defense, mag_attack, mag_defence, "
        "move_speed, attack_speed, hp_regen, mp_regen, "
        "all_element_resist, fire_element_resist, water_element_resist, light_element_resist, dark_element_resist, "
        "all_element_attack, fire_element_attack, water_element_attack, light_element_attack, dark_element_attack, "
        "psy_critical, mag_critical, good_hit, evasion, hit_recovery, "
        "init_count, separate_psy_mag_attack, quest_piece) "
        "VALUES (%u, %u, %hu, %hu, %hu, %hu, %hu, %hu, %hu, %hu, %hu, %hu, "
        "%hu, %hu, %hu, %hu, %hu, %hu, %hu, %hu, %hu, %hu, %hu, %hu, %hu, %hu, %hu, %hu, %hu, %hu)",
        record->charac_no, record->qp,
        record->max_hp, record->max_mp,
        record->psy_attack, record->psy_defense, record->mag_attack, record->mag_defence,
        record->move_speed, record->attack_speed,
        record->hp_regen, record->mp_regen,
        record->all_element_resist, record->fire_element_resist, record->water_element_resist,
        record->light_element_resist, record->dark_element_resist,
        record->all_element_attack, record->fire_element_attack, record->water_element_attack,
        record->light_element_attack, record->dark_element_attack,
        record->psy_critical, record->mag_critical,
        record->good_hit, record->evasion, record->hit_recovery,
        record->init_count, record->separate_psy_mag_attack, record->quest_piece);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacQuestShop_Get(DBConnectionManager* manager, unsigned int charac_no, CharacQuestShop* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[32];

    if (!manager || !record) {
        return -1;
    }

    memset(record, 0, sizeof(CharacQuestShop));

    snprintf(query, sizeof(query),
        "SELECT charac_no, qp, max_hp, max_mp, psy_attack, psy_defense, mag_attack, mag_defence, "
        "move_speed, attack_speed, hp_regen, mp_regen, "
        "all_element_resist, fire_element_resist, water_element_resist, light_element_resist, dark_element_resist, "
        "all_element_attack, fire_element_attack, water_element_attack, light_element_attack, dark_element_attack, "
        "psy_critical, mag_critical, good_hit, evasion, hit_recovery, "
        "init_count, separate_psy_mag_attack, quest_piece "
        "FROM charac_quest_shop WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);
        record->qp = (unsigned int)atoi(values[1]);
        record->max_hp = (unsigned short)atoi(values[2]);
        record->max_mp = (unsigned short)atoi(values[3]);
        record->psy_attack = (unsigned short)atoi(values[4]);
        record->psy_defense = (unsigned short)atoi(values[5]);
        record->mag_attack = (unsigned short)atoi(values[6]);
        record->mag_defence = (unsigned short)atoi(values[7]);
        record->move_speed = (unsigned short)atoi(values[8]);
        record->attack_speed = (unsigned short)atoi(values[9]);
        record->hp_regen = (unsigned short)atoi(values[10]);
        record->mp_regen = (unsigned short)atoi(values[11]);
        record->all_element_resist = (unsigned short)atoi(values[12]);
        record->fire_element_resist = (unsigned short)atoi(values[13]);
        record->water_element_resist = (unsigned short)atoi(values[14]);
        record->light_element_resist = (unsigned short)atoi(values[15]);
        record->dark_element_resist = (unsigned short)atoi(values[16]);
        record->all_element_attack = (unsigned short)atoi(values[17]);
        record->fire_element_attack = (unsigned short)atoi(values[18]);
        record->water_element_attack = (unsigned short)atoi(values[19]);
        record->light_element_attack = (unsigned short)atoi(values[20]);
        record->dark_element_attack = (unsigned short)atoi(values[21]);
        record->psy_critical = (unsigned short)atoi(values[22]);
        record->mag_critical = (unsigned short)atoi(values[23]);
        record->good_hit = (unsigned short)atoi(values[24]);
        record->evasion = (unsigned short)atoi(values[25]);
        record->hit_recovery = (unsigned short)atoi(values[26]);
        record->init_count = (unsigned short)atoi(values[27]);
        record->separate_psy_mag_attack = (unsigned short)atoi(values[28]);
        record->quest_piece = (unsigned short)atoi(values[29]);

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacQuestShop_Update(DBConnectionManager* manager, const CharacQuestShop* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_quest_shop SET "
        "qp = %u, max_hp = %hu, max_mp = %hu, "
        "psy_attack = %hu, psy_defense = %hu, mag_attack = %hu, mag_defence = %hu, "
        "move_speed = %hu, attack_speed = %hu, hp_regen = %hu, mp_regen = %hu, "
        "all_element_resist = %hu, fire_element_resist = %hu, water_element_resist = %hu, "
        "light_element_resist = %hu, dark_element_resist = %hu, "
        "all_element_attack = %hu, fire_element_attack = %hu, water_element_attack = %hu, "
        "light_element_attack = %hu, dark_element_attack = %hu, "
        "psy_critical = %hu, mag_critical = %hu, good_hit = %hu, evasion = %hu, hit_recovery = %hu, "
        "init_count = %hu, separate_psy_mag_attack = %hu, quest_piece = %hu "
        "WHERE charac_no = %u",
        record->qp, record->max_hp, record->max_mp,
        record->psy_attack, record->psy_defense, record->mag_attack, record->mag_defence,
        record->move_speed, record->attack_speed, record->hp_regen, record->mp_regen,
        record->all_element_resist, record->fire_element_resist, record->water_element_resist,
        record->light_element_resist, record->dark_element_resist,
        record->all_element_attack, record->fire_element_attack, record->water_element_attack,
        record->light_element_attack, record->dark_element_attack,
        record->psy_critical, record->mag_critical, record->good_hit, record->evasion, record->hit_recovery,
        record->init_count, record->separate_psy_mag_attack, record->quest_piece,
        record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacQuestShop_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_quest_shop WHERE charac_no = %u", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacQuestShop_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_quest_shop WHERE charac_no = %u", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return (count > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

/* ==================== 专项操作实现 ==================== */

int CharacQuestShop_AddQuestPoints(DBConnectionManager* manager, unsigned int charac_no, unsigned int qp_amount) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_quest_shop SET qp = qp + %u WHERE charac_no = %u",
        qp_amount, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacQuestShop_SpendQuestPoints(DBConnectionManager* manager, unsigned int charac_no, unsigned int qp_cost) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    CharacQuestShop record;

    if (!manager) {
        return -1;
    }

    /* 先检查QP是否足够 */
    if (CharacQuestShop_Get(manager, charac_no, &record) < 0) {
        return -1;
    }

    if (record.qp < qp_cost) {
        return -1; /* QP不足 */
    }

    /* 扣除QP */
    snprintf(query, sizeof(query),
        "UPDATE charac_quest_shop SET qp = qp - %u WHERE charac_no = %u",
        qp_cost, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacQuestShop_UpdateStat(DBConnectionManager* manager, unsigned int charac_no,
                                const char* stat_name, unsigned short value) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !stat_name) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_quest_shop SET %s = %hu WHERE charac_no = %u",
        stat_name, value, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacQuestShop_IncrementStat(DBConnectionManager* manager, unsigned int charac_no,
                                   const char* stat_name, unsigned short increment) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !stat_name) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_quest_shop SET %s = %s + %hu WHERE charac_no = %u",
        stat_name, stat_name, increment, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacQuestShop_ResetAllStats(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_quest_shop SET "
        "max_hp = 0, max_mp = 0, psy_attack = 0, psy_defense = 0, mag_attack = 0, mag_defence = 0, "
        "move_speed = 0, attack_speed = 0, hp_regen = 0, mp_regen = 0, "
        "all_element_resist = 0, fire_element_resist = 0, water_element_resist = 0, "
        "light_element_resist = 0, dark_element_resist = 0, "
        "all_element_attack = 0, fire_element_attack = 0, water_element_attack = 0, "
        "light_element_attack = 0, dark_element_attack = 0, "
        "psy_critical = 0, mag_critical = 0, good_hit = 0, evasion = 0, hit_recovery = 0, "
        "init_count = 0, separate_psy_mag_attack = 0, quest_piece = 0 "
        "WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacQuestShop_UpdateElementResists(DBConnectionManager* manager, unsigned int charac_no,
                                          unsigned short all, unsigned short fire,
                                          unsigned short water, unsigned short light, unsigned short dark) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_quest_shop SET "
        "all_element_resist = %hu, fire_element_resist = %hu, water_element_resist = %hu, "
        "light_element_resist = %hu, dark_element_resist = %hu "
        "WHERE charac_no = %u",
        all, fire, water, light, dark, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacQuestShop_UpdateElementAttacks(DBConnectionManager* manager, unsigned int charac_no,
                                          unsigned short all, unsigned short fire,
                                          unsigned short water, unsigned short light, unsigned short dark) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_quest_shop SET "
        "all_element_attack = %hu, fire_element_attack = %hu, water_element_attack = %hu, "
        "light_element_attack = %hu, dark_element_attack = %hu "
        "WHERE charac_no = %u",
        all, fire, water, light, dark, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作实现 ==================== */

int CharacQuestShop_GetTotalCount(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_quest_shop");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return count;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacQuestShop_GetByQPRange(DBConnectionManager* manager, unsigned int min_qp, unsigned int max_qp,
                                  unsigned int* charac_nos, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];
    int count = 0;

    if (!manager || !charac_nos || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no FROM charac_quest_shop WHERE qp BETWEEN %u AND %u LIMIT %d",
        min_qp, max_qp, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        charac_nos[count++] = (unsigned int)atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacQuestShop_GetTopByQP(DBConnectionManager* manager, int limit, unsigned int* charac_nos) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];
    int count = 0;

    if (!manager || !charac_nos || limit <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no FROM charac_quest_shop ORDER BY qp DESC LIMIT %d",
        limit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < limit) {
        charac_nos[count++] = (unsigned int)atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

long long CharacQuestShop_GetTotalQP(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    long long total = 0;

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT SUM(qp) FROM charac_quest_shop");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        total = atoll(values[0]);
    }

    DBQueryResult_Free(&result);
    return total;
}

/* ==================== 工具函数实现 ==================== */

void CharacQuestShop_PrintInfo(const CharacQuestShop* record) {
    if (!record) {
        printf("CharacQuestShop: NULL\n");
        return;
    }

    printf("CharacQuestShop {\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  qp: %u\n", record->qp);
    printf("  Basic: HP=%hu, MP=%hu\n", record->max_hp, record->max_mp);
    printf("  Physical: Atk=%hu, Def=%hu, Crit=%hu\n",
           record->psy_attack, record->psy_defense, record->psy_critical);
    printf("  Magical: Atk=%hu, Def=%hu, Crit=%hu\n",
           record->mag_attack, record->mag_defence, record->mag_critical);
    printf("  Speed: Move=%hu, Attack=%hu\n", record->move_speed, record->attack_speed);
    printf("  Regen: HP=%hu, MP=%hu\n", record->hp_regen, record->mp_regen);
    printf("  Element Resist: All=%hu, Fire=%hu, Water=%hu, Light=%hu, Dark=%hu\n",
           record->all_element_resist, record->fire_element_resist, record->water_element_resist,
           record->light_element_resist, record->dark_element_resist);
    printf("  Element Attack: All=%hu, Fire=%hu, Water=%hu, Light=%hu, Dark=%hu\n",
           record->all_element_attack, record->fire_element_attack, record->water_element_attack,
           record->light_element_attack, record->dark_element_attack);
    printf("  Combat: Hit=%hu, Evasion=%hu, HitRecovery=%hu\n",
           record->good_hit, record->evasion, record->hit_recovery);
    printf("  Special: InitCount=%hu, SeparatePsyMag=%hu, QuestPiece=%hu\n",
           record->init_count, record->separate_psy_mag_attack, record->quest_piece);
    printf("}\n");
}
