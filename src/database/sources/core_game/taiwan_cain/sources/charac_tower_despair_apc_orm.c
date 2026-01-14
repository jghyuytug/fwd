#include "charac_tower_despair_apc_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* ==================== CRUD 操作实现 ==================== */

int CharacTowerDespairApc_Add(DBConnectionManager* manager, const CharacTowerDespairApc* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_tower_despair_apc "
        "(reg_date, charac_no, seq) "
        "VALUES ('%s', %d, %d)",
        record->reg_date,
        record->charac_no,
        record->seq);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerDespairApc_Get(DBConnectionManager* manager, const char* reg_date, int seq, CharacTowerDespairApc* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];

    if (!manager || !reg_date || !record) {
        return -1;
    }

    memset(record, 0, sizeof(CharacTowerDespairApc));

    snprintf(query, sizeof(query),
        "SELECT reg_date, charac_no, seq "
        "FROM charac_tower_despair_apc WHERE reg_date = '%s' AND seq = %d",
        reg_date, seq);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        if (values[0]) {
            strncpy(record->reg_date, values[0], sizeof(record->reg_date) - 1);
        }
        record->charac_no = atoi(values[1]);
        record->seq = atoi(values[2]);

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacTowerDespairApc_Update(DBConnectionManager* manager, const CharacTowerDespairApc* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_tower_despair_apc SET "
        "charac_no = %d "
        "WHERE reg_date = '%s' AND seq = %d",
        record->charac_no,
        record->reg_date,
        record->seq);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerDespairApc_Delete(DBConnectionManager* manager, const char* reg_date, int seq) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !reg_date) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_tower_despair_apc WHERE reg_date = '%s' AND seq = %d", reg_date, seq);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerDespairApc_Exists(DBConnectionManager* manager, const char* reg_date, int seq) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager || !reg_date) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_tower_despair_apc WHERE reg_date = '%s' AND seq = %d",
        reg_date, seq);

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

/* ==================== 查询操作实现 ==================== */

int CharacTowerDespairApc_GetByDate(DBConnectionManager* manager, const char* reg_date,
                                     CharacTowerDespairApc* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];
    int count = 0;

    if (!manager || !reg_date || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT reg_date, charac_no, seq "
        "FROM charac_tower_despair_apc WHERE reg_date = '%s' ORDER BY seq LIMIT %d",
        reg_date, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        if (values[0]) {
            strncpy(records[count].reg_date, values[0], sizeof(records[count].reg_date) - 1);
        }
        records[count].charac_no = atoi(values[1]);
        records[count].seq = atoi(values[2]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacTowerDespairApc_GetByCharacNo(DBConnectionManager* manager, int charac_no,
                                         CharacTowerDespairApc* records, int max_result) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[16];
    int count = 0;

    if (!manager || !records || max_result <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT reg_date, charac_no, seq "
        "FROM charac_tower_despair_apc WHERE charac_no = %d ORDER BY reg_date DESC, seq LIMIT %d",
        charac_no, max_result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0 && count < max_result) {
        if (values[0]) {
            strncpy(records[count].reg_date, values[0], sizeof(records[count].reg_date) - 1);
        }
        records[count].charac_no = atoi(values[1]);
        records[count].seq = atoi(values[2]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacTowerDespairApc_GetCountByDate(DBConnectionManager* manager, const char* reg_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager || !reg_date) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_tower_despair_apc WHERE reg_date = '%s'",
        reg_date);

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

int CharacTowerDespairApc_GetCountByCharacNo(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_tower_despair_apc WHERE charac_no = %d",
        charac_no);

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

int CharacTowerDespairApc_GetNextSeq(DBConnectionManager* manager, const char* reg_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager || !reg_date) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT IFNULL(MAX(seq), 0) + 1 FROM charac_tower_despair_apc WHERE reg_date = '%s'",
        reg_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int next_seq = atoi(values[0]);
        DBQueryResult_Free(&result);
        return next_seq;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacTowerDespairApc_DeleteByDate(DBConnectionManager* manager, const char* reg_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !reg_date) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_tower_despair_apc WHERE reg_date = '%s'", reg_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacTowerDespairApc_DeleteByCharacNo(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_tower_despair_apc WHERE charac_no = %d", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 工具函数实现 ==================== */

void CharacTowerDespairApc_PrintInfo(const CharacTowerDespairApc* record) {
    if (!record) {
        printf("CharacTowerDespairApc: NULL\n");
        return;
    }

    printf("CharacTowerDespairApc {\n");
    printf("  reg_date: %s\n", record->reg_date);
    printf("  charac_no: %d\n", record->charac_no);
    printf("  seq: %d\n", record->seq);
    printf("}\n");
}
