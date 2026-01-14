#ifndef MANAGER_PROMOTION_DETAIL_ORM_H
#define MANAGER_PROMOTION_DETAIL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int pro_no;
    signed char pro_type;
    int mag_id;
} ManagerPromotionDetail;

/* CRUD Operations */
int ManagerPromotionDetail_Add(DBConnectionManager* manager, const ManagerPromotionDetail* record);
int ManagerPromotionDetail_GetAll(DBConnectionManager* manager, ManagerPromotionDetail* records, int max_count, int* actual_count);

#endif /* MANAGER_PROMOTION_DETAIL_ORM_H */