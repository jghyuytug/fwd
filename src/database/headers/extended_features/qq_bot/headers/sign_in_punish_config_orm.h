#ifndef SIGN_IN_PUNISH_CONFIG_ORM_H
#define SIGN_IN_PUNISH_CONFIG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int Id;
    int Type;
    int MinValue;
    int MaxValue;
} SignInPunishConfig;

/* CRUD Operations */
int SignInPunishConfig_Add(DBConnectionManager* manager, const SignInPunishConfig* record);
int SignInPunishConfig_Get(DBConnectionManager* manager, int Id, SignInPunishConfig* record);
int SignInPunishConfig_Update(DBConnectionManager* manager, const SignInPunishConfig* record);
int SignInPunishConfig_Delete(DBConnectionManager* manager, int Id);
int SignInPunishConfig_Exists(DBConnectionManager* manager, int Id);
int SignInPunishConfig_GetAll(DBConnectionManager* manager, SignInPunishConfig* records, int max_count, int* actual_count);

#endif /* SIGN_IN_PUNISH_CONFIG_ORM_H */