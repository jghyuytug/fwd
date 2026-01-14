#ifndef ACCOUNT_CARGO_MOD_ORM_H
#define ACCOUNT_CARGO_MOD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    int m_id;
    signed char server_info;
    int money_old;
    signed char cargo_capa_old;
    int money_new;
    signed char cargo_capa_new;
    char mng_user_id[31];
    char work_time[20];
    char work_type;
    char memo[256];
} AccountCargoMod;

/* CRUD Operations */
int AccountCargoMod_Add(DBConnectionManager* manager, const AccountCargoMod* record);
int AccountCargoMod_Get(DBConnectionManager* manager, int id, AccountCargoMod* record);
int AccountCargoMod_Update(DBConnectionManager* manager, const AccountCargoMod* record);
int AccountCargoMod_Delete(DBConnectionManager* manager, int id);
int AccountCargoMod_Exists(DBConnectionManager* manager, int id);
int AccountCargoMod_GetAll(DBConnectionManager* manager, AccountCargoMod* records, int max_count, int* actual_count);

#endif /* ACCOUNT_CARGO_MOD_ORM_H */