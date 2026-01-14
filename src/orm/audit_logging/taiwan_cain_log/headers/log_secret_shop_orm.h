#ifndef LOG_SECRET_SHOP_ORM_H
#define LOG_SECRET_SHOP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char npc_idx;
    char occ_date[11];
    int dungeon_idx;
    int show_count;
    int show_charac_count;
    int buy_count;
    int price;
} LogSecretShop;

/* CRUD Operations */
int LogSecretShop_Add(DBConnectionManager* manager, const LogSecretShop* record);
int LogSecretShop_Get(DBConnectionManager* manager, signed char npc_idx, const char* occ_date, int dungeon_idx, LogSecretShop* record);
int LogSecretShop_Update(DBConnectionManager* manager, const LogSecretShop* record);
int LogSecretShop_Delete(DBConnectionManager* manager, signed char npc_idx, const char* occ_date, int dungeon_idx);
int LogSecretShop_Exists(DBConnectionManager* manager, signed char npc_idx, const char* occ_date, int dungeon_idx);
int LogSecretShop_GetAll(DBConnectionManager* manager, LogSecretShop* records, int max_count, int* actual_count);

#endif /* LOG_SECRET_SHOP_ORM_H */