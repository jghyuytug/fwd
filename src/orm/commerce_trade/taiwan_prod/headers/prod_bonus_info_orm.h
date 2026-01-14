#ifndef PROD_BONUS_INFO_ORM_H
#define PROD_BONUS_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int ipg_no;
    int bonus_ipg_no;
    short bonus_rate;
} ProdBonusInfo;

/* CRUD Operations */
int ProdBonusInfo_Add(DBConnectionManager* manager, const ProdBonusInfo* record);
int ProdBonusInfo_Get(DBConnectionManager* manager, int ipg_no, int bonus_ipg_no, ProdBonusInfo* record);
int ProdBonusInfo_Update(DBConnectionManager* manager, const ProdBonusInfo* record);
int ProdBonusInfo_Delete(DBConnectionManager* manager, int ipg_no, int bonus_ipg_no);
int ProdBonusInfo_Exists(DBConnectionManager* manager, int ipg_no, int bonus_ipg_no);
int ProdBonusInfo_GetAll(DBConnectionManager* manager, ProdBonusInfo* records, int max_count, int* actual_count);

#endif /* PROD_BONUS_INFO_ORM_H */