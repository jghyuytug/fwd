#ifndef DNF_ITEM_PRICE_ORM_H
#define DNF_ITEM_PRICE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int it_no;
    int it_cnt;
    int cera_price;
    int ipg_no;
} DnfItemPrice;

/* CRUD Operations */
int DnfItemPrice_Add(DBConnectionManager* manager, const DnfItemPrice* record);
int DnfItemPrice_Get(DBConnectionManager* manager, int ipg_no, DnfItemPrice* record);
int DnfItemPrice_Update(DBConnectionManager* manager, const DnfItemPrice* record);
int DnfItemPrice_Delete(DBConnectionManager* manager, int ipg_no);
int DnfItemPrice_Exists(DBConnectionManager* manager, int ipg_no);
int DnfItemPrice_GetAll(DBConnectionManager* manager, DnfItemPrice* records, int max_count, int* actual_count);

#endif /* DNF_ITEM_PRICE_ORM_H */