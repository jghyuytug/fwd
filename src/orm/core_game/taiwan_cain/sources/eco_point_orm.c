#include "eco_point_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int EcoPoint_Add(DBConnectionManager* manager, const EcoPoint* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO eco_point (charac_no, eco_point, point_500, point_300, "
        "point_100, point_50, point_20) "
        "VALUES (%u, %u, %d, %d, %d, %d, %d)",
        record->charac_no, record->eco_point, record->point_500,
        record->point_300, record->point_100, record->point_50, record->point_20);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EcoPoint_Get(DBConnectionManager* manager, unsigned int charac_no, EcoPoint* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, eco_point, point_500, point_300, point_100, "
        "point_50, point_20 FROM eco_point WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);
        record->eco_point = (unsigned int)atoi(values[1]);
        record->point_500 = (signed char)atoi(values[2]);
        record->point_300 = (signed char)atoi(values[3]);
        record->point_100 = (signed char)atoi(values[4]);
        record->point_50 = (signed char)atoi(values[5]);
        record->point_20 = (signed char)atoi(values[6]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int EcoPoint_Update(DBConnectionManager* manager, const EcoPoint* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE eco_point SET eco_point = %u, point_500 = %d, point_300 = %d, "
        "point_100 = %d, point_50 = %d, point_20 = %d WHERE charac_no = %u",
        record->eco_point, record->point_500, record->point_300,
        record->point_100, record->point_50, record->point_20, record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EcoPoint_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM eco_point WHERE charac_no = %u", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EcoPoint_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int exists = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM eco_point WHERE charac_no = %u LIMIT 1", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = 1;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/*******************************************************************************
 * 业务操作实现
 ******************************************************************************/

int EcoPoint_AddPoints(DBConnectionManager* manager, unsigned int charac_no, unsigned int points) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || points == 0) {
        return -1;
    }

    /* 如果记录不存在，先创建 */
    if (EcoPoint_Exists(manager, charac_no) == 0) {
        EcoPoint new_record;
        memset(&new_record, 0, sizeof(EcoPoint));
        new_record.charac_no = charac_no;
        new_record.eco_point = points;
        return EcoPoint_Add(manager, &new_record);
    }

    /* 记录存在，增加点数 */
    snprintf(query, sizeof(query),
        "UPDATE eco_point SET eco_point = eco_point + %u WHERE charac_no = %u",
        points, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EcoPoint_SubtractPoints(DBConnectionManager* manager, unsigned int charac_no, unsigned int points) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    EcoPoint current;

    if (!manager || points == 0) {
        return -1;
    }

    /* 检查当前点数是否足够 */
    if (EcoPoint_Get(manager, charac_no, &current) < 0) {
        return -1;
    }

    if (current.eco_point < points) {
        fprintf(stderr, "EcoPoint_SubtractPoints: Insufficient points (%u < %u)\n",
                current.eco_point, points);
        return -1;
    }

    /* 减少点数 */
    snprintf(query, sizeof(query),
        "UPDATE eco_point SET eco_point = eco_point - %u WHERE charac_no = %u",
        points, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EcoPoint_IncrementTierCount(DBConnectionManager* manager, unsigned int charac_no, int tier_value) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    const char* field_name;

    if (!manager) {
        return -1;
    }

    /* 根据档次值确定字段名 */
    switch (tier_value) {
        case 500:
            field_name = "point_500";
            break;
        case 300:
            field_name = "point_300";
            break;
        case 100:
            field_name = "point_100";
            break;
        case 50:
            field_name = "point_50";
            break;
        case 20:
            field_name = "point_20";
            break;
        default:
            fprintf(stderr, "EcoPoint_IncrementTierCount: Invalid tier value %d\n", tier_value);
            return -1;
    }

    /* 如果记录不存在，先创建 */
    if (EcoPoint_Exists(manager, charac_no) == 0) {
        EcoPoint new_record;
        memset(&new_record, 0, sizeof(EcoPoint));
        new_record.charac_no = charac_no;
        if (EcoPoint_Add(manager, &new_record) < 0) {
            return -1;
        }
    }

    /* 增加档次计数 */
    snprintf(query, sizeof(query),
        "UPDATE eco_point SET %s = %s + 1 WHERE charac_no = %u",
        field_name, field_name, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EcoPoint_UpdateTierCounts(DBConnectionManager* manager, unsigned int charac_no,
                               signed char count_500, signed char count_300,
                               signed char count_100, signed char count_50,
                               signed char count_20) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE eco_point SET point_500 = %d, point_300 = %d, point_100 = %d, "
        "point_50 = %d, point_20 = %d WHERE charac_no = %u",
        count_500, count_300, count_100, count_50, count_20, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EcoPoint_Reset(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE eco_point SET eco_point = 0, point_500 = 0, point_300 = 0, "
        "point_100 = 0, point_50 = 0, point_20 = 0 WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EcoPoint_GetTopRanking(DBConnectionManager* manager, EcoPoint* records, int max_count) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, eco_point, point_500, point_300, point_100, "
        "point_50, point_20 FROM eco_point ORDER BY eco_point DESC LIMIT %d",
        max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        records[count].eco_point = (unsigned int)atoi(values[1]);
        records[count].point_500 = (signed char)atoi(values[2]);
        records[count].point_300 = (signed char)atoi(values[3]);
        records[count].point_100 = (signed char)atoi(values[4]);
        records[count].point_50 = (signed char)atoi(values[5]);
        records[count].point_20 = (signed char)atoi(values[6]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int EcoPoint_GetTotalPoints(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int total = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT SUM(eco_point) FROM eco_point");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        total = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return total;
}

int EcoPoint_CountCharacters(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM eco_point");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void EcoPoint_PrintInfo(const EcoPoint* record) {
    if (!record) {
        printf("EcoPoint: (null)\n");
        return;
    }

    printf("============ Eco Point Info ============\n");
    printf("  Charac No:   %u\n", record->charac_no);
    printf("  Eco Point:   %u\n", record->eco_point);
    printf("  Tier Counts:\n");
    printf("    500 points: %d times\n", record->point_500);
    printf("    300 points: %d times\n", record->point_300);
    printf("    100 points: %d times\n", record->point_100);
    printf("     50 points: %d times\n", record->point_50);
    printf("     20 points: %d times\n", record->point_20);
    printf("=========================================\n");
}
