#include "item_making_skill_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 15

/* 技能字段名映射表 */
static const char* skill_field_names[] = {
    "weapon", "cloth", "leather", "light_armor", "heavy_armor", "plate",
    "amulet", "wrist", "ring", "support", "magic_stone"
};

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int ItemMakingSkillInfo_Add(DBConnectionManager* manager, const ItemMakingSkillInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "REPLACE INTO item_making_skill_info (charac_no, weapon, cloth, leather, light_armor, "
        "heavy_armor, plate, amulet, wrist, ring, support, magic_stone) "
        "VALUES (%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u)",
        record->charac_no, record->weapon, record->cloth, record->leather, record->light_armor,
        record->heavy_armor, record->plate, record->amulet, record->wrist, record->ring,
        record->support, record->magic_stone);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ItemMakingSkillInfo_Get(DBConnectionManager* manager, int charac_no, ItemMakingSkillInfo* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(ItemMakingSkillInfo));

    snprintf(query, sizeof(query),
        "SELECT charac_no, weapon, cloth, leather, light_armor, heavy_armor, plate, "
        "amulet, wrist, ring, support, magic_stone "
        "FROM item_making_skill_info WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);
        record->weapon = (unsigned short)atoi(values[1]);
        record->cloth = (unsigned short)atoi(values[2]);
        record->leather = (unsigned short)atoi(values[3]);
        record->light_armor = (unsigned short)atoi(values[4]);
        record->heavy_armor = (unsigned short)atoi(values[5]);
        record->plate = (unsigned short)atoi(values[6]);
        record->amulet = (unsigned short)atoi(values[7]);
        record->wrist = (unsigned short)atoi(values[8]);
        record->ring = (unsigned short)atoi(values[9]);
        record->support = (unsigned short)atoi(values[10]);
        record->magic_stone = (unsigned short)atoi(values[11]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int ItemMakingSkillInfo_Update(DBConnectionManager* manager, const ItemMakingSkillInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE item_making_skill_info SET weapon=%u, cloth=%u, leather=%u, light_armor=%u, "
        "heavy_armor=%u, plate=%u, amulet=%u, wrist=%u, ring=%u, support=%u, magic_stone=%u "
        "WHERE charac_no=%u",
        record->weapon, record->cloth, record->leather, record->light_armor, record->heavy_armor,
        record->plate, record->amulet, record->wrist, record->ring, record->support,
        record->magic_stone, record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ItemMakingSkillInfo_Delete(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM item_making_skill_info WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ItemMakingSkillInfo_Exists(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM item_making_skill_info WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return (count > 0) ? 1 : 0;
}

/*******************************************************************************
 * 专项操作实现
 ******************************************************************************/

int ItemMakingSkillInfo_UpgradeSkill(DBConnectionManager* manager, int charac_no,
                                     MakingSkillType skill_type, unsigned short levels) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || skill_type < 0 || skill_type >= MAKING_SKILL_COUNT) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE item_making_skill_info SET %s = %s + %u WHERE charac_no = %d",
        skill_field_names[skill_type], skill_field_names[skill_type], levels, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ItemMakingSkillInfo_SetSkillLevel(DBConnectionManager* manager, int charac_no,
                                       MakingSkillType skill_type, unsigned short level) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || skill_type < 0 || skill_type >= MAKING_SKILL_COUNT) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE item_making_skill_info SET %s = %u WHERE charac_no = %d",
        skill_field_names[skill_type], level, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ItemMakingSkillInfo_GetSkillLevel(DBConnectionManager* manager, int charac_no,
                                       MakingSkillType skill_type) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int level = -1;

    if (!manager || skill_type < 0 || skill_type >= MAKING_SKILL_COUNT) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT %s FROM item_making_skill_info WHERE charac_no = %d",
        skill_field_names[skill_type], charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        level = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return level;
}

int ItemMakingSkillInfo_ResetAllSkills(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE item_making_skill_info SET weapon=0, cloth=0, leather=0, light_armor=0, "
        "heavy_armor=0, plate=0, amulet=0, wrist=0, ring=0, support=0, magic_stone=0 "
        "WHERE charac_no=%d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ItemMakingSkillInfo_GetMaxSkillLevel(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int max_level = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT GREATEST(weapon, cloth, leather, light_armor, heavy_armor, plate, "
        "amulet, wrist, ring, support, magic_stone) "
        "FROM item_making_skill_info WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        max_level = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return max_level;
}

/*******************************************************************************
 * 查询操作实现
 ******************************************************************************/

int ItemMakingSkillInfo_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM item_making_skill_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int ItemMakingSkillInfo_GetAll(DBConnectionManager* manager, ItemMakingSkillInfo* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT charac_no, weapon, cloth, leather, light_armor, heavy_armor, plate, "
        "amulet, wrist, ring, support, magic_stone "
        "FROM item_making_skill_info ORDER BY charac_no LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        records[count].weapon = (unsigned short)atoi(values[1]);
        records[count].cloth = (unsigned short)atoi(values[2]);
        records[count].leather = (unsigned short)atoi(values[3]);
        records[count].light_armor = (unsigned short)atoi(values[4]);
        records[count].heavy_armor = (unsigned short)atoi(values[5]);
        records[count].plate = (unsigned short)atoi(values[6]);
        records[count].amulet = (unsigned short)atoi(values[7]);
        records[count].wrist = (unsigned short)atoi(values[8]);
        records[count].ring = (unsigned short)atoi(values[9]);
        records[count].support = (unsigned short)atoi(values[10]);
        records[count].magic_stone = (unsigned short)atoi(values[11]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int ItemMakingSkillInfo_GetBySkillLevelRange(DBConnectionManager* manager, MakingSkillType skill_type,
                                              unsigned short min_level, unsigned short max_level,
                                              ItemMakingSkillInfo* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0 || skill_type < 0 || skill_type >= MAKING_SKILL_COUNT) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT charac_no, weapon, cloth, leather, light_armor, heavy_armor, plate, "
        "amulet, wrist, ring, support, magic_stone "
        "FROM item_making_skill_info WHERE %s BETWEEN %u AND %u ORDER BY %s DESC LIMIT %d",
        skill_field_names[skill_type], min_level, max_level, skill_field_names[skill_type], max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        records[count].weapon = (unsigned short)atoi(values[1]);
        records[count].cloth = (unsigned short)atoi(values[2]);
        records[count].leather = (unsigned short)atoi(values[3]);
        records[count].light_armor = (unsigned short)atoi(values[4]);
        records[count].heavy_armor = (unsigned short)atoi(values[5]);
        records[count].plate = (unsigned short)atoi(values[6]);
        records[count].amulet = (unsigned short)atoi(values[7]);
        records[count].wrist = (unsigned short)atoi(values[8]);
        records[count].ring = (unsigned short)atoi(values[9]);
        records[count].support = (unsigned short)atoi(values[10]);
        records[count].magic_stone = (unsigned short)atoi(values[11]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int ItemMakingSkillInfo_CountActiveUsers(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM item_making_skill_info WHERE "
        "weapon>0 OR cloth>0 OR leather>0 OR light_armor>0 OR heavy_armor>0 OR plate>0 OR "
        "amulet>0 OR wrist>0 OR ring>0 OR support>0 OR magic_stone>0");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/*******************************************************************************
 * 批量操作实现
 ******************************************************************************/

int ItemMakingSkillInfo_BatchDelete(DBConnectionManager* manager, const int* charac_nos, int count) {
    char query[MAX_QUERY_LEN];
    char in_clause[MAX_QUERY_LEN];
    DBQueryResult result;
    int i, offset = 0;

    if (!manager || !charac_nos || count <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    /* 构建 IN 子句 */
    for (i = 0; i < count && offset < sizeof(in_clause) - 20; i++) {
        if (i > 0) {
            offset += snprintf(in_clause + offset, sizeof(in_clause) - offset, ",");
        }
        offset += snprintf(in_clause + offset, sizeof(in_clause) - offset, "%d", charac_nos[i]);
    }

    snprintf(query, sizeof(query),
        "DELETE FROM item_making_skill_info WHERE charac_no IN (%s)",
        in_clause);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void ItemMakingSkillInfo_PrintInfo(const ItemMakingSkillInfo* record) {
    if (!record) {
        printf("ItemMakingSkillInfo: NULL\n");
        return;
    }

    printf("=== Item Making Skill Info ===\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  weapon: %u\n", record->weapon);
    printf("  cloth: %u\n", record->cloth);
    printf("  leather: %u\n", record->leather);
    printf("  light_armor: %u\n", record->light_armor);
    printf("  heavy_armor: %u\n", record->heavy_armor);
    printf("  plate: %u\n", record->plate);
    printf("  amulet: %u\n", record->amulet);
    printf("  wrist: %u\n", record->wrist);
    printf("  ring: %u\n", record->ring);
    printf("  support: %u\n", record->support);
    printf("  magic_stone: %u\n", record->magic_stone);
    printf("==============================\n");
}
