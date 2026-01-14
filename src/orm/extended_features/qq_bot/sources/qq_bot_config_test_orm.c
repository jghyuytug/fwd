#include "qq_bot_config_test_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int QqBotConfigTest_Add(DBConnectionManager* manager, const QqBotConfigTest* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO qq_bot_config_test (Uin, PassWord, QQGroupNumbersText, AllowSignIn, AllowBlackList, AllowCloudBlackList, MinJoinGroupQQLevel, JoinGroupWelcome, LeaveGroupWelcome, AllowChatGPTMess, OpenAiApiKey) "
        "VALUES (%d, '%s', '%s', %d, %d, %d, %d, '%s', '%s', %d, '%s')",
        record->Uin, record->PassWord, record->QQGroupNumbersText, record->AllowSignIn, record->AllowBlackList, record->AllowCloudBlackList, record->MinJoinGroupQQLevel, record->JoinGroupWelcome, record->LeaveGroupWelcome, record->AllowChatGPTMess, record->OpenAiApiKey);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int QqBotConfigTest_Get(DBConnectionManager* manager, int Id, QqBotConfigTest* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, Uin, PassWord, QQGroupNumbersText, AllowSignIn, AllowBlackList, AllowCloudBlackList, MinJoinGroupQQLevel, JoinGroupWelcome, LeaveGroupWelcome, AllowChatGPTMess, OpenAiApiKey FROM qq_bot_config_test WHERE Id = %d",
        Id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(QqBotConfigTest));
    record->Id = row[0] ? atoi(row[0]) : 0;
    record->Uin = row[1] ? atoll(row[1]) : 0;
    if (row[2]) strncpy(record->PassWord, row[2], sizeof(record->PassWord) - 1);
    if (row[3]) strncpy(record->QQGroupNumbersText, row[3], sizeof(record->QQGroupNumbersText) - 1);
    record->AllowSignIn = row[4] ? atoi(row[4]) : 0;
    record->AllowBlackList = row[5] ? atoi(row[5]) : 0;
    record->AllowCloudBlackList = row[6] ? atoi(row[6]) : 0;
    record->MinJoinGroupQQLevel = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->JoinGroupWelcome, row[8], sizeof(record->JoinGroupWelcome) - 1);
    if (row[9]) strncpy(record->LeaveGroupWelcome, row[9], sizeof(record->LeaveGroupWelcome) - 1);
    record->AllowChatGPTMess = row[10] ? atoi(row[10]) : 0;
    if (row[11]) strncpy(record->OpenAiApiKey, row[11], sizeof(record->OpenAiApiKey) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int QqBotConfigTest_GetAll(DBConnectionManager* manager, QqBotConfigTest* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, Uin, PassWord, QQGroupNumbersText, AllowSignIn, AllowBlackList, AllowCloudBlackList, MinJoinGroupQQLevel, JoinGroupWelcome, LeaveGroupWelcome, AllowChatGPTMess, OpenAiApiKey FROM qq_bot_config_test");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(QqBotConfigTest));
        records[count].Id = row[0] ? atoi(row[0]) : 0;
        records[count].Uin = row[1] ? atoll(row[1]) : 0;
        if (row[2]) strncpy(records[count].PassWord, row[2], sizeof(records[count].PassWord) - 1);
        if (row[3]) strncpy(records[count].QQGroupNumbersText, row[3], sizeof(records[count].QQGroupNumbersText) - 1);
        records[count].AllowSignIn = row[4] ? atoi(row[4]) : 0;
        records[count].AllowBlackList = row[5] ? atoi(row[5]) : 0;
        records[count].AllowCloudBlackList = row[6] ? atoi(row[6]) : 0;
        records[count].MinJoinGroupQQLevel = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].JoinGroupWelcome, row[8], sizeof(records[count].JoinGroupWelcome) - 1);
        if (row[9]) strncpy(records[count].LeaveGroupWelcome, row[9], sizeof(records[count].LeaveGroupWelcome) - 1);
        records[count].AllowChatGPTMess = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].OpenAiApiKey, row[11], sizeof(records[count].OpenAiApiKey) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
