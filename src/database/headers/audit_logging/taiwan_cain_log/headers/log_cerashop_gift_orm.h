#ifndef LOG_CERASHOP_GIFT_ORM_H
#define LOG_CERASHOP_GIFT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int idx;
    int occ_time;
    int from_mid;
    int from_charac_no;
    int to_mid;
    int to_charac_no;
    int item_idx;
    int item_cnt;
} LogCerashopGift;

/* CRUD Operations */
int LogCerashopGift_Add(DBConnectionManager* manager, const LogCerashopGift* record);
int LogCerashopGift_Get(DBConnectionManager* manager, int idx, LogCerashopGift* record);
int LogCerashopGift_Update(DBConnectionManager* manager, const LogCerashopGift* record);
int LogCerashopGift_Delete(DBConnectionManager* manager, int idx);
int LogCerashopGift_Exists(DBConnectionManager* manager, int idx);
int LogCerashopGift_GetAll(DBConnectionManager* manager, LogCerashopGift* records, int max_count, int* actual_count);

#endif /* LOG_CERASHOP_GIFT_ORM_H */