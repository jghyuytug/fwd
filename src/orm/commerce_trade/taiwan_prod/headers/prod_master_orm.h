#ifndef PROD_MASTER_ORM_H
#define PROD_MASTER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    short prod_part_id;
    char prod_part_script[51];
    char prod_part_name[36];
} ProdMaster;

/* CRUD Operations */
int ProdMaster_Add(DBConnectionManager* manager, const ProdMaster* record);
int ProdMaster_Get(DBConnectionManager* manager, short prod_part_id, ProdMaster* record);
int ProdMaster_Update(DBConnectionManager* manager, const ProdMaster* record);
int ProdMaster_Delete(DBConnectionManager* manager, short prod_part_id);
int ProdMaster_Exists(DBConnectionManager* manager, short prod_part_id);
int ProdMaster_GetAll(DBConnectionManager* manager, ProdMaster* records, int max_count, int* actual_count);

#endif /* PROD_MASTER_ORM_H */