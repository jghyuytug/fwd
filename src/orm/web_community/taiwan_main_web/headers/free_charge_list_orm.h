#ifndef FREE_CHARGE_LIST_ORM_H
#define FREE_CHARGE_LIST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int tid;
    int m_id;
    int product_cd;
    int sera_amount;
    int item_cd;
    char reg_date[20];
} FreeChargeList;

/* CRUD Operations */
int FreeChargeList_Add(DBConnectionManager* manager, const FreeChargeList* record);
int FreeChargeList_Get(DBConnectionManager* manager, int tid, FreeChargeList* record);
int FreeChargeList_Update(DBConnectionManager* manager, const FreeChargeList* record);
int FreeChargeList_Delete(DBConnectionManager* manager, int tid);
int FreeChargeList_Exists(DBConnectionManager* manager, int tid);
int FreeChargeList_GetAll(DBConnectionManager* manager, FreeChargeList* records, int max_count, int* actual_count);

#endif /* FREE_CHARGE_LIST_ORM_H */