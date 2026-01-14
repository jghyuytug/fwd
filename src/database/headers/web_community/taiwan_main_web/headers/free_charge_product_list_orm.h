#ifndef FREE_CHARGE_PRODUCT_LIST_ORM_H
#define FREE_CHARGE_PRODUCT_LIST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int product_cd;
    char product_name[51];
    int sera_amount;
    int item_cd;
    char reg_date[20];
} FreeChargeProductList;

/* CRUD Operations */
int FreeChargeProductList_Add(DBConnectionManager* manager, const FreeChargeProductList* record);
int FreeChargeProductList_Get(DBConnectionManager* manager, int product_cd, FreeChargeProductList* record);
int FreeChargeProductList_Update(DBConnectionManager* manager, const FreeChargeProductList* record);
int FreeChargeProductList_Delete(DBConnectionManager* manager, int product_cd);
int FreeChargeProductList_Exists(DBConnectionManager* manager, int product_cd);
int FreeChargeProductList_GetAll(DBConnectionManager* manager, FreeChargeProductList* records, int max_count, int* actual_count);

#endif /* FREE_CHARGE_PRODUCT_LIST_ORM_H */