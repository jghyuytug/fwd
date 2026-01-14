#ifndef MEMBER_PUNISH_HACK_HISTORY_ORM_H
#define MEMBER_PUNISH_HACK_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int occ_time;
    int period;
    signed char now_flag;
    signed char auto_flag;
    char reason[251];
} MemberPunishHackHistory;

/* CRUD Operations */
int MemberPunishHackHistory_Add(DBConnectionManager* manager, const MemberPunishHackHistory* record);
int MemberPunishHackHistory_GetAll(DBConnectionManager* manager, MemberPunishHackHistory* records, int max_count, int* actual_count);

#endif /* MEMBER_PUNISH_HACK_HISTORY_ORM_H */