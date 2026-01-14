#ifndef LOG_ECO_POINT_ORM_H
#define LOG_ECO_POINT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int charac_no;
    int save_time;
    int item_id;
    int item_cnt;
    int add_point;
    int use_point;
    signed char save_type;
} LogEcoPoint;

/* CRUD Operations */
int LogEcoPoint_Add(DBConnectionManager* manager, const LogEcoPoint* record);
int LogEcoPoint_Get(DBConnectionManager* manager, int charac_no, int save_time, LogEcoPoint* record);
int LogEcoPoint_Update(DBConnectionManager* manager, const LogEcoPoint* record);
int LogEcoPoint_Delete(DBConnectionManager* manager, int charac_no, int save_time);
int LogEcoPoint_Exists(DBConnectionManager* manager, int charac_no, int save_time);
int LogEcoPoint_GetAll(DBConnectionManager* manager, LogEcoPoint* records, int max_count, int* actual_count);

#endif /* LOG_ECO_POINT_ORM_H */