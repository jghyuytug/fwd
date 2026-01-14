#ifndef ITEM_PART_SET_ORM_H
#define ITEM_PART_SET_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int idx;
    int part_set_index;
    char part_name[256];
    int part_type;
    int part_grade;
    int part_rarity;
} ItemPartSet;

/* CRUD Operations */
int ItemPartSet_Add(DBConnectionManager* manager, const ItemPartSet* record);
int ItemPartSet_Get(DBConnectionManager* manager, int idx, ItemPartSet* record);
int ItemPartSet_Update(DBConnectionManager* manager, const ItemPartSet* record);
int ItemPartSet_Delete(DBConnectionManager* manager, int idx);
int ItemPartSet_Exists(DBConnectionManager* manager, int idx);
int ItemPartSet_GetAll(DBConnectionManager* manager, ItemPartSet* records, int max_count, int* actual_count);

#endif /* ITEM_PART_SET_ORM_H */