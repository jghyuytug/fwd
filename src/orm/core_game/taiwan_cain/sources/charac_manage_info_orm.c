#include "charac_manage_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 1024
#define MAX_FIELDS 16

/* ==================== CRUD 操作实现 ==================== */

int CharacManageInfo_Add(DBConnectionManager* manager, const CharacManageInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_manage_info "
        "(charac_no, tag_charac_no, striker_skill_index, max_equip_level) "
        "VALUES (%u, %u, %u, %u)",
        record->charac_no, record->tag_charac_no,
        (unsigned int)record->striker_skill_index,
        (unsigned int)record->max_equip_level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacManageInfo_Get(DBConnectionManager* manager, unsigned int charac_no, CharacManageInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];

    if (!manager || !record) {
        return -1;
    }

    memset(record, 0, sizeof(CharacManageInfo));

    snprintf(query, sizeof(query),
        "SELECT charac_no, tag_charac_no, striker_skill_index, max_equip_level "
        "FROM charac_manage_info WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);
        record->tag_charac_no = (unsigned int)atoi(values[1]);
        record->striker_skill_index = (unsigned char)atoi(values[2]);
        record->max_equip_level = (unsigned short)atoi(values[3]);

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacManageInfo_Update(DBConnectionManager* manager, const CharacManageInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_manage_info SET "
        "tag_charac_no = %u, striker_skill_index = %u, max_equip_level = %u "
        "WHERE charac_no = %u",
        record->tag_charac_no,
        (unsigned int)record->striker_skill_index,
        (unsigned int)record->max_equip_level,
        record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacManageInfo_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_manage_info WHERE charac_no = %u", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacManageInfo_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_manage_info WHERE charac_no = %u", charac_no);

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

int CharacManageInfo_UpdateTagCharacNo(DBConnectionManager* manager, unsigned int charac_no, unsigned int tag_charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_manage_info SET tag_charac_no = %u WHERE charac_no = %u",
        tag_charac_no, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacManageInfo_UpdateStrikerSkillIndex(DBConnectionManager* manager, unsigned int charac_no, unsigned char skill_index) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_manage_info SET striker_skill_index = %u WHERE charac_no = %u",
        (unsigned int)skill_index, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacManageInfo_UpdateMaxEquipLevel(DBConnectionManager* manager, unsigned int charac_no, unsigned short max_level) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_manage_info SET max_equip_level = %u WHERE charac_no = %u",
        (unsigned int)max_level, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacManageInfo_GetByTagCharacNo(DBConnectionManager* manager, unsigned int tag_charac_no,
                                       unsigned int* charac_nos, int max_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];
    int count = 0;

    if (!manager || !charac_nos || max_count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no FROM charac_manage_info WHERE tag_charac_no = %u LIMIT %d",
        tag_charac_no, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_count) {
        charac_nos[count++] = (unsigned int)atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacManageInfo_GetCountByEquipLevelRange(DBConnectionManager* manager,
                                                 unsigned short min_level, unsigned short max_level) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_manage_info WHERE max_equip_level BETWEEN %u AND %u",
        (unsigned int)min_level, (unsigned int)max_level);

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

unsigned short CharacManageInfo_GetMaxEquipLevel(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT MAX(max_equip_level) FROM charac_manage_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        unsigned short max_level = (unsigned short)atoi(values[0]);
        DBQueryResult_Free(&result);
        return max_level;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 工具函数实现 ==================== */

void CharacManageInfo_PrintInfo(const CharacManageInfo* record) {
    if (!record) {
        printf("CharacManageInfo: NULL\n");
        return;
    }

    printf("CharacManageInfo {\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  tag_charac_no: %u\n", record->tag_charac_no);
    printf("  striker_skill_index: %u\n", (unsigned int)record->striker_skill_index);
    printf("  max_equip_level: %u\n", (unsigned int)record->max_equip_level);
    printf("}\n");
}
