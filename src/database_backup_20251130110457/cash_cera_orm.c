/**
 * DNF游戏服务器 - CashCera表ORM实现
 */

#define _GNU_SOURCE
#include "cash_cera_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ========================================================================
// 内部辅助函数
// ========================================================================

/**
 * 获取当前时间字符串 (YYYY-MM-DD HH:MM:SS)
 */
static void GetCurrentTimeString(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    snprintf(buffer, size, "%04d-%02d-%02d %02d:%02d:%02d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
}

/**
 * 从查询结果行填充点券结构
 */
static void FillCashFromRow(char** values, CashCera* cash) {
    cash->account_id = atoi(values[0]);
    cash->cera_point = strtoll(values[1], NULL, 10);
    strncpy(cash->update_time, values[2], sizeof(cash->update_time) - 1);
}

// ========================================================================
// CRUD操作实现
// ========================================================================

int CashCera_Create(DBConnectionManager* manager, CashCera* cash) {
    char query[512];
    char update_time[20];

    // 获取当前时间
    GetCurrentTimeString(update_time, sizeof(update_time));
    strncpy(cash->update_time, update_time, sizeof(cash->update_time) - 1);

    // 构建INSERT查询
    snprintf(query, sizeof(query),
             "INSERT INTO cash_cera (account_id, cera_point, update_time) "
             "VALUES (%d, %lld, '%s')",
             cash->account_id, cash->cera_point, cash->update_time);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);

    if (ret > 0) {
        printf("[CashCera ORM] Created cash record for account_id=%d, cera=%lld\n",
               cash->account_id, cash->cera_point);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

int CashCera_GetByAccountId(DBConnectionManager* manager, int account_id, CashCera* cash) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT account_id, cera_point, update_time "
             "FROM cash_cera WHERE account_id = %d",
             account_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    // 使用DBQueryResult_FetchRow获取行数据
    char* values[3];
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        FillCashFromRow(values, cash);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CashCera_Update(DBConnectionManager* manager, const CashCera* cash) {
    char query[512];
    char update_time[20];

    // 更新修改时间
    GetCurrentTimeString(update_time, sizeof(update_time));

    snprintf(query, sizeof(query),
             "UPDATE cash_cera SET cera_point=%lld, update_time='%s' "
             "WHERE account_id=%d",
             cash->cera_point, update_time, cash->account_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);

    if (ret > 0) {
        printf("[CashCera ORM] Updated cash for account_id=%d\n", cash->account_id);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

int CashCera_Delete(DBConnectionManager* manager, int account_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM cash_cera WHERE account_id=%d", account_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);

    if (ret >= 0) {  // DELETE can affect 0 rows if record doesn't exist
        printf("[CashCera ORM] Deleted cash record for account_id=%d\n", account_id);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

// ========================================================================
// 点券操作函数实现
// ========================================================================

int CashCera_AddCera(DBConnectionManager* manager, int account_id, long long amount) {
    char query[512];
    char update_time[20];

    GetCurrentTimeString(update_time, sizeof(update_time));

    // 使用原子操作增加点券
    snprintf(query, sizeof(query),
             "UPDATE cash_cera SET cera_point=cera_point+%lld, update_time='%s' "
             "WHERE account_id=%d",
             amount, update_time, account_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);

    if (ret > 0) {
        printf("[CashCera ORM] Added %lld cera to account_id=%d\n",
               amount, account_id);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

int CashCera_SubtractCera(DBConnectionManager* manager, int account_id, long long amount) {
    char query[512];
    char update_time[20];

    GetCurrentTimeString(update_time, sizeof(update_time));

    // 先检查余额是否足够
    long long current_cera = CashCera_GetCeraAmount(manager, account_id);
    if (current_cera < 0) {
        printf("[CashCera ORM] Error: account_id=%d not found\n", account_id);
        return -1;
    }

    if (current_cera < amount) {
        printf("[CashCera ORM] Error: Insufficient cera (have=%lld, need=%lld)\n",
               current_cera, amount);
        return -2;  // Insufficient balance
    }

    // 使用原子操作扣除点券
    snprintf(query, sizeof(query),
             "UPDATE cash_cera SET cera_point=cera_point-%lld, update_time='%s' "
             "WHERE account_id=%d",
             amount, update_time, account_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);

    if (ret > 0) {
        printf("[CashCera ORM] Subtracted %lld cera from account_id=%d\n",
               amount, account_id);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

int CashCera_UpdateCera(DBConnectionManager* manager, int account_id, long long new_cera) {
    char query[512];
    char update_time[20];

    GetCurrentTimeString(update_time, sizeof(update_time));

    snprintf(query, sizeof(query),
             "UPDATE cash_cera SET cera_point=%lld, update_time='%s' "
             "WHERE account_id=%d",
             new_cera, update_time, account_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);

    if (ret > 0) {
        printf("[CashCera ORM] Updated cera for account_id=%d to %lld\n",
               account_id, new_cera);
        ret = 0;  // Success
    } else {
        ret = -1;  // Failure
    }

    DBQueryResult_Free(&result);
    return ret;
}

// ========================================================================
// 辅助查询函数实现
// ========================================================================

bool CashCera_Exists(DBConnectionManager* manager, int account_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM cash_cera WHERE account_id=%d", account_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return false;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);

    return (count > 0);
}

long long CashCera_GetCeraAmount(DBConnectionManager* manager, int account_id) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT cera_point FROM cash_cera WHERE account_id=%d", account_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    long long cera = strtoll(values[0], NULL, 10);
    DBQueryResult_Free(&result);

    return cera;
}

int CashCera_GetCount(DBConnectionManager* manager) {
    char query[128];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM cash_cera");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);

    return count;
}

int CashCera_GetAll(DBConnectionManager* manager, CashCera* records, int max_count) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT account_id, cera_point, update_time "
             "FROM cash_cera "
             "ORDER BY cera_point DESC LIMIT %d",
             max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = (result.row_count < max_count) ? result.row_count : max_count;

    char* values[3];
    int i;
    for (i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillCashFromRow(values, &records[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

int CashCera_GetByRange(DBConnectionManager* manager, long long min_cera,
                        long long max_cera, CashCera* records, int max_count) {
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT account_id, cera_point, update_time "
             "FROM cash_cera "
             "WHERE cera_point >= %lld AND cera_point <= %lld "
             "ORDER BY cera_point DESC LIMIT %d",
             min_cera, max_cera, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_BILLING, query, &result);

    if (ret <= 0) {  // No rows or error
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = (result.row_count < max_count) ? result.row_count : max_count;

    char* values[3];
    int i;
    for (i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillCashFromRow(values, &records[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

// ========================================================================
// 工具函数实现
// ========================================================================

void CashCera_Init(CashCera* cash) {
    memset(cash, 0, sizeof(CashCera));
    cash->account_id = 0;
    cash->cera_point = 0;
    strcpy(cash->update_time, "0000-00-00 00:00:00");
}

void CashCera_Print(const CashCera* cash) {
    printf("================================================================================\n");
    printf("CashCera Information:\n");
    printf("================================================================================\n");
    printf("  账号ID:        %d\n", cash->account_id);
    printf("  点券数量:      %lld\n", cash->cera_point);
    printf("  更新时间:      %s\n", cash->update_time);
    printf("================================================================================\n");
}

void CashCera_Copy(CashCera* dest, const CashCera* src) {
    dest->account_id = src->account_id;
    dest->cera_point = src->cera_point;
    strncpy(dest->update_time, src->update_time, sizeof(dest->update_time) - 1);
}
