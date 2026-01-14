#ifndef PVF_ITEM_ORM_H
#define PVF_ITEM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int Id;
    int ItemCode;
    char ItemName[256];
    int IconId;
    int IconIndex;
    signed char Rarity;
    signed char ItemCodeType;
    int StackLimit;
} PvfItem;

/* CRUD Operations */
int PvfItem_Add(DBConnectionManager* manager, const PvfItem* record);
int PvfItem_Get(DBConnectionManager* manager, int Id, PvfItem* record);
int PvfItem_Update(DBConnectionManager* manager, const PvfItem* record);
int PvfItem_Delete(DBConnectionManager* manager, int Id);
int PvfItem_Exists(DBConnectionManager* manager, int Id);
int PvfItem_GetAll(DBConnectionManager* manager, PvfItem* records, int max_count, int* actual_count);

#endif /* PVF_ITEM_ORM_H */