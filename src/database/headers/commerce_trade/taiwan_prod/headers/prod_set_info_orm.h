#ifndef PROD_SET_INFO_ORM_H
#define PROD_SET_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int ipg_no;
    int consist_ipg_no;
    signed char fix_flag;
} ProdSetInfo;

/* CRUD Operations */
int ProdSetInfo_Add(DBConnectionManager* manager, const ProdSetInfo* record);
int ProdSetInfo_Get(DBConnectionManager* manager, int ipg_no, int consist_ipg_no, ProdSetInfo* record);
int ProdSetInfo_Update(DBConnectionManager* manager, const ProdSetInfo* record);
int ProdSetInfo_Delete(DBConnectionManager* manager, int ipg_no, int consist_ipg_no);
int ProdSetInfo_Exists(DBConnectionManager* manager, int ipg_no, int consist_ipg_no);
int ProdSetInfo_GetAll(DBConnectionManager* manager, ProdSetInfo* records, int max_count, int* actual_count);

#endif /* PROD_SET_INFO_ORM_H */