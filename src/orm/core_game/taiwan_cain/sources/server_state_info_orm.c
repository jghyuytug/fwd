#include "server_state_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/* ========== CRUD 操作实现 ========== */

int ServerStateInfo_Add(DBConnectionManager* manager, const ServerStateInfo* record) {
    char query[MAX_QUERY_LEN];
    char escaped_state[SERVER_STATE_SIZE * 2 + 1];
    DBQueryResult result;

    if (!manager || !record) {
        fprintf(stderr, "ServerStateInfo_Add: Invalid parameters\n");
        return -1;
    }

    /* 转义state字段为十六进制字符串 */
    int i;
    for (i = 0; i < SERVER_STATE_SIZE; i++) {
        sprintf(escaped_state + i * 2, "%02X", record->state[i]);
    }
    escaped_state[SERVER_STATE_SIZE * 2] = '\0';

    snprintf(query, sizeof(query),
        "REPLACE INTO server_state_info (category, code, state, start_time, end_time) "
        "VALUES (%d, %d, UNHEX('%s'), '%s', '%s')",
        record->category, record->code, escaped_state,
        record->start_time, record->end_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        fprintf(stderr, "ServerStateInfo_Add: Failed to execute query\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ServerStateInfo_Get(DBConnectionManager* manager, int category, int code, ServerStateInfo* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager || !record) {
        fprintf(stderr, "ServerStateInfo_Get: Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT category, code, HEX(state) as state, "
        "DATE_FORMAT(start_time, '%%Y-%%m-%%d %%H:%%i:%%s') as start_time, "
        "DATE_FORMAT(end_time, '%%Y-%%m-%%d %%H:%%i:%%s') as end_time "
        "FROM server_state_info WHERE category = %d AND code = %d",
        category, code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        fprintf(stderr, "ServerStateInfo_Get: Failed to execute query\n");
        return -1;
    }

    if (result.row_count == 0) {
        fprintf(stderr, "ServerStateInfo_Get: Record not found (category=%d, code=%d)\n",
                category, code);
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ServerStateInfo));

    /* 获取一行数据 */
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int col = 0;
    record->category = atoi(values[col++]);
    record->code = atoi(values[col++]);

    /* 解析state字段（十六进制字符串）*/
    const char* hex_state = values[col++];
    if (hex_state) {
        int i;
        for (i = 0; i < SERVER_STATE_SIZE && hex_state[i * 2] != '\0'; i++) {
            unsigned int byte;
            sscanf(hex_state + i * 2, "%02X", &byte);
            record->state[i] = (unsigned char)byte;
        }
        /* 填充剩余字节 */
        for (; i < SERVER_STATE_SIZE; i++) {
            record->state[i] = 0;
        }
    }

    snprintf(record->start_time, sizeof(record->start_time), "%s",
             values[col] ? values[col] : "0000-00-00 00:00:00");
    col++;
    snprintf(record->end_time, sizeof(record->end_time), "%s",
             values[col] ? values[col] : "0000-00-00 00:00:00");

    DBQueryResult_Free(&result);
    return 0;
}

int ServerStateInfo_Update(DBConnectionManager* manager, const ServerStateInfo* record) {
    char query[MAX_QUERY_LEN];
    char escaped_state[SERVER_STATE_SIZE * 2 + 1];
    DBQueryResult result;

    if (!manager || !record) {
        fprintf(stderr, "ServerStateInfo_Update: Invalid parameters\n");
        return -1;
    }

    /* 转义state字段为十六进制字符串 */
    int i;
    for (i = 0; i < SERVER_STATE_SIZE; i++) {
        sprintf(escaped_state + i * 2, "%02X", record->state[i]);
    }
    escaped_state[SERVER_STATE_SIZE * 2] = '\0';

    snprintf(query, sizeof(query),
        "UPDATE server_state_info SET state = UNHEX('%s'), start_time = '%s', end_time = '%s' "
        "WHERE category = %d AND code = %d",
        escaped_state, record->start_time, record->end_time,
        record->category, record->code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        fprintf(stderr, "ServerStateInfo_Update: Failed to execute query\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ServerStateInfo_Delete(DBConnectionManager* manager, int category, int code) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        fprintf(stderr, "ServerStateInfo_Delete: Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM server_state_info WHERE category = %d AND code = %d",
        category, code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        fprintf(stderr, "ServerStateInfo_Delete: Failed to execute query\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ServerStateInfo_Exists(DBConnectionManager* manager, int category, int code) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager) {
        fprintf(stderr, "ServerStateInfo_Exists: Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM server_state_info WHERE category = %d AND code = %d",
        category, code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        fprintf(stderr, "ServerStateInfo_Exists: Failed to execute query\n");
        return -1;
    }

    int exists = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0) ? 1 : 0;
    }
    DBQueryResult_Free(&result);
    return exists;
}

/* ========== 专项操作实现 ========== */

int ServerStateInfo_UpdateState(DBConnectionManager* manager, int category, int code, const unsigned char* state) {
    char query[MAX_QUERY_LEN];
    char escaped_state[SERVER_STATE_SIZE * 2 + 1];
    DBQueryResult result;

    if (!manager || !state) {
        fprintf(stderr, "ServerStateInfo_UpdateState: Invalid parameters\n");
        return -1;
    }

    /* 转义state字段为十六进制字符串 */
    int i;
    for (i = 0; i < SERVER_STATE_SIZE; i++) {
        sprintf(escaped_state + i * 2, "%02X", state[i]);
    }
    escaped_state[SERVER_STATE_SIZE * 2] = '\0';

    snprintf(query, sizeof(query),
        "UPDATE server_state_info SET state = UNHEX('%s') WHERE category = %d AND code = %d",
        escaped_state, category, code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        fprintf(stderr, "ServerStateInfo_UpdateState: Failed to execute query\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ServerStateInfo_UpdateTimeRange(DBConnectionManager* manager, int category, int code,
                                    const char* start_time, const char* end_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !start_time || !end_time) {
        fprintf(stderr, "ServerStateInfo_UpdateTimeRange: Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE server_state_info SET start_time = '%s', end_time = '%s' "
        "WHERE category = %d AND code = %d",
        start_time, end_time, category, code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        fprintf(stderr, "ServerStateInfo_UpdateTimeRange: Failed to execute query\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ServerStateInfo_ExtendEndTime(DBConnectionManager* manager, int category, int code, int seconds) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        fprintf(stderr, "ServerStateInfo_ExtendEndTime: Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE server_state_info SET end_time = DATE_ADD(end_time, INTERVAL %d SECOND) "
        "WHERE category = %d AND code = %d",
        seconds, category, code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        fprintf(stderr, "ServerStateInfo_ExtendEndTime: Failed to execute query\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ========== 查询操作实现 ========== */

int ServerStateInfo_CountByCategory(DBConnectionManager* manager, int category) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager) {
        fprintf(stderr, "ServerStateInfo_CountByCategory: Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM server_state_info WHERE category = %d",
        category);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        fprintf(stderr, "ServerStateInfo_CountByCategory: Failed to execute query\n");
        return -1;
    }

    int count = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }
    DBQueryResult_Free(&result);
    return count;
}

int ServerStateInfo_CountActive(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager) {
        fprintf(stderr, "ServerStateInfo_CountActive: Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM server_state_info WHERE end_time > NOW()");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        fprintf(stderr, "ServerStateInfo_CountActive: Failed to execute query\n");
        return -1;
    }

    int count = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }
    DBQueryResult_Free(&result);
    return count;
}

int ServerStateInfo_DeleteByCategory(DBConnectionManager* manager, int category) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        fprintf(stderr, "ServerStateInfo_DeleteByCategory: Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM server_state_info WHERE category = %d",
        category);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        fprintf(stderr, "ServerStateInfo_DeleteByCategory: Failed to execute query\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ServerStateInfo_DeleteExpired(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        fprintf(stderr, "ServerStateInfo_DeleteExpired: Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM server_state_info WHERE end_time < NOW()");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        fprintf(stderr, "ServerStateInfo_DeleteExpired: Failed to execute query\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ServerStateInfo_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager) {
        fprintf(stderr, "ServerStateInfo_Count: Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM server_state_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        fprintf(stderr, "ServerStateInfo_Count: Failed to execute query\n");
        return -1;
    }

    int count = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }
    DBQueryResult_Free(&result);
    return count;
}

/* ========== 工具函数实现 ========== */

void ServerStateInfo_PrintInfo(const ServerStateInfo* record) {
    if (!record) {
        printf("ServerStateInfo: NULL\n");
        return;
    }

    printf("=== Server State Info ===\n");
    printf("Category: %d\n", record->category);
    printf("Code: %d\n", record->code);

    /* 打印state字段（十六进制）*/
    printf("State: ");
    int i;
    for (i = 0; i < SERVER_STATE_SIZE; i++) {
        printf("%02X ", record->state[i]);
    }
    printf("\n");

    printf("Start Time: %s\n", record->start_time);
    printf("End Time: %s\n", record->end_time);
    printf("========================\n");
}
