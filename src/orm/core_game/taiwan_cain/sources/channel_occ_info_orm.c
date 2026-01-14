#include "channel_occ_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int ChannelOccInfo_Add(DBConnectionManager* manager, const ChannelOccInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "REPLACE INTO channel_occ_info (gc_no, age, occ_num) VALUES (%u, %u, %d)",
        record->gc_no, record->age, record->occ_num);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ChannelOccInfo_Get(DBConnectionManager* manager, unsigned int gc_no, unsigned char age, ChannelOccInfo* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(ChannelOccInfo));

    snprintf(query, sizeof(query),
        "SELECT gc_no, age, occ_num FROM channel_occ_info WHERE gc_no = %u AND age = %u",
        gc_no, age);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->gc_no = (unsigned int)strtoul(values[0], NULL, 10);
        record->age = (unsigned char)atoi(values[1]);
        record->occ_num = (short)atoi(values[2]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int ChannelOccInfo_Update(DBConnectionManager* manager, const ChannelOccInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE channel_occ_info SET occ_num = %d WHERE gc_no = %u AND age = %u",
        record->occ_num, record->gc_no, record->age);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ChannelOccInfo_Delete(DBConnectionManager* manager, unsigned int gc_no, unsigned char age) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM channel_occ_info WHERE gc_no = %u AND age = %u",
        gc_no, age);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ChannelOccInfo_Exists(DBConnectionManager* manager, unsigned int gc_no, unsigned char age) {
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
        "SELECT COUNT(*) FROM channel_occ_info WHERE gc_no = %u AND age = %u",
        gc_no, age);

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
 * 查询操作实现
 ******************************************************************************/

int ChannelOccInfo_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM channel_occ_info");

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

int ChannelOccInfo_GetAll(DBConnectionManager* manager, ChannelOccInfo* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT gc_no, age, occ_num FROM channel_occ_info ORDER BY gc_no, age LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].gc_no = (unsigned int)strtoul(values[0], NULL, 10);
        records[count].age = (unsigned char)atoi(values[1]);
        records[count].occ_num = (short)atoi(values[2]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int ChannelOccInfo_GetByChannel(DBConnectionManager* manager, unsigned int gc_no, ChannelOccInfo* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT gc_no, age, occ_num FROM channel_occ_info WHERE gc_no = %u ORDER BY age LIMIT %d",
        gc_no, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].gc_no = (unsigned int)strtoul(values[0], NULL, 10);
        records[count].age = (unsigned char)atoi(values[1]);
        records[count].occ_num = (short)atoi(values[2]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int ChannelOccInfo_CountByChannel(DBConnectionManager* manager, unsigned int gc_no) {
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
        "SELECT COUNT(*) FROM channel_occ_info WHERE gc_no = %u",
        gc_no);

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

int ChannelOccInfo_BatchDelete(DBConnectionManager* manager, const ChannelOccInfo* records, int count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    int i;

    if (!manager || !records || count <= 0) {
        return -1;
    }

    for (i = 0; i < count; i++) {
        memset(&result, 0, sizeof(DBQueryResult));

        snprintf(query, sizeof(query),
            "DELETE FROM channel_occ_info WHERE gc_no = %u AND age = %u",
            records[i].gc_no, records[i].age);

        if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
            DBQueryResult_Free(&result);
            return -1;
        }

        DBQueryResult_Free(&result);
    }

    return 0;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void ChannelOccInfo_PrintInfo(const ChannelOccInfo* record) {
    if (!record) {
        printf("ChannelOccInfo: NULL\n");
        return;
    }

    printf("=== Channel Occ Info ===\n");
    printf("  gc_no: %u\n", record->gc_no);
    printf("  age: %u\n", record->age);
    printf("  occ_num: %d\n", record->occ_num);
    printf("========================\n");
}
