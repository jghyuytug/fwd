#ifndef LOG_ACQUIRE_ITEM_ORM_H
#define LOG_ACQUIRE_ITEM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    long long m_id;
    int charac_no;
    int item_id;
    int item_type;
    int item_count;
} LogAcquireItem;

/* CRUD Operations */
int LogAcquireItem_Add(DBConnectionManager* manager, const LogAcquireItem* record);
int LogAcquireItem_Get(DBConnectionManager* manager, const char* occ_date, int charac_no, int item_id, LogAcquireItem* record);
int LogAcquireItem_Update(DBConnectionManager* manager, const LogAcquireItem* record);
int LogAcquireItem_Delete(DBConnectionManager* manager, const char* occ_date, int charac_no, int item_id);
int LogAcquireItem_Exists(DBConnectionManager* manager, const char* occ_date, int charac_no, int item_id);
int LogAcquireItem_GetAll(DBConnectionManager* manager, LogAcquireItem* records, int max_count, int* actual_count);

#endif /* LOG_ACQUIRE_ITEM_ORM_H */