#ifndef CHECK_PICK_UP_RANDOM_OPTION_ITEM_ORM_H
#define CHECK_PICK_UP_RANDOM_OPTION_ITEM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char check_count;
} CheckPickUpRandomOptionItem;

/* CRUD Operations */
int CheckPickUpRandomOptionItem_Add(DBConnectionManager* manager, const CheckPickUpRandomOptionItem* record);
int CheckPickUpRandomOptionItem_Get(DBConnectionManager* manager, int m_id, CheckPickUpRandomOptionItem* record);
int CheckPickUpRandomOptionItem_Update(DBConnectionManager* manager, const CheckPickUpRandomOptionItem* record);
int CheckPickUpRandomOptionItem_Delete(DBConnectionManager* manager, int m_id);
int CheckPickUpRandomOptionItem_Exists(DBConnectionManager* manager, int m_id);
int CheckPickUpRandomOptionItem_GetAll(DBConnectionManager* manager, CheckPickUpRandomOptionItem* records, int max_count, int* actual_count);

#endif /* CHECK_PICK_UP_RANDOM_OPTION_ITEM_ORM_H */