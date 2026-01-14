#ifndef INVENTORY_ORM_H
#define INVENTORY_ORM_H

#include "db_connection_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int charac_no;
    int money;
    int coin;
    int pay_coin;
    int event_coin;
    signed char creature_flag;
    int inventory_capacity;
    int avatar_coin;
} Inventory;

/* CRUD Operations */
int Inventory_Add(DBConnectionManager* manager, const Inventory* record);
int Inventory_Get(DBConnectionManager* manager, int charac_no, Inventory* record);
int Inventory_Update(DBConnectionManager* manager, const Inventory* record);
int Inventory_Delete(DBConnectionManager* manager, int charac_no);
int Inventory_Exists(DBConnectionManager* manager, int charac_no);
int Inventory_GetAll(DBConnectionManager* manager, Inventory* records, int max_count, int* actual_count);

#ifdef __cplusplus
}
#endif

#endif /* INVENTORY_ORM_H */
