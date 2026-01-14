#ifndef MEMBER_PASSWD_MOD_ORM_H
#define MEMBER_PASSWD_MOD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char first_time[20];
    char last_time[20];
    signed char cnt;
} MemberPasswdMod;

/* CRUD Operations */
int MemberPasswdMod_Add(DBConnectionManager* manager, const MemberPasswdMod* record);
int MemberPasswdMod_Get(DBConnectionManager* manager, int m_id, MemberPasswdMod* record);
int MemberPasswdMod_Update(DBConnectionManager* manager, const MemberPasswdMod* record);
int MemberPasswdMod_Delete(DBConnectionManager* manager, int m_id);
int MemberPasswdMod_Exists(DBConnectionManager* manager, int m_id);
int MemberPasswdMod_GetAll(DBConnectionManager* manager, MemberPasswdMod* records, int max_count, int* actual_count);

#endif /* MEMBER_PASSWD_MOD_ORM_H */