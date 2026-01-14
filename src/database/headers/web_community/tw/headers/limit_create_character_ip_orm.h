#ifndef LIMIT_CREATE_CHARACTER_IP_ORM_H
#define LIMIT_CREATE_CHARACTER_IP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int ip;
    char ip_str[17];
    char last_access_time[20];
    int count;
    int last_access_mid;
} LimitCreateCharacterIp;

/* CRUD Operations */
int LimitCreateCharacterIp_Add(DBConnectionManager* manager, const LimitCreateCharacterIp* record);
int LimitCreateCharacterIp_Get(DBConnectionManager* manager, int ip, LimitCreateCharacterIp* record);
int LimitCreateCharacterIp_Update(DBConnectionManager* manager, const LimitCreateCharacterIp* record);
int LimitCreateCharacterIp_Delete(DBConnectionManager* manager, int ip);
int LimitCreateCharacterIp_Exists(DBConnectionManager* manager, int ip);
int LimitCreateCharacterIp_GetAll(DBConnectionManager* manager, LimitCreateCharacterIp* records, int max_count, int* actual_count);

#endif /* LIMIT_CREATE_CHARACTER_IP_ORM_H */