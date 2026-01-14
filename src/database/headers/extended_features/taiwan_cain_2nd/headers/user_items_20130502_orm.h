#ifndef USER_ITEMS_20130502_ORM_H
#define USER_ITEMS_20130502_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int ui_id;
    int charac_no;
    int slot;
    int it_id;
    char expire_date[20];
    signed char obtain_from;
    char reg_date[20];
    char ipg_agency_no[33];
    signed char ability_no;
    signed char stat;
    int clear_avatar_id;
    signed char item_lock_key;
    char to_ipg_agency_no[33];
    char m_time[20];
    short hidden_option;
    short emblem_endurance;
    int color1;
    int color2;
    int trade_restrict;
} UserItems20130502;

/* CRUD Operations */
int UserItems20130502_Add(DBConnectionManager* manager, const UserItems20130502* record);
int UserItems20130502_Get(DBConnectionManager* manager, int ui_id, UserItems20130502* record);
int UserItems20130502_Update(DBConnectionManager* manager, const UserItems20130502* record);
int UserItems20130502_Delete(DBConnectionManager* manager, int ui_id);
int UserItems20130502_Exists(DBConnectionManager* manager, int ui_id);
int UserItems20130502_GetAll(DBConnectionManager* manager, UserItems20130502* records, int max_count, int* actual_count);

#endif /* USER_ITEMS_20130502_ORM_H */