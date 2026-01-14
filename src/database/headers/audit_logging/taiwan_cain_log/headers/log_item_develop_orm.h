#ifndef LOG_ITEM_DEVELOP_ORM_H
#define LOG_ITEM_DEVELOP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    int it_no;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    short develop;
    signed char sucess_flag;
} LogItemDevelop;

/* CRUD Operations */
int LogItemDevelop_Add(DBConnectionManager* manager, const LogItemDevelop* record);
int LogItemDevelop_Get(DBConnectionManager* manager, const char* occ_time, int it_no, LogItemDevelop* record);
int LogItemDevelop_Update(DBConnectionManager* manager, const LogItemDevelop* record);
int LogItemDevelop_Delete(DBConnectionManager* manager, const char* occ_time, int it_no);
int LogItemDevelop_Exists(DBConnectionManager* manager, const char* occ_time, int it_no);
int LogItemDevelop_GetAll(DBConnectionManager* manager, LogItemDevelop* records, int max_count, int* actual_count);

#endif /* LOG_ITEM_DEVELOP_ORM_H */