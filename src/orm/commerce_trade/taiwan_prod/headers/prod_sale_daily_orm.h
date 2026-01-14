#ifndef PROD_SALE_DAILY_ORM_H
#define PROD_SALE_DAILY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char sale_date[11];
    short slot_part_code;
    int ipg_no;
    int volume;
    long long amount;
    int un_m_id;
    int un_charac;
} ProdSaleDaily;

/* CRUD Operations */
int ProdSaleDaily_Add(DBConnectionManager* manager, const ProdSaleDaily* record);
int ProdSaleDaily_Get(DBConnectionManager* manager, const char* sale_date, int ipg_no, ProdSaleDaily* record);
int ProdSaleDaily_Update(DBConnectionManager* manager, const ProdSaleDaily* record);
int ProdSaleDaily_Delete(DBConnectionManager* manager, const char* sale_date, int ipg_no);
int ProdSaleDaily_Exists(DBConnectionManager* manager, const char* sale_date, int ipg_no);
int ProdSaleDaily_GetAll(DBConnectionManager* manager, ProdSaleDaily* records, int max_count, int* actual_count);

#endif /* PROD_SALE_DAILY_ORM_H */