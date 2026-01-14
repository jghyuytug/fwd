#ifndef USER_ITEMS_WORK_ORM_H
#define USER_ITEMS_WORK_ORM_H

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
} UserItemsWork;

/* CRUD Operations */
int UserItemsWork_Add(DBConnectionManager* manager, const UserItemsWork* record);
int UserItemsWork_GetAll(DBConnectionManager* manager, UserItemsWork* records, int max_count, int* actual_count);

#endif /* USER_ITEMS_WORK_ORM_H */