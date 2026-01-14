#ifndef QQ_BOT_CONFIG_TEST_ORM_H
#define QQ_BOT_CONFIG_TEST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int Id;
    long long Uin;
    char PassWord[256];
    char QQGroupNumbersText[1024];
    signed char AllowSignIn;
    signed char AllowBlackList;
    signed char AllowCloudBlackList;
    short MinJoinGroupQQLevel;
    char JoinGroupWelcome[1024];
    char LeaveGroupWelcome[1024];
    signed char AllowChatGPTMess;
    char OpenAiApiKey[256];
} QqBotConfigTest;

/* CRUD Operations */
int QqBotConfigTest_Add(DBConnectionManager* manager, const QqBotConfigTest* record);
int QqBotConfigTest_Get(DBConnectionManager* manager, int Id, QqBotConfigTest* record);
int QqBotConfigTest_Update(DBConnectionManager* manager, const QqBotConfigTest* record);
int QqBotConfigTest_Delete(DBConnectionManager* manager, int Id);
int QqBotConfigTest_Exists(DBConnectionManager* manager, int Id);
int QqBotConfigTest_GetAll(DBConnectionManager* manager, QqBotConfigTest* records, int max_count, int* actual_count);

#endif /* QQ_BOT_CONFIG_TEST_ORM_H */