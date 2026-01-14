#ifndef MEMBER_PUNISH_HACK_ORM_H
#define MEMBER_PUNISH_HACK_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int occ_time;
    int period;
    signed char now_flag;
    signed char auto_flag;
    char reason[251];
    char hack_ip[16];
} MemberPunishHack;

/* CRUD Operations */
int MemberPunishHack_Add(DBConnectionManager* manager, const MemberPunishHack* record);
int MemberPunishHack_Get(DBConnectionManager* manager, int m_id, MemberPunishHack* record);
int MemberPunishHack_Update(DBConnectionManager* manager, const MemberPunishHack* record);
int MemberPunishHack_Delete(DBConnectionManager* manager, int m_id);
int MemberPunishHack_Exists(DBConnectionManager* manager, int m_id);
int MemberPunishHack_GetAll(DBConnectionManager* manager, MemberPunishHack* records, int max_count, int* actual_count);

#endif /* MEMBER_PUNISH_HACK_ORM_H */