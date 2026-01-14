#ifndef MEMBER_PREMIUM_MOD_ORM_H
#define MEMBER_PREMIUM_MOD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    int m_id;
    int event_id_old;
    signed char server_id_old;
    char service_start_old[20];
    char service_end_old[20];
    char service_end_new[20];
    char MNG_user_id[31];
    char work_time[20];
    signed char pre_type;
} MemberPremiumMod;

/* CRUD Operations */
int MemberPremiumMod_Add(DBConnectionManager* manager, const MemberPremiumMod* record);
int MemberPremiumMod_Get(DBConnectionManager* manager, int id, MemberPremiumMod* record);
int MemberPremiumMod_Update(DBConnectionManager* manager, const MemberPremiumMod* record);
int MemberPremiumMod_Delete(DBConnectionManager* manager, int id);
int MemberPremiumMod_Exists(DBConnectionManager* manager, int id);
int MemberPremiumMod_GetAll(DBConnectionManager* manager, MemberPremiumMod* records, int max_count, int* actual_count);

#endif /* MEMBER_PREMIUM_MOD_ORM_H */