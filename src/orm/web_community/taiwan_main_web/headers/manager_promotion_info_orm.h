#ifndef MANAGER_PROMOTION_INFO_ORM_H
#define MANAGER_PROMOTION_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int pro_no;
    char promotion_name[256];
    char occ_date[20];
    char user_id[26];
    char promotion_explain[1024];
} ManagerPromotionInfo;

/* CRUD Operations */
int ManagerPromotionInfo_Add(DBConnectionManager* manager, const ManagerPromotionInfo* record);
int ManagerPromotionInfo_Get(DBConnectionManager* manager, int pro_no, ManagerPromotionInfo* record);
int ManagerPromotionInfo_Update(DBConnectionManager* manager, const ManagerPromotionInfo* record);
int ManagerPromotionInfo_Delete(DBConnectionManager* manager, int pro_no);
int ManagerPromotionInfo_Exists(DBConnectionManager* manager, int pro_no);
int ManagerPromotionInfo_GetAll(DBConnectionManager* manager, ManagerPromotionInfo* records, int max_count, int* actual_count);

#endif /* MANAGER_PROMOTION_INFO_ORM_H */