#ifndef PVF_ITEM_OTHER_ORM_H
#define PVF_ITEM_OTHER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int Id;
    int ItemCode;
    char ItemName[256];
    int ShopId;
    signed char Type;
} PvfItemOther;

/* CRUD Operations */
int PvfItemOther_Add(DBConnectionManager* manager, const PvfItemOther* record);
int PvfItemOther_Get(DBConnectionManager* manager, int Id, PvfItemOther* record);
int PvfItemOther_Update(DBConnectionManager* manager, const PvfItemOther* record);
int PvfItemOther_Delete(DBConnectionManager* manager, int Id);
int PvfItemOther_Exists(DBConnectionManager* manager, int Id);
int PvfItemOther_GetAll(DBConnectionManager* manager, PvfItemOther* records, int max_count, int* actual_count);

#endif /* PVF_ITEM_OTHER_ORM_H */