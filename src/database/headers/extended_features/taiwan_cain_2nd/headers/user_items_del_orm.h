#ifndef USER_ITEMS_DEL_ORM_H
#define USER_ITEMS_DEL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char sdate[11];
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
    unsigned char jewel_socket[30];
    signed char item_lock_key;
    char to_ipg_agency_no[33];
    char m_time[20];
    short hidden_option;
    short emblem_endurance;
    short color1;
    short color2;
    signed char trade_restrict;
} UserItemsDel;

/* CRUD Operations */
int UserItemsDel_Add(DBConnectionManager* manager, const UserItemsDel* record);
int UserItemsDel_Get(DBConnectionManager* manager, const char* sdate, int ui_id, UserItemsDel* record);
int UserItemsDel_Update(DBConnectionManager* manager, const UserItemsDel* record);
int UserItemsDel_Delete(DBConnectionManager* manager, const char* sdate, int ui_id);
int UserItemsDel_Exists(DBConnectionManager* manager, const char* sdate, int ui_id);
int UserItemsDel_GetAll(DBConnectionManager* manager, UserItemsDel* records, int max_count, int* actual_count);

#endif /* USER_ITEMS_DEL_ORM_H */